// See https://aka.ms/new-console-template for more information
using System.Runtime.InteropServices;
using YoOld;
using static YoOld.yo_native;

unsafe
{
    //
    // Boiler-plate for setting up a Win32 window with hardware accelerated OpenGL rendering.
    //

    yo_impl_win32_opengl_t impl = new();
    yo_impl_win32_opengl_startup(&impl, 800, 600);

    //
    // Main application loop.
    //

    while (impl.running)
    {
        yo_impl_win32_opengl_begin_frame(&impl);

        yo.Box();
        yo.SetText("aaaa");

        yo_impl_win32_opengl_end_frame(&impl);
    }

    //
    // Boiler-plate clean up.
    //

    yo_impl_win32_opengl_shutdown(&impl);
}
