#include "GUIWalkingDeadMain.h"
#include "Setting/WalkingDeadStoreSetting.h"

namespace BLOCKMAN
{
	GUIWalkingDeadMain::GUIWalkingDeadMain() :
		gui_layout("WalkingDeadMain.json")
		
	{
		
	}

	GUIWalkingDeadMain::~GUIWalkingDeadMain()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUIWalkingDeadMain::onLoad()
	{
		m_refresh_box_button = getWindow<GUIButton>("WalkingDeadMain-Supply-Box");
		m_end_box_button = getWindow<GUIButton>("WalkingDeadMain-Supply-EndBox");
		m_vip_time = getWindow<GUIStaticImage>("WalkingDeadMain-Vip-Time");
		m_vip_time_text = getWindow<GUIStaticText>("WalkingDeadMain-Vip-Time-Text");

		m_red_point = getWindow<GUIStaticImage>("WalkingDeadMain-Red-Point");
		m_red_point->SetVisible(false);
		m_daily_supply_text = getWindow<GUIStaticText>("WalkingDeadMain-Supply-Box-Time");

		//m_vip_time_text->SetText(getString("vip.days.remaining"));
		m_refresh_box_button->subscribeEvent(EventButtonClick, std::bind(&GUIWalkingDeadMain::onButtonClick, this, std::placeholders::_1));
		m_end_box_button->subscribeEvent(EventButtonClick, std::bind(&GUIWalkingDeadMain::onButtonClick, this, std::placeholders::_1));
	}

	bool GUIWalkingDeadMain::onButtonClick(const EventArgs & events)
	{
		WalkingDeadShowStoreEvent::emit(1);
		return true;
	}

	void GUIWalkingDeadMain::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
		{
			return;
		}
		updateVipTime();
		updateDailySupplyTime();
		updateEnderBoxUI();
	}

	void GUIWalkingDeadMain::updateVipTime()
	{
		i64 time = WalkingDeadStoreSetting::getVipTime();
		int level = WalkingDeadStoreSetting::getVipLevel();
		int dayTime = (60 * 24 * 60);
		int day;
		if (time/1000 > dayTime)
		{
			i64 seconds = time / 1000;
			 day = seconds / (60 * 24 * 60);
		}
		else
		{
			if (time > 0)
			{
				day = 1;
			}
			else
			{
				day = 0;
			}
		}

		if (day > 0)
		{
			m_vip_time->SetVisible(true);
			if (level == 1)
			{
				m_vip_time->SetImage("set:walkingdead_vip.json image:vip1");
			}
			else if (level == 2)
			{
				m_vip_time->SetImage("set:walkingdead_vip.json image:vip2");
			}
			else if (level == 3)
			{
				m_vip_time->SetImage("set:walkingdead_vip.json image:vip3");
			}
			m_vip_time_text->SetText(StringUtil::Format("%s%d", LanguageManager::Instance()->getString("vip.days.remaining").c_str(), day).c_str());
				
		}
		else
		{
			m_vip_time->SetVisible(false);
		}
	}

	void GUIWalkingDeadMain::updateDailySupplyTime()
	{
		auto supply = WalkingDeadStoreSetting::findSupplyByType(WalkingDeadSupplyType::DailyBox);
		if (!supply)
		{
			m_refresh_box_button->SetTouchable(false);
			return;
		}
		int64_t time = supply->RefreshTime;
		if (time > 0)
		{
			m_red_point->SetVisible(false);
			m_refresh_box_button->SetTouchable(true);
			int seconds = time / 1000;
			int second = seconds % 60;
			int minute = (seconds % 3600) / 60;
			int hour = seconds / 3600;
			m_daily_supply_text->SetText(StringUtil::Format("%s:%s:%s",
				hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
				minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
				second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
		}
		else
		{
			m_refresh_box_button->SetTouchable(true);
			m_red_point->SetVisible(true);
		}
	}

	void GUIWalkingDeadMain::updateEnderBoxUI()
	{
		auto supply = WalkingDeadStoreSetting::findSupplyByType(WalkingDeadSupplyType::VipBox);
		if (!supply)
		{
			m_end_box_button->SetVisible(false);
			return;
		}
		m_end_box_button->SetVisible(supply->Status == CanReceive);
	}
}
