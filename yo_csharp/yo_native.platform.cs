
global using HWND = System.UIntPtr;
global using HDC = System.UIntPtr;

using System.Diagnostics;
using System.Runtime.InteropServices;
using static Yo.yo_native;

namespace Yo;

public unsafe static partial class yo_native
{
    // WARNING(rune): Do not allocate on stack. This struct contains more fields that just running,
    // but they were too annoying to write in C#.
    public struct yo_platform_win32_opengl_t
    {
        public bool running;
    }

    [DllImport("yo_dll.dll")] public static extern yo_platform_win32_opengl_t* yo_platform_win32_opengl_alloc();
    [DllImport("yo_dll.dll")] public static extern void yo_platform_win32_opengl_free(yo_platform_win32_opengl_t* platform);
    [DllImport("yo_dll.dll")] public static extern void yo_platform_win32_opengl_startup(yo_platform_win32_opengl_t* platform, uint32_t window_width, uint32_t window_heigth);
    [DllImport("yo_dll.dll")] public static extern void yo_platform_win32_opengl_shutdown(yo_platform_win32_opengl_t* platform);
    [DllImport("yo_dll.dll")] public static extern void yo_platform_win32_opengl_before_frame(yo_platform_win32_opengl_t* platform);
    [DllImport("yo_dll.dll")] public static extern void yo_platform_win32_opengl_after_frame(yo_platform_win32_opengl_t* platform);
};
