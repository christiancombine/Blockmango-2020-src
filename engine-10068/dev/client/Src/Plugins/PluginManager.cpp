#include "PluginManager.h"
#include "LuaRegister/Template/LuaEngine.h"
#include "LuaRegister/Template/Util/AutoRunner.h"
#include "LuaRegister/AutoClientRegister.h"

void PluginManager::loadScript(const char * path, const char* name)
{
	LuaEngine::getInstance()->addScriptDir(path);

	if (!PathUtil::IsFileExist(name))
		return;

	LuaEngine::getInstance()->Load(name);
}
