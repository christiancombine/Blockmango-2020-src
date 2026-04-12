#ifndef __GAME_ANALYTICS_SETTING_HEADER__
#define __GAME_ANALYTICS_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class CGameAnalytics
	{
	public:
		String GameType = "";
		String GameKey = "";
		String SecretKey = "";
		String Custom01 = "";
		String Custom02 = "";
	};

	class GameAnalyticsSetting
	{
	public:
		static bool ENABLE_ANALYTICS;

		static String INIT_URL;
		static String EVENT_URL;

		static String GAME_KEY;
		static String SECRET_KEY;
		static String CUSTOM_01;
		static String CUSTOM_02;

	public:
		static bool	loadSetting(bool isClient);
		static void	unloadSetting();
		static void setGameType(String gameType);

		static void design(int64_t userId, float value, std::vector<String> parts);

	private:
		static std::map<String, CGameAnalytics*> m_CGameAnalyticsMap;
	};
}

#endif // !__GAME_ANALYTICS_SETTING_HEADER__
