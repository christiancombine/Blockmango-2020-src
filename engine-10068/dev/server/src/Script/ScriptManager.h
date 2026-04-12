#ifndef __SCRIPT_MANAGER_H__
#define __SCRIPT_MANAGER_H__

#include "Object/Singleton.h"
#include "LuaRegister/Template/LuaEngine.h"
#include "LuaRegister/Template/Util/AutoRunner.h"
#include "LuaRegister/Content/AutoLogicRegister.h"

using namespace LORD;

class ScriptManager : public Singleton<ScriptManager>
{

public:
	void loadScript(const char* path, bool isMain)
	{
		LuaEngine::getInstance()->addScriptDir(path);
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		if (!isMain)
		{
			std::string BaseMainPath = path;
			BaseMainPath += "\\base\\BaseMain.lua";
			LuaEngine::getInstance()->Load(BaseMainPath.c_str());
		}
#else
		std::string BaseMainPath = path;
		BaseMainPath += "/base/BaseMain.lua";
		LuaEngine::getInstance()->Load(BaseMainPath.c_str());
#endif
		if (isMain)
		{
			std::string MainPath = path;
			MainPath += "/ScriptMain.lua";
			LuaEngine::getInstance()->Load(MainPath.c_str());
		}
	}
};

#endif