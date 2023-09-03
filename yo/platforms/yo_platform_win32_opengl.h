#pragma once

// TODO(rune): Pull things into a yo_platform_win32_common.c file.

typedef struct yo_platform_win32_opengl yo_platform_win32_opengl_t;
struct yo_platform_win32_opengl
{
    HWND                 window;
    HDC                  hdc; // TODO(rune): Rename dc
    yo_context_t        *ctx;
    yo_render_info_t     render_info;
    uint64_t             tick_count_start; // TODO(rune): tick_start
    uint64_t             tick_current;
    uint64_t             tick_freq;
    yo_backend_opengl_t  backend;
    HCURSOR              cursor;
    bool                 running;
};

static void yo_platform_win32_opengl_startup(yo_platform_win32_opengl_t *platform, uint32_t window_width, uint32_t window_heigth);
static void yo_platform_win32_opengl_shutdown(yo_platform_win32_opengl_t *platform);
static void yo_platform_win32_opengl_before_frame(yo_platform_win32_opengl_t *platform);
static void yo_platform_win32_opengl_after_frame(yo_platform_win32_opengl_t *platform);
