#version 330 core
out vec4 FragColor;

in vec4     out_color;
in vec2     out_pos;
in vec2     out_rect_center;
in vec2     out_rect_dim;
in float    out_radius;
in vec2     out_tex_coord;
in float    out_border_thickness;
in vec4     out_border_color;

uniform vec2 u_resolution;
uniform sampler2D u_sampler;

// https://www.rfleury.com/p/ui-part-6-rendering?utm_source=profile&utm_medium=reader2
float RoundedRectSDF(vec2 sample_pos,
                     vec2 rect_center,
                     vec2 rect_half_size,
                     float r)
{
    vec2 d2 = (abs(rect_center - sample_pos) - rect_half_size + vec2(r, r));
    return min(max(d2.x, d2.y), 0.0) + length(max(d2, 0.0)) - r;
}

void main()
{

    // Map to linear color space
    vec3 linear_interior_color;
    vec3 linear_border_color;
    {
        linear_interior_color.r = pow(out_color.r, 2.0);
        linear_interior_color.g = pow(out_color.g, 2.0);
        linear_interior_color.b = pow(out_color.b, 2.0);

        linear_border_color.r = pow(out_border_color.r, 2.0);
        linear_border_color.g = pow(out_border_color.g, 2.0);
        linear_border_color.b = pow(out_border_color.b, 2.0);
    }

    float dist_inner = RoundedRectSDF(out_pos, out_rect_center, out_rect_dim / 2, out_radius);

    float dist_outer = -1000;
    if (out_border_thickness != 0)
    {
        dist_outer = RoundedRectSDF(out_pos, out_rect_center, (out_rect_dim / 2) - vec2(out_border_thickness, out_border_thickness), out_radius - out_border_thickness);
    }

    if (out_tex_coord == vec2(0, 0))
    {
        float sdf_factor      = clamp(0.5 - dist_inner,                      0, 1);
        float border_factor   = clamp(min(0.5-dist_inner, 0.5+dist_outer),   0, 1);
        float interior_factor = clamp(0.5 - dist_outer,                      0, 1);

        // TODO(rune): Hack
        if(out_border_thickness == 0 && out_radius == 0)
        {
            sdf_factor      = 1;
            border_factor   = 0;
            interior_factor = 1;
        }

        //FragColor = ((out_color * interior_factor) + (out_border_color * border_factor)) * sdf_factor;

        FragColor = out_color;
        FragColor.a *= sdf_factor;

        FragColor = vec4(mix(linear_border_color, linear_interior_color, interior_factor - border_factor), 1);
        FragColor.a *= sdf_factor * out_color.a;

        FragColor.r = sqrt(FragColor.r);
        FragColor.g = sqrt(FragColor.g);
        FragColor.b = sqrt(FragColor.b);
    }
    else
    {
        FragColor = out_color * texture(u_sampler, out_tex_coord);
    }

    //FragColor = vec4(1 - out_tex_coord, 0, 1);
}

