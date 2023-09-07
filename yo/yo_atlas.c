#pragma once

static yo_atlas_shelf_t *yo_atlas_alloc_shelf(yo_atlas_t *atlas)
{
    yo_atlas_shelf_t *ret = atlas->shelf_freelist.first;
    yo_slist_queue_pop(&atlas->shelf_freelist);

    if (ret)
    {
        yo_zero_struct(ret);
    }
    else
    {
        ret = yo_arena_push_struct(&atlas->storage, yo_atlas_shelf_t, true);
    }

    return ret;
}

static yo_atlas_node_t *yo_atlas_alloc_node(yo_atlas_t *atlas)
{
    yo_atlas_node_t *ret = atlas->node_freelist.first;
    yo_slist_queue_pop(&atlas->node_freelist);

    if (ret)
    {
        yo_zero_struct(ret);
    }
    else
    {
        ret = yo_arena_push_struct(&atlas->storage, yo_atlas_node_t, true);
    }

    return ret;
}

static bool yo_atlas_create(yo_atlas_t *atlas, yo_v2i_t dims)
{
    YO_ASSERT(yo_is_power_of_two(dims.x));
    YO_ASSERT(yo_is_power_of_two(dims.y));

    yo_zero_struct(atlas);

    bool ok = true;

    ok &= yo_arena_create(&atlas->storage, YO_KILOBYTES(64), false, YO_ARENA_TYPE_CHAIN_EXPONENTIAL);

    atlas->dims    = dims;
    atlas->pixels  = yo_heap_alloc(dims.x * dims.y, true);

    ok &= atlas->pixels != NULL;

    if (ok)
    {
        yo_atlas_shelf_t *root_shelf = yo_atlas_alloc_shelf(atlas);
        root_shelf->height = dims.y;
        yo_dlist_add(&atlas->shelf_list, root_shelf);
    }
    else
    {
        yo_atlas_destroy(atlas);
    }

    return ok;
}

static void yo_atlas_destroy(yo_atlas_t *atlas)
{
    yo_heap_free(atlas->pixels);
    yo_arena_destroy(&atlas->storage);
    yo_zero_struct(atlas);
}

static yo_atlas_node_t *yo_atlas_find_node(yo_atlas_t *atlas, uint64_t key)
{
    // TODO(rune): Hashtable lookup?

    yo_atlas_node_t *ret = NULL;

    for (yo_dlist_each(yo_atlas_shelf_t *, shelf, &atlas->shelf_list))
    {
        for (yo_dlist_each(yo_atlas_node_t *, node, &shelf->node_list))
        {
            if (node->key == key)
            {
                node->last_accessed_generation  = atlas->current_generation;
                shelf->last_accessed_generation = atlas->current_generation;

                ret = node;
            }
        }
    }

    return ret;
}

static void yo_atlas_get_node_uv(yo_atlas_t *atlas, yo_atlas_node_t *node, yo_v2f_t *uv0, yo_v2f_t *uv1)
{
    if (node)
    {
        uv0->x = (float)(node->rect.x) / (float)(atlas->dims.x);
        uv0->y = (float)(node->rect.y) / (float)(atlas->dims.y);
        uv1->x = (float)(node->rect.x + node->rect.w) / (float)(atlas->dims.x);
        uv1->y = (float)(node->rect.y + node->rect.h) / (float)(atlas->dims.y);
    }
    else
    {
        uv0->x = 0.0f;
        uv0->y = 0.0f;
        uv1->x = 0.0f;
        uv1->y = 0.0f;
    }
}

static inline bool yo_atlas_shelf_can_fit(yo_atlas_t *atlas, yo_atlas_shelf_t *shelf, yo_v2i_t dims)
{
    bool ret =((dims.y <= shelf->height) &&
               (dims.x <= atlas->dims.x - shelf->used_x));
    return ret;
}

static yo_atlas_shelf_t * yo_atlas_shelf_merge(yo_atlas_t *atlas, yo_atlas_shelf_t *a, yo_atlas_shelf_t *b)
{
    YO_ASSERT(a != b);
    YO_ASSERT(a->used_x == 0);
    YO_ASSERT(b->used_x == 0);
    YO_ASSERT(a->node_list.first == NULL);
    YO_ASSERT(b->node_list.first == NULL);
    YO_ASSERT(a->node_list.last  == NULL);
    YO_ASSERT(b->node_list.last  == NULL);
    YO_ASSERT(a->base_y != b->base_y);

    // NOTE(rune): Always dealloc topmost shelf.
    yo_atlas_shelf_t *bot = a->base_y < b->base_y ? a : b;
    yo_atlas_shelf_t *top = a->base_y < b->base_y ? b : a;

    bot->height += top->height;

    yo_dlist_remove(&atlas->shelf_list, top);
    yo_slist_queue_push(&atlas->shelf_freelist, top);

    return bot;
}

static yo_atlas_shelf_t *yo_atlas_shelf_split(yo_atlas_t *atlas, yo_atlas_shelf_t *split, int32_t y)
{
    yo_atlas_shelf_t *ret = NULL;

    if (split->height > y)
    {
        ret = yo_atlas_alloc_shelf(atlas);

        ret->height = y;
        ret->base_y = split->base_y;

        split->base_y += y;
        split->height -= y;

        yo_dlist_insert_before(&atlas->shelf_list, ret, split);
    }
    else if (split->height == y)
    {
        ret = split; // NOTE(rune): No need to split.
    }
    else
    {
        YO_ASSERT(!"Not enough atlas space.");
    }

    return ret;
}

static yo_atlas_shelf_t * yo_atlas_shelf_reset_and_merge(yo_atlas_t *atlas, yo_atlas_shelf_t *reset)
{
    reset->used_x = 0;
    reset->last_accessed_generation = 0;

    if (reset->node_list.first)  yo_slist_queue_join(&atlas->node_freelist, &reset->node_list);
    reset->node_list.first = NULL;
    reset->node_list.last  = NULL;

    if (reset->next && reset->next->used_x == 0) reset = yo_atlas_shelf_merge(atlas, reset, reset->next);
    if (reset->prev && reset->prev->used_x == 0) reset = yo_atlas_shelf_merge(atlas, reset, reset->prev);

    return reset;
}

static yo_atlas_shelf_t *yo_atlas_prune_until_enough_y(yo_atlas_t *atlas, int32_t y)
{
    YO_UNUSED(atlas, y);

    yo_atlas_shelf_t *ret = NULL;
    yo_atlas_shelf_t *stale = NULL;
    uint64_t stale_last_accessed_generation = 0;
    do
    {
        // NOTE(rune): Find shelf with lowest last_accessed_generation.
        // TODO(rune): Profile! If this turns out to be slow, we could keep a linked list of shelves,
        // sorted by last_accessed_generation.
        stale = NULL;
        stale_last_accessed_generation = atlas->current_generation;
        for (yo_dlist_each(yo_atlas_shelf_t *, it, &atlas->shelf_list))
        {
            // NOTE(rune): We assume that called has already checkout empty for space in empty shelves.
            if (it->used_x > 0 && it->last_accessed_generation < stale_last_accessed_generation)
            {
                stale = it;
                stale_last_accessed_generation = it->last_accessed_generation;
            }
        }

        // NOTE(rune): Reset shelf with lowest last_accessed_generation
        if (stale)
        {
            stale = yo_atlas_shelf_reset_and_merge(atlas, stale);

            if (stale->height >= y)
            {
                ret = yo_atlas_shelf_split(atlas, stale, y);
            }
        }

    } while (stale && !ret);

    return ret;
}

static yo_atlas_node_t *yo_atlas_new_node(yo_atlas_t *atlas, yo_v2i_t dims)
{
    yo_atlas_node_t  *ret                 = NULL;
    yo_atlas_shelf_t *best_shelf_nonempty = NULL;
    yo_atlas_shelf_t *best_shelf_empty    = NULL;
    yo_v2i_t rounded_dims                 = yo_v2i(dims.x, dims.y - dims.y % 8 + 8);

    // NOTE(rune): Find the shelves where we waste the least amount of y-space.
    for (yo_dlist_each(yo_atlas_shelf_t *, shelf, &atlas->shelf_list))
    {
        int32_t wasted = shelf->height - rounded_dims.y;
        bool fits = yo_atlas_shelf_can_fit(atlas, shelf, rounded_dims);

        if (fits && shelf->used_x)
        {
            if (best_shelf_nonempty == NULL || wasted < best_shelf_nonempty->height - rounded_dims.y)
            {
                best_shelf_nonempty = shelf;
            }
        }

        if (fits && !shelf->used_x)
        {
            if (best_shelf_empty == NULL || wasted < best_shelf_empty->height - rounded_dims.y)
            {
                best_shelf_empty = shelf;
            }
        }
    }

    yo_atlas_shelf_t *best_shelf = NULL;

    // NOTE(rune): If it fits in an existing shelf that is currently nonempty, just continue to use that shelf,
    // otherwise we begin a new shelf, by splitting the shelf with least wasted y space.
    // TODO(rune): This approach could lead to a lot of short shelves. Think of another strategy to pick shelf to split?

    if (best_shelf_nonempty)
    {
        best_shelf = best_shelf_nonempty;
    }
    else if (best_shelf_empty)
    {
        best_shelf = yo_atlas_shelf_split(atlas, best_shelf_empty, rounded_dims.y);
    }
    else
    {
        best_shelf = yo_atlas_prune_until_enough_y(atlas, rounded_dims.y);
    }

    if (best_shelf)
    {
        ret = yo_atlas_alloc_node(atlas);
        if (ret)
        {
            yo_dlist_add(&best_shelf->node_list, ret);
            ret->rect.x = best_shelf->used_x;
            ret->rect.y = best_shelf->base_y;
            ret->rect.w = dims.x;
            ret->rect.h = dims.y;

            ret->last_accessed_generation        = atlas->current_generation;
            best_shelf->last_accessed_generation = atlas->current_generation;

            best_shelf->used_x += dims.x;
        }
    }

    return ret;
}

static void yo_atlas_reset(yo_atlas_t *atlas)
{
    YO_UNUSED(atlas);
    // yo_atlas_prune(atlas, 0, UINT64_MAX, true, false);
}
