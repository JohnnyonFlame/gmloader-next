#include <stdio.h>
#include <stdarg.h>

#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"
#include "stdlib.h"

ABI_ATTR create_async_event_with_ds_map_t CreateAsynEventWithDSMap = NULL;
ABI_ATTR create_ds_map_t CreateDsMap = NULL;
ABI_ATTR bool (*dsMapAddDouble)(int, const char*, double) = NULL;
ABI_ATTR bool (*dsMapAddString)(int, const char*, const char*) = NULL;
ABI_ATTR fct_add_t Function_Add = NULL;
ABI_ATTR float (*Audio_GetTrackPos)(int sound_id) = NULL;
ABI_ATTR int (*Audio_WAVs)(uint8_t*, uint32_t, uint8_t*, int) = NULL;
ABI_ATTR int32_t (*Graphics_DisplayHeight)() = NULL;
ABI_ATTR int32_t (*Graphics_DisplayWidth)() = NULL;
ABI_ATTR int32_t (*YYGetInt32)(RValue *val, int idx) = NULL;
ABI_ATTR double (*YYGetReal)(RValue *val, int idx) = NULL;
ABI_ATTR void (*YYCreateString)(RValue *val, const char *str) = NULL;
ABI_ATTR routine_t F_YoYo_DrawTextureFlush = NULL;
ABI_ATTR uint32_t *(*ReadPNGFile)(void *a1, int a2, int *a3, int *a4, int a5) = NULL;
ABI_ATTR void (*Audio_PrepareGroup)(void*) = NULL;
ABI_ATTR void (*CThread__start_NS)(CThread *, void *, void *) = NULL;
ABI_ATTR void (*CThread__start_PcNS)(CThread *, void *, void *, char *) = NULL;
ABI_ATTR void (*FreePNGFile)() = NULL;
ABI_ATTR void (*GamepadUpdate)() = NULL;
ABI_ATTR void (*InitLLVM)(LLVMVars *) = NULL;
ABI_ATTR void (*InvalidateTextureState)() = NULL;
ABI_ATTR void (*MemoryManager__Free)(void *) = NULL;
ABI_ATTR void (*Mutex__ctor)(void*, char*) = NULL;
ABI_ATTR void (*Mutex__dtor)(void *) = NULL;
ABI_ATTR void (*SetWorkingDirectory_ptr)() = NULL;
bionic_off_t *g_GameFileLength = NULL; //android had 32bit off_t???
char **g_pWorkingDirectory = NULL;
char *g_fNoAudio = NULL;
char *g_fYYC = NULL;
int *g_TextureScale = NULL;
int g_fdGameFileBuffer = -1;
int32_t *g_MousePosX = NULL;
int32_t *g_MousePosY = NULL;
LLVMVars **g_pLLVMVars = NULL;
long long *g_GML_DeltaTime = NULL;
int *the_numb = NULL;
RFunction **the_functions = NULL;
uint32_t *g_IOFrameCount = NULL;
uint8_t *_IO_ButtonDown = NULL;
uint8_t *_IO_ButtonPressed = NULL;
uint8_t *_IO_ButtonReleased = NULL;
uint8_t *_IO_CurrentKey = NULL;
uint8_t *_IO_KeyDown = NULL;
uint8_t *_IO_KeyPressed = NULL;
uint8_t *_IO_KeyReleased = NULL;
uint8_t *_IO_LastKey = NULL;
uintptr_t *Current_Room = NULL;
uintptr_t *New_Room = NULL;
void **g_nYYCode = NULL;
void **g_pGameFileBuffer = NULL;
void **g_ppYYStackTrace = NULL;

uint8_t prev_kbd_state[N_KEYS] = {};
uint8_t cur_keys[N_KEYS] = {};

static const char *fake_functs[] = {
    "object_set_collisions",
    "psn_show_error_dialog",
    "psn_check_free_space",
    "psn_get_leaderboard_score_range",
    "psn_default_user_name",
    "psn_name_for_pad",
    "psn_unlock_trophy",
    "psn_get_trophy_unlock_state",
    "psn_init_np_libs",
    "psn_exit_np_libs",
    "psn_get_leaderboard_score",
    "psn_post_leaderboard_score",
    "psn_post_leaderboard_score_comment",
    "psn_check_np_availability",
    "psn_tick_error_dialog",
    "psn_tick",
    "psn_np_status",
    "psn_get_friends_scores",
    "psn_name_for_user",
    "psn_default_user",
    "psn_user_for_pad"
};

ABI_ATTR int _dbg_csol_print(void *csol, const char *fmt, ...) {
    char csol_str[2048];
    va_list list;

    va_start(list, fmt);
    int ret = vsnprintf(csol_str, sizeof(csol_str)-1, fmt, list);
    warning("%s", csol_str);
    va_end(list);
    return ret;
}

ABI_ATTR void show_message(const char *c)
{
    printf("%s\n", c);
}

// Crashes the application, don't want to deal with this.
ABI_ATTR static void alNoop()
{
    return;
}

ABI_ATTR static void stub_gml(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    /* */
}

ABI_ATTR static void game_end_reimpl(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    *New_Room = 0xffffff9c;
}

double FORCE_PLATFORM = os_android;
ABI_ATTR static double force_platform_type()
{
    return FORCE_PLATFORM;
}

ABI_ATTR void force_platform_type_gms2(void *self, int n, RValue *args)
{
    WARN_STUB;
    args[0].kind = VALUE_REAL;
    args[0].rvalue.val = FORCE_PLATFORM;
}

ABI_ATTR static void steam_initialised(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 0;
}

ABI_ATTR static void steam_stats_ready(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 0;
}

void patch_libyoyo(so_module *mod)
{
    // Load all of the native symbols referenced
    // ENSURE_SYMBOL(mod, Audio_GetTrackPos, "_Z17Audio_GetTrackPosi");
    ENSURE_SYMBOL(mod, Audio_PrepareGroup, "_Z18Audio_PrepareGroupi");
    // ENSURE_SYMBOL(mod, Audio_WAVs, "_Z10Audio_WAVsPhjS_i");
    ENSURE_SYMBOL(mod, CreateAsynEventWithDSMap, "_Z24CreateAsynEventWithDSMapii");
    ENSURE_SYMBOL(mod, CreateDsMap, "_Z11CreateDsMapiz");
    ENSURE_SYMBOL(mod, dsMapAddString, "dsMapAddString");
    ENSURE_SYMBOL(mod, dsMapAddDouble, "dsMapAddDouble");
    ENSURE_SYMBOL(mod, Current_Room, "Current_Room");
    ENSURE_SYMBOL(mod, Function_Add, "_Z12Function_AddPKcPFvR6RValueP9CInstanceS4_iPS1_Eib", "_Z12Function_AddPcPFvR6RValueP9CInstanceS3_iPS0_Eib");
    ENSURE_SYMBOL(mod, FreePNGFile, "_Z11FreePNGFilev");
    ENSURE_SYMBOL(mod, GamepadUpdate, "_Z14GamepadUpdateMv");
    ENSURE_SYMBOL(mod, Graphics_DisplayHeight, "_Z22Graphics_DisplayHeightv");
    ENSURE_SYMBOL(mod, Graphics_DisplayWidth, "_Z21Graphics_DisplayWidthv");
    ENSURE_SYMBOL(mod, g_fNoAudio, "g_fNoAudio");
    ENSURE_SYMBOL(mod, g_fYYC, "g_fYYC");
    ENSURE_SYMBOL(mod, g_GameFileLength, "g_GameFileLength");
    ENSURE_SYMBOL(mod, g_GML_DeltaTime, "g_GML_DeltaTime");
    ENSURE_SYMBOL(mod, g_IOFrameCount, "g_IOFrameCount");
    ENSURE_SYMBOL(mod, g_MousePosX, "g_MousePosX");
    ENSURE_SYMBOL(mod, g_MousePosY, "g_MousePosY");
    ENSURE_SYMBOL(mod, g_nYYCode, "g_nYYCode");
    ENSURE_SYMBOL(mod, g_pGameFileBuffer, "g_pGameFileBuffer");
    ENSURE_SYMBOL(mod, g_pLLVMVars, "g_pLLVMVars");
    ENSURE_SYMBOL(mod, g_pWorkingDirectory, "g_pWorkingDirectory");
    ENSURE_SYMBOL(mod, g_TextureScale, "g_TextureScale");
    ENSURE_SYMBOL(mod, InitLLVM, "_Z8InitLLVMP9SLLVMVars");
    ENSURE_SYMBOL(mod, InvalidateTextureState, "_Z23_InvalidateTextureStatev");
    ENSURE_SYMBOL(mod, MemoryManager__Free, "_ZN13MemoryManager4FreeEPv", "_ZN13MemoryManager4FreeEPKv");
    ENSURE_SYMBOL(mod, Mutex__ctor, "_ZN5MutexC1EPKc");
    ENSURE_SYMBOL(mod, Mutex__dtor, "_ZN5MutexD2Ev", "_ZN5MutexD1Ev");
    ENSURE_SYMBOL(mod, New_Room, "New_Room");
    ENSURE_SYMBOL(mod, ReadPNGFile, "_Z11ReadPNGFilePviPiS0_b");
    ENSURE_SYMBOL(mod, the_functions, "the_functions");
    ENSURE_SYMBOL(mod, the_numb, "the_numb");
    ENSURE_SYMBOL(mod, YYGetInt32, "_Z10YYGetInt32PK6RValuei");
    ENSURE_SYMBOL(mod, YYGetReal, "_Z9YYGetRealPK6RValuei");
    ENSURE_SYMBOL(mod, YYCreateString, "_Z14YYCreateStringP6RValuePKc");
    ENSURE_SYMBOL(mod, _IO_ButtonDown, "_IO_ButtonDown");
    ENSURE_SYMBOL(mod, _IO_ButtonPressed, "_IO_ButtonPressed");
    ENSURE_SYMBOL(mod, _IO_ButtonReleased, "_IO_ButtonReleased");
    ENSURE_SYMBOL(mod, _IO_CurrentKey, "_IO_CurrentKey", "l_IO_CurrentKey");
    ENSURE_SYMBOL(mod, _IO_KeyDown, "_IO_KeyDown", "l_IO_KeyDown");
    ENSURE_SYMBOL(mod, _IO_KeyPressed, "_IO_KeyPressed", "l_IO_KeyPressed");
    ENSURE_SYMBOL(mod, _IO_KeyReleased, "_IO_KeyReleased", "l_IO_KeyReleased");
    ENSURE_SYMBOL(mod, _IO_LastKey, "_IO_LastKey", "l_IO_LastKey");

    // Hook messages for debug
    hook_symbol(mod, "_Z11ShowMessagePKc", (uintptr_t)&show_message, 1);
    hook_symbol(mod, "_ZN12DummyConsole6OutputEPKcz", (uintptr_t)&_dbg_csol_print, 1);
    hook_symbol(mod, "_ZN8TConsole6OutputEPKcz", (uintptr_t)&_dbg_csol_print, 1);
    hook_symbol(mod, "_ZN11TRelConsole6OutputEPKcz", (uintptr_t)&_dbg_csol_print, 1);
    hook_symbol(mod, "_ZN17TErrStreamConsole6OutputEPKcz", (uintptr_t)&_dbg_csol_print, 1);

    // Avoid OpenAL crashes due to the static overrides 
    hook_symbol(mod, "alBufferMarkNeedsFreed", (uintptr_t)&alNoop, 1);
    hook_symbol(mod, "_Z22alBufferMarkNeedsFreedj", (uintptr_t)&alNoop, 1);
    hook_symbol(mod, "_Z17alBufferDebugNamejPKc", (uintptr_t)&alNoop, 1);

    // Lie about the runner's platform to the game.
    hook_symbol(mod, "_Z23YoYo_GetPlatform_DoWorkv", (uintptr_t)&force_platform_type, 1);
    hook_symbol(mod, "_Z20GET_YoYo_GetPlatformP9CInstanceiP6RValue", (uintptr_t)&force_platform_type_gms2, 1);

#if defined(__arm__)
    // Add a "game_end" function (useless on 1.4.1788+ until 1.4.1804ish?)
    // From 'https://store.yoyogames.com/downloads/gm-studio/release-notes-studio.html':
    //   "Changed game_end() to be ignored on Android and iOS, following crash 
    //   logs that indicated this was causing memory corruption and crashes in
    //   subsequent runs of your app"
    
    uint32_t *_Z15Command_EndGamev = (uint32_t *)so_symbol(mod, "_Z15Command_EndGamev");
    // _Z15Command_EndGamev+0x00: bx lr (noop function)
    if (_Z15Command_EndGamev != NULL && *_Z15Command_EndGamev == 0xE12FFF1E) {
        Function_Add("game_end", game_end_reimpl, 1, 1);
    }
#endif

    // Define some stubbed gamemaker functions for compatibility with odd games
    for (int i = 0; i < ARRAY_SIZE(fake_functs); i++) {
        Function_Add(fake_functs[i], stub_gml, 1, 1);
    }

    Function_Add("steam_initialised", steam_initialised, 0, 1);
    Function_Add("steam_stats_ready", steam_stats_ready, 0, 1);

    so_symbol_fix_ldmia(mod, "_Z11Shader_LoadPhjS_");
}