#pragma once

////////////////////////////////////////////////////////////////
//
//
// Win32 + OpenGL specific platform code.
//
//
////////////////////////////////////////////////////////////////

#ifndef YO_PLATFORM_API
#define YO_PLATFORM_API
#endif

typedef struct yo_impl_win32_opengl yo_impl_win32_opengl_t;
struct yo_impl_win32_opengl
{
    yo_platform_win32_t  platform;
    yo_backend_opengl_t  backend;
};

// NOTE(rune): For managed C# we provide these helpers, to allocate yo_impl_win32_opengl_t on the heap,
// but they are not needed in C, where you just allocate on the stack.
YO_PLATFORM_API yo_impl_win32_opengl_t * yo_impl_win32_opengl_alloc();
YO_PLATFORM_API void yo_impl_win32_opengl_free(yo_impl_win32_opengl_t *platform);

YO_PLATFORM_API void yo_impl_win32_opengl_startup(yo_impl_win32_opengl_t *platform, uint32_t window_width, uint32_t window_heigth);
YO_PLATFORM_API void yo_impl_win32_opengl_shutdown(yo_impl_win32_opengl_t *platform);
YO_PLATFORM_API void yo_impl_win32_opengl_begin_frame(yo_impl_win32_opengl_t *platform);
YO_PLATFORM_API void yo_impl_win32_opengl_end_frame(yo_impl_win32_opengl_t *platform);
