#pragma once

typedef struct yo_atlas_node yo_atlas_node_t;
struct yo_atlas_node
{
    yo_recti rect;
    uint64_t key;
    uint64_t last_accessed_generation;
    yo_atlas_node_t *next, *prev;

    // TODO(rune): Can the metrics be stored elsewhere? Not all texture atlases are for glyphs.
    struct
    {
        // NOTE(rune): Glyph metrics
        int32_t horizontal_advance;
        int32_t bearing_x;
        int32_t bearing_y;

        // NOTE(rune): Font metrics
        // TODO(rune): Font metrics shouldn't be stored per glyph, but should be per font instead
        float line_gap;
        float ascent;
        float descent;
    };
};

typedef struct yo_shelf yo_shelf_t;
struct yo_shelf
{
    int32_t used_x;
    int32_t base_y;
    int32_t height;
    uint64_t last_accessed_generation;
    yo_shelf_t *next, *prev;
    yo_dlist(yo_atlas_node_t) nodes;
};

typedef struct yo_atlas yo_atlas_t;
struct yo_atlas
{
    uint8_t *pixels;
    yo_v2i_t dims;
    yo_dlist(yo_shelf_t) shelves;

    yo_slist(yo_shelf_t)      shelf_freelist;
    yo_slist(yo_atlas_node_t) node_freelist;
    yo_arena_t *storage;

    bool dirty;
    bool pending_prune;
};

static void             yo_init_atlas(yo_atlas_t *atlas, yo_v2i_t dims, yo_arena_t *arena);
static yo_atlas_node_t *yo_atlas_get_node(yo_atlas_t *atlas, uint64_t key, uint64_t generation);
static void             yo_atlas_get_node_uv(yo_atlas_t *atlas, yo_atlas_node_t *node, yo_v2f_t *uv0, yo_v2f_t *uv1);
static yo_atlas_node_t *yo_atlas_partition(yo_atlas_t *atlas, yo_v2i_t dims, uint64_t generation);
static void             yo_atlas_prune(yo_atlas_t *atlas, uint64_t prune_generation_min, uint64_t prune_generation_max,
                                       bool nuke_shelves, bool merge_shelves);
