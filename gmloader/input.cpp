#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"
#include "configuration.h"

int app_in_focus = 0;
int mouse_has_warped = 0;

typedef struct controller_t {
    SDL_GameController *controller;
    int which; //instance id
    int slot;  //player slot
} controller_t;

static controller_t sdl_controllers[8] = {
    {.controller = NULL, .which = -1, .slot = -1},
    {.controller = NULL, .which = -1, .slot = -1},
    {.controller = NULL, .which = -1, .slot = -1},
    {.controller = NULL, .which = -1, .slot = -1},
    {.controller = NULL, .which = -1, .slot = -1},
    {.controller = NULL, .which = -1, .slot = -1},
    {.controller = NULL, .which = -1, .slot = -1},
    {.controller = NULL, .which = -1, .slot = -1}
};

static int get_free_controller_slot()
{
    for (int i = 0; i < ARRAY_SIZE(sdl_controllers); i++) {
        if (sdl_controllers[i].controller == NULL) {
            return i;
        }
    }

    return -1;
}

static int get_which_controller(int which)
{
    for (int i = 0; i < ARRAY_SIZE(sdl_controllers); i++) {
        if (sdl_controllers[i].which == which) {
            return i;
        }
    }

    return -1;
}

static int get_free_yoyogamepad_slot()
{
    for (int i = 0; i < ARRAY_SIZE(yoyo_gamepads); i++) {
        if (!yoyo_gamepads[i].is_available) //not connected
            return i;
    }

    return -1;
}

static void keyboard_set_key_raw(int key, int state)
{
    if (state)
        *_IO_CurrentKey = key;
    else if (!state && *_IO_CurrentKey == key)
        *_IO_CurrentKey = -1;
    _IO_KeyDown[key] = state;
    _IO_KeyPressed[key] = state;
    _IO_KeyReleased[key] = !state;
}

static int update_button(int new_state, int old_state)
{
    if (new_state == GAMEPAD_BUTTON_STATE_HELD) {
        if (old_state == GAMEPAD_BUTTON_STATE_NEUTRAL)
            return GAMEPAD_BUTTON_STATE_DOWN;
        return GAMEPAD_BUTTON_STATE_HELD;
    } else {
        if (old_state == GAMEPAD_BUTTON_STATE_HELD)
            return GAMEPAD_BUTTON_STATE_UP;
        return GAMEPAD_BUTTON_STATE_NEUTRAL;
    }
}

static int mouse_button_map(int btn)
{
    switch(btn) {
        case SDL_BUTTON_LEFT: return 0;
        case SDL_BUTTON_MIDDLE: return 2;
        case SDL_BUTTON_RIGHT: return 1;
        default: return 2;
    }
}

static void keyboard_set_key(SDL_Keycode key, int state)
{
    if (key == SDLK_LSHIFT || key == SDLK_RSHIFT)
        keyboard_set_key_raw(vk_shift, state);
    if (key == SDLK_LALT || key == SDLK_RALT)
        keyboard_set_key_raw(vk_alt, state);
    if (key == SDLK_LCTRL || key == SDLK_RCTRL)
        keyboard_set_key_raw(vk_control, state);

    if (key >= SDLK_a && key <= SDLK_z) {
        keyboard_set_key_raw(key + ('A' - 'a'), state);
    }
    if (key >= SDLK_0 && key <= SDLK_9) {
        keyboard_set_key_raw(key, state);
    }

    switch (key)
    {
    case SDLK_RETURN:      keyboard_set_key_raw(vk_enter, state);       break;
    case SDLK_ESCAPE:      keyboard_set_key_raw(vk_escape, state);      break;
    case SDLK_SPACE:       keyboard_set_key_raw(vk_space, state);       break;
    case SDLK_BACKSPACE:   keyboard_set_key_raw(vk_backspace, state);   break;
    case SDLK_TAB:         keyboard_set_key_raw(vk_tab, state);         break;
    case SDLK_PAUSE:       keyboard_set_key_raw(vk_pause, state);       break;
    case SDLK_PRINTSCREEN: keyboard_set_key_raw(vk_printscreen, state); break;
    case SDLK_LEFT:        keyboard_set_key_raw(vk_left, state);        break;
    case SDLK_RIGHT:       keyboard_set_key_raw(vk_right, state);       break;
    case SDLK_UP:          keyboard_set_key_raw(vk_up, state);          break;
    case SDLK_DOWN:        keyboard_set_key_raw(vk_down, state);        break;
    case SDLK_HOME:        keyboard_set_key_raw(vk_home, state);        break;
    case SDLK_END:         keyboard_set_key_raw(vk_end, state);         break;
    case SDLK_DELETE:      keyboard_set_key_raw(vk_delete, state);      break;
    case SDLK_INSERT:      keyboard_set_key_raw(vk_insert, state);      break;
    case SDLK_PAGEUP:      keyboard_set_key_raw(vk_pageup, state);      break;
    case SDLK_PAGEDOWN:    keyboard_set_key_raw(vk_pagedown, state);    break;
    case SDLK_F1:          keyboard_set_key_raw(vk_f1, state);          break;
    case SDLK_F2:          keyboard_set_key_raw(vk_f2, state);          break;
    case SDLK_F3:          keyboard_set_key_raw(vk_f3, state);          break;
    case SDLK_F4:          keyboard_set_key_raw(vk_f4, state);          break;
    case SDLK_F5:          keyboard_set_key_raw(vk_f5, state);          break;
    case SDLK_F6:          keyboard_set_key_raw(vk_f6, state);          break;
    case SDLK_F7:          keyboard_set_key_raw(vk_f7, state);          break;
    case SDLK_F8:          keyboard_set_key_raw(vk_f8, state);          break;
    case SDLK_F9:          keyboard_set_key_raw(vk_f9, state);          break;
    case SDLK_F10:         keyboard_set_key_raw(vk_f10, state);         break;
    case SDLK_F11:         keyboard_set_key_raw(vk_f11, state);         break;
    case SDLK_F12:         keyboard_set_key_raw(vk_f12, state);         break;
    case SDLK_KP_0:        keyboard_set_key_raw(vk_numpad0, state);     break;
    case SDLK_KP_1:        keyboard_set_key_raw(vk_numpad1, state);     break;
    case SDLK_KP_2:        keyboard_set_key_raw(vk_numpad2, state);     break;
    case SDLK_KP_3:        keyboard_set_key_raw(vk_numpad3, state);     break;
    case SDLK_KP_4:        keyboard_set_key_raw(vk_numpad4, state);     break;
    case SDLK_KP_5:        keyboard_set_key_raw(vk_numpad5, state);     break;
    case SDLK_KP_6:        keyboard_set_key_raw(vk_numpad6, state);     break;
    case SDLK_KP_7:        keyboard_set_key_raw(vk_numpad7, state);     break;
    case SDLK_KP_8:        keyboard_set_key_raw(vk_numpad8, state);     break;
    case SDLK_KP_9:        keyboard_set_key_raw(vk_numpad9, state);     break;
    case SDLK_KP_DIVIDE:   keyboard_set_key_raw(vk_divide, state);      break;
    case SDLK_KP_MULTIPLY: keyboard_set_key_raw(vk_multiply, state);    break;
    case SDLK_KP_MINUS:    keyboard_set_key_raw(vk_subtract, state);    break;
    case SDLK_KP_PLUS:     keyboard_set_key_raw(vk_add, state);         break;
    case SDLK_KP_DECIMAL:  keyboard_set_key_raw(vk_decimal, state);     break;
    case SDLK_LSHIFT:      keyboard_set_key_raw(vk_lshift, state);      break;
    case SDLK_LCTRL:       keyboard_set_key_raw(vk_lcontrol, state);    break;
    case SDLK_LALT:        keyboard_set_key_raw(vk_lalt, state);        break;
    case SDLK_RSHIFT:      keyboard_set_key_raw(vk_rshift, state);      break;
    case SDLK_RCTRL:       keyboard_set_key_raw(vk_rcontrol, state);    break;
    case SDLK_RALT:        keyboard_set_key_raw(vk_ralt, state);        break;
    default:
        break;
    }
}

ABI_ATTR void RegisterAndroidKeyEvent(int state, int key)
{
    keyboard_set_key_raw(key, state ? 0 : 1);
    printf("RegisterAndroidKeyEvent(%d, %d)\n", state, key);
}

// This hook disabled the majority of the input code, leaving this up to be implemented
// in a per-platform basis. Check sdl2_media.c as an example.
ABI_ATTR void IO_Start_Step_hook()
{
    if (g_MousePosX[0] < 0) g_MousePosX[0] = 0;
    if (g_MousePosY[0] < 0) g_MousePosY[0] = 0;
    if (g_MousePosX[0] > Graphics_DisplayWidth()) g_MousePosX[0] = Graphics_DisplayWidth();
    if (g_MousePosY[0] > Graphics_DisplayHeight()) g_MousePosY[0] = Graphics_DisplayHeight();

    *g_IOFrameCount = *g_IOFrameCount + 1;
    GamepadUpdate();
}

void patch_input(so_module *mod)
{
    hook_symbol(mod, "_Z23RegisterAndroidKeyEventii", (uintptr_t)&RegisterAndroidKeyEvent, 1);
    hook_symbol(mod, "_Z23RegisterAndroidKeyEventiiii", (uintptr_t)&RegisterAndroidKeyEvent, 1);
    hook_symbol(mod, "_Z13IO_Start_Stepv", (uintptr_t)&IO_Start_Step_hook, 1);
}

static Uint32 prev_mouse_state = 0;
int update_inputs(SDL_Window *win)
{
    app_in_focus = SDL_GetWindowFlags(win) & (SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS);

    memset(_IO_KeyPressed, 0, N_KEYS);
    memset(_IO_KeyReleased, 0, N_KEYS);

    *_IO_LastKey = *_IO_CurrentKey;
    // Other events (keyboard, gamepad hotplug, etc)
    SDL_Event ev;

    int any_key_down = 0;
    int any_key_pressed = 0;
    int any_key_released = 0;

    // The events are buffered for a single frame, but we want their availability to be known at frame one.
    // Fixes Death's Gambit
    for (int i = 0; i < ARRAY_SIZE(yoyo_gamepads); i++) {
        Gamepad *pad = &yoyo_gamepads[i];
        if (pad->is_available != pad->was_available) {
            pad->was_available = pad->is_available;
            const char *ev = (pad->is_available) ? "gamepad discovered" : "gamepad lost";

            // Send the async gamepad event
            int dsMap = CreateDsMap(0);
            dsMapAddString(dsMap, "event_type", strdup(ev));
            dsMapAddInt(dsMap, "pad_index", i);
            CreateAsynEventWithDSMap(dsMap, 0x4b);
        }
    }

    int evs = 0;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_CONTROLLERDEVICEADDED:
            if(gmloader_config.disable_controller == 1){
                printf("Controller %d ignored (disable_controller = True)\n", ev.cdevice.which);
                break;
            }

            // For this event, ev.cdevice.which refers to the Active device index.
            if (SDL_IsGameController(ev.cdevice.which)) {
                int slot = get_free_controller_slot();
                if (slot == -1) {
                    warning("Controller skipped, too many controllers.\n");
                    break;
                }

                controller_t *controller = &sdl_controllers[slot];
                SDL_GameController * ptr = SDL_GameControllerOpen(ev.cdevice.which);
                SDL_Joystick* joy = SDL_GameControllerGetJoystick(ptr);

                controller->which = SDL_JoystickInstanceID(joy);
                controller->controller = ptr;

                controller->slot = get_free_yoyogamepad_slot();
                if (controller->slot >= 0) {
                    yoyo_gamepads[controller->slot].is_available = 1;
                    warning("Controller '%s' assigned to player %d.\n", 
                        SDL_GameControllerName(controller->controller), controller->slot);
                } else {
                    warning("Controller '%s' assigned to none (no free slots).\n", 
                        SDL_GameControllerName(controller->controller));
                }
            }
            break;
        case SDL_CONTROLLERDEVICEREMOVED: {
            // For this event, ev.cdevice.which refers to the Joystick index.
            int slot = get_which_controller(ev.cdevice.which);
            if (slot < 0)
                break;

            controller_t *controller = &sdl_controllers[slot];
            if (controller->slot >= 0) {
                yoyo_gamepads[controller->slot].is_available = 0;
                warning("Disconnected player %d controller (%d)!\n", controller->slot, slot);
            } else {
                warning("Disconnected unassigned controller (%d)!\n", slot);
            }

            SDL_GameControllerClose(controller->controller);
            controller->controller = NULL;
            controller->which = -1;
            controller->slot = -1;
            break;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (ev.key.repeat == 1)
                break;

            if (ev.key.state == SDL_PRESSED) any_key_pressed = 1;
            if (ev.key.state == SDL_RELEASED) any_key_released = 1;
            keyboard_set_key(ev.key.keysym.sym, ev.key.state);
            break;
        case SDL_QUIT:
            return 0;
            break;
        }
    }

    for (int i = 2; i < N_KEYS; i++) {
        if (_IO_KeyDown[i]) {
            any_key_down = 1;
            break;
        }
    }

    // Gamepad Input Code
    SDL_GameControllerUpdate();
    for (int i = 0; i < ARRAY_SIZE(sdl_controllers); i++) {
        controller_t *controller = &sdl_controllers[i];
        int slot = controller->slot;
        if (slot < 0 || slot >= ARRAY_SIZE(yoyo_gamepads))
            continue;

        uint8_t new_states[16] = {};
        int k = 0;
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_A);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_B);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_X);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_Y);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        new_states[k++] = SDL_GameControllerGetAxis  (controller->controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 8000;
        new_states[k++] = SDL_GameControllerGetAxis  (controller->controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 8000;
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_BACK);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_START);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_LEFTSTICK);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        new_states[k++] = SDL_GameControllerGetButton(controller->controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

        for (int j = 0; j < ARRAY_SIZE(yoyo_gamepads[slot].buttons); j++) {
            // down -> held or up -> cleared
            yoyo_gamepads[slot].buttons[j] = (double)update_button(new_states[j], (int)yoyo_gamepads[slot].buttons[j]);
        }

        k = 0;
        yoyo_gamepads[slot].axis[k++] = ((double)SDL_GameControllerGetAxis(controller->controller, SDL_CONTROLLER_AXIS_LEFTX)) / 32767.f;
        yoyo_gamepads[slot].axis[k++] = ((double)SDL_GameControllerGetAxis(controller->controller, SDL_CONTROLLER_AXIS_LEFTY)) / 32767.f;
        yoyo_gamepads[slot].axis[k++] = ((double)SDL_GameControllerGetAxis(controller->controller, SDL_CONTROLLER_AXIS_RIGHTX)) / 32767.f;
        yoyo_gamepads[slot].axis[k++] = ((double)SDL_GameControllerGetAxis(controller->controller, SDL_CONTROLLER_AXIS_RIGHTY)) / 32767.f;
    }

    // Mouse Code
    int x, y;
    Uint32 mouse_state = SDL_GetMouseState(&x, &y);

    memset(_IO_ButtonDown, 0, N_BUTTONS);
    memset(_IO_ButtonPressed, 0, N_BUTTONS);
    memset(_IO_ButtonReleased, 0, N_BUTTONS);

    if (mouse_has_warped == 1) {
        mouse_has_warped = 0;
        SDL_WarpMouseInWindow(win, g_MousePosX[0], g_MousePosY[0]);
    }

    g_MousePosX[0] = x;
    g_MousePosY[0] = y;

    #define IS_SET(state, mask) (((state) & (mask)) == (mask))
    for (int i = 0; i < 3; i++) {
        int id = mouse_button_map(i + SDL_BUTTON_LEFT);
        Uint32 mask = (1 << i);
        _IO_ButtonDown[id]     =  IS_SET(mouse_state, mask);
        _IO_ButtonPressed[id]  =  IS_SET(mouse_state, mask) && !IS_SET(prev_mouse_state, mask);
        _IO_ButtonReleased[id] = !IS_SET(mouse_state, mask) &&  IS_SET(prev_mouse_state, mask);
    }

    prev_mouse_state = mouse_state;

    return 1;
}
