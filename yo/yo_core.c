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
    yo_font_slot_t *slot = yo_font_table_slot_get(font);
    if (slot)
    {
        ret = yo_font_backend_get_font_metrics(&yo_ctx->font_backend, &slot->backend_info, font_size);
    }
    return ret;
}

static uint64_t yo_glyph_key(yo_font_id_t font, uint32_t codepoint, uint16_t font_size)
{
    uint64_t key = (((uint64_t)(codepoint) << 0)  |
                    ((uint64_t)(font_size) << 32) |
                    ((uint64_t)(font.slot) << 48));
    return key;
}

static yo_atlas_node_t *yo_glyph_get(yo_font_id_t font, yo_atlas_t *atlas, uint32_t codepoint, uint32_t font_size, bool rasterize)
{
    yo_atlas_node_t *ret = NULL;
    yo_font_slot_t *slot = yo_font_table_slot_get(font);

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

                yo_font_backend_rasterize(&yo_ctx->font_backend, &slot->backend_info,
                                          codepoint, font_size, pixel, yo_v2i(ret->rect.w, ret->rect.h), stride);

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
                ret = slot->id;
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
    yo_font_slot_t *slot = yo_font_table_slot_get(font);
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
    // DEBUG(rune):
    if (id == 5696385795564432402)
    {
        __nop();
    }

    if (yo_ctx->error)
    {
        yo_zero_struct(&stub_box);
        return &stub_box;
    }

    yo_box_t *box = yo_alloc_box(id);
    if (box == NULL)
    {
        yo_set_error(YO_ERROR_OUT_OF_PERSITENT_MEMORY);
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
    box->dim_h.min  = 0.0f;
    box->dim_v.min  = 0.0f;
    box->dim_h.max  = YO_FLOAT32_MAX;
    box->dim_v.max  = YO_FLOAT32_MAX;
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

static yo_measure_text_result_t yo_measure_text(yo_string_t text, yo_font_id_t font, uint32_t font_size)
{
    yo_measure_text_result_t ret = { 0 };
    yo_font_metrics_t font_metrics = yo_font_metrics(font, font_size);

    for (uint32_t codepoint = yo_utf8_codepoint_advance(&text);
         codepoint;
         codepoint = yo_utf8_codepoint_advance(&text))
    {
        yo_atlas_node_t *glyph = yo_glyph_get(font, &yo_ctx->atlas, codepoint, font_size, false);
        if (glyph)
        {
            ret.dim.x += glyph->advance_x;
            ret.dim.y = font_metrics.line_gap;

            ret.ascent   = (int32_t)font_metrics.ascent;
            ret.descent  = (int32_t)font_metrics.descent;
            ret.line_gap = (int32_t)font_metrics.line_gap;
        }
        else
        {
            // NOTE(rune): Not enough space on glyph atlas
            __nop();
        }
    }

    return ret;
}

static void yo_measure_content_recurse(yo_box_t *box)
{
    yo_anim_box(box);

    if (box->text)
    {
        box->measured_text = yo_measure_text(yo_from_cstring(box->text), box->font, box->font_size);
        box->content_size.x = box->measured_text.dim.x + (uint32_t)(box->border.thickness * 2);
        box->content_size.y = box->measured_text.dim.y + (uint32_t)(box->border.thickness * 2);
    }
    else
    {
        box->content_size.x = 0;
        box->content_size.y = 0;
    }

    for (yo_box_t *child = box->children.first;
         child != NULL;
         child = child->next)
    {
        yo_measure_content_recurse(child);

        // TODO(rune): Think of a better way that allows for flexbox layouts

        switch (box->child_layout)
        {
#if 1
            case YO_LAYTOUT_NONE:
                if (box->overflow_h == YO_OVERFLOW_FIT || box->overflow_h == YO_OVERFLOW_SCROLL)
                {
                    box->content_size.x = YO_MAX(box->content_size.x, child->content_size.x);
                }

                if (box->overflow_v == YO_OVERFLOW_FIT || box->overflow_v == YO_OVERFLOW_SCROLL)
                {
                    box->content_size.y = YO_MAX(box->content_size.y, child->content_size.y);
                }
                break;

            case YO_LAYOUT_HORIZONTAL:
                if (box->overflow_h == YO_OVERFLOW_FIT || box->overflow_h == YO_OVERFLOW_SCROLL)
                {
                    box->content_size.x += child->content_size.x;
                }

                if (child->overflow_v == YO_OVERFLOW_FIT || child->overflow_v == YO_OVERFLOW_SCROLL)
                {
                    box->content_size.y = YO_MAX(box->content_size.y, child->content_size.y);
                }
                break;

            case YO_LAYOUT_VERTICAL:
                if (child->overflow_h == YO_OVERFLOW_FIT || child->overflow_h == YO_OVERFLOW_SCROLL)
                {
                    box->content_size.x = YO_MAX(box->content_size.x, child->content_size.x);
                }

                if (box->overflow_v == YO_OVERFLOW_FIT || box->overflow_v == YO_OVERFLOW_SCROLL)
                {
                    box->content_size.y += child->content_size.y;
                }
                break;

#else
            case YO_LAYTOUT_NONE:
            case YO_LAYOUT_HORIZONTAL:
            case YO_LAYOUT_VERTICAL:

                bool stack_on_axis[2] = { false, false };
                if (box->child_layout == YO_LAYOUT_HORIZONTAL) stack_on_axis[YO_AXIS_X] = true;
                if (box->child_layout == YO_LAYOUT_VERTICAL)   stack_on_axis[YO_AXIS_Y] = true;

                for (yo_axis_t i = YO_AXIS_X; i <= YO_AXIS_Y; i++)
                {
                    if ((box->overflow[i] == YO_OVERFLOW_FIT) ||
                        (box->overflow[i] == YO_OVERFLOW_SCROLL))
                    {
                        if (stack_on_axis[i])
                        {
                            box->content_size.axis[i] += child->content_size.w;
                        }
                        else
                        {
                            box->content_size.axis[i] = YO_MAX(box->content_size.axis[i],
                                                               child->content_size.axis[i]);
                        }
                    }
                }

                break;
#endif
            default:
                YO_ASSERT(false);
                break;
        }
    }

    if (yo_cstring_equal(box->tag, "scrollcon"))
    {
        __nop();
    }

    // TODO(rune): Think of a better way that allows for flexbox layouts
    // TODO(rune): Cleanup, maybe a yo_shrink_rect function

    box->content_size.axis[YO_AXIS_X] += box->padding.axis[YO_AXIS_X].forward;
    box->content_size.axis[YO_AXIS_X] += box->padding.axis[YO_AXIS_X].backward;
    box->content_size.axis[YO_AXIS_Y] += box->padding.axis[YO_AXIS_Y].forward;
    box->content_size.axis[YO_AXIS_Y] += box->padding.axis[YO_AXIS_Y].backward;

    box->content_size.axis[YO_AXIS_X] = YO_MAX(box->content_size.axis[YO_AXIS_X], box->dim_a[YO_AXIS_X].min);
    box->content_size.axis[YO_AXIS_Y] = YO_MAX(box->content_size.axis[YO_AXIS_Y], box->dim_a[YO_AXIS_Y].min);

    box->content_size.axis[YO_AXIS_X] += box->margin.axis[YO_AXIS_X].forward;
    box->content_size.axis[YO_AXIS_X] += box->margin.axis[YO_AXIS_X].backward;
    box->content_size.axis[YO_AXIS_Y] += box->margin.axis[YO_AXIS_Y].forward;
    box->content_size.axis[YO_AXIS_Y] += box->margin.axis[YO_AXIS_Y].backward;

#if 1
    // TODO(rune): This feels very hacky, just need to rewrite measure/arrange...

    for (yo_axis_t i = YO_AXIS_X; i <= YO_AXIS_Y; i++)
    {
        if ((box->overflow_a[i] != YO_OVERFLOW_CLIP) &&
            (box->overflow_a[i] != YO_OVERFLOW_SCROLL))
        {
            box->dim_a[i].min = YO_MAX(box->dim_a[i].min, box->content_size.axis[i]);
        }
    }
#endif
}

////////////////////////////////////////////////////////////////
//
//
// Arrange
//
//
////////////////////////////////////////////////////////////////

static void yo_arrange_children_in_stack(yo_box_t *parent, yo_v2f_t avail_dim, yo_axis_t stack_axis)
{
    if (yo_cstring_equal(parent->tag, "YO_OF1") && parent->children.first->next)
    {
        __nop();
    }

    // TODO(rune): Think about possible non O(n^2) solution for stack layout
    float abs_sum = 0;
    float rel_sum = 0;

    for (yo_box_t *child = parent->children.first;
         child != NULL;
         child = child->next)
    {
        if (child->dim_a[stack_axis].is_rel && (!child->arranged))
        {
            rel_sum += child->dim_a[stack_axis].rel;
        }
        else
        {
            abs_sum += child->content_size.axis[stack_axis];
        }
    }

try_again:
    float relAvailable = abs_sum < avail_dim.v[stack_axis] ? avail_dim.v[stack_axis] - abs_sum : 0;
    float relUsed = 0;

    float stackPos = 0;

    for (yo_box_t *child = parent->children.first;
         child != NULL;
         child = child->next)
    {
        child->arranged_rect.pos[stack_axis] = stackPos;

        if (child->dim_a[stack_axis].is_rel)
        {
            if (!child->arranged)
            {
                if (relAvailable > 0)
                {
                    float computed = (child->dim_a[stack_axis].rel * (relAvailable / rel_sum));
                    float computed_clamp = YO_CLAMP(computed, child->dim_a[stack_axis].min, child->dim_a[stack_axis].max);

                    if (computed != computed_clamp)
                    {
                        rel_sum -= child->dim_a[stack_axis].rel;
                        abs_sum += computed_clamp;

                        child->arranged_rect.dim_a[stack_axis] = computed_clamp;

                        // NOTE(rune): Mark as arranged with computed_lamp so on the next try, we know that
                        // this child has been arranged with an absolute size, even though is_rel
                        // is true.
                        child->arranged = true;

                        goto try_again;
                    }
                    else
                    {
                        relUsed++;

                        child->arranged_rect.dim_a[stack_axis] = computed_clamp;
                        child->arranged = true;
                    }
                }
                else
                {
                    child->arranged_rect.dim_a[stack_axis] = child->dim_a[stack_axis].min;
                }
            }
            else
            {
                // Child was already arranged with computed_clamp
            }
        }
        else
        {
            child->arranged_rect.dim_a[stack_axis] = YO_CLAMP((child->content_size.axis[stack_axis]),
                                                              (child->dim_a[stack_axis].min + child->margin.axis[stack_axis].forward + child->margin.axis[stack_axis].backward),
                                                              (child->dim_a[stack_axis].max + child->margin.axis[stack_axis].forward + child->margin.axis[stack_axis].backward));
        }

        stackPos += child->arranged_rect.dim_a[stack_axis];
    }
}

// TODO(rune): Better name than than "Simple" layout
static void yo_arrange_children_simple(yo_box_t *parent, yo_v2f_t avail_dim, yo_axis_t axis)
{
    for (yo_box_t *child = parent->children.first;
         child != NULL;
         child = child->next)
    {
        float parent_pos    = 0;
        float parent_length = avail_dim.v[axis];

        float available_length = parent_length;

        float child_length;
        if (child->dim_a[axis].is_rel)
        {
            child_length = available_length * child->dim_a[axis].rel;
            child_length = YO_CLAMP(child_length, child->dim_a[axis].min, child->dim_a[axis].max);
        }
        else
        {
            if (child->align_a[axis] == YO_ALIGN_STRETCH)
            {
                child_length = child->dim_a[axis].max;
            }
            else
            {
                child_length = child->dim_a[axis].min;
            }
        }

        child_length = YO_CLAMP_HIGH(child_length, available_length);

        child->arranged_rect.dim_a[axis] = child_length;

        switch (child->align_a[axis])
        {
            case YO_ALIGN_STRETCH:
            case YO_ALIGN_CENTER:
            {
                float parent_center = parent_pos + (parent_length / 2);
                child->arranged_rect.pos[axis] = parent_center - (child_length / 2);
            }
            break;

            case YO_ALIGN_FRONT:
            {
                child->arranged_rect.pos[axis] = parent_pos;
                child->arranged_rect.dim_a[axis] = child_length;
            }
            break;

            case YO_ALIGN_BACK:
            {
                child->arranged_rect.pos[axis] = parent_pos + parent_length - child_length;
                child->arranged_rect.dim_a[axis] = child_length;
            }
            break;

            default:
            {
                YO_ASSERT(false);
            }
            break;
        }
    }
}

static void yo_arrange_children_recurse(yo_box_t *parent)
{
    yo_v2f_t avail_dim = yo_v2f(parent->arranged_rect.w, parent->arranged_rect.h);
    avail_dim.x -= parent->margin.axis[YO_AXIS_X].forward;
    avail_dim.x -= parent->margin.axis[YO_AXIS_X].backward;
    avail_dim.x -= parent->padding.axis[YO_AXIS_X].forward;
    avail_dim.x -= parent->padding.axis[YO_AXIS_X].backward;
    avail_dim.x -= parent->border.thickness;
    avail_dim.x -= parent->border.thickness;

    avail_dim.y -= parent->margin.axis[YO_AXIS_Y].forward;
    avail_dim.y -= parent->margin.axis[YO_AXIS_Y].backward;
    avail_dim.y -= parent->padding.axis[YO_AXIS_Y].forward;
    avail_dim.y -= parent->padding.axis[YO_AXIS_Y].backward;
    avail_dim.y -= parent->border.thickness;
    avail_dim.y -= parent->border.thickness;

    if (parent->overflow_h == YO_OVERFLOW_SPILL || parent->overflow_h == YO_OVERFLOW_SCROLL) avail_dim.x = parent->content_size.x;
    if (parent->overflow_v == YO_OVERFLOW_SPILL || parent->overflow_v == YO_OVERFLOW_SCROLL) avail_dim.y = parent->content_size.y;

    if (parent->children.first)
    {
        switch (parent->child_layout)
        {
            case YO_LAYTOUT_NONE:
            {
                yo_arrange_children_simple(parent, avail_dim, YO_AXIS_X);
                yo_arrange_children_simple(parent, avail_dim, YO_AXIS_Y);
            }
            break;

            case YO_LAYOUT_HORIZONTAL:
            {
                yo_arrange_children_in_stack(parent, avail_dim, YO_AXIS_X);
                yo_arrange_children_simple(parent, avail_dim, YO_AXIS_Y);
            }
            break;

            case YO_LAYOUT_VERTICAL:
            {
                yo_arrange_children_simple(parent, avail_dim, YO_AXIS_X);
                yo_arrange_children_in_stack(parent, avail_dim, YO_AXIS_Y);
            }
            break;

            default:
            {
                YO_ASSERT(false);
            }
            break;
        }

        for (yo_box_t *child = parent->children.first;
             child != NULL;
             child = child->next)
        {
            child->screen_rect =  yo_rectf(child->arranged_rect.x - (parent->scroll_offset.x) + parent->screen_rect.x + parent->border.thickness + parent->margin.axis[YO_AXIS_X].forward + parent->padding.axis[YO_AXIS_X].forward,
                                           child->arranged_rect.y - (parent->scroll_offset.y) + parent->screen_rect.y + parent->border.thickness + parent->margin.axis[YO_AXIS_Y].forward + parent->padding.axis[YO_AXIS_Y].forward,
                                           child->arranged_rect.w,
                                           child->arranged_rect.h);
            yo_arrange_children_recurse(child);
        }
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

static void yo_draw_text(yo_string_t text,
                         yo_v2f_t p0, yo_v2f_t p1,
                         yo_v2f_t clip_p0, yo_v2f_t clip_p1,
                         yo_font_id_t font, uint32_t font_size, yo_v4f_t font_color,
                         yo_measure_text_result_t measured_text,
                         yo_text_field_state_t *text_field_state)
{
    YO_UNUSED(p1);
    YO_UNUSED(measured_text);

    uint32_t index = 0;

    uint32_t highlight_begin = 0;
    uint32_t highlight_end   = 0;
    float highlight_x0       = 0;
    float highlight_x1       = 0;
    float cursor_x           = 0;

    if (text_field_state)
    {
        highlight_begin = YO_MIN(text_field_state->cursor, text_field_state->marker);
        highlight_end   = YO_MAX(text_field_state->cursor, text_field_state->marker);
    }

    float baseline_y = p1.y + measured_text.descent;

    for (uint32_t codepoint = yo_utf8_codepoint_advance(&text);
         codepoint;
         codepoint = yo_utf8_codepoint_advance(&text))
    {
        yo_atlas_node_t *glyph = yo_glyph_get(font, &yo_ctx->atlas, codepoint, font_size, true);
        if (glyph)
        {
            yo_v2f_t glyph_p0 = yo_v2f((p0.x + glyph->bearing_x),
                                       (baseline_y + glyph->bearing_y));

            yo_v2f_t glyph_p1 = yo_v2f((p0.x + glyph->bearing_x + glyph->rect.w),
                                       (baseline_y + glyph->bearing_y + glyph->rect.h));

#if 0
            if ((pGlyph1.x >= p1.x) || (pGlyph1.y >= p1.y))
            {
                break;
            }
#endif
            if (text_field_state)
            {
                if (index == highlight_begin)          highlight_x0 = p0.x;
                if (index == highlight_end - 1)        highlight_x1 = p0.x + glyph->advance_x;
                if (index == text_field_state->cursor) cursor_x     = p0.x;
            }

            yo_v2f_t uv0;
            yo_v2f_t uv1;
            yo_atlas_node_uv(&yo_ctx->atlas, glyph, &uv0, &uv1);

            yo_draw_aabb_t draw =
            {
                .p0         = glyph_p0,
                .p1         = glyph_p1,
                .clip_p0    = clip_p0,
                .clip_p1    = clip_p1,
                .color      = { font_color, font_color, font_color, font_color },
                .texture_id = 42, // TODO(rune): Hardcoded texture id
                .uv0        = uv0,
                .uv1        = uv1,
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

            p0.x += glyph->advance_x;
        }
        else
        {
            // NOTE(rune): Not enough space on glyph atlas
            __nop();
        }

        index++;
    }

    if (text_field_state)
    {
        if (text_field_state->cursor == text.length)    cursor_x = p0.x;

        //
        // Draw highlight for selected text
        //
        if (highlight_begin != highlight_end)
        {
            yo_v2f_t highlight_p0 = yo_v2f(highlight_x0, baseline_y - measured_text.ascent);
            yo_v2f_t highlight_p1 = yo_v2f(highlight_x1, baseline_y - measured_text.descent);
            yo_v4f_t highlight_color = yo_rgba(0x88, 0x88, 0xff, 0x80);
            yo_draw_aabb_t draw =
            {
                .p0      = highlight_p0,
                .p1      = highlight_p1,
                .clip_p0 = highlight_p0,
                .clip_p1 = highlight_p1,
                .color   = { highlight_color, highlight_color, highlight_color, highlight_color },
            };
            yo_draw_aabb(draw);
        }

        //
        // Draw cursor
        //
#if 1
        if (cursor_x)
        {
            yo_v2f_t cursor_p0 = yo_v2f(cursor_x + 0, baseline_y - measured_text.ascent);
            yo_v2f_t cursor_p1 = yo_v2f(cursor_x + 1, baseline_y - measured_text.descent);
            yo_v4f_t cursor_color = yo_rgba(0xff, 0xff, 0xff, 0xff);
            yo_draw_aabb_t draw =
            {
                .p0      = cursor_p0,
                .p1      = cursor_p1,
                .clip_p0 = cursor_p0,
                .clip_p1 = cursor_p1,
                .color = { cursor_color, cursor_color, cursor_color, cursor_color }
            };

            yo_draw_aabb(draw);
        }
#endif
    }
}

static void yo_render_recurse(yo_box_t *box, yo_render_info_t *render_info, bool on_top)
{
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

    yo_rectf_t rect = box->screen_rect;

    yo_v2f_t p0 = yo_v2f(rect.x, rect.y);
    yo_v2f_t p1 = yo_v2f(rect.x + rect.w, rect.y + rect.h);

    bool clip_to_parent_x = (box->parent) && (box->parent->overflow_h != YO_OVERFLOW_SPILL) && (box->parent->overflow_h != YO_OVERFLOW_SCROLL);
    bool clip_to_parent_y = (box->parent) && (box->parent->overflow_h != YO_OVERFLOW_SPILL) && (box->parent->overflow_h != YO_OVERFLOW_SCROLL);

    yo_v2f_t clip_p0 = p0;
    yo_v2f_t clip_p1 = p1;

    if (clip_to_parent_x)
    {
        clip_p0.x = box->parent->screen_rect.x;
        clip_p1.x = box->parent->screen_rect.x + box->parent->screen_rect.w;
    }

    if (clip_to_parent_y)
    {
        clip_p0.y = box->parent->screen_rect.y;
        clip_p1.y = box->parent->screen_rect.y + box->parent->screen_rect.h;
    }

    if (box->on_top == on_top && box->id != YO_ID_ROOT)
    {

        p0.x += box->margin.axis[YO_AXIS_X].forward;
        p0.y += box->margin.axis[YO_AXIS_Y].forward;

        p1.x -= box->margin.axis[YO_AXIS_X].backward;
        p1.y -= box->margin.axis[YO_AXIS_Y].backward;

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

        p0.axis[YO_AXIS_X] += box->padding.axis[YO_AXIS_X].forward;
        p0.axis[YO_AXIS_Y] += box->padding.axis[YO_AXIS_Y].forward;

        p1.axis[YO_AXIS_X] -= box->padding.axis[YO_AXIS_X].backward;
        p1.axis[YO_AXIS_Y] -= box->padding.axis[YO_AXIS_Y].backward;

        p0.axis[YO_AXIS_X] += (int32_t)(box->border.thickness);
        p0.axis[YO_AXIS_Y] += (int32_t)(box->border.thickness);
        p1.axis[YO_AXIS_X] -= (int32_t)(box->border.thickness);
        p1.axis[YO_AXIS_Y] -= (int32_t)(box->border.thickness);

        //
        // Draw content
        //

        if (box->text)
        {
            yo_text_field_state_t *text_field_state = NULL;
            if ((box->id == yo_ctx->this_frame->active_id) && (box->flags & YO_BOX_DRAW_TEXT_CURSOR))
            {
                text_field_state = &box->text_field_state;
            }

            yo_draw_text(yo_from_cstring(box->text),
                         p0, p1, clip_p0, clip_p1,
                         box->font,
                         box->font_size,
                         box->font_color,
                         box->measured_text,
                         text_field_state);
        }

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
}

////////////////////////////////////////////////////////////////
//
//
// Debug
//
//
////////////////////////////////////////////////////////////////

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
    WriteFile(out, yo_g_print_buffer.chars, yo_g_print_buffer.pos, &bytesWritten, NULL);
#endif

    OutputDebugStringA(yo_global_print_buffer.chars);
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

    yo_debug_print("Box:");

    for (uint32_t i = 0; i < 20 - depth; i++)
    {
        yo_debug_print("  ");
    }

    yo_debug_print("(%4i x %-4i)       (%4i , %-4i) (%4i x %-4i)       %s\n",
                   box->content_size.x,
                   box->content_size.y,
                   box->arranged_rect.x,
                   box->arranged_rect.y,
                   box->arranged_rect.w,
                   box->arranged_rect.h,
                   YO_COALESCE(YO_COALESCE(box->text, box->tag), "no tag"));

    uint32_t i = 0;
    yo_box_t *child = box->children.first;
    while (child)
    {
        yo_debug_print_hierarchy(child, depth + 1);
        child = child->next;
        i++;
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
    yo_debug_print(ANSI_FG_BLUE
                   "Memory usage             | "
                   "Vertex buffer:   %-12llu | "
                   "Index buffer:    %-12llu | "
                   "Persist arena*:  %-12llu | "
                   "Frame arena:     %-12llu | "
                   "Font arena:      %-12llu |\n"
                   ANSI_RESET,
                   yo_array_size(&yo_global_context->storage.vertex_array),
                   yo_array_size(&yo_global_context->storage.index_array),
                   yo_global_context->storage.persistent.size_used - sizeof(yo_context_t),
                   yo_global_context->storage.transient.size_used,
                   yo_global_context->atlas.storage->size_used);
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
    yo_debug_print(ANSI_FG_BLUE
                   "Graphics                 | "
                   "Vertex count:    %-12llu | "
                   "Index count:     %-12llu |\n"
                   ANSI_RESET,
                   yo_global_context->storage.vertex_array.count,
                   yo_global_context->storage.index_array.count);

    yo_debug_print(ANSI_FG_CYAN
                   "Timing (last frame)      | "
                   "Build:           %-12.4f | "
                   "Measure:         %-12.4f | "
                   "Arrange:         %-12.4f | "
                   "Render:          %-12.4f | "
                   "Prune:           %-12.4f |  \n"
                   ANSI_RESET,
                   frameTimings.build.millis,
                   frameTimings.measure.millis,
                   frameTimings.arrange.millis,
                   frameTimings.render.millis,
                   frameTimings.prune.millis);

    yo_debug_print(ANSI_FG_YELLOW
                   "Timing (rolling max 100) | "
                   "Build:           %-12.4f | "
                   "Measure:         %-12.4f | "
                   "Arrange:         %-12.4f | "
                   "Render:          %-12.4f | "
                   "Prune:           %-12.4f |  \n"
                   ANSI_RESET,
                   maxTimings.build.millis,
                   maxTimings.measure.millis,
                   maxTimings.arrange.millis,
                   maxTimings.render.millis,
                   maxTimings.prune.millis);
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
    ok &= yo_arena_create(&context_on_stack.persist, YO_MEGABYTES(1), true, YO_ARENA_TYPE_NO_CHAIN);
    ok &= yo_arena_create(&context_on_stack.frame_states[0].arena, YO_MEGABYTES(1), true, YO_ARENA_TYPE_NO_CHAIN);
    ok &= yo_arena_create(&context_on_stack.frame_states[1].arena, YO_MEGABYTES(1), true, YO_ARENA_TYPE_NO_CHAIN);
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
    //
    // Lazy input / animations
    //

    if ((flags & YO_FRAME_FLAG_LAZY) && (yo_ctx->frame_count > 4))
    {

        // TODO(rune): Would be easier if all relevant fields were in a nested struct.
        bool lazy_input = ((yo_ctx->this_frame->events.count == 0) &&
                           (yo_ctx->prev_frame->events.count == 0) &&
                           yo_equal_struct(yo_ctx->this_frame->mouse_button, yo_ctx->prev_frame->mouse_button) &&
                           yo_equal_struct(yo_ctx->this_frame->mouse_button_down, yo_ctx->prev_frame->mouse_button_down) &&
                           yo_equal_struct(yo_ctx->this_frame->mouse_button_up, yo_ctx->prev_frame->mouse_button_up) &&
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

    return yo_ctx->this_frame->lazy;
}

YO_API void yo_end_frame(yo_render_info_t *info)
{
    yo_end_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].build);

    info->lazy = yo_ctx->this_frame->lazy;

    if (!yo_ctx->this_frame->lazy)
    {

        //
        // Measure
        //
        {
            yo_begin_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].measure);
            yo_measure_content_recurse(yo_ctx->root);
            yo_end_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].measure);
        }

        //
        // Arrange
        //
        {
            yo_begin_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].arrange);
            yo_ctx->root->arranged_rect = yo_rectf(0.0f, 0.0f, (float)info->w, (float)info->h);
            yo_ctx->root->screen_rect   = yo_rectf(0.0f, 0.0f, (float)info->w, (float)info->h);
            yo_arrange_children_recurse(yo_ctx->root);
            yo_end_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].arrange);
        }

        //
        // Render
        //
        {
            yo_array_reset(&yo_ctx->draw_cmds, true);

            info->tex.id     = 42; // TODO(rune): Hardcoded texture id
            info->tex.dim   = yo_ctx->atlas.dim;
            info->tex.pixels = yo_ctx->atlas.pixels;
            info->tex.dirty  = yo_ctx->atlas.dirty;

            yo_begin_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].render);
            yo_render_recurse(yo_ctx->root, info, false);
            yo_render_recurse(yo_ctx->root, info, true);
            yo_end_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].render);

            info->draw_cmds       = yo_ctx->draw_cmds.elems;
            info->draw_cmds_count = yo_ctx->draw_cmds.count;
        }

        //
        // Print debug information
        //
#if 1
        {
            yo_clear_print_buffer();

            yo_debug_print_clear();
            //yo_debug_print_input();
            //yo_debug_print_cache(context);
            //yo_debug_print_hierarchy(yo_global_context->root, 0);
            //yo_debug_print_freelist(context);
            yo_debug_print_performance();
            //yo_debug_print_popups();
            //yo_debug_svg_atlas(yo_g_context->glyph_atlas);

            yo_flush_print_buffer();
        }
#endif

        //
        // Cache prune
        //
        {
            yo_begin_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].prune);
            yo_end_performance_timing(&yo_ctx->timings[yo_ctx->timings_index].prune);
        }
    }

    //
    // Swap frame state
    //

    {
        yo_ctx->frame_count++;
        yo_ctx->this_frame = &yo_ctx->frame_states[(yo_ctx->frame_count + 0) % 2];
        yo_ctx->prev_frame = &yo_ctx->frame_states[(yo_ctx->frame_count + 1) % 2];
    }
}

////////////////////////////////////////////////////////////////
//
//
// Input
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_input_begin(void)
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

YO_API void yo_input_end(void)
{
    // TODO(rune): Since yo_input_end() doesn't do any thing, maybe the api should be more like:
    //  yo_input_clear()
    //  ...
    //  yo_begin_frame()
    //  ...
    //  yo_end_frame()
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
    yo_box_t *internal_box = (yo_box_t *)box;

    if (internal_box)
    {
        if (internal_box->id)
        {
            yo_push_id(internal_box->id);
        }

        yo_array_put(&yo_ctx->parent_stack, internal_box);
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

    yo_rectf_t hit_test_rect = yo_rectf(0, 0, 9999, 9999);

    if (box)
    {
        yo_box_t *box_internal = (yo_box_t *)box;
        yo_id_t id = box_internal->id;
        YO_ASSERT(id && "Missing id.");

        if (id == yo_ctx->prev_frame->hot_id)    ret.is_hot    = true;
        if (id == yo_ctx->prev_frame->active_id) ret.is_active = true;

        yo_box_t *prev = yo_get_box_by_id_prev_frame(id);
        if (prev) hit_test_rect = prev->screen_rect;
    }

    if (yo_clips_rectf(hit_test_rect, yo_ctx->this_frame->mouse_pos))
    {
        ret.hovered = true;
    }

    for (uint32_t i = 0; i < yo_ctx->this_frame->events.count; i++)
    {
        yo_event_t *event = &yo_ctx->this_frame->events.elems[i];

        if (yo_clips_rectf(hit_test_rect, event->pos))
        {
            if (event->type == YO_EVENT_TYPE_MOUSE_CLICK_LEFT)  ret.left_clicked  = true;
            if (event->type == YO_EVENT_TYPE_MOUSE_CLICK_RIGHT) ret.right_clicked = true;
        }

        if (event->type == YO_EVENT_TYPE_KEY_PRESS)
        {
            ret.keycode       = event->keycode;
            ret.is_alt_down   = (event->modifiers & YO_MODIFIER_ALT)   != 0;
            ret.is_ctrl_down  = (event->modifiers & YO_MODIFIER_CTLR)  != 0;
            ret.is_shift_down = (event->modifiers & YO_MODIFIER_SHIFT) != 0;
        }
    }

    yo_v2i_t mouse = yo_query_mouse_pos();
    ret.mouse_pos.x = mouse.x - (int32_t)hit_test_rect.x; // TODO(rune): Input mouse in floating point?
    ret.mouse_pos.y = mouse.y - (int32_t)hit_test_rect.y;

    return ret;
}

YO_API yo_recti_t yo_get_screen_rect(yo_box_t *box)
{
    yo_recti_t ret = { 0 };
    yo_box_t *box_internal = (yo_box_t *)box;
    if (box_internal && box_internal->id)
    {
        yo_box_t *from_prev_frame = yo_get_box_by_id_prev_frame(box_internal->id);
        if (from_prev_frame)
        {
            // TODO(rune): Return in floating point.
            ret.x = (int32_t)from_prev_frame->screen_rect.x;
            ret.y = (int32_t)from_prev_frame->screen_rect.y;
            ret.w = (uint32_t)from_prev_frame->screen_rect.w;
            ret.h = (uint32_t)from_prev_frame->screen_rect.h;

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
    yo_box_t *box_internal = (yo_box_t *)box;
    if (box_internal && box_internal->id)
    {
        yo_box_t *from_prev_frame = yo_get_box_by_id_prev_frame(box_internal->id);
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
    yo_box_t *box_internal = (yo_box_t *)box;
    if (box_internal && box_internal->id)
    {
        yo_box_t *from_prev_frame = yo_get_box_by_id_prev_frame(box_internal->id);
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
YO_API void  yo_set_overflow_h(yo_overflow_t overflow)                          { yo_ctx->latest_child->overflow_h = overflow; }
YO_API void  yo_set_overflow_v(yo_overflow_t overflow)                          { yo_ctx->latest_child->overflow_v = overflow; }
YO_API void  yo_set_align_a(yo_align_t align, yo_axis_t axis)                   { yo_ctx->latest_child->align_a[axis] = align; }
YO_API void  yo_set_align_h(yo_align_t align)                                   { yo_ctx->latest_child->align_h = align; }
YO_API void  yo_set_align_v(yo_align_t align)                                   { yo_ctx->latest_child->align_v = align; }
YO_API void  yo_set_dim_a(yo_length_t length, yo_axis_t axis)                   { yo_ctx->latest_child->dim_a[axis] = length; }
YO_API void  yo_set_dim_h(yo_length_t length)                                   { yo_ctx->latest_child->dim_h = length; }
YO_API void  yo_set_dim_v(yo_length_t length)                                   { yo_ctx->latest_child->dim_v = length; }
YO_API void  yo_set_margin(float left, float top, float right, float bottom)    { yo_ctx->latest_child->margin = yo_margin(left, top, right, bottom); }
YO_API void  yo_set_margin_hv(float h, float v)                                 { yo_ctx->latest_child->margin = yo_margin_hv(h, v); }
YO_API void  yo_set_margin_a(float forward, float backward, yo_axis_t axis)     { yo_ctx->latest_child->margin.axis[axis].forward = forward; yo_ctx->latest_child->margin.axis[axis].backward = backward; }
YO_API void  yo_set_margin_left(float left)                                     { yo_ctx->latest_child->margin.left = left; }
YO_API void  yo_set_margin_top(float top)                                       { yo_ctx->latest_child->margin.top = top; }
YO_API void  yo_set_margin_right(float right)                                   { yo_ctx->latest_child->margin.right = right; }
YO_API void  yo_set_margin_bottom(float bottom)                                 { yo_ctx->latest_child->margin.bottom = bottom; }
YO_API void  yo_set_padding(float left, float top, float right, float bottom)   { yo_ctx->latest_child->padding = yo_padding(left, top, right, bottom); }
YO_API void  yo_set_padding_hv(float h, float v)                                { yo_ctx->latest_child->padding = yo_padding_hv(h, v); }
YO_API void  yo_set_anim(yo_anim_flags_t anim, float rate)                      { yo_ctx->latest_child->anim = anim; yo_ctx->latest_child->anim_rate = rate; }
YO_API void  yo_set_anim_flags(yo_anim_flags_t anim)                            { yo_ctx->latest_child->anim = anim; }
YO_API void  yo_set_anim_rate(float rate)                                       { yo_ctx->latest_child->anim_rate = rate; }
YO_API void  yo_set_scroll(yo_v2f_t offset)                                     { yo_ctx->latest_child->scroll_offset = offset; }
YO_API void  yo_set_scroll_a(float offset, yo_axis_t axis)                      { yo_ctx->latest_child->scroll_offset.v[axis] = offset; }
YO_API void  yo_set_scroll_h(float offset)                                      { yo_ctx->latest_child->scroll_offset.x = offset; }
YO_API void  yo_set_scroll_v(float offset)                                      { yo_ctx->latest_child->scroll_offset.y = offset; }

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
    yo_set_dim_h(yo_px((float)atlas->dim.x));
    yo_set_dim_v(yo_px((float)atlas->dim.y));
    yo_set_fill(yo_rgb(30, 30, 30));
    yo_begin_children();

    for (yo_dlist_each(yo_atlas_shelf_t *, shelf, &atlas->shelf_list))
    {
        yo_box(0, 0);
        yo_set_align_v(YO_ALIGN_TOP);
        yo_set_dim_v(yo_px((float)shelf->dim_y));
        yo_set_margin_top((float)shelf->base_y);
        yo_set_fill(shelf->used_x ? yo_rgb(250, 230, 150) : yo_rgb(250, 200, 100));
        yo_set_border_color(YO_BLACK);
        yo_set_border_thickness(1);

        for (yo_dlist_each(yo_atlas_node_t *, node, &shelf->node_list))
        {
            yo_box(0, 0);
            yo_set_align_h(YO_ALIGN_LEFT);
            yo_set_align_v(YO_ALIGN_TOP);
            yo_set_dim_h(yo_px((float)yo_recti_width(node->rect)));
            yo_set_dim_v(yo_px((float)yo_recti_height(node->rect)));
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
    yo_set_dim_h(yo_px((float)yo_ctx->atlas.dim.x));
    yo_set_dim_v(yo_px((float)yo_ctx->atlas.dim.y));
    yo_set_fill(yo_rgb(255, 255, 255));
}

// DEBUG(rune):
static void yo_debug_show_atlas_partitions_of(yo_atlas_t *atlas)
{
    float scale = 4.0f;

    yo_box(0, 0);
    yo_set_dim_h(yo_px((float)atlas->dim.x * scale));
    yo_set_dim_v(yo_px((float)atlas->dim.y * scale));
    yo_set_fill(yo_rgb(30, 30, 30));
    yo_begin_children();

    for (yo_dlist_each(yo_atlas_shelf_t *, shelf, &atlas->shelf_list))
    {
        yo_box(0, 0);
        yo_set_align_v(YO_ALIGN_TOP);
        yo_set_dim_v(yo_px((float)shelf->dim_y * scale));
        yo_set_dim_h(yo_px((float)atlas->dim.x * scale));
        yo_set_margin_top((float)shelf->base_y * scale);
        yo_set_fill(yo_rgb(shelf->last_accessed_generation, 0, 0));
        yo_set_border_color(YO_CYAN);
        yo_set_border_thickness(1);

        for (yo_dlist_each(yo_atlas_node_t *, node, &shelf->node_list))
        {
            yo_box(0, 0);
            yo_set_align_h(YO_ALIGN_LEFT);
            yo_set_align_v(YO_ALIGN_TOP);
            yo_set_dim_h(yo_px((float)yo_recti_width(node->rect) * scale));
            yo_set_dim_v(yo_px((float)yo_recti_height(node->rect) * scale));
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
