#ifndef __LUA_ENGINE_HEADER_H__
#define __LUA_ENGINE_HEADER_H__

#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/LuaExt.h"
#include "LuaRegister/Template/Util/LuaDebug.h"
#include "LuaRegister/Template/LuaCall.h"
#include "LuaRegister/3rd/lbitop.h"

class LuaEngine
{
public:
	static LuaEngine *getInstance()
	{
		static LuaEngine obj;
		return &obj;
	}

	lua_State * getLuaState() { return m_L; }

	void Load(const char *path);

	void addScriptDir(const char *path);

	void removeScriptDir(const char* path);

	bool isLoadSuc() { return m_loadSuc; }

private:
	LuaEngine() { init(); };
	~LuaEngine()
	{
		if (m_L) {
			Log::setLuaState(NULL);
			/**
			 * 这里销毁 lua_State 会引发退出客户端时崩溃，
			 * 原因是 LuaEngine 销毁后 lua_State 还会被 LuaRefDeleter 持有，
			 * 而 LuaRefDeleter 会调用 luaL_unref 这个 lua API 进行解引用操作
			 */
			//lua_close(m_L);
			//m_L = NULL;
		}
	}

	void init();

	static int l_print(lua_State* L);
	static int l_traceback(lua_State* L);

private:
	lua_State *m_L;
	bool   m_loadSuc = false;
};

inline void LuaEngine::init()
{
	m_L = luaL_newstate();
	Log::setLuaState(m_L);
	luaL_openlibs(m_L);
	luaopen_bitop(m_L);

	LuaExt::init(m_L);
	lua_getglobal(m_L, "tostring");
	lua_rawseti(m_L, LUA_REGISTRYINDEX, -1);
	lua_pushcclosure(m_L, l_traceback, 0);
	lua_pushvalue(m_L, -1);
	lua_rawseti(m_L, LUA_REGISTRYINDEX, -2);
	lua_setglobal(m_L, "traceback");
	lua_pushcclosure(m_L, l_print, 0);
	lua_setglobal(m_L, "print");

}

inline void LuaEngine::addScriptDir(const char *path)
{
	lua_getglobal(m_L, "package");
	lua_getfield(m_L, -1, "path");
	const char* cur_path = lua_tostring(m_L, -1);
	lua_pushfstring(m_L, "%s;%s/?.lua", cur_path, path);
	lua_setfield(m_L, -3, "path");
	lua_pop(m_L, 2);
}

inline void LuaEngine::removeScriptDir(const char* path)
{
	lua_getglobal(m_L, "package");
	lua_getfield(m_L, -1, "path");
	std::string cur_path = std::string(lua_tostring(m_L, -1));
	std::string del_path = ";" + std::string(path) + "/?.lua";
	std::string::size_type pos = 0;
	pos = cur_path.find(del_path);
	if (pos != std::string::npos)
	{
		cur_path = cur_path.replace(pos, del_path.size(), "");
		lua_pushfstring(m_L, cur_path.c_str());
		lua_setfield(m_L, -3, "path");
		lua_pop(m_L, 2);
	}
}

inline int LuaEngine::l_print(lua_State* L)
{
	int n = lua_gettop(L);
	String str = "";
	for (int i = 1; i <= n; i++)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, -1);	// tostring
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		size_t len = 0;
		const char * s = lua_tolstring(L, -1, &len);
		if (s)
			str.append(s, len);
		else
			str += luaL_typename(L, i);
		if (i < n)
			str += '\t';
	}
	LOG(G3LOG_INFO) << "[Lua] " << str;
	return 0;
}

inline int LuaEngine::l_traceback(lua_State* L)
{
	int n = lua_gettop(L);
	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_insert(L, 1);
	lua_pop(L, 1);
	lua_call(L, n, 1);
#ifdef _WINDOWS
	if (lua_type(L, 1) == LUA_TSTRING)
	{
		lua_getfield(L, 1, "gsub");
		lua_insert(L, 1);
		lua_pushstring(L, "(lua/[^ ]+%.lua):([0-9]+):");
		lua_pushstring(L, "../res/%1(%2):");
		lua_call(L, 3, 1);
	}
#endif
	return 1;
}

inline void LuaEngine::Load(const char *path)
{
	lua_pushcclosure(m_L, LuaDebug::onError, 0);
	int errfunc = lua_gettop(m_L);

	if (luaL_loadfile(m_L, path) == 0)
	{
		 lua_pcall(m_L, 0, 1, errfunc);
		 m_loadSuc = true;
	}
	else
	{
		LuaDebug::printError(m_L, "%s", lua_tostring(m_L, -1));
	}

	lua_remove(m_L, errfunc);
	lua_pop(m_L, 1);
}

#endif // 
