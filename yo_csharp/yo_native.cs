// WARNING(rune): These bindings are direct translations of their C counterparts, and are not C# friendly,
// or written following C# style.
// TODO(rune): C-sharp bindings are not fun to write manually.

global using uint32_t = System.UInt32;
global using uint64_t = System.UInt64;
global using int32_t = System.Int64;
global using int64_t = System.Int64;
global using size_t = System.UIntPtr;
global using yo_id_t = System.UInt64;
global using yo_color_t = Yo.yo_native.yo_v4f_t;
global using yo_error_t = System.UInt32;
global using static Yo.yo_native.yo_error_t;
global using static Yo.yo_native.yo_axis_t;
global using static Yo.yo_native.yo_layout_t;
global using static Yo.yo_native.yo_align_t;
global using static Yo.yo_native.yo_overflow_t;
global using static Yo.yo_native.yo_side_t;
global using static Yo.yo_native.yo_corner_t;
global using static Yo.yo_native.yo_popup_flags_t;
global using static Yo.yo_native.yo_box_flags_t;
global using static Yo.yo_native.yo_anim_flags_t;
global using static Yo.yo_native.yo_mouse_button_t;
global using static Yo.yo_native.yo_keycode_t;
global using static Yo.yo_native.yo_modifier_t;
global using static Yo.yo_native.yo_frame_flags_t;

namespace Yo;

public unsafe static partial class yo_native
{
    public struct yo_v2f_t { public float x, y; }
    public struct yo_v3f_t { public float x, y, z; }
    public struct yo_v4f_t { public float x, y, z, w; } // TODO(rune): How to write unions in C#?
    public struct yo_v2i_t { public int32_t x, y; }

    public struct yo_recti_t { public int32_t x, y; public uint32_t h_dim, v_dim; }
    public struct yo_rectf_t { public float x, y; public float h_dim, v_dim; }

    public static yo_v4f_t yo_rgba(byte r, byte g, byte b, byte a)
    {
        yo_v4f_t ret = new()
        {
            x = (float)(r) / 255.0f,
            y = (float)(g) / 255.0f,
            z = (float)(b) / 255.0f,
            w = (float)(a) / 255.0f,
        };
        return ret;
    }

    public static yo_v4f_t yo_argb(byte a, byte r, byte g, byte b)
    {
        yo_v4f_t ret = yo_rgba(r, g, b, a);
        return ret;
    }

    public static yo_v4f_t yo_rgb(byte r, byte g, byte b)
    {
        yo_v4f_t ret = yo_rgba(r, g, b, 0xff);
        return ret;
    }

    public static yo_length_t yo_px(float px)
    {
        yo_length_t ret = new()
        {
            min = px,
            max = px,
            rel = 0.0f,
            is_rel = false,
        };

        return ret;
    }

    public static yo_length_t yo_rel(float rel)
    {
        yo_length_t ret = new()
        {
            min = 0.0f,
            max = float.MinValue,
            rel = rel,
            is_rel = true,
        };

        return ret;
    }

    public static readonly yo_v4f_t YO_TRANSPARENT = yo_argb(0x00, 0x00, 0x00, 0x00);
    public static readonly yo_v4f_t YO_WHITE = yo_argb(0xff, 0xff, 0xff, 0xff);
    public static readonly yo_v4f_t YO_BLACK = yo_argb(0xff, 0x00, 0x00, 0x00);
    public static readonly yo_v4f_t YO_RED = yo_argb(0xff, 0xff, 0x00, 0x00);
    public static readonly yo_v4f_t YO_GREEN = yo_argb(0xff, 0x00, 0xff, 0x00);
    public static readonly yo_v4f_t YO_BLUE = yo_argb(0xff, 0x00, 0x00, 0xff);
    public static readonly yo_v4f_t YO_YELLOW = yo_argb(0xff, 0xff, 0xff, 0x00);
    public static readonly yo_v4f_t YO_CYAN = yo_argb(0xff, 0x00, 0xff, 0xff);
    public static readonly yo_v4f_t YO_MAGENTA = yo_argb(0xff, 0xff, 0x00, 0xff);
    public static readonly yo_v4f_t YO_GRAY = yo_argb(0xff, 0x80, 0x80, 0x80);
    public static readonly yo_v4f_t YO_ORANGE = yo_argb(0xff, 0xff, 0xa5, 0x00);
}
