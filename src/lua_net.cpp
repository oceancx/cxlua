#include "lua_net.h"
#include "ezio/tcp_connection.h"
#include <iostream>



using namespace ezio;

static const char* skey_mt_tcp_connection = "key_mt_tcp_connection";
static const char* skey_mt_buffer = "key_mt_buffer";


using LuaEzioBuffer = Buffer* ;

void lua_push_tcp_connection(lua_State*L, const TCPConnectionPtr& conn)
{
	TCPConnection**  ptr = (TCPConnection**)lua_newuserdata(L, sizeof(TCPConnection*));
	*ptr = conn.get();
	luaL_setmetatable(L, skey_mt_tcp_connection);
}

TCPConnection* lua_check_tcpconnection(lua_State* L,int index)
{
	TCPConnection**  ptr = (TCPConnection**)lua_touserdata(L, index);
	return *ptr;
}


int tcp_connection_to_host_port(lua_State* L)
{
	TCPConnection* conn = lua_check_tcpconnection(L, 1);
	lua_pushstring(L, conn->peer_addr().ToHostPort().c_str());
	return 1;
}

int tcp_connection_connected(lua_State* L)
{
	TCPConnection* conn = lua_check_tcpconnection(L, 1);
	lua_pushboolean(L, conn->connected());
	return 1;
}
//
//int tcp_connection_cleanup(lua_State* L)
//{
////	TCPConnectionPtr* conn = lua_check_tcpconnection(L, 1);
//	
//	//printf("TCPConnection cleanup %d\n",(int)L);
//	return 0;
//}

int tcp_connection_send(lua_State* L)
{
	TCPConnection* conn = lua_check_tcpconnection(L, 1);
	size_t len = 0;
	const char* data = lua_tolstring(L, 2, &len);
	conn->Send(kbase::StringView(data, len));
	return 0;
}

luaL_Reg mt_tcp_connection_reg[] = {
	//{"__gc",tcp_connection_cleanup},
	{ "tohostport",tcp_connection_to_host_port},
	{ "connected",tcp_connection_connected},
	{ "send",tcp_connection_send },
	{ NULL, NULL }
};



Buffer* lua_check_buffer(lua_State*L, int index)
{
	LuaEzioBuffer* buffer = (LuaEzioBuffer*)lua_touserdata(L, index);
	return *buffer;
}

int buffer_readablesize(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	lua_pushinteger(L, buffer->readable_size());
	return 1;
}

int buffer_preview(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	luaL_Buffer lbuf;
	luaL_buffinitsize(L, &lbuf, len);
	luaL_addlstring(&lbuf, buffer->Peek(), len);
	luaL_pushresult(&lbuf);
	return 1;
}

int buffer_consume(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	buffer->Consume(len);
	return 0;
}


int buffer_prependint(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int n = (int)lua_tointeger(L, 2);
	buffer->Prepend(n);
	
	return 0;
}

int buffer_prependstring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	size_t len = 0;
	const char* data = lua_tolstring(L, 2, &len);
	buffer->Prepend(data, len);
	return 0;
}


int buffer_writestring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	size_t len = 0;
	const char* data = lua_tolstring(L, 2, &len);
	buffer->Write(data, len);
	return 0;
}

int buffer_writeint(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int n = (int)lua_tointeger(L, 2);
	buffer->Write(n);
	return 0;
}

int buffer_writeint64(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	uint64_t n = (uint64_t)lua_tointeger(L, 2);
	buffer->Write(n);
	return 0;
}


int buffer_writefloat(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	float num = (float)lua_tonumber(L, 2);
	buffer->Write(num);
	return 0;
}


int buffer_readstring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	lua_pushstring(L, (buffer->ReadAsString(len)).c_str());
	return 1;
}

int buffer_readallstring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	std::string str(buffer->ReadAllAsString());
	lua_pushstring(L, str.c_str());
	return 1;
}


int buffer_peekstring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	lua_pushlstring(L, buffer->Peek(), len);
	return 1;
}

int buffer_readfloat(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	float v = buffer->ReadAsFloat();
	lua_pushnumber(L, v);
	return 1;
}

int buffer_peekfloat(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	float v = buffer->PeekAsFloat();
	lua_pushnumber(L, v);
	return 1;
}


int buffer_readint(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int v = buffer->ReadAsInt32();
	lua_pushinteger(L, v);
	return 1;
}

int buffer_peekint(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int v = buffer->PeekAsInt32();
	lua_pushinteger(L, v);
	return 1;
}


int buffer_gc(lua_State* L)
{
	LuaEzioBuffer* buf = (LuaEzioBuffer*)lua_touserdata(L, 1);
	free(buf);
	buf = nullptr;
	printf("TCPConnection :gc\n");
	return 0;
}

luaL_Reg mt_buffer_reg[] = {
	{ "readable_size",buffer_readablesize },
	{ "Preview",buffer_preview },
	{ "Consume",buffer_consume },
	{ "PrependInt",buffer_prependint},
	{ "PrependString",buffer_prependstring },
	{ "WriteString",buffer_writestring },
	{ "WriteFloat",buffer_writefloat },
	{ "WriteInt",buffer_writeint },
	{ "WriteInt64",buffer_writeint64 },
	{ "ReadAsString",buffer_readstring },
	{ "ReadAllAsString",buffer_readallstring },
	
	{ "ReadAsFloat",buffer_readfloat },
	{ "ReadAsInt",buffer_readint },
	{ "PeekAsInt",buffer_peekint},
	{ "PeekAsFloat",buffer_peekfloat },
	{ "PeekAsString",buffer_peekstring },

	//{ "__gc",buffer_gc },
	{ "Destroy",buffer_gc },
	{ NULL, NULL }
};

void lua_push_ezio_buffer(lua_State*L, Buffer& buf)
{
	LuaEzioBuffer* ptr =(LuaEzioBuffer*)lua_newuserdata(L, sizeof(LuaEzioBuffer));
	*ptr = &buf;
	luaL_setmetatable(L, skey_mt_buffer);
}

int ezio_buffer_create(lua_State*L)
{
	LuaEzioBuffer* ptr = (LuaEzioBuffer*)lua_newuserdata(L, sizeof(LuaEzioBuffer));
	*ptr = new Buffer();
	luaL_setmetatable(L, skey_mt_buffer);
	return 1;
}


int ezio_buffer_destroy(lua_State*L)
{
	Buffer* ptr = lua_check_buffer(L, 1);
	delete ptr;
	return 0;
}

#define register_luac_function(L, fn) (lua_pushcfunction(L, (fn)), lua_setglobal(L, #fn))
void luaopen_netlib(lua_State* L)
{
	if (luaL_newmetatable(L, skey_mt_tcp_connection)) {
		luaL_setfuncs(L,mt_tcp_connection_reg, 0);
		lua_setfield(L, -1, "__index");	
	}
	else {
		std::cout << "associate mt_tcp_connection error!" << std::endl;
	}

	if (luaL_newmetatable(L, skey_mt_buffer)) {
		luaL_setfuncs(L, mt_buffer_reg, 0);
		lua_setfield(L, -1, "__index");	
	}
	else {
		std::cout << "associate mt_buffer error!" << std::endl;
	}

	register_luac_function(L, ezio_buffer_create);
	register_luac_function(L, ezio_buffer_destroy);
}
