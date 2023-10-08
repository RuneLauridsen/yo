#pragma once

////////////////////////////////////////////////////////////////
//
//
// Font slot table
//
//
////////////////////////////////////////////////////////////////

#ifndef YO_FONT_TABLE_SLOT_COUNT
#define YO_FONT_TABLE_SLOT_COUNT 128 // NOTE(rune): Must be less than UINT16_MAX
#endif

typedef struct yo_font_slot yo_font_slot_t;
struct yo_font_slot
{
    union
    {
        struct
        {
            yo_font_id_t font_id;
        };

        struct
        {
            uint16_t slot_id; // NOTE(rune): 1-based index in yo_font_slots[]. 0 is reserved for invalid/null font.
            uint16_t _unused;
            uint32_t generation;
        };
    };

    bool occupied;

    // NOTE(rune): Font backend specific data.
    yo_font_backend_info_t backend_info;
};

// TODO(rune): Consider placing inside yo_context_t
static yo_font_slot_t yo_font_slots[YO_FONT_TABLE_SLOT_COUNT];

static yo_font_id_t         yo_font_id_none();
static yo_font_slot_t *     yo_font_slot_alloc();
static yo_font_slot_t *     yo_font_slot_find(yo_font_id_t id);
static void                 yo_font_slot_free(yo_font_slot_t *slot);

////////////////////////////////////////////////////////////////
//
//
// Load/unload
//
//
////////////////////////////////////////////////////////////////

static yo_font_id_t      yo_font_load_(void *data, size_t data_size, yo_font_backend_t *backend);
static void              yo_font_unload_(yo_font_id_t font, yo_font_backend_t *backend);

////////////////////////////////////////////////////////////////
//
//
// Metrics
//
//
////////////////////////////////////////////////////////////////

static uint64_t          yo_glyph_key(yo_font_id_t font, uint16_t font_size, uint32_t codepoint);
static yo_atlas_node_t * yo_glyph_get(yo_font_slot_t *font, uint32_t font_size, yo_atlas_t *atlas, uint32_t codepoint, bool rasterize);
static yo_font_metrics_t yo_font_metrics(yo_font_slot_t *font, uint32_t font_size);

////////////////////////////////////////////////////////////////
//
//
// Text layout
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_text_layout_chunk yo_text_layout_chunk_t;
struct yo_text_layout_chunk
{
    yo_string_t string;

    float start_x;
    float advance_x;
    bool  allow_stretch;

    yo_text_layout_chunk_t *next;
};

typedef struct yo_text_layout_line yo_text_layout_line_t;
struct yo_text_layout_line
{
    yo_slist(yo_text_layout_chunk_t) chunks;

    float start_x;
    float start_y;
    float advance_x;

    uint16_t chunk_count;
    bool wrapped;

    yo_text_layout_line_t *next;
};

typedef struct yo_text_layout yo_text_layout_t;
struct yo_text_layout
{
    // NOTE(rune): Result data.
    struct
    {
        yo_slist(yo_text_layout_line_t) lines;

        yo_font_id_t      font;
        uint32_t          font_size;
        yo_font_metrics_t font_metrics;

        yo_v2f_t dim;
    };

    // NOTE(rune): Layout calculation state.
    struct
    {
        yo_string_t            remaining;
        yo_text_layout_line_t  current_line;
        yo_text_layout_chunk_t current_chunk;
    };
};

static yo_text_layout_t yo_text_layout(yo_font_id_t font, uint32_t font_size, yo_string_t text,
                                       yo_text_align_t align, yo_text_flags_t flags, yo_v2f_t wrap);
