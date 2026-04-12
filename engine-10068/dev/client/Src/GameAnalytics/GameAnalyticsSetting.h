#ifndef __GAME_ANALYTICS_SETTING_HEADER__
#define __GAME_ANALYTICS_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GameAnalyticsSetting : public Singleton<GameAnalyticsSetting>, public ObjectAlloc
	{
	public:
		String INIT_URL = "http://api.gameanalytics.com/v2/46ba9366773d89c6ce05ee6aa8b3cac1/init";
		String EVENT_URL = "http://api.gameanalytics.com/v2/46ba9366773d89c6ce05ee6aa8b3cac1/events";
		
		String GAME_KEY = "46ba9366773d89c6ce05ee6aa8b3cac1";
		String SECRET_KEY = "f74a45057bc15e72fc40cd9bc834340398ea388e";
		String CLIENT_IP = "0.0.0.0";
		String CUSTOM_01 = "G1";
		String CUSTOM_02 = "Test";

	public:
		void setGameAnalytics(const String& analyticsData);

		void design(float value, std::vector<String> parts);
	};
}

#endif // !__GAME_ANALYTICS_SETTING_HEADER__
