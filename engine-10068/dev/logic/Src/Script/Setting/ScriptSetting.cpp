#include "ScriptSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::string ScriptSetting::m_gameType = "";
	std::string ScriptSetting::m_mapName = "gameName-debug";
	std::string ScriptSetting::m_serverIp = "127.0.0.1";
	int ScriptSetting::m_serverPort = 19130;
	int ScriptSetting::m_engineVersion = 90900;
	String ScriptSetting::m_scriptPath = "";

	String ScriptSetting::loadGameScriptDir(const String& gameType, bool isClient, bool checkLock)
	{
		CsvReader* pReader = new CsvReader();
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		std::string filePath = PathUtil::ConcatPath(".//", "Media", "Scripts", "ScriptSetting.csv").c_str();
#else 
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
		std::string filePath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "ScriptSetting.csv").c_str();
#else
		std::string filePath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media", "Scripts", "ScriptSetting.csv").c_str();
#endif
#endif
		bool bRet = pReader->LoadFile(filePath);
		if (!bRet)
		{
			LordLogError("Can not open ScriptSetting setting file %s", filePath.c_str());
			LordSafeDelete(pReader);
			return "";
		}
		pReader->Read(); // discard the second line
		bool Finded = false;
		String GameType = "";
		//String ScriptPath = "";
		int MaxWeight = -1;
		bool Enable = false;
		while (pReader->Read())
		{
			if (isClient)
			{
				pReader->GetString("GameType", GameType);
				if (gameType != GameType)
					continue;
			}
			else
			{
				pReader->GetBool("Enable", Enable);
				if (!Enable)
					continue;
				pReader->GetString("GameType", GameType);
				if (checkLock)
				{
					String lockFile = GameType + ".lock";
					if (PathUtil::IsFileExist(lockFile))
						continue;
				}
			}
			Finded = true;
			int Weight = 0;
			pReader->GetInt("Enable", Weight);
			if (Weight > MaxWeight)
			{
				MaxWeight = Weight;
				m_scriptPath = "Media/Scripts/";
				String Cursor = isClient ? "ClientGame" : "ServerGame";
				m_scriptPath += Cursor;
				pReader->GetString("GroupDirectory", Cursor);
				m_scriptPath += "/";
				m_scriptPath += Cursor;
				pReader->GetString("Name", Cursor);
				m_scriptPath += "/";
				m_scriptPath += Cursor;
				pReader->GetString("GameType", m_gameType);
				pReader->GetString("MapName", m_mapName);
				pReader->GetString("ServerIP", m_serverIp);
				pReader->GetInt("ServerPort", m_serverPort);
				pReader->GetInt("EngineVersion", m_engineVersion);
			}
		}
		if (m_mapName.empty())
			m_mapName = "gameName-debug";
		LordSafeDelete(pReader);
		if (Finded)
			return m_scriptPath;
		else
			return "";
	}

	void ScriptSetting::loadAllGameResource()
	{
		CsvReader* pReader = new CsvReader();
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		std::string filePath = PathUtil::ConcatPath(".//", "Media", "Scripts", "ScriptSetting.csv").c_str();
#else
		std::string filePath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media", "Scripts", "ScriptSetting.csv").c_str();
#endif
		bool bRet = pReader->LoadFile(filePath);
		if (!bRet)
		{
			LordLogError("Can not open ScriptSetting setting file %s", filePath.c_str());
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			String ScriptPath = "Media/Scripts/ClientGame";
			String Cursor;
			pReader->GetString("GroupDirectory", Cursor);
			ScriptPath += "/";
			ScriptPath += Cursor;
			pReader->GetString("Name", Cursor);
			ScriptPath += "/";
			ScriptPath += Cursor;
			String cfgPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), ScriptPath, "res", "resource.cfg");
			bool bEnable{false};
			pReader->GetBool("Enable", bEnable);
			if (PathUtil::IsFileExist(cfgPath))
				Root::Instance()->loadResourceByConfig(ScriptPath, cfgPath);
		}
		LordSafeDelete(pReader);
	}
}