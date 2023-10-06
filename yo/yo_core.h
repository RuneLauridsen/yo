#pragma once
#pragma warning ( disable : 4201 ) // C4201: nonstandard extension used: nameless struct/union

#ifndef YO_API
#ifdef __cplusplus
#define YO_API extern "C"
#else
#define YO_API
#endif
#endif

////////////////////////////////////////////////////////////////
//
//
// Macros
//
//
////////////////////////////////////////////////////////////////

#define YO_UNUSED(...)                  (void)(__VA_ARGS__)
#if 0
#define YO_ASSERT(expression)           assert(expression)
#else
#define YO_ASSERT(expression)           if(!(expression)) { __debugbreak(); }
#endif
#define YO_PRINTF_FORMAT_STRING         _Printf_format_string_
#define yo_range32(it, min, max)        uint32_t it = (min); it <= (max); it++
#define YO_DEFER_LOOP__(begin, end, i)  for(int _defer##i##_ = ((begin), 0); _defer##i##_ == 0; _defer##i##_ += 1, (end))
#define YO_DEFER_LOOP_(begin, end, i)   YO_DEFER_LOOP__(begin, end, i)
#define YO_DEFER_LOOP(begin, end)       YO_DEFER_LOOP_(begin, end, __LINE__)
#define YO_CHILD_SCOPE()                YO_DEFER_LOOP(yo_begin_children(), yo_end_children())
#define YO_ID_SCOPE(id)                 YO_DEFER_LOOP(yo_push_id(id), yo_pop_id())
#define YO_VA_ARGS(...)

#define YO_FLOAT32_MAX 3.402823466e+38f

////////////////////////////////////////////////////////////////
//
//
// Types
//
//
////////////////////////////////////////////////////////////////

typedef uint64_t yo_id_t;
typedef yo_v4f_t yo_color_t;

typedef uint32_t yo_error_t;
enum yo_error
{
    YO_ERROR_NONE,

    // TODO(rune): Cleanup error messages.
    YO_ERROR_MEMORY_ALLOCATION_FAILED,
    YO_ERROR_OUT_OF_PERSITENT_MEMORY,
    YO_ERROR_OUT_OF_TEMPORARY_MEMORY,

    YO_ERROR_PARENT_STACK_UNDERFLOW,   // NOTE(rune): Mismatched yo_begin_children()/yo_end_children() calls.
    YO_ERROR_ID_COLLISION,             // NOTE(rune): key passed yo_box() was already used this frame

    YO_ERROR_COUNT,
};

typedef uint32_t yo_axis_t;
enum yo_axis
{
    YO_AXIS_X,
    YO_AXIS_Y,

    YO_AXIS_COUNT,
};


// TODO(rune): Better explanations
// TODO(rune): Some kind of wrapping layout like CSS flexbox

// NOTE(rune): Determines how yo_box_t arranges its children.
typedef uint32_t yo_layout_t;
enum yo_layout
{
    // NOTE(rune): For YO_LAYOUT_NONE layout axis is not relevant.
    YO_LAYTOUT_NONE         = 0,

    // NOTE(rune): First bit determines layout axis.
    YO_LAYOUT_X             = 0,
    YO_LAYOUT_Y             = 1,

    YO_LAYOUT_STACK         = 2,

    YO_LAYOUT_STACK_X        = YO_LAYOUT_STACK|YO_LAYOUT_X,
    YO_LAYOUT_STACK_Y        = YO_LAYOUT_STACK|YO_LAYOUT_Y,

    YO_LAYOUT_COUNT,
};

typedef uint32_t yo_align_t;
enum yo_align
{
    YO_ALIGN_STRETCH,
    YO_ALIGN_CENTER,
    YO_ALIGN_FRONT,
    YO_ALIGN_BACK,

    YO_ALIGN_COUNT,

    YO_ALIGN_LEFT   = YO_ALIGN_FRONT,
    YO_ALIGN_TOP    = YO_ALIGN_FRONT,
    YO_ALIGN_RIGHT  = YO_ALIGN_BACK,
    YO_ALIGN_BOTTOM = YO_ALIGN_BACK,
};

// TODO(rune): If we don't need more than 2 overflow, it could just be a boolean.
typedef uint32_t yo_overflow_t;
enum yo_overflow
{
    YO_OVERFLOW_NONE,
    YO_OVERFLOW_SCROLL,

    YO_OVERFLOW_COUNT
};

typedef uint32_t yo_side_t;
enum yo_side
{
    YO_SIDE_LEFT,
    YO_SIDE_RIGHT,
    YO_SIDE_TOP,
    YO_SIDE_BOTTOM,

    YO_SIDE_COUNT,
};

typedef uint32_t yo_corner_t;
enum yo_corner
{
    YO_CORNER_TOP_LEFT,
    YO_CORNER_TOP_RIGHT,
    YO_CORNER_BOTTOM_LEFT,
    YO_CORNER_BOTTOM_RIGHT,

    YO_CORNER_COUNT,
};

typedef uint32_t yo_popup_flags_t;
enum yo_popup_flags
{
    YO_POPUP_FLAG_NONE,

    // NOTE(rune): If this flags is set, all sibling popups get closed when a new popup is opened.
    // This is useful within context-menu trees, when you only want a single submenu to be open at a time.
    YO_POPUP_FLAG_EXCLUSIVE = 1,
};

typedef uint32_t yo_box_flags_t;
enum yo_box_flags
{
    YO_BOX_NONE              = 0,
    YO_BOX_ACTIVATE_ON_CLICK = (1 << 0),
    YO_BOX_ACTIVATE_ON_HOLD  = (1 << 1),
    YO_BOX_HOT_ON_HOVER      = (1 << 2),
    YO_BOX_FULL_ATLAS        = (1 << 3), // NOTE(rune): Ignore uv coordinates and read full texture atlas instead.
    YO_BOX_DRAW_TEXT_CURSOR  = (1 << 4), // NOTE(rune): Render text field cursor and selection when active.
};

typedef uint32_t yo_anim_flags_t;
enum yo_anim_flags
{
    YO_ANIM_NONE             = 0,
    YO_ANIM_FILL             = (1 << 0),
    YO_ANIM_DIM              = (1 << 1),
    YO_ANIM_MARGIN           = (1 << 2),
    YO_ANIM_PADDING          = (1 << 3),
    YO_ANIM_SCROLL           = (1 << 4),
    YO_ANIM_BORDER_COLOR     = (1 << 5),
    YO_ANIM_BORDER_THICKNESS = (1 << 6),
    YO_ANIM_BORDER_RADIUS    = (1 << 7),
    YO_ANIM_FONT_COLOR       = (1 << 8),
    YO_ANIM_BORDER = YO_ANIM_BORDER_COLOR|YO_ANIM_BORDER_THICKNESS|YO_ANIM_BORDER_RADIUS
};

// TODO(rune): Why not just use flags like in yo_modifier_t?
typedef uint32_t yo_mouse_button_t;
enum yo_mouse_button
{
    YO_MOUSE_BUTTON_LEFT,
    YO_MOUSE_BUTTON_RIGHT,
    YO_MOUSE_BUTTON_MIDDLE,
    YO_MOUSE_BUTTON_COUNT,
};

typedef uint32_t yo_keycode_t;
enum yo_keycode
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

typedef uint16_t yo_modifier_t;
enum yo_modifier
{
    YO_MODIFIER_NONE,

    YO_MODIFIER_SHIFT = (1 << 0),
    YO_MODIFIER_CTLR  = (1 << 1),
    YO_MODIFIER_ALT   = (1 << 2),

    YO_MODIFIER_COUNT,
};

typedef uint32_t yo_text_flags_t;
enum yo_text_flags
{
    YO_TEXT_WRAP = 1,
};

typedef uint32_t yo_text_align_t;
enum yo_text_align
{
    YO_TEXT_ALIGN_LEFT,
    YO_TEXT_ALIGN_RIGHT,
    YO_TEXT_ALIGN_CENTER,
    YO_TEXT_ALIGN_JUSTIFY,
};

typedef uint32_t yo_frame_flags_t;
enum yo_frame_flags
{
    YO_FRAME_FLAG_NONE,
    YO_FRAME_FLAG_LAZY,
};

typedef struct yo_signal yo_signal_t;
struct yo_signal
{
    union
    {
        struct { bool left_clicked, right_clicked, middle_clicked; };
        struct { bool mouse_button_clicked[YO_MOUSE_BUTTON_COUNT]; };
        struct { bool clicked; };
    };

    bool     hovered;
    yo_v2i_t mouse_pos;  // NOTE(rune): Relative to arranged rect

    yo_keycode_t keycode;
    yo_modifier_t modifiers;

    bool is_hot;
    bool is_active;
};

////////////////////////////////////////////////////////////////
//
//
// Basic
//
//
////////////////////////////////////////////////////////////////

// TODO(rune): Should these structs be moved to yo_internal.h? Since we moved to the yo_set_xyz API,
// they aren't part of the public API anymore, but maybe they're still a nice utility to provide, if
// the user want's to build e.g. a styling system, on top af the base API. Experimentation needed.

typedef union yo_sides_u32 yo_sides_u32_t;
union yo_sides_u32
{
    struct { uint32_t left, right, top, bottom; };
    struct { float forward[2], backward[2]; };
    struct { uint32_t side[4]; };
};

typedef union yo_sides_f32 yo_sides_f32_t;
union yo_sides_f32
{
    struct { float left, top, right, bottom; };
    struct { float side[4]; };
    struct { yo_v2f_t p[2]; };
};

typedef union yo_corners_f32 yo_corners_f32_t;
union yo_corners_f32
{
    struct { float top_left, top_right, bottom_left, bottom_right; };
    struct { float corner[4]; };
};

typedef union yo_corners_u32 yo_corners_u32_t;
union yo_corners_u32
{
    struct { uint32_t top_left, top_right, bottom_left, bottom_right; };
    struct { uint32_t corner[4]; };
};

typedef union yo_corners_v4f yo_corners_v4f_t;
union yo_corners_v4f
{
    struct { yo_v4f_t top_left, top_right, bottom_left, bottom_right; };
    struct { yo_v4f_t corner[4]; };
};

typedef struct yo_border yo_border_t;
struct yo_border
{
    yo_corners_f32_t radius;
    float            thickness;
    yo_v4f_t         color;
};

////////////////////////////////////////////////////////////////
//
//
// Graphics
//
//
////////////////////////////////////////////////////////////////

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

typedef struct yo_render_info yo_render_info_t;
struct yo_render_info
{
    uint32_t w;
    uint32_t h;

    yo_draw_cmd_t *draw_cmds;
    size_t         draw_cmds_count;

    // TODO(rune): Multiple textures
    struct
    {
        uint32_t id;
        yo_v2i_t dim;
        void    *pixels;
        bool     dirty;
    } tex;

    bool lazy;
};

////////////////////////////////////////////////////////////////
//
//
// Configuration
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_config yo_config_t;
struct yo_config
{
    uint32_t popup_close_delay;
};

////////////////////////////////////////////////////////////////
//
//
// Fonts
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_font_id yo_font_id_t;
struct yo_font_id
{
    uint64_t u64;
};

////////////////////////////////////////////////////////////////
//
//
// Box
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_length yo_length_t;
struct yo_length
{
    float min, max, rel;
    bool is_rel;
};

typedef struct yo_text_field_state yo_text_field_state_t;
struct yo_text_field_state
{
    uint32_t cursor;
    uint32_t marker;
};

typedef struct yo_box     yo_box_t;
typedef struct yo_context yo_context_t;

////////////////////////////////////////////////////////////////
//
//
// Context
//
//
////////////////////////////////////////////////////////////////

YO_API yo_context_t *   yo_create_context(YO_PRINTF_FORMAT_STRING yo_config_t *config);
YO_API void             yo_destroy_context(yo_context_t *context);
YO_API void             yo_select_context(yo_context_t *context);

////////////////////////////////////////////////////////////////
//
//
// Frame
//
//
////////////////////////////////////////////////////////////////

YO_API bool             yo_begin_frame(float time, yo_frame_flags_t flags);
YO_API void             yo_end_frame(yo_render_info_t *buffer);

////////////////////////////////////////////////////////////////
//
//
// Input
//
//
////////////////////////////////////////////////////////////////

YO_API void             yo_input_clear(void);
YO_API void             yo_input_mouse_state(bool buttons[YO_MOUSE_BUTTON_COUNT], uint32_t x, uint32_t y);
YO_API void             yo_input_mouse_click(yo_mouse_button_t button, uint32_t x, uint32_t y, yo_modifier_t modifiers);
YO_API void             yo_input_scroll(float x, float y);
YO_API void             yo_input_key(yo_keycode_t key, yo_modifier_t modifiers);
YO_API void             yo_input_char(char c, yo_modifier_t modifiers);
YO_API void             yo_input_unicode(uint32_t codepoint, yo_modifier_t modifiers);

YO_API yo_v2i_t         yo_query_mouse_pos(void);
YO_API bool             yo_query_mouse_button(yo_mouse_button_t button);      // NOTE(m2dx): Was mouse button down during frame?
YO_API bool             yo_query_mouse_button_down(yo_mouse_button_t button); // NOTE(m2dx): Did mouse button change from up->down during frame?
YO_API bool             yo_query_mouse_button_up(yo_mouse_button_t button);   // NOTE(m2dx): Dis mouse button change from down->up during frame?
YO_API yo_v2f_t         yo_query_scroll(void);

// TODO(rune): Procedures for querying keyboard state.

YO_API float            yo_delta(void);

////////////////////////////////////////////////////////////////
//
//
// Id
//
//
////////////////////////////////////////////////////////////////

#define YO_ID_NONE      ((yo_id_t)(0x0000'0000'0000'0000))
#define YO_ID_ROOT      ((yo_id_t)(0xffff'ffff'ffff'ffff))
#define yo_id(...)      yo_id_from_format(__VA_ARGS__) // TODO(rune): If only 1 arg is supplied, we could just use yo_id_from_string instead, to be more efficient.

YO_API yo_id_t          yo_id_from_string(char *string);
YO_API yo_id_t          yo_id_from_format(YO_PRINTF_FORMAT_STRING char *format, ...);
YO_API yo_id_t          yo_id_from_format_v(char *format, va_list args);
YO_API void             yo_push_id(yo_id_t id);
YO_API yo_id_t          yo_pop_id(void);

////////////////////////////////////////////////////////////////
//
//
// Box
//
//
////////////////////////////////////////////////////////////////

YO_API yo_box_t *       yo_box(yo_id_t id, yo_box_flags_t flags);
YO_API yo_signal_t      yo_get_signal(yo_box_t *box);
YO_API yo_v2f_t         yo_get_screen_dim(yo_box_t *box);   // NOTE(rune): Post-layout dimensions, relative to screen top-left.
YO_API yo_rectf_t       yo_get_screen_rect(yo_box_t *box);  // NOTE(rune): Post-layout rect, relative to screen top-left.
YO_API yo_rectf_t       yo_get_layout_rect(yo_box_t *box);  // NOTE(rune): Post-layout rect, relative to parent top-left.
YO_API yo_v2f_t         yo_get_content_dim(yo_box_t *box);  // NOTE(rune): Post-layout content dimensions.
YO_API void             yo_set_tag(char *tag);
YO_API void             yo_set_text(char *text);
YO_API void             yo_set_text_align(yo_text_align_t text_align);
YO_API void             yo_set_layout(yo_layout_t layout);
YO_API void             yo_set_fill(yo_v4f_t fill);
YO_API void             yo_set_border_s(yo_border_t border);
YO_API void             yo_set_border(float thickness, yo_v4f_t color, float radius);
YO_API void             yo_set_border_thickness(float thickness);
YO_API void             yo_set_border_color(yo_v4f_t color);
YO_API void             yo_set_border_radius(float radius);
YO_API void             yo_set_font(yo_font_id_t font, uint32_t size, yo_v4f_t color);
YO_API void             yo_set_font_id(yo_font_id_t font);
YO_API void             yo_set_font_size(uint32_t size);
YO_API void             yo_set_font_color(yo_v4f_t color);
YO_API void             yo_set_on_top(bool on_top);
YO_API void             yo_set_overflow(yo_overflow_t overflow_x, yo_overflow_t overflow_y);
YO_API void             yo_set_overflow_a(yo_overflow_t overflow, yo_axis_t axis);
YO_API void             yo_set_overflow_x(yo_overflow_t overflow);
YO_API void             yo_set_overflow_y(yo_overflow_t overflow);
YO_API void             yo_set_noclip(bool noclip_x, bool noclip_y);
YO_API void             yo_set_noclip_a(bool noclip, yo_axis_t axis);
YO_API void             yo_set_noclip_x(bool noclip);
YO_API void             yo_set_noclip_y(bool noclip);
YO_API void             yo_set_align(yo_align_t align_x, yo_align_t align_y);
YO_API void             yo_set_align_a(yo_align_t align, yo_axis_t axis);
YO_API void             yo_set_align_x(yo_align_t align);
YO_API void             yo_set_align_y(yo_align_t align);
YO_API void             yo_set_dim(yo_length_t dim_x, yo_length_t dim_y);
YO_API void             yo_set_dim_a(yo_length_t dim, yo_axis_t axis);
YO_API void             yo_set_dim_x(yo_length_t dim);
YO_API void             yo_set_dim_y(yo_length_t dim);
YO_API void             yo_set_margin(float left, float top, float right, float bottom);
YO_API void             yo_set_margin_xy(float x, float y);
YO_API void             yo_set_margin_a(float front, float back, yo_axis_t axis);
YO_API void             yo_set_margin_left(float left);
YO_API void             yo_set_margin_top(float top);
YO_API void             yo_set_margin_right(float right);
YO_API void             yo_set_margin_bottom(float bottom);
YO_API void             yo_set_padding(float left, float top, float right, float bottom);
YO_API void             yo_set_padding_xy(float x, float y);
YO_API void             yo_set_anim(yo_anim_flags_t flags, float rate);
YO_API void             yo_set_anim_flags(yo_anim_flags_t flags);
YO_API void             yo_set_anim_rate(float rate);
YO_API void             yo_set_scroll(float offset_x, float offset_y);
YO_API void             yo_set_scroll_a(float offset, yo_axis_t axis);
YO_API void             yo_set_scroll_x(float offset);
YO_API void             yo_set_scroll_y(float offset);

// TODO(rune): Can we store text_field_state like normal userdata?
// Renderer needs to know about text_field_state to draw cursor+selection.
YO_API yo_text_field_state_t *  yo_get_text_field_state(void);
YO_API void *                   yo_get_userdata(size_t size);

////////////////////////////////////////////////////////////////
//
//
// Children
//
//
////////////////////////////////////////////////////////////////

YO_API void             yo_begin_children(void);
YO_API void             yo_begin_children_of(yo_box_t *box);
YO_API void             yo_end_children(void);

////////////////////////////////////////////////////////////////
//
//
// Popup
//
//
////////////////////////////////////////////////////////////////

// WARNING(rune): Not finished
YO_API void             yo_open_popup(yo_id_t id, yo_popup_flags_t flags);
YO_API bool             yo_begin_popup(yo_id_t id);
YO_API void             yo_end_popup(void);

////////////////////////////////////////////////////////////////
//
//
// Scaled
//
//
////////////////////////////////////////////////////////////////

YO_API void             yo_scaled_triangle(yo_v2f_t p0, yo_v2f_t p1, yo_v2f_t p2, yo_v4f_t color0, yo_v4f_t color1, yo_v4f_t color2);
YO_API void             yo_scaled_checkmark(yo_v4f_t color);

////////////////////////////////////////////////////////////////
//
//
// Font
//
//
////////////////////////////////////////////////////////////////

YO_API yo_font_id_t     yo_font_load(void *data, size_t data_size);
YO_API void             yo_font_unload(yo_font_id_t font);

////////////////////////////////////////////////////////////////
//
//
// Memory
//
//
////////////////////////////////////////////////////////////////

YO_API void *           yo_alloc_temp(size_t size);

// NOTE(rune): Temporary string allocations return a pointer to "" instead of NULL, if the allocation fails.
YO_API char *           yo_alloc_temp_string(YO_PRINTF_FORMAT_STRING char *format, ...);
YO_API char *           yo_alloc_temp_string_v(YO_PRINTF_FORMAT_STRING char *format, va_list args);

////////////////////////////////////////////////////////////////
//
//
// Errors
//
//
////////////////////////////////////////////////////////////////

YO_API yo_error_t       yo_get_error(void);
YO_API char *           yo_get_error_message(void);

////////////////////////////////////////////////////////////////
//
//
// Debug
//
//
////////////////////////////////////////////////////////////////

YO_API void             yo_debug_show_atlas_partitions(void);
YO_API void             yo_debug_show_atlas_texture(void);

////////////////////////////////////////////////////////////////
//
//
// Color
//
//
////////////////////////////////////////////////////////////////

#define YO_TRANSPARENT   (yo_argb(0x00, 0x00, 0x00, 0x00))
#define YO_WHITE         (yo_argb(0xff, 0xff, 0xff, 0xff))
#define YO_BLACK         (yo_argb(0xff, 0x00, 0x00, 0x00))
#define YO_RED           (yo_argb(0xff, 0xff, 0x00, 0x00))
#define YO_GREEN         (yo_argb(0xff, 0x00, 0xff, 0x00))
#define YO_BLUE          (yo_argb(0xff, 0x00, 0x00, 0xff))
#define YO_YELLOW        (yo_argb(0xff, 0xff, 0xff, 0x00))
#define YO_CYAN          (yo_argb(0xff, 0x00, 0xff, 0xff))
#define YO_MAGENTA       (yo_argb(0xff, 0xff, 0x00, 0xff))
#define YO_GRAY          (yo_argb(0xff, 0x80, 0x80, 0x80))
#define YO_ORANGE        (yo_argb(0xff, 0xff, 0xa5, 0x00))

////////////////////////////////////////////////////////////////
//
//
// Units
//
//
////////////////////////////////////////////////////////////////

static yo_length_t yo_px(float pixels)
{
    yo_length_t ret =
    {
        .min    = pixels,
        .max    = pixels,
        .rel    = 0.0f,
        .is_rel = false,
    };

    return ret;
}

static yo_length_t yo_rel(float rel)
{
    yo_length_t ret =
    {
        .min    = 0,
        .max    = YO_FLOAT32_MAX,
        .rel    = rel,
        .is_rel = true,
    };

    return ret;
}

////////////////////////////////////////////////////////////////
//
//
// Simple constructors
//
//
////////////////////////////////////////////////////////////////

static inline yo_corners_f32_t yo_corners(float top_left, float top_right, float bottom_left, float bottom_right)
{
    yo_corners_f32_t ret =
    {
        .top_left      = top_left,
        .top_right     = top_right,
        .bottom_left   = bottom_left,
        .bottom_right  = bottom_right,
    };
    return ret;
}

static inline yo_sides_f32_t yo_margin(float left, float top, float right, float bottom)
{
    yo_sides_f32_t ret =
    {
        .left   = left,
        .right  = right,
        .top    = top,
        .bottom = bottom,
    };
    return ret;
}

static inline yo_sides_f32_t yo_margin_xy(float x, float y)
{
    yo_sides_f32_t ret = yo_margin(x, y, x, y);
    return ret;
}

static inline yo_sides_f32_t yo_margin_all(float all_sides)
{
    yo_sides_f32_t ret = yo_margin(all_sides, all_sides, all_sides, all_sides);
    return ret;
}

static inline yo_sides_f32_t yo_padding(float left, float top, float right, float bottom)
{
    yo_sides_f32_t ret =
    {
        .left   = left,
        .right  = right,
        .top    = top,
        .bottom = bottom,
    };
    return ret;
}

static inline yo_sides_f32_t yo_padding_xy(float x, float y)
{
    yo_sides_f32_t ret = yo_padding(x, y, x, y);
    return ret;
}

static inline yo_sides_f32_t yo_padding_all(float all_sides)
{
    yo_sides_f32_t ret = yo_padding(all_sides, all_sides, all_sides, all_sides);
    return ret;
}

#define YO_BORDER_NONE (yo_border_t){ 0 }

static inline yo_border_t yo_border(float thickness, yo_v4f_t color, float radius)
{
    yo_border_t ret =
    {
        .radius    = { radius, radius, radius, radius },
        .thickness = thickness,
        .color     = color,
    };
    return ret;
}
