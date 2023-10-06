#pragma once

////////////////////////////////////////////////////////////////
//
//
// Rectangle
//
//
////////////////////////////////////////////////////////////////

YO_API yo_box_t *yo_rectangle(yo_id_t id, yo_v4f_t fill, yo_length_t width, yo_length_t height);

////////////////////////////////////////////////////////////////
//
//
// Fill
//
//
////////////////////////////////////////////////////////////////

YO_API yo_box_t *yo_fill(yo_v4f_t fill);

////////////////////////////////////////////////////////////////
//
//
// Circle
//
//
////////////////////////////////////////////////////////////////

typedef uint32_t yo_circle_flags_t;
enum yo_circle_flags
{
    YO_CIRCLE_NONE                 = 0,

    // NOTE(rune): If this flag is set, the diameter argument does not include border thickness,
    // which means that is circles actual diameter will be diameter+2*border_thickness.
    YO_CIRCLE_BORDER_ADDS_DIAMETER = 1,
};

YO_API yo_box_t *yo_circle(yo_id_t id, float diameter, yo_v4f_t fill, uint32_t border_thickness, yo_v4f_t border_color, yo_circle_flags_t flags);


////////////////////////////////////////////////////////////////
//
//
// Layout
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_space_a(yo_length_t amount, yo_axis_t axis);
YO_API void yo_space_x(yo_length_t amount);
YO_API void yo_space_y(yo_length_t amount);
YO_API void yo_layout_a(yo_axis_t axis);
YO_API void yo_layout_x(void);
YO_API void yo_layout_y(void);

////////////////////////////////////////////////////////////////
//
//
// Text
//
//
////////////////////////////////////////////////////////////////

YO_API yo_box_t *yo_text(char *text);
YO_API yo_box_t *yo_format_text(char *format, ...);
YO_API yo_box_t *yo_format_text_v(char *format, va_list args);

////////////////////////////////////////////////////////////////
//
//
// Text field
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_text_field(yo_id_t id, char *buffer, size_t buffer_size);

////////////////////////////////////////////////////////////////
//
//
// Button
//
//
////////////////////////////////////////////////////////////////

YO_API yo_signal_t yo_button(char *text);

////////////////////////////////////////////////////////////////
//
//
// Checkbox
//
//
////////////////////////////////////////////////////////////////

YO_API bool yo_checkbox(char *label, bool *is_checked);

////////////////////////////////////////////////////////////////
//
//
// Radio
//
//
////////////////////////////////////////////////////////////////

YO_API bool yo_radio(char *label, uint32_t index, uint32_t *selected_index);

////////////////////////////////////////////////////////////////
//
//
// Bullet list
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_bullet_item(char *label);

////////////////////////////////////////////////////////////////
//
//
// Slider
//
//
////////////////////////////////////////////////////////////////

YO_API void        yo_slider(yo_id_t id, float *value, float min, float max);
YO_API void        yo_slider_ex(yo_id_t id, float *value, float min, float max, yo_slider_style_t *ex);
YO_API void        yo_slider_with_label(char *label, float *value, float min, float max);
YO_API yo_signal_t yo_slider_behaviour(float *value, float min, float max, yo_axis_t axis, float thumb_dim);

////////////////////////////////////////////////////////////////
//
//
// Menu bar
//
//
////////////////////////////////////////////////////////////////

YO_API void        yo_menubar();
YO_API bool        yo_menubar_item(char *label);

////////////////////////////////////////////////////////////////
//
//
// Menu
//
//
////////////////////////////////////////////////////////////////

YO_API void        yo_menu();
YO_API yo_signal_t yo_menu_item(char *label);
YO_API void        yo_menu_separator(void);

////////////////////////////////////////////////////////////////
//
//
// Scroll area
//
//
////////////////////////////////////////////////////////////////

YO_API void yo_begin_scroll_area(yo_id_t id);
YO_API void yo_begin_scroll_area_ex(yo_id_t id, float scroll_rate, float anim_rate);
YO_API void yo_end_scroll_area(void);

////////////////////////////////////////////////////////////////
//
//
// Table
//
//
////////////////////////////////////////////////////////////////

typedef uint32_t yo_table_falgs_t;
enum yo_table_falgs
{
    YO_TABLE_FLAG_NONE,
    YO_TABLE_FLAG_NO_HEADERS = 1
};

typedef struct yo_table_col yo_table_col_t;
struct yo_table_col
{
    char *header;
    yo_length_t width;
};

typedef struct yo_table yo_table_t;
struct yo_table
{
    yo_table_falgs_t flags;
    yo_table_col_t  *cols;
    uint32_t         col_count;
    uint32_t         col_idx;
};

YO_API void yo_table_def(yo_table_t *table, uint32_t col_count, yo_table_falgs_t flags);
YO_API void yo_table_def_col(yo_table_t *table, char *header, yo_length_t width);
YO_API void yo_table_begin(yo_table_t *table);
YO_API void yo_table_begin_row(yo_table_t *table);
YO_API void yo_table_end(yo_table_t *table);
YO_API void yo_table_end_row(yo_table_t *table);
YO_API void yo_table_cell(yo_table_t *table);
YO_API void yo_table_cell_text(yo_table_t *table, char *text);
YO_API void yo_table_cell_format_text_v(yo_table_t *table, char *format, va_list args);
YO_API void yo_table_cell_format_text(yo_table_t *table, char *format, ...);
YO_API void yo_table_cell_checkbox(yo_table_t *table, char *label, bool *is_checked);
