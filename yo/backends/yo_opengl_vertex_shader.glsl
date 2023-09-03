#version 330 core

layout (location = 0) in vec2   a_rect[2];
layout (location = 2) in vec2   a_corner;
layout (location = 3) in vec4   a_cornerColor;
layout (location = 4) in float  a_cornerRadius;
layout (location = 5) in vec2   a_tex_coord;
layout (location = 6) in float  a_border_thickness;
layout (location = 7) in vec4   a_border_color;

out vec4    out_color;
out vec2    out_pos;
out vec2    out_rect_center;
out vec2    out_rect_dim;
out float   out_radius;
out vec2    out_tex_coord;
out float   out_border_thickness;
out vec4    out_border_color;

uniform vec2 u_resolution;

void main()
{
    vec2 rect_center = a_rect[0] + (0.5 * (a_rect[1] - a_rect[0]));

    gl_Position = vec4(((a_corner / u_resolution) * 2.0 - 1.0) * vec2(1, -1), 0.0, 1.0) ;

    out_color            = a_cornerColor;
    out_tex_coord        = a_tex_coord;
    out_pos              = a_corner;
    out_rect_center      = rect_center;
    out_rect_dim         = a_rect[1] - a_rect[0];
    out_radius           = a_cornerRadius;
    out_border_thickness = a_border_thickness;
    out_border_color     = a_border_color;
}