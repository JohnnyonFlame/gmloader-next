#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unordered_map>

#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"
#include "configuration.h"

ABI_ATTR create_async_event_with_ds_map_t CreateAsynEventWithDSMap = NULL;
ABI_ATTR create_ds_map_t CreateDsMap = NULL;
ABI_ATTR bool (*dsMapAddDouble)(int, const char*, double) = NULL;
ABI_ATTR bool (*dsMapAddInt)(int, const char*, int) = NULL;
ABI_ATTR bool (*dsMapAddString)(int, const char*, const char*) = NULL;
ABI_ATTR fct_add_t Function_Add = NULL;
ABI_ATTR float (*Audio_GetTrackPos)(int sound_id) = NULL;
ABI_ATTR int (*Audio_WAVs)(uint8_t*, uint32_t, uint8_t*, int) = NULL;
ABI_ATTR int32_t (*Graphics_DisplayHeight)() = NULL;
ABI_ATTR int32_t (*Graphics_DisplayWidth)() = NULL;
ABI_ATTR int32_t (*YYGetInt32)(RValue *val, int idx) = NULL;
ABI_ATTR int64_t (*YYGetInt64)(RValue *val, int idx) = NULL;
ABI_ATTR double (*YYGetReal)(RValue *val, int idx) = NULL;
ABI_ATTR void (*YYCreateString)(RValue *val, const char *str) = NULL;
ABI_ATTR routine_t F_YoYo_DrawTextureFlush = NULL;
ABI_ATTR uint32_t *(*ReadPNGFile)(void *a1, int a2, int *a3, int *a4, char a5) = NULL;
ABI_ATTR void (*Audio_PrepareGroup)(void*) = NULL;
ABI_ATTR void (*CThread__start_NS)(CThread *, void *, void *) = NULL;
ABI_ATTR void (*CThread__start_PcNS)(CThread *, void *, void *, char *) = NULL;
ABI_ATTR void (*FreePNGFile)() = NULL;
ABI_ATTR void (*GamepadUpdate)() = NULL;
ABI_ATTR void (*InitLLVM)(LLVMVars *) = NULL;
ABI_ATTR void (*InvalidateTextureState)() = NULL;
ABI_ATTR void (*MemoryManager__Free)(void *) = NULL;
ABI_ATTR void (*MemoryManager__Free_2)(void *) = NULL;
ABI_ATTR void (*Mutex__ctor)(void*, char*) = NULL;
ABI_ATTR void (*Mutex__dtor)(void *) = NULL;
ABI_ATTR void (*SetWorkingDirectory_ptr)() = NULL;
ABI_ATTR void (*surface_depth_disable)(RValue *ret, void *self, void *other, int argc, RValue *args) = NULL;
ABI_ATTR char (*Variable_GetValue_Direct)(void* inst, int var_slot, int array_idx, RValue *val, char unk1, char unk2) = NULL;
ABI_ATTR char (*Variable_SetValue_Direct)(void* inst, int var_slot, int array_idx, RValue *val) = NULL;
ABI_ATTR char (*Variable_GetBuiltIn_Direct)(void* inst, int var_slot, int array_idx, RValue *val) = NULL;
ABI_ATTR char (*Variable_SetBuiltIn_Direct)(void* inst, int var_slot, int array_idx, RValue *val) = NULL;
ABI_ATTR void (*Code_Function_GET_the_function)(int numb,char **name,void **code,int *args, int *unk) = NULL;
ABI_ATTR void (*_RefThing__dec)(void *ref) = NULL;
ABI_ATTR int (*Variable_BuiltIn_Find)(const char *name) = NULL;
ABI_ATTR long (*PrepareGame)(void) = NULL;
ABI_ATTR int (*Code_Variable_Find_Slot_From_Name)(void *instance, const char *name) = NULL;
ABI_ATTR int (*Variable_FindName)(const char *name) = NULL; /* Unavailable in GMS 1.4+, very old symbol */
ABI_ATTR long (*ExecuteIt)(void *self, void *other, void *code, RValue *args) = NULL;
ABI_ATTR long (*ExecuteIt_flags)(void *self, void *other, void *code, RValue *args, int argc) = NULL;

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
int *g_nInstanceVariables = NULL;
void **g_pGlobal = NULL;
struct CCode **g_pFirstCode = NULL;
int *g_TotalCodeBlocks = NULL;
int *g_ArgumentCount = NULL;
RValue **Argument = NULL;
FunctionEntry **the_functions = NULL;
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
int *Extension_Main_number = NULL;
uint8_t prev_kbd_state[N_KEYS] = {};
uint8_t cur_keys[N_KEYS] = {};
static const char *fake_functs[] = {
    "object_set_collisions",
};
double FORCE_PLATFORM = os_android;

size_t g_function_stride = 0;
fct_add_t Original_Function_Add = NULL;
static std::unordered_map<std::string, FunctionEntry*> g_function_map;

ReentrantHook REHPrepareGame = {};
ReentrantHook REHFunctionAdd = {};

/*
* YoYo's Function_Add used to blindly append functions to the function table.
* This allowed duplicate functions which worked since our functions are added to the table before the engine does its own thing.
* As of 2024.14, Function_Add checks for duplicates and fails if a function with the same name already exists.
* Sadly it adds a new index to the table array before this check, so when the check fails there's a null index which is later dereferenced.
* Oops.
*/

// Detect the size/stride of FunctionEntry in the function table
size_t detect_function_entry_stride(FunctionEntry **functions, int count) {
    if (!functions || count < 2) return sizeof(FunctionEntry);

    FunctionEntry* first = functions[0];
    FunctionEntry* second = functions[1];

    size_t stride = reinterpret_cast<uintptr_t>(second) - reinterpret_cast<uintptr_t>(first);
    if (stride < sizeof(FunctionEntry)) {
        stride = sizeof(FunctionEntry);
    }
    return stride;
}

void Function_Add_Hook(const char* f_name, routine_t func, int arg_count, char reg)
{
    if (!the_functions || !the_numb) {
        return;
    }

    // Stride Detection
    if (g_function_stride == 0) {
        if (*the_numb >= 2) {
            g_function_stride = detect_function_entry_stride(the_functions, *the_numb);
        } else {
            g_function_stride = 0x18;
        }
    }

    auto it = g_function_map.find(f_name);
    if (it != g_function_map.end()) {
        return;
    }

    rehook_unhook(&REHFunctionAdd);
    Original_Function_Add(f_name, func, arg_count, reg);
    rehook_hook(&REHFunctionAdd);

    FunctionEntry* list_base = (FunctionEntry*)*the_functions;
    int n = *the_numb;

    FunctionEntry* new_entry = reinterpret_cast<FunctionEntry*>(
        reinterpret_cast<uintptr_t>(list_base) + (n - 1) * g_function_stride
    );

    g_function_map[f_name] = new_entry;
}

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

ABI_ATTR static void dont_init_extensions()
{
    //*Extension_Main_number = 0;
    warning("dont_init_extensions!!!\n");
}

ABI_ATTR static void stub_gml(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 0;
    /* */
}

ABI_ATTR static void game_end_reimpl(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    *New_Room = 0xffffff9c;
}

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

ABI_ATTR static void window_handle(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    warning("-- Called window_handle stub ! --\n");
    ret->kind = VALUE_INT64;
    ret->rvalue.v64 = 0;
}

void send_async_social(const char* type)
{
        if (CreateAsynEventWithDSMap != NULL && CreateDsMap != NULL && dsMapAddString != NULL) {
        int ds_map = CreateDsMap(0);
        dsMapAddString(ds_map, "type", type);
        CreateAsynEventWithDSMap(ds_map, 70);
        }
}

#ifndef VIDEO_SUPPORT
ABI_ATTR static void video_open_reimpl(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_BOOL;
    ret->rvalue.val = 1;

    if (CreateAsynEventWithDSMap != NULL && CreateDsMap != NULL && dsMapAddString != NULL) {
        int ds_map = CreateDsMap(0);
        dsMapAddString(ds_map, "type", "video_end");
        if (argc > 0 && args[0].kind == VALUE_STRING) {
            dsMapAddString(ds_map, "path", YYGetCStrHelper(args, 0));
        }
        CreateAsynEventWithDSMap(ds_map, 70);
    }
}
#endif

// Implementation of game_change which is not available for android normally
//  Takes two arguments: work_dir and launch_params (example: "/chapter1_windows" "-game data.win")
//  https://manual.gamemaker.io/beta/en/GameMaker_Language/GML_Reference/General_Game_Control/game_change.htm
ABI_ATTR void game_change_reimpl(RValue *ret, void *self, void *other, int argc, RValue *args) {
    std::string buffer = "game_change(): ";
    for (int i = 0; i < argc; i++) {
        const char *arg = (args[i].kind == VALUE_STRING) ? YYGetCStrHelper(args, i) : "INVALID";
        if (i > 0)
            buffer += ", ";
        buffer += arg;
    }

    warning("%s\n", buffer.c_str());

    if (ret) {
        ret->kind = VALUE_BOOL;
        ret->rvalue.val = 0;
    }

    if (argc < 2) {
        warning("game_change(): Requires at least two arguments (workdir and params)\n");
        return;
    }

    for (int i = 0; i < argc; i++) {
        if (args[i].kind != VALUE_STRING || !args[i].rvalue.str || !args[i].rvalue.str->m_thing) {
            warning("game_change(): Argument %d is not a valid string\n", i);
            return;
        }
    }

    gc_workdir = strdup(YYGetCStrHelper(args, 0));
    if (!gc_workdir) {
        warning("game_change(): Failed to duplicate workdir\n");
        return;
    }

    std::string sub_path(gc_workdir);
    if (!sub_path.empty() && sub_path.back() != '/') {
        sub_path += '/';
    }

    std::string full_path = gmloader_config.save_dir + sub_path;
    warning("game_change(): Resolved game path: '%s'\n", full_path.c_str());

    relaunch_flag = 1;

    if (ret) {
        ret->kind = VALUE_BOOL;
        ret->rvalue.val = 1;
    }
}

extern int setup_ended;
ABI_ATTR long PrepareGame_hook()
{
    setup_ended = 1;
    rehook_unhook(&REHPrepareGame);
    long ret = PrepareGame();
    warning("- PrepareGame done.\n");
    return ret;
}

void patch_libyoyo(so_module *mod)
{
    // Load all of the native symbols referenced
    // ENSURE_SYMBOL(mod, Audio_GetTrackPos, "_Z17Audio_GetTrackPosi");
    ENSURE_SYMBOL(mod, Audio_PrepareGroup, "_Z18Audio_PrepareGroupi");
    // ENSURE_SYMBOL(mod, Audio_WAVs, "_Z10Audio_WAVsPhjS_i");
    ENSURE_SYMBOL(mod, CreateAsynEventWithDSMap, "_Z24CreateAsynEventWithDSMapii");
    ENSURE_SYMBOL(mod, CreateDsMap, "_Z11CreateDsMapiz", "dsMapCreate");
    ENSURE_SYMBOL(mod, dsMapAddString, "dsMapAddString");
    // ENSURE_SYMBOL(mod, dsMapAddDouble, "dsMapAddDouble");
    ENSURE_SYMBOL(mod, dsMapAddInt, "dsMapAddInt");
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
    ENSURE_SYMBOL(mod, Mutex__ctor, "_ZN5MutexC1EPKc");
    ENSURE_SYMBOL(mod, Mutex__dtor, "_ZN5MutexD2Ev", "_ZN5MutexD1Ev");
    ENSURE_SYMBOL(mod, New_Room, "New_Room");
    ENSURE_SYMBOL(mod, ReadPNGFile, "_Z11ReadPNGFilePviPiS0_b");
    ENSURE_SYMBOL(mod, the_functions, "the_functions");
    ENSURE_SYMBOL(mod, the_numb, "the_numb");
    ENSURE_SYMBOL(mod, g_pGlobal, "g_pGlobal");
    ENSURE_SYMBOL(mod, g_pFirstCode, "g_pFirstCode");
    ENSURE_SYMBOL(mod, g_TotalCodeBlocks, "g_TotalCodeBlocks");
    ENSURE_SYMBOL(mod, g_ArgumentCount, "g_ArgumentCount");
    ENSURE_SYMBOL(mod, Argument, "Argument");
    ENSURE_SYMBOL(mod, YYGetInt32, "_Z10YYGetInt32PK6RValuei");
    ENSURE_SYMBOL(mod, YYGetInt64, "_Z10YYGetInt64PK6RValuei");
    ENSURE_SYMBOL(mod, YYGetReal, "_Z9YYGetRealPK6RValuei");
    ENSURE_SYMBOL(mod, _IO_ButtonDown, "_IO_ButtonDown");
    ENSURE_SYMBOL(mod, _IO_ButtonPressed, "_IO_ButtonPressed");
    ENSURE_SYMBOL(mod, _IO_ButtonReleased, "_IO_ButtonReleased");
    ENSURE_SYMBOL(mod, _IO_CurrentKey, "_IO_CurrentKey", "l_IO_CurrentKey");
    ENSURE_SYMBOL(mod, _IO_KeyDown, "_IO_KeyDown", "l_IO_KeyDown");
    ENSURE_SYMBOL(mod, _IO_KeyPressed, "_IO_KeyPressed", "l_IO_KeyPressed");
    ENSURE_SYMBOL(mod, _IO_KeyReleased, "_IO_KeyReleased", "l_IO_KeyReleased");
    ENSURE_SYMBOL(mod, _IO_LastKey, "_IO_LastKey", "l_IO_LastKey");
    ENSURE_SYMBOL(mod, Variable_GetValue_Direct, "_Z24Variable_GetValue_DirectP12YYObjectBaseiiP6RValue", "_Z24Variable_GetValue_DirectP12YYObjectBaseiiP6RValuebb", "_Z24Variable_GetValue_DirectP9CInstanceiiP6RValue");
    ENSURE_SYMBOL(mod, Variable_SetValue_Direct, "_Z24Variable_SetValue_DirectP12YYObjectBaseiiP6RValue", "_Z24Variable_SetValue_DirectP9CInstanceiiP6RValue");
    ENSURE_SYMBOL(mod, Code_Function_GET_the_function, "_Z30Code_Function_GET_the_functioniPPKcPPvPi", "_Z30Code_Function_GET_the_functioniPPKcPPvPiS4_", "_Z30Code_Function_GET_the_functioniPPcPPvPiS3_");
    ENSURE_SYMBOL(mod, Variable_BuiltIn_Find, "_Z21Variable_BuiltIn_FindPKc", "_Z21Variable_BuiltIn_FindPc");
    // Versioned symbols
    FIND_SYMBOL(mod, ExecuteIt, "_Z9ExecuteItP9CInstanceS0_P5CCodeP6RValue");
    FIND_SYMBOL(mod, ExecuteIt_flags, "_Z9ExecuteItP9CInstanceS0_P5CCodeP6RValuei");
    FIND_SYMBOL(mod, MemoryManager__Free, "_ZN13MemoryManager4FreeEPv", "_ZN13MemoryManager4FreeEPKv");
    FIND_SYMBOL(mod, MemoryManager__Free_2, "_ZN13MemoryManager4FreeEPKvb");

    // Unavailable in newer versions
    FIND_SYMBOL(mod, g_nInstanceVariables, "g_nInstanceVariables"); /* Can be used to sus out for the older OUYA runners... */
    FIND_SYMBOL(mod, Variable_FindName, "_Z17Variable_FindNamePc");

    // Unavailable in older versions
    FIND_SYMBOL(mod, Variable_GetBuiltIn_Direct, "_Z26Variable_GetBuiltIn_DirectP12YYObjectBaseiiP6RValue");
    FIND_SYMBOL(mod, Variable_SetBuiltIn_Direct, "_Z26Variable_SetBuiltIn_DirectP12YYObjectBaseiiP6RValue");
    FIND_SYMBOL(mod, Code_Variable_Find_Slot_From_Name, "_Z33Code_Variable_Find_Slot_From_NameP12YYObjectBasePKc");
    FIND_SYMBOL(mod, YYCreateString, "_Z14YYCreateStringP6RValuePKc");
    FIND_SYMBOL(mod, _RefThing__dec, "_ZN9_RefThingIPKcE3decEv");

    // Depth disable
    FIND_SYMBOL(mod, surface_depth_disable, "_Z21F_SurfaceDepthDisableR6RValueP9CInstanceS2_iPS_");

    // Hook the start of the game so we know setup is done
    ENSURE_SYMBOL(mod, PrepareGame, "_Z11PrepareGamev");
    rehook_new(mod, &REHPrepareGame, (uintptr_t)PrepareGame, (uintptr_t)&PrepareGame_hook);
    rehook_hook(&REHPrepareGame);

    // Disable extension support
    FIND_SYMBOL(mod, Extension_Main_number, "Extension_Main_number");
    if (gmloader_config.disable_extensions == 1) {
        // hook_symbol(mod, "_Z20Extension_Initializev", (uintptr_t)&dont_init_extensions, 1);
        // hook_symbol(mod, "_Z20Extension_PrePreparev", (uintptr_t)&dont_init_extensions, 1);
        hook_symbol(mod, "_Z14Extension_LoadPhjS_", (uintptr_t)&dont_init_extensions, 1);
    }

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

    // Wrap Function_Add
    if (Function_Add && !Original_Function_Add)
    {
        Original_Function_Add = Function_Add;
        rehook_new(mod, &REHFunctionAdd, (uintptr_t)Function_Add, (uintptr_t)&Function_Add_Hook);
        rehook_hook(&REHFunctionAdd);
    }
    
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

    Function_Add("window_handle", window_handle, 0, 1);
    #ifndef VIDEO_SUPPORT //Add the stub if real video support isn't included
    Function_Add("video_open", video_open_reimpl, 1, 1);
    #endif
    Function_Add("game_change", game_change_reimpl, 2, 0);

    so_symbol_fix_ldmia(mod, "_Z11Shader_LoadPhjS_");
}

void disable_depth()
{      
    if (surface_depth_disable != NULL) {
        RValue ret;
        RValue args[1] = {
            {
                { .val = 1 },  // Union initialization
                0,             // Default value for flags
                VALUE_BOOL     // RValueType kind
            }
        };

        surface_depth_disable(&ret, NULL, NULL, 1, &args[0]);
        warning("HACK:: Disabled surface depth.\n");
    } else {
        warning("Depth disable hack requested but surface_depth_disable unusable.\n");
    }

}

int UsesRefStrings()
{
    return g_nInstanceVariables != NULL;
}

void YYCreateStringHelper(RValue *rval, const char *str)
{
    if (UsesRefStrings())
    {
        if (YYCreateString != NULL)
        {
            YYCreateString(rval, str);
        }
        else // need to create it manually...
        {
            YYThingDerefHelper(rval);
            Ref *ref = (Ref*)malloc(sizeof(*ref));
            *ref = (Ref){
                .m_thing = strdup(str),
                .m_refCount = 1,
                .m_size = strlen(str)
            };

            rval->kind = VALUE_STRING;
            rval->rvalue.str = ref;
        }
    }
    else // very old runners, strings werent refs
    {
        YYThingDerefHelper(rval);
        rval->kind = VALUE_STRING;
        rval->rvalue.str = (Ref*)strdup(str);
    }
}

void YYThingDerefHelper(RValue *rval)
{
    if (rval->kind != VALUE_STRING || rval->kind != VALUE_REF)
        return;

    if (UsesRefStrings())
    {
        if (_RefThing__dec)
        {
            _RefThing__dec(rval);
        }
        else
        {
            free(rval->rvalue.str->m_thing);
            rval->rvalue.str->m_thing = NULL;
            free(rval->rvalue.str);
            rval->rvalue.str = NULL;
            rval->kind = VALUE_UNSET;
        }
    }
    else
    {
        rval->rvalue.str = NULL;
        free(rval->rvalue.str);
        rval->kind = VALUE_UNSET;
    }
}

const char *YYGetCStrHelper(struct RValue *rval, int idx)
{
    if (UsesRefStrings())
        return (const char *)rval[idx].rvalue.str->m_thing;
    else
        return (const char *)rval[idx].rvalue.str;
}