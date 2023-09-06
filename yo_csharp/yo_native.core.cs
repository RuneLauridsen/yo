using System.Runtime.InteropServices;

namespace Yo;

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

    ////////////////////////////////////////////////////////////////
    //
    //
    // Basic
    //
    //
    ////////////////////////////////////////////////////////////////

    public struct yo_sides_u32_t { public uint32_t left, right, top, bottom; };
    public struct yo_sides_f32_t { public float left, right, top, bottom; };
    public struct yo_corners_u32_t { public uint32_t top_left, top_right, bottom_left, bottom_right; };
    public struct yo_corners_f32_t { public float top_left, top_right, bottom_left, bottom_right; };
    public struct yo_corners_v4f_t { public yo_v4f_t top_left, top_right, bottom_left, bottom_right; };

    ////////////////////////////////////////////////////////////////
    //
    //
    // Graphics
    //
    //
    ////////////////////////////////////////////////////////////////

#if false
    typedef uint32_t yo_draw_cmd_type_t;
    enum yo_draw_cmd_type
    {
        YO_DRAW_CMD_NONE,
        YO_DRAW_CMD_AABB,
        YO_DRAW_CMD_TRI,
        YO_DRAW_CMD_QUAD,
    };

    // TODO(rune): Consider making this variably sized. Currently we waste a lot of space for YO_DRAW_CMD_TRI.
    // TODO(rune): This seems very fat for a basic draw_cmd struct.
    typedef struct yo_draw_cmd yo_draw_cmd_t;
    struct yo_draw_cmd
    {
        yo_draw_cmd_type_t type;
        union
        {
            struct
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
            } aabb;

            struct
            {
                yo_v2f_t p[3];
                yo_v4f_t color[3];
            } tri;

            struct
            {
                yo_v2f_t p[4];
                yo_v4f_t color[4];
            } quad;
        };
    };

    typedef struct yo_vert yo_vert_t;
    struct yo_vert
    {
        yo_v2f_t rect_p0;
        yo_v2f_t rect_p1;

        yo_v2f_t corner;
        yo_v4f_t corner_color;
        float    corner_radius;

        float    border_thickness;
        yo_v4f_t border_color;

        yo_v2f_t tex_coord;
        float    z;
    };

    typedef uint32_t yo_idx_t;

#endif
    public struct yo_draw_cmd_t
    {
        // TODO(rune): How to unions in C#?
    };

    public struct yo_render_info_t
    {
        public uint32_t w;
        public uint32_t h;

        public yo_draw_cmd_t* draw_cmds;
        public size_t draw_cmds_count;

        public uint32_t tex_id;
        public yo_v2i_t tex_dims;
        public void* tex_pixels;
        public bool tex_dirty;

        public bool lazy;
    };


    ////////////////////////////////////////////////////////////////
    //
    //
    // Configuration
    //
    //
    ////////////////////////////////////////////////////////////////

    public struct yo_config_t
    {
        public uint32_t popup_close_delay;
    };

    ////////////////////////////////////////////////////////////////
    //
    //
    // Box
    //
    //
    ////////////////////////////////////////////////////////////////

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

    public struct yo_placement_t
    {
        public yo_length_t h_dim, v_dim;
        public yo_align_t h_align, v_align;
        public yo_sides_f32_t margin, padding;
    };

    public struct yo_border_t
    {
        public yo_corners_f32_t radius;
        public float thickness;
        public yo_v4f_t color;
    };

    public struct yo_box_t
    {
        public byte* tag;   // NOTE(rune): Null terminated ASCII string.
        public byte* text;  // NOTE(rune): Null terminated ASCII string.

        public yo_layout_t child_layout;
        public yo_border_t border;
        public yo_v4f_t fill;
        public uint32_t font_size;
        public yo_v4f_t font_color;
        public bool on_top;

#if false
        public yo_placement_t placement;
#else
        public yo_length_t h_dim, v_dim;
        public yo_align_t h_align, v_align;
        public yo_sides_f32_t margin, padding;
#endif
        public yo_overflow_t h_overflow, v_overflow;

        public yo_v2f_t target_scroll_offset;
        public yo_v2f_t scroll_offset;

        // NOTE(rune): Controls which properties are animated.
        public yo_anim_flags_t anim;

        // NOTE(rune): All animations play at the same rate. If you want different animation rates,
        // for different properties, you need to split them up into multiple boxes.
        public float anim_rate;

        // TODO(rune): We could just make size of the persistent state user defined, which would allow the
        // user code could store anything across frame.
        public yo_text_field_state_t text_field_state;
    };

    ////////////////////////////////////////////////////////////////
    //
    //
    // Context
    //
    //
    ////////////////////////////////////////////////////////////////
    ///
    public struct yo_context_t { }

    [DllImport("yo_dll.dll")] public static extern yo_context_t* yo_create_context(yo_config_t* config);
    [DllImport("yo_dll.dll")] public static extern void yo_destroy_context(yo_context_t* context);
    [DllImport("yo_dll.dll")] public static extern void yo_select_context(yo_context_t* context);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Frame
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern bool yo_begin_frame(float time, yo_frame_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern void yo_end_frame(yo_render_info_t* buffer);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Input
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern void yo_input_begin();
    // [DllImport("yo_dll.dll")] public static extern void yo_input_mouse_state(bool buttons[YO_MOUSE_BUTTON_COUNT], uint32_t x, uint32_t y); TODO(rune): How to port statically sized bool arrays?
    [DllImport("yo_dll.dll")] public static extern void yo_input_mouse_click(yo_mouse_button_t button, uint32_t x, uint32_t y, yo_modifier_t modifiers);
    [DllImport("yo_dll.dll")] public static extern void yo_input_scroll(float x, float y);
    [DllImport("yo_dll.dll")] public static extern void yo_input_key(yo_keycode_t key, yo_modifier_t modifiers);
    [DllImport("yo_dll.dll")] public static extern void yo_input_char(char c, yo_modifier_t modifiers);
    [DllImport("yo_dll.dll")] public static extern void yo_input_unicode(uint32_t codepoint, yo_modifier_t modifiers);
    [DllImport("yo_dll.dll")] public static extern void yo_input_end();
    [DllImport("yo_dll.dll")] public static extern yo_v2i_t yo_query_mouse_pos();
    [DllImport("yo_dll.dll")] public static extern bool yo_query_mouse_button(yo_mouse_button_t button);      // NOTE(m2dx): Was mouse button down during frame?
    [DllImport("yo_dll.dll")] public static extern bool yo_query_mouse_button_down(yo_mouse_button_t button); // NOTE(m2dx): Did mouse button change from up->down during frame?
    [DllImport("yo_dll.dll")] public static extern bool yo_query_mouse_button_up(yo_mouse_button_t button);   // NOTE(m2dx): Dis mouse button change from down->up during frame?
    [DllImport("yo_dll.dll")] public static extern yo_v2f_t yo_query_scroll();

    // TODO(rune): Procedures for querying keyboard state.

    [DllImport("yo_dll.dll")] public static extern float yo_delta();

    ////////////////////////////////////////////////////////////////
    //
    //
    // Id
    //
    //
    ////////////////////////////////////////////////////////////////

    /*
#define YO_ID_NONE      ((yo_id_t)(0x0000'0000'0000'0000))
#define YO_ID_ROOT      ((yo_id_t)(0xffff'ffff'ffff'ffff))
#define yo_id(...)      yo_id_from_format(__VA_ARGS__) // TODO(rune): If only 1 arg is supplied, we could just use yo_id_from_string instead, to be more efficient.
    */

    public const yo_id_t YO_ID_NONE = 0;
    public const yo_id_t YO_ID_ROOT = 0xffff_ffff_ffff_ffff;

    // TODO(rune): How to port yo_id(...) ???

    [DllImport("yo_dll.dll")] public static extern yo_id_t yo_id_from_string(char* string_);
    [DllImport("yo_dll.dll")] public static extern yo_id_t yo_id_from_format(char* format, __arglist);
    // [DllImport("yo_dll.dll")] public static extern yo_id_t yo_id_from_format_v(char* format, va_list args);
    [DllImport("yo_dll.dll")] public static extern void yo_push_id(yo_id_t id);
    [DllImport("yo_dll.dll")] public static extern yo_id_t yo_pop_id();


    ////////////////////////////////////////////////////////////////
    //
    //
    // Box
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_box(yo_id_t id, yo_box_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern yo_signal_t yo_get_signal(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern yo_recti_t yo_get_screen_rect(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern yo_rectf_t yo_get_arranged_rect(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern yo_v2f_t yo_get_content_dim(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_new();

    ////////////////////////////////////////////////////////////////
    //
    //
    // Children
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern void yo_begin_children();
    [DllImport("yo_dll.dll")] public static extern void yo_begin_children_of(yo_box_t* box);
    [DllImport("yo_dll.dll")] public static extern void yo_end_children();

    ////////////////////////////////////////////////////////////////
    //
    //
    // Popup
    //
    //
    ////////////////////////////////////////////////////////////////

    // WARNING(rune): Not finished
    [DllImport("yo_dll.dll")] public static extern void yo_open_popup(yo_id_t id, yo_popup_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern bool yo_begin_popup(yo_id_t id);
    [DllImport("yo_dll.dll")] public static extern void yo_end_popup();

    ////////////////////////////////////////////////////////////////
    //
    //
    // Scaled
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern void yo_scaled_triangle(yo_v2f_t p0, yo_v2f_t p1, yo_v2f_t p2, yo_v4f_t color0, yo_v4f_t color1, yo_v4f_t color2);
    [DllImport("yo_dll.dll")] public static extern void yo_scaled_checkmark(yo_v4f_t color);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Memory
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern void* yo_alloc_temp(size_t size);

    // NOTE(rune): Temporary string allocations return a pointer to "" instead of NULL, if the allocation fails.
    [DllImport("yo_dll.dll")] public static extern char* yo_alloc_temp_string(char* format, __arglist);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Errors
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern yo_error_t yo_get_error();
    [DllImport("yo_dll.dll")] public static extern char* yo_get_error_message();

    ////////////////////////////////////////////////////////////////
    //
    //
    // Debug
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern void yo_debug_show_atlas_partitions();
    [DllImport("yo_dll.dll")] public static extern void yo_debug_show_atlas_texture();
}
