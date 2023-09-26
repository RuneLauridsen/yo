////////////////////////////////////////////////////////////////
//
//
// Vertex shader
//
//
////////////////////////////////////////////////////////////////

// TODO(rune): Make a tool to automatically generate a c header file from shader source files.

static char *yo_opengl_vs_source =
"#version 330 core                                                                                                     \n"
"                                                                                                                      \n"
"layout(location = 0) in vec2   a_rect[2];                                                                             \n"
"layout(location = 2) in vec2   a_corner;                                                                              \n"
"layout(location = 3) in vec4   a_cornerColor;                                                                         \n"
"layout(location = 4) in float  a_cornerRadius;                                                                        \n"
"layout(location = 5) in vec2   a_tex_coord;                                                                           \n"
"layout(location = 6) in float  a_border_thickness;                                                                    \n"
"layout(location = 7) in vec4   a_border_color;                                                                        \n"
"                                                                                                                      \n"
"out vec4    out_color;                                                                                                \n"
"out vec2    out_pos;                                                                                                  \n"
"out vec2    out_rect_center;                                                                                          \n"
"out vec2    out_rect_dim;                                                                                             \n"
"out float   out_radius;                                                                                               \n"
"out vec2    out_tex_coord;                                                                                            \n"
"out float   out_border_thickness;                                                                                     \n"
"out vec4    out_border_color;                                                                                         \n"
"                                                                                                                      \n"
"uniform vec2 u_resolution;                                                                                            \n"
"                                                                                                                      \n"
"void main()                                                                                                           \n"
"{                                                                                                                     \n"
"    vec2 rect_center = a_rect[0] + (0.5 * (a_rect[1] - a_rect[0]));                                                   \n"
"                                                                                                                      \n"
"    gl_Position = vec4(((a_corner / u_resolution) * 2.0 - 1.0) * vec2(1, -1), 0.0, 1.0);                              \n"
"                                                                                                                      \n"
"    out_color            = a_cornerColor;                                                                             \n"
"    out_tex_coord        = a_tex_coord;                                                                               \n"
"    out_pos              = a_corner;                                                                                  \n"
"    out_rect_center      = rect_center;                                                                               \n"
"    out_rect_dim         = a_rect[1] - a_rect[0];                                                                     \n"
"    out_radius           = a_cornerRadius;                                                                            \n"
"    out_border_thickness = a_border_thickness;                                                                        \n"
"    out_border_color     = a_border_color;                                                                            \n"
"}                                                                                                                     \n"
;

////////////////////////////////////////////////////////////////
//
//
// Fragment shader
//
//
////////////////////////////////////////////////////////////////

static char *yo_opengl_fs_source =
"#version 330 core                                                                                                     \n"
"out vec4 FragColor;                                                                                                   \n"
"                                                                                                                      \n"
"in vec4     out_color;                                                                                                \n"
"in vec2     out_pos;                                                                                                  \n"
"in vec2     out_rect_center;                                                                                          \n"
"in vec2     out_rect_dim;                                                                                             \n"
"in float    out_radius;                                                                                               \n"
"in vec2     out_tex_coord;                                                                                            \n"
"in float    out_border_thickness;                                                                                     \n"
"in vec4     out_border_color;                                                                                         \n"
"                                                                                                                      \n"
"uniform vec2 u_resolution;                                                                                            \n"
"uniform sampler2D u_sampler;                                                                                          \n"
"                                                                                                                      \n"
"// https://www.rfleury.com/p/ui-part-6-rendering?utm_source=profile&utm_medium=reader2                                \n"
"float RoundedRectSDF(vec2 sample_pos,                                                                                 \n"
"                     vec2 rect_center,                                                                                \n"
"                     vec2 rect_half_size,                                                                             \n"
"                     float r)                                                                                         \n"
"{                                                                                                                     \n"
"    vec2 d2 = (abs(rect_center - sample_pos) - rect_half_size + vec2(r, r));                                          \n"
"    return min(max(d2.x, d2.y), 0.0) + length(max(d2, 0.0)) - r;                                                      \n"
"}                                                                                                                     \n"
"                                                                                                                      \n"
"void main()                                                                                                           \n"
"{                                                                                                                     \n"
"                                                                                                                      \n"
"    // Map to linear color space                                                                                      \n"
"    vec3 linear_interior_color;                                                                                       \n"
"    vec3 linear_border_color;                                                                                         \n"
"    {                                                                                                                 \n"
"        linear_interior_color.r = pow(out_color.r, 2.0);                                                              \n"
"        linear_interior_color.g = pow(out_color.g, 2.0);                                                              \n"
"        linear_interior_color.b = pow(out_color.b, 2.0);                                                              \n"
"                                                                                                                      \n"
"        linear_border_color.r = pow(out_border_color.r, 2.0);                                                         \n"
"        linear_border_color.g = pow(out_border_color.g, 2.0);                                                         \n"
"        linear_border_color.b = pow(out_border_color.b, 2.0);                                                         \n"
"    }                                                                                                                 \n"
"                                                                                                                      \n"
"    float dist_inner = RoundedRectSDF(out_pos, out_rect_center, out_rect_dim / 2, out_radius);                        \n"
"                                                                                                                      \n"
"    float dist_outer = -1000;                                                                                         \n"
"    if (out_border_thickness != 0)                                                                                    \n"
"    {                                                                                                                 \n"
"        dist_outer = RoundedRectSDF(out_pos, out_rect_center, (out_rect_dim / 2) - vec2(out_border_thickness, out_border_thickness), out_radius - out_border_thickness);"
"    }                                                                                                                 \n"
"                                                                                                                      \n"
"    if (out_tex_coord == vec2(0, 0))                                                                                  \n"
"    {                                                                                                                 \n"
"        float sdf_factor      = clamp(0.5 - dist_inner, 0, 1);                                                        \n"
"        float border_factor   = clamp(min(0.5-dist_inner, 0.5+dist_outer), 0, 1);                                     \n"
"        float interior_factor = clamp(0.5 - dist_outer, 0, 1);                                                        \n"
"                                                                                                                      \n"
"        // TODO(rune): Hack                                                                                           \n"
"        if (out_border_thickness == 0 && out_radius == 0)                                                             \n"
"        {                                                                                                             \n"
"            sdf_factor      = 1;                                                                                      \n"
"            border_factor   = 0;                                                                                      \n"
"            interior_factor = 1;                                                                                      \n"
"        }                                                                                                             \n"
"                                                                                                                      \n"
"        //FragColor = ((out_color * interior_factor) + (out_border_color * border_factor)) * sdf_factor;              \n"
"                                                                                                                      \n"
"        FragColor = out_color;                                                                                        \n"
"        FragColor.a *= sdf_factor;                                                                                    \n"
"                                                                                                                      \n"
"        FragColor = vec4(mix(linear_border_color, linear_interior_color, interior_factor - border_factor), 1);        \n"
"        FragColor.a *= sdf_factor * out_color.a;                                                                      \n"
"                                                                                                                      \n"
"        FragColor.r = sqrt(FragColor.r);                                                                              \n"
"        FragColor.g = sqrt(FragColor.g);                                                                              \n"
"        FragColor.b = sqrt(FragColor.b);                                                                              \n"
"    }                                                                                                                 \n"
"    else                                                                                                              \n"
"    {                                                                                                                 \n"
"        FragColor = out_color * texture(u_sampler, out_tex_coord);                                                    \n"
"    }                                                                                                                 \n"
"                                                                                                                      \n"
"    //FragColor = vec4(1 - out_tex_coord, 0, 1);                                                                      \n"
"}                                                                                                                     \n"
;

static void yo_backend_opengl_startup(yo_backend_opengl_t *backend)
{
        // TODO(rune): Error handling
    yo_array_create(&backend->vertex_array, 256, true);
    yo_array_create(&backend->index_array, 256, false);


    //
    // Compile vertex shader
    //
    {
        backend->vs = backend->glCreateShader(GL_VERTEX_SHADER);
        backend->glShaderSource(backend->vs, 1, &yo_opengl_vs_source, NULL);
        backend->glCompileShader(backend->vs);

        int  success;
        char info_log[512];
        backend->glGetShaderiv(backend->vs, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            backend->glGetShaderInfoLog(backend->vs, 512, NULL, info_log);
            printf("Vertex shader compilation error %s", info_log);
        }
    }

    //
    // Compile fragment shader
    //
    {
        backend->fs = backend->glCreateShader(GL_FRAGMENT_SHADER);
        backend->glShaderSource(backend->fs, 1, &yo_opengl_fs_source, NULL);
        backend->glCompileShader(backend->fs);

        int  success;
        char info_log[512];
        backend->glGetShaderiv(backend->fs, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            backend->glGetShaderInfoLog(backend->fs, 512, NULL, info_log);
            printf("Fragment shader compilation error %s", info_log);
        }
    }

    //
    // Create program
    //
    {
        backend->program = backend->glCreateProgram();
        backend->glAttachShader(backend->program, backend->vs);
        backend->glAttachShader(backend->program, backend->fs);
        backend->glLinkProgram(backend->program);

        int  success;
        char info_log[512];
        backend->glGetProgramiv(backend->program, GL_LINK_STATUS, &success);
        if (!success)
        {
            backend->glGetProgramInfoLog(backend->program, 512, NULL, info_log);
            printf("Fragment shader compilation error %s", info_log);
        }

        // NOTE(rune): Shader objects are no longer needed after they have been linked to the program objects.
        backend->glDeleteShader(backend->vs);
        backend->glDeleteShader(backend->fs);

        backend->program_u_resolution = backend->glGetUniformLocation(backend->program, "u_resolution");
    }


    //
    // Create buffers
    //
    {
        backend->glGenVertexArrays(1, &backend->VAO);
        backend->glGenBuffers(1, &backend->VBO);
        backend->glGenBuffers(1, &backend->EBO);

        backend->glBindVertexArray(backend->VAO);

        backend->glBindBuffer(GL_ARRAY_BUFFER, backend->VBO);
        backend->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backend->EBO);

        // aRect[0]
        backend->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, rect_p0)));
        backend->glEnableVertexAttribArray(0);

        // aRect[1]
        backend->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, rect_p1)));
        backend->glEnableVertexAttribArray(1);

        // aCorner
        backend->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, corner)));
        backend->glEnableVertexAttribArray(2);

        // aCornerColor
        backend->glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, corner_color)));
        backend->glEnableVertexAttribArray(3);

        // aCornerColor
        backend->glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, corner_radius)));
        backend->glEnableVertexAttribArray(4);

        // aTexCoord
        backend->glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, tex_coord)));
        backend->glEnableVertexAttribArray(5);

        // aBorderThickness
        backend->glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, border_thickness)));
        backend->glEnableVertexAttribArray(6);

        // aBorderColor
        backend->glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, border_color)));
        backend->glEnableVertexAttribArray(7);
    }

    //
    // Blending
    //
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    }

    //
    // Z buffer
    //
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
}

static void yo_backend_opengl_shutdown(yo_backend_opengl_t *backend)
{
    yo_array_destroy(&backend->vertex_array);
    yo_array_destroy(&backend->index_array);

    // TODO(rune): Properly un-initialize opengl
}

static bool yo_backend_opengl_reserve_vert_idx(yo_backend_opengl_t *backend, size_t vert_count, size_t idx_count, yo_vert_t **vertices, yo_idx_t **indices)
{
    bool ok = true;

    ok &= yo_array_reserve(&backend->vertex_array, vert_count, false);
    ok &= yo_array_reserve(&backend->index_array, idx_count, false);

    if (ok)
    {
        backend->idx = (yo_idx_t)backend->vertex_array.count;
        *vertices = yo_array_add(&backend->vertex_array, vert_count, false);
        *indices = yo_array_add(&backend->index_array, idx_count, false);
    }

    return ok;
}

static void yo_backend_opengl_render_frame(yo_backend_opengl_t *backend, yo_render_info_t *info)
{
    YO_PROFILE_BEGIN(yo_backend_opengl_render_frame);

    yo_array_reset(&backend->vertex_array, true);
    yo_array_reset(&backend->index_array, true);

    // TODO(rune): Currently we use the rounded aabb shader for everything, even simple triangles.
    // While this makes the implementation simpler, it is quite wasteful.

    YO_PROFILE_BEGIN(openg_loop);
    for (size_t i = 0; i < info->draw_cmds_count; i++)
    {
        yo_draw_cmd_t *cmd = &info->draw_cmds[i];
        switch (cmd->type)
        {
            case YO_DRAW_CMD_AABB:
            {
                yo_vert_t *vertices = NULL;
                yo_idx_t *indices = NULL;
                if (!yo_backend_opengl_reserve_vert_idx(backend, 4, 6, &vertices, &indices))
                {
                    break;
                }

                yo_v2f_t clipped_p0 = yo_v2f(YO_MAX(cmd->aabb.p0.x, cmd->aabb.clip_p0.x), YO_MAX(cmd->aabb.p0.y, cmd->aabb.clip_p0.y));
                yo_v2f_t clipped_p1 = yo_v2f(YO_MIN(cmd->aabb.p1.x, cmd->aabb.clip_p1.x), YO_MIN(cmd->aabb.p1.y, cmd->aabb.clip_p1.y));

                if (!((clipped_p0.x <= clipped_p1.x) && (clipped_p0.y <= clipped_p1.y)))
                {
                    break; // NOTE(rune): Outside of clip rect
                }
                else if (!(yo_v2f_equal(cmd->aabb.p0, clipped_p0) && yo_v2f_equal(cmd->aabb.p1, clipped_p1)))
                {
                    // NOTE(rune): Adjust UV coords if clipped

                    assert(clipped_p0.x <= clipped_p1.x);
                    assert(clipped_p0.y <= clipped_p1.y);

                    yo_v2f_t cof0 = yo_v2f((float)(clipped_p0.x - cmd->aabb.p0.x) / (float)(cmd->aabb.p1.x - cmd->aabb.p0.x),
                                           (float)(clipped_p0.y - cmd->aabb.p0.y) / (float)(cmd->aabb.p1.y - cmd->aabb.p0.y));

                    yo_v2f_t cof1 = yo_v2f((float)(clipped_p1.x - cmd->aabb.p1.x) / (float)(cmd->aabb.p1.x - cmd->aabb.p0.x),
                                           (float)(clipped_p1.y - cmd->aabb.p1.y) / (float)(cmd->aabb.p1.y - cmd->aabb.p0.y));

                    yo_v2f_t uv_dim = yo_v2f(cmd->aabb.uv1.x - cmd->aabb.uv0.x,
                                             cmd->aabb.uv1.y - cmd->aabb.uv0.y);

                    cmd->aabb.uv0.x += uv_dim.x * cof0.x;
                    cmd->aabb.uv0.y += uv_dim.y * cof0.y;

                    cmd->aabb.uv1.x += uv_dim.x * cof1.x;
                    cmd->aabb.uv1.y += uv_dim.y * cof1.y;
                }

                //
                // Vertices
                //

                // NOTE(rune): Set attributes which are the same on all 4 vertices
                yo_vert_t vertex_data        = { 0 };
                vertex_data.rect_p0 = cmd->aabb.p0;
                vertex_data.rect_p1 = cmd->aabb.p1;
                vertex_data.rect_p0 = cmd->aabb.p0;
                vertex_data.rect_p1 = cmd->aabb.p1;
                vertex_data.rect_p0 = cmd->aabb.p0;
                vertex_data.rect_p1 = cmd->aabb.p1;
                vertex_data.border_thickness = cmd->aabb.border_thickness;
                vertex_data.border_color     = cmd->aabb.border_color;

                vertices[0] = vertex_data;
                vertices[1] = vertex_data;
                vertices[2] = vertex_data;
                vertices[3] = vertex_data;

                // NOTE(rune): Set attributes which are specific per vertex.
                vertices[0].corner_radius = cmd->aabb.radius.corner[YO_CORNER_TOP_LEFT];
                vertices[1].corner_radius = cmd->aabb.radius.corner[YO_CORNER_TOP_RIGHT];
                vertices[2].corner_radius = cmd->aabb.radius.corner[YO_CORNER_BOTTOM_LEFT];
                vertices[3].corner_radius = cmd->aabb.radius.corner[YO_CORNER_BOTTOM_RIGHT];
                vertices[0].corner_color  = cmd->aabb.color.corner[YO_CORNER_TOP_LEFT];
                vertices[1].corner_color  = cmd->aabb.color.corner[YO_CORNER_TOP_RIGHT];
                vertices[2].corner_color  = cmd->aabb.color.corner[YO_CORNER_BOTTOM_LEFT];
                vertices[3].corner_color  = cmd->aabb.color.corner[YO_CORNER_BOTTOM_RIGHT];
                vertices[0].corner        = yo_v2f((float)(clipped_p0.x), (float)(clipped_p0.y));
                vertices[1].corner        = yo_v2f((float)(clipped_p0.x), (float)(clipped_p1.y));
                vertices[2].corner        = yo_v2f((float)(clipped_p1.x), (float)(clipped_p0.y));
                vertices[3].corner        = yo_v2f((float)(clipped_p1.x), (float)(clipped_p1.y));
                vertices[0].tex_coord     = yo_v2f(cmd->aabb.uv0.x, cmd->aabb.uv0.y);
                vertices[1].tex_coord     = yo_v2f(cmd->aabb.uv0.x, cmd->aabb.uv1.y);
                vertices[2].tex_coord     = yo_v2f(cmd->aabb.uv1.x, cmd->aabb.uv0.y);
                vertices[3].tex_coord     = yo_v2f(cmd->aabb.uv1.x, cmd->aabb.uv1.y);

                //
                // Indices
                //

                indices[0] = backend->idx + 0;
                indices[1] = backend->idx + 1;
                indices[2] = backend->idx + 2;

                indices[3] = backend->idx + 1;
                indices[4] = backend->idx + 2;
                indices[5] = backend->idx + 3;

            } break;

            case YO_DRAW_CMD_TRI:
            {
                yo_vert_t *vertices = NULL;
                yo_idx_t  *indices  = NULL;
                if (!yo_backend_opengl_reserve_vert_idx(backend, 3, 3, &vertices, &indices))
                {
                    break;
                }

                vertices[0].corner       = cmd->tri.p[0];
                vertices[1].corner       = cmd->tri.p[1];
                vertices[2].corner       = cmd->tri.p[2];
                vertices[0].corner_color = cmd->tri.color[0];
                vertices[1].corner_color = cmd->tri.color[1];
                vertices[2].corner_color = cmd->tri.color[2];

                vertices[0].rect_p0 = yo_v2f(0, 0);
                vertices[1].rect_p1 = yo_v2f(0, 0);
                vertices[2].rect_p0 = yo_v2f(0, 0);
                vertices[0].rect_p1 = yo_v2f(0, 0);
                vertices[1].rect_p0 = yo_v2f(0, 0);
                vertices[2].rect_p1 = yo_v2f(0, 0);

                indices[0] = backend->idx + 0;
                indices[1] = backend->idx + 1;
                indices[2] = backend->idx + 2;

            } break;

            case YO_DRAW_CMD_QUAD:
            {
                yo_vert_t *vertices = NULL;
                yo_idx_t  *indices  = NULL;
                if (!yo_backend_opengl_reserve_vert_idx(backend, 4, 6, &vertices, &indices))
                {
                    break;
                }

                vertices[0].corner       = cmd->quad.p[0];
                vertices[1].corner       = cmd->quad.p[1];
                vertices[2].corner       = cmd->quad.p[2];
                vertices[3].corner       = cmd->quad.p[3];
                vertices[0].corner_color = cmd->quad.color[0];
                vertices[1].corner_color = cmd->quad.color[1];
                vertices[2].corner_color = cmd->quad.color[2];
                vertices[3].corner_color = cmd->quad.color[3];

                vertices[0].rect_p0 = yo_v2f(0, 0);
                vertices[1].rect_p1 = yo_v2f(0, 0);
                vertices[2].rect_p0 = yo_v2f(0, 0);
                vertices[3].rect_p0 = yo_v2f(0, 0);
                vertices[0].rect_p1 = yo_v2f(0, 0);
                vertices[1].rect_p0 = yo_v2f(0, 0);
                vertices[2].rect_p1 = yo_v2f(0, 0);
                vertices[3].rect_p1 = yo_v2f(0, 0);

                indices[0] = backend->idx + 0;
                indices[1] = backend->idx + 1;
                indices[2] = backend->idx + 2;

                indices[3] = backend->idx + 1;
                indices[4] = backend->idx + 2;
                indices[5] = backend->idx + 3;

            } break;

            default:
            {
                YO_ASSERT(!"Unknown draw command.");
            } break;
        }
    }
    YO_PROFILE_END(openg_loop);


    //
    // Upload vertex and index buffers
    //

    YO_PROFILE_BEGIN(opengl_upload_vertices);
    backend->glBindBuffer(GL_ARRAY_BUFFER, backend->VBO);
    backend->glBufferData(GL_ARRAY_BUFFER, yo_array_size(&backend->vertex_array), backend->vertex_array.elems, GL_STREAM_DRAW);
    YO_PROFILE_END(opengl_upload_vertices);

    YO_PROFILE_BEGIN(opengl_upload_indicies);
    backend->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backend->EBO);
    backend->glBufferData(GL_ELEMENT_ARRAY_BUFFER, yo_array_size(&backend->index_array), backend->index_array.elems, GL_STREAM_DRAW);
    YO_PROFILE_END(opengl_upload_indicies);

    //
    // Upload texture
    //

    YO_PROFILE_BEGIN(opengl_upload_texture);
    // TODO(rune): Multiple textures
    glBindTexture(GL_TEXTURE_2D, info->tex.id);

    if (info->tex.dirty)
    {
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     info->tex.dim.x,
                     info->tex.dim.y,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     info->tex.pixels);

        info->tex.dirty = false;
    }

    GLint swizzle[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    YO_PROFILE_END(opengl_upload_texture);

    //
    // Render
    //

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0f);

    glViewport(0, 0, info->w, info->h);

#if 1
    backend->glUseProgram(backend->program);
    backend->glUniform2f(backend->program_u_resolution, (float)(info->w), (float)(info->h));

    glBindTexture(GL_TEXTURE_2D, 42);
    backend->glBindVertexArray(backend->VAO);
    glDrawElements(GL_TRIANGLES, (int32_t)(backend->index_array.count), GL_UNSIGNED_INT, 0);

#else
    glLoadIdentity();

    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f( 1.0f,  1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f( 1.0f, -1.0f, 0.0f);
    glVertex3f( 1.0f,  1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();
#endif

    YO_PROFILE_END(yo_backend_opengl_render_frame);
}
