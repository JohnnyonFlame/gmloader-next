#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"

extern int app_in_focus;
extern int mouse_has_warped;
ABI_ATTR void window_mouse_set(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    if (app_in_focus) {
        g_MousePosX[0] = YYGetInt32(args, 0);
        g_MousePosY[0] = YYGetInt32(args, 1);
        mouse_has_warped = 1;
    }
}

ABI_ATTR void mouse_get_x(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = *g_MousePosX;
}

ABI_ATTR void mouse_get_y(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = *g_MousePosY;
}

ABI_ATTR void display_mouse_set(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    if (app_in_focus) {
        g_MousePosX[0] = YYGetInt32(args, 0);
        g_MousePosY[0] = YYGetInt32(args, 1);
        mouse_has_warped = 1;
    }
}

void patch_mouse(so_module *mod)
{
    Function_Add("display_mouse_set", display_mouse_set, 2, 0);
    Function_Add("window_mouse_set", window_mouse_set, 2, 0);
    Function_Add("display_mouse_get_x", mouse_get_x, 1, 0);
    Function_Add("display_mouse_get_y", mouse_get_y, 1, 0);
    Function_Add("window_mouse_get_x", mouse_get_x, 1, 0);
    Function_Add("window_mouse_get_y", mouse_get_y, 1, 0);
}