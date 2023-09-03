#pragma once

////////////////////////////////////////////////////////////////
//
//
// Theme
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_palette yo_palette_t;
struct yo_palette
{
    yo_color_t background;
    yo_color_t background_1;
    yo_color_t background_2;
    yo_color_t background_3;
    yo_color_t background_selected;
    yo_color_t background_hot;

    struct yo_palette_control
    {
        yo_color_t fill;
        yo_color_t font;
        yo_color_t border;
    } control;

    struct yo_palette_control control_hot;
    struct yo_palette_control control_active;
    struct yo_palette_control control_disabled;
};

static const yo_palette_t yo_palette_dark()
{
    yo_palette_t ret =
    {
        .background   = yo_rgb(30, 30, 30),
        .background_1 = yo_rgb(40, 40, 40),
        .background_2 = yo_rgb(50, 50, 50),
        .background_3 = yo_rgb(60, 60, 60),
        .background_hot = yo_rgb(60, 60, 60),

        .control =
        {
            .fill   = yo_rgb(60, 60, 60),
            .font   = yo_rgb(240, 240, 240),
            .border = yo_rgb(80, 80, 80),
        },

        .control_hot =
        {
            .fill   = yo_rgb(80, 80, 80),
            .font   = yo_rgb(230, 230, 230),
            .border = yo_rgb(100, 100, 100),
        },

        // TODO(rune): control_active

        .control_disabled =
        {
            .fill   = yo_rgb(42, 42, 42),
            .font   = yo_rgb(128, 128, 128),
            .border = yo_rgb(48, 48, 48),
        }
    };

    return ret;
};

////////////////////////////////////////////////////////////////
//
//
// Button style
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_button_style yo_button_style_t;
struct yo_button_style
{
    yo_color_t  fill;
    yo_border_t border;
    yo_color_t  font_color;
    uint32_t    font_size;
    float        h_padding;
    float        v_padding;

    struct
    {
        yo_color_t  fill;
        yo_border_t border;
        yo_color_t  font_color;
    } hot;

    struct
    {
        yo_color_t  fill;
        yo_border_t border;
        yo_color_t  font_color;
    } active;
};

////////////////////////////////////////////////////////////////
//
//
// Slider style
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_thumb_style yo_thumb_style_t;
struct yo_thumb_style
{
    yo_border_t border;
    yo_color_t fill;
    float h_dim;
    float v_dim;
};

typedef struct yo_slider_style yo_slider_style_t;
struct yo_slider_style
{
    yo_axis_t axis;

    // NOTE(rune): Thumb dim can change depending on wether it is active, but we need
    // to know thumb dim in advance, for slider behaviour calculations.
    float thumb_container_h_dim;
    float thumb_container_v_dim;

    yo_thumb_style_t thumb;
    yo_thumb_style_t thumb_hot;
    yo_thumb_style_t thumb_active;
};

static yo_slider_style_t yo_default_slider_style()
{
    yo_slider_style_t ret =
    {
        .axis = YO_AXIS_X,
        .thumb_container_h_dim = 22,
        .thumb_container_v_dim = 22,

        .thumb.fill   = yo_rgb(64, 64, 64),
        .thumb.border = yo_border(2, yo_rgb(128, 128, 128), 20 / 2),
        .thumb.h_dim  = 20,
        .thumb.v_dim  = 20,

        .thumb_hot.fill    = yo_rgb(64, 64, 64),
        .thumb_hot.border  = yo_border(3, yo_rgb(150, 150, 150), 22 / 2),
        .thumb_hot.h_dim   = 22,
        .thumb_hot.v_dim   = 22,

        .thumb_active.fill    = yo_rgb(64, 64, 64),
        .thumb_active.border  = yo_border(3, yo_rgb(150, 150, 150), 22 / 2),
        .thumb_active.h_dim   = 22,
        .thumb_active.v_dim   = 22,
    };

    return ret;
};

////////////////////////////////////////////////////////////////
//
//
// Default style
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_style yo_style_t;
struct yo_style
{
    yo_button_style_t button_style;
    yo_slider_style_t slider_style;
};

static yo_style_t yo_default_style()
{
    yo_palette_t palette = yo_palette_dark();

    yo_style_t ret =
    {
        .slider_style = yo_default_slider_style(),
        .button_style =
        {
            .border     = yo_border(1, palette.control.border, 5),
            .fill       = palette.control.fill,
            .font_color = palette.control.font,
            .font_size = 20,
            .v_padding = 5,
            .h_padding = 8,
            .active =
            {
                .border     = yo_border(1, palette.control_active.border, 5),
                .fill       = palette.control_active.fill,
                .font_color = palette.control_active.font
            },
            .hot =
            {
                .border     = yo_border(1, palette.control_hot.border, 5),
                .fill       = palette.control_hot.fill,
                .font_color = palette.control_hot.font
            },
        },
    };

    return ret;
}
