using System.Diagnostics;
using System.Runtime.InteropServices;

namespace YoOld;

public unsafe static partial class yo_native
{
    public enum yo_error_t : uint32_t
    {
        YO_ERROR_NONE,

        YO_ERROR_MEMORY_ALLOCATION_FAILED,
        YO_ERROR_OUT_OF_PERSITENT_MEMORY,
        YO_ERROR_OUT_OF_TEMPORARY_MEMORY,

        YO_ERROR_PARENT_STACK_UNDERFLOW,    // Mismatched yo_begin_children/yo_end_children calls.
        YO_ERROR_STYLE_STACK_UNDERFLOW,     // Mismatched yo_PushStyle/yo_PopStyle calls

        YO_ERROR_ID_COLLISION,             // key passed yo_Box was already used this frame

        YO_ERROR_COUNT,
    };

    public enum yo_axis_t : uint32_t
    {
        YO_AXIS_X,
        YO_AXIS_Y,

        YO_AXIS_COUNT,
    };


    // TODO(rune): Better explanations
    // TODO(rune): Some kind of wrapping layout like CSS flexbox
    // Determines how yo_box_node arranges its children
    public enum yo_layout_t : uint32_t
    {
        // NOTE(rune): Children take up as much of the parent space as possible.
        // Relative sizes are relative to parent size.
        YO_LAYTOUT_NONE,

        // NOTE(rune): Children are stacked horizontally.
        // Relative horizontal size is relative to remaining horizontal space no taken by absolute sized children.
        // Relative vertical size is relative to parent vertical size.
        YO_LAYOUT_HORIZONTAL,

        // NOTE(rune): Children are stacked vertically.
        // Relative horizontal size is relative to parent horizontal size.
        // Relative vertical size is relative to remaining vertical space no taken by absolute sized children.
        YO_LAYOUT_VERTICAL,

        YO_LAYOUT_TABLE_COLUMN,

        YO_LAYOUT_COUNT,
    };

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
    };

    public enum yo_overflow_t : uint32_t
    {
        // NOTE(rune): If content is larger than maximum width/height, the box is expanded
        // to fits its contents.
        YO_OVERFLOW_FIT,

        // NOTE(rune): If content is larger than maximum width/height, the content will
        // "spill" outside the box and potentially overlap with other boxes.
        YO_OVERFLOW_SPILL,

        // NOTE(rune): If content is larger than maximum width/height, the content is
        // clipped to the maximum width/height.
        YO_OVERFLOW_CLIP,

        // TODO(rune): Could YO_OVERFLOW_SCROLL more generalized are removed entirely?
        YO_OVERFLOW_SCROLL,

        YO_OVERFLOW_COUNT
    };

    public enum yo_side_t : uint32_t
    {
        YO_SIDE_LEFT,
        YO_SIDE_RIGHT,
        YO_SIDE_TOP,
        YO_SIDE_BOTTOM,

        YO_SIDE_COUNT,
    };

    public enum yo_corner_t : uint32_t
    {
        YO_CORNER_TOP_LEFT,
        YO_CORNER_TOP_RIGHT,
        YO_CORNER_BOTTOM_LEFT,
        YO_CORNER_BOTTOM_RIGHT,

        YO_CORNER_COUNT,
    };

    public enum yo_popup_flags_t : uint32_t
    {
        YO_POPUP_FLAG_NONE,

        // NOTE(rune): If this flags is set, all sibling popups get closed when a new popup is opened.
        // This is useful within context-menu trees, when you only want a single submenu to be open at a time.
        YO_POPUP_FLAG_EXCLUSIVE = 1,
    };

    public enum yo_box_flags_t : uint32_t
    {
        YO_BOX_NONE = 0,
        YO_BOX_ACTIVATE_ON_CLICK = (1 << 0),
        YO_BOX_ACTIVATE_ON_HOLD = (1 << 1),
        YO_BOX_HOT_ON_HOVER = (1 << 2),
        YO_BOX_FULL_ATLAS = (1 << 3), // NOTE(rune): Ignore uv coordinates and read full texture atlas instead.
        YO_BOX_DRAW_TEXT_CURSOR = (1 << 4), // NOTE(rune): Render text field cursor and selection when active.
    };

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
        YO_ANIM_BORDER = YO_ANIM_BORDER_COLOR | YO_ANIM_BORDER_THICKNESS | YO_ANIM_BORDER_RADIUS
    };

    public enum yo_mouse_button_t : uint32_t
    {
        YO_MOUSE_BUTTON_LEFT,
        YO_MOUSE_BUTTON_RIGHT,
        YO_MOUSE_BUTTON_MIDDLE,
        YO_MOUSE_BUTTON_COUNT,
    };

    public enum yo_keycode_t : uint32_t
    {
        YO_KEYCODE_NONE = 0,

        YO_KEYCODE_LEFT_ARROW = 128, // NOTE(rune): Reserve first 128 values to match ASCII
        YO_KEYCODE_RIGHT_ARROW,
        YO_KEYCODE_UP_ARROW,
        YO_KEYCODE_DOWN_ARROW,

        YO_KEYCODE_HOME,
        YO_KEYCODE_END,

        YO_KEYCODE_SPACE,
        YO_KEYCODE_RETURN,
        YO_KEYCODE_BACKSPACE,
        YO_KEYCODE_DELETE,
    };

    public enum yo_modifier_t : uint32_t
    {
        YO_MODIFIER_NONE,

        YO_MODIFIER_SHIFT,
        YO_MODIFIER_CTLR,
        YO_MODIFIER_ALT,

        YO_MODIFIER_COUNT,
    };

    public enum yo_frame_flags_t : uint32_t
    {
        YO_FRAME_FLAG_NONE,
        YO_FRAME_FLAG_LAZY,
    };

    ////////////////////////////////////////////////////////////////
    //
    //
    // Structs
    //
    //
    ////////////////////////////////////////////////////////////////

    public struct yo_sides_u32_t { public uint32_t left, right, top, bottom; };
    public struct yo_sides_f32_t { public float left, right, top, bottom; };
    public struct yo_corners_u32_t { public uint32_t top_left, top_right, bottom_left, bottom_right; };
    public struct yo_corners_f32_t { public float top_left, top_right, bottom_left, bottom_right; };
    public struct yo_corners_v4f_t { public yo_v4f_t top_left, top_right, bottom_left, bottom_right; };

    public struct yo_signal_t
    {
        public bool clicked; // NOTE(rune): Left clicked
        public bool right_clicked;

        public bool hovered;
        public yo_v2i_t mouse_pos;  // NOTE(rune): Relative to arranged rect

        public yo_keycode_t keycode;
        public bool is_alt_down;
        public bool is_ctrl_down;
        public bool is_shift_down;

        public bool is_hot;
        public bool is_active;
    };

    public struct yo_length_t
    {
        public float min, max, rel;
        public bool is_rel;
    };

    public struct yo_text_field_state_t
    {
        public uint32_t cursor;
        public uint32_t marker;
    };

    public struct yo_border_t
    {
        public yo_corners_f32_t radius;
        public float thickness;
        public yo_v4f_t color;
    };

    public struct yo_config_t
    {
        public uint32_t popup_close_delay;
    }

    public struct yo_font_id_t { public uint64_t u64; }

    public struct yo_context_t { }
    public struct yo_box_t { }

    [DllImport("yo_dll.dll")] public static extern yo_context_t* yo_create_context(yo_config_t* config);
    [DllImport("yo_dll.dll")] public static extern void yo_destroy_context(yo_context_t* context);
    [DllImport("yo_dll.dll")] public static extern void yo_select_context(yo_context_t* context);
    [DllImport("yo_dll.dll")] public static extern bool yo_begin_frame(float time, yo_frame_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern void yo_end_frame(yo_render_info_t* buffer);
    [DllImport("yo_dll.dll")] public static extern void yo_input_begin();
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
    [DllImport("yo_dll.dll")] public static extern yo_id_t yo_id_from_format(string format);
    [DllImport("yo_dll.dll")] public static extern yo_id_t yo_id_from_format_v(string format, va_list args);
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
    [DllImport("yo_dll.dll")] public static extern string yo_alloc_temp_string(string format);
    [DllImport("yo_dll.dll")] public static extern string yo_alloc_temp_string_v(string format, va_list args);
    [DllImport("yo_dll.dll")] public static extern yo_error_t yo_get_error();
    [DllImport("yo_dll.dll")] public static extern string yo_get_error_message();
    [DllImport("yo_dll.dll")] public static extern void yo_debug_show_atlas_partitions();
    [DllImport("yo_dll.dll")] public static extern void yo_debug_show_atlas_texture();
}
