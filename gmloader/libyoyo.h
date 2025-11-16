#pragma once

#define MASK_KIND_RVALUE 0x0ffffff
typedef enum RValueType {
	VALUE_REAL = 0,
	VALUE_STRING = 1,
	VALUE_ARRAY = 2,
	VALUE_PTR = 3,
	VALUE_VEC3 = 4,
	VALUE_UNDEFINED = 5,
	VALUE_OBJECT = 6,
	VALUE_INT32 = 7,
	VALUE_VEC4 = 8,
	VALUE_MATRIX = 9,
	VALUE_INT64 = 10,
	VALUE_ACCESSOR = 11,
	VALUE_JSNULL = 12,
	VALUE_BOOL = 13,
	VALUE_ITERATOR = 14,
	VALUE_REF = 15,
	VALUE_UNSET = MASK_KIND_RVALUE
} RValueType;

typedef struct Ref {
	void* m_thing;
	int m_refCount;
	int m_size;
} Ref;

typedef struct RValue {
	/* ... */
	union {
		int v32;
		long long v64;
		double val;
		/* pointers */

        Ref *str;
	} rvalue;

	int flags;
	RValueType kind;
} RValue;

typedef struct VariableDecl {
	char *name;
	RValue *rval;
	void *unk1;
	void *unk2;
} VariableDecl;

typedef enum GamepadButtonState {
	GAMEPAD_BUTTON_STATE_UP = -1,
	GAMEPAD_BUTTON_STATE_NEUTRAL = 0,
	GAMEPAD_BUTTON_STATE_HELD = 1,
	GAMEPAD_BUTTON_STATE_DOWN = 2
} GamepadButtonState;

#define MAX_GAMEPADS 4
typedef struct Gamepad {
	int is_available;
	int was_available;
	double buttons[16];
	double deadzone;
	double axis[4];
	double motors[2];

	Gamepad()
	: is_available(0), was_available(0), buttons{0}, deadzone(0.1), axis{0}, motors{}
	{ /* ... */ }
} Gamepad;

typedef ABI_ATTR void (*routine_t)(RValue *ret, void *self, void *other, int argc, RValue *args);

typedef struct FunctionEntry {
    char *f_name;
    routine_t func_ptr;
    int arg_count;
    int param_4;
} FunctionEntry;

typedef struct RFunction {
	char f_name[64];
	routine_t f_routine;
	int f_argnumb;
	uint32_t m_UsageCount;
} RFunction;

typedef struct CAudioGroup {
    int m_eLoadState;
    int m_groupId;
    int m_soundCount;
    int **m_soundsAdded;
    int m_soundsLoaded;
    int m_loadProgress;
    void * m_pData;
    char * m_pszName;
    struct CThread *m_pLoadThread;
    struct cAudio_Sound **m_ppSoundList;
} CAudioGroup;

typedef struct CAudioGroupMan {
    struct CAudioGroup **m_ppGroups;
    int m_groupCount;
} CAudioGroupMan;

typedef struct CThread {
    uint32_t m_hThread;
    int m_errorCode;
    char m_bTerminate;
    char m_bRunning;
    char m_bPaused;
    char padding;
    void * m_pFunctionArg;
    void * (* m_pThreadFunc)(void *);
    struct BIONIC_Mutex * m_pTermMutex;
} CThread;

typedef struct YYFunc
{
	const char *name;
	RValue *(*procedure)(struct CInstance *self, struct CInstance *other, RValue *ret, int count, RValue** args);
} YYFunc;

// TODO:: track down more usage of this structure and
// document it's internals a bit better.
typedef struct CCode {
	void *unk1; /* vmt? */
	struct CCode *m_next;
	int m_kind; /* < 3 are functions, == 3 are static accessors? what if we use & 0x10 to denote hooked? */
	void *m_text; /* unsure how to get this, investigate _ZN5CCode7GetTextEv? */
    void *padding1[5];
    int padding2[14];
	const char *m_name;
	int m_index; /* See how this is passed around, e.g. to CProfiler */
	const YYFunc *func;
} CCode;

#if INTPTR_MAX == INT32_MAX
static_assert(offsetof(CCode, m_name) == 0x5c, "Bad offset, check data/padding sizes.");
#else
static_assert(offsetof(CCode, m_name) == 0x80, "Bad offset, check data/padding sizes.");
#endif

static const int offsname = offsetof(CCode, m_name);

typedef struct LLVMVars {
    char *pWad;
    int	nWadFileLength;
    int	nGlobalVariables;
    int	nInstanceVariables;
    int	nYYCode;
    struct YYVAR **ppVars;
    struct YYVAR **ppFuncs;
    struct YYGMLFuncs *pGMLFuncs;
    void *pYYStackTrace;
} LLVMVars;

// OS values
// https://manual.gamemaker.io/monthly/en/GameMaker_Language/GML_Reference/OS_And_Compiler/os_type.htm
// https://github.com/UnderminersTeam/UndertaleModTool/blob/caf04126f273b57bb2200b8739bb9d57e25d8f92/UndertaleModLib/Compiler/BuiltinList.cs#L3165
// os_gxgames is unknown
// os_ps5 value is unknown
#define os_unknown    ((double)(-1.0f))
#define os_windows    ((double)(0.0f))
#define os_macosx     ((double)(1.0f))
#define os_ios        ((double)(3.0f))
#define os_android    ((double)(4.0f))
#define os_linux      ((double)(6.0f))
#define os_psvita     ((double)(12.0f))
#define os_ps4        ((double)(14.0f))
#define os_xboxone    ((double)(15.0f))
#define os_tvos       ((double)(20.0f))
#define os_switch     ((double)(21.0f))
#define gp_face1      ((double)(32769.0f))
#define gp_face2      ((double)(32770.0f))
#define gp_face3      ((double)(32771.0f))
#define gp_face4      ((double)(32772.0f))
#define gp_shoulderl  ((double)(32773.0f))
#define gp_shoulderr  ((double)(32774.0f))
#define gp_shoulderlb ((double)(32775.0f))
#define gp_shoulderrb ((double)(32776.0f))
#define gp_select     ((double)(32777.0f))
#define gp_start      ((double)(32778.0f))
#define gp_stickl     ((double)(32779.0f))
#define gp_stickr     ((double)(32780.0f))
#define gp_padu       ((double)(32781.0f))
#define gp_padd       ((double)(32782.0f))
#define gp_padl       ((double)(32783.0f))
#define gp_padr       ((double)(32784.0f))
#define gp_axislh     ((double)(32785.0f))
#define gp_axislv     ((double)(32786.0f))
#define gp_axisrh     ((double)(32787.0f))
#define gp_axisrv     ((double)(32788.0f))
#define vk_nokey       (0)
#define vk_anykey      (1)
#define vk_enter       (13)
#define vk_return      (13)
#define vk_shift       (16)
#define vk_control     (17)
#define vk_alt         (18)
#define vk_escape      (27)
#define vk_space       (32)
#define vk_backspace   (8)
#define vk_tab         (9)
#define vk_pause       (19)
#define vk_printscreen (44)
#define vk_left        (37)
#define vk_right       (39)
#define vk_up          (38)
#define vk_down        (40)
#define vk_home        (36)
#define vk_end         (35)
#define vk_delete      (46)
#define vk_insert      (45)
#define vk_pageup      (33)
#define vk_pagedown    (34)
#define vk_f1          (112)
#define vk_f2          (113)
#define vk_f3          (114)
#define vk_f4          (115)
#define vk_f5          (116)
#define vk_f6          (117)
#define vk_f7          (118)
#define vk_f8          (119)
#define vk_f9          (120)
#define vk_f10         (121)
#define vk_f11         (122)
#define vk_f12         (123)
#define vk_numpad0     (96)
#define vk_numpad1     (97)
#define vk_numpad2     (98)
#define vk_numpad3     (99)
#define vk_numpad4     (100)
#define vk_numpad5     (101)
#define vk_numpad6     (102)
#define vk_numpad7     (103)
#define vk_numpad8     (104)
#define vk_numpad9     (105)
#define vk_divide      (111)
#define vk_multiply    (106)
#define vk_subtract    (109)
#define vk_add         (107)
#define vk_decimal     (110)
#define vk_lshift      (160)
#define vk_lcontrol    (162)
#define vk_lalt        (164)
#define vk_rshift      (161)
#define vk_rcontrol    (163)
#define vk_ralt        (165)

extern Gamepad yoyo_gamepads[MAX_GAMEPADS];
extern int MAP_DPAD_AS_AXIS;

#define N_BUTTONS 30
#define N_KEYS 256

typedef ABI_ATTR void (*fct_add_t)(const char *name, routine_t func, int argc, char reg);
typedef ABI_ATTR int  (*create_ds_map_t)(int, ...);
typedef ABI_ATTR void (*create_async_event_with_ds_map_t)(int, int);

extern ABI_ATTR create_async_event_with_ds_map_t CreateAsynEventWithDSMap;
extern ABI_ATTR create_ds_map_t CreateDsMap;
extern ABI_ATTR fct_add_t Function_Add;
extern ABI_ATTR bool (*dsMapAddDouble)(int, const char*, double);
extern ABI_ATTR bool (*dsMapAddInt)(int, const char*, int);
extern ABI_ATTR bool (*dsMapAddString)(int, const char*, const char*);

extern ABI_ATTR float (*Audio_GetTrackPos)(int sound_id);
extern ABI_ATTR int (*Audio_WAVs)(uint8_t*, uint32_t, uint8_t*, int);
extern ABI_ATTR int32_t (*Graphics_DisplayHeight)();
extern ABI_ATTR int32_t (*Graphics_DisplayWidth)();
extern ABI_ATTR int32_t (*YYGetInt32)(RValue *val, int idx);
extern ABI_ATTR int64_t (*YYGetInt64)(RValue *val, int idx);
extern ABI_ATTR double (*YYGetReal)(RValue *val, int idx);
extern ABI_ATTR void (*YYCreateString)(RValue *, const char *);
extern ABI_ATTR routine_t F_YoYo_DrawTextureFlush;
extern ABI_ATTR uint32_t *(*ReadPNGFile)(void *a1, int a2, int *a3, int *a4, char a5);
extern ABI_ATTR void (*Audio_PrepareGroup)(void*);
extern ABI_ATTR void (*CThread__start_NS)(CThread *, void *, void *);
extern ABI_ATTR void (*CThread__start_PcNS)(CThread *, void *, void *, char *);
extern ABI_ATTR void (*FreePNGFile)();
extern ABI_ATTR void (*GamepadUpdate)();
extern ABI_ATTR void (*InitLLVM)(LLVMVars *);
extern ABI_ATTR void (*InvalidateTextureState)();
extern ABI_ATTR void (*MemoryManager__Free)(void *);
extern ABI_ATTR void (*Mutex__ctor)(void*, char*);
extern ABI_ATTR void (*Mutex__dtor)(void *);
extern ABI_ATTR void (*SetWorkingDirectory_ptr)();
extern ABI_ATTR char (*Variable_GetValue_Direct)(void* inst, int var_slot, int array_idx, RValue *val, char unk1, char unk2);
extern ABI_ATTR char (*Variable_SetValue_Direct)(void* inst, int var_slot, int array_idx, RValue *val);
extern ABI_ATTR char (*Variable_GetBuiltIn_Direct)(void* inst, int var_slot, int array_idx, RValue *val);
extern ABI_ATTR char (*Variable_SetBuiltIn_Direct)(void* inst, int var_slot, int array_idx, RValue *val);
extern ABI_ATTR void (*Code_Function_GET_the_function)(int numb,char **name,void **code,int *args, int *unk);
extern ABI_ATTR void (*_RefThing__dec)(void *ref);
extern ABI_ATTR int (*Variable_BuiltIn_Find)(const char *name);
extern ABI_ATTR int (*Code_Variable_Find_Slot_From_Name)(void *instance, const char *name);
extern ABI_ATTR int (*Variable_FindName)(const char *name);
extern ABI_ATTR long (*ExecuteIt)(void *self, void *other, void *code, RValue *ret);
extern ABI_ATTR long (*ExecuteIt_flags)(void *self, void *other, void *code, RValue *ret, int flags);

extern bionic_off_t *g_GameFileLength;
extern char **g_pWorkingDirectory;
extern char *g_fNoAudio;
extern char *g_fYYC;
extern fct_add_t fct_add;
extern int *g_TextureScale;
extern int g_fdGameFileBuff;
extern int32_t *g_MousePosX;
extern int32_t *g_MousePosY;
extern LLVMVars **g_pLLVMVars;
extern long long *g_GML_DeltaTime;
extern int *the_numb;
extern int *g_nInstanceVariables;
extern void **g_pGlobal;
extern int *g_TotalCodeBlocks;
extern struct CCode **g_pFirstCode;
extern int *g_ArgumentCount;
extern RValue **Argument;
extern FunctionEntry **the_functions;
extern uint32_t *g_IOFrameCount;
extern uint8_t *_IO_ButtonDown;
extern uint8_t *_IO_ButtonPressed;
extern uint8_t *_IO_ButtonReleased;
extern uint8_t *_IO_CurrentKey;
extern uint8_t *_IO_KeyDown;
extern uint8_t *_IO_KeyPressed;
extern uint8_t *_IO_KeyReleased;
extern uint8_t *_IO_LastKey;
extern uintptr_t *Current_Room;
extern uintptr_t *New_Room;
extern void **g_nYYCode;
extern void **g_pGameFileBuffer;
extern void **g_ppYYStackTrace;
extern int *Extension_Main_number;
extern ReentrantHook REHPrepareGame;

extern const char *gc_workdir;
extern int relaunch_flag;

extern void patch_libyoyo(struct so_module *mod);
extern void patch_input(struct so_module *mod);
extern void patch_gamepad(struct so_module *mod);
extern void patch_mouse(struct so_module *mod);
extern void patch_fmod(struct so_module *mod);
extern void patch_display_mouse_lock(struct so_module *mod);
extern void patch_gameframe(struct so_module *mod);
extern void patch_psn(struct so_module *mod);
extern void patch_steam(struct so_module *mod);
extern void patch_texture(struct so_module *mod);
extern void patch_lua(struct so_module* mod);
extern void run_lua(const char *code);
extern "C" void send_async_social(const char* type);

extern int update_inputs(struct SDL_Window *sdl_win);
void disable_depth();

extern void YYCreateStringHelper(struct RValue *rval, const char *str);
extern void YYThingDerefHelper(struct RValue *rval);
extern const char *YYGetCStrHelper(struct RValue *rval, int idx);
extern const char *YYCCodeName(struct CCode *code);
extern int UsesRefStrings();

extern "C" fct_add_t Original_Function_Add;
void Function_Add_Hook(const char* f_name, routine_t func, int argc, char reg);
