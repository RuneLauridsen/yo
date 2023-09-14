// WARNING(rune): These bindings are direct translations of their C counterparts, and are not C# friendly,
// or written following C# style.
// TODO(rune): C-sharp bindings are not fun to write manually.

global using uint32_t = System.UInt32;
global using uint64_t = System.UInt64;
global using int32_t = System.Int64;
global using int64_t = System.Int64;
global using size_t = System.UIntPtr;
global using yo_id_t = System.UInt64;
global using yo_color_t = YoOld.yo_native.yo_v4f_t;
global using yo_error_t = System.UInt32;
global using static YoOld.yo_native.yo_error_t;
global using static YoOld.yo_native.yo_axis_t;
global using static YoOld.yo_native.yo_layout_t;
global using static YoOld.yo_native.yo_align_t;
global using static YoOld.yo_native.yo_overflow_t;
global using static YoOld.yo_native.yo_side_t;
global using static YoOld.yo_native.yo_corner_t;
global using static YoOld.yo_native.yo_popup_flags_t;
global using static YoOld.yo_native.yo_box_flags_t;
global using static YoOld.yo_native.yo_anim_flags_t;
global using static YoOld.yo_native.yo_mouse_button_t;
global using static YoOld.yo_native.yo_keycode_t;
global using static YoOld.yo_native.yo_modifier_t;
global using static YoOld.yo_native.yo_frame_flags_t;

namespace YoOld;

public unsafe static partial class yo_native
{

}
