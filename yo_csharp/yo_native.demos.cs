using System.Runtime.InteropServices;

namespace Yo;

public unsafe static partial class yo_native
{
    [DllImport("yo_dll")] public static extern void yo_demo();
}
