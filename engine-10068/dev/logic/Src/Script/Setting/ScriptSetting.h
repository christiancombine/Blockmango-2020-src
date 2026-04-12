#pragma once

#ifndef __SCRIPT_SETTING_HEADER__
#define __SCRIPT_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class ScriptSetting
	{
	public:
		static std::string m_gameType;
		static std::string m_mapName;
		static std::string m_serverIp;
		static int m_serverPort;
		static int m_engineVersion;
		static String m_scriptPath;

		static String loadGameScriptDir(const String& gameType, bool isClient, bool checkLock = true);
		static String getScriptPath() {
			return m_scriptPath;
		};
		static void loadAllGameResource();
	};
}

#endif // !__SCRIPT_SETTING_HEADER__
