#pragma once
#include "lua_bind.h"
#include "cxezio/buffer.h"
//void lua_push_tcp_connection(lua_State*L,const ezio::TCPConnectionPtr& conn);
void lua_push_cxezio_buffer(lua_State*L, cxezio::Buffer& buf);
cxezio::Buffer* lua_check_cxezio_buffer(lua_State* L, int index);


void luaopen_netlib(lua_State* L);


