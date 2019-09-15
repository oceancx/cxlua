#include "any.h"
#include "lua_bind.h"
#include "lua_net.h"
#include "net_thread_queue.h"


void init_default_cwd(const char* arg0);
std::string  get_default_cwd();
void luaopen_cxlua(lua_State* L);