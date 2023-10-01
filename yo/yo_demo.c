#pragma once

static void yo_demo_header(char *header_text)
{
    yo_layout_y();
    YO_CHILD_SCOPE()
    {
        const yo_v4f_t LINE_COLOR =  yo_rgb(70, 70, 70);

        yo_fill(LINE_COLOR);
        yo_set_dim_y(yo_px(2));

        yo_text(header_text);
        yo_set_padding_xy(10, 5);
        yo_set_font_size(30);
        yo_set_font_color(yo_rgb(100, 230, 250));

        yo_fill(LINE_COLOR);
        yo_set_dim_y(yo_px(2));
    }
}

static void yo_demo_desc(char *desc)
{
    yo_text(desc);
    yo_set_font_size(18);
    yo_set_font_color(yo_rgb(180, 180, 180));
    yo_set_padding_xy(10, 5);
}

YO_API void yo_demo(void)
{
    yo_palette_t pallette = yo_palette_dark();

    // Holds all user state which needs to persist across frames.
    static struct
    {
        uint32_t    button_counter;
        uint32_t    radio_index;
        bool        checkbox_value;
        float       slider_value;
        float       scroll_smooth_rate;
        char        buffer[256];
    } state;

    yo_box(0, 0);
    yo_set_fill(pallette.background);
    yo_set_dim(yo_rel(1.0f), yo_rel(1.0f));

    yo_begin_scroll_area_ex(yo_id("demo_scroller"), 20.f, state.scroll_smooth_rate);

    yo_box(0, 0);
    yo_set_dim(yo_rel(1.0f), yo_rel(1.0f));
    yo_set_layout(YO_LAYOUT_STACK_Y);
    yo_set_padding(10, 10, 10, 10);

    YO_CHILD_SCOPE()
    {
        //
        // Section: yo_button()
        //

        {
            // Reusable components can be written as plain functions, no classes/inheritance are necessary.
            //
            yo_demo_header("Widget: yo_button()");
            yo_demo_desc("Simple button with a mouse hover animation.");

            yo_layout_x();
            YO_CHILD_SCOPE()
            {
                if (yo_button("Increment").clicked) { state.button_counter++; }
                if (yo_button("Decrement").clicked) { state.button_counter--; }
                yo_space_x(yo_px(20));
                yo_format_text("Counter = %i", state.button_counter);
                yo_set_align_y(YO_ALIGN_CENTER);
            }

            yo_space_y(yo_px(20));

            yo_button("Blue");     yo_set_fill(YO_BLUE);    yo_set_font_color(YO_CYAN);     yo_set_border(2, YO_BLACK, 10);
            yo_button("Yellow");   yo_set_fill(YO_YELLOW);  yo_set_font_color(YO_ORANGE);   yo_set_border(2, YO_BLACK, 0);
            yo_button("Red");      yo_set_fill(YO_RED);     yo_set_font_color(YO_MAGENTA);  yo_set_border(2, YO_BLACK, 15);

        }

        yo_space_y(yo_px(50));

        //
        // Section: yo_checkbox()
        //

        {
            yo_demo_header("Widget: yo_checkbox()");
            yo_demo_desc("Toggleable checkbox with a label.");

            yo_checkbox("Checkbox", &state.checkbox_value);

            yo_box(0, 0);
            yo_set_text(state.checkbox_value ? "(-: " : ":-(");
            yo_set_font_size(50);
            yo_set_font_color(YO_BLACK);
            yo_set_fill(YO_YELLOW);
            yo_set_align_x(YO_ALIGN_LEFT);
            yo_set_border(0, YO_TRANSPARENT, 10);

        }

        yo_space_y(yo_px(50));

        //
        // Section: yo_radio()
        //

        {
            yo_demo_header("Widget: yo_radio()");
            yo_demo_desc("Radio buttons, that automatically make sure only one option is selected at a time.");
            yo_radio("Option 1", 0, &state.radio_index);
            yo_radio("Option 2", 1, &state.radio_index);
            yo_radio("Option 3", 2, &state.radio_index);
            yo_format_text("Selected index %i", state.radio_index);
        }

        yo_space_y(yo_px(50));

        //
        // Section: yo_slider()
        //

        {
            yo_demo_header("Widget: yo_slider()");
            yo_demo_desc("Slider for selecting floating point values.");
            yo_slider(yo_id("my_slider"), &state.slider_value, 0.0f, 10.0f);
            yo_format_text("Value: %f", state.slider_value);

            yo_demo_desc("Sliders can also be vertical.");
            yo_slider_style_t style = yo_default_slider_style();
            style.axis = YO_AXIS_Y;
            yo_slider_ex(yo_id("my_vertical_slider"), &state.slider_value, 0.0f, 10.0f, &style);
            yo_set_dim_y(yo_px(100));
            yo_set_align_x(YO_ALIGN_LEFT);
        }

        yo_space_y(yo_px(50));

        //
        // Section yo_text_field()
        //

        {
            if (state.buffer[0] == '\0') state.buffer[0] = ' ';

            yo_demo_header("Widget: yo_text_field()");
            yo_demo_desc("Single-line text edit control. Multi-line text is not supported yet. Single-line edit is still buggy.");
            yo_text_field(yo_id("my_text_field"), state.buffer, sizeof(state.buffer));
        }

        yo_space_y(yo_px(50));

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

        yo_space_y(yo_px(50));

        //
        // Section: Scrolling
        //

        {
            yo_demo_header("Scrolling");
            yo_demo_desc("Use yo_begin_scroll_area() and yo_end_scroll_area() to create scroll bars. ");
            yo_demo_desc("Supports smooth scrolling.");

            if (state.scroll_smooth_rate == 0.0f) state.scroll_smooth_rate = 20.0f;

            yo_format_text("Smooth scroll rate: %f", state.scroll_smooth_rate);
            yo_slider(yo_id("smooth"), &state.scroll_smooth_rate, 1.0f, 50.0f);
        }

        yo_space_y(yo_px(50));

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

        yo_space_y(yo_px(50));

    }

    yo_end_scroll_area();
}
