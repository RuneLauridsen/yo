#pragma once

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "thirdparty/stb_truetype.h"

// TODO(rune): Multiple fonts
static stbtt_fontinfo g_font;
static int32_t        g_font_ascent;
static int32_t        g_font_descent;
static int32_t        g_font_line_gap;
static void *         g_font_buffer;

// TODO(rune): Multiple fonts
static void yo_init_font()
{
    FILE *file = fopen("C:\\Windows\\Fonts\\segoeui.ttf", "rb");
    fseek(file, 0, SEEK_END);
    size_t font_buffer_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    g_font_buffer = yo_heap_alloc(font_buffer_size, false);
    fread(g_font_buffer, 1, font_buffer_size, file);
    fclose(file);

    if (!stbtt_InitFont(&g_font, g_font_buffer, 0))
    {
        YO_ASSERT(false);
    }

    stbtt_GetFontVMetrics(&g_font, &g_font_ascent, &g_font_descent, &g_font_line_gap);

}

static void yo_uninit_font()
{
    yo_heap_free(g_font_buffer);
}

static void yo_init_glyph_atlas(yo_atlas_t *atlas)
{
    YO_UNUSED(atlas);
    yo_init_font();
}

static uint64_t yo_get_glyph_key(uint32_t codepoint, uint32_t fontsize)
{
    uint64_t key = (uint64_t)(codepoint) | ((uint64_t)(fontsize) << 32);
    return key;
}

static yo_atlas_node_t *yo_get_glyph(yo_atlas_t *atlas, uint32_t codepoint, uint32_t fontsize, uint64_t generation)
{
    uint64_t key = yo_get_glyph_key(codepoint, fontsize);

    yo_atlas_node_t *node = yo_atlas_get_node(atlas, key, generation);

    if (node == NULL)
    {
        //
        // Generate bitmap with stb truetype
        //
        float scale = (float)(fontsize) / (float)(g_font_ascent);

        int32_t x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&g_font, codepoint, scale, scale, &x0, &y0, &x1, &y1);

        int32_t w = x1 - x0;
        int32_t h = y1 - y0;

        int32_t stride = atlas->dims.x;

        node = yo_atlas_partition(atlas, yo_v2i(w, h), generation);

        if (node)
        {
            node->key       = yo_get_glyph_key(codepoint, fontsize);
            node->ascent    = g_font_ascent * scale;
            node->descent   = g_font_descent * scale;
            node->line_gap  = g_font_line_gap * scale;


            int32_t scaledHorizontalAdvance;
            int32_t scaledLeftBearing;
            stbtt_GetCodepointHMetrics(&g_font, codepoint, &scaledHorizontalAdvance, &scaledLeftBearing);

            node->bearing_y =          y0;
            node->bearing_x =          (int32_t)(scale * (float)scaledLeftBearing);
            node->horizontal_advance = (int32_t)(scale * (float)scaledHorizontalAdvance);

            uint8_t *pixel = atlas->pixels + (node->rect.x + node->rect.y * stride);
            stbtt_MakeCodepointBitmap(&g_font, pixel, node->rect.w, node->rect.h, stride, scale, scale, codepoint);

            atlas->dirty = true;
        }
        else
        {
            // Not enough space in glyph atlas
            __nop();
        }
    }

    return node;
}

static void yo_get_glyph_uv(yo_atlas_t *atlas, yo_atlas_node_t *glyph, yo_v2f_t *uv0, yo_v2f_t *uv1)
{
    yo_atlas_get_node_uv(atlas, glyph, uv0, uv1);
}
