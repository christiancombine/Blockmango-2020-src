#include "GameAnalyticsSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "GameVersionDefine.h"
#include "Script/Event/LogicScriptEvents.h"

namespace BLOCKMAN
{
	std::map<String, CGameAnalytics*>  GameAnalyticsSetting::m_CGameAnalyticsMap = {};

	bool GameAnalyticsSetting::ENABLE_ANALYTICS = false;

	String GameAnalyticsSetting::INIT_URL = "http://api.gameanalytics.com/v2/46ba9366773d89c6ce05ee6aa8b3cac1/init";
	String GameAnalyticsSetting::EVENT_URL = "http://api.gameanalytics.com/v2/46ba9366773d89c6ce05ee6aa8b3cac1/events";

	String GameAnalyticsSetting::GAME_KEY = "46ba9366773d89c6ce05ee6aa8b3cac1";
	String GameAnalyticsSetting::SECRET_KEY = "f74a45057bc15e72fc40cd9bc834340398ea388e";
	String GameAnalyticsSetting::CUSTOM_01 = "G1";
	String GameAnalyticsSetting::CUSTOM_02 = "Test";

	bool GameAnalyticsSetting::loadSetting(bool isClient)
	{
		m_CGameAnalyticsMap.clear();
		CGameAnalytics* pGameAnalytics = NULL;
		CsvReader* pReader = new CsvReader();

		std::string path;
		if (isClient)
		{
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/GameAnalytics.csv")).c_str();
		}
		else
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/GameAnalytics.csv")).c_str();
#else
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "GameAnalytics.csv")).c_str();
#endif
		}
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open game analytics setting file %s", path.c_str());
			LordSafeDelete(pReader);
			return false;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pGameAnalytics = new CGameAnalytics();
			pReader->GetString("GameType", pGameAnalytics->GameType);
			pReader->GetString("GameKey", pGameAnalytics->GameKey);
			pReader->GetString("SecretKey", pGameAnalytics->SecretKey);
			pReader->GetString("Custom01", pGameAnalytics->Custom01);
			pReader->GetString("Custom02", pGameAnalytics->Custom02);
			m_CGameAnalyticsMap[pGameAnalytics->GameType] = pGameAnalytics;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void GameAnalyticsSetting::unloadSetting()
	{
		for (auto iter = m_CGameAnalyticsMap.begin(); iter != m_CGameAnalyticsMap.end(); iter++)
		{
			delete iter->second;
		}
		m_CGameAnalyticsMap.clear();
	}

	void GameAnalyticsSetting::setGameType(String gameType)
	{
		if (EngineVersionSetting::canUseCmd())
		{
			gameType = "g1000";
		}
		auto iter = m_CGameAnalyticsMap.find(gameType);
		if (iter != m_CGameAnalyticsMap.end())
		{
			GAME_KEY = iter->second->GameKey;
			SECRET_KEY = iter->second->SecretKey;
			CUSTOM_01 = iter->second->Custom01;
			CUSTOM_02 = iter->second->Custom02;
			INIT_URL = StringUtil::Format("http://api.gameanalytics.com/v2/%s/init", GAME_KEY.c_str()).c_str();
			EVENT_URL = StringUtil::Format("http://api.gameanalytics.com/v2/%s/events", GAME_KEY.c_str()).c_str();
		}
	}

	void GameAnalyticsSetting::design(int64_t userId, float value, std::vector<String> parts)
	{
		SCRIPT_EVENT::GameAnalyticsDesignEvent::invoke(userId, value, parts);
	}

}