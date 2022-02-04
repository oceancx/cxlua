#include "cxlua.h"
#include "tsv.h"

std::string DEFAULT_CWD = "";
bool check_lua_error(lua_State* L, int res, const char* func)
{
	if (res != LUA_OK) {
		luaL_traceback(L, L, lua_tostring(L, -1), 0);
		const char* errmsg = lua_tostring(L, -1);
		printf("%s\npcall error:\t[func]%s\n", errmsg, func);
		return false;
	}
	return true;
}


std::map<std::string, std::string> g_CmdArgs;
void handle_command_args(int argc,char *argv[])
{
	g_CmdArgs["argv0"] = argv[0];
	for (int i = 1; i < argc; i++)
	{
		std::string argi = argv[i];
		printf("%s\n", argi.c_str());

		if (argi.substr(0, 2) == "--") {
			argi = argi.substr(2, argi.size());
			auto kv = utils::split(argi, '=');
			g_CmdArgs[kv[0]] = kv[1];
		}
		else if (argi.substr(0, 1) == "-") {
			argi = argi.substr(1, argi.size());
			g_CmdArgs[argi] = "";
		}
	}
}

bool command_arg_check(const char* key){
	return g_CmdArgs.find(key) != g_CmdArgs.end();
}

const char* command_arg_get(const char* key)
{
	if (g_CmdArgs.find(key) == g_CmdArgs.end()) {
		return nullptr;
	}else{
		return g_CmdArgs[key].c_str();
	}
}

const char* command_arg_opt_str(const char* key, const char* def){
	auto arg = command_arg_get(key);
	if (arg) {
		return arg;
	}
	else {
		return def;
	}
}

int command_arg_opt_int(const char* key, int def)
{
	auto arg = command_arg_get(key);
	if (arg) {
		int intarg = std::stoi(arg);
		return intarg;
	}
	else {
		return def;
	}
}


void init_default_cwd(const char* arg0)
{
	std::string PATH_SEP = "";
	std::string param(arg0);
	if (param.find_last_of("\\") != std::string::npos) {
		PATH_SEP = "\\";
	}
	else if (param.find_last_of("/") != std::string::npos)
	{
		PATH_SEP = "/";
	}
	DEFAULT_CWD = param.substr(0, param.find_last_of(PATH_SEP))  + PATH_SEP;
}
std::string get_default_cwd()
{
	return  DEFAULT_CWD;
}
int lua_get_default_cwd(lua_State*L )
{
	lua_pushstring(L,get_default_cwd().c_str());
	return 1;
}

static int s_EnumCounter = 0;
int enum_reset(){
	s_EnumCounter = 0;
	return s_EnumCounter;
}

int enum_next() {
	return ++s_EnumCounter;
}


#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State * L);

typedef struct _LuaProxy
{
	lua_State* (*lua_newstate)(lua_Alloc f, void* ud);
	void(*lua_close)(lua_State* L);
	lua_State* (*lua_newthread)(lua_State* L);
	lua_CFunction(*lua_atpanic)(lua_State* L, lua_CFunction panicf);
	const lua_Number* (*lua_version)(lua_State* L);
	int(*lua_absindex)(lua_State* L, int idx);
	int(*lua_gettop)(lua_State* L);
	void(*lua_settop)(lua_State* L, int idx);
	void(*lua_pushvalue)(lua_State* L, int idx);
	void(*lua_rotate)(lua_State* L, int idx, int n);
	void(*lua_copy)(lua_State* L, int fromidx, int toidx);
	int(*lua_checkstack)(lua_State* L, int n);
	void(*lua_xmove)(lua_State* from, lua_State* to, int n);
	int(*lua_isnumber)(lua_State* L, int idx);
	int(*lua_isstring)(lua_State* L, int idx);
	int(*lua_iscfunction)(lua_State* L, int idx);
	int(*lua_isinteger)(lua_State* L, int idx);
	int(*lua_isuserdata)(lua_State* L, int idx);
	int(*lua_type)(lua_State* L, int idx);
	const char* (*lua_typename)(lua_State* L, int tp);
	lua_Number(*lua_tonumberx)(lua_State* L, int idx, int* isnum);
	lua_Integer(*lua_tointegerx)(lua_State* L, int idx, int* isnum);
	int(*lua_toboolean)(lua_State* L, int idx);
	const char* (*lua_tolstring)(lua_State* L, int idx, size_t* len);
	size_t(*lua_rawlen)(lua_State* L, int idx);
	lua_CFunction(*lua_tocfunction)(lua_State* L, int idx);
	void* (*lua_touserdata)(lua_State* L, int idx);
	lua_State* (*lua_tothread)(lua_State* L, int idx);
	const void* (*lua_topointer)(lua_State* L, int idx);
	void(*lua_arith)(lua_State* L, int op);
	int(*lua_rawequal)(lua_State* L, int idx1, int idx2);
	int(*lua_compare)(lua_State* L, int idx1, int idx2, int op);
	void(*lua_pushnil)(lua_State* L);
	void(*lua_pushnumber)(lua_State* L, lua_Number n);
	void(*lua_pushinteger)(lua_State* L, lua_Integer n);
	const char* (*lua_pushlstring)(lua_State* L, const char* s, size_t len);
	const char* (*lua_pushstring)(lua_State* L, const char* s);
	const char* (*lua_pushvfstring)(lua_State* L, const char* fmt, va_list argp);
	const char* (*lua_pushfstring)(lua_State* L, const char* fmt, ...);
	void(*lua_pushcclosure)(lua_State* L, lua_CFunction fn, int n);
	void(*lua_pushboolean)(lua_State* L, int b);
	void(*lua_pushlightuserdata)(lua_State* L, void* p);
	int(*lua_pushthread)(lua_State* L);
	int(*lua_getglobal)(lua_State* L, const char* name);
	int(*lua_gettable)(lua_State* L, int idx);
	int(*lua_getfield)(lua_State* L, int idx, const char* k);
	int(*lua_geti)(lua_State* L, int idx, lua_Integer n);
	int(*lua_rawget)(lua_State* L, int idx);
	int(*lua_rawgeti)(lua_State* L, int idx, lua_Integer n);
	int(*lua_rawgetp)(lua_State* L, int idx, const void* p);
	void(*lua_createtable)(lua_State* L, int narr, int nrec);
	void* (*lua_newuserdata)(lua_State* L, size_t sz);
	int(*lua_getmetatable)(lua_State* L, int objindex);
	int(*lua_getuservalue)(lua_State* L, int idx);
	void(*lua_setglobal)(lua_State* L, const char* name);
	void(*lua_settable)(lua_State* L, int idx);
	void(*lua_setfield)(lua_State* L, int idx, const char* k);
	void(*lua_seti)(lua_State* L, int idx, lua_Integer n);
	void(*lua_rawset)(lua_State* L, int idx);
	void(*lua_rawseti)(lua_State* L, int idx, lua_Integer n);
	void(*lua_rawsetp)(lua_State* L, int idx, const void* p);
	int(*lua_setmetatable)(lua_State* L, int objindex);
	void(*lua_setuservalue)(lua_State* L, int idx);
	void(*lua_callk)(lua_State* L, int nargs, int nresults, lua_KContext ctx, lua_KFunction k);
	int(*lua_pcallk)(lua_State* L, int nargs, int nresults, int errfunc, lua_KContext ctx, lua_KFunction k);
	int(*lua_load)(lua_State* L, lua_Reader reader, void* data, const char* chunkname, const char* mode);
	int(*lua_dump)(lua_State* L, lua_Writer writer, void* data, int strip);
	int(*lua_yieldk)(lua_State* L, int nresults, lua_KContext ctx, lua_KFunction k);
	int(*lua_resume)(lua_State* L, lua_State* from, int narg);
	int(*lua_status)(lua_State* L);
	int(*lua_isyieldable)(lua_State* L);
	int(*lua_gc)(lua_State* L, int what, int data);
	int(*lua_error)(lua_State* L);
	int(*lua_next)(lua_State* L, int idx);
	void(*lua_concat)(lua_State* L, int n);
	void(*lua_len)(lua_State* L, int idx);
	size_t(*lua_stringtonumber)(lua_State* L, const char* s);
	lua_Alloc(*lua_getallocf)(lua_State* L, void** ud);
	void(*lua_setallocf)(lua_State* L, lua_Alloc f, void* ud);
	int(*lua_getstack)(lua_State* L, int level, lua_Debug* ar);
	int(*lua_getinfo)(lua_State* L, const char* what, lua_Debug* ar);
	const char* (*lua_getlocal)(lua_State* L, const lua_Debug* ar, int n);
	const char* (*lua_setlocal)(lua_State* L, const lua_Debug* ar, int n);
	const char* (*lua_getupvalue)(lua_State* L, int funcindex, int n);
	const char* (*lua_setupvalue)(lua_State* L, int funcindex, int n);
	void* (*lua_upvalueid)(lua_State* L, int fidx, int n);
	void(*lua_upvaluejoin)(lua_State* L, int fidx1, int n1, int fidx2, int n2);
	void(*lua_sethook)(lua_State* L, lua_Hook func, int mask, int count);
	lua_Hook(*lua_gethook)(lua_State* L);
	int(*lua_gethookmask)(lua_State* L);
	int(*lua_gethookcount)(lua_State* L);
	int(*luaopen_base)(lua_State* L);
	int(*luaopen_coroutine)(lua_State* L);
	int(*luaopen_table)(lua_State* L);
	int(*luaopen_io)(lua_State* L);
	int(*luaopen_os)(lua_State* L);
	int(*luaopen_string)(lua_State* L);
	int(*luaopen_utf8)(lua_State* L);
	int(*luaopen_bit32)(lua_State* L);
	int(*luaopen_math)(lua_State* L);
	int(*luaopen_debug)(lua_State* L);
	int(*luaopen_package)(lua_State* L);
	void(*luaL_checkversion_)(lua_State* L, lua_Number ver, size_t sz);
	int(*luaL_getmetafield)(lua_State* L, int obj, const char* e);
	int(*luaL_callmeta)(lua_State* L, int obj, const char* e);
	const char* (*luaL_tolstring)(lua_State* L, int idx, size_t* len);
	int(*luaL_argerror)(lua_State* L, int arg, const char* extramsg);
	const char* (*luaL_checklstring)(lua_State* L, int arg, size_t* l);
	const char* (*luaL_optlstring)(lua_State* L, int arg, const char* def, size_t* l);
	lua_Number(*luaL_checknumber)(lua_State* L, int arg);
	lua_Number(*luaL_optnumber)(lua_State* L, int arg, lua_Number def);
	lua_Integer(*luaL_checkinteger)(lua_State* L, int arg);
	lua_Integer(*luaL_optinteger)(lua_State* L, int arg, lua_Integer def);
	void(*luaL_checkstack)(lua_State* L, int sz, const char* msg);
	void(*luaL_checktype)(lua_State* L, int arg, int t);
	void(*luaL_checkany)(lua_State* L, int arg);
	int(*luaL_newmetatable)(lua_State* L, const char* tname);
	void(*luaL_setmetatable)(lua_State* L, const char* tname);
	void* (*luaL_testudata)(lua_State* L, int ud, const char* tname);
	void* (*luaL_checkudata)(lua_State* L, int ud, const char* tname);
	void(*luaL_where)(lua_State* L, int lvl);
	int(*luaL_error)(lua_State* L, const char* fmt, ...);
	int(*luaL_checkoption)(lua_State* L, int arg, const char* def, const char* const lst[]);
	int(*luaL_fileresult)(lua_State* L, int stat, const char* fname);
	int(*luaL_execresult)(lua_State* L, int stat);
	int(*luaL_ref)(lua_State* L, int t);
	void(*luaL_unref)(lua_State* L, int t, int ref);
	int(*luaL_loadfilex)(lua_State* L, const char* filename, const char* mode);
	int(*luaL_loadbufferx)(lua_State* L, const char* buff, size_t sz, const char* name, const char* mode);
	int(*luaL_loadstring)(lua_State* L, const char* s);
	lua_State* (*luaL_newstate)(void);
	lua_Integer(*luaL_len)(lua_State* L, int idx);
	const char* (*luaL_gsub)(lua_State* L, const char* s, const char* p, const char* r);
	void(*luaL_setfuncs)(lua_State* L, const luaL_Reg* l, int nup);
	int(*luaL_getsubtable)(lua_State* L, int idx, const char* fname);
	void(*luaL_traceback)(lua_State* L, lua_State* L1, const char* msg, int level);
	void(*luaL_requiref)(lua_State* L, const char* modname, lua_CFunction openf, int glb);
	void(*luaL_buffinit)(lua_State* L, luaL_Buffer* B);
	char* (*luaL_prepbuffsize)(luaL_Buffer* B, size_t sz);
	void(*luaL_addlstring)(luaL_Buffer* B, const char* s, size_t l);
	void(*luaL_addstring)(luaL_Buffer* B, const char* s);
	void(*luaL_addvalue)(luaL_Buffer* B);
	void(*luaL_pushresult)(luaL_Buffer* B);
	void(*luaL_pushresultsize)(luaL_Buffer* B, size_t sz);
	char* (*luaL_buffinitsize)(lua_State* L, luaL_Buffer* B, size_t sz);
	void(*luaL_pushmodule)(lua_State* L, const char* modname, int sizehint);
	void(*luaL_openlib)(lua_State* L, const char* libname, const luaL_Reg* l, int nup);
	void(*luaL_openlibs)(lua_State* L);
	int(*luaopen_cjson)(lua_State* L);
}LuaProxy;
extern "C"  int luaopen_cjson(lua_State * L);
static LuaProxy* __lua_proxy_impl__()
{
	static LuaProxy __impl__{
		lua_newstate,
		lua_close,
		lua_newthread,
		lua_atpanic,
		lua_version,
		lua_absindex,
		lua_gettop,
		lua_settop,
		lua_pushvalue,
		lua_rotate,
		lua_copy,
		lua_checkstack,
		lua_xmove,
		lua_isnumber,
		lua_isstring,
		lua_iscfunction,
		lua_isinteger,
		lua_isuserdata,
		lua_type,
		lua_typename,
		lua_tonumberx,
		lua_tointegerx,
		lua_toboolean,
		lua_tolstring,
		lua_rawlen,
		lua_tocfunction,
		lua_touserdata,
		lua_tothread,
		lua_topointer,
		lua_arith,
		lua_rawequal,
		lua_compare,
		lua_pushnil,
		lua_pushnumber,
		lua_pushinteger,
		lua_pushlstring,
		lua_pushstring,
		lua_pushvfstring,
		lua_pushfstring,
		lua_pushcclosure,
		lua_pushboolean,
		lua_pushlightuserdata,
		lua_pushthread,
		lua_getglobal,
		lua_gettable,
		lua_getfield,
		lua_geti,
		lua_rawget,
		lua_rawgeti,
		lua_rawgetp,
		lua_createtable,
		lua_newuserdata,
		lua_getmetatable,
		lua_getuservalue,
		lua_setglobal,
		lua_settable,
		lua_setfield,
		lua_seti,
		lua_rawset,
		lua_rawseti,
		lua_rawsetp,
		lua_setmetatable,
		lua_setuservalue,
		lua_callk,
		lua_pcallk,
		lua_load,
		lua_dump,
		lua_yieldk,
		lua_resume,
		lua_status,
		lua_isyieldable,
		lua_gc,
		lua_error,
		lua_next,
		lua_concat,
		lua_len,
		lua_stringtonumber,
		lua_getallocf,
		lua_setallocf,
		lua_getstack,
		lua_getinfo,
		lua_getlocal,
		lua_setlocal,
		lua_getupvalue,
		lua_setupvalue,
		lua_upvalueid,
		lua_upvaluejoin,
		lua_sethook,
		lua_gethook,
		lua_gethookmask,
		lua_gethookcount,
		luaopen_base,
		luaopen_coroutine,
		luaopen_table,
		luaopen_io,
		luaopen_os,
		luaopen_string,
		luaopen_utf8,
		luaopen_bit32,
		luaopen_math,
		luaopen_debug,
		luaopen_package,
		luaL_checkversion_,
		luaL_getmetafield,
		luaL_callmeta,
		luaL_tolstring,
		luaL_argerror,
		luaL_checklstring,
		luaL_optlstring,
		luaL_checknumber,
		luaL_optnumber,
		luaL_checkinteger,
		luaL_optinteger,
		luaL_checkstack,
		luaL_checktype,
		luaL_checkany,
		luaL_newmetatable,
		luaL_setmetatable,
		luaL_testudata,
		luaL_checkudata,
		luaL_where,
		luaL_error,
		luaL_checkoption,
		luaL_fileresult,
		luaL_execresult,
		luaL_ref,
		luaL_unref,
		luaL_loadfilex,
		luaL_loadbufferx,
		luaL_loadstring,
		luaL_newstate,
		luaL_len,
		luaL_gsub,
		luaL_setfuncs,
		luaL_getsubtable,
		luaL_traceback,
		luaL_requiref,
		luaL_buffinit,
		luaL_prepbuffsize,
		luaL_addlstring,
		luaL_addstring,
		luaL_addvalue,
		luaL_pushresult,
		luaL_pushresultsize,
		luaL_buffinitsize,
		luaL_pushmodule,
		luaL_openlib,
		luaL_openlibs,
		luaopen_cjson
	};
	return &__impl__;
}

#ifdef __cplusplus
extern "C" {
#endif	 
	int _luaopen_luadbg(LuaProxy* (*proxy)(), lua_State* L);
#ifdef __cplusplus
}
#endif

#define luaopen_luadbg(L) _luaopen_luadbg(&__lua_proxy_impl__,L)


void luaopen_cxlua(lua_State* L)
{
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_tsv(L);
	luaopen_net_thread_queue(L);
	luaopen_netlib(L);
	
	script_system_register_function(L, enum_reset);
	script_system_register_function(L, enum_next);
	
	script_system_register_function(L, command_arg_check);
	script_system_register_function(L, command_arg_get);
	script_system_register_function(L, command_arg_opt_str);
	script_system_register_function(L, command_arg_opt_int);

	script_system_register_luac_function_with_name(L, "get_default_cwd", lua_get_default_cwd);


	typedef 	int (*LuaDbgFunc)(LuaProxy* (*proxy)(), lua_State* L);
	HINSTANCE hDll = nullptr;//句柄
	hDll = ::LoadLibrary("luadbg_shared.dll");//动态加载DLL模块句柄
	if (hDll)
	{
		LuaDbgFunc instance = (LuaDbgFunc)GetProcAddress(hDll, "_luaopen_luadbg");//得到所加载DLL模块中函数的地址
		instance(&__lua_proxy_impl__, L);
		//FreeLibrary(hDll);//释放已经加载的DLL模块
	}

}
