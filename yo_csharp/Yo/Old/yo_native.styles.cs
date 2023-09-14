using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static YoOld.yo_native;
using YoOld;

namespace YoOld;

public unsafe static partial class yo_native
{
    static yo_border_t yo_border(float thickness, yo_v4f_t color, float radius)
    {
        yo_border_t ret;
        ret.radius.top_right = radius;
        ret.radius.top_left = radius;
        ret.radius.bottom_right = radius;
        ret.radius.bottom_left = radius;
        ret.thickness = thickness;
        ret.color = color;
        return ret;
    }

    ////////////////////////////////////////////////////////////////
    //
    //
    // Theme
    //
    //
    ////////////////////////////////////////////////////////////////

    public struct yo_palette_control_t
    {
        public yo_color_t fill;
        public yo_color_t font;
        public yo_color_t border;
    }

    public struct yo_palette_t
    {
        public yo_color_t background;
        public yo_color_t background_1;
        public yo_color_t background_2;
        public yo_color_t background_3;
        public yo_color_t background_selected;
        public yo_color_t background_hot;

        public yo_palette_control_t control;
        public yo_palette_control_t control_hot;
        public yo_palette_control_t control_active;
        public yo_palette_control_t control_disabled;
    };

    public static yo_palette_t yo_palette_dark()
    {
        yo_palette_t ret = new()
        {
            background = yo_rgb(30, 30, 30),
            background_1 = yo_rgb(40, 40, 40),
            background_2 = yo_rgb(50, 50, 50),
            background_3 = yo_rgb(60, 60, 60),
            background_hot = yo_rgb(60, 60, 60),

            control = new()
            {
                fill = yo_rgb(60, 60, 60),
                font = yo_rgb(240, 240, 240),
                border = yo_rgb(80, 80, 80),
            },

            control_hot = new()
            {
                fill = yo_rgb(80, 80, 80),
                font = yo_rgb(230, 230, 230),
                border = yo_rgb(100, 100, 100),
            },

            // TODO(rune): control_active

            control_disabled = new()
            {
                fill = yo_rgb(42, 42, 42),
                font = yo_rgb(128, 128, 128),
                border = yo_rgb(48, 48, 48),
            }
        };

        return ret;
    }

    ////////////////////////////////////////////////////////////////
    //
    //
    // Button style
    //
    //
    ////////////////////////////////////////////////////////////////


    public struct yo_button_style_t
    {
        public yo_color_t fill;
        public yo_border_t border;
        public yo_color_t font_color;
        public uint32_t font_size;
        public float h_padding;
        public float v_padding;

        public yo_color_t fill_hot;
        public yo_border_t border_hot;
        public yo_color_t font_color_hot;

        public yo_color_t fill_active;
        public yo_border_t border_active;
        public yo_color_t font_color_active;
    };

    ////////////////////////////////////////////////////////////////
    //
    //
    // Slider style
    //
    //
    ////////////////////////////////////////////////////////////////

    public struct yo_thumb_style_t
    {
        public yo_border_t border;
        public yo_color_t fill;
        public float h_dim;
        public float v_dim;
    };


    public struct yo_slider_style_t
    {
        public yo_axis_t axis;

        // NOTE(rune): Thumb dim can change depending on wether it is active, but we need
        // to know thumb dim in advance, for slider behaviour calculations.
        public float thumb_container_h_dim;
        public float thumb_container_v_dim;

        public yo_thumb_style_t thumb;
        public yo_thumb_style_t thumb_hot;
        public yo_thumb_style_t thumb_active;
    };

    static yo_slider_style_t yo_default_slider_style()
    {
        yo_slider_style_t ret = new()
        {
            axis = YO_AXIS_X,
            thumb_container_h_dim = 22,
            thumb_container_v_dim = 22,

            thumb = new()
            {
                fill = yo_rgb(64, 64, 64),
                border = yo_border(2, yo_rgb(128, 128, 128), 20 / 2),
                h_dim = 20,
                v_dim = 20,
            },

            thumb_hot = new()
            {
                fill = yo_rgb(64, 64, 64),
                border = yo_border(3, yo_rgb(150, 150, 150), 22 / 2),
                h_dim = 22,
                v_dim = 22,
            },

            thumb_active = new()
            {
                fill = yo_rgb(64, 64, 64),
                border = yo_border(3, yo_rgb(150, 150, 150), 22 / 2),
                h_dim = 22,
                v_dim = 22,
            }
        };

        return ret;
    }

    ////////////////////////////////////////////////////////////////
    //
    //
    // Default style
    //
    //
    ////////////////////////////////////////////////////////////////


    public struct yo_style_t
    {
        public yo_button_style_t button_style;
        public yo_slider_style_t slider_style;
    };

    public static yo_style_t yo_default_style()
    {
        yo_palette_t palette = yo_palette_dark();

        yo_style_t ret = new()
        {
            slider_style = yo_default_slider_style(),
            button_style = new()
            {
                border = yo_border(1, palette.control.border, 5),
                fill = palette.control.fill,
                font_color = palette.control.font,
                font_size = 20,
                v_padding = 5,
                h_padding = 8,
                border_active = yo_border(1, palette.control_active.border, 5),
                fill_active = palette.control_active.fill,
                font_color_active = palette.control_active.font,
                border_hot = yo_border(1, palette.control_hot.border, 5),
                fill_hot = palette.control_hot.fill,
                font_color_hot = palette.control_hot.font
            },
        };

        return ret;
    }

}
