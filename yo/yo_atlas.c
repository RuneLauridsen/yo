#pragma once

static yo_shelf_t *yo_alloc_shelf(yo_atlas_t *atlas)
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

static yo_atlas_node_t *yo_alloc_atlas_node(yo_atlas_t *atlas)
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

static void yo_init_atlas(yo_atlas_t *atlas, yo_v2i_t dims, yo_arena_t *arena)
{
    YO_ASSERT(yo_is_power_of_two(dims.x));
    YO_ASSERT(yo_is_power_of_two(dims.y));

    yo_zero_struct(atlas);
    atlas->dims   = dims;
    atlas->storage  = arena;
    atlas->pixels = yo_arena_push_size(arena, dims.x * dims.y, false);

    yo_shelf_t *main_shelf = yo_alloc_shelf(atlas);
    yo_dlist_add(&atlas->shelves, main_shelf);
    main_shelf->height = dims.y;

    YO_ASSERT(atlas->pixels); // TODO(rune): Error handling
    YO_ASSERT(main_shelf);    // TODO(rune): Error handling
}

static yo_atlas_node_t *yo_atlas_get_node(yo_atlas_t *atlas, uint64_t key, uint64_t generation)
{
    yo_atlas_node_t *ret = NULL;

    for (yo_dlist_each(yo_shelf_t *, shelf, &atlas->shelves))
    {
        for (yo_dlist_each(yo_atlas_node_t *, node, &shelf->nodes))
        {
            if (node->key == key)
            {
                node->last_accessed_generation  = generation;
                shelf->last_accessed_generation = generation;

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

static yo_atlas_node_t *yo_atlas_partition(yo_atlas_t *atlas, yo_v2i_t dims, uint64_t generation)
{
    yo_atlas_node_t *ret      = NULL;
    yo_shelf_t *best_fit      = NULL;
    yo_shelf_t *best_fit_nuke = NULL;

    uint32_t actual_y = dims.y;
    dims.y            = dims.y - dims.y % 8 + 8;

    //
    // Find existing shelf with fitting size
    //
    for (yo_dlist_each(yo_shelf_t *, shelf, &atlas->shelves))
    {
        if ((shelf->height >= dims.v_dim) &&
            (!best_fit || shelf->height < best_fit->height))
        {
            if (!shelf->nodes.first)
            {
                best_fit = shelf;
            }

            if (!best_fit_nuke || shelf->last_accessed_generation < best_fit_nuke->last_accessed_generation)
            {
                best_fit_nuke = shelf;
            }
        }

        if ((shelf->used_x + dims.x <= atlas->dims.x) &&
            (shelf->height == dims.v_dim))
        {
            best_fit      = shelf;
            best_fit_nuke = shelf;
            break;
        }
    }

    if (best_fit)
    {
        if (best_fit->height != dims.y)
        {
            //
            // Split shelf
            //

            YO_ASSERT(best_fit->used_x == 0);
            YO_ASSERT(best_fit->base_y + dims.y <= atlas->dims.y);

            yo_shelf_t *new_shelf = yo_alloc_shelf(atlas); // TODO(rune): Error handling
            new_shelf->height = dims.y;
            new_shelf->base_y = best_fit->base_y;
            best_fit->height -= dims.y;
            best_fit->base_y += dims.y;

            yo_dlist_insert_after(&atlas->shelves, new_shelf, best_fit);

            best_fit = new_shelf;
        }

        YO_ASSERT(best_fit->used_x + dims.x <= atlas->dims.x);

        yo_atlas_node_t *new_node = yo_alloc_atlas_node(atlas); // TODO(rune): Error handling
        new_node->rect.x = best_fit->used_x;
        new_node->rect.y = best_fit->base_y;
        new_node->rect.w = dims.x;
        new_node->rect.h = actual_y; // dims.y;
        best_fit->used_x += dims.x;

        yo_dlist_add(&best_fit->nodes, new_node);

        new_node->last_accessed_generation = generation;
        best_fit->last_accessed_generation = generation;

        ret = new_node;
    }
    else
    {
        atlas->pending_prune = true;
#if 0
        yo_atlas_prune(atlas, 0, yo_safe_subtract_u64(generation, 1), true, true);
#endif
    }

    return ret;
}

static bool yo_atlas_shelf_can_fit(yo_atlas_t *atlas, yo_shelf_t *shelf, yo_v2i_t dims)
{
    bool ret =((dims.y <= shelf->height) &&
               (dims.x <= atlas->dims.x - shelf->used_x));
    return ret;
}

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
