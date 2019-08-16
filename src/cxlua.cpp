#include "cxlua.h"

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