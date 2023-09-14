using System.Runtime.InteropServices;

using uint16_t = System.UInt16;

namespace YoNew;

internal unsafe partial class YoNative
{
    public enum yo_error_t : uint32_t
    {
        YO_ERROR_NONE,
        YO_ERROR_MEMORY_ALLOCATION_FAILED,
        YO_ERROR_OUT_OF_PERSITENT_MEMORY,
        YO_ERROR_OUT_OF_TEMPORARY_MEMORY,
        YO_ERROR_PARENT_STACK_UNDERFLOW,
        YO_ERROR_STYLE_STACK_UNDERFLOW,
        YO_ERROR_ID_COLLISION,
        YO_ERROR_COUNT,
    }
    public enum yo_axis_t : uint32_t
    {
        YO_AXIS_X,
        YO_AXIS_Y,
        YO_AXIS_COUNT,
    }
    public enum yo_layout_t : uint32_t
    {
        YO_LAYTOUT_NONE,
        YO_LAYOUT_HORIZONTAL,
        YO_LAYOUT_VERTICAL,
        YO_LAYOUT_TABLE_COLUMN,
        YO_LAYOUT_COUNT,
    }
    public enum yo_align_t : uint32_t
    {
        YO_ALIGN_STRETCH,
        YO_ALIGN_CENTER,
        YO_ALIGN_FRONT,
        YO_ALIGN_BACK,
        YO_ALIGN_COUNT,
        YO_ALIGN_LEFT = YO_ALIGN_FRONT,
        YO_ALIGN_TOP = YO_ALIGN_FRONT,
        YO_ALIGN_RIGHT = YO_ALIGN_BACK,
        YO_ALIGN_BOTTOM = YO_ALIGN_BACK,
    }
    public enum yo_overflow_t : uint32_t
    {
        YO_OVERFLOW_FIT,
        YO_OVERFLOW_SPILL,
        YO_OVERFLOW_CLIP,
        YO_OVERFLOW_SCROLL,
        YO_OVERFLOW_COUNT,
    }
    public enum yo_side_t : uint32_t
    {
        YO_SIDE_LEFT,
        YO_SIDE_RIGHT,
        YO_SIDE_TOP,
        YO_SIDE_BOTTOM,
        YO_SIDE_COUNT,
    }
    public enum yo_corner_t : uint32_t
    {
        YO_CORNER_TOP_LEFT,
        YO_CORNER_TOP_RIGHT,
        YO_CORNER_BOTTOM_LEFT,
        YO_CORNER_BOTTOM_RIGHT,
        YO_CORNER_COUNT,
    }
    public enum yo_popup_flags_t : uint32_t
    {
        YO_POPUP_FLAG_NONE,
        YO_POPUP_FLAG_EXCLUSIVE = 1,
    }
    public enum yo_box_flags_t : uint32_t
    {
        YO_BOX_NONE = 0,
        YO_BOX_ACTIVATE_ON_CLICK = (1 << 0),
        YO_BOX_ACTIVATE_ON_HOLD = (1 << 1),
        YO_BOX_HOT_ON_HOVER = (1 << 2),
        YO_BOX_FULL_ATLAS = (1 << 3),
        YO_BOX_DRAW_TEXT_CURSOR = (1 << 4),
    }
    public enum yo_anim_flags_t : uint32_t
    {
        YO_ANIM_NONE = 0,
        YO_ANIM_FILL = (1 << 0),
        YO_ANIM_DIM = (1 << 1),
        YO_ANIM_MARGIN = (1 << 2),
        YO_ANIM_PADDING = (1 << 3),
        YO_ANIM_SCROLL = (1 << 4),
        YO_ANIM_BORDER_COLOR = (1 << 5),
        YO_ANIM_BORDER_THICKNESS = (1 << 6),
        YO_ANIM_BORDER_RADIUS = (1 << 7),
        YO_ANIM_FONT_COLOR = (1 << 8),
        YO_ANIM_BORDER = YO_ANIM_BORDER_COLOR | YO_ANIM_BORDER_THICKNESS | YO_ANIM_BORDER_RADIUS,
    }
    public enum yo_mouse_button_t : uint32_t
    {
        YO_MOUSE_BUTTON_LEFT,
        YO_MOUSE_BUTTON_RIGHT,
        YO_MOUSE_BUTTON_MIDDLE,
        YO_MOUSE_BUTTON_COUNT,
    }
    public enum yo_keycode_t : uint32_t
    {
        YO_KEYCODE_NONE = 0,
        YO_KEYCODE_LEFT_ARROW = 128,
        YO_KEYCODE_RIGHT_ARROW,
        YO_KEYCODE_UP_ARROW,
        YO_KEYCODE_DOWN_ARROW,
        YO_KEYCODE_HOME,
        YO_KEYCODE_END,
        YO_KEYCODE_SPACE,
        YO_KEYCODE_RETURN,
        YO_KEYCODE_BACKSPACE,
        YO_KEYCODE_DELETE,
    }
    public enum yo_modifier_t : uint16_t
    {
        YO_MODIFIER_NONE,
        YO_MODIFIER_SHIFT,
        YO_MODIFIER_CTLR,
        YO_MODIFIER_ALT,
        YO_MODIFIER_COUNT,
    }
    public enum yo_frame_flags_t : uint32_t
    {
        YO_FRAME_FLAG_NONE,
        YO_FRAME_FLAG_LAZY,
    }
    public enum yo_draw_cmd_type_t : uint32_t
    {
        YO_DRAW_CMD_NONE,
        YO_DRAW_CMD_AABB,
        YO_DRAW_CMD_TRI,
        YO_DRAW_CMD_QUAD,
    }
    [DllImport("yo_dll.dll")] public static extern yo_context_t* yo_create_context(yo_config_t* config);
    [DllImport("yo_dll.dll")] public static extern void yo_destroy_context(yo_context_t* context);
    [DllImport("yo_dll.dll")] public static extern void yo_select_context(yo_context_t* context);
    [DllImport("yo_dll.dll")] public static extern bool yo_begin_frame(float time, yo_frame_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern void yo_end_frame(yo_render_info_t* buffer);
    [DllImport("yo_dll.dll")] public static extern void yo_input_begin();
    [DllImport("yo_dll.dll")] public static extern void yo_input_mouse_state(uint32_t x, uint32_t y);
    [DllImport("yo_dll.dll")] public static extern void yo_input_mouse_click(yo_mouse_button_t button, uint32_t x, uint32_t y, yo_modifier_t modifiers);
    [DllImport("yo_dll.dll")] public static extern void yo_input_scroll(float x, float y);
    [DllImport("yo_dll.dll")] public static extern void yo_input_key(yo_keycode_t key, yo_modifier_t modifiers);
    [DllImport("yo_dll.dll")] public static extern void yo_input_char(char c, yo_modifier_t modifiers);
    [DllImport("yo_dll.dll")] public static extern void yo_input_unicode(uint32_t codepoint, yo_modifier_t modifiers);
    [DllImport("yo_dll.dll")] public static extern void yo_input_end();
    [DllImport("yo_dll.dll")] public static extern yo_v2i_t yo_query_mouse_pos();
    [DllImport("yo_dll.dll")] public static extern bool yo_query_mouse_button(yo_mouse_button_t button);
    [DllImport("yo_dll.dll")] public static extern bool yo_query_mouse_button_down(yo_mouse_button_t button);
    [DllImport("yo_dll.dll")] public static extern bool yo_query_mouse_button_up(yo_mouse_button_t button);
    [DllImport("yo_dll.dll")] public static extern yo_v2f_t yo_query_scroll();
    [DllImport("yo_dll.dll")] public static extern float yo_delta();
    [DllImport("yo_dll.dll")] public static extern yo_id_t yo_id_from_string(string @string);
    [DllImport("yo_dll.dll")] public static extern yo_id_t yo_id_from_format(string format, __arglist);
    [DllImport("yo_dll.dll")] public static extern yo_id_t yo_id_from_format_v(string format, nint args);
    [DllImport("yo_dll.dll")] public static extern void yo_push_id(yo_id_t id);
    [DllImport("yo_dll.dll")] public static extern yo_id_t yo_pop_id();
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_box(yo_id_t id, yo_box_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern yo_signal_t yo_get_signal(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern yo_recti_t yo_get_screen_rect(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern yo_rectf_t yo_get_arranged_rect(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern yo_v2f_t yo_get_content_dim(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern void yo_set_tag(string tag);
    [DllImport("yo_dll.dll")] public static extern void yo_set_text(string text);
    [DllImport("yo_dll.dll")] public static extern void yo_set_layout(yo_layout_t layout);
    [DllImport("yo_dll.dll")] public static extern void yo_set_fill(yo_v4f_t fill);
    [DllImport("yo_dll.dll")] public static extern void yo_set_border_s(yo_border_t border);
    [DllImport("yo_dll.dll")] public static extern void yo_set_border(float thickness, yo_v4f_t color, float radius);
    [DllImport("yo_dll.dll")] public static extern void yo_set_border_thickness(float thickness);
    [DllImport("yo_dll.dll")] public static extern void yo_set_border_color(yo_v4f_t color);
    [DllImport("yo_dll.dll")] public static extern void yo_set_border_radius(float radius);
    [DllImport("yo_dll.dll")] public static extern void yo_set_font(yo_font_id_t font, uint32_t size, yo_v4f_t color);
    [DllImport("yo_dll.dll")] public static extern void yo_set_font_id(yo_font_id_t font);
    [DllImport("yo_dll.dll")] public static extern void yo_set_font_size(uint32_t size);
    [DllImport("yo_dll.dll")] public static extern void yo_set_font_color(yo_v4f_t color);
    [DllImport("yo_dll.dll")] public static extern void yo_set_on_top(bool on_top);
    [DllImport("yo_dll.dll")] public static extern void yo_set_overflow_a(yo_overflow_t overflow, yo_axis_t axis);
    [DllImport("yo_dll.dll")] public static extern void yo_set_overflow_h(yo_overflow_t overflow);
    [DllImport("yo_dll.dll")] public static extern void yo_set_overflow_v(yo_overflow_t overflow);
    [DllImport("yo_dll.dll")] public static extern void yo_set_align_a(yo_align_t align, yo_axis_t axis);
    [DllImport("yo_dll.dll")] public static extern void yo_set_align_h(yo_align_t align);
    [DllImport("yo_dll.dll")] public static extern void yo_set_align_v(yo_align_t align);
    [DllImport("yo_dll.dll")] public static extern void yo_set_dim_a(yo_length_t length, yo_axis_t axis);
    [DllImport("yo_dll.dll")] public static extern void yo_set_dim_h(yo_length_t length);
    [DllImport("yo_dll.dll")] public static extern void yo_set_dim_v(yo_length_t length);
    [DllImport("yo_dll.dll")] public static extern void yo_set_margin(float left, float top, float right, float bottom);
    [DllImport("yo_dll.dll")] public static extern void yo_set_margin_hv(float h, float v);
    [DllImport("yo_dll.dll")] public static extern void yo_set_margin_a(float forward, float backward, yo_axis_t axis);
    [DllImport("yo_dll.dll")] public static extern void yo_set_margin_left(float left);
    [DllImport("yo_dll.dll")] public static extern void yo_set_margin_top(float top);
    [DllImport("yo_dll.dll")] public static extern void yo_set_margin_right(float right);
    [DllImport("yo_dll.dll")] public static extern void yo_set_margin_bottom(float bottom);
    [DllImport("yo_dll.dll")] public static extern void yo_set_padding(float left, float top, float right, float bottom);
    [DllImport("yo_dll.dll")] public static extern void yo_set_padding_hv(float h, float v);
    [DllImport("yo_dll.dll")] public static extern void yo_set_anim(yo_anim_flags_t flags, float rate);
    [DllImport("yo_dll.dll")] public static extern void yo_set_anim_flags(yo_anim_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern void yo_set_anim_rate(float rate);
    [DllImport("yo_dll.dll")] public static extern void yo_set_scroll(yo_v2f_t offset);
    [DllImport("yo_dll.dll")] public static extern void yo_set_scroll_a(float offset, yo_axis_t axis);
    [DllImport("yo_dll.dll")] public static extern void yo_set_scroll_h(float offset);
    [DllImport("yo_dll.dll")] public static extern void yo_set_scroll_v(float offset);
    [DllImport("yo_dll.dll")] public static extern yo_text_field_state_t* yo_get_text_field_state();
    [DllImport("yo_dll.dll")] public static extern void* yo_get_userdata(size_t size);
    [DllImport("yo_dll.dll")] public static extern void yo_begin_children();
    [DllImport("yo_dll.dll")] public static extern void yo_begin_children_of(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern void yo_end_children();
    [DllImport("yo_dll.dll")] public static extern void yo_open_popup(yo_id_t id, yo_popup_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern bool yo_begin_popup(yo_id_t id);
    [DllImport("yo_dll.dll")] public static extern void yo_end_popup();
    [DllImport("yo_dll.dll")] public static extern void yo_scaled_triangle(yo_v2f_t p0, yo_v2f_t p1, yo_v2f_t p2, yo_v4f_t color0, yo_v4f_t color1, yo_v4f_t color2);
    [DllImport("yo_dll.dll")] public static extern void yo_scaled_checkmark(yo_v4f_t color);
    [DllImport("yo_dll.dll")] public static extern yo_font_id_t yo_font_load(void* data, size_t data_size);
    [DllImport("yo_dll.dll")] public static extern void yo_font_unload(yo_font_id_t font);
    [DllImport("yo_dll.dll")] public static extern void* yo_alloc_temp(size_t size);
    [DllImport("yo_dll.dll")] public static extern string yo_alloc_temp_string(string format, __arglist);
    [DllImport("yo_dll.dll")] public static extern string yo_alloc_temp_string_v(string format, nint args);
    [DllImport("yo_dll.dll")] public static extern yo_error_t yo_get_error();
    [DllImport("yo_dll.dll")] public static extern string yo_get_error_message();
    [DllImport("yo_dll.dll")] public static extern void yo_debug_show_atlas_partitions();
    [DllImport("yo_dll.dll")] public static extern void yo_debug_show_atlas_texture();
    public enum yo_circle_flags_t : uint32_t
    {
        YO_CIRCLE_NONE = 0,
        YO_CIRCLE_BORDER_ADDS_DIAMETER = 1,
    }
    public enum yo_table_falgs_t : uint32_t
    {
        YO_TABLE_FLAG_NONE,
        YO_TABLE_FLAG_NO_HEADERS = 1,
    }
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_rectangle(yo_v4f_t fill, yo_length_t width, yo_length_t height, yo_id_t id);
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_fill(yo_v4f_t fill);
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_circle(yo_id_t id, float diameter, yo_v4f_t fill, uint32_t border_thickness, yo_v4f_t border_color, yo_circle_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern void yo_space_a(yo_length_t amount, yo_axis_t axis);
    [DllImport("yo_dll.dll")] public static extern void yo_space_h(yo_length_t amount);
    [DllImport("yo_dll.dll")] public static extern void yo_space_v(yo_length_t amount);
    [DllImport("yo_dll.dll")] public static extern void yo_layout_a(yo_axis_t axis);
    [DllImport("yo_dll.dll")] public static extern void yo_layout_h();
    [DllImport("yo_dll.dll")] public static extern void yo_layout_v();
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_text(string text);
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_format_text(string format, __arglist);
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_format_text_v(string format, nint args);
    [DllImport("yo_dll.dll")] public static extern void yo_text_field(yo_id_t id, string buffer, size_t buffer_size);
    [DllImport("yo_dll.dll")] public static extern yo_signal_t yo_button(string text);
    [DllImport("yo_dll.dll")] public static extern bool yo_checkbox(string label, bool* is_checked);
    [DllImport("yo_dll.dll")] public static extern bool yo_radio(string label, uint32_t index, uint32_t* selected_index);
    [DllImport("yo_dll.dll")] public static extern void yo_bullet_item(string label);
    [DllImport("yo_dll.dll")] public static extern void yo_slider(yo_id_t id, float* value, float min, float max);
    [DllImport("yo_dll.dll")] public static extern void yo_slider_with_label(string label, float* value, float min, float max);
    [DllImport("yo_dll.dll")] public static extern yo_signal_t yo_slider_behaviour(float* value, float min, float max, yo_axis_t axis, uint32_t thumb_dim, yo_box_t* bounding_box);
    [DllImport("yo_dll.dll")] public static extern bool yo_menu_begin(string label);
    [DllImport("yo_dll.dll")] public static extern bool yo_menu_begin_ex(string label, yo_v4f_t bg);
    [DllImport("yo_dll.dll")] public static extern void yo_menu_end();
    [DllImport("yo_dll.dll")] public static extern yo_signal_t yo_menu_item(string label);
    [DllImport("yo_dll.dll")] public static extern void yo_menu_separator();
    [DllImport("yo_dll.dll")] public static extern void yo_begin_scroll_area(yo_id_t id);
    [DllImport("yo_dll.dll")] public static extern void yo_begin_scroll_area_ex(yo_id_t id, float scroll_rate, float anim_rate);
    [DllImport("yo_dll.dll")] public static extern void yo_end_scroll_area();
    [DllImport("yo_dll.dll")] public static extern void yo_table_def(yo_table_t* table, uint32_t col_count, yo_table_falgs_t flags);
    [DllImport("yo_dll.dll")] public static extern void yo_table_def_col(yo_table_t* table, string header, yo_length_t width);
    [DllImport("yo_dll.dll")] public static extern void yo_table_begin(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_begin_row(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_end(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_end_row(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_cell(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_cell_text(yo_table_t* table, string text);
    [DllImport("yo_dll.dll")] public static extern void yo_table_cell_format_text_v(yo_table_t* table, string format, nint args);
    [DllImport("yo_dll.dll")] public static extern void yo_table_cell_format_text(yo_table_t* table, string format, __arglist);
    [DllImport("yo_dll.dll")] public static extern void yo_table_cell_checkbox(yo_table_t* table, string label, bool* is_checked);
    [DllImport("yo_dll.dll")] public static extern void yo_demo();
    [DllImport("yo_dll.dll")] public static extern yo_impl_win32_opengl_t* yo_impl_win32_opengl_alloc();
    [DllImport("yo_dll.dll")] public static extern void yo_impl_win32_opengl_free(yo_impl_win32_opengl_t* platform);
    [DllImport("yo_dll.dll")] public static extern void yo_impl_win32_opengl_startup(yo_impl_win32_opengl_t* platform, uint32_t window_width, uint32_t window_heigth);
    [DllImport("yo_dll.dll")] public static extern void yo_impl_win32_opengl_shutdown(yo_impl_win32_opengl_t* platform);
    [DllImport("yo_dll.dll")] public static extern void yo_impl_win32_opengl_begin_frame(yo_impl_win32_opengl_t* platform);
    [DllImport("yo_dll.dll")] public static extern void yo_impl_win32_opengl_end_frame(yo_impl_win32_opengl_t* platform);
}
