#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <string>
#include <string_view>
#include <set>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
};
#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"

static lua_State *gLua = NULL;

static void cleanup_rvalue(RValue *args, int i)
{
    if ((args[i].kind == VALUE_STRING) || (args[i].kind == VALUE_REF))
    {
        YYThingDerefHelper(&args[i]);
    }
}

static int get_variable_slot(void *inst, const char *var_name)
{
    int slot = Variable_BuiltIn_Find(var_name);
    if (slot < 0 && Code_Variable_Find_Slot_From_Name != NULL) /* gms1.x+ */
        slot = Code_Variable_Find_Slot_From_Name(inst, var_name);
    if (slot < 0 && Variable_FindName != NULL) /* old gm (ouya runner) */
        slot = Variable_FindName(var_name);
    
    return slot;
}

static int lua_push_rvalue(lua_State *L, RValue *val, int v)
{
    // Push rvalue into the lua stack
    switch (val[v].kind)
    {
	case VALUE_INT32:
	case VALUE_INT64:
    {
        int64_t r = YYGetInt64(val, v);
        lua_pushinteger(L, r);
        break;
    }
	case VALUE_REAL:
	case VALUE_BOOL:
    {
        double r = YYGetReal(val, v);
        lua_pushnumber(L, r);
        break;
    }
    case VALUE_STRING:
    {
        if (UsesRefStrings())
            lua_pushstring(L, (const char *)val[v].rvalue.str->m_thing);
        else
            lua_pushstring(L, (const char *)val[v].rvalue.str);
        break;
    }
    case VALUE_UNDEFINED:
    case VALUE_UNSET:
        lua_pushnil(L);
        break;
    default:
        return luaL_error(L, "Return type %d can't be marshalled to Lua", val[v].kind);
    }

    return 1;
}

static int lua_to_rvalue(lua_State *L, int idx, RValue *val, int v)
{
    int type = lua_type(L, idx);

    switch (type)
    {
    case LUA_TNIL:
        val[v].kind = VALUE_INT64;
        val[v].rvalue.val = lua_tointeger(L, idx);
        break;
    case LUA_TNUMBER:
        if (lua_isinteger(L, idx))
        {
            val[v].kind = VALUE_INT64;
            val[v].rvalue.val = lua_tointeger(L, idx);
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

static int GMLua_DispatchFunc(lua_State *L)
{
    int method_id = lua_tointeger(L, lua_upvalueindex(1));
    int top = lua_gettop(L);
    int nargs = top - 1;
    char *name = NULL;
    routine_t code = NULL;
    int _args = -1;
    int _unk = -1;

    Code_Function_GET_the_function(method_id, &name, (void**)&code, &_args, &_unk);
    if (!code)
        return luaL_error(L, "Built-In Function %d has no code", method_id);
    
    // Marshall types from the LUA Stack into GML args list
    RValue retval = {.kind = VALUE_UNSET};
    RValue *args = NULL;
    if (nargs > 0)
    {
        int argi = 0;
        args = (RValue *)alloca(sizeof(RValue) * nargs);
        for (int i = 2; i <= top; i++)
        {
            int res = lua_to_rvalue(L, i, args, argi++);
            if (res != 1)
                return res;
        }
    }

    code(&retval, NULL, NULL, nargs, args);
    
    // Free any strings that might've been allocated and now are no longer needed.
    for (int i = 0; i < nargs; i++)
    {
        cleanup_rvalue(args, i);
    }

    int r = lua_push_rvalue(L, &retval, 0);
    cleanup_rvalue(&retval, 0);
    return r;
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

            lua_pushstring(L, name); /* method name */
            lua_pushinteger(L, i); /* method id */
            lua_pushcclosure(L, GMLua_DispatchFunc, 1);
            lua_settable(L, -3);
            registered.insert(std::string_view(name));
        }
    }

    lua_setglobal(L, "gml");
}

static int CInstance_Getter(lua_State *L) {
    int r = 1;
    RValue v = {.kind = VALUE_UNSET};
    void *inst = *(void **)luaL_checkudata(L, 1, "CInstanceMeta");
    const char *key = luaL_checkstring(L, 2);

    int slot = get_variable_slot(inst, key);

    if (slot < 0)
    {
        r = luaL_error(L, "Variable '%s' does not exist.", key);
        goto cleanup_cisetter;
    }

    if (Variable_GetBuiltIn_Direct && slot < 100000)
    {
        if (Variable_GetBuiltIn_Direct(inst, slot, -0x80000000, &v) == 0)
        {
            r = luaL_error(L, "Failed to get variable '%s'", key);
            goto cleanup_cisetter;
        }
    }
    else if (Variable_GetValue_Direct(inst, slot, -0x80000000, &v, 0, 0) == 0)
    {
        r = luaL_error(L, "Failed to get variable '%s'", key);
        goto cleanup_cisetter;
    }

    lua_push_rvalue(L, &v, 0);

cleanup_cisetter:
    cleanup_rvalue(&v, 0);
    return r;
}

static int CInstance_Setter(lua_State *L) {
    int r = 1;
    RValue v = {.kind = VALUE_UNSET};
    void *inst = *(void **)luaL_checkudata(L, 1, "CInstanceMeta");
    const char *key = luaL_checkstring(L, 2);
    lua_to_rvalue(L, 3, &v, 0);

    int slot = get_variable_slot(inst, key);

    if (slot < 0)
    {
        r = luaL_error(L, "Variable '%s' does not exist.", key);
        goto cleanup_cisetter;
    }

    if (Variable_SetBuiltIn_Direct && slot < 100000)
    {
        if (Variable_SetBuiltIn_Direct(inst, slot, 0, &v) == 0)
        {
            r = luaL_error(L, "Failed to set variable '%s'", key);
            goto cleanup_cisetter;
        }
    }
    else if (Variable_SetValue_Direct(inst, slot, 0, &v) == 0)
    {
        r = luaL_error(L, "Failed to set variable '%s'", key);
        goto cleanup_cisetter;
    }

cleanup_cisetter:
    cleanup_rvalue(&v, 0);
    return r;
}

static void wrap_cinstance(lua_State *L, void *inst) {
    void **pinst = (void **)lua_newuserdata(L, sizeof(void *));
    *pinst = inst;

    luaL_getmetatable(L, "CInstanceMeta");
    lua_setmetatable(L, -2);
}

static void register_cinstancemeta(lua_State *L)
{
    luaL_newmetatable(L, "CInstanceMeta");

    lua_pushcfunction(L, CInstance_Getter);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, CInstance_Setter);
    lua_setfield(L, -2, "__newindex");

    wrap_cinstance(L, *g_pGlobal);
    lua_setglobal(L, "global");
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
    register_cinstancemeta(gLua);

    lua_atpanic(gLua, lua_panic);
    warning("-- LUA Initialization performed.\n");
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
    StartRoom(arg1, arg2);
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
}