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

#if false
        yo_v_layout();
        yo_begin_children();
        yo_box(0, 0); yo_new()->v_dim = yo_rel(1.0f); yo_new()->fill = YO_RED;
        yo_box(0, 0); yo_new()->v_dim = yo_rel(1.0f); yo_new()->fill = YO_ORANGE;
        yo_box(0, 0); yo_new()->v_dim = yo_rel(1.0f); yo_new()->fill = YO_BLUE;
        yo_box(0, 0); yo_new()->v_dim = yo_rel(1.0f); yo_new()->fill = YO_CYAN;
        yo_end_children();
#endif

        yo_demo();

        yo_platform_win32_opengl_after_frame(platform);
    }

    //
    // Boiler-plate clean up.
    //

    yo_platform_win32_opengl_shutdown(platform);
    yo_platform_win32_opengl_free(platform);
}
