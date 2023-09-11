#pragma once




static void yo_freetype_test(uint8_t *pixels, yo_v2i_t dim)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("ERROR: FT_Init_FreeType failed.\n");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "C:\\Windows\\Fonts\\segoeui.ttf", 0, &face))
    {
        printf("ERROR: FT_New_Face failed.\n");
        return;
    }

    if (FT_Set_Pixel_Sizes(face, 0, 48))
    {
        printf("ERROR: FT_Set_Pixel_Sizes failed.\n");
        return;
    }

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
    {
        printf("ERROR: FT_Load_Char failed.\n");
        return;
    }

    FT_Bitmap bitmap = face->glyph->bitmap;


    for (uint32_t x = 0; x < bitmap.width; x++)
    {
        for (uint32_t y = 0; y < bitmap.rows; y++)
        {
            uint8_t *bitmap_pixel = &bitmap.buffer[x + y * bitmap.pitch];
            uint8_t *atlas_pixel  = &pixels[x + y * dim.x];

            *atlas_pixel = *bitmap_pixel;
        }
    }
}

static bool yo_font_backend_startup(yo_font_backend_t *backend)
{
    bool ret = false;

    if (!FT_Init_FreeType(&backend->library))
    {
        ret = true;
    }
    else
    {
        printf("ERROR: FT_Init_FreeType failed.\n");
    }

    return ret;
}

static void yo_font_backend_shutdown(yo_font_backend_t *backend)
{
    if (!FT_Done_FreeType(backend->library))
    {
        // NOTE(rune): All good
    }
    else
    {
        printf("ERROR: FT_Done_FreeType failed.\n");
    }
}

static bool yo_font_backend_load_font(yo_font_backend_t *backend, yo_font_backend_info_t *info, void *data, size_t data_size)
{
    bool ret = false;

    FT_Error error = FT_New_Memory_Face(backend->library, data, (FT_Long)data_size, 0, &info->face);

    if (!error)
    {
        ret = true;
    }
    else
    {
        printf("ERROR: FT_New_Memory_Face failed.\n");
    }

    return ret;
}

static void yo_font_backend_unload_font(yo_font_backend_t *backend, yo_font_backend_info_t *info)
{
    YO_UNUSED(backend);

    if (!FT_Done_Face(info->face))
    {
        // NOTE(rune): All good
    }
    else
    {
        printf("ERROR: FT_Done_Face failed.\n");
    }
}

static yo_font_metrics_t yo_font_backend_get_font_metrics(yo_font_backend_t *backend, yo_font_backend_info_t *info, uint32_t fontsize)
{
    YO_UNUSED(backend);
    YO_UNUSED(fontsize);

    yo_font_metrics_t ret = { 0 };

    if (!FT_Set_Pixel_Sizes(info->face, 0, fontsize))
    {
        // NOTE(rune): FreeType's metrics are in 1/64 pixels
        ret.ascent   = (float)info->face->size->metrics.ascender  / 64.0f;
        ret.descent  = (float)info->face->size->metrics.descender / 64.0f;
        ret.line_gap = (float)info->face->size->metrics.height    / 64.0f; // TODO(rune): What is face->height used for?
        ret.line_gap = ret.ascent - ret.descent;
    }
    else
    {
        printf("ERROR: FT_Set_Pixel_Sizes failed.\n");
    }

    return ret;
}

static yo_glyph_metrics_t yo_font_backend_get_glyph_metrics(yo_font_backend_t *backend, yo_font_backend_info_t *info, uint32_t codepoint, uint32_t fontsize)
{
    YO_UNUSED(backend);

    yo_glyph_metrics_t ret = { 0 };

    if (!FT_Set_Pixel_Sizes(info->face, 0, fontsize))
    {
        // TODO(rune): We shouldn't render here
        if (!FT_Load_Char(info->face, codepoint, FT_LOAD_RENDER))
        {
            ret.bearing_x = (float)info->face->glyph->bitmap_left;
            ret.bearing_y = (float)-info->face->glyph->bitmap_top ;
            ret.advance_x = (float)info->face->glyph->advance.x  / 64.0f;
            ret.dim.x     = info->face->glyph->bitmap.width;
            ret.dim.y     = info->face->glyph->bitmap.rows;
        }
        else
        {
            printf("ERROR: FT_Load_Char failed.\n");
        }
    }
    else
    {
        printf("ERROR: FT_Set_Pixel_Sizes failed.\n");
    }

    return ret;
}

static void yo_font_backend_rasterize(yo_font_backend_t *backend, yo_font_backend_info_t *info, uint32_t codepoint, uint32_t fontsize, void *pixel, yo_v2i_t dim, int32_t stride)
{
    YO_UNUSED(backend);

    if (!FT_Set_Pixel_Sizes(info->face, 0, fontsize))
    {
        if (!FT_Load_Char(info->face, codepoint, FT_LOAD_RENDER))
        {
            FT_Bitmap bitmap = info->face->glyph->bitmap;

            for (uint32_t src_x = 0; src_x < bitmap.width; src_x++)
            {
                for (uint32_t src_y = 0; src_y < bitmap.rows; src_y++)
                {
                    YO_ASSERT((int32_t)src_x < dim.x);
                    YO_ASSERT((int32_t)src_y < dim.y);

                    uint8_t *bitmap_pixel = &bitmap.buffer[src_x + src_y * bitmap.pitch];
                    uint8_t *atlas_pixel  = &((uint8_t *)pixel)[src_x + src_y * stride];

                    *atlas_pixel = *bitmap_pixel;
                }
            }
        }
        else
        {
            printf("ERROR: FT_Load_Char failed.\n");
        }
    }
    else
    {
        printf("ERROR: FT_Set_Pixel_Sizes failed.\n");
    }
}
