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

////////////////////////////////////////////////////////////////
//
//
// OpenGL function pointers
//
//
////////////////////////////////////////////////////////////////

// TODO(rune): Wrap in struct prefixed with yo
static PFNGLGENBUFFERSPROC              glGenBuffers;
static PFNGLBINDBUFFERPROC              glBindBuffer;
static PFNGLBUFFERDATAPROC              glBufferData;
static PFNGLCREATESHADERPROC            glCreateShader;
static PFNGLSHADERSOURCEPROC            glShaderSource;
static PFNGLCOMPILESHADERPROC           glCompileShader;
static PFNGLGETSHADERIVPROC             glGetShaderiv;
static PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
static PFNGLCREATEPROGRAMPROC           glCreateProgram;
static PFNGLATTACHSHADERPROC            glAttachShader;
static PFNGLLINKPROGRAMPROC             glLinkProgram;
static PFNGLGETPROGRAMIVPROC            glGetProgramiv;
static PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
static PFNGLUSEPROGRAMPROC              glUseProgram;
static PFNGLDELETESHADERPROC            glDeleteShader;
static PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
static PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
static PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
static PFNGLUNIFORM4FPROC               glUniform4f;
static PFNGLUNIFORM2FPROC               glUniform2f;
static PFNGLUNIFORM1IPROC               glUniform1i;

// See: https://stackoverflow.com/a/589232

static bool yo_backend_opengl_win32_check_ext(const char *extension_name)
{
    bool ret = false;

    PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

    const char *extension_string = wglGetExtensionsStringEXT();
    if (strstr(extension_string, extension_name))
    {
        ret = true;
    }

    return ret;
}

static void *yo_backend_opengl_win32_load_function(char *name)
{
    void *p = (void *)wglGetProcAddress(name);

    if (p == NULL)
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        YO_ASSERT(module);

        p = (void *)GetProcAddress(module, name);
    }

    return p;
}

#if 0
static bool yo_backend_opengl_win32_load_shader_file(char *file_name, char **content, size_t *content_size)
{
    bool ret = false;

    HANDLE file =  CreateFileA(file_name,
                               FILE_READ_ACCESS,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

    if (file != INVALID_HANDLE_VALUE)
    {
        DWORD file_size =  GetFileSize(file, NULL);
        if (file_size != INVALID_FILE_SIZE)
        {
            void *file_content_buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, file_size);
            if (file_content_buffer)
            {
                DWORD bytes_read = 0;
                if (ReadFile(file, file_content_buffer, file_size, &bytes_read, NULL))
                {
                    *content      = file_content_buffer;
                    *content_size = bytes_read;
                    ret = true;
                }
                else
                {
                    printf("ReadFile failed (%i)\n", GetLastError());
                }
            }
            else
            {
                printf("HeapAlloc failed (%i)\n", GetLastError());
            }
        }
        else
        {
            printf("GetFileSize failed (%i)\n", GetLastError());
        }
    }
    else
    {
        printf("CreateFileA failed (%i)\n", GetLastError());
    }

    CloseHandle(file);

    return ret;
}
#endif

static void yo_backend_opengl_win32_startup(yo_backend_opengl_t *state, HWND window)
{
    // TODO(rune): Error handling
    yo_array_create(&state->vertex_array, 256, true);
    yo_array_create(&state->index_array, 256, false);

    HDC window_dc = GetDC(window);

    PIXELFORMATDESCRIPTOR desired_pixel_format = { 0 };
    desired_pixel_format.nSize = sizeof(desired_pixel_format);
    desired_pixel_format.nVersion = 1;
    desired_pixel_format.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desired_pixel_format.cColorBits = 32;
    desired_pixel_format.cAlphaBits = 8;
    desired_pixel_format.iLayerType = PFD_MAIN_PLANE;

    uint32_t suggested_pixel_format_index = ChoosePixelFormat(window_dc, &desired_pixel_format);
    PIXELFORMATDESCRIPTOR suggestedPixelFormat = { 0 };

    DescribePixelFormat(window_dc, suggested_pixel_format_index, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(window_dc, suggested_pixel_format_index, &suggestedPixelFormat);

    HGLRC glrc = wglCreateContext(window_dc);

    if (wglMakeCurrent(window_dc, glrc))
    {
        //
        // Load OpenGL extensions
        //
        {
            glGenBuffers                = (PFNGLGENBUFFERSPROC)(yo_backend_opengl_win32_load_function("glGenBuffers"));
            glBindBuffer                = (PFNGLBINDBUFFERPROC)(yo_backend_opengl_win32_load_function("glBindBuffer"));
            glBufferData                = (PFNGLBUFFERDATAPROC)(yo_backend_opengl_win32_load_function("glBufferData"));
            glCreateShader              = (PFNGLCREATESHADERPROC)(yo_backend_opengl_win32_load_function("glCreateShader"));
            glShaderSource              = (PFNGLSHADERSOURCEPROC)(yo_backend_opengl_win32_load_function("glShaderSource"));
            glCompileShader             = (PFNGLCOMPILESHADERPROC)(yo_backend_opengl_win32_load_function("glCompileShader"));
            glGetShaderiv               = (PFNGLGETSHADERIVPROC)(yo_backend_opengl_win32_load_function("glGetShaderiv"));
            glGetShaderInfoLog          = (PFNGLGETSHADERINFOLOGPROC)(yo_backend_opengl_win32_load_function("glGetShaderInfoLog"));
            glCreateProgram             = (PFNGLCREATEPROGRAMPROC)(yo_backend_opengl_win32_load_function("glCreateProgram"));
            glAttachShader              = (PFNGLATTACHSHADERPROC)(yo_backend_opengl_win32_load_function("glAttachShader"));
            glGetProgramiv              = (PFNGLGETPROGRAMIVPROC)(yo_backend_opengl_win32_load_function("glGetProgramiv"));
            glGetProgramInfoLog         = (PFNGLGETPROGRAMINFOLOGPROC)(yo_backend_opengl_win32_load_function("glGetProgramInfoLog"));
            glUseProgram                = (PFNGLUSEPROGRAMPROC)(yo_backend_opengl_win32_load_function("glUseProgram"));
            glDeleteShader              = (PFNGLDELETESHADERPROC)(yo_backend_opengl_win32_load_function("glDeleteShader"));
            glVertexAttribPointer       = (PFNGLVERTEXATTRIBPOINTERPROC)(yo_backend_opengl_win32_load_function("glVertexAttribPointer"));
            glEnableVertexAttribArray   = (PFNGLENABLEVERTEXATTRIBARRAYPROC)(yo_backend_opengl_win32_load_function("glEnableVertexAttribArray"));
            glGenVertexArrays           = (PFNGLGENVERTEXARRAYSPROC)(yo_backend_opengl_win32_load_function("glGenVertexArrays"));
            glBindVertexArray           = (PFNGLBINDVERTEXARRAYPROC)(yo_backend_opengl_win32_load_function("glBindVertexArray"));
            glLinkProgram               = (PFNGLLINKPROGRAMPROC)(yo_backend_opengl_win32_load_function("glLinkProgram"));
            glGetUniformLocation        = (PFNGLGETUNIFORMLOCATIONPROC)(yo_backend_opengl_win32_load_function("glGetUniformLocation"));
            glUniform4f                 = (PFNGLUNIFORM4FPROC)(yo_backend_opengl_win32_load_function("glUniform4f"));
            glUniform2f                 = (PFNGLUNIFORM2FPROC)(yo_backend_opengl_win32_load_function("glUniform2f"));
            glUniform1i                 = (PFNGLUNIFORM1IPROC)(yo_backend_opengl_win32_load_function("glUniform1i"));
        }

        //
        // Compile vertex shader
        //
        {
            state->vs = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(state->vs, 1, &yo_opengl_vs_source, NULL);
            glCompileShader(state->vs);

            int  success;
            char info_log[512];
            glGetShaderiv(state->vs, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                glGetShaderInfoLog(state->vs, 512, NULL, info_log);
                printf("Vertex shader compilation error %s", info_log);
            }
        }

        //
        // Compile fragment shader
        //
        {
            state->fs = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(state->fs, 1, &yo_opengl_fs_source, NULL);
            glCompileShader(state->fs);

            int  success;
            char info_log[512];
            glGetShaderiv(state->fs, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                glGetShaderInfoLog(state->fs, 512, NULL, info_log);
                printf("Fragment shader compilation error %s", info_log);
            }
        }

        //
        // Create program
        //
        {
            state->program = glCreateProgram();
            glAttachShader(state->program, state->vs);
            glAttachShader(state->program, state->fs);
            glLinkProgram(state->program);

            int  success;
            char info_log[512];
            glGetProgramiv(state->program, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(state->program, 512, NULL, info_log);
                printf("Fragment shader compilation error %s", info_log);
            }

            // NOTE(rune): Shader objects are no longer needed after they have been linked to the program objects.
            glDeleteShader(state->vs);
            glDeleteShader(state->fs);
        }


        //
        // Create buffers
        //
        {
            glGenVertexArrays(1, &state->VAO);
            glGenBuffers(1, &state->VBO);
            glGenBuffers(1, &state->EBO);

            glBindVertexArray(state->VAO);

            glBindBuffer(GL_ARRAY_BUFFER, state->VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state->EBO);

            // aRect[0]
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, rect_p0)));
            glEnableVertexAttribArray(0);

            // aRect[1]
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, rect_p1)));
            glEnableVertexAttribArray(1);

            // aCorner
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, corner)));
            glEnableVertexAttribArray(2);

            // aCornerColor
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, corner_color)));
            glEnableVertexAttribArray(3);

            // aCornerColor
            glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, corner_radius)));
            glEnableVertexAttribArray(4);

            // aTexCoord
            glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, tex_coord)));
            glEnableVertexAttribArray(5);

            // aBorderThickness
            glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, border_thickness)));
            glEnableVertexAttribArray(6);

            // aBorderColor
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(yo_vert_t), (void *)(offsetof(yo_vert_t, border_color)));
            glEnableVertexAttribArray(7);
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

        //
        // V-Sync
        //
        {
            if (yo_backend_opengl_win32_check_ext("WGL_EXT_swap_control"))
            {
                PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
                wglSwapIntervalEXT(1);
            }
        }
    }
    else
    {
        YO_ASSERT(false);
    }

    ReleaseDC(window, window_dc);
}

static void yo_backend_opengl_win32_shutdown(yo_backend_opengl_t *state, HWND window)
{
    YO_UNUSED(window);

    yo_array_destroy(&state->vertex_array);
    yo_array_destroy(&state->index_array);

    // TODO(rune): Properly un-initialize opengl
}

static bool yo_backend_opengl_vert_idx(yo_backend_opengl_t *state, size_t vert_count, size_t idx_count, yo_vert_t **vertices, yo_idx_t **indices)
{
    bool ok = true;

    ok &= yo_array_reserve(&state->vertex_array, vert_count, false);
    ok &= yo_array_reserve(&state->index_array, idx_count, false);

    if (ok)
    {
        state->idx = (yo_idx_t)state->vertex_array.count;
        *vertices = yo_array_add(&state->vertex_array, vert_count, false);
        *indices = yo_array_add(&state->index_array, idx_count, false);
    }

    return ok;
}

static void yo_backend_opengl_render(yo_backend_opengl_t *state, yo_render_info_t *info)
{
    // DEBUG(rune):
    if (info->draw_cmds_count == 0)
    {
        __nop();
    }

    yo_array_reset(&state->vertex_array, true);
    yo_array_reset(&state->index_array, true);

    // TODO(rune): Currently we use the rounded aabb shader for everything, even simple triangles.
    // While this makes the implementation simpler, it is quite wasteful.

    for (size_t i = 0; i < info->draw_cmds_count; i++)
    {
        yo_draw_cmd_t *cmd = &info->draw_cmds[i];
        switch (cmd->type)
        {
            case YO_DRAW_CMD_AABB:
            {
                yo_vert_t *vertices = NULL;
                yo_idx_t *indices = NULL;
                if (!yo_backend_opengl_vert_idx(state, 4, 6, &vertices, &indices))
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

                    YO_ASSERT(clipped_p0.x <= clipped_p1.x);
                    YO_ASSERT(clipped_p0.y <= clipped_p1.y);

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

                indices[0] = state->idx + 0;
                indices[1] = state->idx + 1;
                indices[2] = state->idx + 2;

                indices[3] = state->idx + 1;
                indices[4] = state->idx + 2;
                indices[5] = state->idx + 3;

            } break;

            case YO_DRAW_CMD_TRI:
            {
                yo_vert_t *vertices = NULL;
                yo_idx_t *indices = NULL;
                if (!yo_backend_opengl_vert_idx(state, 3, 3, &vertices, &indices))
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

                indices[0] = state->idx + 0;
                indices[1] = state->idx + 1;
                indices[2] = state->idx + 2;

            } break;

            case YO_DRAW_CMD_QUAD:
            {
                yo_vert_t *vertices = NULL;
                yo_idx_t *indices = NULL;
                if (!yo_backend_opengl_vert_idx(state, 4, 6, &vertices, &indices))
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

                indices[0] = state->idx + 0;
                indices[1] = state->idx + 1;
                indices[2] = state->idx + 2;

                indices[3] = state->idx + 1;
                indices[4] = state->idx + 2;
                indices[5] = state->idx + 3;

            } break;

            default:
            {
                YO_ASSERT(!"Unknown draw command.");
            } break;
        }
    }

    //
    // Upload vertex and index buffers
    //

    glBindBuffer(GL_ARRAY_BUFFER, state->VBO);
    glBufferData(GL_ARRAY_BUFFER, yo_array_size(&state->vertex_array), state->vertex_array.elems, GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, yo_array_size(&state->index_array), state->index_array.elems, GL_STREAM_DRAW);

    //
    // Upload texture
    //

    // TODO(rune): Multiple textures
    glBindTexture(GL_TEXTURE_2D, info->tex.id);

    if (info->tex.dirty)
    {
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     info->tex.dims.x,
                     info->tex.dims.y,
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

    //
    // Render
    //

    glViewport(0, 0, info->w, info->h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0f);

    int32_t resolution_uniform_location = glGetUniformLocation(state->program, "u_resolution");

    glUseProgram(state->program);
    glUniform2f(resolution_uniform_location, (float)(info->w), (float)(info->h));

    glBindTexture(GL_TEXTURE_2D, 42);
    glBindVertexArray(state->VAO);
    glDrawElements(GL_TRIANGLES, (int32_t)(state->index_array.count), GL_UNSIGNED_INT, 0);
}
