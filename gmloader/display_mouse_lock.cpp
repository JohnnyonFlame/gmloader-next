#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"
#include "configuration.h"

ABI_ATTR void display_mouse_lock(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 1.0;
}

ABI_ATTR void display_mouse_unlock(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 1.0;
}

ABI_ATTR void display_mouse_bounds_raw(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 1.0;
}

void patch_display_mouse_lock(struct so_module *mod)
{
    Function_Add("display_mouse_lock", display_mouse_lock, 1, 0);
    Function_Add("display_mouse_unlock", display_mouse_unlock, 1, 0);
    Function_Add("display_mouse_bounds_raw", display_mouse_bounds_raw, 1, 0);
}