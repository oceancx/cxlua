#include "any.h"
#include "lua_bind.h"
#include "lua_net.h"
#include "net_thread_queue.h"

bool check_lua_error(lua_State* L, int res, const char* func = "");