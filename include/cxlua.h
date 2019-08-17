#include "any.h"
#include "lua_bind.h"
#include "lua_net.h"
#include "net_thread_queue.h"

bool check_lua_error(lua_State* L, int res, const char* func = "");

void init_default_cwd(const char* arg0);
std::string  get_default_cwd();
void luaopen_cxlua(lua_State* L);