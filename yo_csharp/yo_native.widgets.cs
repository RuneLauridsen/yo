global using static Yo.yo_native.yo_circle_flags_t;

using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Yo;

public unsafe static partial class yo_native
{
    ////////////////////////////////////////////////////////////////
    //
    //
    // Rectangle
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_rectangle(yo_v4f_t fill, yo_length_t width, yo_length_t height, yo_id_t id);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Fill
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_fill(yo_v4f_t fill);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Circle
    //
    //
    ////////////////////////////////////////////////////////////////

    public enum yo_circle_flags_t : uint32_t
    {
        YO_CIRCLE_NONE = 0,

        // NOTE(rune): If this flag is set, the diameter argument does not include border thickness,
        // which means that is circles actual diameter will be diameter+2*border_thickness.
        YO_CIRCLE_BORDER_ADDS_DIAMETER = 1,
    };

    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_circle(yo_id_t id, float diameter, yo_v4f_t fill, uint32_t border_thickness, yo_v4f_t border_color, yo_circle_flags_t flags);


    ////////////////////////////////////////////////////////////////
    //
    //
    // Layout
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_h_space(yo_length_t amount);
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_v_space(yo_length_t amount);
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_h_layout();
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_v_layout();
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_container(yo_id_t id);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Text
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_text(char* text);
    [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_format_text(char* format, __arglist);
    // [DllImport("yo_dll.dll")] public static extern yo_box_t* yo_format_text_v(char* format, va_list args);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Text field
    //
    //
    ////////////////////////////////////////////////////////////////

    public struct yo_text_field_t
    {
        public bool text_changed;
        public bool state_changed;
        public yo_box_t* box;
    };

    [DllImport("yo_dll.dll")] public static extern yo_text_field_t yo_text_field(yo_id_t id, char* buffer, size_t buffer_size);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Button
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern yo_signal_t yo_button(char* text);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Checkbox
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern bool yo_checkbox(char* label, bool* is_checked);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Radio
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern bool yo_radio(char* label, uint32_t index, uint32_t* selected_index);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Bullet list
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern void yo_bullet_item(char* label);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Slider
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern void yo_slider(yo_id_t id, float* value, float min, float max);
    [DllImport("yo_dll.dll")] public static extern void yo_slider_ex(yo_id_t id, float* value, float min, float max, yo_slider_style_t* ex);
    [DllImport("yo_dll.dll")] public static extern void yo_slider_with_label(char* label, float* value, float min, float max);
    [DllImport("yo_dll.dll")] public static extern yo_signal_t yo_slider_behaviour(float* value, float min, float max, yo_axis_t axis, uint32_t thumb_dim, yo_box_t* bounding_box);

    ////////////////////////////////////////////////////////////////
    //
    //
    // Menu
    //
    //
    ////////////////////////////////////////////////////////////////

    // WARNING(rune): Not finished
    [DllImport("yo_dll.dll")] public static extern bool yo_menu_begin(char* label);
    [DllImport("yo_dll.dll")] public static extern bool yo_menu_begin_ex(char* label, yo_v4f_t bg);
    [DllImport("yo_dll.dll")] public static extern void yo_menu_end();
    [DllImport("yo_dll.dll")] public static extern yo_signal_t yo_menu_item(char* label);
    [DllImport("yo_dll.dll")] public static extern void yo_menu_separator();

    ////////////////////////////////////////////////////////////////
    //
    //
    // Scroll area
    //
    //
    ////////////////////////////////////////////////////////////////

    [DllImport("yo_dll.dll")] public static extern void yo_begin_scroll_area(yo_id_t id);
    [DllImport("yo_dll.dll")] public static extern void yo_begin_scroll_area_ex(yo_id_t id, float scroll_rate, float anim_rate);
    [DllImport("yo_dll.dll")] public static extern void yo_end_scroll_area();

    ////////////////////////////////////////////////////////////////
    //
    //
    // Table
    //
    //
    ////////////////////////////////////////////////////////////////

    public enum yo_table_flags_t : uint32_t
    {
        YO_TABLE_FLAG_NONE,
        YO_TABLE_FLAG_NO_HEADERS = 1
    };

    public struct yo_table_col_t
    {
        public char* header;
        public yo_length_t width;
    };

    public struct yo_table_t
    {
        public yo_table_flags_t flags;
        public yo_table_col_t* cols;
        public uint32_t col_count;
        public uint32_t col_idx;
    };

    [DllImport("yo_dll.dll")] public static extern void yo_table_def(yo_table_t* table, uint32_t col_count, yo_table_flags_t flags);
    [DllImport("yo_dll.dll")] public static extern void yo_table_def_col(yo_table_t* table, char* header, yo_length_t width);
    [DllImport("yo_dll.dll")] public static extern void yo_table_begin(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_begin_row(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_end(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_end_row(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_cell(yo_table_t* table);
    [DllImport("yo_dll.dll")] public static extern void yo_table_cell_text(yo_table_t* table, char* text);
    // [DllImport("yo_dll.dll")] public static extern void yo_table_cell_format_text_v(yo_table_t* table, char* format, va_list args);
    [DllImport("yo_dll.dll")] public static extern void yo_table_cell_format_text(yo_table_t* table, char* format, __arglist);
    [DllImport("yo_dll.dll")] public static extern void yo_table_cell_checkbox(yo_table_t* table, char* label, bool* is_checked);

}
