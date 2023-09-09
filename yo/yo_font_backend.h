#pragma once

#include "thirdparty/stb_truetype.h"

typedef struct yo_font_metrics yo_font_metrics_t;
struct yo_font_metrics
{
    float line_gap;
    float ascent;
    float descent;
};

typedef struct yo_codepoint_metrics yo_codepoint_metrics_t;
struct yo_codepoint_metrics
{
    yo_v2i_t dim;
    float advance_x;
    float bearing_x;
    float bearing_y;
};

typedef stbtt_fontinfo yo_font_backend_info_t; // TODO(rune): Seperate font backend headers?

extern bool                     yo_font_backend_load_font(yo_font_backend_info_t *info, void *data, size_t data_size);
extern yo_font_metrics_t        yo_font_backend_get_font_metrics(yo_font_backend_info_t *info);
extern yo_codepoint_metrics_t   yo_font_backend_get_codepoint_metrics(yo_font_backend_info_t *info, uint32_t codepoint, float scale);
extern void                     yo_font_backend_rasterize(yo_font_backend_info_t *info, uint32_t codepoint, float scale, void *pixel, yo_v2i_t dim, int32_t stride);
