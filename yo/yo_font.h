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

typedef struct yo_font yo_font_t;
struct yo_font
{
    // NOTE(rune): Font backend data
    stbtt_fontinfo font_info;

    // NOTE(rune): Font metrics
    int32_t line_gap;
    int32_t ascent;
    int32_t descent;

    // NOTE(rune): Zero initialized if not loaded with yo_font_load_from_file.
    yo_file_content_t file_content;
};

static bool             yo_font_load(yo_font_t *font, void *data);
static bool             yo_font_load_from_file(yo_font_t *font, char *file_name);
static void             yo_font_unload(yo_font_t *font);

static void             yo_init_glyph_atlas(yo_atlas_t *atlas);
static yo_atlas_node_t *yo_font_get_glyph(yo_font_t *font, yo_atlas_t *atlas, uint32_t code_point, uint32_t font_size, bool rasterize);

// TODO(rune): Remove
static void             yo_get_glyph_uv(yo_atlas_t *atlas, yo_atlas_node_t *glyph, yo_v2f_t *uv0, yo_v2f_t *uv1);
