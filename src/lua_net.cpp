#include "lua_net.h"
#include <iostream>
#include <chrono>
#include "cxlua.h"
using namespace cxezio;



static const char* skey_mt_buffer = "key_mt_cxbuffer";
using LuaEzioBuffer = Buffer*;

Buffer* lua_check_cxezio_buffer(lua_State* L, int index)
{
	LuaEzioBuffer* buffer = (LuaEzioBuffer*)lua_touserdata(L, index);
	return *buffer;
}

int cxbuffer_readablesize(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	lua_pushinteger(L, buffer->readable_size());
	return 1;
}

int cxbuffer_preview(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	luaL_Buffer lbuf;
	luaL_buffinitsize(L, &lbuf, len);
	luaL_addlstring(&lbuf, buffer->Peek(), len);
	luaL_pushresult(&lbuf);
	return 1;
}

int cxbuffer_consume(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	buffer->Consume(len);
	return 0;
}


int cxbuffer_prependint(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int n = (int)lua_tointeger(L, 2);
	buffer->Prepend(n);

	return 0;
}

int cxbuffer_prependstring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	size_t len = 0;
	const char* data = lua_tolstring(L, 2, &len);
	buffer->Prepend(data, len);
	return 0;
}


int cxbuffer_writestring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	size_t len = 0;
	const char* data = lua_tolstring(L, 2, &len);
	buffer->Write(data, len);
	return 0;
}

int cxbuffer_writeint(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int n = (int)lua_tointeger(L, 2);
	buffer->Write(n);
	return 0;
}

int cxbuffer_writebyte(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int8_t n = (int8_t)lua_tointeger(L, 2);
	buffer->Write((int8_t)n);
	return 0;
}


int cxbuffer_writeint64(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	uint64_t n = (uint64_t)lua_tointeger(L, 2);
	buffer->Write(n);
	return 0;
}

int cxbuffer_writebuffer(lua_State* L)
{
	Buffer* buf = (Buffer*)lua_check_cxezio_buffer(L, 1);
	Buffer* src = (Buffer*)lua_check_cxezio_buffer(L, 2);
	int len = (int)lua_tointeger(L, 3);
	buf->Write(src->Peek(), len);
	return 0;
}




int cxbuffer_writefloat(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	float num = (float)lua_tonumber(L, 2);
	buffer->Write(num);
	return 0;
}


int cxbuffer_readstring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	lua_pushstring(L, (buffer->ReadAsString(len)).c_str());
	return 1;
}

int cxbuffer_readallstring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	std::string str(buffer->ReadAllAsString());
	lua_pushstring(L, str.c_str());
	return 1;
}

int cxbuffer_peekstring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	lua_pushlstring(L, buffer->Peek(), len);
	return 1;
}

int cxbuffer_readfloat(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	float v = buffer->ReadAsFloat();
	lua_pushnumber(L, v);
	return 1;
}

int cxbuffer_peekfloat(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	float v = buffer->PeekAsFloat();
	lua_pushnumber(L, v);
	return 1;
}


int cxbuffer_readint(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int v = buffer->ReadAsInt32();
	lua_pushinteger(L, v);
	return 1;
}


int cxbuffer_readint64(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int64_t v = buffer->ReadAsInt64();
	lua_pushinteger(L, v);
	return 1;
}



int cxbuffer_peekint(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_cxezio_buffer(L, 1);
	int v = buffer->PeekAsInt32();
	lua_pushinteger(L, v);
	return 1;
}


int cxbuffer_gc(lua_State* L)
{
	LuaEzioBuffer* buf = (LuaEzioBuffer*)lua_touserdata(L, 1);
	free(buf);
	buf = nullptr;
	printf("TCPConnection :gc\n");
	return 0;
}

luaL_Reg mt_cxbuffer_reg[] = {
	{ "readable_size",cxbuffer_readablesize },
	{ "Preview",cxbuffer_preview },
	{ "Consume",cxbuffer_consume },
	{ "PrependInt",cxbuffer_prependint},
	{ "PrependString",cxbuffer_prependstring },
	{ "WriteString",cxbuffer_writestring },
	{ "WriteFloat",cxbuffer_writefloat },
	{ "WriteInt",cxbuffer_writeint },
	{ "WriteByte",cxbuffer_writebyte },
	{ "WriteInt64",cxbuffer_writeint64 },
	{ "WriteBuffer",cxbuffer_writebuffer},
	{ "ReadAsString",cxbuffer_readstring },
	{ "ReadAllAsString",cxbuffer_readallstring },

	{ "ReadAsFloat",cxbuffer_readfloat },
	{ "ReadAsInt",cxbuffer_readint },
	{ "ReadAsInt64",cxbuffer_readint64 },
	{ "PeekAsInt",cxbuffer_peekint},
	{ "PeekAsFloat",cxbuffer_peekfloat },
	{ "PeekAsString",cxbuffer_peekstring },

	//{ "__gc",cxbuffer_gc },
	{ "Destroy",cxbuffer_gc },
	{ NULL, NULL }
};

void lua_push_cxezio_buffer(lua_State* L, Buffer& buf)
{
	LuaEzioBuffer* ptr = (LuaEzioBuffer*)lua_newuserdata(L, sizeof(LuaEzioBuffer));
	*ptr = &buf;
	luaL_setmetatable(L, skey_mt_buffer);
}

int cxezio_buffer_create(lua_State* L)
{
	LuaEzioBuffer* ptr = (LuaEzioBuffer*)lua_newuserdata(L, sizeof(LuaEzioBuffer));
	*ptr = new Buffer();
	luaL_setmetatable(L, skey_mt_buffer);

	if (!lua_isnil(L, 1)) {
		auto* buf = lua_check_cxezio_buffer(L, 1);
		int len = (int)lua_tointeger(L, 2);
		(*ptr)->Write(buf->Peek(), len);
	}
	return 1;
}

int cxezio_buffer_destroy(lua_State* L)
{
	Buffer* ptr = lua_check_cxezio_buffer(L, 1);
	delete ptr;
	return 0;
}
 
 
 
//int newthread_dofile(lua_State* L) {
//	const char* path = lua_tostring(L, 1);
//	new std::thread([path]() {
//		lua_State* _L = luaL_newstate();
//		luaL_openlibs(_L);
//		luaopen_cxlua(_L);
//		int res = luaL_dofile(_L, path);
//		check_lua_error(_L, res);
//		lua_close(_L);
//		});
//	return 0;
//}
//
//int newthread_dostring(lua_State* L) {
//	const char* code = lua_tostring(L, 1);
//	new std::thread([code]() {
//		lua_State* _L = luaL_newstate();
//		luaL_openlibs(_L);
//		luaopen_cxlua(_L);
//		int res = luaL_dostring(_L, code);
//		check_lua_error(_L, res);
//		lua_close(_L);
//		});
//	return 0;
//}


 

#define register_luac_function(L, fn) (lua_pushcfunction(L, (fn)), lua_setglobal(L, #fn))
void luaopen_netlib(lua_State* L)
{
 

	if (luaL_newmetatable(L, skey_mt_buffer)) {
		luaL_setfuncs(L, mt_cxbuffer_reg, 0);
		lua_setfield(L, -1, "__index");
	}
	else {
		std::cout << "associate mt_buffer error!" << std::endl;
	}


	register_luac_function(L, cxezio_buffer_create);
	register_luac_function(L, cxezio_buffer_destroy);

	/*register_luac_function(L, newthread_dofile);
	register_luac_function(L, newthread_dostring);*/
	  

}
