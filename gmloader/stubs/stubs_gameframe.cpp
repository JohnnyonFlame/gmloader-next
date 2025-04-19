#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"
#include "configuration.h"

static const char* gameframe_stubs[] = {
    "gameframe_mouse_in_window_raw",
    "gameframe_init_raw_raw",
    "gameframe_syscommand_raw",
    "gameframe_get_monitors_1_raw",
    "gameframe_get_double_click_time_raw",
    "gameframe_is_natively_minimized_raw",
    "gameframe_set_shadow",
    "gameframe_get_shadow",
    "gameframe_get_monitors_2",
    "gameframe_check_native_extension",
    "gameframe_mouse_in_window_raw",
    "gameframe_init_raw_raw",
    "gameframe_syscommand_raw",
    "gameframe_get_monitors_1_raw",
    "gameframe_get_double_click_time_raw",
    "gameframe_is_natively_minimized_raw",
    "gameframe_set_shadow",
    "gameframe_get_shadow",
    "gameframe_get_monitors_2",
    "gameframe_check_native_extension",
    "gameframe_x64.dll",
    "gameframe_init_native",
    "gameframe_get_monitors",
    "gameframe_prepare_buffer",
    "gameframe_buffer_read_chars",
    "gameframe_init_native",
    "gameframe_get_monitors",
    "gameframe_prepare_buffer",
    "gameframe_buffer_read_chars",
    "gameframe_mouse_in_window",
    "gameframe_init_raw",
    "gameframe_syscommand",
    "gameframe_get_monitors_1",
    "gameframe_get_double_click_time",
    "gameframe_is_natively_minimized",
    "gameframe_mouse_in_window",
    "gameframe_init_raw",
    "gameframe_syscommand",
    "gameframe_get_monitors_1",
    "gameframe_get_double_click_time",
    "gameframe_is_natively_minimized"
};

ABI_ATTR void gameframe_stub(RValue *ret, void *self, void *other, int argc, RValue *args)
{

}

void patch_gameframe(struct so_module *mod)
{
    for (auto &func: gameframe_stubs)
        Function_Add(func, gameframe_stub, 1, 0);
}