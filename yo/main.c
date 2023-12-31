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
#include "thirdparty/khrplatform.h"
#include "thirdparty/glext.h"
#include "thirdparty/wglext.h"

#pragma comment ( lib, "kernel32.lib" )
#pragma comment ( lib, "user32.lib" )
#pragma comment ( lib, "opengl32.lib" )
#pragma comment ( lib, "gdi32.lib" )


//
// yo
//

#define YO_API static
#include "yo.h"
#include "yo.c"
#include "yo_memory.h" // NOTE(rune): OpenGL backend uses yo_array
#include "yo_memory.c"

//
// Platform & backend
//

#include "backends/yo_backend_opengl.h"
#include "backends/yo_backend_opengl.c"
#include "platforms/yo_platform_win32_opengl.h"
#include "platforms/yo_platform_win32_opengl.c"

static void build_ui(void);

int main()
{
    yo_platform_win32_opengl_t platform = { 0 };
    yo_platform_win32_opengl_startup(&platform, 800, 600);

    while (platform.running)
    {
        yo_platform_win32_opengl_before_frame(&platform);

        //build_ui();
        yo_demo();

        yo_platform_win32_opengl_after_frame(&platform);
    }

    yo_platform_win32_opengl_shutdown(&platform);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    YO_UNUSED(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    main();
}

#define countof(x) (sizeof(x) / sizeof(x[0]))

#if 0
void build_ui(void)
{
    yo_v_layout();

    YO_CHILD_SCOPE()
    {
        yo_text("a");
        yo_text("a");
        yo_text("a");
    }

}
#elif 1
void build_ui(void)
{
    static uint32_t i = 0;
    yo_v_layout();
    yo_new()->tag = "ABCDEF";
    yo_new()->fill =  yo_palette_dark().background;
    YO_CHILD_SCOPE()
    {
        float offset = (i % 240) > 120 ? 50.0f : 150.0f;

        yo_button("Standard button");
        yo_new()->margin = yo_margin(offset, 0, 0, 0);
        yo_new()->anim |= YO_ANIM_MARGIN;
        yo_new()->anim_rate = 10.0f;

        yo_container(0);
        yo_new()->h_dim = yo_px(200);
        yo_new()->v_dim = yo_px(300);
        yo_new()->fill  = yo_palette_dark().background_1;
        YO_CHILD_SCOPE()
        {
            yo_begin_scroll_area(yo_id("my_scroll_area"));

            for (int j = 0; j < 100; j++)
            {
                yo_format_text("%i", j);
            }

            yo_end_scroll_area();
        }
    }

    i++;
}

#elif 0
void build_ui(void)
{
    static char buffer[128] = { 'y', 'o', 'y', 'o', 'y', 'o', '\0' };

    yo_v_layout();
    YO_CHILD_SCOPE()
    {
        yo_text_field(buffer, sizeof(buffer));
        yo_new()->fill       = yo_rgb(30, 30, 30);
        yo_new()->font_color = yo_rgb(210, 210, 210);
        yo_new()->font_size  = 20;

        yo_text("static text");
        yo_new()->font_color = yo_rgb(210, 210, 255);

        yo_debug_show_atlas_partitions();

        yo_slider(yo_id("a"), 0, 0, 1);
    }
}
#elif 0
yo_placement_t p = {
  .h_dim = yo_px(200),
  .v_dim = yo_px(150)
};

yo_begin_scroll_area(yo_id("hello"), p);

for (yo_range32(it, 0, 1000))
{
    yo_format_text("%i", it);
    yo_new()->font_color = YO_RED;
}

yo_end_scroll_area();

}
#elif 0
void build_ui(void)
{
    static float f = 0.0f;

    yo_v_layout();
    yo_new()->tag = "AAA";
    yo_new()->fill = yo_rgb(20, 20, 20);

    YO_CHILD_SCOPE()
    {
        yo_button("bbbb");

        static char text_field_buffer[256] = { 'a', 'b', 'c' };
        yo_text_field(yo_id("txtf"), text_field_buffer, sizeof(text_field_buffer));

        yo_container(yo_id_from_string("a"));
        yo_new()->margin     = yo_margin_all(10);
        yo_new()->fill = yo_rgb(40, 0, 0);
        yo_begin_children();
        yo_slider(yo_id("slider"), &f, 1.0f, 10.0f);
    }

    yo_container(0);
    yo_new()->v_dim = yo_px(200);
    yo_new()->fill = YO_ORANGE;
    YO_CHILD_SCOPE()
    {
        yo_slider_style_t style = yo_default_slider_style();
        style.axis = YO_AXIS_Y;
        yo_slider_ex(yo_id("vert slider"), &f, 1.0f, 10.0f, &style);
    }


    yo_format_text("The value is %f", f);
    yo_new()->font_size = (uint32_t)(f * 8) + 10;

    yo_text("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

#if 1
    struct { char *name, *name2; yo_v4f_t rgb; } rows_values[] =
    {
        { "Red",     "red",     YO_RED,     },
        { "Green",   "green",   YO_GREEN,   },
        { "Blue",    "blue",    YO_BLUE,    },
        { "Yellow",  "yellow",  YO_YELLOW,  },
        { "Magenta", "magenta", YO_MAGENTA, },
        { "Cyan",    "cyan",    YO_CYAN,    },
    };

    yo_table_t table;
    yo_table_def(&table, 4, 0);
    yo_table_def_col(&table, "Color", yo_rel(1));
    yo_table_def_col(&table, "Rating", yo_px(100));
    yo_table_def_col(&table, "Misc", yo_rel(1));
    yo_table_def_col(&table, "Misc", yo_rel(1));

    yo_table_begin(&table);

    for (uint32_t row_idx = 0; row_idx < countof(rows_values); row_idx++)
    {
        static bool b = false;

        yo_table_begin_row(&table);
        yo_table_cell_text(&table, rows_values[row_idx].name);
        yo_new()->fill = YO_GREEN;
        yo_table_cell_text(&table, "fixed");
        yo_new()->fill = YO_CYAN;
        yo_table_cell_format_text(&table, "val: %f", f);
        yo_new()->fill = YO_BLUE;
        yo_table_cell_checkbox(&table, rows_values[row_idx].name2, &b);
        yo_new()->fill = YO_MAGENTA;
        yo_table_end_row(&table);
    }
    yo_table_end(&table);

    yo_h_layout();
    yo_new()->h_align = YO_ALIGN_CENTER;
    YO_CHILD_SCOPE()
    {
        yo_debug_show_atlas_partitions();
        yo_debug_show_atlas_texture();
    }
#endif
}
#else
void build_ui()
{
    yo_text("A");
    yo_new()->h_align = YO_ALIGN_LEFT;
    yo_new()->v_align = YO_ALIGN_TOP;
    //yo_new()->h_dim = yo_px(200);
    //yo_new()->v_dim = yo_px(200);
    yo_new()->font_color = YO_RED;
    yo_new()->font_size = 200;
}

#endif
