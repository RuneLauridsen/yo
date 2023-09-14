
global using HWND = System.UIntPtr;
global using HDC = System.UIntPtr;

using System.Diagnostics;
using System.Runtime.InteropServices;
using static YoOld.yo_native;

namespace YoOld;

public unsafe static partial class yo_native
{

    // NOTE(rune): Struct has many more members in the native code, but rather than
    // writing them all out, we just allocate enough stack space.
    [StructLayout(LayoutKind.Sequential, Size = 1024)]
    public struct yo_impl_win32_opengl_t
    {
        public bool running;
    }

    [DllImport("yo_dll.dll")] public static extern void yo_impl_win32_opengl_startup([In, Out] yo_impl_win32_opengl_t* platform, uint32_t window_width, uint32_t window_height);
    [DllImport("yo_dll.dll")] public static extern void yo_impl_win32_opengl_shutdown([In, Out] yo_impl_win32_opengl_t* platform);
    [DllImport("yo_dll.dll")] public static extern void yo_impl_win32_opengl_begin_frame([In, Out] yo_impl_win32_opengl_t* platform);
    [DllImport("yo_dll.dll")] public static extern void yo_impl_win32_opengl_end_frame([In, Out] yo_impl_win32_opengl_t* platform);
};
