#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <string>
#include <string_view>
#include <set>
#include <algorithm>
#include <vector>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
};
#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"

#define GMLUA_HOOKED_FLAG 0x10
#define GMLUA_INSTANCE_META "$MT_inst"
#define GMLUA_INSTANCE_VAR_REG "$var_reg"
#define GMLUA_HOOK_TABLE "$hook_tab"

static lua_State *gLua = NULL;

static void deref_rvalue(RValue *args, int i)
{
    if ((args[i].kind == VALUE_STRING) || (args[i].kind == VALUE_REF))
    {
        YYThingDerefHelper(&args[i]);
    }
}

static int get_yyvar_slot(void *inst, const char *var_name)
{
    int slot = Variable_BuiltIn_Find(var_name);
    if (slot < 0 && Code_Variable_Find_Slot_From_Name != NULL) /* gms1.x+ */
        slot = Code_Variable_Find_Slot_From_Name(inst, var_name);
    if (slot < 0 && Variable_FindName != NULL) /* old gm (ouya runner) */
        slot = Variable_FindName(var_name);
    
    return slot;
}

static int lua_pushrvalue(lua_State *L, RValue *val, int v)
{
    // Push rvalue into the lua stack
    switch (val[v].kind)
    {
	case VALUE_INT32:
	case VALUE_INT64:
    case VALUE_REF:
        lua_pushinteger(L, YYGetInt64(val, v));
        break;
	case VALUE_REAL:
	case VALUE_BOOL:
        lua_pushnumber(L, YYGetReal(val, v));
        break;
    case VALUE_STRING:
        lua_pushstring(L, YYGetCStrHelper(val, v));
        break;
    case VALUE_UNDEFINED:
    case VALUE_UNSET:
    case 85: // ??
        lua_pushnil(L);
        break;
    default:
        return luaL_error(L, "Return type %d can't be marshalled to Lua", val[v].kind);
    }

    return 1;
}

static int lua_torvalue(lua_State *L, int idx, RValue *val, int v)
{
    int type = lua_type(L, idx);

    switch (type)
    {
    case LUA_TNIL:
        val[v].kind = VALUE_UNDEFINED;
        val[v].rvalue.v64 = 0;
        break;
    case LUA_TNUMBER:
        if (lua_isinteger(L, idx))
        {
            val[v].kind = VALUE_INT64;
            val[v].rvalue.v64 = lua_tointeger(L, idx);
        }
        else
        {
            val[v].kind = VALUE_REAL;
            val[v].rvalue.val = lua_tonumber(L, idx);
        }
        break;
    case LUA_TBOOLEAN:
        val[v].kind = VALUE_REAL;
        val[v].rvalue.val = (lua_toboolean(L, idx) != 0) ? 1.0 : 0.0;
        break;
    case LUA_TSTRING:
    {
        YYCreateStringHelper(&val[v], luaL_checkstring(L, idx));
        break;
    }
    default:
        return luaL_error(L, "LUA Type %d can't be marshalled to GM", type);
    }

    return 1;
}

static void lua_pushcinstance(lua_State *L, void *inst) {
    void **pinst = (void **)lua_newuserdata(L, sizeof(void *));
    *pinst = inst;

    luaL_getmetatable(L, GMLUA_INSTANCE_META);
    lua_setmetatable(L, -2);
}

static int ffi_lua_to_rfunction(lua_State *L)
{
    int top = lua_gettop(L);
    int nargs = top - 1;
    routine_t code = (routine_t)lua_topointer(L, lua_upvalueindex(1));
    int _args = lua_tointeger(L, lua_upvalueindex(2));
    
    // Marshall types from the LUA Stack into GML args list
    RValue retval = {.kind = VALUE_UNSET};
    RValue *args = NULL;
    if (nargs > 0)
    {
        int argi = 0;
        args = (RValue *)alloca(sizeof(RValue) * nargs);
        for (int i = 2; i <= top; i++)
        {
            int res = lua_torvalue(L, i, args, argi++);
            if (res != 1)
                return res;
        }
    }

    code(&retval, NULL, NULL, nargs, args);
    
    // Free any strings that might've been allocated and now are no longer needed.
    for (int i = 0; i < nargs; i++)
    {
        deref_rvalue(args, i);
    }

    int r = lua_pushrvalue(L, &retval, 0);
    deref_rvalue(&retval, 0);
    return r;
}

struct CCode * find_ccode_entry(const char *name)
{
    struct CCode *code = *g_pFirstCode;
    for (int i = 0; i < *g_TotalCodeBlocks; i++)
    {
        if (!code)
            break;

        if (strcmp(code->m_name, name) == 0)
            return code;

        code = code->m_next;
    }

    return NULL;
}

static int register_ccode_hook(lua_State *L)
{
    const char *func = luaL_checkstring(L, -2);
    luaL_checktype(L, -1, LUA_TFUNCTION); 

    struct CCode *curCode = find_ccode_entry(func);

    if (!curCode)
        return luaL_error(L, "Unable to find function %s", func);

    curCode->m_kind |= GMLUA_HOOKED_FLAG;
    lua_getfield(L, LUA_REGISTRYINDEX, GMLUA_HOOK_TABLE);
    lua_pushlightuserdata(L, (void*)curCode);
    lua_pushvalue(L, -3);
    lua_settable(L, -3);
    lua_pop(L, 1);

    return 0;
}

static void register_utils_api(lua_State *L)
{
    /* --- GML Built-in metatable --- */
    lua_newtable(L);

    lua_pushstring(L, "register_hook"); /* method name */
    lua_pushcclosure(L, register_ccode_hook, 0);
    lua_settable(L, -3);

    lua_setglobal(L, "utils");

    /* hook table */
    lua_newtable(L);
    lua_setfield(L, LUA_REGISTRYINDEX, GMLUA_HOOK_TABLE);
}

static void register_gml_funcs(lua_State *L)
{
    std::set<std::string_view> registered = {};
    /* --- GML Built-in metatable --- */
    lua_newtable(L);

    for (int i = 0; i < *the_numb; i++) {
        char *name = NULL;
        void *code = NULL;
        int args = -1;
        int unk = -1;
        Code_Function_GET_the_function(i, &name, &code, &args, &unk);

        if (name)
        {
            if (registered.contains(std::string_view(name)))
                continue;

            /* gml[name] = (code, args, ffi_lua_to_rfunction) */
            lua_pushstring(L, name); /* function name (gml[name] = closure) */
            lua_pushlightuserdata(L, code); /* function code */
            lua_pushinteger(L, args); /* function arg count */
            lua_pushcclosure(L, ffi_lua_to_rfunction, 2);
            lua_settable(L, -3);
            registered.insert(std::string_view(name));
        }
    }

    lua_setglobal(L, "gml");
}

/*
 * Due to how Lua interns strings, we want to ensure we wrap variable lookups so we can cache the
 * results in a lua table, so that we can do a hashed lookup that leverages lua interned strings.
 */
static int get_yyvar_slot_cached(lua_State *L, int inst_idx, void *inst_udata, const char *key)
{
    // top = instance.$var_reg["key"]
    lua_getmetatable(L, inst_idx);
    lua_getfield(L, -1, GMLUA_INSTANCE_VAR_REG);
    int type = lua_getfield(L, -1, key);
    int slot;

    if (type == LUA_TNIL)
    {
        slot = get_yyvar_slot(inst_udata, key);
        
        if (slot < 0)
        {
            return luaL_error(L, "Variable '%s' does not exist.", key);
        }
        
        // instance.$var_reg["key"] = slot
        lua_pushinteger(L, slot);
        lua_setfield(L, -3, key);
    }
    else
    {
        // slot = instance.$var_reg["key"]
        slot = lua_tointeger(L, -1);
    }

    lua_pop(L, 3);
    return slot;
}

// wrapper for the  __index metamethod
static int cinstance_getter(lua_State *L) {
    int r = 1;
    RValue v = {.kind = VALUE_UNSET};
    void *inst = *(void **)luaL_checkudata(L, 1, GMLUA_INSTANCE_META);
    const char *key = luaL_checkstring(L, 2);

    int slot = get_yyvar_slot_cached(L, 1, inst, key);
    if (slot < 0)
        return slot;

    if (Variable_GetBuiltIn_Direct && slot < 100000)
    {
        if (Variable_GetBuiltIn_Direct(inst, slot, -0x80000000, &v) == 0)
        {
            r = luaL_error(L, "Failed to get variable '%s'", key);
            goto cleanup_cigetter;
        }
    }
    else if (Variable_GetValue_Direct(inst, slot, -0x80000000, &v, 0, 0) == 0)
    {
        r = luaL_error(L, "Failed to get variable '%s'", key);
        goto cleanup_cigetter;
    }

    lua_pushrvalue(L, &v, 0);

cleanup_cigetter:
    deref_rvalue(&v, 0);
    return r;
}

// wrapper for the __newindex metamethod
static int cinstance_setter(lua_State *L) {
    int r = 1;
    RValue v = {.kind = VALUE_UNSET};
    void *inst = *(void **)luaL_checkudata(L, 1, GMLUA_INSTANCE_META);
    const char *key = luaL_checkstring(L, 2);

    r = lua_torvalue(L, 3, &v, 0);
    if (r != 1)
        return r;

    int slot = get_yyvar_slot_cached(L, 1, inst, key);
    if (slot < 0)
        return slot;

    if (Variable_SetBuiltIn_Direct && slot < 100000)
    {
        if (Variable_SetBuiltIn_Direct(inst, slot, -0x80000000, &v) == 0)
        {
            r = luaL_error(L, "Failed to set variable '%s'", key);
            goto cleanup_cisetter;
        }
    }
    else if (Variable_SetValue_Direct(inst, slot, -0x80000000, &v) == 0)
    {
        r = luaL_error(L, "Failed to set variable '%s'", key);
        goto cleanup_cisetter;
    }

cleanup_cisetter:
    deref_rvalue(&v, 0);
    return r;
}

/*
 * Create the Instance Accessor metatable used to
 * do interop between GM instances and LUA, also creates
 * an instance for the global variables.
 */
static void register_cinstance(lua_State *L)
{
    luaL_newmetatable(L, GMLUA_INSTANCE_META);
    
    lua_pushcfunction(L, cinstance_getter);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, cinstance_setter);
    lua_setfield(L, -2, "__newindex");

    lua_newtable(L);
    lua_setfield(L, -2, GMLUA_INSTANCE_VAR_REG);

    lua_pushcinstance(L, *g_pGlobal);
    lua_setglobal(L, "global");
    lua_pop(L, 1);
}

static int lua_panic(lua_State *L) {
    fprintf(stderr, "Lua PANIC: %s\n", lua_tostring(L, -1));
    return 0;
}


void initialize_lua()
{
    static int init = 0;
    if (init)
        return;
    init = 1;

    gLua = luaL_newstate();
    luaL_openlibs(gLua);

    register_gml_funcs(gLua);
    register_cinstance(gLua);
    register_utils_api(gLua);

    lua_atpanic(gLua, lua_panic);
}

void run_lua(const char *code)
{
    lua_atpanic(gLua, lua_panic);
    if (luaL_dostring(gLua, code))
    {
        printf("failed: %s.\n", lua_tostring(gLua, -1));
        lua_pop(gLua, 1);
        //exit(-1);
    }
}

ABI_ATTR static void (*StartRoom)(int arg1, char arg2) = NULL;
int32_t StartRoom_prologue[4] = {};
ABI_ATTR static void do_lua_init_sequence(int arg1, char arg2)
{
    initialize_lua();

    // Restore the StartRoom function and jump to it.
    memcpy((void*)StartRoom, (void*)StartRoom_prologue, sizeof(StartRoom_prologue));
    __builtin___clear_cache((void *)StartRoom, (void *)StartRoom+16);

    StartRoom(arg1, arg2);
}

ABI_ATTR long Code_Execute_Hook(void *self, void *other, struct CCode *code, RValue *args, int argc)
{
    return ExecuteIt(self, other, code, args);
}

ABI_ATTR long Code_Execute_Hook_argc(void *self, void *other, struct CCode *code, RValue *args, int argc)
{

    if (gLua && ((code->m_kind & GMLUA_HOOKED_FLAG) == GMLUA_HOOKED_FLAG))
    {
        lua_getfield(gLua, LUA_REGISTRYINDEX, GMLUA_HOOK_TABLE);
        lua_pushlightuserdata(gLua, (void*)code);
        lua_gettable(gLua, -2);

        lua_pushcinstance(gLua, self);
        lua_pushcinstance(gLua, other);

        for (int i = 0; i < argc; i++)
        {
            lua_pushrvalue(gLua, args, i);
        }

        if (lua_pcall(gLua, argc+2, 0, 0))
        {
            printf("failed: %s.\n", lua_tostring(gLua, -1));
            lua_pop(gLua, 1);
            exit(-1);
        }

        // Todo:: return values, etc.
        lua_pop(gLua, 1);
        return 0;
    }

    return ExecuteIt_argc(self, other, code, args, argc);
}

void patch_lua(struct so_module* mod)
{
    /*
     * We're going to hook StartRoom since it's the first place where game code runs,
     * and we already have all we need to know about the game initialized by the runner.
     * After we're done initializing the LUA interfaces, we will unhook the code and jump to it.
     * 
     * You can think of this as deffered initialization.
     */
    FIND_SYMBOL(mod, StartRoom, "_Z9StartRoomib");
    memcpy((void*)StartRoom_prologue, (void*)StartRoom, sizeof(StartRoom_prologue));
    hook_symbol(mod, "_Z9StartRoomib", (uintptr_t)&do_lua_init_sequence, 1);
    hook_symbol(mod, "_Z12Code_ExecuteP9CInstanceS0_P5CCodeP6RValue", (uintptr_t)&Code_Execute_Hook, 1);
    hook_symbol(mod, "_Z12Code_ExecuteP9CInstanceS0_P5CCodeP6RValuei", (uintptr_t)&Code_Execute_Hook_argc, 1);
}