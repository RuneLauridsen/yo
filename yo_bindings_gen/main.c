#define _CRT_SECURE_NO_WARNINGS

//
// Standard library
//

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

//
// Yo
//

#define YO_ASSERT assert
#include "../yo/yo_memory.h"
#include "../yo/yo_string.h"
#include "../yo/yo_file.h"

#include "../yo/yo_memory.c"
#include "../yo/yo_string.c"
#include "../yo/yo_file.c"

//
// Bindings gen
//

#define countof(x) (sizeof(x) / sizeof(x[0]))
#include "yo_bindings_lex.h"
#include "yo_bindings_lex.c"
#include "yo_bindings_parse.h"
#include "yo_bindings_parse.c"
#include "yo_bindings_csharp.h"
#include "yo_bindings_csharp.c"

int main()
{
    char *file_names[] =
    {
        "../yo/yo_core.h",
        "../yo/yo_widgets.h",
        "../yo/yo_demo.h",
        "../yo/impl/yo_impl_win32_opengl.h"
    };



    yo_arena_t arena;
    yo_arena_create(&arena, 0, 0, 0);



    for (uint32_t i = 0; i < countof(file_names); i++)
    {
        yo_arena_reset(&arena);

        yo_string_t file_content = yo_load_file(file_names[i]);

        yo_token_t *lexed    = yo_lex(file_content, &arena);
        yo_api_spec_t parsed = yo_parse(lexed, &arena);

        yo_string_t bindings_for_csharp = yo_get_bindings_for_csharp(parsed, &arena);
        printf("%.*s", yo_string_fmt(bindings_for_csharp));

        yo_unload_file(file_content);
    }



    yo_arena_destroy(&arena);

}
