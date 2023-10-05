#pragma once


static yo_platform_win32_t *global_platform; // TODO(rune): Remove global

static void *yo_platform_win32_opengl_load_function(char *name)
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

static DWORD WINAPI yo_platform_win32_app_thread(LPVOID param)
{
    YO_UNUSED(param);

    //
    // (rune): Set OpenGL context.
    //

    HDC dc = GetDC(global_platform->window);
    wglMakeCurrent(dc, global_platform->opengl_context);
    ReleaseDC(global_platform->window, dc);

    //
    // (rune): Compile shaders, create buffers, etc.
    //

    yo_backend_opengl_startup(&global_platform->backend);

    //
    // (rune): Set yo context
    //

    yo_context_t *yo = yo_create_context(0);
    yo_select_context(yo);
    global_platform->ctx = yo;

    //
    // (rune): Input state
    //

    yo_modifier_t modifiers = 0;
    yo_v2i_t mouse_pos = yo_v2i(0, 0);
    bool mouse_button[3] = { false, false, false };

    uint64_t tick_start = GetTickCount64();
    uint64_t tick_prev = 0;

    //
    // (rune): Main application loop.
    //

    while (global_platform->running)
    {
        uint64_t tick_current = GetTickCount64();

        //
        // (rune): Get window client rect
        //

        RECT client_rect = { 0 };
        GetClientRect(global_platform->window, &client_rect);
        global_platform->render_info.w = client_rect.right - client_rect.left;
        global_platform->render_info.h = client_rect.bottom - client_rect.top;

        //
        // (rune): Receive messages from main thread.
        //

        yo_input_clear();

        MSG msg;
        while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
        {
            switch (msg.message)
            {
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                {
                    uint32_t x = LOWORD(msg.lParam);
                    uint32_t y = HIWORD(msg.lParam);

                    yo_mouse_button_t button =
                        msg.message == WM_LBUTTONDOWN ? YO_MOUSE_BUTTON_LEFT :
                        msg.message == WM_RBUTTONDOWN ? YO_MOUSE_BUTTON_RIGHT :
                        msg.message == WM_MBUTTONDOWN ? YO_MOUSE_BUTTON_MIDDLE :
                        0;

                    mouse_button[button] = true;

                    yo_input_mouse_click(button, x, y, modifiers);

                } break;

                case WM_LBUTTONUP:
                case WM_RBUTTONUP:
                case WM_MBUTTONUP:
                {
                    yo_mouse_button_t button =
                        msg.message == WM_LBUTTONDOWN ? YO_MOUSE_BUTTON_LEFT :
                        msg.message == WM_RBUTTONDOWN ? YO_MOUSE_BUTTON_RIGHT :
                        msg.message == WM_MBUTTONDOWN ? YO_MOUSE_BUTTON_MIDDLE :
                        0;

                    mouse_button[button] = false;

                } break;

                case WM_MOUSEWHEEL:
                {
                    // TODO(rune): Horizontal scrolling.
                    int16_t wheel_delta16 = HIWORD(msg.wParam);
                    float   wheel_delta   = (float)wheel_delta16 / WHEEL_DELTA;
                    yo_input_scroll(0.0f, wheel_delta);
                } break;

                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                {
                    uint32_t vk_code = (uint32_t)msg.wParam;

                    if (vk_code >= 'A' && vk_code  <= 'Z') yo_input_char((char)vk_code, modifiers);
                    if (vk_code >= '0' && vk_code  <= '9') yo_input_char((char)vk_code, modifiers);
                    if (vk_code == VK_HOME)                yo_input_key(YO_KEYCODE_HOME, modifiers);
                    if (vk_code == VK_END)                 yo_input_key(YO_KEYCODE_END, modifiers);
                    if (vk_code == VK_LEFT)                yo_input_key(YO_KEYCODE_LEFT_ARROW, modifiers);
                    if (vk_code == VK_RIGHT)               yo_input_key(YO_KEYCODE_RIGHT_ARROW, modifiers);
                    if (vk_code == VK_UP)                  yo_input_key(YO_KEYCODE_UP_ARROW, modifiers);
                    if (vk_code == VK_DOWN)                yo_input_key(YO_KEYCODE_DOWN_ARROW, modifiers);
                    if (vk_code == VK_DELETE)              yo_input_key(YO_KEYCODE_DELETE, modifiers);
                    if (vk_code == VK_BACK)                yo_input_key(YO_KEYCODE_BACKSPACE, modifiers);
                    if (vk_code == VK_SPACE)               yo_input_key(YO_KEYCODE_SPACE, modifiers);
                    if (vk_code == VK_RETURN)              yo_input_key(YO_KEYCODE_RETURN, modifiers);

                    if (vk_code == VK_MENU)     modifiers |= YO_MODIFIER_ALT;
                    if (vk_code == VK_CONTROL)  modifiers |= YO_MODIFIER_CTLR;
                    if (vk_code == VK_SHIFT)    modifiers |= YO_MODIFIER_SHIFT;
                } break;

                case WM_KEYUP:
                case WM_SYSKEYUP:
                {
                    uint32_t vk_code = (uint32_t)msg.wParam;

                    if (vk_code == VK_MENU)     modifiers &= ~YO_MODIFIER_ALT;
                    if (vk_code == VK_CONTROL)  modifiers &= ~YO_MODIFIER_CTLR;
                    if (vk_code == VK_SHIFT)    modifiers &= ~YO_MODIFIER_SHIFT;
                } break;

                case WM_MOUSEMOVE:
                {
                    mouse_pos.x = LOWORD(msg.lParam);
                    mouse_pos.y = HIWORD(msg.lParam);

                } break;
            }
        }

        yo_input_mouse_state(mouse_button, mouse_pos.x, mouse_pos.y);

        //
        // (rune): Build UI
        //

        yo_begin_frame((tick_current - tick_start) / 1000.0f, 0);
        yo_demo();
        yo_end_frame(&global_platform->render_info);

        //
        // (rune): Render
        //

        dc = GetDC(global_platform->window);
        yo_backend_opengl_render_frame(&global_platform->backend, &global_platform->render_info);
        SwapBuffers(dc);
        ReleaseDC(global_platform->window, dc);

        tick_prev = tick_current;

        YO_PROFILE_FRAME_MARK();
    }

    yo_platform_win32_app_main();

    return 0;
}

static void yo_platform_win32_poll_events(void)
{
}

static LRESULT CALLBACK yo_platform_win32_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            global_platform->running = false;
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

int main()
{
    yo_platform_win32_t platform = { 0 };

    // TODO(rune): Error handling
    global_platform = &platform;

    // TODO(rune): We aren't actually DPI aware yet
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    platform.tick_start = GetTickCount64();
    platform.running = true;

    //
    // (rune): Window creation
    //

    WNDCLASSA window_class = { 0 };
    window_class.lpszClassName = "Yo Window Class";
    window_class.lpfnWndProc = yo_platform_win32_callback;
    RegisterClassA(&window_class);

    platform.dc = CreateCompatibleDC(0);

    platform.window = CreateWindowExA(0,
                                      window_class.lpszClassName,
                                      "Yo",
                                      WS_OVERLAPPEDWINDOW,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      800,
                                      600,
                                      0,
                                      0,
                                      0,
                                      0);

    ShowWindow(platform.window, SW_SHOW);

    //
    // (rune): Pixel format
    //

    HDC window_dc = GetDC(platform.window);

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

    //
    // (rune): OpenGL context
    //

    platform.opengl_context = wglCreateContext(window_dc);
    wglMakeCurrent(window_dc, platform.opengl_context);

    //
    // (rune): Load OpenGL extensions
    //

    platform.backend.glGenBuffers                = (PFNGLGENBUFFERSPROC)(yo_platform_win32_opengl_load_function("glGenBuffers"));
    platform.backend.glBindBuffer                = (PFNGLBINDBUFFERPROC)(yo_platform_win32_opengl_load_function("glBindBuffer"));
    platform.backend.glBufferData                = (PFNGLBUFFERDATAPROC)(yo_platform_win32_opengl_load_function("glBufferData"));
    platform.backend.glCreateShader              = (PFNGLCREATESHADERPROC)(yo_platform_win32_opengl_load_function("glCreateShader"));
    platform.backend.glShaderSource              = (PFNGLSHADERSOURCEPROC)(yo_platform_win32_opengl_load_function("glShaderSource"));
    platform.backend.glCompileShader             = (PFNGLCOMPILESHADERPROC)(yo_platform_win32_opengl_load_function("glCompileShader"));
    platform.backend.glGetShaderiv               = (PFNGLGETSHADERIVPROC)(yo_platform_win32_opengl_load_function("glGetShaderiv"));
    platform.backend.glGetShaderInfoLog          = (PFNGLGETSHADERINFOLOGPROC)(yo_platform_win32_opengl_load_function("glGetShaderInfoLog"));
    platform.backend.glCreateProgram             = (PFNGLCREATEPROGRAMPROC)(yo_platform_win32_opengl_load_function("glCreateProgram"));
    platform.backend.glAttachShader              = (PFNGLATTACHSHADERPROC)(yo_platform_win32_opengl_load_function("glAttachShader"));
    platform.backend.glGetProgramiv              = (PFNGLGETPROGRAMIVPROC)(yo_platform_win32_opengl_load_function("glGetProgramiv"));
    platform.backend.glGetProgramInfoLog         = (PFNGLGETPROGRAMINFOLOGPROC)(yo_platform_win32_opengl_load_function("glGetProgramInfoLog"));
    platform.backend.glUseProgram                = (PFNGLUSEPROGRAMPROC)(yo_platform_win32_opengl_load_function("glUseProgram"));
    platform.backend.glDeleteShader              = (PFNGLDELETESHADERPROC)(yo_platform_win32_opengl_load_function("glDeleteShader"));
    platform.backend.glVertexAttribPointer       = (PFNGLVERTEXATTRIBPOINTERPROC)(yo_platform_win32_opengl_load_function("glVertexAttribPointer"));
    platform.backend.glEnableVertexAttribArray   = (PFNGLENABLEVERTEXATTRIBARRAYPROC)(yo_platform_win32_opengl_load_function("glEnableVertexAttribArray"));
    platform.backend.glGenVertexArrays           = (PFNGLGENVERTEXARRAYSPROC)(yo_platform_win32_opengl_load_function("glGenVertexArrays"));
    platform.backend.glBindVertexArray           = (PFNGLBINDVERTEXARRAYPROC)(yo_platform_win32_opengl_load_function("glBindVertexArray"));
    platform.backend.glLinkProgram               = (PFNGLLINKPROGRAMPROC)(yo_platform_win32_opengl_load_function("glLinkProgram"));
    platform.backend.glGetUniformLocation        = (PFNGLGETUNIFORMLOCATIONPROC)(yo_platform_win32_opengl_load_function("glGetUniformLocation"));
    platform.backend.glUniform4f                 = (PFNGLUNIFORM4FPROC)(yo_platform_win32_opengl_load_function("glUniform4f"));
    platform.backend.glUniform2f                 = (PFNGLUNIFORM2FPROC)(yo_platform_win32_opengl_load_function("glUniform2f"));
    platform.backend.glUniform1i                 = (PFNGLUNIFORM1IPROC)(yo_platform_win32_opengl_load_function("glUniform1i"));

    //
    // (rune): V-Sync
    //

    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)yo_platform_win32_opengl_load_function("wglSwapIntervalEXT");
    wglSwapIntervalEXT(1);

    //
    // (rune): Cleanup
    //

    wglMakeCurrent(window_dc, NULL);
    ReleaseDC(platform.window, window_dc);

    //
    // (rune): App thread
    //

    CreateThread(0, 0, yo_platform_win32_app_thread, 0, 0, &platform.app_thread_id);

    //
    // (rune): Message loop
    //

    MSG msg = { 0 };
    while (GetMessageW(&msg, 0, 0, 0))
    {
        PostThreadMessageA(global_platform->app_thread_id, WM_SIZE, 0, (platform.render_info.w) | (platform.render_info.h << 16));

        yo_modifier_t modifiers = 0;

        if (GetKeyState(VK_LMENU)   & 0xf000) modifiers |= YO_MODIFIER_ALT;
        if (GetKeyState(VK_CONTROL) & 0xf000) modifiers |= YO_MODIFIER_CTLR;
        if (GetKeyState(VK_SHIFT)   & 0xf000) modifiers |= YO_MODIFIER_SHIFT;

        switch (msg.message)
        {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_MOUSEMOVE:
            {
                PostThreadMessageA(platform.app_thread_id, msg.message, msg.wParam, msg.lParam);
            } break;

#if 0
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            {
                uint32_t x = LOWORD(msg.lParam);
                uint32_t y = HIWORD(msg.lParam);
                yo_mouse_button_t button = msg.message == WM_LBUTTONDOWN ? YO_MOUSE_BUTTON_LEFT : YO_MOUSE_BUTTON_RIGHT;
                yo_input_mouse_click(button, x, y, modifiers);

            } break;

            case WM_MOUSEWHEEL:
            {
                // TODO(rune): Horizontal scrolling.
                int16_t wheel_delta16 = HIWORD(msg.wParam);
                float   wheel_delta   = (float)wheel_delta16 / WHEEL_DELTA;
                yo_input_scroll(0.0f, wheel_delta);
            } break;

            case WM_KEYDOWN:
            {
                uint32_t vk_code = (uint32_t)msg.wParam;

                if (vk_code >= 'A' && vk_code  <= 'Z') yo_input_char((char)vk_code, modifiers);
                if (vk_code >= '0' && vk_code  <= '9') yo_input_char((char)vk_code, modifiers);
                if (vk_code == VK_HOME)                yo_input_key(YO_KEYCODE_HOME, modifiers);
                if (vk_code == VK_END)                 yo_input_key(YO_KEYCODE_END, modifiers);
                if (vk_code == VK_LEFT)                yo_input_key(YO_KEYCODE_LEFT_ARROW, modifiers);
                if (vk_code == VK_RIGHT)               yo_input_key(YO_KEYCODE_RIGHT_ARROW, modifiers);
                if (vk_code == VK_UP)                  yo_input_key(YO_KEYCODE_UP_ARROW, modifiers);
                if (vk_code == VK_DOWN)                yo_input_key(YO_KEYCODE_DOWN_ARROW, modifiers);
                if (vk_code == VK_DELETE)              yo_input_key(YO_KEYCODE_DELETE, modifiers);
                if (vk_code == VK_BACK)                yo_input_key(YO_KEYCODE_BACKSPACE, modifiers);
                if (vk_code == VK_SPACE)               yo_input_key(YO_KEYCODE_SPACE, modifiers);
                if (vk_code == VK_RETURN)              yo_input_key(YO_KEYCODE_RETURN, modifiers);
            } break;
#endif
        }

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}
