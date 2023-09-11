#pragma once

typedef struct yo_font_backend yo_font_backend_t;
struct yo_font_backend
{
    FT_Library library;
};

typedef struct yo_font_backend_info yo_font_backend_info_t;
struct yo_font_backend_info
{
    FT_Face face;
};