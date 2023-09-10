#pragma once

#ifndef YO_FONT_SLOT_COUNT
#define YO_FONT_SLOT_COUNT 128 // NOTE(rune): Must be less than UINT16_MAX
#endif

typedef struct yo_font_slot yo_font_slot_t;
struct yo_font_slot
{
    yo_font_id_t id;
    bool occupied;

    // NOTE(rune): Font metrics
    yo_font_metrics_t metrics;

    // NOTE(rune): Font backend data
    yo_font_backend_info_t backend_info;

    // NOTE(rune): Zero initialized if not loaded with yo_font_load_from_file.
    yo_file_content_t file_content;
};

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

static yo_font_id_t yo_font_load(void *data, size_t data_size)
{
    yo_font_id_t ret     = yo_font_id_none();
    yo_font_slot_t *slot = NULL;
    bool ok = false;

    if (data)
    {
        slot = yo_font_alloc_slot();
        if (slot)
        {
            if (yo_font_backend_load_font(&slot->backend_info, data, data_size))
            {
                slot->metrics = yo_font_backend_get_font_metrics(&slot->backend_info);
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

static uint64_t yo_font_get_glyph_key(yo_font_id_t font, uint32_t codepoint, uint16_t font_size)
{
    uint64_t key = (((uint64_t)(codepoint) << 0)  |
                    ((uint64_t)(font_size)  << 32) |
                    ((uint64_t)(font.slot) << 48));
    return key;
}

static yo_atlas_node_t *yo_font_get_glyph(yo_font_id_t font, yo_atlas_t *atlas, uint32_t codepoint, uint32_t font_size, bool rasterize)
{
    yo_atlas_node_t *ret = NULL;
    yo_font_slot_t *slot = yo_font_slot_from_id(font);

    if (slot)
    {
        float scale = (float)(font_size) / (float)(slot->metrics.ascent);

        // TODO(rune): Store font_size as uint16_t?
        uint64_t key = yo_font_get_glyph_key(font, codepoint, (uint16_t)font_size);

        ret = yo_atlas_node_find(atlas, key);
        if (!ret)
        {
            yo_codepoint_metrics_t metrics = yo_font_backend_get_codepoint_metrics(&slot->backend_info, codepoint, scale);

            //
            // Allocate atlas region
            //

            ret = yo_atlas_node_new(atlas, yo_v2i(metrics.dim.x, metrics.dim.y));

            if (ret)
            {
                ret->key       = key;
                ret->bearing_y = metrics.bearing_y;
                ret->bearing_x = metrics.bearing_x;
                ret->advance_x = metrics.advance_x;
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
            // TODO(rune): We could just make rasterization a separate function, e.g. with at yo_font_rasterize_pending() function.
            if (rasterize && !ret->rasterized)
            {
                int32_t stride = atlas->dim.x;
                uint8_t *pixel = atlas->pixels + (ret->rect.x + ret->rect.y * stride);

                yo_font_backend_rasterize(&slot->backend_info, codepoint, scale, pixel, yo_v2i(ret->rect.w, ret->rect.h), stride);
                ret->rasterized = true;
                atlas->dirty    = true;
            }
        }
    }

    return ret;
}
