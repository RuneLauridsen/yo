#define _CRT_SECURE_NO_WARNINGS

//
// Standard library
//

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

//
// Windows & OpenGL
//

#include <windows.h>
#include <gl/gl.h>
#include "../yo/thirdparty/khrplatform.h"
#include "../yo/thirdparty/glext.h"
#include "../yo/thirdparty/wglext.h"
#pragma comment ( lib, "opengl32.lib" )

//
// yo
//

#define YO_API          __declspec(dllexport)
#include "../yo/yo.h"
#include "../yo/yo.c"
#include "../yo/yo_memory.h" // NOTE(rune): OpenGL backend uses yo_array
#include "../yo/yo_memory.c"

//
// Platform & backend
//

#define YO_PLATFORM_API __declspec(dllexport)
#include "../yo/backends/yo_backend_opengl.h"
#include "../yo/backends/yo_backend_opengl.c"
#include "../yo/platforms/yo_platform_win32_opengl.h"
#include "../yo/platforms/yo_platform_win32_opengl.c"
