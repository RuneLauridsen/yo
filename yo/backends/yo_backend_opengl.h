#pragma once

YO_TYPEDEF_ARRAY(yo_vert_t);
YO_TYPEDEF_ARRAY(yo_idx_t);

typedef struct yo_backend_opengl yo_backend_opengl_t;
struct yo_backend_opengl
{
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;

    GLuint vs;
    GLuint fs;
    GLuint program;

    yo_array(yo_vert_t) vertex_array;
    yo_array(yo_idx_t)  index_array;

    yo_idx_t idx;
};

// TODO(rune): #ifdef PLATFORM_IS_WIN32
static void yo_backend_opengl_win32_startup(yo_backend_opengl_t *state, HWND window);
static void yo_backend_opengl_win32_shutdown(yo_backend_opengl_t *state, HWND window);
// TODO(rune): #endif

static void yo_backend_opengl_render(yo_backend_opengl_t *state, yo_render_info_t *info);
