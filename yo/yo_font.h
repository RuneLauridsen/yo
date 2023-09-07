#pragma once

// TODO(rune): Multiple font backends.
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "thirdparty/stb_truetype.h"

// TODO(rune): File system layer?
typedef struct yo_file_content yo_file_content_t;
struct yo_file_content
{
    void *data;
    size_t size;
};

typedef uint16_t u16;

typedef struct yo_font_id yo_font_id_t;
struct yo_font_id
{
    uint16_t slot;
    uint16_t _unused;
    uint32_t generation;
};

typedef struct yo_font_slot yo_font_slot_t;
struct yo_font_slot
{
    yo_font_id_t id;
    bool occupied;

    // NOTE(rune): Font metrics
    int32_t line_gap;
    int32_t ascent;
    int32_t descent;

    // NOTE(rune): Font backend data
    stbtt_fontinfo font_info;

    // NOTE(rune): Zero initialized if not loaded with yo_font_load_from_file.
    yo_file_content_t file_content;
};

static yo_font_id_t     yo_font_load(void *data);
//static yo_font_id_t     yo_font_load_from_file(char *file_name);
static void             yo_font_unload(yo_font_id_t id);

static yo_atlas_node_t *yo_font_get_glyph(yo_font_id_t id, yo_atlas_t *atlas, uint32_t code_point, uint32_t font_size, bool rasterize);

// TODO(rune): Remove
static void             yo_get_glyph_uv(yo_atlas_t *atlas, yo_atlas_node_t *glyph, yo_v2f_t *uv0, yo_v2f_t *uv1);
