#pragma once

typedef struct yo_font_metrics yo_font_metrics_t;
struct yo_font_metrics
{
    float line_gap;
    float ascent;
    float descent;
};

// TODO(rune): Rename glyph_metrics
typedef struct yo_codepoint_metrics yo_glyph_metrics_t;
struct yo_codepoint_metrics
{
    yo_v2i_t dim;
    float advance_x;
    float bearing_x;
    float bearing_y;
};

typedef struct yo_font_backend      yo_font_backend_t;      // NOTE(rune): Defined in backend-specific .h file.
typedef struct yo_font_backend_info yo_font_backend_info_t; // NOTE(rune): Defined in backend-specific .h file.

static bool                     yo_font_backend_startup(yo_font_backend_t *backend);
static void                     yo_font_backend_shutdown(yo_font_backend_t *backend);
static bool                     yo_font_backend_load_font(yo_font_backend_t *backend, yo_font_backend_info_t *info, void *data, size_t data_size);
static void                     yo_font_backend_unload_font(yo_font_backend_t *backend, yo_font_backend_info_t *info);
static yo_font_metrics_t        yo_font_backend_get_font_metrics(yo_font_backend_t *backend, yo_font_backend_info_t *info, uint32_t fontsize);
static yo_glyph_metrics_t       yo_font_backend_get_glyph_metrics(yo_font_backend_t *backend, yo_font_backend_info_t *info, uint32_t codepoint, uint32_t fontsize);
static void                     yo_font_backend_rasterize(yo_font_backend_t *backend, yo_font_backend_info_t *info, uint32_t codepoint, uint32_t fontsize, void *pixel, yo_v2i_t dim, int32_t stride);
