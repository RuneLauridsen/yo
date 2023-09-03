#pragma once

////////////////////////////////////////////////////////////////
//
//
// Types
//
//
////////////////////////////////////////////////////////////////

typedef struct yo_svg_doc yo_svg_doc_t;
struct yo_svg_doc
{
    uint32_t fill;
    uint32_t stroke;
    uint32_t stroke_width;
    yo_array(char) out;
};

////////////////////////////////////////////////////////////////
//
//
// Core
//
//
////////////////////////////////////////////////////////////////

static void yo_svg_doc_create(yo_svg_doc_t *doc);
static void yo_svg_doc_destroy(yo_svg_doc_t *doc);
static void yo_svg_begin(yo_svg_doc_t *doc, yo_v2i_t dims);
static void yo_svg_end(yo_svg_doc_t *doc);
static void yo_svg_write_file(yo_svg_doc_t *doc, char *file_path);

////////////////////////////////////////////////////////////////
//
//
// Style
//
//
////////////////////////////////////////////////////////////////

static void yo_svg_set_fill(yo_svg_doc_t *doc, uint32_t fill);
static void yo_svg_set_stroke(yo_svg_doc_t *doc, uint32_t stroke, uint32_t stroke_width);

////////////////////////////////////////////////////////////////
//
//
// Shapes
//
//
////////////////////////////////////////////////////////////////

static void yo_svg_draw_rectangle(yo_svg_doc_t *doc, yo_v2i_t p, yo_v2i_t dims);
static void yo_svg_draw_circle(yo_svg_doc_t *doc, yo_v2i_t c, int32_t r);
static void yo_svg_draw_ellipse(yo_svg_doc_t *doc, yo_v2i_t c, yo_v2i_t r);
static void yo_svg_draw_line(yo_svg_doc_t *doc, yo_v2i_t p0, yo_v2i_t p1);
static void yo_svg_draw_polygon(yo_svg_doc_t *doc, yo_v2i_t *points, size_t points_count);
