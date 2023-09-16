#pragma once

////////////////////////////////////////////////////////////////
//
//
// OpenGL specific backend code, independent of platform.
//
//
////////////////////////////////////////////////////////////////

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
    GLuint program_u_resolution;

    yo_array(yo_vert_t) vertex_array;
    yo_array(yo_idx_t)  index_array;

    yo_idx_t idx;

    PFNGLGENBUFFERSPROC              glGenBuffers;
    PFNGLBINDBUFFERPROC              glBindBuffer;
    PFNGLBUFFERDATAPROC              glBufferData;
    PFNGLCREATESHADERPROC            glCreateShader;
    PFNGLSHADERSOURCEPROC            glShaderSource;
    PFNGLCOMPILESHADERPROC           glCompileShader;
    PFNGLGETSHADERIVPROC             glGetShaderiv;
    PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
    PFNGLCREATEPROGRAMPROC           glCreateProgram;
    PFNGLATTACHSHADERPROC            glAttachShader;
    PFNGLLINKPROGRAMPROC             glLinkProgram;
    PFNGLGETPROGRAMIVPROC            glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
    PFNGLUSEPROGRAMPROC              glUseProgram;
    PFNGLDELETESHADERPROC            glDeleteShader;
    PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
    PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
    PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
    PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
    PFNGLUNIFORM4FPROC               glUniform4f;
    PFNGLUNIFORM2FPROC               glUniform2f;
    PFNGLUNIFORM1IPROC               glUniform1i;
};

static void yo_backend_opengl_startup(yo_backend_opengl_t *backend);
static void yo_backend_opengl_shutdown(yo_backend_opengl_t *backend);
static void yo_backend_opengl_render_frame(yo_backend_opengl_t *backend, yo_render_info_t *info);
