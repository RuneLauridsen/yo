#pragma once

////////////////////////////////////////////////////////////////
//
//
// Basic
//
//
////////////////////////////////////////////////////////////////

YO_API  yo_box_t *yo_rectangle(yo_v4f_t fill, yo_length_t width, yo_length_t height, yo_id_t id)
{
    yo_box_t *box   = yo_box(id, 0);
    yo_set_tag("TAG_RECTANGLE");
    yo_set_fill(fill);
    yo_set_dim_x(width);
    yo_set_dim_y(height);
    return box;
}

YO_API  yo_box_t *yo_fill(yo_v4f_t fill)
{
    yo_box_t *box = yo_rectangle(fill, yo_rel(1.0f), yo_rel(1.0f), 0);
    return box;
}

YO_API  yo_box_t *yo_circle(yo_id_t id, float diameter, yo_v4f_t fill, uint32_t border_thickness, yo_v4f_t border_color, yo_circle_flags_t flags)
{
    if (flags & YO_CIRCLE_BORDER_ADDS_DIAMETER) { diameter += border_thickness * 2; }

    yo_box_t *box   = yo_box(id, 0);
    yo_set_border((float)(border_thickness), border_color, (float)(diameter / 2));
    yo_set_dim_x(yo_px(diameter));
    yo_set_dim_y(yo_px(diameter));
    yo_set_fill(fill);
    return box;
}


////////////////////////////////////////////////////////////////
//
//
// Layout
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_space_x(yo_length_t amount)
{
    yo_box(0, 0);
    yo_set_dim_y(yo_rel(1.0f));
    yo_set_dim_x(amount);
    yo_set_tag("TAG_SPACE_X");
}

YO_API void yo_space_y(yo_length_t amount)
{
    yo_box(0, 0);
    yo_set_dim_y(amount);
    yo_set_dim_x(yo_rel(1.0f));
    yo_set_tag("TAG_SPACE_Y");
}

YO_API void yo_layout_x(void)
{
    yo_box(0, 0);
    yo_set_tag("TAG_HORIZONTAL_LAYOUT");
    yo_set_layout(YO_LAYOUT_STACK_X);
}

YO_API void yo_layout_y(void)
{
    yo_box(0, 0);
    yo_set_tag("TAG_VERTICAL_LAYOUT");
    yo_set_layout(YO_LAYOUT_STACK_Y);
}

YO_API void yo_layout_a(yo_axis_t axis)
{
    if (axis == YO_AXIS_X)
    {
        yo_layout_x();
    }
    else
    {
        yo_layout_y();
    }
}

YO_API void yo_space_a(yo_length_t length, yo_axis_t axis)
{
    if (axis == YO_AXIS_X)
    {
        yo_space_x(length);
    }
    else
    {
        yo_space_y(length);
    }
}

////////////////////////////////////////////////////////////////
//
//
// Text
//
//
////////////////////////////////////////////////////////////////

YO_API  yo_box_t *yo_text(char *text)
{
    yo_box_t *box         = yo_box(0, 0);
    yo_set_tag("TAG_TEXT");
    yo_set_text(text);
    yo_set_font_color(YO_WHITE); // TODO(rune): Dark/light mode
    return box;
}

YO_API  yo_box_t *yo_format_text_v(char *format, va_list args)
{
    char *text     = yo_alloc_temp_string_v(format, args);
    yo_box_t *box  = yo_text(text);
    yo_set_tag("TAG_FORMAT_TEXT");
    return box;
}

YO_API  yo_box_t *yo_format_text(char *format, ...)
{
    va_list args;
    va_start(args, format);
    yo_box_t *ret = yo_format_text_v(format, args);
    va_end(args);

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Text field
//
//
////////////////////////////////////////////////////////////////

// NOTE(rune): Replaces a substring in a buffer with another string from outside the buffer. This single function
// can be used for insert/delete/replace operations, by passing either a replace string or insert string, with a
// length_t of zero. To prevent buffer overflow, only the part of the insert string which actually fits in the buffer
// is inserted. The functions returns the number of characters inserted. The resulting buffer is null-terminated.
YO_API  size_t yo_do_text_replace(char *buffer, size_t buffer_strlen, size_t buffer_size, size_t replace_begin, size_t replace_strlen, char* insert, size_t insert_strlen)
{
    YO_ASSERT(buffer_strlen < buffer_size);

    // NOTE(rune): Avoid overflow by clamping length_t of insert string.
    // We also subtract 1 so there's always space for a null terminator.
    insert_strlen = YO_CLAMP(insert_strlen, (size_t)0, buffer_size - buffer_strlen - 1);

    char    *move        = buffer + replace_begin + replace_strlen;
    intptr_t move_offset = insert_strlen - replace_strlen;
    size_t   move_size   = buffer_strlen - insert_strlen - replace_begin + 1;

    memmove(move + move_offset,
            move,
            move_size);

    memmove(buffer + replace_begin,
            insert,
            insert_strlen);

    return insert_strlen;
}

YO_API  bool yo_is_word_char(char c)
{
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_'))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// TODO(rune): Cleanup
YO_API void yo_text_field(yo_id_t id, char *buffer, size_t buffer_size)
{
    yo_box_t *box = yo_box(id,
                           YO_BOX_ACTIVATE_ON_CLICK |
                           YO_BOX_DRAW_TEXT_CURSOR);

    yo_set_text(buffer);
    yo_set_padding(5, 5, 5, 5);
    yo_set_font_color(yo_rgb(220, 220, 220));
    yo_set_fill(yo_rgb(56, 56, 56));
    yo_set_border(1, yo_rgb(66, 66, 66), 0);
    yo_set_tag("TAG_TEXT_FIELD");

    yo_text_field_state_t *state = yo_get_text_field_state();

    yo_signal_t box_signal = yo_get_signal(box);

    if (box_signal.is_active)
    {
        yo_set_border_color(yo_rgb(153, 153, 153));
        yo_set_fill(yo_rgb(31, 31, 31));

        if (box_signal.keycode)
        {
            // TODO(rune): strlen
            uint32_t buffer_strlen = (uint32_t)strlen(buffer);

            //
            // Move cursor
            //
            {
                bool cursor_moved = true;

                switch (box_signal.keycode)
                {
                    case YO_KEYCODE_LEFT_ARROW:
                    {
                        if (box_signal.modifiers & YO_MODIFIER_CTLR)
                        {
                            size_t i;
                            size_t next_cursor_pos = 0;
                            for (i = yo_safe_sub_u32(state->cursor, 1);
                                 i > 0;
                                 i--)
                            {
                                if (yo_is_word_char(buffer[i]) && !yo_is_word_char(buffer[i - 1]))
                                {
                                    next_cursor_pos = i;
                                    break;
                                }
                            }

                            state->cursor = (uint32_t)next_cursor_pos;
                        }
                        else
                        {
                            state->cursor = yo_safe_sub_u32(state->cursor, 1);
                        }
                    } break;

                    case YO_KEYCODE_RIGHT_ARROW:
                    {
                        if (box_signal.modifiers & YO_MODIFIER_CTLR)
                        {
                            size_t i;
                            size_t next_cursor_pos = buffer_strlen;
                            for (i = state->cursor + 1; i < yo_safe_add_u32(buffer_strlen, 1); i++)
                            {
                                if (yo_is_word_char(buffer[i]) && !yo_is_word_char(buffer[i - 1]))
                                {
                                    next_cursor_pos = i;
                                    break;
                                }
                            }

                            state->cursor = (uint32_t)next_cursor_pos;
                        }
                        else
                        {
                            state->cursor = yo_safe_add_u32(state->cursor, 1);
                        }
                    } break;

                    case YO_KEYCODE_HOME:
                    {
                        state->cursor = 0;
                    } break;

                    case YO_KEYCODE_END:
                    {
                        state->cursor = buffer_strlen;
                    } break;

                    default:
                    {
                        cursor_moved = false;
                    } break;
                }

                if (cursor_moved)
                {
                    if (!(box_signal.modifiers & YO_MODIFIER_SHIFT))
                    {
                        state->marker = state->cursor;
                    }

                    state->cursor = YO_CLAMP(state->cursor, 0u, buffer_strlen);
                    state->marker = YO_CLAMP(state->marker, 0u, buffer_strlen);
                }
            }

            size_t selection_begin  = YO_MIN(state->cursor, state->marker);
            size_t selection_end    = YO_MAX(state->cursor, state->marker);
            size_t selection_strlen = selection_end - selection_begin;

            //
            // Add character
            //
            {
                bool is_letter = box_signal.keycode >= 'A' && box_signal.keycode <= 'Z';
                bool is_space  = box_signal.keycode == YO_KEYCODE_SPACE;
                if (is_letter || is_space)
                {
                    char c = 0;

                    if (is_letter)
                    {
                        c = (char)box_signal.keycode + ((box_signal.modifiers & YO_MODIFIER_SHIFT) ? 0x0 : 0x20);
                    }

                    if (is_space)
                    {
                        c = ' ';
                    }

                    YO_ASSERT(c);

                    size_t inserted_strlen = yo_do_text_replace(buffer, buffer_strlen, buffer_size, selection_begin, selection_strlen, &c, 1);
                    if (inserted_strlen)
                    {
                        state->cursor = YO_MIN(state->cursor, state->marker) + 1;
                        state->marker = state->cursor;
                    }
                }
            }

            //
            // Delete character
            //
            {
                // TODO(rune): Delete whole words with CTRL

                if ((box_signal.keycode == YO_KEYCODE_BACKSPACE && (state->cursor != 0 || state->marker != 0)) ||
                    (box_signal.keycode == YO_KEYCODE_DELETE    && (state->cursor != buffer_strlen || state->marker != buffer_strlen)))
                {
                    if (state->cursor == state->marker)
                    {
                        if (box_signal.keycode == YO_KEYCODE_BACKSPACE)
                        {
                            selection_begin = yo_safe_sub_usize(selection_begin, 1);
                            selection_strlen++;
                        }

                        if (box_signal.keycode == YO_KEYCODE_DELETE)
                        {
                            selection_end = yo_safe_add_usize(selection_end, 1);
                            selection_strlen++;
                        }
                    }

                    yo_do_text_replace(buffer, buffer_strlen, buffer_size, selection_begin, selection_strlen, NULL, 0);

                    state->cursor = (uint32_t)selection_begin;
                    state->marker = state->cursor;
                }
            }

            state->cursor = YO_CLAMP(state->cursor, 0, buffer_strlen);
            state->marker = YO_CLAMP(state->marker, 0, buffer_strlen);
        }
    }
}

////////////////////////////////////////////////////////////////
//
//
// Button
//
//
////////////////////////////////////////////////////////////////

YO_API  yo_signal_t yo_button(char *text)
{
    yo_button_style_t style = yo_default_style().button_style;

    yo_box_t *box             = yo_box(yo_id_from_string(text),
                                       YO_BOX_HOT_ON_HOVER);
    yo_set_text(text);
    yo_set_tag("TAG_BUTTON");
    yo_set_fill(style.fill);
    yo_set_font_size(style.font_size);
    yo_set_font_color(style.font_color);
    yo_set_border_s(style.border);
    yo_set_padding_xy(style.h_padding, style.v_padding);
    yo_set_align_x(YO_ALIGN_LEFT);
    yo_set_anim(YO_ANIM_FILL, 20.0f);

    yo_signal_t signal = yo_get_signal(box);
    if (signal.is_hot)
    {
        yo_set_fill(style.hot.fill);
        yo_set_border_s(style.hot.border);
    }

    return signal;
}


////////////////////////////////////////////////////////////////
//
//
// Checkbox
//
//
////////////////////////////////////////////////////////////////

YO_API  bool yo_checkbox(char *label, bool *is_checked)
{
    bool dummy = false;
    if (!is_checked) is_checked = &dummy;

    bool was_clicked = false;

    yo_layout_x();
    YO_CHILD_SCOPE()
    {
        yo_box_t *check_square    = yo_box(yo_id_from_string(label), 0);
        yo_set_tag("TAG_CHECK_SQUARE");
        yo_set_dim_x(yo_px(20));
        yo_set_dim_y(yo_px(20));
        yo_set_padding(2, 2, 2, 2);
        yo_set_border(1, yo_rgb(128, 128, 128), 2);
        yo_set_fill(yo_rgb(64, 64, 64));
        yo_set_anim(YO_ANIM_FILL, 20.0f);

        yo_signal_t signal = yo_get_signal(check_square);
        if (signal.clicked)
        {
            *is_checked = !(*is_checked);
            was_clicked = true;
        }

        if (signal.hovered)
        {
            yo_set_fill(yo_rgb(100, 100, 100));
        }

        if (*is_checked)
        {
            yo_scaled_checkmark(yo_rgb(200, 200, 200));
        }

        yo_space_x(yo_px(5));

        yo_text(label);
        yo_set_align_y(YO_ALIGN_CENTER);
    }

    return was_clicked;
}

////////////////////////////////////////////////////////////////
//
//
// Radio
//
//
////////////////////////////////////////////////////////////////

YO_API  bool yo_radio(char *label, uint32_t index, uint32_t *selected_index)
{
    bool was_clicked = false;

    yo_layout_x();
    yo_begin_children();
    {
        yo_box_t *circle_container = yo_box(yo_id_from_string(label), 0);
        yo_set_dim_x(yo_px(24));
        yo_set_dim_y(yo_px(24));
        yo_signal_t signal = yo_get_signal(circle_container);

        yo_begin_children();
        {
            if (signal.hovered)
            {
                yo_circle(yo_id("circle"), 16, yo_rgb(100, 100, 100), 2, yo_rgb(172, 172, 172), YO_CIRCLE_BORDER_ADDS_DIAMETER);
            }
            else
            {
                yo_circle(yo_id("circle"), 16, yo_rgb(64, 64, 64), 0, yo_v4f(0, 0, 0, 0), 0);
            }

            yo_set_align_y(YO_ALIGN_CENTER);
            yo_set_align_x(YO_ALIGN_CENTER);
            yo_set_anim(YO_ANIM_FILL|YO_ANIM_BORDER, 20.0f);


            if (signal.clicked)
            {
                *selected_index = index;
                was_clicked = true;
            }

            if (*selected_index == index)
            {
                yo_circle(0, 6, yo_rgb(200, 200, 200), 0, yo_v4f(0, 0, 0, 0), 0);
            }

        }
        yo_end_children();

        yo_text(label);
        yo_set_align_y(YO_ALIGN_CENTER);
    }
    yo_end_children();

    return was_clicked;
}

////////////////////////////////////////////////////////////////
//
//
// Bullet list
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_bullet_item(char *label)
{
    yo_layout_x();
    YO_CHILD_SCOPE()
    {
        yo_circle(0, 7, yo_rgb(200, 200, 200), 0, YO_TRANSPARENT, 0);
        yo_set_align_y(YO_ALIGN_CENTER);
        yo_set_margin(10, 7, 6, 0);

        yo_text(label);
    }
}

////////////////////////////////////////////////////////////////
//
//
// Slider
//
//
////////////////////////////////////////////////////////////////

YO_API  yo_signal_t yo_slider_behaviour(float *value, float min, float max, yo_axis_t axis, float thumb_dim, yo_box_t *bounding_box)
{
    float dummy_value = 0.0f;
    if (!value) { value = &dummy_value; }

    *value = YO_CLAMP(*value, min, max);

    yo_signal_t signal      = yo_get_signal(bounding_box);
    yo_rectf_t  screen_rect = yo_get_screen_rect(bounding_box);

    if (signal.is_active && yo_query_mouse_button(YO_MOUSE_BUTTON_LEFT))
    {
        float fx = (float)(signal.mouse_pos.axis[axis]);
        float fw = (float)(screen_rect.dim_a[axis]);

        float offset = (fx - thumb_dim / 2) / (fw - thumb_dim);
        *value = yo_lerp(min, max, offset);
    }

    return signal;
}

YO_API  void yo_slider(yo_id_t id, float *value, float min, float max)
{
    yo_slider_style_t style = yo_default_slider_style();
    yo_slider_ex(id, value, min, max, &style);
}

YO_API  void yo_slider_ex(yo_id_t id, float *value, float min, float max, yo_slider_style_t *style)
{
    float thumb_dim = style->thumb_container_dim_a[style->axis];

    float dummy_value = 0.0f;
    if (!value) value = &dummy_value;

    yo_box_t *bounding_box = yo_box(id, YO_BOX_ACTIVATE_ON_HOLD);
    yo_set_tag("TAG_SLIDER_MAIN");
    yo_set_dim_a(yo_rel(1.0f), style->axis);
    yo_signal_t signal     = yo_slider_behaviour(value, min, max, style->axis, thumb_dim, bounding_box);
    YO_CHILD_SCOPE()
    {

        //
        // Slider line
        //
        {
            yo_box(0, 0);
            yo_set_tag("TAG_SLIDER_LINE");
            yo_set_fill(yo_rgb(64, 64, 64));
            yo_set_border(0, yo_v4f(0, 0, 0, 0), 2);
            yo_set_dim_a(yo_rel(1.0f), style->axis);
            yo_set_dim_a(yo_px(10), !style->axis);
            yo_set_margin_a(2, 2, style->axis);
            yo_set_align_a(YO_ALIGN_CENTER, style->axis);
        }

        //
        // Slider circle
        //
        {
            yo_layout_a(style->axis);
            YO_CHILD_SCOPE()
            {
                yo_space_a(yo_rel((*value - min) / (max - min)), style->axis);

                yo_box_t   *circle_container        = yo_box(yo_id_from_string("slider circle"), 0);
                yo_signal_t circle_container_signal = yo_get_signal(circle_container);

                yo_set_dim_x(yo_px(style->thumb_container_dim_x));
                yo_set_dim_y(yo_px(style->thumb_container_dim_y));
                yo_set_align_a(YO_ALIGN_CENTER, style->axis);

                bool thumb_hot    = circle_container_signal.hovered;
                bool thumb_active = signal.is_active;

                YO_CHILD_SCOPE()
                {
                    yo_thumb_style_t *thumb_style = &style->thumb;
                    if (thumb_hot)    thumb_style = &style->thumb_hot;
                    if (thumb_active) thumb_style = &style->thumb_active;

                    yo_box(0, 0);
                    yo_set_border_s(thumb_style->border);
                    yo_set_fill(thumb_style->fill);
                    yo_set_dim_x(yo_px(thumb_style->dim_x));
                    yo_set_dim_y(yo_px(thumb_style->dim_y));
                }

                yo_space_a(yo_rel(1.0f - (*value - min) / (max - min)), style->axis);
            }
        }
    }
}

YO_API  void yo_slider_with_label(char *label, float *value, float min, float max)
{
    YO_UNUSED(label, value, min, max);
}

////////////////////////////////////////////////////////////////
//
//
// Menu
//
//
////////////////////////////////////////////////////////////////

YO_API  void yo_menu_separator(void)
{
    yo_space_y(yo_px(5));
    yo_rectangle(yo_rgb(64, 64, 64), yo_rel(1.0f), yo_px(1), 0);
    yo_space_y(yo_px(5));
}

YO_API  yo_signal_t yo_menu_item(char *label)
{
    yo_box_t *box = yo_box(yo_id_from_string(label), 0);
    yo_set_text(label);
    yo_set_dim_x(yo_rel(1.0f));
    yo_set_text(label);
    yo_set_tag("TAG_BUTTON");
    yo_set_font_color(yo_rgb(230, 230, 230));
    yo_set_padding(5, 5, 5, 5);
    yo_set_fill(yo_rgb(48, 48, 48));
    yo_set_border(1, yo_rgb(48, 48, 48), 0); // NOTE(rune): Dummy border, to keep same layout when items is hovered

    yo_signal_t signal = yo_get_signal(box);
    if (signal.hovered)
    {
        yo_set_fill(yo_rgb(64, 64, 64));
        yo_set_border_color(yo_rgb(110, 110, 110));
    }

    return signal;
}

YO_API  bool yo_menu_begin_ex(char *label, yo_v4f_t bg)
{
    bool ret = false;

    yo_id_t id = yo_id_from_string(label);
    yo_box(id, 0);
    yo_set_layout(YO_LAYOUT_STACK_X);
    yo_begin_children();
    {
        if (yo_menu_item(label).hovered)
        {
            yo_open_popup(id, YO_POPUP_FLAG_EXCLUSIVE);
        }

    }
    yo_end_children();

    if (yo_begin_popup(id))
    {
        ret = true;

        yo_begin_children();
        yo_layout_x();
        yo_set_overflow_x(YO_OVERFLOW_SPILL);
        yo_set_overflow_y(YO_OVERFLOW_SPILL);
        yo_begin_children();

        yo_layout_y();
        yo_set_fill(bg);
        yo_set_border(1, yo_rgb(64, 64, 64), 0);
        yo_set_padding(5, 5, 5, 0);
        yo_begin_children();
    }

    return ret;
}

YO_API  bool yo_menu_begin(char *label)
{
    bool ret = yo_menu_begin_ex(label, yo_rgb(48, 48, 48));
    return ret;
}

YO_API  void yo_menu_end(void)
{
    yo_end_children();
    yo_end_children();
    yo_end_children();
    yo_end_popup();
}

////////////////////////////////////////////////////////////////
//
//
// Scroll area
//
//
////////////////////////////////////////////////////////////////

YO_API  void yo_begin_scroll_area(yo_id_t id)
{
    yo_begin_scroll_area_ex(id, 0, 0);
}

YO_API  void yo_begin_scroll_area_ex(yo_id_t id, float scroll_rate, float anim_rate)
{
    // TODO(rune): Horizontal scrolling

    if (!scroll_rate) scroll_rate = 20.0f;
    if (!anim_rate)   anim_rate   = 20.f;

    yo_box_t *area = yo_box(id, 0);
    yo_set_layout(YO_LAYOUT_STACK_X);

    bool mouse_hover = yo_get_signal(area).hovered;
    float scroll_delta_y = 0.0f;

    if (mouse_hover)
    {
        yo_v2f_t scroll = yo_query_scroll();
        scroll_delta_y = scroll.y * -scroll_rate;
    }

    yo_begin_children();
    {
        //
        // Content container
        //

        yo_box_t *content_container     = yo_box(yo_id("scroll_container"), 0);
        yo_set_layout(YO_LAYOUT_STACK_Y);
        yo_set_tag("scroll_container");
        yo_set_dim_x(yo_rel(1.0f));
        yo_set_overflow_y(YO_OVERFLOW_SCROLL);
        yo_set_anim(YO_ANIM_SCROLL, anim_rate);

        yo_v2f_t *offset = yo_get_userdata(sizeof(yo_v2f_t));

        //
        // Scroll behaviour
        //

        float content_dim_y       = yo_get_content_dim(content_container).y;
        float content_dim_avail_y = yo_get_arranged_rect(content_container).h;

        float offset_min = 0.0f;
        float offset_max = YO_MAX(content_dim_y - content_dim_avail_y, 0.0f);

        float content_ratio = content_dim_avail_y * YO_CLAMP01(content_dim_avail_y / content_dim_y);

        offset->y = YO_CLAMP(offset->y + scroll_delta_y, offset_min, offset_max);
        yo_set_scroll(*offset);

        //
        // Scroll bar
        //

        yo_box(0, 0);
        yo_set_dim_y(yo_rel(1.0f));
        YO_CHILD_SCOPE()
        {
            yo_slider_style_t slider_style = yo_default_slider_style();
            slider_style.axis = YO_AXIS_Y;
            slider_style.thumb_container_dim_x = 10;
            slider_style.thumb_container_dim_y = content_ratio;

            yo_thumb_style_t thumb_style  = slider_style.thumb;
            thumb_style.border            = yo_border(0, yo_v4f(0, 0, 0, 0), 5);
            thumb_style.fill              = yo_rgb(128, 128, 128);
            thumb_style.dim_x             = slider_style.thumb_container_dim_x;
            thumb_style.dim_y             = slider_style.thumb_container_dim_y;

            yo_thumb_style_t thumb_style_hot = thumb_style;
            thumb_style_hot.fill             = yo_rgb(172, 172, 172);

            slider_style.thumb            = thumb_style;
            slider_style.thumb_active     = thumb_style_hot;
            slider_style.thumb_hot        = thumb_style_hot;

            yo_slider_ex(yo_id("vert slider"), &offset->y, offset_min, offset_max, &slider_style);
        }

        yo_begin_children_of(content_container);
    }
}

YO_API  void yo_end_scroll_area(void)
{
    yo_end_children();
    yo_end_children();
}


////////////////////////////////////////////////////////////////
//
//
// Table
//
//
////////////////////////////////////////////////////////////////

YO_API  void yo_table_def(yo_table_t *table, uint32_t col_count, yo_table_falgs_t flags)
{
    table->cols      = yo_alloc_temp(col_count * sizeof(yo_table_col_t));
    table->col_count = table->cols ? col_count : 0;
    table->col_idx   = 0;
    table->flags     = flags;

    for (uint32_t i = 0; i < table->col_count; i++)
    {
        table->cols[i].header = "";
        table->cols[i].width = yo_rel(1.0f);
    }
}

YO_API  void yo_table_def_col(yo_table_t *table, char *header, yo_length_t width)
{
    if (table->col_idx < table->col_count)
    {
        table->cols[table->col_idx].header = header;
        table->cols[table->col_idx].width  = width;
    }

    table->col_idx++;
}

YO_API  void yo_table_headers(yo_table_t *table);

YO_API  void yo_table_begin(yo_table_t *table)
{
    yo_layout_y();
    yo_begin_children();

    if (!(table->flags & YO_TABLE_FLAG_NO_HEADERS))
    {
        yo_table_headers(table);
    }
}

YO_API  void yo_table_end(yo_table_t *table)
{
    YO_UNUSED(table);

    yo_end_children();
}

YO_API  void yo_table_begin_row(yo_table_t *table)
{
    table->col_idx = 0;

    yo_layout_x();
    yo_begin_children();
}

YO_API  void yo_table_end_row(yo_table_t *table)
{
    YO_UNUSED(table);

    yo_end_children();
}

YO_API  void yo_table_cell(yo_table_t *table)
{
    if (table->col_idx < table->col_count)
    {
        yo_box(0, 0);
        yo_set_border(1, yo_rgb(128, 128, 128), 0);
        yo_set_fill(yo_rgb(20, 20, 20));
        yo_set_dim_x(table->cols[table->col_idx].width);

        if (table->cols[table->col_idx].width.is_rel)
        {
            yo_set_overflow_x(YO_OVERFLOW_CLIP);
        }
    }

    table->col_idx++;
}

YO_API  void yo_table_cell_text(yo_table_t *table, char *text)
{
    yo_table_cell(table);
    YO_CHILD_SCOPE()
    {
        yo_text(text);
        yo_set_tag("TAG_TABLE_CELL_TEXT");
    }
}

YO_API  void yo_table_cell_format_text_v(yo_table_t *table, char *format, va_list args)
{
    char *text = yo_alloc_temp_string_v(format, args);
    yo_table_cell_text(table, text);
    yo_set_tag("TAG_TABLE_CELL_FORMAT_TEXT");
}

YO_API  void yo_table_cell_format_text(yo_table_t *table, char *format, ...)
{
    va_list args;
    va_start(args, format);
    yo_table_cell_format_text_v(table, format, args);
    va_end(args);
}

YO_API  void yo_table_cell_checkbox(yo_table_t *table, char *label, bool *is_checked)
{
    yo_table_cell(table);
    YO_CHILD_SCOPE()
    {
        yo_checkbox(label, is_checked);
        yo_set_tag("TAG_TABLE_CELL_CHECKBOX");
    }
}

YO_API  void yo_table_cell_header(yo_table_t *table, char *text)
{
    yo_table_cell_text(table, text);
    yo_set_fill(yo_rgb(64, 64, 64));
}

YO_API  void yo_table_headers(yo_table_t *table)
{
    yo_table_begin_row(table);

    for (uint32_t i = 0; i < table->col_count; i++)
    {
        yo_table_cell_header(table, table->cols[i].header);
    }

    yo_table_end_row(table);
}
