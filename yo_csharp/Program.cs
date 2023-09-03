// See https://aka.ms/new-console-template for more information
using static Yo.yo_native;

unsafe
{
    //
    // Boiler-plate for setting up a Win32 window with hardware accelerated OpenGL rendering.
    //

    yo_platform_win32_opengl_t* platform;
    platform = yo_platform_win32_opengl_alloc();
    yo_platform_win32_opengl_startup(platform, 800, 600);

    //
    // Main application loop.
    //

    while (platform->running)
    {
        yo_platform_win32_opengl_before_frame(platform);

        yo_box(0, 0);
        yo_new()->fill = YO_WHITE;
        yo_new()->h_dim = yo_px(200);
        yo_new()->v_dim = yo_px(100);
        yo_new()->h_align = YO_ALIGN_LEFT;

        yo_platform_win32_opengl_after_frame(platform);
    }

    //
    // Boiler-plate clean up.
    //

    yo_platform_win32_opengl_shutdown(platform);
    yo_platform_win32_opengl_free(platform);
}
