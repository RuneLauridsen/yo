#pragma once

#ifndef YO_PLATFORM_API
#define YO_PLATFORM_API
#endif

// TODO(rune): Pull things into a yo_platform_win32_common.c file.

typedef struct yo_platform_win32_opengl yo_platform_win32_opengl_t;
struct yo_platform_win32_opengl
{
    bool                 running;
    HWND                 window;
    HDC                  hdc; // TODO(rune): Rename dc
    yo_context_t        *ctx;
    yo_render_info_t     render_info;
    uint64_t             tick_count_start; // TODO(rune): tick_start
    uint64_t             tick_current;
    uint64_t             tick_freq;
    yo_backend_opengl_t  backend;
    HCURSOR              cursor;
};

YO_PLATFORM_API yo_platform_win32_opengl_t * yo_platform_win32_opengl_alloc();
YO_PLATFORM_API void yo_platform_win32_opengl_free(yo_platform_win32_opengl_t *platform);
YO_PLATFORM_API void yo_platform_win32_opengl_startup(yo_platform_win32_opengl_t *platform, uint32_t window_width, uint32_t window_heigth);
YO_PLATFORM_API void yo_platform_win32_opengl_shutdown(yo_platform_win32_opengl_t *platform);
YO_PLATFORM_API void yo_platform_win32_opengl_before_frame(yo_platform_win32_opengl_t *platform);
YO_PLATFORM_API void yo_platform_win32_opengl_after_frame(yo_platform_win32_opengl_t *platform);
