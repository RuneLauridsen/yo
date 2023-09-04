#pragma once

#ifndef YO_PLATFORM_API
#define YO_PLATFORM_API
#endif

static void *yo_impl_win32_opengl_load_function(char *name)
{
    void *p = (void *)wglGetProcAddress(name);

    if (p == NULL)
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        YO_ASSERT(module);

        p = (void *)GetProcAddress(module, name);
    }

    return p;
}

YO_PLATFORM_API yo_impl_win32_opengl_t * yo_impl_win32_opengl_alloc()
{
    yo_impl_win32_opengl_t *ret = yo_heap_alloc(sizeof(*ret), false);
    return ret;
}

YO_PLATFORM_API void yo_impl_win32_opengl_free(yo_impl_win32_opengl_t *impl)
{
    yo_heap_free(impl);
}

YO_PLATFORM_API void yo_impl_win32_opengl_startup(yo_impl_win32_opengl_t *impl, uint32_t window_width, uint32_t window_heigth)
{
    //
    // Win32
    //

    yo_platform_win32_startup(&impl->platform, window_width, window_heigth);

    //
    // Win32 + OpenGL
    //

    HDC window_dc = GetDC(impl->platform.window);

    PIXELFORMATDESCRIPTOR desired_pixel_format = { 0 };
    desired_pixel_format.nSize = sizeof(desired_pixel_format);
    desired_pixel_format.nVersion = 1;
    desired_pixel_format.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desired_pixel_format.cColorBits = 32;
    desired_pixel_format.cAlphaBits = 8;
    desired_pixel_format.iLayerType = PFD_MAIN_PLANE;

    uint32_t suggested_pixel_format_index = ChoosePixelFormat(window_dc, &desired_pixel_format);
    PIXELFORMATDESCRIPTOR suggestedPixelFormat = { 0 };

    DescribePixelFormat(window_dc, suggested_pixel_format_index, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(window_dc, suggested_pixel_format_index, &suggestedPixelFormat);

    HGLRC glrc = wglCreateContext(window_dc);

    if (wglMakeCurrent(window_dc, glrc))
    {
        //
        // Load OpenGL extensions
        //
        {
            impl->backend.glGenBuffers                = (PFNGLGENBUFFERSPROC)(yo_impl_win32_opengl_load_function("glGenBuffers"));
            impl->backend.glBindBuffer                = (PFNGLBINDBUFFERPROC)(yo_impl_win32_opengl_load_function("glBindBuffer"));
            impl->backend.glBufferData                = (PFNGLBUFFERDATAPROC)(yo_impl_win32_opengl_load_function("glBufferData"));
            impl->backend.glCreateShader              = (PFNGLCREATESHADERPROC)(yo_impl_win32_opengl_load_function("glCreateShader"));
            impl->backend.glShaderSource              = (PFNGLSHADERSOURCEPROC)(yo_impl_win32_opengl_load_function("glShaderSource"));
            impl->backend.glCompileShader             = (PFNGLCOMPILESHADERPROC)(yo_impl_win32_opengl_load_function("glCompileShader"));
            impl->backend.glGetShaderiv               = (PFNGLGETSHADERIVPROC)(yo_impl_win32_opengl_load_function("glGetShaderiv"));
            impl->backend.glGetShaderInfoLog          = (PFNGLGETSHADERINFOLOGPROC)(yo_impl_win32_opengl_load_function("glGetShaderInfoLog"));
            impl->backend.glCreateProgram             = (PFNGLCREATEPROGRAMPROC)(yo_impl_win32_opengl_load_function("glCreateProgram"));
            impl->backend.glAttachShader              = (PFNGLATTACHSHADERPROC)(yo_impl_win32_opengl_load_function("glAttachShader"));
            impl->backend.glGetProgramiv              = (PFNGLGETPROGRAMIVPROC)(yo_impl_win32_opengl_load_function("glGetProgramiv"));
            impl->backend.glGetProgramInfoLog         = (PFNGLGETPROGRAMINFOLOGPROC)(yo_impl_win32_opengl_load_function("glGetProgramInfoLog"));
            impl->backend.glUseProgram                = (PFNGLUSEPROGRAMPROC)(yo_impl_win32_opengl_load_function("glUseProgram"));
            impl->backend.glDeleteShader              = (PFNGLDELETESHADERPROC)(yo_impl_win32_opengl_load_function("glDeleteShader"));
            impl->backend.glVertexAttribPointer       = (PFNGLVERTEXATTRIBPOINTERPROC)(yo_impl_win32_opengl_load_function("glVertexAttribPointer"));
            impl->backend.glEnableVertexAttribArray   = (PFNGLENABLEVERTEXATTRIBARRAYPROC)(yo_impl_win32_opengl_load_function("glEnableVertexAttribArray"));
            impl->backend.glGenVertexArrays           = (PFNGLGENVERTEXARRAYSPROC)(yo_impl_win32_opengl_load_function("glGenVertexArrays"));
            impl->backend.glBindVertexArray           = (PFNGLBINDVERTEXARRAYPROC)(yo_impl_win32_opengl_load_function("glBindVertexArray"));
            impl->backend.glLinkProgram               = (PFNGLLINKPROGRAMPROC)(yo_impl_win32_opengl_load_function("glLinkProgram"));
            impl->backend.glGetUniformLocation        = (PFNGLGETUNIFORMLOCATIONPROC)(yo_impl_win32_opengl_load_function("glGetUniformLocation"));
            impl->backend.glUniform4f                 = (PFNGLUNIFORM4FPROC)(yo_impl_win32_opengl_load_function("glUniform4f"));
            impl->backend.glUniform2f                 = (PFNGLUNIFORM2FPROC)(yo_impl_win32_opengl_load_function("glUniform2f"));
            impl->backend.glUniform1i                 = (PFNGLUNIFORM1IPROC)(yo_impl_win32_opengl_load_function("glUniform1i"));
        }
    }

    //
    // V-Sync
    //

    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)yo_impl_win32_opengl_load_function("wglSwapIntervalEXT");
    wglSwapIntervalEXT(1);

    ReleaseDC(impl->platform.window, window_dc);

    //
    // OpenGL
    //

    yo_backend_opengl_startup(&impl->backend);
}

YO_PLATFORM_API void yo_impl_win32_opengl_shutdown(yo_impl_win32_opengl_t *impl)
{
    yo_platform_win32_shutdown(&impl->platform);
    yo_backend_opengl_shutdown(&impl->backend);
}

YO_PLATFORM_API void yo_impl_win32_opengl_begin_frame(yo_impl_win32_opengl_t *impl)
{
    yo_platform_win32_begin_frame(&impl->platform);
    yo_begin_frame((float)impl->platform.tick_current / (float)impl->platform.tick_freq, 0);
}

YO_PLATFORM_API void yo_impl_win32_opengl_end_frame(yo_impl_win32_opengl_t *impl)
{
    yo_end_frame(&impl->platform.render_info);

    SetCursor(impl->platform.cursor);

    HDC dc = GetDC(impl->platform.window);

    yo_backend_opengl_render_frame(&impl->backend, &impl->platform.render_info);

    SwapBuffers(dc);
    ReleaseDC(impl->platform.window, dc);
}
