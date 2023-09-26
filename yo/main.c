#define _CRT_SECURE_NO_WARNINGS

//
// Standard library
//

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

//
// Tracy
//

#if 0
#   define TRACY_ENABLE
#   include "thirdparty/tracy/tracy/TracyC.h"
#   define YO_PROFILE_FRAME_MARK() TracyCFrameMark
#   define YO_PROFILE_BEGIN(name)  TracyCZoneN(__##name, #name, true)
#   define YO_PROFILE_END(name)    TracyCZoneEnd(__##name)
#else
#   define YO_PROFILE_FRAME_MARK()
#   define YO_PROFILE_BEGIN(name)
#   define YO_PROFILE_END(name)
#endif

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

//
// Platform & backend
//

#define YO_PLATFORM_API static
#include "impl/yo_backend_opengl.h"
#include "impl/yo_backend_opengl.c"
#include "impl/yo_platform_win32.h"
#include "impl/yo_platform_win32.c"

static void build_ui(void);

static void yo_platform_win32_app_main()
{

}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    YO_UNUSED(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    //main();
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
#elif 0
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
#elif 1
void build_ui(void)
{
    yo_layout_y();
    yo_begin_children();

    yo_button("Button 1");
    yo_button("Button 2");
    yo_button("Button 3");

    yo_end_children();
}
#elif 0
void build_ui(void)
{
    yo_box(0, 0);
    yo_set_fill(YO_RED);
    //yo_set_layout(YO_LAYOUT_STACK_X);
    yo_set_dim(yo_rel(1.0f), yo_rel(1.0f));
    YO_CHILD_SCOPE()
    {
        yo_rectangle(0, YO_CYAN, yo_px(100), yo_px(50));
        yo_set_align(YO_ALIGN_CENTER, YO_ALIGN_CENTER);

        yo_rectangle(0, YO_BLUE, yo_px(75), yo_px(125));
        yo_set_align(YO_ALIGN_CENTER, YO_ALIGN_CENTER);
    }
}
#elif 0
void build_ui(void)
{
    yo_box(0, 0);
    yo_set_fill(YO_RED);
    //yo_set_layout(YO_LAYOUT_STACK_X);
    yo_set_dim(yo_px(195), yo_px(100));
    yo_set_padding(5, 5, 5, 5);
    YO_CHILD_SCOPE()
    {
        yo_text("I am the Walrus. gg! Lots of text that needswraapping asdgha gdga sdgahsdg J!JHgasgd gahsgd");
        yo_set_fill(YO_ORANGE);
    }
}
#elif 1
void build_ui(void)
{
    static float f = 0.0f;

    yo_layout_y();
    yo_begin_children();
    //yo_set_dim_x(yo_rel(1.0f));
    yo_set_fill(yo_rgb(20, 20, 20));
    yo_set_tag("AAA");

    YO_CHILD_SCOPE()
    {
        // NOTE(rune): Text field.
        static char text_field_buffer[256] = { 'a', 'b', 'c' };
        yo_text_field(yo_id("txt"), text_field_buffer, sizeof(text_field_buffer));

        // NOTE(rune): Horizontal slider, fills whole parent.
        yo_slider(yo_id("my-slider"), &f, 1.0f, 10.f);

        // NOTE(rune): Vertical slider in a orange container.
        yo_box(0, 0);
        yo_set_tag("TAG_ORANGE");
        yo_set_dim(yo_rel(1.0f), yo_px(200));
        yo_set_fill(YO_ORANGE);
        yo_set_layout(YO_LAYOUT_STACK_X);
        YO_CHILD_SCOPE()
        {
            yo_space_x(yo_rel(1.0f));

            yo_slider_style_t style = yo_default_slider_style();
            style.axis = YO_AXIS_Y;
            yo_slider_ex(yo_id("vert slider"), &f, 1.0f, 10.0f, &style);

            yo_space_x(yo_rel(1.0f));
        }


        yo_format_text("The value is %f", f);
        yo_set_font_size((uint32_t)(f * 8) + 10);

        yo_text("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");



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
            yo_set_fill(YO_GREEN);
            yo_table_cell_text(&table, "fixed");
            yo_set_fill(YO_CYAN);
            yo_table_cell_format_text(&table, "val: %f", f);
            yo_set_fill(YO_BLUE);
            yo_table_cell_checkbox(&table, rows_values[row_idx].name2, &b);
            yo_set_fill(YO_MAGENTA);
            yo_table_end_row(&table);
        }
        yo_table_end(&table);

        yo_layout_x();
        yo_set_align_x(YO_ALIGN_CENTER);
        YO_CHILD_SCOPE()
        {
            yo_debug_show_atlas_partitions();
            yo_debug_show_atlas_texture();
        }
    }

    yo_end_children();
}
#elif 0
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
