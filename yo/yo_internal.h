#pragma once

////////////////////////////////////////////////////////////////
//
//
// Macros
//
//
////////////////////////////////////////////////////////////////

#define fallthrough                 // NOTE(rune): empty macro to mark intentional fall through in switch cases
#define countof(x)                  (sizeof(x) / sizeof(x[0]))

#define YO_KILOBYTES(x)             ((x) * 1024LL)
#define YO_MEGABYTES(x)             (YO_KILOBYTES(x) * 1024LL)
#define YO_GIGABYTES(x)             (YO_MEGABYTES(x) * 1024LL)

// TODO(rune): Make this a parameter in yo_config_t
#define YO_BOX_CACHE_COUNT          (256)

#define YO_HASH(x)                  (((x) * 42727) ^ ((x) * 86197))

#define YO_COALESCE(a,b)            ((a)?(a):(b))
#define YO_ZERO(T)                  ((T){0})

////////////////////////////////////////////////////////////////
//
//
// Types
//
//
////////////////////////////////////////////////////////////////

typedef uint16_t yo_event_type_t;
enum yo_event_type
{
    YO_EVENT_TYPE_NONE,
    YO_EVENT_TYPE_MOUSE_CLICK_LEFT,
    YO_EVENT_TYPE_MOUSE_CLICK_RIGHT,
    YO_EVENT_TYPE_MOUSE_CLICK_MIDDLE,
    YO_EVENT_TYPE_KEY_PRESS,
};

typedef struct yo_event yo_event_t;
struct yo_event
{
    yo_event_type_t type;
    yo_modifier_t modifiers;
    union
    {
        struct { yo_v2f_t pos; }; // TODO(rune): Modifiers on mouse too
        struct { yo_keycode_t keycode; };
    };
};

typedef struct yo_perf_timing yo_perf_timing_t;
struct yo_perf_timing
{
    uint64_t counterBegin;
    uint64_t counterEnd;
    double millis;
};

typedef struct yo_measure_text_result yo_measure_text_result_t;
struct yo_measure_text_result
{
    yo_v2f_t rect; // TODO(rune): Rename to dim

    int32_t ascent;
    int32_t descent;
    int32_t line_gap;
};

typedef uint32_t yo_scaled_element_type_t;
enum
{
    YO_SCALED_ELEMENT_TYPE_NONE,
    YO_SCALED_ELEMENT_TYPE_TRIANGLE,
    YO_SCALED_ELEMENT_TYPE_COUNT
};

typedef struct yo_scaled_triangle yo_scaled_triangle_t;
struct yo_scaled_triangle
{
    union
    {
        struct { yo_v2f_t p0, p1, p2; };
        struct { yo_v2f_t p[3]; };
    };
    union
    {
        struct { yo_v4f_t color0, color1, color2; };
        struct { yo_v4f_t color[3]; };
    };
};

typedef struct yo_scaled_element yo_scaled_element_t;
struct yo_scaled_element
{
    yo_scaled_element_t *next;
    yo_scaled_element_type_t type;
    union
    {
        yo_scaled_triangle_t triangle;
    };
};

typedef struct yo_box_internal yo_internal_box_t;
struct yo_box_internal
{
    // WARNING(rune):
    // Must be first field in yo_box_internal_t, so that we can safely cast from (yo_box_t *) to (yo_box_internal_t *).
    union { struct yo_box base; struct yo_box; };

    yo_id_t id;
    yo_box_flags_t flags;

    // NOTE(rune): Hierarchy
    yo_internal_box_t *parent;
    yo_internal_box_t *next;
    yo_slist(yo_internal_box_t) children;

    // NOTE(rune): Scaled draw commands, whose sizes are calculated after the measure + arrange pass.
    yo_scaled_element_t *scaled_elements;

    // NOTE(rune): Hashtable
    yo_internal_box_t *next_hash;

    // NOTE(rune): Calculated during measure pass
    union
    {
        struct
        {
            float w;
            float h;
        };
        struct
        {
            float h_dim;
            float v_dim;
        };
        float axis[2];
    } content_size; // TODO(rune): Use yo_v2f_t
    yo_measure_text_result_t measured_text;

    // NOTE(rune): Calculated during arrange pass
    bool arranged;
    yo_rectf_t arranged_rect; // NOTE(rune): Relative to parent top-left
    yo_rectf_t screen_rect;   // NOTE(rune): Relative to screen top-left
};

typedef struct yo_popup yo_popup_t;
struct yo_popup
{
    yo_id_t id;
    yo_id_t group_id;
    bool got_click;
    bool got_hover;
    bool is_building;
    uint64_t close_on_frame_count;
};

YO_TYPEDEF_ARRAY(yo_event_t);

typedef struct yo_frame yo_frame_t;
struct yo_frame
{
    yo_arena_t arena;
    yo_internal_box_t *hash_table[YO_BOX_CACHE_COUNT]; // TODO(rune): Dynamic hash table size
    yo_id_t active_id;
    yo_id_t hot_id;
    bool played_anim;

    yo_array(yo_event_t) events;
    yo_v2f_t scroll;
    yo_v2f_t mouse_pos;
    bool mouse_button[YO_MOUSE_BUTTON_COUNT];       // NOTE(m2dx): Was mouse button down during frame?
    bool mouse_button_down[YO_MOUSE_BUTTON_COUNT];  // NOTE(m2dx): Did mouse button change from up->down during frame?
    bool mouse_button_up[YO_MOUSE_BUTTON_COUNT];    // NOTE(m2dx): Dis mouse button change from down->up during frame?

    // TODO(rune): Probably not a good idea to store time in floating point. Delta is OK as floating point.
    float time;
    float delta;

    bool lazy;
};

typedef yo_internal_box_t *yo_internal_box_ptr_t;

YO_TYPEDEF_ARRAY(yo_internal_box_ptr_t);
YO_TYPEDEF_ARRAY(yo_popup_t);
YO_TYPEDEF_ARRAY(yo_draw_cmd_t);
YO_TYPEDEF_ARRAY(yo_id_t);

typedef struct yo_context yo_context_t;
struct yo_context
{
    yo_arena_t  persist;
    yo_array(yo_draw_cmd_t) draw_cmds;

    // NOTE(m2dx): Frame state pointers are swapped every frame, similar to a gpu swap chain works.
    // Every frame persistent state pr. box is copied from prev_frame to this_frame, and animation
    // state is interpolated between prev_frame and this_frame.
    yo_frame_t frame_states[2];
    yo_frame_t *this_frame;
    yo_frame_t *prev_frame;

    // Data for building current frame hierarchy
    yo_internal_box_t *root;
    yo_internal_box_t *latest_child;
    yo_array(yo_internal_box_ptr_t) parent_stack;
    yo_array(yo_id_t)  id_stack;

    // Popups
    yo_array(yo_popup_t) popups;            // NOTE(rune): Tracks yo_open_popup calls across frames.
    yo_array(yo_id_t)    popup_build_stack; // NOTE(rune): Tracks yo_begin_popup/yo_end_popup calls. Cleared every frame.

    // Fonts
    yo_atlas_t  atlas;

    uint64_t    frame_count;
    yo_config_t config;
    yo_error_t  error;

    struct yo_perf_timing_set { yo_perf_timing_t build, render, measure, arrange, prune; } timings[100];

    uint32_t timings_index;

};

////////////////////////////////
//
// Performance timing
//
////////////////////////////////

static inline uint64_t yo_get_performance_counter(void)
{
    uint64_t a = 0;
    QueryPerformanceCounter((LARGE_INTEGER *)&a);
    return a;
}

static inline uint64_t yo_get_performance_frequency(void)
{
    uint64_t a = 0;
    QueryPerformanceFrequency((LARGE_INTEGER *)&a);
    return a;
}

static inline void yo_begin_performance_timing(yo_perf_timing_t *timing)
{
    timing->counterBegin = yo_get_performance_counter();
}

static inline void yo_end_performance_timing(yo_perf_timing_t *timing)
{
    timing->counterEnd = yo_get_performance_counter();

    uint64_t frequency = yo_get_performance_frequency();
    timing->millis = 1000.0 * (double)(timing->counterEnd - timing->counterBegin) / (double)(frequency);
}
