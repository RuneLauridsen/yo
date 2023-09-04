#pragma once

////////////////////////////////////////////////////////////////
//
//
// Common win32 code for yo_impl_win32_x.
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_platform_win32 yo_platform_win32_t;
struct yo_platform_win32
{
    bool                 running;
    HWND                 window;
    HDC                  dc;
    yo_context_t        *ctx;              // TODO(rune): Move yo_context_t out of platform layer, and into user code instead.
    yo_render_info_t     render_info;
    uint64_t             tick_count_start; // TODO(rune): tick_start
    uint64_t             tick_current;
    uint64_t             tick_freq;
    HCURSOR              cursor;
};

static void yo_platform_win32_startup(yo_platform_win32_t *platform, uint32_t window_width, uint32_t window_heigth);
static void yo_platform_win32_shutdown(yo_platform_win32_t *platform);
static void yo_platform_win32_begin_frame(yo_platform_win32_t *platform);
static void yo_platform_win32_end_frame(yo_platform_win32_t *platform);
