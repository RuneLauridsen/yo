#pragma once

#ifndef YO_FONT_SLOT_COUNT
#define YO_FONT_SLOT_COUNT 128 // NOTE(rune): Must be less than UINT16_MAX
#endif

static yo_font_slot_t yo_font_slots[YO_FONT_SLOT_COUNT];

static yo_font_id_t yo_font_id_none()
{
    yo_font_id_t ret = { 0 };
    return ret;
}

static yo_font_slot_t *yo_font_slot_from_id(yo_font_id_t id)
{
    yo_font_slot_t *ret = NULL;

    if ((id.slot >= 1) && (id.slot <= YO_FONT_SLOT_COUNT))
    {
        yo_font_slot_t *slot = &yo_font_slots[id.slot - 1];
        if ((slot->id.generation == id.generation) && (slot->occupied))
        {
            ret = slot;
        }
    }

    return ret;
}

static yo_font_slot_t *yo_font_alloc_slot()
{
    yo_font_slot_t *ret = NULL;

    for (uint16_t i = 1; i <= YO_FONT_SLOT_COUNT; i++)
    {
        yo_font_slot_t *slot = &yo_font_slots[i - 1];
        if (!slot->occupied)
        {
            slot->id.slot = i;
            slot->id.generation++;
            slot->occupied = true;
            ret = slot;
            break;
        }
    }

    return ret;
}

static void yo_font_free_slot(yo_font_slot_t *slot)
{
    if (slot)
    {
        slot->occupied = false;
    }
}

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
        size = fread(data, 1, size, file);
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

static yo_font_id_t yo_font_load(void *data)
{
    yo_font_id_t ret     = yo_font_id_none();
    yo_font_slot_t *slot = NULL;
    bool ok = false;

    if (data)
    {
        slot = yo_font_alloc_slot();
        if (slot)
        {
            if (stbtt_InitFont(&slot->font_info, data, 0))
            {
                int32_t ascent, descent, line_gap;
                stbtt_GetFontVMetrics(&slot->font_info,
                                      &ascent,
                                      &descent,
                                      &line_gap);

                slot->metrics.ascent   = (float)ascent;
                slot->metrics.descent  = (float)descent;
                slot->metrics.line_gap = (float)line_gap;

                ret = slot->id;
                ok = true;
            }
        }
    }

    if (!ok)
    {
        yo_font_free_slot(slot);
    }

    return ret;
}

static void yo_font_unload(yo_font_id_t font)
{
    yo_font_slot_t *slot = yo_font_slot_from_id(font);
    yo_font_free_slot(slot);
}

static yo_font_metrics_t yo_font_metrics(yo_font_id_t font, uint32_t font_size)
{
    yo_font_metrics_t ret = { 0 };
    yo_font_slot_t *slot = yo_font_slot_from_id(font);
    if (slot)
    {
        float scale  = (float)(font_size) / (float)(slot->metrics.ascent);

        ret.ascent   = slot->metrics.ascent   * scale;
        ret.descent  = slot->metrics.descent  * scale;
        ret.line_gap = slot->metrics.line_gap * scale;
    }
    return ret;
}

static uint64_t yo_font_get_glyph_key(yo_font_id_t font, uint32_t code_point, uint16_t font_size)
{
    uint64_t key = (((uint64_t)(code_point) << 0)  |
                    ((uint64_t)(font_size)  << 32) |
                    ((uint64_t)(font.slot) << 48));
    return key;
}

static yo_atlas_node_t *yo_font_get_glyph(yo_font_id_t font, yo_atlas_t *atlas, uint32_t code_point, uint32_t font_size, bool rasterize)
{
    yo_atlas_node_t *ret = NULL;
    yo_font_slot_t *slot = yo_font_slot_from_id(font);

    if (slot)
    {
        float scale = (float)(font_size) / (float)(slot->metrics.ascent);

        // TODO(rune): Store font_size as uint16_t?
        uint64_t key = yo_font_get_glyph_key(font, code_point, (uint16_t)font_size);

        ret = yo_atlas_node_find(atlas, key);
        if (!ret)
        {
            //
            // Get code point rasterized dims
            //

            int32_t x0, y0, x1, y1;
            stbtt_GetCodepointBitmapBox(&slot->font_info, code_point, scale, scale, &x0, &y0, &x1, &y1);

            int32_t w = x1 - x0;
            int32_t h = y1 - y0;

            //
            // Allocate atlas region
            //

            ret = yo_atlas_node_new(atlas, yo_v2i(w, h));

            if (ret)
            {
                ret->key       = key;

                //
                // Store code point metrics
                //

                int32_t scaled_advance_h;
                int32_t scaled_bearing_x;
                stbtt_GetCodepointHMetrics(&slot->font_info, code_point, &scaled_advance_h, &scaled_bearing_x);

                ret->bearing_y =          (float)y0;
                ret->bearing_x =          scale * scaled_bearing_x;
                ret->advance_x = scale * scaled_advance_h;
            }
            else
            {
                __nop(); // NOTE(rune): Not enough space in glyph atlas
            }
        }

        //
        // Rasterize
        //

        if (ret)
        {
            // TODO(rune): We could just make rasterization a seperate function, e.g. with at yo_font_rasterize_pending() function.
            if (rasterize && !ret->rasterized)
            {
                int32_t stride = atlas->dims.x;
                uint8_t *pixel = atlas->pixels + (ret->rect.x + ret->rect.y * stride);
                stbtt_MakeCodepointBitmap(&slot->font_info, pixel, ret->rect.w, ret->rect.h, stride, scale, scale, code_point);
                ret->rasterized = true;
                atlas->dirty    = true;
            }
        }
    }

    return ret;
}
