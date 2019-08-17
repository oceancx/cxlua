#include "cxlua.h"

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

void luaopen_cxlua(lua_State* L)
{
	script_system_register_luac_function_with_name(L, "get_default_cwd", lua_get_default_cwd);
}
