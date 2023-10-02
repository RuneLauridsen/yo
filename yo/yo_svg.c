#pragma once

static void yo_svg_doc_create(yo_svg_doc_t *doc)
{
    yo_zero_struct(doc);
    yo_array_create(&doc->out, YO_KILOBYTES(4), true);
}

static void yo_svg_doc_destroy(yo_svg_doc_t *doc)
{
    yo_array_destroy(&doc->out);
}

static void yo_svg_printf_v(yo_svg_doc_t *doc, char *format, va_list args)
{
    size_t size = vsnprintf(NULL, 0, format, args) + 1;
    char *buffer = yo_array_add(&doc->out, size, false);
    vsnprintf(buffer, size, format, args);
    yo_array_pop(&doc->out, 1); // NOTE(rune): Pop null terminator
}

static void yo_svg_printf(yo_svg_doc_t *doc, YO_PRINTF_FORMAT_STRING char *format, ...)
{
    va_list args;
    va_start(args, format);
    yo_svg_printf_v(doc, format, args);
    va_end(args);
}

static void yo_svg_begin(yo_svg_doc_t *doc, yo_v2i_t dim)
{
    yo_array_reset(&doc->out, true);
    yo_svg_printf(doc, "<svg width=\"%i\" height=\"%i\" xmlns=\"http://www.w3.org/2000/svg\">\n", dim.x, dim.y);
    yo_svg_set_fill(doc, yo_rgb32(230, 230, 230));
    yo_svg_set_stroke(doc, yo_rgb32(200, 200, 200), 1);
}

static void yo_svg_end(yo_svg_doc_t *doc)
{
    yo_svg_printf(doc, "</svg>");
    yo_array_add(&doc->out, 1, true); // NOTE(rune): Null terminator
}

static void yo_svg_write_file(yo_svg_doc_t *doc, char *file_path)
{
    // TODO(rune): Error handling

    FILE *file = fopen(file_path, "wb");
    if (file)
    {
        fputs(doc->out.elems, file);
        fclose(file);
    }
}

static inline uint32_t yo_svg_color_swizzle(uint32_t argb)
{
    // NOTE(rune): Alpha is ignored
    uint32_t r = (argb >> 0) & 0xff;
    uint32_t g = (argb >> 8)  & 0xff;
    uint32_t b = (argb >> 16)  & 0xff;
    uint32_t swizzle = (r << 16) | (g << 8) | (b << 0);
    return swizzle;
}

static void yo_svg_set_fill(yo_svg_doc_t *doc, uint32_t fill)
{
    doc->fill = yo_svg_color_swizzle(fill);
}

static void yo_svg_set_stroke(yo_svg_doc_t *doc, uint32_t stroke, uint32_t stroke_width)
{
    doc->stroke       = yo_svg_color_swizzle(stroke);
    doc->stroke_width = stroke_width;
}

static void yo_svg_draw_rectangle(yo_svg_doc_t *doc, yo_v2i_t p, yo_v2i_t dim)
{
    yo_svg_printf(doc, "<rect x=\"%i\" y=\"%i\" width=\"%i\" height=\"%i\" fill=\"#%06x\" stroke=\"#%06x\" stroke-width=\"%i\"/>\n",
                  p.x, p.y, dim.x, dim.y, doc->fill, doc->stroke, doc->stroke_width);
}

static void yo_svg_draw_circle(yo_svg_doc_t *doc, yo_v2i_t c, int32_t r)
{
    yo_svg_printf(doc, "<circle cx=\"%i\" cy=\"%i\" r=\"%i\" fill=\"#%06x\" stroke=\"#%06x\" stroke-width=\"%i\"/>\n",
                  c.x, c.y, r, doc->fill, doc->stroke, doc->stroke_width);
}

static void yo_svg_draw_ellipse(yo_svg_doc_t *doc, yo_v2i_t c, yo_v2i_t r)
{
    yo_svg_printf(doc, "<ellipse cx=\"%i\" cy=\"%i\" rx=\"%i\" ry=\"%i\" fill=\"#%06x\" stroke=\"#%06x\" stroke-width=\"%i\"/>\n",
                  c.x, c.y, r.x, r.y, doc->fill, doc->stroke, doc->stroke_width);
}

static void yo_svg_draw_line(yo_svg_doc_t *doc, yo_v2i_t p0, yo_v2i_t p1)
{
    yo_svg_printf(doc, "<circle x1=\"%i\" y1=\"%i\" x2=\"%i\" y2=\"%i\" stroke=\"#%06x\" stroke-width=\"%i\"/>\n",
                  p0.x, p0.y, p1.x, p1.y, doc->stroke, doc->stroke_width);
}

static void yo_svg_draw_polygon(yo_svg_doc_t *doc, yo_v2i_t *points, size_t points_count)
{
    yo_svg_printf(doc, "<polygon fill=\"#%06x\" stroke=\"#%06x\" stroke-width=\"%i\" points=\"",
                  doc->fill, doc->stroke, doc->stroke_width);

    for (size_t i = 0; i < points_count; i++)
    {
        yo_svg_printf(doc, "%i,%i ", points[i].x, points[i].y);
    }

    yo_svg_printf(doc, "\"\n/>");
}
