#pragma once

#ifndef __BASE_TYPE_TRANSLATE_HEADER__
#define __BASE_TYPE_TRANSLATE_HEADER__

#include "LuaRegister/Template/Public.h"
#include "LuaRegister/Template/Util/Traits.h"
#include "Core.h"
#include "CoreDef.h"
#include <string>
#include <UI/GUIString.h>
#include <UI/GUIUDim.h>

using namespace LORD;
template<typename T>
struct IsBaseType
{
	static constexpr bool value = false;
};

template<> struct IsBaseType<char *>             { static constexpr bool value = true; };
template<> struct IsBaseType<const char *>       { static constexpr bool value = true; };
template<> struct IsBaseType<char >              { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned char>      { static constexpr bool value = true; };
template<> struct IsBaseType<short>              { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned short>     { static constexpr bool value = true; };
template<> struct IsBaseType<long>               { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned long>      { static constexpr bool value = true; };
template<> struct IsBaseType<int>                { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned int>       { static constexpr bool value = true; };
template<> struct IsBaseType<float>              { static constexpr bool value = true; };
template<> struct IsBaseType<double>             { static constexpr bool value = true; };
template<> struct IsBaseType<bool>               { static constexpr bool value = true; };
template<> struct IsBaseType<void>               { static constexpr bool value = true; };
template<> struct IsBaseType<long long>          { static constexpr bool value = true; };
template<> struct IsBaseType<unsigned long long> { static constexpr bool value = true; };
template<> struct IsBaseType<std::string>        { static constexpr bool value = true; };
template<> struct IsBaseType<String>             { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::Vector2>		 { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::Vector2i>	 { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::Vector3>		 { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::Vector3i>	 { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::Vector4>		 { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::GUIString>	 { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::UDim>		 { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::Color>		 { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::ERect>		 { static constexpr bool value = true; };
template<> struct IsBaseType<LORD::Quaternion>	 { static constexpr bool value = true; };

namespace LUA_CALL
{
	template<typename T>
	static bool setField(lua_State *L, int tableIndex, const char *fieldName, T t);
	template<typename T>
	static bool getField(lua_State *L, int tableIndex, const char *fieldName, T&t);
	template<typename T>
	static bool seti(lua_State *L, int tableIndex, int i, T t);
	template<typename T>
	static bool geti(lua_State *L, int tableIndex, int i, T&t);
}

namespace BaseTypeTranslate
{
	// read
	template<typename T>
	inline T read(lua_State *L, int index)
	{
		return (T)lua_tonumber(L, index);
	}

	template<>
	inline char* read<char*>(lua_State *L, int index)
	{
		auto rt =  (char*)lua_tostring(L, index);
		if (rt == nullptr)
		{
			rt = "";
			LordLogError("SCRIPT_EXCEPTION");
			Log::printLuaStack();
		}
		return rt;
	}

	template<>
	inline std::string read<std::string>(lua_State *L, int index)
	{
		size_t len = 0;
		auto rt = lua_tolstring(L, index, &len);
		if (rt == nullptr)
		{
			rt = "";
			LordLogError("SCRIPT_EXCEPTION");
			Log::printLuaStack();
		}
		return std::string(rt, len);
	}

	template<>
	inline String read<String>(lua_State *L, int index)
	{
		size_t len = 0;
		auto rt = lua_tolstring(L, index, &len);
		if (rt == nullptr)
		{
			rt = "";
			LordLogError("SCRIPT_EXCEPTION");
			Log::printLuaStack();
		}
		return String(rt, len);
	}

	template<>
	inline const char* read<const char*>(lua_State *L, int index)
	{
		auto rt = lua_tostring(L, index);
		if (rt == nullptr)
		{
			rt = "";
			LordLogError("SCRIPT_EXCEPTION");
			Log::printLuaStack();
		}
		return rt;
	}

	template<>
	inline bool read<bool>(lua_State *L, int index)
	{
		if (lua_isboolean(L, index))
			return lua_toboolean(L, index) != 0;
		else
			return lua_tonumber(L, index) != 0;
	}

	template<>
	inline void read<void>(lua_State *L, int index)
	{
		(void)L;
		(void)index;
		return;
	}

	template<>
	inline long long read(lua_State *L, int index)
	{
		if (lua_isnumber(L, index))
			return (long long)lua_tonumber(L, index);
		else
			return *(long long*)lua_touserdata(L, index);
	}

	template<>
	inline unsigned long long read(lua_State *L, int index)
	{
		if (lua_isnumber(L, index))
			return (unsigned long long)lua_tonumber(L, index);
		else
			return *(unsigned long long*)lua_touserdata(L, index);
	}

	template<>
	inline Vector2 read(lua_State *L, int index)
	{
		Vector2 v2;
		LUA_CALL::getField(L, index, "x", v2.x);
		LUA_CALL::getField(L, index, "y", v2.y);
		return v2;
	}

	template<>
	inline Vector2i read(lua_State *L, int index)
	{
		Vector2i v2;
		LUA_CALL::getField(L, index, "x", v2.x);
		LUA_CALL::getField(L, index, "y", v2.y);
		return v2;
	}

	template<>
	inline Vector3 read(lua_State *L, int index)
	{
		Vector3 v3;
		LUA_CALL::getField(L, index, "x", v3.x);
		LUA_CALL::getField(L, index, "y", v3.y);
		LUA_CALL::getField(L, index, "z", v3.z);
		return v3;
	}

	template<>
	inline Vector3i read(lua_State *L, int index)
	{
		Vector3i v3;
		LUA_CALL::getField(L, index, "x", v3.x);
		LUA_CALL::getField(L, index, "y", v3.y);
		LUA_CALL::getField(L, index, "z", v3.z);
		return v3;
	}

	template<>
	inline LORD::Vector4 read<LORD::Vector4>(lua_State *L, int index)
	{
		if (!lua_istable(L, index))
			return LORD::Vector4::INVALID;
		LORD::Vector4 v4;
		LUA_CALL::getField(L, index, "x", v4.x);
		LUA_CALL::getField(L, index, "y", v4.y);
		LUA_CALL::getField(L, index, "z", v4.z);
		LUA_CALL::getField(L, index, "w", v4.w);
		return v4;
	}

	template<>
	inline LORD::GUIString read<LORD::GUIString>(lua_State*L, int index)
	{
		size_t len = 0;
		const char *data = lua_tolstring(L, index, &len);
		if (data) {
			return LORD::GUIString((const utf8*)data, len);
		}
		return LORD::GUIString();
	}

	template<>
	inline LORD::UDim read(lua_State *L, int index)
	{
		LORD::UDim v;
		LUA_CALL::geti(L, index, 1, v.d_scale);
		LUA_CALL::geti(L, index, 2, v.d_offset);
		return v;
	}

	template<>
	inline LORD::Color read(lua_State *L, int index)
	{
		LORD::Color v;
		LUA_CALL::geti(L, index, 1, v.r);
		LUA_CALL::geti(L, index, 2, v.g);
		LUA_CALL::geti(L, index, 3, v.b);
		LUA_CALL::geti(L, index, 4, v.a);
		return v;
	}

	template<>
	inline LORD::ERect read(lua_State *L, int index)
	{
		LORD::ERect v;
		LUA_CALL::geti(L, index, 1, v.left);
		LUA_CALL::geti(L, index, 2, v.top);
		LUA_CALL::geti(L, index, 3, v.right);
		LUA_CALL::geti(L, index, 4, v.bottom);
		return v;
	}

	template<>
	inline LORD::Quaternion read(lua_State *L, int index)
	{
		LORD::Quaternion q;
		LUA_CALL::getField(L, index, "x", q.x);
		LUA_CALL::getField(L, index, "y", q.y);
		LUA_CALL::getField(L, index, "z", q.z);
		LUA_CALL::getField(L, index, "w", q.w);
		return q;
	}

	//write
	template<typename T>
	inline void write(lua_State *L, T t)
	{
		lua_pushnumber(L, t);
	}

	template<>
	inline void write<char*>(lua_State *L, char* ret)
	{
		lua_pushstring(L, ret);
	}

	template<>
	inline void write<std::string>(lua_State *L, std::string ret)
	{
		lua_pushlstring(L, ret.c_str(), ret.size());
	}

	template<>
	inline void write<String>(lua_State *L, String ret)
	{
		lua_pushlstring(L, ret.c_str(), ret.size());
	}

	template<>
	inline void write(lua_State *L, const char* ret)
	{
		lua_pushstring(L, ret);
	}

	template<>
	inline void write(lua_State *L, bool ret)
	{
		lua_pushboolean(L, ret);
	}

	template<>
	inline void write(lua_State *L, long long ret)
	{
		*(long long*)lua_newuserdata(L, sizeof(long long)) = ret;
		lua_getglobal(L, "__s64");
		lua_setmetatable(L, -2);
	}

	template<>
	inline void write(lua_State *L, unsigned long long ret)
	{
		*(unsigned long long*)lua_newuserdata(L, sizeof(unsigned long long)) = ret;
		lua_getglobal(L, "__u64");
		lua_setmetatable(L, -2);
	}

	template<>
	inline void write(lua_State *L, Vector2 v2)
	{
		lua_createtable(L, 0, 2);
		LUA_CALL::setField(L, -1, "x", v2.x);
		LUA_CALL::setField(L, -1, "y", v2.y);
	}

	template<>
	inline void write(lua_State *L, Vector2i v2)
	{
		lua_createtable(L, 0, 2);
		LUA_CALL::setField(L, -1, "x", v2.x);
		LUA_CALL::setField(L, -1, "y", v2.y);
	}

	template<>
	inline void write(lua_State *L, Vector3 v3)
	{
		lua_createtable(L, 0, 3);
		LUA_CALL::setField(L, -1, "x", v3.x);
		LUA_CALL::setField(L, -1, "y", v3.y);
		LUA_CALL::setField(L, -1, "z", v3.z);
	}

	template<>
	inline void write(lua_State *L, Vector3i v3)
	{
		lua_createtable(L, 0, 3);
		LUA_CALL::setField(L, -1, "x", v3.x);
		LUA_CALL::setField(L, -1, "y", v3.y);
		LUA_CALL::setField(L, -1, "z", v3.z);
	}

	template<>
	inline void write(lua_State *L, Vector4 v4)
	{
		if (v4.isInvalid())
		{
			lua_pushnil(L);
			return;
		}
		lua_createtable(L, 0, 4);
		LUA_CALL::setField(L, -1, "x", v4.x);
		LUA_CALL::setField(L, -1, "y", v4.y);
		LUA_CALL::setField(L, -1, "z", v4.z);
		LUA_CALL::setField(L, -1, "w", v4.w);
	}

	template<>
	inline void write(lua_State*L, LORD::GUIString str)
	{
		const char *strUtf8 = str.c_str();
		lua_pushstring(L, strUtf8);
	}

	template<>
	inline void write(lua_State *L, LORD::UDim v)
	{
		lua_createtable(L, 0, 2);
		LUA_CALL::seti(L, -1, 1, v.d_scale);
		LUA_CALL::seti(L, -1, 2, v.d_offset);
	}

	template<>
	inline void write(lua_State *L, LORD::ERect v)
	{
		lua_createtable(L, 0, 4);
		LUA_CALL::seti(L, -1, 1, v.left);
		LUA_CALL::seti(L, -1, 2, v.top);
		LUA_CALL::seti(L, -1, 3, v.right);
		LUA_CALL::seti(L, -1, 4, v.bottom);
	}

	template<>
	inline void write(lua_State *L, LORD::Color v)
	{
		lua_createtable(L, 0, 4);
		LUA_CALL::seti(L, -1, 1, v.r);
		LUA_CALL::seti(L, -1, 2, v.g);
		LUA_CALL::seti(L, -1, 3, v.b);
		LUA_CALL::seti(L, -1, 4, v.a);
	}
	

	template<>
	inline void write(lua_State *L, LORD::Quaternion q)
	{
		if (q.isInvalid())
		{
			lua_pushnil(L);
			return;
		}
		lua_createtable(L, 0, 4);
		LUA_CALL::setField(L, -1, "x", q.x);
		LUA_CALL::setField(L, -1, "y", q.y);
		LUA_CALL::setField(L, -1, "z", q.z);
		LUA_CALL::setField(L, -1, "w", q.w);
	}

};

#endif