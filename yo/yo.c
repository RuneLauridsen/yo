
#pragma once

#if 1
#define YO_FONT_BACKEND_STB
#else
#define YO_FONT_BACKEND_FREETYPE
#endif

//
// Fasthash
//

#include "thirdparty/fasthash.h"
#include "thirdparty/fasthash.c"

//
// Font backend
//

#include "yo_font_backend.h"
#ifdef YO_FONT_BACKEND_STB
#   define STBTT_STATIC
#   define STB_TRUETYPE_IMPLEMENTATION
#   include "thirdparty/stb_truetype.h"
#   include "yo_font_backend_stb.h"
#   include "yo_font_backend_stb.c"
#endif
#ifdef YO_FONT_BACKEND_FREETYPE
#   pragma comment ( lib, "freetype.lib" )
#   include <ft2build.h>
#   include FT_FREETYPE_H
#   include "yo_font_backend_freetype.h"
#   include "yo_font_backend_freetype.c"
#endif

//
// Static data
//

#include "yo_font_data.h"

//
// Internal headers
//

#include "yo_memory.h"
#include "yo_string.h"
#include "yo_atlas.h"
#include "yo_font_table.h"
#include "yo_svg.h"
#include "yo_internal.h"

//
// Implementation
//

#include "yo_memory.c"
#include "yo_string.c"
#include "yo_atlas.c"
#include "yo_font_table.c"
#include "yo_svg.c"
#include "yo_core.c"
#include "yo_widgets.c"
#include "yo_demo.c"
