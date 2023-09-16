////////////////////////////////////////////////////////////////
//
//
// Standard library
//
//
////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////
//
//
// Yo
//
//
////////////////////////////////////////////////////////////////

#define YO_PROFILE_BEGIN(...)
#define YO_PROFILE_END(...)

#include "../yo/yo.h"
#include "../yo/yo.c"

////////////////////////////////////////////////////////////////
//
//
// Win32+OpenGL platform
//
//
////////////////////////////////////////////////////////////////

#pragma comment ( lib, "opengl32.lib" )

#include <gl/gl.h>
#include <windows.h>
#include "../yo/thirdparty/glext.h"
#include "../yo/thirdparty/wglext.h"
#include "../yo/impl/yo_backend_opengl.h"
#include "../yo/impl/yo_backend_opengl.c"
#include "../yo/impl/yo_platform_win32.h"
#include "../yo/impl/yo_platform_win32.c"
#include "../yo/impl/yo_impl_win32_opengl.h"
#include "../yo/impl/yo_impl_win32_opengl.c"

////////////////////////////////////////////////////////////////
//
//
// Entrypoint
//
//
////////////////////////////////////////////////////////////////

#include "yo_layout_experiments.c"

int main()
{

    yo_impl_win32_opengl_t impl = { 0 };
    yo_impl_win32_opengl_startup(&impl, 800, 600);

    while (impl.platform.running)
    {
        yo_impl_win32_opengl_begin_frame(&impl);

        yo_exp(yo_v2i(impl.platform.render_info.w, impl.platform.render_info.h));

        yo_impl_win32_opengl_end_frame(&impl);
    }

    yo_impl_win32_opengl_shutdown(&impl);

    return 0;
}
