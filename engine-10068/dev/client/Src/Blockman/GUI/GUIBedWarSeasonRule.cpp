#include "GUIBedWarSeasonRule.h"
#include "UI/GUIStaticImage.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Setting/SeasonSetting.h"
#include "Util/PlayerPrefs.h"
#include "Network/ClientHttpRequest.h"

namespace BLOCKMAN
{
	GUIBedWarSeasonRule::GUIBedWarSeasonRule() :
		gui_layout("BedWarSeasonRule.json")
	{
	}

	GUIBedWarSeasonRule::~GUIBedWarSeasonRule()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUIBedWarSeasonRule::onLoad()
	{
		auto SeasonRule = getWindow("BedWarSeasonRule");
		SeasonRule->subscribeEvent(EventWindowTouchUp, std::bind(&GUIBedWarSeasonRule::onContentClick, this, std::placeholders::_1));

		getWindow("BedWarSeasonRule-Title")->SetText(getString(LanguageKey::GUI_SEASON_RULE_TITLE));
		getWindow("BedWarSeasonRule-Bottom-Hint")->SetText(getString(LanguageKey::GUI_SEASON_RULE_BOTTOM_HINT));
		getWindow("BedWarSeasonRule-Rule-Text")->SetText(getString(LanguageKey::GUI_SEASON_RULE_CONTENT));

		for (int i = SeasonHonorId::BRONZE; i <= SeasonHonorId::KING; ++i)
		{
			auto season = SeasonSetting::findSeasonByHonorId(static_cast<SeasonHonorId>(i));
			if (season)
			{
				String honorIconStr = StringUtil::Format("BedWarSeasonRule-Honor-Lv%d-Icon", i + 1);
				String honorNameStr = StringUtil::Format("BedWarSeasonRule-Honor-Lv%d-Name", i + 1);
				String honorValueStr = StringUtil::Format("BedWarSeasonRule-Honor-Lv%d-Value-Text", i + 1);
				String honorFlagStr = StringUtil::Format("BedWarSeasonRule-Honor-Lv%d-Bg", i + 1);
				getWindow<GUIStaticImage>(honorIconStr.c_str())->SetImage(season->HonorIcon.c_str());
				getWindow(honorNameStr.c_str())->SetText(getString(season->HonorName));
				getWindow(honorValueStr.c_str())->SetText(StringUtil::ToString(season->Score).c_str());
				getWindow<GUIStaticImage>(honorFlagStr.c_str())->SetImage(season->HonorFlag.c_str());
			}
		}

		if (PlayerPrefs::Instance()->getBoolPrefs("IsLoadSeason"))
		{
			ClientHttpRequest::loadBedWarSeasonRule("g1008");
		}
		m_subscriptionGuard.add(UpdateBedWarSeasonRuleEvent::subscribe(std::bind(&GUIBedWarSeasonRule::onUpdateSeasonRule, this, std::placeholders::_1)));
	}

	void GUIBedWarSeasonRule::onUpdate(ui32 nTimeElapse)
	{

	}

	bool GUIBedWarSeasonRule::onContentClick(const EventArgs & events)
	{
		hide();
		return true;
	}

	bool GUIBedWarSeasonRule::onUpdateSeasonRule(const String & data)
	{
		SeasonSetting::parseSeasonRuleData(data);
		for (int i = SeasonHonorId::BRONZE; i <= SeasonHonorId::KING; ++i)
		{
			auto season = SeasonSetting::findSeasonByHonorId(static_cast<SeasonHonorId>(i));
			if (season)
			{
				String honorValueLvStr = StringUtil::Format("BedWarSeasonRule-Honor-Lv%d-Value-Text", i + 1);
				getWindow(honorValueLvStr.c_str())->SetText(StringUtil::ToString(season->Score).c_str());
			}
		}
		return true;
	}

}
