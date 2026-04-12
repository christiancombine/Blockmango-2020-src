#include "GameAnalyticsSetting.h"
#include "GameAnalyticsRequest.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Script/Event/LogicScriptEvents.h"

namespace BLOCKMAN
{
	void GameAnalyticsSetting::setGameAnalytics(const String& analyticsData)
	{
		auto builder = CommonDataBuilder().fromData(analyticsData);
		CLIENT_IP = builder.getStringParam("ClientIp");
		GAME_KEY = builder.getStringParam("GameKey");
		SECRET_KEY = builder.getStringParam("SecretKey");
		CUSTOM_01 = builder.getStringParam("Custom01");
		CUSTOM_02 = builder.getStringParam("Custom02");
		INIT_URL = StringUtil::Format("http://api.gameanalytics.com/v2/%s/init", GAME_KEY.c_str()).c_str();
		EVENT_URL = StringUtil::Format("http://api.gameanalytics.com/v2/%s/events", GAME_KEY.c_str()).c_str();
		GameAnalyticsRequest::Instance()->asyncInit(3);
	}

	void GameAnalyticsSetting::design(float value, std::vector<String> parts)
	{
		SCRIPT_EVENT::GameAnalyticsDesignEvent::invoke(0, value, parts);
	}

}