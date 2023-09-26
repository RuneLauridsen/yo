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
    uint64_t             tick_start;
    uint64_t             tick_current;
    uint64_t             tick_freq;
    HCURSOR              cursor;

    HGLRC                opengl_context;

    DWORD                app_thread_id;

    yo_backend_opengl_t  backend;
};

// NOTE(rune): Implemented by user. yo_platform_win32_startup launches as separate user thread,
// which calls yo_platform_win32_app_main.
static void yo_platform_win32_app_main();