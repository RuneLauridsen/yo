#pragma once

typedef struct yo_font_backend yo_font_backend_t;
struct yo_font_backend
{
    uint32_t _unused;
};

typedef struct yo_font_backend_info yo_font_backend_info_t;
struct yo_font_backend_info
{
    int32_t unscaled_line_gap;
    int32_t unscaled_ascent;
    int32_t unscaled_descent;
    stbtt_fontinfo stbtt_info;
};
