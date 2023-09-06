#pragma once

// TODO(rune): File system layer?
static yo_file_content_t yo_load_file_content(char *file_name)
{
    yo_file_content_t ret = { 0 };

    FILE *file = fopen(file_name, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = yo_heap_alloc(size + 1, false);

    if (data)
    {
        fread(data, 1, size, file);

        ret.data = data;
        ret.size = size;
    }

    fclose(file);

    return ret;
}

static void yo_unload_file_content(yo_file_content_t file_content)
{
    yo_heap_free(file_content.data);
}

static bool yo_font_load(yo_font_t *font, void *data)
{
    bool ret = false;

    if (stbtt_InitFont(&font->font_info, data, 0))
    {
        stbtt_GetFontVMetrics(&font->font_info,
                              &font->ascent,
                              &font->descent,
                              &font->line_gap);

        ret = true;
    }

    return ret;
}

static bool yo_font_load_from_file(yo_font_t *font, char *file_name)
{
    bool ret = false;

    font->file_content = yo_load_file_content(file_name);
    if (font->file_content.data)
    {
        if (yo_font_load(font, font->file_content.data))
        {
            ret = true;
        }
    }

    return ret;
}

static void yo_font_unload(yo_font_t *font)
{
    if (font->file_content.data)
    {
        yo_unload_file_content(font->file_content);
    }
}


















// TODO(rune): Multiple fonts
static void yo_init_font()
{
}

static void yo_uninit_font()
{
}

static void yo_init_glyph_atlas(yo_atlas_t *atlas)
{
    YO_UNUSED(atlas);
}

static uint64_t yo_font_get_glyph_key(uint32_t codepoint, uint32_t fontsize)
{
    uint64_t key = (uint64_t)(codepoint) | ((uint64_t)(fontsize) << 32);
    return key;
}

static yo_atlas_node_t *yo_font_get_glyph(yo_font_t *font, yo_atlas_t *atlas, uint32_t code_point, uint32_t font_size, bool rasterize)
{
    uint64_t key = yo_font_get_glyph_key(code_point, font_size);

    yo_atlas_node_t *node = yo_atlas_get_node(atlas, key);

    if (!node)
    {
        //
        // Get code point rasterized dims
        //

        float scale = (float)(font_size) / (float)(font->ascent);

        int32_t x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&font->font_info, code_point, scale, scale, &x0, &y0, &x1, &y1);

        int32_t w = x1 - x0;
        int32_t h = y1 - y0;

        //
        // Allocate atlas region
        //

        node = yo_atlas_new_node(atlas, yo_v2i(w, h));

        if (node)
        {
            node->key       = key;

            node->ascent    = font->ascent   * scale; // TODO(rune): Remove
            node->descent   = font->descent  * scale; // TODO(rune): Remove
            node->line_gap  = font->line_gap * scale; // TODO(rune): Remove

            //
            // Store code point metrics
            //

            int32_t scaled_advance_h;
            int32_t scaled_bearing_x;
            stbtt_GetCodepointHMetrics(&font->font_info, code_point, &scaled_advance_h, &scaled_bearing_x);

            node->bearing_y =          (float)y0;
            node->bearing_x =          scale * scaled_bearing_x;
            node->horizontal_advance = scale * scaled_advance_h;

            //
            // Rasterize
            //

            if (rasterize || 1) // TODO(rune): Optional rasterize
            {
                int32_t stride = atlas->dims.x;
                uint8_t *pixel = atlas->pixels + (node->rect.x + node->rect.y * stride);
                stbtt_MakeCodepointBitmap(&font->font_info, pixel, node->rect.w, node->rect.h, stride, scale, scale, code_point);
                atlas->dirty = true;
            }
        }
        else
        {
            __nop(); // NOTE(rune): Not enough space in glyph atlas
        }
    }

    return node;
}

// TODO(rune): Remove
static void yo_get_glyph_uv(yo_atlas_t *atlas, yo_atlas_node_t *glyph, yo_v2f_t *uv0, yo_v2f_t *uv1)
{
    yo_atlas_get_node_uv(atlas, glyph, uv0, uv1);
}
