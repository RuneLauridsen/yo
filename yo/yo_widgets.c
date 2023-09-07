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
    box->tag        = "TAG_RECTANGLE";
    box->fill      = fill;
    box->h_dim      = width;
    box->v_dim     = height;
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
    box->border     = yo_border((float)(border_thickness), border_color, (float)(diameter / 2));
    box->h_dim      = yo_px(diameter);
    box->v_dim      = yo_px(diameter);
    box->fill = fill;
    return box;
}


////////////////////////////////////////////////////////////////
//
//
// Layout
//
//
////////////////////////////////////////////////////////////////

YO_API  yo_box_t *yo_h_space(yo_length_t amount)
{
    yo_box_t *box = yo_box(0, 0);
    box->v_dim    = yo_rel(1.0f);
    box->h_dim    = amount;
    return box;
}

YO_API  yo_box_t *yo_v_space(yo_length_t amount)
{
    yo_box_t *box = yo_box(0, 0);
    box->v_dim    = amount;
    box->h_dim    = yo_rel(1.0f);
    return box;
}

YO_API  yo_box_t *yo_h_layout(void)
{
    yo_box_t *box      = yo_box(0, 0);
    box->tag           = "TAG_HORIZONTAL_LAYOUT";
    box->child_layout  = YO_LAYOUT_HORIZONTAL;
    return box;
}

YO_API  yo_box_t *yo_v_layout(void)
{
    yo_box_t *box     = yo_box(0, 0);
    box->tag          = "TAG_VERTICAL_LAYOUT";
    box->child_layout = YO_LAYOUT_VERTICAL;
    return box;
}

YO_API  yo_box_t *yo_container(yo_id_t id)
{
    yo_box_t *box     = yo_box(id, 0);
    box->tag          = "TAG_CONTAINER";
    box->child_layout = YO_LAYTOUT_NONE;
    return box;
}

YO_API  yo_box_t *yo_axis_layout(yo_axis_t axis)
{
    yo_box_t *ret = NULL;

    if (axis == YO_AXIS_X)
    {
        ret = yo_h_layout();
    }
    else
    {
        ret = yo_v_layout();
    }

    return ret;
}

YO_API  yo_box_t *yo_axis_space(yo_axis_t axis, yo_length_t length)
{
    yo_box_t *ret = NULL;

    if (axis == YO_AXIS_X)
    {
        ret = yo_h_space(length);
    }
    else
    {
        ret = yo_v_space(length);
    }

    return ret;
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
    box->tag              = "TAG_TEXT";
    box->text             = text;
    box->font_color       = YO_WHITE; // TODO(rune): Dark/light mode
    return box;
}

YO_API  yo_box_t *yo_format_text_v(char *format, va_list args)
{
    char *text     = yo_alloc_temp_string_v(format, args);
    yo_box_t *box  = yo_text(text);
    box->tag       = "TAG_FORMAT_TEXT";
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
YO_API  yo_text_field_t yo_text_field(yo_id_t id, char *buffer, size_t buffer_size)
{
    yo_text_field_state_t prev_state = { 0 };
    yo_text_field_t ret = { 0 };

    ret.box = yo_box(id,
                     YO_BOX_ACTIVATE_ON_CLICK |
                     YO_BOX_DRAW_TEXT_CURSOR);

    ret.box->text              = buffer;
    ret.box->padding           = yo_padding(5, 5, 5, 5);
    ret.box->font_color        = yo_rgb(220, 220, 220);
    ret.box->fill              = yo_rgb(56, 56, 56);
    ret.box->border            = yo_border(1, yo_rgb(66, 66, 66), 0);
    ret.box->tag               = "TAG_TEXT_FIELD";

    yo_text_field_state_t *state = &ret.box->text_field_state;
    prev_state = *state;

    yo_signal_t box_signal = yo_get_signal(ret.box);

    if (box_signal.is_active)
    {
        ret.box->border.color = yo_rgb(153, 153, 153);
        ret.box->fill         = yo_rgb(31, 31, 31);

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
                        if (box_signal.is_ctrl_down)
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
                        if (box_signal.is_ctrl_down)
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
                    if (box_signal.is_shift_down == false)
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
                        c = (char)box_signal.keycode + (box_signal.is_shift_down ? 0x0 : 0x20);
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

                ret.text_changed = true;
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

                    ret.text_changed = true;
                }
            }

            state->cursor = YO_CLAMP(state->cursor, 0, buffer_strlen);
            state->marker = YO_CLAMP(state->marker, 0, buffer_strlen);
        }
    }

    ret.box->text_field_state = *state;

    if (memcmp(state, &prev_state, sizeof(yo_text_field_state_t)) != 0)
    {
        ret.state_changed = true;
    }

    return ret;
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
    box->text                 = text;
    box->tag                  = "TAG_BUTTON";
    box->fill                 = style.fill;
    box->font_size            = style.font_size;
    box->font_color           = style.font_color;
    box->border               = style.border;
    box->padding              = yo_padding_hv(style.h_padding, style.v_padding);
    box->h_align              = YO_ALIGN_LEFT;
    box->anim                 = YO_ANIM_FILL;
    box->anim_rate            = 20.0f;

    yo_signal_t signal = yo_get_signal(box);
    if (signal.is_hot)
    {
        box->fill = style.hot.fill;
        box->border = style.hot.border;
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

    yo_h_layout();
    YO_CHILD_SCOPE()
    {
        yo_box_t *check_square    = yo_box(yo_id_from_string(label), 0);
        check_square->tag         = "TAG_CHECK_SQUARE";
        check_square->h_dim       = yo_px(20);
        check_square->v_dim       = yo_px(20);
        check_square->padding     = yo_padding(2, 2, 2, 2);
        check_square->border      = yo_border(1, yo_rgb(128, 128, 128), 2);
        check_square->fill        = yo_rgb(64, 64, 64);
        check_square->anim        = YO_ANIM_FILL;

        yo_signal_t signal = yo_get_signal(check_square);
        if (signal.clicked)
        {
            *is_checked = !(*is_checked);
            was_clicked = true;
        }

        if (signal.hovered)
        {
            check_square->fill = yo_rgb(100, 100, 100);
        }

        if (*is_checked)
        {
            yo_scaled_checkmark(yo_rgb(200, 200, 200));
        }

        yo_h_space(yo_px(5));

        yo_box_t *text_box = yo_text(label);
        text_box->v_align = YO_ALIGN_CENTER;
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

    yo_h_layout();
    yo_begin_children();
    {
        yo_box_t *circle_container = yo_box(yo_id_from_string(label), 0);
        circle_container->h_dim    = yo_px(24);
        circle_container->v_dim    = yo_px(24);
        yo_signal_t signal         = yo_get_signal(circle_container);

        yo_begin_children();
        {
            yo_box_t *circle;

            if (signal.hovered)
            {
                circle = yo_circle(yo_id("circle"), 16, yo_rgb(100, 100, 100), 2, yo_rgb(172, 172, 172), YO_CIRCLE_BORDER_ADDS_DIAMETER);
            }
            else
            {
                circle = yo_circle(yo_id("circle"), 16, yo_rgb(64, 64, 64), 0, yo_v4f(0, 0, 0, 0), 0);
            }

            circle->v_align  = YO_ALIGN_CENTER;
            circle->h_align  = YO_ALIGN_CENTER;
            circle->anim = YO_ANIM_FILL|YO_ANIM_BORDER;
            circle->anim_rate = 20.0f;


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

        yo_box_t *text = yo_text(label);
        text->v_align = YO_ALIGN_CENTER;
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
    yo_h_layout();
    YO_CHILD_SCOPE()
    {
        yo_circle(0, 7, yo_rgb(200, 200, 200), 0, YO_TRANSPARENT, 0);
        yo_new()->v_align      = YO_ALIGN_CENTER;
        yo_new()->margin.top   = 7;
        yo_new()->margin.right = 6;
        yo_new()->margin.left  = 10;

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

YO_API  yo_signal_t yo_slider_behaviour(float *value, float min, float max, yo_axis_t axis, uint32_t thumb_dim, yo_box_t *bounding_box)
{
    float dummy_value = 0.0f;
    if (!value) { value = &dummy_value; }

    *value = YO_CLAMP(*value, min, max);

    yo_signal_t signal = yo_get_signal(bounding_box);
    yo_recti_t screen_rect = yo_get_screen_rect(bounding_box);
    if (signal.is_active && yo_query_mouse_button(YO_MOUSE_BUTTON_LEFT))
    {
        float fx = (float)(signal.mouse_pos.axis[axis]);
        float fw = (float)(screen_rect.a_dim[axis]);

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
    const uint32_t THUMB_DIM = 22;

    float dummy_value = 0.0f;
    if (!value) value = &dummy_value;

    yo_box_t *bounding_box = yo_box(id, YO_BOX_ACTIVATE_ON_HOLD);
    bounding_box->tag      = "TAG_SLIDER_MAIN";
    yo_signal_t signal     = yo_slider_behaviour(value, min, max, style->axis, THUMB_DIM, bounding_box);
    YO_CHILD_SCOPE()
    {

        //
        // Slider line
        //
        {
            yo_box_t *line                          = yo_box(0, 0);
            line->fill                              = yo_rgb(64, 64, 64);
            line->border                            = yo_border(0, yo_v4f(0, 0, 0, 0), 2);
            line->a_dim[style->axis]                = yo_rel(1.0f);
            line->a_dim[!style->axis]               = yo_px(10);
            line->margin.axis[style->axis].forward  = 2;
            line->margin.axis[style->axis].backward = 2;
        }

        //
        // Slider circle
        //
        {
            yo_axis_layout(style->axis);
            YO_CHILD_SCOPE()
            {
                yo_axis_space(style->axis, yo_rel((*value - min) / (max - min)));

                yo_box_t   *circle_container        = yo_box(yo_id_from_string("slider circle"), 0);
                yo_signal_t circle_container_signal = yo_get_signal(circle_container);

                circle_container->h_dim = yo_px(style->thumb_container_h_dim);
                circle_container->v_dim = yo_px(style->thumb_container_v_dim);

                bool thumb_hot    = circle_container_signal.hovered;
                bool thumb_active = signal.is_active;

                YO_CHILD_SCOPE()
                {
                    yo_thumb_style_t *thumb_style = &style->thumb;
                    if (thumb_hot)    thumb_style = &style->thumb_hot;
                    if (thumb_active) thumb_style = &style->thumb_active;

                    yo_box_t *thumb = yo_box(0, 0);
                    thumb->border   = thumb_style->border;
                    thumb->fill     = thumb_style->fill;
                    thumb->h_dim    = yo_px(thumb_style->h_dim);
                    thumb->v_dim    = yo_px(thumb_style->v_dim);
                }

                yo_axis_space(style->axis, yo_rel(1.0f - (*value - min) / (max - min)));
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
    yo_v_space(yo_px(5));
    yo_rectangle(yo_rgb(64, 64, 64), yo_rel(1.0f), yo_px(1), 0);
    yo_v_space(yo_px(5));
}

YO_API  yo_signal_t yo_menu_item(char *label)
{
    yo_box_t *box = yo_box(yo_id_from_string(label), 0);
    box->text = label;
    box->h_dim = yo_rel(1.0f);
    box->text                   = label;
    box->tag                    = "TAG_BUTTON";
    box->font_color             = yo_rgb(230, 230, 230);
    box->padding                = yo_padding(5, 5, 5, 5);
    box->fill                   = yo_rgb(48, 48, 48);
    box->border                 = yo_border(1, yo_rgb(48, 48, 48), 0); // NOTE(rune): Dummy border, to keep same layout when items is hovered

    yo_signal_t signal = yo_get_signal(box);
    if (signal.hovered)
    {
        box->fill         = yo_rgb(64, 64, 64);
        box->border.color = yo_rgb(110, 110, 110);
    }

    return signal;
}

YO_API  bool yo_menu_begin_ex(char *label, yo_v4f_t bg)
{
    bool ret = false;

    yo_id_t id = yo_id_from_string(label);
    yo_container(id);
    yo_new()->child_layout = YO_LAYOUT_HORIZONTAL;
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
        yo_h_layout();
        yo_new()->h_overflow = YO_OVERFLOW_SPILL;
        yo_new()->v_overflow   = YO_OVERFLOW_SPILL;
        yo_begin_children();

        yo_v_layout();
        yo_new()->fill       = bg;
        yo_new()->border     = yo_border(1, yo_rgb(64, 64, 64), 0);
        yo_new()->padding    = yo_padding(5, 5, 5, 0);
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
    yo_begin_scroll_area_ex(id, 20.0f, 20.0f);
}

YO_API  void yo_begin_scroll_area_ex(yo_id_t id, float scroll_rate, float anim_rate)
{
    // TODO(rune): Horizontal scrolling

    yo_box_t *area =  yo_container(id);
    area->child_layout = YO_LAYOUT_HORIZONTAL;

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

        yo_box_t *content_container     = yo_container(yo_id("scroll_container"));
        content_container->child_layout = YO_LAYOUT_VERTICAL;
        content_container->tag          = "scroll_container";
        content_container->h_dim        = yo_rel(1.0f);
        content_container->v_overflow   = YO_OVERFLOW_SCROLL;
        content_container->anim         = YO_ANIM_SCROLL;
        content_container->anim_rate    = anim_rate;
        content_container->target_scroll_offset.y += scroll_delta_y;

        //
        // Scroll bar
        //

        yo_container(0);
        yo_new()->v_dim = yo_rel(1.0f);
        YO_CHILD_SCOPE()
        {
            yo_slider_style_t slider_style = yo_default_slider_style();
            slider_style.axis = YO_AXIS_Y;
            slider_style.thumb_container_h_dim = 10; // 20;
            slider_style.thumb_container_v_dim = 20; // 30;

            yo_thumb_style_t thumb_style  = slider_style.thumb;
            thumb_style.border            = yo_border(0, yo_v4f(0, 0, 0, 0), 5);
            thumb_style.fill              = yo_rgb(128, 128, 128);
            thumb_style.h_dim             = 10;
            thumb_style.v_dim             = 20;

            yo_thumb_style_t thumb_style_hot = thumb_style;
            thumb_style_hot.fill             = yo_rgb(172, 172, 172);
            //thumb_style_hot.h_dim            = 20;
            //thumb_style_hot.v_dim            = 30;
            //thumb_style_hot.border           = yo_border(0, 0, 10);

            slider_style.thumb            = thumb_style;
            slider_style.thumb_active     = thumb_style_hot;
            slider_style.thumb_hot        = thumb_style_hot;

            float content_dim_y = yo_get_content_dim(content_container).y;
            float content_area_y = yo_get_arranged_rect(content_container).h;

            float slider_min = 0.0f;
            float slider_max = YO_MAX(content_dim_y - content_area_y, 0.0f);

            yo_slider_ex(yo_id("vert slider"), &content_container->target_scroll_offset.y, slider_min, slider_max, &slider_style);
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
    yo_v_layout();
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

    yo_h_layout();
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
        yo_box_t *box = yo_box(0, 0);
        box->border   = yo_border(1, yo_rgb(128, 128, 128), 0);
        box->fill     = yo_rgb(20, 20, 20);
        box->h_dim    = table->cols[table->col_idx].width;

        if (box->h_dim.is_rel)
        {
            box->h_overflow = YO_OVERFLOW_CLIP;
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
        yo_new()->tag = "TAG_TABLE_CELL_TEXT";
    }
}

YO_API  void yo_table_cell_format_text_v(yo_table_t *table, char *format, va_list args)
{
    char *text = yo_alloc_temp_string_v(format, args);
    yo_table_cell_text(table, text);
    yo_new()->tag = "TAG_TABLE_CELL_FORMAT_TEXT";
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
        yo_new()->tag = "TAG_TABLE_CELL_CHECKBOX";
    }
}

YO_API  void yo_table_cell_header(yo_table_t *table, char *text)
{
    yo_table_cell_text(table, text);
    yo_new()->fill = yo_rgb(64, 64, 64);
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
