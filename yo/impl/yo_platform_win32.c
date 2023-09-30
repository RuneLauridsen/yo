#pragma once


static yo_platform_win32_t *global_platform; // TODO(rune): Remove global

// DEBUG(rune):
#if 0
#include <time.h>

static void yo_print_msg(UINT uMsg)
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("%u\t%s", uMsg, asctime(timeinfo));

}
#else
#define yo_print_msg(...)
#endif

#if 1
char *lorem =
"Lorem ipsum dolor sit amet, consectetur adipiscing (elit). Aliquam volutpat sem eget felis facilisis euismod.Suspendisse id ante turpis.Vivamus tincidunt at sapien eu ornare.Aliquam auctor nibh sed dictum sodales.In sed accumsan diam, eget facilisis dui.Nulla tincidunt orci nunc, vitae sollicitudin risus lacinia ac.Phasellus imperdiet suscipit euismod.Duis sodales, odio at sollicitudin hendrerit, urna nisl varius tellus, vel aliquam purus ipsum et augue.Morbi viverra tristique lorem, nec varius eros mattis in.Aliquam erat volutpat.Duis et arcu suscipit, blandit tortor sed, dapibus nisi.Nunc volutpat a neque in blandit.Vivamus vulputate tellus non auctor auctor.Sed luctus turpis nisi, placerat fringilla metus posuere fermentum.Sed lacinia hendrerit gravida.Donec sem erat, rutrum nec velit ac, cursus malesuada diam.\n"
"Morbi varius bibendum augue in pulvinar.Integer vel pretium massa, nec malesuada massa.Praesent sed tristique ipsum.Nullam vitae ex mollis, imperdiet felis sodales, fringilla turpis.Nullam bibendum gravida venenatis.Integer turpis mauris, tempor consectetur sollicitudin sit amet, interdum nec enim.Sed vestibulum maximus ante vitae convallis.Proin luctus tortor ut ultrices aliquet.Fusce dapibus in nunc eget dignissim.Sed non pretium nunc, sed commodo nisi.Fusce interdum dolor id urna varius feugiat.Vivamus at tellus eu orci semper fringilla.Donec dapibus mollis semper.\n"
"Mauris sed nunc vitae mi laoreet iaculis et ut orci.Sed lacinia vitae lorem a viverra.Donec ultrices elit ut luctus mattis.Phasellus purus massa, dictum nec luctus vitae, aliquam id sem.Integer pulvinar ornare risus a suscipit.Ut orci purus, laoreet a tempus vel, euismod a magna.Curabitur vitae purus ut lectus fermentum molestie.Praesent at massa sodales, aliquet dui sit amet, placerat massa.Nam a neque turpis.Nulla a sapien vitae ante blandit suscipit sed ultrices urna.Donec fringilla, nisi in scelerisque facilisis, risus libero sollicitudin mi, vitae ultrices turpis eros eu ex.Duis eu turpis sed magna bibendum blandit quis ut urna.Praesent eu efficitur massa.\n"
"Morbi congue lobortis orci, non ultrices nunc pellentesque imperdiet.Fusce at nulla aliquet, ornare justo a, cursus massa.Aenean bibendum, erat eu tristique aliquam, nibh est dignissim lacus, eget porttitor elit libero viverra quam.In eu viverra risus, eu placerat metus.Sed tincidunt finibus tortor, sit amet gravida ligula ullamcorper non.Integer malesuada efficitur ipsum.Nullam hendrerit mollis mi ac eleifend.Curabitur in ultrices lacus.Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.Nullam quis magna sed nulla tempor efficitur.Morbi tristique tincidunt felis, sed semper tellus luctus in.Mauris elementum, quam et placerat hendrerit, lectus eros pulvinar augue, ut aliquet tortor massa quis massa.Nulla fringilla dolor urna, ac imperdiet nisi lacinia sit amet.Vivamus porttitor malesuada sem.Vivamus tempus porttitor libero hendrerit posuere.Suspendisse hendrerit nibh quis luctus accumsan.\n"
"Sed accumsan et neque ut tincidunt.Quisque sagittis blandit quam, non maximus tortor laoreet ac.Phasellus non mauris mauris.Sed commodo sodales pellentesque.Suspendisse tristique dui eu pharetra sagittis.Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.Suspendisse ultricies hendrerit enim.Praesent fermentum ex aliquet, ultrices odio nec, commodo neque.";
#else
char *lorem =
"a aaaaaaaaaaaaaa aaaaaaaa\taaaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
#endif

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
    // Set OpenGL context.
    //

    HDC dc = GetDC(global_platform->window);
    wglMakeCurrent(dc, global_platform->opengl_context);
    ReleaseDC(global_platform->window, dc);

    //
    // Compile shaders, create buffers, etc.
    //

    yo_backend_opengl_startup(&global_platform->backend);

    //
    // Set yo context
    //

    yo_context_t *yo = yo_create_context(0);
    yo_select_context(yo);
    global_platform->ctx = yo;

    //
    // Input state
    //

    yo_modifier_t modifiers = 0;
    yo_v2i_t mouse_pos = yo_v2i(0, 0);
    bool mouse_button[3] = { false, false, false };

    uint64_t tick_start = GetTickCount64();
    uint64_t tick_prev = 0;

    //
    // Main application loop.
    //

    while (global_platform->running)
    {
        uint64_t tick_current = GetTickCount64();

        //
        // Get window client rect
        //

        RECT client_rect = { 0 };
        GetClientRect(global_platform->window, &client_rect);
        global_platform->render_info.w = client_rect.right - client_rect.left;
        global_platform->render_info.h = client_rect.bottom - client_rect.top;

        //
        // Receive messages from main thread.
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
        // Build UI
        //

        yo_begin_frame((tick_current - tick_start) / 1000.0f, 0);
        yo_rectangle(0, YO_RED, yo_rel(1.0f), yo_rel(1.0f));
        yo_text(lorem);
        yo_end_frame(&global_platform->render_info);

        //
        // Render
        //

        dc = GetDC(global_platform->window);
        yo_backend_opengl_render_frame(&global_platform->backend, &global_platform->render_info);
        SwapBuffers(dc);
        ReleaseDC(global_platform->window, dc);

        tick_prev = tick_current;

        TracyCFrameMark;
    }

    yo_platform_win32_app_main();

    return 0;
}

static void yo_platform_win32_poll_events()
{
}

static LRESULT CALLBACK yo_platform_win32_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    yo_print_msg(uMsg);

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
    // Window creation
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
    // Pixel format
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
    // OpenGL context
    //

    platform.opengl_context = wglCreateContext(window_dc);
    wglMakeCurrent(window_dc, platform.opengl_context);

    //
    // Load OpenGL extensions
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
    // V-Sync
    //

    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)yo_platform_win32_opengl_load_function("wglSwapIntervalEXT");
    wglSwapIntervalEXT(1);

    //
    // Cleanup
    //

    wglMakeCurrent(window_dc, NULL);
    ReleaseDC(platform.window, window_dc);

    //
    // App thread
    //

    CreateThread(0, 0, yo_platform_win32_app_thread, 0, 0, &platform.app_thread_id);

    //
    // Message loop
    //

    MSG msg = { 0 };
    while (GetMessageW(&msg, 0, 0, 0))
    {
        PostThreadMessageA(global_platform->app_thread_id, WM_SIZE, 0, (platform.render_info.w) | (platform.render_info.h << 16));

        yo_print_msg(msg.message);

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
