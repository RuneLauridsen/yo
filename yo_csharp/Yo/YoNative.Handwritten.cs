using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static YoOld.yo_native;

namespace YoNew;

internal unsafe partial class YoNative
{
    internal struct yo_context_t { };
    internal struct yo_box_t { };
    internal struct yo_render_info_t { };

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
            max = float.MaxValue,
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
