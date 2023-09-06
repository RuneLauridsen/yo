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
        float horizontal_advance;
        float bearing_x;
        float bearing_y;

        // NOTE(rune): Font metrics
        // TODO(rune): Font metrics shouldn't be stored per glyph, but should be per font instead
        float line_gap;
        float ascent;
        float descent;
    };
};

typedef struct yo_atlas_shelf yo_atlas_shelf_t;
struct yo_atlas_shelf
{
    int32_t used_x;
    int32_t base_y;
    int32_t height;
    uint64_t last_accessed_generation;
    yo_atlas_shelf_t *next, *prev;
    yo_dlist(yo_atlas_node_t) node_list;
};

typedef struct yo_atlas yo_atlas_t;
struct yo_atlas
{
    // TODO(rune): Altas auto resizing

    uint8_t *pixels;
    yo_v2i_t dims;
    yo_dlist(yo_atlas_shelf_t) shelf_list;

    yo_slist(yo_atlas_shelf_t) shelf_freelist;
    yo_slist(yo_atlas_node_t)  node_freelist;

    // NOTE(rune): Storage for yo_atlas_shelf_t and yo_atlas_node_t.
    yo_arena_t storage;

    bool dirty;
    uint64_t current_generation;
};

static bool             yo_atlas_create(yo_atlas_t *atlas, yo_v2i_t initial_dims);
static void             yo_atlas_destroy(yo_atlas_t *atlas);
static yo_atlas_node_t *yo_atlas_get_node(yo_atlas_t *atlas, uint64_t key);
static void             yo_atlas_get_node_uv(yo_atlas_t *atlas, yo_atlas_node_t *node, yo_v2f_t *uv0, yo_v2f_t *uv1);
static yo_atlas_node_t *yo_atlas_new_node(yo_atlas_t *atlas, yo_v2i_t dims);
static void             yo_atlas_reset(yo_atlas_t *atlas);
