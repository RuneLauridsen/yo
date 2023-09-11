#pragma once

static bool yo_font_backend_startup(yo_font_backend_t *backend)
{
    // NOTE(rune): no-op
    YO_UNUSED(backend);
    return true;
}

static void yo_font_backend_shutdown(yo_font_backend_t *backend)
{
    // NOTE(rune): no-op
    YO_UNUSED(backend);
}

static bool yo_font_backend_load_font(yo_font_backend_t *backend, yo_font_backend_info_t *info, void *data, size_t data_size)
{
    YO_UNUSED(backend, data_size);

    bool ret = false;

    if (stbtt_InitFont(&info->stbtt_info, data, 0))
    {
        stbtt_GetFontVMetrics(&info->stbtt_info,
                              &info->unscaled_ascent,
                              &info->unscaled_descent,
                              &info->unscaled_line_gap);

        if (!info->unscaled_line_gap)
        {
            info->unscaled_line_gap = info->unscaled_ascent - info->unscaled_descent;
        }

        ret = true;
    }

    return ret;
}

static yo_font_metrics_t yo_font_backend_get_font_metrics(yo_font_backend_t *backend, yo_font_backend_info_t *info, uint32_t fontsize)
{
    YO_UNUSED(backend);

    yo_font_metrics_t ret = { 0 };

    float scale  = (float)(fontsize) / (float)(info->unscaled_ascent);

    ret.ascent   = (float)info->unscaled_ascent   * scale;
    ret.descent  = (float)info->unscaled_descent  * scale;
    ret.line_gap = (float)info->unscaled_line_gap * scale;

    return ret;
}

static yo_glyph_metrics_t yo_font_backend_get_glyph_metrics(yo_font_backend_t *backend, yo_font_backend_info_t *info, uint32_t codepoint, uint32_t fontsize)
{
    YO_UNUSED(backend);

    yo_glyph_metrics_t ret = { 0 };

    float scale  = (float)(fontsize) / (float)(info->unscaled_ascent);

    int32_t x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(&info->stbtt_info, codepoint, scale, scale, &x0, &y0, &x1, &y1);

    int32_t dim_x = x1 - x0;
    int32_t dim_y = y1 - y0;

    int32_t scaled_advance_h;
    int32_t scaled_bearing_x;
    stbtt_GetCodepointHMetrics(&info->stbtt_info, codepoint, &scaled_advance_h, &scaled_bearing_x);

    ret.dim.x = dim_x;
    ret.dim.y = dim_y;
    ret.bearing_y = (float)y0;
    ret.bearing_x = scale * scaled_bearing_x;
    ret.advance_x = scale * scaled_advance_h;

    return ret;
}

static void yo_font_backend_rasterize(yo_font_backend_t *backend, yo_font_backend_info_t *info, uint32_t codepoint, uint32_t fontsize, void *pixel, yo_v2i_t dim, int32_t stride)
{
    YO_UNUSED(backend);

    float scale  = (float)(fontsize) / (float)(info->unscaled_ascent);

    stbtt_MakeCodepointBitmap(&info->stbtt_info, pixel, dim.x, dim.y, stride, scale, scale, codepoint);
}
