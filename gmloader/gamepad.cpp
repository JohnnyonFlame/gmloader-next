#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"

int MAP_DPAD_AS_AXIS = 0;
Gamepad yoyo_gamepads[4];

#define IS_AXIS_BOUNDS ((axis >= 0) && (axis < ARRAY_SIZE(yoyo_gamepads[0].axis)))
#define IS_BTN_BOUNDS  ((btn  >= 0) && (btn  < ARRAY_SIZE(yoyo_gamepads[0].buttons)))
#define IS_CONTROLLER_BOUNDS ((id >= 0) && (id < ARRAY_SIZE(yoyo_gamepads)))
#define IS_JOYSTICK_BOUNDS ((id >= 1) && (id <= 2))

int translateButton(RValue *val, int index)
{
    int v = YYGetInt32(val, index);
    if ((v >= (int)gp_face1) && (v - (int)gp_face1 < 16))
        v = v - (int)gp_face1;
    
    return v;
}

int translateAxis(RValue *val, int index)
{
    int v = YYGetInt32(val, index);
    if ((v >= (int)gp_axislh) && (v - (int)gp_axislh < 4))
        v = v - (int)gp_axislh;
    
    return v;
}

// (RValue *ret, void *self, void *other, int argc, RValue *args)
ABI_ATTR void gamepad_is_supported(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 1.0;
}

ABI_ATTR void gamepad_get_device_count(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    // Documentation claims this can be "the number of available "slots" for game pads to be connected to."
    // and since this is the upper-end, we're using that.
    ret->kind = VALUE_REAL;
    ret->rvalue.val = (double)ARRAY_SIZE(yoyo_gamepads);
}

ABI_ATTR void gamepad_is_connected(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;

    int id = YYGetInt32(args, 0);
    if (!IS_CONTROLLER_BOUNDS) {
        ret->rvalue.val = 0.0;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id];
    ret->rvalue.val = (gamepad->is_available) ? 1.0 : 0.0;
}

ABI_ATTR void gamepad_get_description(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_STRING;
    ret->rvalue.val = 0.0;
    YYCreateString(ret, "Xbox 360 Controller (XInput STANDARD GAMEPAD)");
}

ABI_ATTR void gamepad_get_button_threshold(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 0.5f;
}

ABI_ATTR void gamepad_set_button_threshold(RValue *ret, void *self, void *other, int argc, RValue *args)
{

}

ABI_ATTR void gamepad_axis_count(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 4.f;
}

ABI_ATTR void gamepad_axis_value(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    int axis = translateAxis(args, 1);

    if (!IS_CONTROLLER_BOUNDS || !IS_AXIS_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id];
    if (MAP_DPAD_AS_AXIS && axis < 2) {
        if (axis == 0) {
            if (gamepad->buttons[14])
                ret->rvalue.val = -1.0f;
            else if (gamepad->buttons[15])
                ret->rvalue.val = 1.0f;
            else
                ret->rvalue.val = 0.0f;
        } else if (axis == 1) {
            if (gamepad->buttons[12])
                ret->rvalue.val = -1.0f;
            else if (gamepad->buttons[13])
                ret->rvalue.val = 1.0f;
            else
                ret->rvalue.val = 0.0f;
        }

        // Revert back to analog if nothing was pressed.
        if (fabs(ret->rvalue.val) < gamepad->deadzone)
            ret->rvalue.val = gamepad->axis[axis];
    }
    else {
        ret->rvalue.val = gamepad->axis[axis];
    }
    
    if (fabs(ret->rvalue.val) < gamepad->deadzone)
        ret->rvalue.val = 0.0f;
}

ABI_ATTR void gamepad_button_check(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    int btn = translateButton(args, 1);

    if (!IS_CONTROLLER_BOUNDS || !IS_BTN_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id];
    if (MAP_DPAD_AS_AXIS && (btn >= 12 && btn <= 15))
        ret->rvalue.val = 0.0f;
    else
        ret->rvalue.val = (gamepad->buttons[btn] > 0) ? 1.0f : 0.0f;
}

ABI_ATTR void gamepad_button_check_pressed(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    int btn = translateButton(args, 1);

    if (!IS_CONTROLLER_BOUNDS || !IS_BTN_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id];
    if (MAP_DPAD_AS_AXIS && (btn >= 12 && btn <= 15))
        ret->rvalue.val = 0.0f;
    else
        ret->rvalue.val = (gamepad->buttons[btn] == 2) ? 1.0f : 0.0f;
}

ABI_ATTR void gamepad_button_check_released(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    int btn = translateButton(args, 1);

    if (!IS_CONTROLLER_BOUNDS || !IS_BTN_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id];
    if (MAP_DPAD_AS_AXIS && (btn >= 12 && btn <= 15))
        ret->rvalue.val = 0.0f;
    else
        ret->rvalue.val = (gamepad->buttons[btn] == -1) ? 1.0f : 0.0f;
}

ABI_ATTR void gamepad_button_count(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 16.f;
}

ABI_ATTR void gamepad_button_value(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    int btn = translateButton(args, 1);

    if (!IS_CONTROLLER_BOUNDS || !IS_BTN_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id];
    if (MAP_DPAD_AS_AXIS && (btn >= 12 && btn <= 15))
        ret->rvalue.val = 0.0f;
    else
        ret->rvalue.val = (gamepad->buttons[btn] > 0) ? 1.0f : 0.0f;
}

ABI_ATTR void gamepad_set_vibration(RValue *ret, void *self, void *other, int argc, RValue *args)
{
}

ABI_ATTR void gamepad_set_axis_deadzone(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    int id = YYGetInt32(args, 0);
    double deadzone = YYGetReal(args, 1);
    if (!IS_CONTROLLER_BOUNDS) {
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id];
    gamepad->deadzone = deadzone;
}

ABI_ATTR void gamepad_get_axis_deadzone(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    if (!IS_CONTROLLER_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id];
    ret->rvalue.val = gamepad->deadzone;
}

ABI_ATTR void gamepad_set_colour(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    
}

ABI_ATTR void GamePadRestart()
{
    for (int i = 0; i < ARRAY_SIZE(yoyo_gamepads); i++) {
        if (yoyo_gamepads[i].is_available) {
            int dsMap = CreateDsMap(2, "event_type", 0, 0, "gamepad discovered", "pad_index", (double)i, 0);
            CreateAsynEventWithDSMap(dsMap, 0x4b);
        }
    }
}

ABI_ATTR void joystick_exists(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    if (!IS_JOYSTICK_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id-1];
    ret->rvalue.val = gamepad->is_available ? 1.0 : 0.0;
}

ABI_ATTR void joystick_check_button(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    int btn = translateButton(args, 1);
    if (!IS_JOYSTICK_BOUNDS || !IS_BTN_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id-1];
    ret->rvalue.val = gamepad->buttons[btn] ? 1.0 : 0.0;
}

ABI_ATTR void joystick_buttons(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    if (!IS_JOYSTICK_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id-1];
    ret->rvalue.val = gamepad->is_available ? 16.0f : 0.0f;
}

ABI_ATTR void joystick_has_pov(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    if (!IS_JOYSTICK_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id-1];
    ret->rvalue.val = gamepad->is_available ? 1.0f : 0.0f;
}

ABI_ATTR void joystick_direction(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    if (!IS_JOYSTICK_BOUNDS) {
        ret->rvalue.val = vk_numpad5;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id-1];
    int UP    = gamepad->buttons[12] > 0.0;
    int DOWN  = gamepad->buttons[13] > 0.0;
    int LEFT  = gamepad->buttons[14] > 0.0;
    int RIGHT = gamepad->buttons[15] > 0.0;

    if (LEFT)
        ret->rvalue.val = vk_numpad4;
    else if (RIGHT)
        ret->rvalue.val = vk_numpad6;
    else
        ret->rvalue.val = vk_numpad5;

    if (UP)
        ret->rvalue.val += 3.0;
    else if (DOWN)
        ret->rvalue.val -= 3.0;
}

ABI_ATTR void joystick_pov(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    if (!IS_JOYSTICK_BOUNDS) {
        ret->rvalue.val = -1.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id-1];
    int UP    = gamepad->buttons[12] > 0.0;
    int DOWN  = gamepad->buttons[13] > 0.0;
    int LEFT  = gamepad->buttons[14] > 0.0;
    int RIGHT = gamepad->buttons[15] > 0.0;

    if (UP && RIGHT) {
        ret->rvalue.val = 45.0;
    } else if (RIGHT && DOWN) {
        ret->rvalue.val = 135.0;
    } else if (DOWN && LEFT) {
        ret->rvalue.val = 225.0;
    } else if (LEFT && UP) {
        ret->rvalue.val = 315.0;
    } else if (UP) {
        ret->rvalue.val = 0.0;
    } else if (RIGHT) {
        ret->rvalue.val = 90.0;
    } else if (DOWN) {
        ret->rvalue.val = 180.0;
    } else if (LEFT) {
        ret->rvalue.val = 270.0;
    } else {
        ret->rvalue.val = -1.0;
    }
}

void joystick_common_pos(RValue *ret, void *self, void *other, int argc, RValue *args, int axis)
{
    ret->kind = VALUE_REAL;
    int id = YYGetInt32(args, 0);
    if (!IS_JOYSTICK_BOUNDS) {
        ret->rvalue.val = 0.0f;
        return;
    }

    Gamepad *gamepad = &yoyo_gamepads[id-1];
    ret->rvalue.val = gamepad->axis[axis];
}

ABI_ATTR void joystick_xpos(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    joystick_common_pos(ret, self, other, argc, args, 0);
}

ABI_ATTR void joystick_ypos(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    joystick_common_pos(ret, self, other, argc, args, 1);
}

void patch_gamepad(so_module *mod)
{
    Function_Add("gamepad_is_supported", gamepad_is_supported, 0, 1);
    Function_Add("gamepad_get_device_count", gamepad_get_device_count, 0, 1);
    Function_Add("gamepad_is_connected", gamepad_is_connected, 1, 1);
    Function_Add("gamepad_get_description", gamepad_get_description, 1, 1);
    Function_Add("gamepad_get_button_threshold", gamepad_get_button_threshold, 1, 1);
    Function_Add("gamepad_set_button_threshold", gamepad_set_button_threshold, 2, 1);
    Function_Add("gamepad_get_axis_deadzone", gamepad_get_axis_deadzone, 1, 1);
    Function_Add("gamepad_set_axis_deadzone", gamepad_set_axis_deadzone, 2, 1);
    Function_Add("gamepad_button_count", gamepad_button_count, 1, 1);
    Function_Add("gamepad_button_check", gamepad_button_check, 2, 1);
    Function_Add("gamepad_button_check_pressed", gamepad_button_check_pressed, 2, 1);
    Function_Add("gamepad_button_check_released", gamepad_button_check_released, 2, 1);
    Function_Add("gamepad_button_value", gamepad_button_value, 2, 1);
    Function_Add("gamepad_axis_count", gamepad_axis_count, 1, 1);
    Function_Add("gamepad_axis_value", gamepad_axis_value, 2, 1);
    Function_Add("gamepad_set_vibration", gamepad_set_vibration, 3, 1);
    Function_Add("gamepad_set_color", gamepad_set_colour, 2, 1);
    Function_Add("gamepad_set_colour", gamepad_set_colour, 2, 1);

    Function_Add("joystick_exists", joystick_exists, 1, 1);
    Function_Add("joystick_check_button", joystick_check_button, 2, 1);
    Function_Add("joystick_buttons", joystick_buttons, 1, 1);
    Function_Add("joystick_has_pov", joystick_has_pov, 1, 1);
    Function_Add("joystick_direction", joystick_direction, 1, 1);
    Function_Add("joystick_pov", joystick_pov, 1, 1);
    Function_Add("joystick_xpos", joystick_xpos, 1, 1);
    Function_Add("joystick_ypos", joystick_ypos, 1, 1);

    hook_symbol(mod, "_Z14GamePadRestartv", (uintptr_t)GamePadRestart, 1);
}
