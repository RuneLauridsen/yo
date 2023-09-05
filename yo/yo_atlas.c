#pragma once

static yo_shelf_t *yo_atlas_alloc_shelf(yo_atlas_t *atlas)
{
    yo_shelf_t *ret = atlas->shelf_freelist.first;
    yo_slist_queue_pop(&atlas->shelf_freelist);

    if (ret)
    {
        yo_zero_struct(ret);
    }
    else
    {
        ret = yo_arena_push_struct(atlas->storage, yo_shelf_t, true);
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
        ret = yo_arena_push_struct(atlas->storage, yo_atlas_node_t, true);
    }

    return ret;
}

// TODO(rune): Own memory, yo_atlas_create(), yo_atlas_destroy()
static void yo_atlas_init(yo_atlas_t *atlas, yo_v2i_t dims, yo_arena_t *arena)
{
    YO_ASSERT(yo_is_power_of_two(dims.x));
    YO_ASSERT(yo_is_power_of_two(dims.y));

    yo_zero_struct(atlas);
    atlas->dims   = dims;
    atlas->storage  = arena;
    atlas->pixels = yo_arena_push_size(arena, dims.x * dims.y, false);

    yo_shelf_t *main_shelf = yo_atlas_alloc_shelf(atlas);
    yo_dlist_add(&atlas->shelf_list, main_shelf);
    main_shelf->height = dims.y;

    YO_ASSERT(atlas->pixels); // TODO(rune): Error handling
    YO_ASSERT(main_shelf);    // TODO(rune): Error handling
}

static yo_atlas_node_t *yo_atlas_get_node(yo_atlas_t *atlas, uint64_t key)
{
    yo_atlas_node_t *ret = NULL;

    for (yo_dlist_each(yo_shelf_t *, shelf, &atlas->shelf_list))
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

static inline bool yo_atlas_shelf_can_fit(yo_atlas_t *atlas, yo_shelf_t *shelf, yo_v2i_t dims)
{
    bool ret =((dims.y <= shelf->height) &&
               (dims.x <= atlas->dims.x - shelf->used_x));
    return ret;
}

static yo_shelf_t * yo_atlas_shelf_merge(yo_atlas_t *atlas, yo_shelf_t *a, yo_shelf_t *b)
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
    yo_shelf_t *bot = a->base_y < b->base_y ? a : b;
    yo_shelf_t *top = a->base_y < b->base_y ? b : a;

    bot->height += top->height;

    yo_dlist_remove(&atlas->shelf_list, top);
    yo_slist_queue_push(&atlas->shelf_freelist, top);

    return a;
}

static yo_shelf_t *yo_atlas_shelf_split(yo_atlas_t *atlas, yo_shelf_t *split, int32_t y)
{
    yo_shelf_t *ret = NULL;

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

static void yo_atlas_shelf_evict(yo_atlas_t *atlas, yo_shelf_t *evict)
{
    evict->used_x = 0;
    evict->last_accessed_generation = 0;

    if (evict->node_list.first)  yo_slist_queue_join(&atlas->node_freelist, &evict->node_list);
    if (evict->next && evict->next->used_x == 0) evict = yo_atlas_shelf_merge(atlas, evict, evict->next);
    if (evict->prev && evict->prev->used_x == 0) evict = yo_atlas_shelf_merge(atlas, evict, evict->prev);

    evict->node_list.first = NULL;
    evict->node_list.last  = NULL;
}

static yo_atlas_node_t *yo_atlas_new_node(yo_atlas_t *atlas, yo_v2i_t dims)
{
    yo_atlas_node_t *ret            = NULL;
    yo_shelf_t *best_shelf_nonempty = NULL;
    yo_shelf_t *best_shelf_empty    = NULL;
    yo_v2i_t rounded_dims           = yo_v2i(dims.x, dims.y - dims.y % 8 + 8);

    // NOTE(rune): Find the shelves where we waste the least amount of y-space.
    for (yo_dlist_each(yo_shelf_t *, shelf, &atlas->shelf_list))
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

        if(fits && !shelf->used_x)
        {
            if (best_shelf_empty == NULL || wasted < best_shelf_empty->height - rounded_dims.y)
            {
                best_shelf_empty = shelf;
            }
        }
    }

    yo_shelf_t *best_shelf = NULL;

    // NOTE(rune): If it fits in an existing shelf that is currently nonempty, just continue to use that shelf,
    // otherwise we begin a new shelf, by splitting the shelf with least wasted y space.
    // TODO(rune): This approach could lead to a lot of short shelves. Think of another strategy to pick shelf to split?

    if (best_shelf_nonempty)
    {
        best_shelf = best_shelf_nonempty;
    }
    else
    {
        best_shelf = yo_atlas_shelf_split(atlas, best_shelf_empty, rounded_dims.y);
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

            ret->last_accessed_generation      = atlas->current_generation;
            best_shelf->last_accessed_generation = atlas->current_generation;

            best_shelf->used_x += dims.x;

        }
    }

    return ret;
}


#if 0
static void yo_atlas_prune(yo_atlas_t *atlas, uint64_t prune_generation_min, uint64_t prune_generation_max,
                           bool nuke_shelves, bool merge_shelves)
{
    if (nuke_shelves)
    {
        for (yo_dlist_each(yo_shelf_t *, shelf, &atlas->shelves))
        {
            if ((shelf->last_accessed_generation >= prune_generation_min) &&
                (shelf->last_accessed_generation <= prune_generation_max))
            {
                if (shelf->nodes.first)
                {
                    yo_slist_queue_join(&atlas->node_freelist, &shelf->nodes);
                }

                shelf->used_x      = 0;
                shelf->nodes.first = NULL;
                shelf->nodes.last  = NULL;
            }
        }
    }

    if (merge_shelves)
    {
        for (yo_dlist_each(yo_shelf_t *, shelf, &atlas->shelves))
        {
            if (shelf->next)
            {
                if ((!shelf->nodes.first) && (!shelf->next->nodes.first))
                {
                    shelf->base_y -= shelf->next->height;
                    shelf->height += shelf->next->height;
                    yo_shelf_t *to_remove = shelf->next;

                    yo_dlist_remove(&atlas->shelves, to_remove);
                    yo_slist_queue_push(&atlas->shelf_freelist, to_remove);
                }
            }
        }
    }
}
#endif

static void yo_atlas_reset(yo_atlas_t *atlas)
{
    YO_UNUSED(atlas);
    // yo_atlas_prune(atlas, 0, UINT64_MAX, true, false);
}
