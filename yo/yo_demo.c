#pragma once

// Reusable components can be written plain functions, no classes/inheritance necessary.
static void yo_demo_header(char *header)
{
    yo_v_layout();
    YO_CHILD_SCOPE()
    {
        const yo_v4f_t LINE_COLOR =  yo_rgb(70, 70, 70);

        yo_fill(LINE_COLOR);
        yo_new()->v_dim = yo_px(2);

        yo_text(header);
        yo_new()->padding = yo_padding_hv(10, 5);
        yo_new()->font_size = 30;
        yo_new()->font_color = yo_rgb(100, 230, 250);

        yo_fill(LINE_COLOR);
        yo_new()->v_dim = yo_px(2);
    }
}

static void yo_demo_desc(char *desc)
{
    yo_text(desc);
    yo_new()->font_size = 18;
    yo_new()->font_color = yo_rgb(180, 180, 180);
    yo_new()->padding = yo_padding_hv(10, 5);
}

YO_API void yo_demo(void)
{
    yo_palette_t pallette = yo_palette_dark();

    // Holds all state which needs to persist across frames.
    static struct
    {
        uint32_t i, r;
        bool b;
        float f, smooth;
        char buffer[256];
    } state;


    // Background box covering the whole window
    yo_box(0, 0);
    yo_new()->fill = pallette.background;

    // Scroll area for the whole window
    yo_begin_scroll_area_ex(yo_id("demo_scroller"), 20.f, state.smooth);

    // Stack items inside scroll area vertically
    yo_v_layout();
    yo_new()->padding = yo_padding(10, 10, 10, 10);
    YO_CHILD_SCOPE()
    {
        //
        // Section: yo_button()
        //

        {
            yo_demo_header("Widget: yo_button()");
            yo_demo_desc("Simple button with a mouse hover animation.");
            yo_h_layout();
            YO_CHILD_SCOPE()
            {
                if (yo_button("Increment").clicked) { state.i++; }
                if (yo_button("Decrement").clicked) { state.i--; }
                yo_h_space(yo_px(20));
                yo_format_text("Counter = %i", state.i);
                yo_new()->v_align = YO_ALIGN_CENTER;
            }

            yo_v_space(yo_px(20));

            yo_button("Blue");     yo_new()->fill = YO_BLUE;    yo_new()->font_color = YO_CYAN;     yo_new()->border = yo_border(2, YO_BLACK, 10);
            yo_button("Yellow");   yo_new()->fill = YO_YELLOW;  yo_new()->font_color = YO_ORANGE;   yo_new()->border = yo_border(2, YO_BLACK, 0);
            yo_button("Red");      yo_new()->fill = YO_RED;     yo_new()->font_color = YO_MAGENTA;  yo_new()->border = yo_border(2, YO_BLACK, 15);

        }

        yo_v_space(yo_px(50));

        //
        // Section: yo_checkbox()
        //

        {
            yo_demo_header("Widget: yo_checkbox()");
            yo_demo_desc("Toggleable checkbox with a label.");

            yo_checkbox("Checkbox", &state.b);

            yo_box(0, 0);
            yo_new()->text       = state.b ? "(-: " : ":-(";
            yo_new()->font_size  = 50;
            yo_new()->font_color = YO_BLACK;
            yo_new()->fill       = YO_YELLOW;
            yo_new()->h_align    = YO_ALIGN_LEFT;
            yo_new()->h_dim      = yo_px(50);
            yo_new()->v_dim      = yo_px(50);
            yo_new()->border     = yo_border(0, YO_TRANSPARENT, 10);

        }

        yo_v_space(yo_px(50));

        //
        // Section: yo_radio()
        //

        {
            yo_demo_header("Widget: yo_radio()");
            yo_demo_desc("Radio buttons, that automatically make sure only one option is selected at a time.");
            yo_radio("Option 1", 0, &state.r);
            yo_radio("Option 2", 1, &state.r);
            yo_radio("Option 3", 2, &state.r);
            yo_format_text("Selected index %i", state.r);
        }

        yo_v_space(yo_px(50));

        //
        // Section: yo_slider()
        //

        {
            yo_demo_header("Widget: yo_slider()");
            yo_demo_desc("Slider for selecting floating point values.");
            yo_slider(yo_id("my_slider"), &state.f, 0.0f, 10.0f);
            yo_format_text("Value: %f", state.f);

            yo_demo_desc("Sliders can also be vertical.");
            yo_slider_style_t style = yo_default_slider_style();
            style.axis = YO_AXIS_Y;
            yo_slider_ex(yo_id("my_vertical_slider"), &state.f, 0.0f, 10.0f, &style);
            yo_new()->v_dim = yo_px(100);
            yo_new()->h_align = YO_ALIGN_LEFT;
        }

        yo_v_space(yo_px(50));

        //
        // Section yo_text_field()
        //

        {
            if (state.buffer[0] == '\0') state.buffer[0] = ' ';

            yo_demo_header("Widget: yo_text_field()");
            yo_demo_desc("Single-line text edit control. Multi-line text is not supported yet. Single-line edit is still buggy.");
            yo_text_field(yo_id("my_text_field"), state.buffer, sizeof(state.buffer));
        }

        yo_v_space(yo_px(50));

        //
        // Section yo_bullet_item
        //

        {
            yo_demo_header("Widget: yo_bullet_item()");
            yo_demo_desc("Bullet list.");
            yo_bullet_item("abdefg");
            yo_bullet_item("hijklmn");
            yo_bullet_item("opqrstu");
        }

        yo_v_space(yo_px(50));

        //
        // Section: Scrolling
        //

        {
            yo_demo_header("Scrolling");
            yo_demo_desc("Use yo_begin_scroll_area() and yo_end_scroll_area() to create scroll bars. ");
            yo_demo_desc("Supports smooth scrolling.");

            if (state.smooth == 0.0f) state.smooth = 20.0f;

            yo_format_text("Smooth scroll rate: %f", state.smooth);
            yo_slider(yo_id("smooth"), &state.smooth, 1.0f, 50.0f);
        }

        yo_v_space(yo_px(50));

        //
        // Section: Tables
        //

        {
            yo_demo_header("Tables");
            yo_demo_desc("The table api is currently very basic, and does not scale well for a larger number of rows.");

            yo_table_t table;
            yo_table_def(&table, 3, 0);
            yo_table_def_col(&table, "Fixed 100px column", yo_px(300));
            yo_table_def_col(&table, "Auto sized column", yo_rel(1.0f));
            yo_table_def_col(&table, "Auto sized column", yo_rel(1.0f));

            yo_table_begin(&table);
            for (int i = 0; i < 10; i++)
            {
                yo_table_begin_row(&table);
                yo_table_cell_format_text(&table, "%i", i);
                yo_table_cell_text(&table, "I am a table cell.");
                yo_table_cell_text(&table, "I am a table cell too.");
                yo_table_end_row(&table);
            }
            yo_table_end(&table);
        }

        yo_v_space(yo_px(50));

    }

    yo_end_scroll_area();
}
