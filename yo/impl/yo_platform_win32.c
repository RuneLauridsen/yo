#pragma once

static yo_platform_win32_t *global_platform; // TODO(rune): Remove global

static LRESULT CALLBACK yo_platform_win32_opengl_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            global_platform->running = false;
            PostQuitMessage(0);
            return 0;
        }

        case WM_SIZE:
        {
            // TODO(rune): Rebuild and redraw on window resize.

            global_platform->render_info.w = LOWORD(lParam);
            global_platform->render_info.h = HIWORD(lParam);

            InvalidateRect(hwnd, NULL, true);
        }
    }

    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

static void yo_platform_win32_startup(yo_platform_win32_t *platform, uint32_t window_width, uint32_t window_heigth)
{
    // TODO(rune): Error handling

    global_platform = platform;

    // TODO(rune): We aren't actually DPI aware yet
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    platform->tick_freq = 1000;
    platform->tick_count_start = GetTickCount64();
    platform->ctx = yo_create_context(NULL);
    platform->running = true;
    yo_select_context(platform->ctx);

    //
    // Window creation
    //

    WNDCLASSA window_class = { 0 };
    window_class.lpszClassName = "Yo Window Class";
    window_class.lpfnWndProc = yo_platform_win32_opengl_callback;
    RegisterClassA(&window_class);

    platform->render_info.w = window_width;
    platform->render_info.h = window_heigth;
    platform->dc = CreateCompatibleDC(0);

    platform->window = CreateWindowExA(0,
                                       window_class.lpszClassName,
                                       "Yo",
                                       WS_OVERLAPPEDWINDOW,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       platform->render_info.w,
                                       platform->render_info.h,
                                       0,
                                       0,
                                       0,
                                       0);


}

static void yo_platform_win32_shutdown(yo_platform_win32_t *platform)
{
    // TODO(rune): CloseHandle etc.

    YO_UNUSED(platform);
}

static void yo_platform_win32_begin_frame(yo_platform_win32_t *platform)
{
    yo_input_begin();

    //
    // Message queue
    //

    {
        MSG msg = { 0 };
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
        {
            yo_modifier_t modifiers = 0;

            if (GetKeyState(VK_LMENU)   & 0xf000) modifiers |= YO_MODIFIER_ALT;
            if (GetKeyState(VK_CONTROL) & 0xf000) modifiers |= YO_MODIFIER_CTLR;
            if (GetKeyState(VK_SHIFT)   & 0xf000) modifiers |= YO_MODIFIER_SHIFT;

            switch (msg.message)
            {
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

            }

            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    //
    // Mouse state
    //

    {
        POINT pos;
        GetCursorPos(&pos);
        ScreenToClient(platform->window, &pos);

        bool buttons[YO_MOUSE_BUTTON_COUNT] = { 0 };
        if (GetKeyState(VK_LBUTTON) & 0xf000) buttons[YO_MOUSE_BUTTON_LEFT]   = true;
        if (GetKeyState(VK_RBUTTON) & 0xf000) buttons[YO_MOUSE_BUTTON_RIGHT]  = true;
        if (GetKeyState(VK_MBUTTON) & 0xf000) buttons[YO_MOUSE_BUTTON_MIDDLE] = true;

        yo_input_mouse_state(buttons, pos.x, pos.y);
    }

    yo_input_end();

    //
    // Wall-clock
    //

    platform->tick_current =  GetTickCount64() - platform->tick_count_start;

    ShowWindow(platform->window, SW_SHOW);

    platform->cursor = LoadCursorA(NULL, IDC_ARROW);
}

static void yo_platform_win32_end_frame(yo_platform_win32_t *platform)
{
    YO_UNUSED(platform);
}
