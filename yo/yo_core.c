#pragma once

////////////////////////////////////////////////////////////////
//
//
// Globals
//
//
////////////////////////////////////////////////////////////////

static yo_context_t *yo_ctx;

// NOTE(rune): If yo runs out of memory we return pointers to stubs instead of returning null.
static yo_box_t stub_box = { 0 };

static char *error_message_table[YO_ERROR_COUNT] =
{
    [YO_ERROR_NONE] = "",
    [YO_ERROR_OUT_OF_PERSITENT_MEMORY]  = "Out of persistent memory",
    [YO_ERROR_OUT_OF_TEMPORARY_MEMORY]  = "Out of temporary memory",
    [YO_ERROR_PARENT_STACK_UNDERFLOW]   = "Mismatched yo_begin_children/yo_end_children",
    [YO_ERROR_STYLE_STACK_UNDERFLOW]    = "Style stack underflow",
    [YO_ERROR_ID_COLLISION]             = "Id collision"
};

////////////////////////////////////////////////////////////////
//
//
// Error handling
//
//
////////////////////////////////////////////////////////////////

static inline void yo_set_error(yo_error_t error)
{
    if (yo_ctx->error == YO_ERROR_NONE)
    {
        yo_ctx->error = error;

#if 1
        YO_ASSERT(false);
#endif
    }
}

////////////////////////////////////////////////////////////////
//
//
// Defaults
//
//
////////////////////////////////////////////////////////////////

static inline yo_config_t yo_default_config(void)
{
    yo_config_t ret =
    {
        .popup_close_delay = 30,
    };

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Memory
//
//
////////////////////////////////////////////////////////////////

static yo_box_t *yo_alloc_box(yo_id_t id)
{
    yo_arena_t *arena = &yo_ctx->this_frame->arena;
    yo_box_t *ret = yo_arena_push_struct(arena, yo_box_t, true);
    if (ret) ret->id = id;
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Hash table
//
//
////////////////////////////////////////////////////////////////

static yo_box_t *yo_get_box_by_id_(yo_id_t id, yo_frame_t *frame)
{
    yo_box_t *ret = NULL;

    if (id)
    {
        size_t slot_idx = id % countof(frame->hash_table);
        yo_box_t *slot = frame->hash_table[slot_idx];
        for (yo_box_t *box = slot; box; box = box->next_hash)
        {
            if (box->id == id)
            {
                ret = box;
                break;
            }
        }
    }

    return ret;
}

static yo_box_t *yo_get_box_by_id_this_frame(yo_id_t id)
{
    yo_box_t *ret = yo_get_box_by_id_(id, yo_ctx->this_frame);
    return ret;
}

static yo_box_t *yo_get_box_by_id_prev_frame(yo_id_t id)
{
    if (id == 5696385795564432402)
    {
        __nop();
    }

    yo_box_t *ret = yo_get_box_by_id_(id, yo_ctx->prev_frame);
    return ret;
}

static void yo_add_box_to_hash(yo_box_t *box, yo_frame_t *frame)
{
    if (box->id == 5696385795564432402)
    {
        __nop();
    }

    size_t slot_idx = box->id % countof(frame->hash_table);
    YO_SLSTACK_PUSH(frame->hash_table[slot_idx], next_hash, box);
}

////////////////////////////////////////////////////////////////
//
//
// Font
//
//
////////////////////////////////////////////////////////////////

static yo_font_metrics_t yo_font_metrics(yo_font_id_t font, uint32_t font_size)
{
    yo_font_metrics_t ret = { 0 };
    yo_font_slot_t *slot = yo_font_table_slot_find(font);
    if (slot)
    {
        ret = yo_font_backend_get_font_metrics(&yo_ctx->font_backend, &slot->backend_info, font_size);
    }
    return ret;
}

static uint64_t yo_glyph_key(yo_font_id_t font, uint32_t codepoint, uint16_t font_size)
{
    uint64_t key = (((uint64_t)(codepoint)          << 0)  |
                    ((uint64_t)(font_size)          << 32) |
                    ((uint64_t)(font.u64 & 0xffff)  << 48));

    // NOTE(rune): Only the first 16 bits of font.u16 specify the font_slot.
    // TODO(rune): If we increase the glyph key to 128-bits, we can skip this masking step.

    return key;
}

// TODO(rune): Just return by value? Profile.
static yo_atlas_node_t *yo_glyph_get(yo_font_id_t font, yo_atlas_t *atlas, uint32_t codepoint, uint32_t font_size, bool rasterize)
{
    yo_atlas_node_t *ret = NULL;
    yo_font_slot_t *slot = yo_font_table_slot_find(font);

    if (slot)
    {
        // yo_font_metrics_t slot_metrics = yo_font_backend_get_font_metrics(&yo_ctx->font_backend, &slot->backend_info, font_size);

        // TODO(rune): Store font_size as uint16_t?
        uint64_t key = yo_glyph_key(font, codepoint, (uint16_t)font_size);

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

// NOTE(rune): The default is loaded in yo_create_context(), so we can't rely on the
// yo_ctx global variable being set yet.
static yo_font_id_t yo_font_load_ctx(void *data, size_t data_size, yo_context_t *ctx)
{
    yo_font_id_t ret     = yo_font_id_none();
    yo_font_slot_t *slot = NULL;
    bool ok = false;

    if (data)
    {
        slot = yo_font_table_slot_alloc();
        if (slot)
        {
            if (yo_font_backend_load_font(&ctx->font_backend, &slot->backend_info, data, data_size))
            {
                ret = slot->font_id;
                ok = true;
            }
        }
    }

    if (!ok)
    {
        yo_font_table_slot_free(slot);
    }

    return ret;
}

YO_API yo_font_id_t yo_font_load(void *data, size_t data_size)
{
    yo_font_id_t ret = yo_font_load_ctx(data, data_size, yo_ctx);
    return ret;
}

YO_API void yo_font_unload(yo_font_id_t font)
{
    yo_font_slot_t *slot = yo_font_table_slot_find(font);
    yo_font_table_slot_free(slot);
}

////////////////////////////////////////////////////////////////
//
//
// Box tree
//
//
////////////////////////////////////////////////////////////////

static void yo_debug_check_recursive_parents(yo_context_t *context, yo_box_t *box)
{
#define YO_LOOP_CHILDREN(box) yo_box_t *child = box->children.first; child != NULL; child = child->next
#define YO_LOOP_PARENTS(box) yo_box_t *parent = box->parent; parent != NULL; parent = parent->parent

    if (box)
    {
        for (YO_LOOP_PARENTS(box))
        {
            if (parent == box) { YO_ASSERT(false); }
        }

        for (YO_LOOP_CHILDREN(box))
        {
            yo_debug_check_recursive_parents(context, child);
        }
    }
}

static void yo_add_child_box(yo_box_t *parent, yo_box_t *child)
{
    YO_ASSERT(parent != child);

    child->parent = parent;

    yo_slist_queue_push(&parent->children, child);
}

static yo_popup_t *yo_get_popup_by_id(yo_id_t id); // TODO(rune): Cleanup declaration order

static yo_box_t *yo_push_box(yo_id_t id, yo_box_flags_t flags)
{
    if (yo_ctx->error)
    {
        yo_zero_struct(&stub_box);
        return &stub_box;
    }

    yo_box_t *box = yo_alloc_box(id);
    if (box == NULL)
    {
        yo_set_error(YO_ERROR_OUT_OF_TEMPORARY_MEMORY);
        yo_zero_struct(&stub_box);
        return &stub_box;
    }

    if (id)
    {
        //
        // Check id collision
        //

        yo_box_t *already_in_hash = yo_get_box_by_id_this_frame(id);
        YO_ASSERT(already_in_hash == NULL && "Id collision");

        //
        // Store in hash
        //

        yo_add_box_to_hash(box, yo_ctx->this_frame);

        //
        // Copy persistent state from previous frame
        //

        yo_box_t *box_prev = yo_get_box_by_id_prev_frame(id);
        if (box_prev)
        {
            box->text_field_state = box_prev->text_field_state;

            if (box_prev->userdata)
            {
                box->userdata = yo_arena_push_size(&yo_ctx->this_frame->arena, box_prev->userdata_size, false);
                if (box->userdata)
                {
                    box->userdata_size = box_prev->userdata_size;
                    yo_memcpy(box->userdata, box_prev->userdata, box_prev->userdata_size);
                }
            }
        }
    }


    //
    // Setup per frame parameters
    //

    box->flags      = flags;
    box->anim_rate  = 10.0f;
    box->dim_x.min  = 0.0f;
    box->dim_y.min  = 0.0f;
    box->dim_x.max  = YO_FLOAT32_MAX;
    box->dim_y.max  = YO_FLOAT32_MAX;
    box->font       = yo_ctx->default_font;
    box->font_color = yo_v4f(1.0f, 0.0f, 1.0f, 1.0f);
    box->font_size  = 20;

    //
    // Hierarchy
    //

    if (yo_ctx->parent_stack.count > 0)
    {
        yo_box_t *current_parent = yo_ctx->parent_stack.elems[yo_ctx->parent_stack.count - 1];
        yo_add_child_box(current_parent, box);
    }

    //
    // Persistent state
    //

    if (id)
    {
        // TODO(rune): Better way to check for input. We don't need the whole yo_signal_t struct.
        yo_signal_t signal = yo_get_signal(box);

        //
        // Popup
        //

        for (size_t i = 0; i < yo_array_count(&yo_ctx->popup_build_stack); i++)
        {
            yo_id_t     current_popup_id = yo_ctx->popup_build_stack.elems[i];
            yo_popup_t *current_popup    = yo_get_popup_by_id(current_popup_id);

            if (current_popup && current_popup->is_building)
            {
                box->on_top = true;

                if (signal.clicked) { current_popup->got_click = true; }
                if (signal.hovered)
                {
                    current_popup->got_hover = true;
                    current_popup->close_on_frame_count = yo_ctx->frame_count + yo_ctx->config.popup_close_delay;
                }
            }
        }

        //
        // Input flags
        //

        if ((box->flags & YO_BOX_HOT_ON_HOVER) &&
            (signal.hovered))
        {
            yo_ctx->this_frame->hot_id = id;
        }

        if ((box->flags & (YO_BOX_ACTIVATE_ON_CLICK|YO_BOX_ACTIVATE_ON_HOLD)) &&
            (signal.clicked))
        {
            yo_ctx->this_frame->active_id = id;
        }
    }

    yo_ctx->latest_child = box;

    return box;
}

////////////////////////////////////////////////////////////////
//
//
// Animation
//
//
////////////////////////////////////////////////////////////////

static float yo_anim_f32(float rate, float origin, float target)
{
    // TODO(rune): This is quite branchy. Could we do something branch prediction friendly?

    // TODO(rune): User configurable epsilon?
    const float EPSILON = 0.01f;

    float ret = origin + (target - origin) * rate * yo_delta();
    ret = YO_CLAMP(ret, YO_MIN(origin, target), YO_MAX(origin, target));

    if (fabsf(origin - target) < EPSILON) ret = target;
    if (ret != origin) yo_ctx->this_frame->played_anim = true;

    return ret;
}

static yo_v2f_t yo_anim_v2f(float rate, yo_v2f_t origin, yo_v2f_t target)
{
    yo_v2f_t ret =
    {
        .x = yo_anim_f32(rate, origin.x, target.x),
        .y = yo_anim_f32(rate, origin.y, target.y),
    };

    return ret;
}

static yo_v4f_t yo_anim_v4f(float rate, yo_v4f_t origin, yo_v4f_t target)
{
    // TODO(rune): SIMD?

    yo_v4f_t ret =
    {
        .a = yo_anim_f32(rate, origin.a, target.a),
        .r = yo_anim_f32(rate, origin.r, target.r),
        .g = yo_anim_f32(rate, origin.g, target.g),
        .b = yo_anim_f32(rate, origin.b, target.b),
    };

    return ret;
}

static yo_corners_f32_t yo_anim_corners_f32(float rate, yo_corners_f32_t origin, yo_corners_f32_t target)
{
    // TODO(rune): SIMD?

    yo_corners_f32_t ret =
    {
        .corner[0] = yo_anim_f32(rate, origin.corner[0], target.corner[0]),
        .corner[1] = yo_anim_f32(rate, origin.corner[1], target.corner[1]),
        .corner[2] = yo_anim_f32(rate, origin.corner[2], target.corner[2]),
        .corner[3] = yo_anim_f32(rate, origin.corner[3], target.corner[3]),
    };

    return ret;
}

static yo_sides_f32_t yo_anim_sides_f32(float rate, yo_sides_f32_t origin, yo_sides_f32_t target)
{
    // TODO(rune): SIMD?

    yo_sides_f32_t ret =
    {
        .side[0] = yo_anim_f32(rate, origin.side[0], target.side[0]),
        .side[1] = yo_anim_f32(rate, origin.side[1], target.side[1]),
        .side[2] = yo_anim_f32(rate, origin.side[2], target.side[2]),
        .side[3] = yo_anim_f32(rate, origin.side[3], target.side[3]),
    };

    return ret;
}

static void yo_anim_box(yo_box_t *box)
{
    if (!box->anim) return;

    yo_box_t *prev = yo_get_box_by_id_prev_frame(box->id);

    if (!prev) return;

    float rate = box->anim_rate;

    // TODO(rune): This is quite branchy. Could we do something branch prediction friendly?
    if (box->anim & YO_ANIM_FILL)           box->fill          = yo_anim_v4f(rate, prev->fill, box->fill);
    if (box->anim & YO_ANIM_BORDER_COLOR)   box->border.color  = yo_anim_v4f(rate, prev->border.color, box->border.color);
    if (box->anim & YO_ANIM_BORDER_RADIUS)  box->border.radius = yo_anim_corners_f32(rate, prev->border.radius, box->border.radius);
    if (box->anim & YO_ANIM_FONT_COLOR)     box->font_color    = yo_anim_v4f(rate, prev->font_color, box->font_color);
    if (box->anim & YO_ANIM_MARGIN)         box->margin        = yo_anim_sides_f32(rate, prev->margin, box->margin);
    if (box->anim & YO_ANIM_PADDING)        box->padding       = yo_anim_sides_f32(rate, prev->padding, box->padding);
    if (box->anim & YO_ANIM_SCROLL)         box->scroll_offset = yo_anim_v2f(rate, prev->scroll_offset, box->scroll_offset);
}

////////////////////////////////////////////////////////////////
//
//
// Measure
//
//
////////////////////////////////////////////////////////////////

static yo_laid_out_codepoint_t *yo_alloc_laid_out_codepoint(yo_laid_out_text_t *text)
{
    yo_laid_out_codepoint_t *ret = NULL;
    yo_laid_out_text_chunk_t *chunk = text->chunks.last;

    if ((!chunk) || (chunk->codepoint_count < countof(chunk->codepoints)))
    {
        chunk = yo_arena_push_struct(&yo_ctx->this_frame->arena, yo_laid_out_text_chunk_t, true);
        if (chunk)
        {
            yo_slist_add(&text->chunks, chunk);
        }
    }

    if (chunk)
    {
        ret = &chunk->codepoints[chunk->codepoint_count++];
    }

    return ret;
}

static yo_laid_out_text_t yo_layout_text(yo_string_t text, yo_text_flags_t flags, yo_v2f_t wrap, yo_font_id_t font, uint32_t font_size)
{
    yo_laid_out_text_t ret = { 0 };
    ret.font = font;
    ret.font_size = font_size;

    yo_font_metrics_t font_metrics = yo_font_metrics(font, font_size);
    yo_v2f_t pos = yo_v2f(0, 0);

    float height = font_metrics.line_gap;
    float baseline = font_metrics.ascent;

    for (uint32_t codepoint = yo_utf8_codepoint_advance(&text);
         codepoint;
         codepoint = yo_utf8_codepoint_advance(&text))
    {
        yo_laid_out_codepoint_t *u = yo_alloc_laid_out_codepoint(&ret);
        if (u)
        {
            yo_atlas_node_t *glyph = yo_glyph_get(font, &yo_ctx->atlas, codepoint, font_size, false);
            if (glyph)
            {
                if (pos.x + glyph->advance_x > wrap.x)
                {
                    if ((flags & YO_TEXT_WRAP) && (pos.y + height < wrap.y))
                    {
                        pos.x = 0.0f;
                        pos.y += height;
                    }
                    else
                    {
                        break;
                    }
                }

                yo_v2f_t dim    = yo_v2f((float)glyph->rect.w, (float)glyph->rect.h);
                yo_v2f_t offset = yo_v2f(glyph->bearing_x, glyph->bearing_y + baseline);

                u->u32       = codepoint;
                u->rect.p0 = yo_v2f_add(pos, offset);
                u->rect.p1 = yo_v2f_add(pos, yo_v2f_add(offset, dim));

                pos.x += glyph->advance_x;

                ret.dim.x = YO_MAX(ret.dim.x, pos.x);
                ret.dim.y = YO_MAX(ret.dim.y, pos.y);
            }
        }
        else
        {
            break;
        }
    }

    ret.dim.y += height;

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Layout
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_range_f32 yo_range_f32_t;
struct yo_range_f32
{
    float min, max;
};

static float yo_align(yo_align_t alignment, float pref_dim, float avail_dim)
{
    float ret = { 0 };

    // TODO(rune): This could just be a lookup table.
    switch (alignment)
    {
        case YO_ALIGN_CENTER:   ret = (avail_dim - pref_dim) / 2.0f; break;
        case YO_ALIGN_FRONT:    ret = 0.0f;                          break;
        case YO_ALIGN_BACK:     ret = avail_dim - pref_dim;          break;
        case YO_ALIGN_STRETCH:  ret = 0.0f;                          break; // TODO(rune): Is there any reason to keep YO_ALIGN_STRETCH?
        default:                ret = 0.0f;                          break;
    }

    return ret;
}

static yo_v2f_t yo_layout_recurse(yo_box_t *box, yo_v2f_t avail_min, yo_v2f_t avail_max)
{
    yo_v2f_t ret = { 0.0f, 0.0f };

    //
    // NOTE(rune): Obey fixed sized constraints.
    //

    yo_v2f_max_assign(&avail_min, yo_v2f(box->dim_x.min, box->dim_y.min));
    yo_v2f_min_assign(&avail_max, yo_v2f(box->dim_x.max, box->dim_y.max));

    //
    // NOTE(rune): Padding and margin gives less room for children.
    //

    yo_v2f_t avail_for_children_min = avail_min;
    yo_v2f_t avail_for_children_max = avail_max;

    yo_v2f_sub_assign(&avail_for_children_min, box->padding.p[0]);
    yo_v2f_sub_assign(&avail_for_children_min, box->padding.p[1]);
    yo_v2f_sub_assign(&avail_for_children_max, box->padding.p[0]);
    yo_v2f_sub_assign(&avail_for_children_max, box->padding.p[1]);

    yo_v2f_sub_assign(&avail_for_children_min, box->margin.p[0]);
    yo_v2f_sub_assign(&avail_for_children_min, box->margin.p[1]);
    yo_v2f_sub_assign(&avail_for_children_max, box->margin.p[0]);
    yo_v2f_sub_assign(&avail_for_children_max, box->margin.p[1]);

    if (box->children.first)
    {
        const uint32_t LAYOUT_AXIS_MASK = 1;

        // TODO(rune): We could just write the layout code, so that it always lays out children in the x-axis,
        // and then just swap the x-y coordinates when layout is finished. Would probably make give the compiler
        // a better opportunity to optimize. Profile!

        // TODO(rune): We iterate the children multiple times, but they are not laid out sequentially in memory.
        // We should consider iterating fewer times, or changing the data layout. Profile!

        yo_axis_t axis = box->child_layout & LAYOUT_AXIS_MASK;
        yo_axis_t orto = !axis;

        switch (box->child_layout & ~LAYOUT_AXIS_MASK)
        {
            case YO_LAYTOUT_NONE:
            {
                //
                // NOTE(rune): Measure children.
                //

                for (yo_slist_each(yo_box_t *, child, box->children.first))
                {
                    yo_v2f_t min = yo_v2f(0, 0);
                    yo_v2f_t max = avail_for_children_max;

                    if (child->dim_x.is_rel) min.x = max.x * YO_CLAMP(child->dim_x.rel, 0.0f, 1.0f);
                    if (child->dim_y.is_rel) min.y = max.y * YO_CLAMP(child->dim_y.rel, 0.0f, 1.0f);

                    child->pref_dim = yo_layout_recurse(child, min, max);

                    yo_v2f_max_assign(&ret, child->pref_dim);
                }

                yo_v2f_clamp_assign(&ret, avail_for_children_min, avail_for_children_max);

                //
                // NOTE(rune): Place children.
                //

                for (yo_slist_each(yo_box_t *, child, box->children.first))
                {
                    float aligned_x = yo_align(child->align_x, child->pref_dim.x, ret.x);
                    float aligned_y = yo_align(child->align_y, child->pref_dim.y, ret.y);

                    child->layout_rect.p0 = yo_v2f(aligned_x, aligned_y);
                    child->layout_rect.p1 = yo_v2f_add(child->layout_rect.p0, child->pref_dim);
                }

            } break;

            case YO_LAYOUT_STACK:
            {
                yo_v2f_t child_pref_dim_max = yo_v2f(0, 0);

                yo_v2f_t remaining_min = avail_for_children_min;
                yo_v2f_t remaining_max = avail_for_children_max;

                uint32_t child_count     = 0;
                uint32_t child_count_abs = 0;
                uint32_t child_count_rel = 0;
                float    total_abs       = 0.0f;
                float    total_rel       = 0.0f;

                //
                // NOTE(rune): Measure fixed sized children, and calculate sum of children rel.
                //

                for (yo_slist_each(yo_box_t *, child, box->children.first))
                {
                    if (child->dim_a[axis].is_rel == false)
                    {
                        yo_v2f_t min = yo_v2f(0, 0);
                        yo_v2f_t max = remaining_max;

                        if (child->dim_a[orto].is_rel) min.v[orto] = child_pref_dim_max.v[orto] * child->dim_a[orto].rel;

                        yo_v2f_t child_pref_dim = yo_layout_recurse(child, min, max);
                        child->pref_dim = child_pref_dim;

                        remaining_min.v[axis] -= child_pref_dim.v[axis];
                        remaining_max.v[axis] -= child_pref_dim.v[axis];

                        child_pref_dim_max.x = YO_MAX(child_pref_dim_max.x, child_pref_dim.x);
                        child_pref_dim_max.y = YO_MAX(child_pref_dim_max.y, child_pref_dim.y);

                        child_count_abs++;
                        total_abs += child_pref_dim.v[axis];

                        ret.v[axis] += child->pref_dim.v[axis];
                        ret.v[orto] = YO_MAX(ret.v[orto], child->pref_dim.v[orto]);
                    }
                    else
                    {
                        child_count_rel++;
                        total_rel += child->dim_a[axis].rel;
                    }

                    child_count++;
                }

                remaining_min.x = YO_CLAMP_LOW(remaining_min.x, 0.0f);
                remaining_min.y = YO_CLAMP_LOW(remaining_min.y, 0.0f);

                //
                // NOTE(rune): Measure relative sized children.
                //

                for (yo_slist_each(yo_box_t *, child, box->children.first))
                {
                    if (child->dim_a[axis].is_rel)
                    {
                        float fraction = child->dim_a[axis].rel / total_rel;

                        yo_v2f_t space_for_child = { 0.0f, 0.0f };
                        space_for_child.v[axis] = (avail_max.v[axis] - total_abs) * fraction;
                        space_for_child.v[orto] = avail_for_children_max.v[orto];

                        yo_v2f_t min = yo_v2f(0, 0);
                        yo_v2f_t max = space_for_child;

                        min.v[axis] = max.v[axis];

                        if (child->dim_a[orto].is_rel)
                        {
                            min.v[orto] = child_pref_dim_max.v[orto] * child->dim_a[orto].rel;
                        }

                        yo_v2f_t child_pref_dim = yo_layout_recurse(child, min, max);
                        child->pref_dim = child_pref_dim;

                        remaining_min.v[axis] -= child_pref_dim.v[axis];
                        remaining_max.v[axis] -= child_pref_dim.v[axis];

                        ret.v[axis] += child->pref_dim.v[axis];
                        ret.v[orto] = YO_MAX(ret.v[orto], child->pref_dim.v[orto]);
                    }
                }



                //
                // NOTE(rune): Place children.
                //

                yo_v2f_clamp_assign(&ret, avail_for_children_min, avail_for_children_max);
                float space = YO_CLAMP_LOW(remaining_min.v[axis], 0.0f) / (child_count + 1);
                float pos = space;

                for (yo_slist_each(yo_box_t *, child, box->children.first))
                {
                    child->layout_rect.p0.v[axis] = pos;
                    child->layout_rect.p1.v[axis] = pos + child->pref_dim.v[axis];

                    float aligned_orto = yo_align(child->align_a[orto], child->pref_dim.v[orto], ret.v[orto]);

                    child->layout_rect.p0.v[orto] = aligned_orto;
                    child->layout_rect.p1.v[orto] = aligned_orto + child->pref_dim.v[orto];

                    pos += child->pref_dim.v[axis];
                    pos += space;
                }

            } break;

            default:
            {
                YO_ASSERT(!"Invalid child layout");
            } break;
        }
    }
    else
    {
        if (box->text)
        {
            yo_string_t string = yo_from_cstring(box->text);
            box->laid_out_text = yo_layout_text(string, YO_TEXT_WRAP, avail_for_children_max, box->font, box->font_size);

            ret.x = box->laid_out_text.dim.x;
            ret.y = box->laid_out_text.dim.y;
        }
    }

    //
    // NOTE(rune): Padding + margin means parent needs to allocate more space for box.
    //

    yo_v2f_add_assign(&ret, box->padding.p[0]);
    yo_v2f_add_assign(&ret, box->padding.p[1]);

    yo_v2f_add_assign(&ret, box->margin.p[0]);
    yo_v2f_add_assign(&ret, box->margin.p[1]);

    //
    // NOTE(rune): Obey constraints given by parent.
    //

    yo_v2f_clamp_assign(&ret, avail_min, avail_max);

    return ret;
}

static void yo_post_layout_recurse(yo_box_t *box, yo_v2f_t parent_top_left)
{
    box->screen_rect.x0 = box->layout_rect.x0 + parent_top_left.x + box->margin.p[0].x;
    box->screen_rect.y0 = box->layout_rect.y0 + parent_top_left.y + box->margin.p[0].y;
    box->screen_rect.x1 = box->layout_rect.x1 + parent_top_left.x + box->margin.p[0].x;
    box->screen_rect.y1 = box->layout_rect.y1 + parent_top_left.y + box->margin.p[0].y;

    parent_top_left.x = box->screen_rect.x0 + box->padding.p[0].x;
    parent_top_left.y = box->screen_rect.y0 + box->padding.p[0].y;

    for (yo_slist_each(yo_box_t *, child, box->children.first))
    {
        yo_post_layout_recurse(child, parent_top_left);
    }
}

////////////////////////////////////////////////////////////////
//
//
// Render
//
//
////////////////////////////////////////////////////////////////

static inline yo_v2f_t yo_scaled_space_to_pixel_space(yo_v2f_t scaled_point, yo_rectf_t pixel_rect)
{
    yo_v2f_t ret = yo_v2f(pixel_rect.x + (pixel_rect.w * scaled_point.x),
                          pixel_rect.y + (pixel_rect.h * scaled_point.y));

    return ret;
}

static inline yo_v4f_t yo_v4f_from_argb32(uint32_t argb32)
{
    yo_v4f_t ret =
    {
        .a = (float)((argb32 >> 24) & 0xff) / (float)(0xff),
        .r = (float)((argb32 >> 16) & 0xff) / (float)(0xff),
        .g = (float)((argb32 >> 8)  & 0xff) / (float)(0xff),
        .b = (float)((argb32 >> 0)  & 0xff) / (float)(0xff),
    };

    return ret;
}

static inline uint32_t yo_argb32_from_v4f(yo_v4f_t v)
{
#if 0
    YO_ASSERT(v.a >= 0.0f && v.a <= 1.0f);
    YO_ASSERT(v.r >= 0.0f && v.r <= 1.0f);
    YO_ASSERT(v.g >= 0.0f && v.g <= 1.0f);
    YO_ASSERT(v.b >= 0.0f && v.b <= 1.0f);
#endif

    uint32_t ret = (((uint32_t)(v.a * 0xff) << 24) |
                    ((uint32_t)(v.r * 0xff) << 16) |
                    ((uint32_t)(v.g * 0xff) << 8)  |
                    ((uint32_t)(v.b * 0xff) << 0));

    return ret;
}

static void yo_draw_triangle(yo_v2f_t p0, yo_v2f_t p1, yo_v2f_t p2,
                             yo_v4f_t color0, yo_v4f_t color1, yo_v4f_t color2)
{
    yo_draw_cmd_t *cmd = yo_array_add(&yo_ctx->draw_cmds, 1, true);
    if (cmd)
    {
        cmd->type = YO_DRAW_CMD_TRI;
        cmd->tri.p[0] = p0;
        cmd->tri.p[1] = p1;
        cmd->tri.p[2] = p2;
        cmd->tri.color[0] = color0;
        cmd->tri.color[1] = color1;
        cmd->tri.color[2] = color2;
    }
}

static void yo_draw_quad(yo_v2f_t p[4], yo_v4f_t color[4])
{
    yo_draw_cmd_t *cmd = yo_array_add(&yo_ctx->draw_cmds, 1, true);
    if (cmd)
    {
        cmd->type = YO_DRAW_CMD_QUAD;
        cmd->quad.p[0] = p[0];
        cmd->quad.p[1] = p[1];
        cmd->quad.p[2] = p[2];
        cmd->quad.p[3] = p[3];
        cmd->quad.color[0] = color[0];
        cmd->quad.color[1] = color[1];
        cmd->quad.color[2] = color[2];
        cmd->quad.color[3] = color[3];
    }
}

// TODO(rune): This struct isn't necessary anymore, we can just use yo_draw_cmd_t.aabb directly instead.
typedef struct yo_draw_aabb yo_draw_aabb_t;
struct yo_draw_aabb
{
    // NOTE(rune): Position
    yo_v2f_t p0, p1, clip_p0, clip_p1;

    // NOTE(rune): Fill
    yo_corners_v4f_t color;
    yo_corners_f32_t radius;

    // NOTE(rune): Border
    float border_thickness;
    yo_v4f_t border_color;

    // NOTE(rune): Texturing
    uint32_t texture_id;
    yo_v2f_t uv0, uv1;
};

static void yo_draw_aabb(yo_draw_aabb_t draw)
{
#if 0
    YO_ASSERT(draw.p0.x <= draw.p1.x);
    YO_ASSERT(draw.p0.y <= draw.p1.y);
#else
    if ((draw.p0.x > draw.p1.x) || (draw.p0.y > draw.p1.y)) return;
#endif

    yo_draw_cmd_t *cmd = yo_array_add(&yo_ctx->draw_cmds, 1, false);
    if (cmd)
    {
        cmd->type                  = YO_DRAW_CMD_AABB;
        cmd->aabb.p0               = draw.p0;
        cmd->aabb.p1               = draw.p1;
        cmd->aabb.clip_p0          = draw.clip_p0;
        cmd->aabb.clip_p1          = draw.clip_p1;
        cmd->aabb.color            = draw.color;
        cmd->aabb.radius           = draw.radius;
        cmd->aabb.border_thickness = draw.border_thickness;
        cmd->aabb.border_color     = draw.border_color;
        cmd->aabb.texture_id       = draw.texture_id;
        cmd->aabb.uv0              = draw.uv0;
        cmd->aabb.uv1              = draw.uv1;
    }
}

static void yo_draw_laid_out_text(yo_laid_out_text_t laid_out, yo_v2f_t p0, yo_v2f_t p1)
{
    // TODO(rune): Cursor + selection

    for (yo_slist_each(yo_laid_out_text_chunk_t *, chunk, laid_out.chunks.first))
    {
        for (uint32_t i = 0; i < chunk->codepoint_count; i++)
        {
            yo_laid_out_codepoint_t codepoint = chunk->codepoints[i];
            yo_atlas_node_t *glyph_node = yo_glyph_get(laid_out.font, &yo_ctx->atlas, codepoint.u32, laid_out.font_size, true);
            if (glyph_node)
            {
                yo_rectf2_t uv = yo_atlas_node_uv(&yo_ctx->atlas, glyph_node);

                yo_draw_aabb_t draw =
                {
                    .p0         = yo_v2f_add(p0, codepoint.rect.p0),
                    .p1         = yo_v2f_add(p0, codepoint.rect.p1),
                    .clip_p0    = p0,
                    .clip_p1    = p1,
                    .color      = { YO_WHITE, YO_WHITE, YO_WHITE, YO_WHITE}, // TODO(rune): Text color.
                    .texture_id = 42, // TODO(rune): Hardcoded texture id
                    .uv0        = uv.p0,
                    .uv1        = uv.p1,
                };

                // TODO(rune): Support sub-pixel anti aliasing for text.
                draw.p0.x      = roundf(draw.p0.x);
                draw.p1.x      = roundf(draw.p1.x);
                draw.clip_p0.x = roundf(draw.clip_p0.x);
                draw.clip_p0.x = roundf(draw.clip_p0.x);
                draw.p0.y      = roundf(draw.p0.y);
                draw.p1.y      = roundf(draw.p1.y);
                draw.clip_p0.y = roundf(draw.clip_p0.y);
                draw.clip_p0.y = roundf(draw.clip_p0.y);

                yo_draw_aabb(draw);
            }
        }
    }
}

static void yo_render_recurse(yo_box_t *box, yo_render_info_t *render_info, bool on_top)
{
    YO_PROFILE_BEGIN(yo_render_recurse);

    yo_anim_box(box);

#if 1
    if (yo_cstring_equal(box->tag, "ABCDEF"))
    {
        __nop();
    }

    if (yo_cstring_equal(box->text, "31"))
    {
        __nop();
    }
#endif

    yo_rectf2_t rect = box->screen_rect;

    yo_v2f_t p0 = yo_v2f(rect.x0, rect.y0);
    yo_v2f_t p1 = yo_v2f(rect.x1, rect.y1);

    bool clip_to_parent_x = (box->parent) && (box->parent->overflow_x != YO_OVERFLOW_SPILL) && (box->parent->overflow_x != YO_OVERFLOW_SCROLL);
    bool clip_to_parent_y = (box->parent) && (box->parent->overflow_x != YO_OVERFLOW_SPILL) && (box->parent->overflow_x != YO_OVERFLOW_SCROLL);

    yo_v2f_t clip_p0 = p0;
    yo_v2f_t clip_p1 = p1;

    if (clip_to_parent_x)
    {
        clip_p0.x = box->parent->screen_rect.x0;
        clip_p1.x = box->parent->screen_rect.x1;
    }

    if (clip_to_parent_y)
    {
        clip_p0.y = box->parent->screen_rect.y0;
        clip_p1.y = box->parent->screen_rect.y1;
    }

    if (box->on_top == on_top && box->id != YO_ID_ROOT)
    {
        //
        // Draw fill and border
        //
        {
            yo_corners_v4f_t corner_background = { box->fill, box->fill, box->fill, box->fill };
            yo_draw_aabb_t draw =
            {
                .p0               = p0,
                .p1               = p1,
                .clip_p0          = clip_p0,
                .clip_p1          = clip_p1,
                .color            = corner_background,
                .border_color     = box->border.color,
                .border_thickness = box->border.thickness,
                .radius           = box->border.radius,
            };

            if (box->flags & YO_BOX_FULL_ATLAS)
            {
                draw.texture_id = 42; // TODO(rune): Hardcoded texture id.
                draw.uv0        = yo_v2f(0.0f, 0.0f);
                draw.uv1        = yo_v2f(1.0f, 1.0f);
            }

            yo_draw_aabb(draw);
        }

        p0.axis[YO_AXIS_X] += (int32_t)(box->border.thickness);
        p0.axis[YO_AXIS_Y] += (int32_t)(box->border.thickness);
        p1.axis[YO_AXIS_X] -= (int32_t)(box->border.thickness);
        p1.axis[YO_AXIS_Y] -= (int32_t)(box->border.thickness);

        //
        // Draw content
        //

#if 0
        if (box->text)
        {
            yo_text_field_state_t *text_field_state = NULL;
            if ((box->id == yo_ctx->this_frame->active_id) && (box->flags & YO_BOX_DRAW_TEXT_CURSOR))
            {
                text_field_state = &box->text_field_state;
            }

            yo_draw_text(yo_from_cstring(box->text),
                         0, 0,
                         p0, p1, clip_p0, clip_p1,
                         box->font,
                         box->font_size,
                         box->font_color,
                         box->measured_text,
                         text_field_state);
        }
#else
        if (box->laid_out_text.chunks.first)
        {
            yo_draw_laid_out_text(box->laid_out_text, p0, p1);
        }
#endif
        //
        // Draw scaled elems
        //

        {
            for (yo_scaled_element_t *elm = box->scaled_elements;
                 elm != NULL;
                 elm = elm->next)
            {
                switch (elm->type)
                {
                    case YO_SCALED_ELEMENT_TYPE_TRIANGLE:
                    {
                        yo_rectf_t map_rect = { p0.x, p0.y, p1.x - p0.x, p1.y - p0.y };

                        elm->triangle.p0 = yo_scaled_space_to_pixel_space(elm->triangle.p0, map_rect);
                        elm->triangle.p1 = yo_scaled_space_to_pixel_space(elm->triangle.p1, map_rect);
                        elm->triangle.p2 = yo_scaled_space_to_pixel_space(elm->triangle.p2, map_rect);
                        yo_draw_triangle(elm->triangle.p0, elm->triangle.p1, elm->triangle.p2,
                                         elm->triangle.color0, elm->triangle.color1, elm->triangle.color2);
                    } break;

                    default:
                    {
                        YO_ASSERT(false);
                    } break;
                }
            }
        }
    }

    //
    // Draw content
    //

    for (yo_box_t *child = box->children.first;
         child != NULL;
         child = child->next)
    {
        yo_render_recurse(child, render_info, on_top);
    }

    YO_PROFILE_END(yo_render_recurse);
}

////////////////////////////////////////////////////////////////
//
//
// Debug
//
//
////////////////////////////////////////////////////////////////

#define YO_DEBUG_PRINT_ANSI_CONSOLE
#include "colors.h"

typedef struct yo_print_buffer yo_print_buffer_t;
struct yo_print_buffer
{
    char chars[YO_MEGABYTES(1)];
    uint32_t pos;
};

static yo_print_buffer_t yo_global_print_buffer;

static void yo_clear_print_buffer(void)
{
    yo_zero_struct(&yo_global_print_buffer);
}

static void yo_flush_print_buffer(void)
{
#ifdef YO_DEBUG_PRINT_ANSI_CONSOLE
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD bytesWritten = 0;
    WriteFile(out, yo_global_print_buffer.chars, yo_global_print_buffer.pos, &bytesWritten, NULL);
#else
    OutputDebugStringA(yo_global_print_buffer.chars);
#endif
}

static void yo_debug_print(YO_PRINTF_FORMAT_STRING char *format, ...)
{
    va_list args;
    va_start(args, format);

    yo_global_print_buffer.pos += vsprintf_s(yo_global_print_buffer.chars + yo_global_print_buffer.pos,
                                             sizeof(yo_global_print_buffer.chars) - yo_global_print_buffer.pos,
                                             format,
                                             args);

    va_end(args);
}

static void yo_debug_print_clear(void)
{
#ifdef YO_DEBUG_PRINT_ANSI_CONSOLE
    yo_debug_print("\033[2J");   // Erase entire screen
    yo_debug_print("\033[H");    // Move cursor to (0, 0)
#endif
}

static void yo_debug_print_divider(void)
{
#ifdef YO_DEBUG_PRINT_ANSI_CONSOLE
    yo_debug_print(ANSI_RESET "--------------------------\n");
#else
    yo_debug_print("==========================================\n");
#endif
}

static void yo_debug_print_input(void)
{
    yo_debug_print_divider();
    yo_debug_print("Mouse: (%8i %8i)\n", yo_ctx->this_frame->mouse_pos.x, yo_ctx->this_frame->mouse_pos.y);
}

static void yo_debug_print_hierarchy(yo_box_t *box, uint32_t depth)
{
    YO_UNUSED(box);
    YO_UNUSED(depth);

#if 1
    if (depth == 0)
    {
        yo_debug_print_divider();
    }

    yo_debug_print("(%4i, %4i)-(%4i, %4i)       (%4i, %4i) (%4i x %4i) ",
                   (int)box->layout_rect.x0,
                   (int)box->layout_rect.y0,
                   (int)box->layout_rect.x1,
                   (int)box->layout_rect.y1,
                   (int)box->screen_rect.x0,
                   (int)box->screen_rect.y0,
                   (int)box->screen_rect.x1 - box->screen_rect.x0,
                   (int)box->screen_rect.y1 - box->screen_rect.y0);

    for (uint32_t i = 0; i < depth; i++)
    {
        yo_debug_print("  ");
    }

#if 0
    if (box->persistent)
    {
        yo_debug_print(ANSI_FG_DARK_MAGENTA);
    }
    else
    {
        yo_debug_print(ANSI_FG_DEFAULT);
    }
#endif

    yo_debug_print("%s\n", YO_COALESCE(YO_COALESCE(box->text, box->tag), "no tag"));


    for (yo_slist_each(yo_box_t *, child, box->children.first))
    {
        yo_debug_print_hierarchy(child, depth + 1);
    }

#endif
}

static void yo_debug_print_cache(void)
{
    yo_debug_print_divider();
}

static void yo_debug_print_freelist(void)
{
}

static void yo_debug_print_performance(void)
{
    yo_debug_print_divider();

#ifdef YO_DEBUG_PRINT_ANSI_CONSOLE

#else
    yo_debug_print("\n === Memory usage ===  \n"
                   "Draw commands:   %-12llu \n"
                   "Frame arena:     %-12llu \n"
                   "Atlas arena:     %-12llu \n",
                   yo_array_size(&yo_ctx->draw_cmds),
                   yo_ctx->this_frame->arena.self.size_used - sizeof(yo_context_t),
                   yo_ctx->atlas.storage.self.size_used);
#endif

    struct yo_perf_timing_set frameTimings = yo_ctx->timings[yo_ctx->timings_index];
    struct yo_perf_timing_set maxTimings = { 0 };

    for (uint32_t i = 0; i < countof(yo_ctx->timings); i++)
    {
        maxTimings.build.millis   = YO_MAX(maxTimings.build.millis, yo_ctx->timings[i].build.millis);
        maxTimings.measure.millis = YO_MAX(maxTimings.measure.millis, yo_ctx->timings[i].measure.millis);
        maxTimings.arrange.millis = YO_MAX(maxTimings.arrange.millis, yo_ctx->timings[i].arrange.millis);
        maxTimings.render.millis  = YO_MAX(maxTimings.render.millis, yo_ctx->timings[i].render.millis);
        maxTimings.prune.millis   = YO_MAX(maxTimings.prune.millis, yo_ctx->timings[i].prune.millis);
    }

#ifdef YO_DEBUG_PRINT_ANSI_CONSOLE
    YO_UNUSED(frameTimings, maxTimings);
#else
    yo_debug_print("\n=== Graphics ===         \n"
                   "Draw cmd count:    %-12llu \n",
                   yo_ctx->draw_cmds.count);

    yo_debug_print("\n=== Timing (last frame) === \n"
                   "Build:           %-12.4f \n"
                   "Measure:         %-12.4f \n"
                   "Arrange:         %-12.4f \n"
                   "Render:          %-12.4f \n"
                   "Prune:           %-12.4f \n",
                   frameTimings.build.millis,
                   frameTimings.measure.millis,
                   frameTimings.arrange.millis,
                   frameTimings.render.millis,
                   frameTimings.prune.millis);

    yo_debug_print("\n=== Timing (rolling max 100) === \n"
                   "Build:           %-12.4f \n"
                   "Measure:         %-12.4f \n"
                   "Arrange:         %-12.4f \n"
                   "Render:          %-12.4f \n"
                   "Prune:           %-12.4f \n",
                   maxTimings.build.millis,
                   maxTimings.measure.millis,
                   maxTimings.arrange.millis,
                   maxTimings.render.millis,
                   maxTimings.prune.millis);
#endif

    if (yo_ctx->error)
    {
        char *errorMessage = yo_get_error_message();
#ifdef YO_DEBUG_PRINT_ANSI_CONSOLE
        yo_debug_print(ANSI_FG_RED "Error: %s\n", errorMessage);
#else
        yo_debug_print("Error: %s\n", errorMessage);
#endif
    }
}

static void yo_debug_print_popups(void)
{
    for (size_t i = 0; i < yo_ctx->popups.count; i++)
    {
        yo_popup_t *p = &yo_ctx->popups.elems[i];
        yo_debug_print("%llu\t%llu\t%llu\n", p->id, p->group_id, p->close_on_frame_count);
    }
}

////////////////////////////////////////////////////////////////
//
//
// Core
//
//
////////////////////////////////////////////////////////////////

YO_API yo_context_t *yo_create_context(yo_config_t *user_config)
{
    bool ok = true;

    yo_config_t config = user_config ? *user_config : yo_default_config();

    yo_context_t context_on_stack = { .config = config };
    ok &= yo_arena_create(&context_on_stack.persist, YO_MEGABYTES(1), true, YO_ARENA_TYPE_CHAIN_EXPONENTIAL);
    ok &= yo_arena_create(&context_on_stack.frame_states[0].arena, YO_MEGABYTES(1), true, YO_ARENA_TYPE_CHAIN_EXPONENTIAL);
    ok &= yo_arena_create(&context_on_stack.frame_states[1].arena, YO_MEGABYTES(1), true, YO_ARENA_TYPE_CHAIN_EXPONENTIAL);
    ok &= yo_array_create(&context_on_stack.frame_states[0].events, 256, true);
    ok &= yo_array_create(&context_on_stack.frame_states[1].events, 256, true);
    ok &= yo_array_create(&context_on_stack.parent_stack, 4096, false);
    ok &= yo_array_create(&context_on_stack.id_stack, 4096, false);
    ok &= yo_array_create(&context_on_stack.popups, 4096, false);
    ok &= yo_array_create(&context_on_stack.popup_build_stack, 4096, false);
    ok &= yo_array_create(&context_on_stack.draw_cmds, 4096, false);
    ok &= yo_atlas_create(&context_on_stack.atlas, yo_v2i(512, 512));

    yo_context_t *ret = NULL;
    if (ok)
    {
        // NOTE(rune): Store context inside it's own persistent storage.
        ret = yo_arena_push_struct(&context_on_stack.persist, yo_context_t, false);
        if (ret)
        {
            *ret = context_on_stack;

            //
            // Frame state
            //

            ret->this_frame = &ret->frame_states[0];
            ret->prev_frame = &ret->frame_states[1];

            //
            // Font backend
            //

            yo_font_backend_startup(&ret->font_backend);
            ret->default_font = yo_font_load_ctx(yo_default_font_data, sizeof(yo_default_font_data), ret);

            ok = true;
        }
        else
        {
            ok = false;
        }
    }

    if (!ok)
    {
        yo_destroy_context(&context_on_stack);
    }

    return ret;
}

YO_API void yo_destroy_context(yo_context_t *context)
{
    if (context)
    {
        yo_font_backend_shutdown(&context->font_backend);

        // WARNING(rune): Must free persistent storage last, since the context structure itself
        // is stored in persistent storage.
        yo_atlas_destroy(&context->atlas);
        yo_array_destroy(&context->parent_stack);
        yo_array_destroy(&context->id_stack);
        yo_array_destroy(&context->popups);
        yo_array_destroy(&context->popup_build_stack);
        yo_array_destroy(&context->draw_cmds);
        yo_array_destroy(&context->frame_states[0].events);
        yo_array_destroy(&context->frame_states[1].events);
        yo_arena_destroy(&context->frame_states[0].arena);
        yo_arena_destroy(&context->frame_states[1].arena);
        yo_arena_destroy(&context->persist);
    }
}

YO_API void yo_select_context(yo_context_t *context)
{
    yo_ctx = context;
}

YO_API bool yo_begin_frame(float time, yo_frame_flags_t flags)
{
    YO_PROFILE_BEGIN(yo_begin_frame);

    //
    // Lazy input / animations
    //

    if ((flags & YO_FRAME_FLAG_LAZY) && (yo_ctx->frame_count > 4))
    {

        // TODO(rune): Would be easier if all relevant fields were in a nested struct.
        bool lazy_input = ((yo_ctx->this_frame->events.count == 0) &&
                           (yo_ctx->prev_frame->events.count == 0) &&
                           yo_struct_equal(yo_ctx->this_frame->mouse_button, yo_ctx->prev_frame->mouse_button) &&
                           yo_struct_equal(yo_ctx->this_frame->mouse_button_down, yo_ctx->prev_frame->mouse_button_down) &&
                           yo_struct_equal(yo_ctx->this_frame->mouse_button_up, yo_ctx->prev_frame->mouse_button_up) &&
                           yo_v2f_equal(yo_ctx->this_frame->mouse_pos, yo_ctx->prev_frame->mouse_pos) &&
                           yo_v2f_equal(yo_ctx->this_frame->scroll, yo_ctx->prev_frame->scroll));

        bool lazy_anim = ((yo_ctx->this_frame->played_anim == false) &&
                          (yo_ctx->prev_frame->played_anim == false));

        yo_ctx->this_frame->lazy = lazy_input && lazy_anim;
    }
    else
    {
        yo_ctx->this_frame->lazy = false;
    }

    //
    // Setup per-frame builder data
    //

    yo_arena_reset(&yo_ctx->this_frame->arena);
    yo_array_reset(&yo_ctx->parent_stack, false);
    yo_array_reset(&yo_ctx->popup_build_stack, false);
    yo_array_reset(&yo_ctx->id_stack, false);

    yo_memset(yo_ctx->this_frame->hash_table, 0, sizeof(yo_ctx->this_frame->hash_table));

    yo_ctx->error                    = YO_ERROR_NONE;
    yo_ctx->root                     = yo_push_box(YO_ID_ROOT, 0);
    yo_ctx->root->tag                = "TAG_ROOT";
    yo_ctx->latest_child             = yo_ctx->root;
    yo_ctx->atlas.current_generation = yo_ctx->frame_count;

    yo_ctx->this_frame->time        = time;
    yo_ctx->this_frame->delta       = 1.0f / 60.0f; // TODO(rune): Fix time
    yo_ctx->this_frame->played_anim = false;

    //
    // Update hot id
    //

    yo_ctx->this_frame->hot_id = 0;

    //
    // Update active id
    //

    yo_box_t *prev_active_box = yo_get_box_by_id_prev_frame(yo_ctx->prev_frame->active_id);
    if (prev_active_box)
    {
        if (prev_active_box->flags & YO_BOX_ACTIVATE_ON_CLICK)
        {
            yo_ctx->this_frame->active_id = yo_query_mouse_button_down(YO_MOUSE_BUTTON_LEFT) ? 0 : yo_ctx->prev_frame->active_id;
        }
        else
        {
            yo_ctx->this_frame->active_id = yo_query_mouse_button(YO_MOUSE_BUTTON_LEFT) ? yo_ctx->prev_frame->active_id : 0;
        }
    }
    else
    {
        yo_ctx->this_frame->active_id = 0;
    }

    //
    // Setup perf timing
    //

    yo_ctx->timings_index = (yo_ctx->timings_index + 1) % countof(yo_ctx->timings);
    yo_begin_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].build);

    yo_begin_children();

    YO_PROFILE_END(yo_begin_frame);

    return yo_ctx->this_frame->lazy;

}

YO_API void yo_end_frame(yo_render_info_t *info)
{
    YO_PROFILE_BEGIN(yo_end_frame);

    yo_end_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].build);

    info->lazy = yo_ctx->this_frame->lazy;

    if (!yo_ctx->this_frame->lazy)
    {

        //
        // Layout
        //
        {
            YO_PROFILE_BEGIN(layout_pass);
            yo_v2f_t layout_dim = yo_v2f((float)info->w, (float)info->h);
            yo_ctx->root->pref_dim = layout_dim;
            yo_ctx->root->layout_rect.p0 = yo_v2f(0.0f, 0.0f);
            yo_ctx->root->layout_rect.p1 = layout_dim;
            yo_layout_recurse(yo_ctx->root, layout_dim, layout_dim);
            YO_PROFILE_END(layout_pass);

            YO_PROFILE_BEGIN(post_layout_pass);
            yo_post_layout_recurse(yo_ctx->root, yo_v2f(0.0f, 0.0f));
            YO_PROFILE_END(post_layout_pass);
        }

        //
        // Render
        //
        {
            YO_PROFILE_BEGIN(render_pass);

            yo_array_reset(&yo_ctx->draw_cmds, true);

            info->tex.id     = 42; // TODO(rune): Hardcoded texture id
            info->tex.dim   = yo_ctx->atlas.dim;
            info->tex.pixels = yo_ctx->atlas.pixels;
            info->tex.dirty  = yo_ctx->atlas.dirty;

            yo_render_recurse(yo_ctx->root, info, false);
            yo_render_recurse(yo_ctx->root, info, true);

            info->draw_cmds       = yo_ctx->draw_cmds.elems;
            info->draw_cmds_count = yo_ctx->draw_cmds.count;

            YO_PROFILE_END(render_pass);
        }

        //
        // Print debug information
        //
#if 0
        {
            yo_clear_print_buffer();

            yo_debug_print_clear();
            //yo_debug_print_input();
            //yo_debug_print_cache(context);
            yo_debug_print_hierarchy(yo_ctx->root, 0);
            //yo_debug_print_freelist(context);
            //yo_debug_print_performance();
            //yo_debug_print_popups();
            //yo_debug_svg_atlas(yo_g_context->glyph_atlas);

            yo_flush_print_buffer();
        }
#endif

    }

    //
    // Swap frame state
    //

    {
        yo_ctx->frame_count++;
        yo_ctx->this_frame = &yo_ctx->frame_states[(yo_ctx->frame_count + 0) % 2];
        yo_ctx->prev_frame = &yo_ctx->frame_states[(yo_ctx->frame_count + 1) % 2];
    }

    YO_PROFILE_END(yo_end_frame);
}

////////////////////////////////////////////////////////////////
//
//
// Input
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_input_clear(void)
{
    yo_ctx->this_frame->scroll = yo_v2f(0, 0);
    yo_array_reset(&yo_ctx->this_frame->events, false);
}

YO_API void yo_input_mouse_state(bool buttons[YO_MOUSE_BUTTON_COUNT], uint32_t x, uint32_t y)
{
    yo_ctx->this_frame->mouse_pos.x = (float)x; // TODO(rune): Input mouse in floating point?
    yo_ctx->this_frame->mouse_pos.y = (float)y;

    for (uint32_t i = 0; i < YO_MOUSE_BUTTON_COUNT; i++)
    {
        yo_ctx->this_frame->mouse_button[i]      =  buttons[i];
        yo_ctx->this_frame->mouse_button_down[i] =  buttons[i] && !yo_ctx->prev_frame->mouse_button[i];
        yo_ctx->this_frame->mouse_button_up[i]   = !buttons[i] &&  yo_ctx->prev_frame->mouse_button[i];
    }
}

YO_API void yo_input_mouse_click(yo_mouse_button_t button, uint32_t x, uint32_t y, yo_modifier_t modifiers)
{
    yo_event_t *e = yo_array_add(&yo_ctx->this_frame->events, 1, false);
    if (e)
    {
        switch (button)
        {
            case YO_MOUSE_BUTTON_LEFT:   e->type = YO_EVENT_TYPE_MOUSE_CLICK_LEFT;   break;
            case YO_MOUSE_BUTTON_RIGHT:  e->type = YO_EVENT_TYPE_MOUSE_CLICK_RIGHT;  break;
            case YO_MOUSE_BUTTON_MIDDLE: e->type = YO_EVENT_TYPE_MOUSE_CLICK_MIDDLE; break;
            default:                     e->type = YO_EVENT_TYPE_NONE;               break;
        }

        e->pos.x     = (float)x; // TODO(rune): Input mouse in floating point?
        e->pos.y     = (float)y;
        e->modifiers = modifiers;
    }
    else
    {
        // TODO(rune): Error handling
    }
}

YO_API void yo_input_scroll(float x, float y)
{
    yo_ctx->this_frame->scroll.x += x;
    yo_ctx->this_frame->scroll.y += y;
}

YO_API void yo_input_key(yo_keycode_t key, yo_modifier_t modifiers)
{
    yo_event_t *e = yo_array_add(&yo_ctx->this_frame->events, 1, false);
    if (e)
    {
        e->keycode   = key;
        e->modifiers = modifiers;
        e->type      = YO_EVENT_TYPE_KEY_PRESS;
    }
    else
    {
        // TODO(rune): Error handling
    }
}

YO_API void yo_input_char(char c, yo_modifier_t modifiers)
{
    yo_input_key(c, modifiers);
}

YO_API void yo_input_unicode(uint32_t codepoint, yo_modifier_t modifiers)
{
    yo_input_key(codepoint, modifiers);
}

YO_API yo_v2i_t yo_query_mouse_pos()
{
    // TODO(rune): Should mouse be in floating point?
    yo_v2i_t ret = yo_v2i((int32_t)yo_ctx->this_frame->mouse_pos.x,
                          (int32_t)yo_ctx->this_frame->mouse_pos.y);

    return ret;
}

YO_API bool yo_query_mouse_button(yo_mouse_button_t button)
{
    bool ret = yo_ctx->this_frame->mouse_button[button];
    return ret;
}

YO_API bool yo_query_mouse_button_down(yo_mouse_button_t button)
{
    bool ret = yo_ctx->this_frame->mouse_button_down[button];
    return ret;
}

YO_API bool yo_query_mouse_button_up(yo_mouse_button_t button)
{
    bool ret = yo_ctx->this_frame->mouse_button_up[button];
    return ret;
}

YO_API yo_v2f_t yo_query_scroll()
{
    yo_v2f_t ret = yo_ctx->this_frame->scroll;
    return ret;
}

YO_API float yo_delta()
{
    float ret = yo_ctx->this_frame->delta;
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Children
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_begin_children(void)
{
    yo_begin_children_of((yo_box_t *)yo_ctx->latest_child);
}

YO_API void yo_begin_children_of(yo_box_t *box)
{
    if (box)
    {
        if (box->id)
        {
            yo_push_id(box->id);
        }

        yo_array_put(&yo_ctx->parent_stack, box);
    }
    else
    {
        yo_set_error(YO_ERROR_PARENT_STACK_UNDERFLOW);
    }
}

YO_API void yo_end_children(void)
{
    if (yo_ctx->parent_stack.count > 0)
    {
        yo_box_t *current_parent = yo_array_last(&yo_ctx->parent_stack);
        if (current_parent->id)
        {
            yo_id_t popped = yo_pop_id();
            YO_ASSERT(popped == current_parent->id);
        }

        yo_ctx->latest_child = current_parent;
        yo_array_pop(&yo_ctx->parent_stack, 1);
    }
    else
    {
        yo_set_error(YO_ERROR_PARENT_STACK_UNDERFLOW);
    }
}

////////////////////////////////////////////////////////////////
//
//
// Popup
//
//
////////////////////////////////////////////////////////////////

static yo_popup_t *yo_get_popup_by_id(yo_id_t id)
{
    yo_popup_t *ret = NULL;

    for (size_t i = 0; i < yo_ctx->popups.count; i++)
    {
        if (yo_ctx->popups.elems[i].id == id)
        {
            ret = &yo_ctx->popups.elems[i];
            break;
        }
    }

    return ret;
}

YO_API void yo_open_popup(yo_id_t id, yo_popup_flags_t flags)
{
    yo_id_t group_id  = yo_array_last_or_default(&yo_ctx->popup_build_stack, 0);
    yo_popup_t *popup = yo_get_popup_by_id(id);

    if (!popup)
    {
        popup           = yo_array_add(&yo_ctx->popups, 1, true);
        popup->id       = id;
        popup->group_id = group_id;
    }

    if (popup)
    {
        // NOTE(rune): Close all other popups in same group.
        if ((flags & YO_POPUP_FLAG_EXCLUSIVE) && (group_id))
        {
            for (size_t i = 0; i < yo_ctx->popups.count; i++)
            {
                yo_popup_t *check = &yo_ctx->popups.elems[i];
                if (check->group_id == group_id)
                {
                    check->close_on_frame_count = 0;
                }
            }
        }

        popup->got_click = false;
        popup->got_hover = false;
        popup->close_on_frame_count = yo_ctx->frame_count + yo_ctx->config.popup_close_delay;
    }
}

static bool yo_popup_is_open(yo_id_t id)
{
    yo_popup_t *popup = yo_get_popup_by_id(id);

    bool ret = false;

    if (popup)
    {
        ret = (yo_ctx->frame_count <= popup->close_on_frame_count);
    }

    return ret;
}

YO_API bool yo_begin_popup(yo_id_t id)
{
    yo_popup_t *popup = yo_get_popup_by_id(id);

    bool is_open = yo_popup_is_open(id);

    if (popup)
    {
        popup->got_click = false;
        popup->got_hover = false;

        if (is_open)
        {
            if (yo_array_put(&yo_ctx->popup_build_stack, id))
            {
                popup->is_building = true;
            }
            else
            {
                is_open = false;
            }
        }
    }

    return is_open;
}

YO_API void yo_end_popup(void)
{
    if (yo_ctx->popup_build_stack.count > 0)
    {
        yo_id_t current_popup_id = yo_array_last(&yo_ctx->popup_build_stack);
        yo_popup_t *popup = yo_get_popup_by_id(current_popup_id);
        YO_ASSERT(popup);

        bool close = false;
        if (yo_ctx->frame_count >= popup->close_on_frame_count)
        {
            close = true;
        }

        yo_array_pop(&yo_ctx->popup_build_stack, 1);

        if (close)
        {
            size_t idx = popup - yo_ctx->popups.elems;

            if (!yo_array_remove(&yo_ctx->popups, idx, 1))
            {
                YO_ASSERT(false);
            }
        }
    }
    else
    {
        // NOTE(rune): Popup stack underflow
        // TODO(rune): yo_set_error
        YO_ASSERT(false);
    }
}

////////////////////////////////////////////////////////////////
//
//
// Id
//
//
////////////////////////////////////////////////////////////////

YO_API yo_id_t yo_id_from_string(char *string)
{
    yo_id_t ret = 0;

    if (string)
    {
        yo_id_t seed = yo_array_last_or_default(&yo_ctx->id_stack, 0);

        ret = fasthash64(string, strlen(string), seed);

        // NOTE(rune): Don't use reserved keys
        if (ret == YO_ID_NONE) ret = YO_ID_NONE + 1;
        if (ret == YO_ID_ROOT) ret = YO_ID_ROOT - 1;
    }

    return ret;
}

YO_API yo_id_t yo_id_from_format(YO_PRINTF_FORMAT_STRING char *format, ...)
{
    va_list args;
    va_start(args, format);
    yo_id_t ret = yo_id_from_format_v(format, args);
    va_end(args);

    return ret;
}

YO_API yo_id_t yo_id_from_format_v(char *format, va_list args)
{
    yo_id_t ret = 0;

    YO_ARENA_TEMP_SCOPE(&yo_ctx->this_frame->arena)
    {
        char *string = yo_alloc_temp_string_v(format, args);
        ret          = yo_id_from_string(string);
    }

    return ret;
}

YO_API void yo_push_id(yo_id_t id)
{
    yo_id_t *pushed = yo_array_add(&yo_ctx->id_stack, 1, false); // TODO(rune): Error handling
    *pushed = id;
}

YO_API yo_id_t yo_pop_id(void)
{
    yo_id_t *popped = yo_array_pop(&yo_ctx->id_stack, 1); // TODO(rune): Error handling
    return *popped;
}

////////////////////////////////////////////////////////////////
//
//
// Box
//
//
////////////////////////////////////////////////////////////////

YO_API yo_box_t *yo_box(yo_id_t id, yo_box_flags_t flags)
{
    yo_box_t *ret = yo_push_box(id, flags);
    return ret;
}

YO_API yo_signal_t yo_get_signal(yo_box_t *box)
{
    yo_signal_t ret = { 0 };

    yo_rectf2_t hit_test_rect = yo_rectf2(yo_v2f(0, 0), yo_v2f(9999, 9999));

    if (box)
    {
        yo_id_t id = box->id;
        YO_ASSERT(id && "Missing id.");

        if (id == yo_ctx->prev_frame->hot_id)    ret.is_hot    = true;
        if (id == yo_ctx->prev_frame->active_id) ret.is_active = true;

        yo_box_t *prev = yo_get_box_by_id_prev_frame(id);
        if (prev) hit_test_rect = prev->screen_rect;
    }

    if (yo_clips_rectf2(hit_test_rect, yo_ctx->this_frame->mouse_pos))
    {
        ret.hovered = true;
    }

    for (uint32_t i = 0; i < yo_ctx->this_frame->events.count; i++)
    {
        yo_event_t *event = &yo_ctx->this_frame->events.elems[i];

        if (yo_clips_rectf2(hit_test_rect, event->pos))
        {
            if (event->type == YO_EVENT_TYPE_MOUSE_CLICK_LEFT)  ret.left_clicked  = true;
            if (event->type == YO_EVENT_TYPE_MOUSE_CLICK_RIGHT) ret.right_clicked = true;
        }

        if (event->type == YO_EVENT_TYPE_KEY_PRESS)
        {
            ret.keycode       = event->keycode;
            ret.modifiers     = event->modifiers;
        }
    }

    yo_v2i_t mouse = yo_query_mouse_pos();
    ret.mouse_pos.x = mouse.x - (int32_t)hit_test_rect.x0; // TODO(rune): Input mouse in floating point?
    ret.mouse_pos.y = mouse.y - (int32_t)hit_test_rect.y0;

    return ret;
}

YO_API yo_rectf_t yo_get_screen_rect(yo_box_t *box)
{
    yo_rectf_t ret = { 0 };

    if (box && box->id)
    {
        yo_box_t *from_prev_frame = yo_get_box_by_id_prev_frame(box->id);
        if (from_prev_frame)
        {
            // TODO(rune): Return in floating point.
            ret.x = from_prev_frame->screen_rect.x0;
            ret.y = from_prev_frame->screen_rect.y0;
            ret.w = from_prev_frame->screen_rect.x1 - ret.x;
            ret.h = from_prev_frame->screen_rect.y1 - ret.y;

        }
    }
    else
    {
        YO_ASSERT(false); // NOTE(rune): Box was missing an id
    }

    return ret;
}

YO_API yo_rectf_t yo_get_arranged_rect(yo_box_t *box)
{
    yo_rectf_t ret = { 0 };

    if (box && box->id)
    {
        yo_box_t *from_prev_frame = yo_get_box_by_id_prev_frame(box->id);
        if (from_prev_frame) ret = from_prev_frame->arranged_rect;
    }
    else
    {
        YO_ASSERT(false); // NOTE(rune): Box was missing an id
    }

    return ret;
}

YO_API yo_v2f_t yo_get_content_dim(yo_box_t *box)
{
    yo_v2f_t ret = { 0 };

    if (box && box->id)
    {
        yo_box_t *from_prev_frame = yo_get_box_by_id_prev_frame(box->id);
        if (from_prev_frame)
        {
            ret.x = from_prev_frame->content_size.x;
            ret.y = from_prev_frame->content_size.y;
        }
    }
    else
    {
        YO_ASSERT(false); // NOTE(rune): Box was missing an id
    }

    return ret;
}

YO_API void  yo_set_tag(char *tag)                                              { yo_ctx->latest_child->tag = tag; }
YO_API void  yo_set_text(char *text)                                            { yo_ctx->latest_child->text = text; }
YO_API void  yo_set_layout(yo_layout_t layout)                                  { yo_ctx->latest_child->child_layout = layout; }
YO_API void  yo_set_fill(yo_v4f_t fill)                                         { yo_ctx->latest_child->fill = fill; }
YO_API void  yo_set_border_s(yo_border_t border)                                { yo_ctx->latest_child->border = border; }
YO_API void  yo_set_border(float thickness, yo_v4f_t color, float radius)       { yo_ctx->latest_child->border = yo_border(thickness, color, radius); }
YO_API void  yo_set_border_thickness(float thickness)                           { yo_ctx->latest_child->border.thickness = thickness; }
YO_API void  yo_set_border_color(yo_v4f_t color)                                { yo_ctx->latest_child->border.color = color; }
YO_API void  yo_set_border_radius(float radius)                                 { yo_ctx->latest_child->border.radius = yo_corners(radius, radius, radius, radius); }
YO_API void  yo_set_font(yo_font_id_t font, uint32_t size, yo_v4f_t color)      { yo_ctx->latest_child->font = font; yo_ctx->latest_child->font_size = size; yo_ctx->latest_child->font_color = color; }
YO_API void  yo_set_font_id(yo_font_id_t font)                                  { yo_ctx->latest_child->font = font; }
YO_API void  yo_set_font_size(uint32_t size)                                    { yo_ctx->latest_child->font_size = size; }
YO_API void  yo_set_font_color(yo_v4f_t color)                                  { yo_ctx->latest_child->font_color = color; }
YO_API void  yo_set_on_top(bool on_top)                                         { yo_ctx->latest_child->on_top = on_top; }
YO_API void  yo_set_overflow_a(yo_overflow_t overflow, yo_axis_t axis)          { yo_ctx->latest_child->overflow_a[axis] = overflow; }
YO_API void  yo_set_overflow_x(yo_overflow_t overflow)                          { yo_ctx->latest_child->overflow_x = overflow; }
YO_API void  yo_set_overflow_y(yo_overflow_t overflow)                          { yo_ctx->latest_child->overflow_y = overflow; }
YO_API void  yo_set_align(yo_align_t align_x, yo_align_t align_y)               { yo_ctx->latest_child->align_x = align_x; yo_ctx->latest_child->align_y = align_y; }
YO_API void  yo_set_align_a(yo_align_t align, yo_axis_t axis)                   { yo_ctx->latest_child->align_a[axis] = align; }
YO_API void  yo_set_align_x(yo_align_t align)                                   { yo_ctx->latest_child->align_x = align; }
YO_API void  yo_set_align_y(yo_align_t align)                                   { yo_ctx->latest_child->align_y = align; }
YO_API void  yo_set_align_x_left()                                              { yo_ctx->latest_child->align_x = YO_ALIGN_LEFT; }
YO_API void  yo_set_align_x_right()                                             { yo_ctx->latest_child->align_x = YO_ALIGN_RIGHT; }
YO_API void  yo_set_align_x_center()                                            { yo_ctx->latest_child->align_x = YO_ALIGN_CENTER; }
YO_API void  yo_set_align_y_top()                                               { yo_ctx->latest_child->align_y = YO_ALIGN_TOP; }
YO_API void  yo_set_align_y_bottom()                                            { yo_ctx->latest_child->align_y = YO_ALIGN_BOTTOM; }
YO_API void  yo_set_align_y_center()                                            { yo_ctx->latest_child->align_y = YO_ALIGN_CENTER; }
YO_API void  yo_set_align_center()                                              { yo_ctx->latest_child->align_x = YO_ALIGN_CENTER; yo_ctx->latest_child->align_y = YO_ALIGN_CENTER; }
YO_API void  yo_set_dim(yo_length_t dim_x, yo_length_t dim_y)                   { yo_ctx->latest_child->dim_x = dim_x;  yo_ctx->latest_child->dim_y = dim_y; }
YO_API void  yo_set_dim_a(yo_length_t dim, yo_axis_t axis)                      { yo_ctx->latest_child->dim_a[axis] = dim; }
YO_API void  yo_set_dim_x(yo_length_t dim)                                      { yo_ctx->latest_child->dim_x = dim; }
YO_API void  yo_set_dim_y(yo_length_t dim)                                      { yo_ctx->latest_child->dim_y = dim; }
YO_API void  yo_set_margin(float left, float top, float right, float bottom)    { yo_ctx->latest_child->margin = yo_margin(left, top, right, bottom); }
YO_API void  yo_set_margin_xy(float x, float y)                                 { yo_ctx->latest_child->margin = yo_margin_xy(x, y); }
YO_API void  yo_set_margin_a(float forward, float backward, yo_axis_t axis)     { yo_ctx->latest_child->margin.forward[axis] = forward; yo_ctx->latest_child->margin.backward[axis] = backward; }
YO_API void  yo_set_margin_left(float left)                                     { yo_ctx->latest_child->margin.left = left; }
YO_API void  yo_set_margin_top(float top)                                       { yo_ctx->latest_child->margin.top = top; }
YO_API void  yo_set_margin_right(float right)                                   { yo_ctx->latest_child->margin.right = right; }
YO_API void  yo_set_margin_bottom(float bottom)                                 { yo_ctx->latest_child->margin.bottom = bottom; }
YO_API void  yo_set_padding(float left, float top, float right, float bottom)   { yo_ctx->latest_child->padding = yo_padding(left, top, right, bottom); }
YO_API void  yo_set_padding_xy(float x, float y)                                { yo_ctx->latest_child->padding = yo_padding_xy(x, y); }
YO_API void  yo_set_anim(yo_anim_flags_t anim, float rate)                      { yo_ctx->latest_child->anim = anim; yo_ctx->latest_child->anim_rate = rate; }
YO_API void  yo_set_anim_flags(yo_anim_flags_t anim)                            { yo_ctx->latest_child->anim = anim; }
YO_API void  yo_set_anim_rate(float rate)                                       { yo_ctx->latest_child->anim_rate = rate; }
YO_API void  yo_set_scroll(yo_v2f_t offset)                                     { yo_ctx->latest_child->scroll_offset = offset; }
YO_API void  yo_set_scroll_a(float offset, yo_axis_t axis)                      { yo_ctx->latest_child->scroll_offset.v[axis] = offset; }
YO_API void  yo_set_scroll_x(float offset)                                      { yo_ctx->latest_child->scroll_offset.x = offset; }
YO_API void  yo_set_scroll_y(float offset)                                      { yo_ctx->latest_child->scroll_offset.y = offset; }

YO_API yo_text_field_state_t *yo_get_text_field_state(void)                     { return &yo_ctx->latest_child->text_field_state; }

YO_API void *yo_get_userdata(size_t size)
{
    yo_box_t *box = yo_ctx->latest_child;

    if (!box->userdata)
    {
        box->userdata = yo_arena_push_size(&yo_ctx->this_frame->arena, size, true);
        if (box->userdata)
        {
            box->userdata_size = size;
        }
    }
    else
    {
        YO_ASSERT(box->userdata_size == size);

    }

    void *ret = box->userdata;
    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Error
//
//
////////////////////////////////////////////////////////////////

YO_API yo_error_t yo_get_error(void)
{
    return yo_ctx->error;
}

YO_API char *yo_get_error_message(void)
{
    char *message = NULL;

    if (yo_ctx->error >= 0 && yo_ctx->error < YO_ERROR_COUNT)
    {
        message = error_message_table[yo_ctx->error];
    }

    return message;
}

////////////////////////////////////////////////////////////////
//
//
// Memory
//
//
////////////////////////////////////////////////////////////////

YO_API void *yo_alloc_temp(size_t size)
{
    void *ret = yo_arena_push_size(&yo_ctx->this_frame->arena, size, false);

    if (!ret)
    {
        yo_set_error(YO_ERROR_OUT_OF_TEMPORARY_MEMORY);
    }

    return ret;
}

YO_API char *yo_alloc_temp_string(YO_PRINTF_FORMAT_STRING char *format, ...)
{
    va_list args;
    va_start(args, format);
    char *ret = yo_alloc_temp_string_v(format, args);
    va_end(args);

    return ret;
}

YO_API char *yo_alloc_temp_string_v(YO_PRINTF_FORMAT_STRING char *format, va_list args)
{
    // TODO(rune): Custom printf functions

    size_t size  = vsnprintf(NULL, 0, format, args) + 1;
    char *ret = yo_alloc_temp(size);

    if (ret)
    {
        vsnprintf(ret, size, format, args);
    }
    else
    {
        ret = "";
    }

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Scaled
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_scaled_triangle(yo_v2f_t p0, yo_v2f_t p1, yo_v2f_t p2,
                               yo_v4f_t color0, yo_v4f_t color1, yo_v4f_t color2)
{
    if (yo_ctx->latest_child)
    {
        yo_scaled_element_t *new_element = yo_arena_push_struct(&yo_ctx->this_frame->arena, yo_scaled_element_t, false);
        if (new_element)
        {
            new_element->type            = YO_SCALED_ELEMENT_TYPE_TRIANGLE;
            new_element->triangle.p0     = p0;
            new_element->triangle.p1     = p1;
            new_element->triangle.p2     = p2;
            new_element->triangle.color0 = color0;
            new_element->triangle.color1 = color1;
            new_element->triangle.color2 = color2;

            YO_SLSTACK_PUSH(yo_ctx->latest_child->scaled_elements, next, new_element);
        }
        else
        {
            yo_set_error(YO_ERROR_OUT_OF_TEMPORARY_MEMORY);
        }
    }
}

YO_API void yo_scaled_checkmark(yo_v4f_t color)
{
    yo_scaled_triangle(yo_v2f(0.16f, 0.44f), yo_v2f(0.35f, 0.62f), yo_v2f(0.03f, 0.60f), color, color, color);
    yo_scaled_triangle(yo_v2f(0.35f, 0.90f), yo_v2f(0.35f, 0.62f), yo_v2f(0.03f, 0.60f), color, color, color);
    yo_scaled_triangle(yo_v2f(0.35f, 0.90f), yo_v2f(0.35f, 0.62f), yo_v2f(0.97f, 0.16f), color, color, color);
    yo_scaled_triangle(yo_v2f(0.82f, 0.03f), yo_v2f(0.35f, 0.62f), yo_v2f(0.97f, 0.16f), color, color, color);
}

////////////////////////////////////////////////////////////////
//
//
// Debug
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_debug_show_atlas_partitions(void)
{
    yo_atlas_t *atlas = &yo_ctx->atlas;

    yo_box(0, 0);
    yo_set_dim_x(yo_px((float)atlas->dim.x));
    yo_set_dim_y(yo_px((float)atlas->dim.y));
    yo_set_fill(yo_rgb(30, 30, 30));
    yo_begin_children();

    for (yo_dlist_each(yo_atlas_shelf_t *, shelf, &atlas->shelf_list))
    {
        yo_box(0, 0);
        yo_set_align_y(YO_ALIGN_TOP);
        yo_set_dim_y(yo_px((float)shelf->dim_y));
        yo_set_margin_top((float)shelf->base_y);
        yo_set_fill(shelf->used_x ? yo_rgb(250, 230, 150) : yo_rgb(250, 200, 100));
        yo_set_border_color(YO_BLACK);
        yo_set_border_thickness(1);

        for (yo_dlist_each(yo_atlas_node_t *, node, &shelf->node_list))
        {
            yo_box(0, 0);
            yo_set_align_x(YO_ALIGN_LEFT);
            yo_set_align_y(YO_ALIGN_TOP);
            yo_set_dim_x(yo_px((float)yo_recti_width(node->rect)));
            yo_set_dim_y(yo_px((float)yo_recti_height(node->rect)));
            yo_set_margin_left((float)node->rect.x);
            yo_set_margin_top((float)node->rect.y);
            yo_set_fill(yo_rgb(50, 50, 150));
            yo_set_border_color(YO_BLACK);
            yo_set_border_thickness(1);
        }
    }

    yo_end_children();

#if 0
    yo_svg_doc_t doc;
    yo_svg_doc_create(&doc);
    yo_svg_begin(&doc, atlas->dim);

    yo_svg_set_fill(&doc, yo_rgb32(200, 200, 200));
    yo_svg_set_stroke(&doc, 0, 0);
    yo_svg_draw_rectangle(&doc, yo_v2i(0, 0), atlas->dim);

    for (yo_dlist_each(yo_shelf_t *, shelf, &atlas->shelves))
    {
        for (yo_dlist_each(yo_atlas_node_t *, node, &shelf->nodes))
        {
            yo_svg_set_fill(&doc, yo_rgb32(50, 50, 50));
            yo_svg_set_stroke(&doc, yo_rgb32(0, 0, 0), 1);
            yo_svg_draw_rectangle(&doc, yo_v2i(node->rect.x, node->rect.y), yo_v2i(node->rect.w, node->rect.h));
        }
    }

    yo_end_children();

    yo_svg_end(&doc);
    yo_svg_write_file(&doc, "C:\\Users\\runel\\source\\repos\\yo\\svg\\temp.svg");
    yo_svg_doc_destroy(&doc);
#endif
}

YO_API void yo_debug_show_atlas_texture()
{
    yo_box(0, YO_BOX_FULL_ATLAS);
    yo_set_dim_x(yo_px((float)yo_ctx->atlas.dim.x));
    yo_set_dim_y(yo_px((float)yo_ctx->atlas.dim.y));
    yo_set_fill(yo_rgb(255, 255, 255));
}

// DEBUG(rune):
static void yo_debug_show_atlas_partitions_of(yo_atlas_t *atlas)
{
    float scale = 4.0f;

    yo_box(0, 0);
    yo_set_dim_x(yo_px((float)atlas->dim.x * scale));
    yo_set_dim_y(yo_px((float)atlas->dim.y * scale));
    yo_set_fill(yo_rgb(30, 30, 30));
    yo_begin_children();

    for (yo_dlist_each(yo_atlas_shelf_t *, shelf, &atlas->shelf_list))
    {
        yo_box(0, 0);
        yo_set_align_y(YO_ALIGN_TOP);
        yo_set_dim_y(yo_px((float)shelf->dim_y * scale));
        yo_set_dim_x(yo_px((float)atlas->dim.x * scale));
        yo_set_margin_top((float)shelf->base_y * scale);
        yo_set_fill(yo_rgb(shelf->last_accessed_generation, 0, 0));
        yo_set_border_color(YO_CYAN);
        yo_set_border_thickness(1);

        for (yo_dlist_each(yo_atlas_node_t *, node, &shelf->node_list))
        {
            yo_box(0, 0);
            yo_set_align_x(YO_ALIGN_LEFT);
            yo_set_align_y(YO_ALIGN_TOP);
            yo_set_dim_x(yo_px((float)yo_recti_width(node->rect) * scale));
            yo_set_dim_y(yo_px((float)yo_recti_height(node->rect) * scale));
            yo_set_margin_left((float)node->rect.x * scale);
            yo_set_margin_top((float)node->rect.y * scale);
            yo_set_fill(yo_rgb(0, node->last_accessed_generation, 0));
            yo_set_border_color(YO_CYAN);
            yo_set_border_thickness(1);
        }
    }

    yo_end_children();

#if 0
    yo_svg_doc_t doc;
    yo_svg_doc_create(&doc);
    yo_svg_begin(&doc, atlas->dim);

    yo_svg_set_fill(&doc, yo_rgb32(200, 200, 200));
    yo_svg_set_stroke(&doc, 0, 0);
    yo_svg_draw_rectangle(&doc, yo_v2i(0, 0), atlas->dim);

    for (yo_dlist_each(yo_shelf_t *, shelf, &atlas->shelves))
    {
        for (yo_dlist_each(yo_atlas_node_t *, node, &shelf->nodes))
        {
            yo_svg_set_fill(&doc, yo_rgb32(50, 50, 50));
            yo_svg_set_stroke(&doc, yo_rgb32(0, 0, 0), 1);
            yo_svg_draw_rectangle(&doc, yo_v2i(node->rect.x, node->rect.y), yo_v2i(node->rect.w, node->rect.h));
        }
    }

    yo_end_children();

    yo_svg_end(&doc);
    yo_svg_write_file(&doc, "C:\\Users\\runel\\source\\repos\\yo\\svg\\temp.svg");
    yo_svg_doc_destroy(&doc);
#endif
}
