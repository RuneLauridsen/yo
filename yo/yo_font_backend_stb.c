#pragma once

#define STBTT_STATIC

#include <stdbool.h>
#include <stdint.h>
#include "yo_math.h"
#include "yo_font_backend.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "thirdparty/stb_truetype.h"

extern bool yo_font_backend_load_font(yo_font_backend_info_t *info, void *data, size_t data_size)
{
    (void)data_size;

    bool ret = false;

    if (stbtt_InitFont(info, data, 0))
    {
        ret = true;
    }

    return ret;
}

extern yo_font_metrics_t yo_font_backend_get_font_metrics(yo_font_backend_info_t *info)
{
    yo_font_metrics_t ret = { 0 };

    int32_t ascent, descent, line_gap;
    stbtt_GetFontVMetrics(info, &ascent, &descent, &line_gap);

    ret.ascent   = (float)ascent;
    ret.descent  = (float)descent;
    ret.line_gap = (float)line_gap;

    return ret;
}

extern yo_codepoint_metrics_t yo_font_backend_get_codepoint_metrics(yo_font_backend_info_t *info, uint32_t codepoint, float scale)
{
    yo_codepoint_metrics_t ret = { 0 };

    int32_t x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(info, codepoint, scale, scale, &x0, &y0, &x1, &y1);

    int32_t dim_x = x1 - x0;
    int32_t dim_y = y1 - y0;

    int32_t scaled_advance_h;
    int32_t scaled_bearing_x;
    stbtt_GetCodepointHMetrics(info, codepoint, &scaled_advance_h, &scaled_bearing_x);

    ret.dim.x = dim_x;
    ret.dim.y = dim_y;
    ret.bearing_y = (float)y0;
    ret.bearing_x = scale * scaled_bearing_x;
    ret.advance_x = scale * scaled_advance_h;

    return ret;
}

extern void yo_font_backend_rasterize(yo_font_backend_info_t *info, uint32_t codepoint, float scale, void *pixel, yo_v2i_t dim, int32_t stride)
{
    stbtt_MakeCodepointBitmap(info, pixel, dim.x, dim.y, stride, scale, scale, codepoint);
}
