#pragma once

////////////////////////////////////////////////////////////////
//
//
// Font slot table
//
//
////////////////////////////////////////////////////////////////

static yo_font_id_t yo_font_id_none()
{
    yo_font_id_t ret = { 0 };
    return ret;
}

static yo_font_slot_t *yo_font_slot_find(yo_font_id_t id)
{
    yo_font_slot_t *ret = NULL;

    uint16_t get_slot_id    = (uint16_t)((id.u64 & 0x0000'0000'0000'ffff) >> 0);
    uint32_t get_generation = (uint32_t)((id.u64 & 0xffff'ffff'0000'0000) >> 32);

    if ((get_slot_id >= 1) && (get_slot_id <= YO_FONT_TABLE_SLOT_COUNT))
    {
        uint16_t slot_idx = get_slot_id - 1;
        yo_font_slot_t *slot = &yo_font_slots[slot_idx];

        YO_ASSERT(slot->slot_id == get_slot_id);

        if ((slot->generation == get_generation) && (slot->occupied))
        {
            ret = slot;
        }
    }

    return ret;
}

static yo_font_slot_t *yo_font_slot_alloc()
{
    yo_font_slot_t *ret = NULL;

    for (uint16_t i = 1; i <= YO_FONT_TABLE_SLOT_COUNT; i++)
    {
        yo_font_slot_t *slot = &yo_font_slots[i - 1];
        if (!slot->occupied)
        {
            slot->slot_id = i;
            slot->generation++;
            slot->occupied = true;
            ret = slot;
            break;
        }
    }

    return ret;
}

static void yo_font_slot_free(yo_font_slot_t *slot)
{
    if (slot)
    {
        slot->occupied = false;
    }
}

////////////////////////////////////////////////////////////////
//
//
// Load/unload
//
//
////////////////////////////////////////////////////////////////

static yo_font_id_t yo_font_load_(void *data, size_t data_size, yo_font_backend_t *backend)
{
    yo_font_id_t ret = yo_font_id_none();
    yo_font_slot_t *slot = NULL;
    bool ok = false;

    if (data)
    {
        slot = yo_font_slot_alloc();
        if (slot)
        {
            if (yo_font_backend_load(backend, &slot->backend_info, data, data_size))
            {
                ret = slot->font_id;
                ok = true;
            }
        }
    }

    if (!ok)
    {
        yo_font_slot_free(slot);
    }

    return ret;
}

static void yo_font_unload_(yo_font_id_t font, yo_font_backend_t *backend)
{
    yo_font_slot_t *slot = yo_font_slot_find(font);
    yo_font_backend_unload(backend, &slot->backend_info);
    yo_font_slot_free(slot);
}

////////////////////////////////////////////////////////////////
//
//
// Metrics
//
//
////////////////////////////////////////////////////////////////

static yo_font_metrics_t yo_font_metrics(yo_font_id_t font, uint32_t font_size)
{
    yo_font_metrics_t ret = { 0 };
    yo_font_slot_t *slot = yo_font_slot_find(font);
    if (slot)
    {
        ret = yo_font_backend_get_font_metrics(&yo_ctx->font_backend, &slot->backend_info, font_size);
    }
    return ret;
}

static uint64_t yo_glyph_key(yo_font_id_t font, uint16_t font_size, uint32_t codepoint)
{
    uint64_t key = (((uint64_t)(codepoint)          << 0)  |
                    ((uint64_t)(font_size)          << 32) |
                    ((uint64_t)(font.u64 & 0xffff)  << 48));

    // NOTE(rune): Only the first 16 bits of font.u64 specify the font_slot.
    // TODO(rune): If we increase the glyph key to 128-bits, we can skip this masking step.

    return key;
}

// TODO(rune): Just return by value? Profile.
static yo_atlas_node_t *yo_glyph_get(yo_font_id_t font, uint32_t font_size, yo_atlas_t *atlas, uint32_t codepoint, bool rasterize)
{
    yo_atlas_node_t *ret = NULL;
    yo_font_slot_t *slot = yo_font_slot_find(font);

    if (slot)
    {
        // TODO(rune): Store font_size as uint16_t?
        uint64_t key = yo_glyph_key(font, (uint16_t)font_size, codepoint);

        ret = yo_atlas_node_find(atlas, key);
        if (!ret)
        {
            yo_glyph_metrics_t metrics = yo_font_backend_get_glyph_metrics(&yo_ctx->font_backend,
                                                                           &slot->backend_info,
                                                                           codepoint,
                                                                           font_size);

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

                YO_PROFILE_BEGIN(yo_font_backend_rasterize);
                yo_font_backend_rasterize(&yo_ctx->font_backend, &slot->backend_info,
                                          codepoint, font_size, pixel, yo_v2i(ret->rect.w, ret->rect.h), stride);

                YO_PROFILE_END(yo_font_backend_rasterize);

                ret->rasterized = true;
                atlas->dirty    = true;
            }
        }
    }

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Text layout
//
//
////////////////////////////////////////////////////////////////

#if 0
static yo_text_layout_char_t *yo_text_layout_char(yo_text_layout_t *layout)
{
    yo_text_layout_char_t *ret = NULL;
    yo_text_layout_line_t *line = layout->lines.last;
    yo_text_layout_chunk_t *chunk = line->chunks.last;

    bool enough_space_in_chunk = chunk && chunk->count < countof(chunk->data);
    if (!enough_space_in_chunk)
    {
        chunk = yo_arena_push_struct(&yo_ctx->this_frame->arena, yo_text_layout_chunk_t, true);
        if (chunk)
        {
            yo_slist_add(&line->chunks, chunk);
        }
    }

    if (line)
    {
        ret = &chunk->data[chunk->count++];
    }

    return ret;
}
#endif

static void yo_text_layout_commit_chunk(yo_text_layout_t *l)
{
    if (l->current_chunk.string.length > 0)
    {
        yo_text_layout_chunk_t *chunk = yo_arena_push_struct(&yo_ctx->this_frame->arena, yo_text_layout_chunk_t, true);
        if (chunk)
        {
            *chunk = l->current_chunk;
            yo_slist_add(&l->current_line.chunks, chunk);
            l->current_line.chunk_count++;
        }
    }

    l->current_line.advance_x += l->current_chunk.advance_x;

    l->current_chunk.string.data   = NULL;
    l->current_chunk.string.length = 0;
    l->current_chunk.start_x       = 0.0f;
    l->current_chunk.advance_x     = 0.0f;
}

static void yo_text_layout_commit_line(yo_text_layout_t *l, bool wrapped)
{
    yo_text_layout_line_t *line = l->lines.last;

    line = yo_arena_push_struct(&yo_ctx->this_frame->arena, yo_text_layout_line_t, true);

    if (line)
    {
        *line = l->current_line;
        line->wrapped = wrapped;
        yo_slist_add(&l->lines, line);
    }

    l->current_chunk.start_x     = 0.0f;
    l->current_line.start_x      = 0.0f;
    l->current_line.start_y     += l->font_metrics.line_gap;
    l->current_line.advance_x    = 0.0;
    l->current_line.chunks.first = NULL;
    l->current_line.chunks.last  = NULL;
    l->current_line.chunk_count  = 0;
}

static yo_text_layout_t yo_text_layout(yo_font_id_t font, uint32_t font_size, yo_string_t text,
                                       yo_text_align_t align, yo_text_flags_t flags, yo_v2f_t wrap)
{
    YO_PROFILE_BEGIN(yo_text_layout);

    yo_text_layout_t l = { 0 };

    l.font             = font;
    l.font_size        = font_size;
    l.font_metrics     = yo_font_metrics(font, font_size);

    yo_atlas_node_t *space_glyph = yo_glyph_get(font, font_size, &yo_ctx->atlas, ' ', false);
    float space_advance = space_glyph ? space_glyph->advance_x : 0.0f;

    //
    // NOTE(rune): Divide text into chunks and lines.
    //

    YO_PROFILE_BEGIN(yo_text_layout_first_pass);
    {
        yo_decoded_codepoint_t decoded = { 0 };
        while (yo_utf8_advance_codepoint(&text, &decoded))
        {
            switch (decoded.codepoint)
            {
                case '\n':
                {
                    if (flags & YO_TEXT_WRAP)
                    {
                        yo_text_layout_commit_chunk(&l);
                        yo_text_layout_commit_line(&l, false);
                    }
                } break;

                case '\r':
                {
                    if (flags & YO_TEXT_WRAP)
                    {
                        yo_text_layout_commit_chunk(&l);
                        l.current_line.advance_x = 0.0f;
                    }
                } break;

                case '\t':
                {
                    yo_text_layout_commit_chunk(&l);
                    l.current_line.advance_x += space_advance * 8.0f;
                } break;

                case ' ':
                {
                    yo_text_layout_commit_chunk(&l);
                    l.current_line.advance_x += space_advance;
                } break;

                default:
                {
                    if (l.current_chunk.string.data)
                    {
                        l.current_chunk.string.length += decoded.byte_length;
                    }
                    else
                    {
                        l.current_chunk.start_x       = l.current_line.advance_x;
                        l.current_chunk.string.length = decoded.byte_length;;
                        l.current_chunk.string.data   = text.data - decoded.byte_length;
                    }

                    yo_atlas_node_t *glyph = yo_glyph_get(font, font_size, &yo_ctx->atlas, decoded.codepoint, false);
                    l.current_chunk.advance_x     += glyph->advance_x;

                    if (l.current_line.advance_x + l.current_chunk.advance_x > wrap.x)
                    {
                        if (flags & YO_TEXT_WRAP)
                        {
                            yo_text_layout_commit_line(&l, true);
                            yo_text_layout_commit_chunk(&l);
                        }
                    }

                } break;
            }

            l.dim.x = YO_MAX(l.dim.x, l.current_line.advance_x);
        }

        l.dim.y = l.current_line.start_y + l.font_metrics.line_gap;
    }
    YO_PROFILE_END(yo_text_layout_first_pass);

    //
    // NOTE(rune): Align chunks within each line.
    //

    YO_PROFILE_BEGIN(yo_text_layout_second_pass);
    {
        for (yo_slist_each(yo_text_layout_line_t *, line, l.lines.first))
        {
            float extra = wrap.x - line->advance_x;

            switch (align)
            {
                default:
                case YO_TEXT_ALIGN_LEFT:
                {
                    line->start_x = 0.0f;
                } break;

                case YO_TEXT_ALIGN_RIGHT:
                {
                    line->start_x = extra;
                } break;

                case YO_TEXT_ALIGN_CENTER:
                {
                    line->start_x = extra / 2.0f;
                } break;

                case YO_TEXT_ALIGN_JUSTIFY:
                {
                    if (line->wrapped)
                    {
                        float per_chunk = extra / (line->chunk_count - 1);
                        uint32_t i = 0;
                        for (yo_slist_each(yo_text_layout_chunk_t *, chunk, line->chunks.first))
                        {
                            chunk->start_x += per_chunk * i;
                            i++;
                        }
                    }
                } break;
            }
        }
    }
    YO_PROFILE_END(yo_text_layout_second_pass);

    YO_PROFILE_END(yo_text_layout);

    // NOTE(rune): Because Visual Studio :(
    yo_text_layout_t *l2 = &l;
    return *l2;
}