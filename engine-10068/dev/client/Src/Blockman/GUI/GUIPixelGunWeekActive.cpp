#include "GUIPixelGunWeekActive.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GUIPixelGunActiveChest.h"
#include "GUI/GuiDef.h"
#include "Setting/TaskSystemSetting.h"

namespace BLOCKMAN
{
	GUIPixelGunWeekActive::GUIPixelGunWeekActive() :
		gui_layout("PixelGunWeekActive.json")
	{
	}

	GUIPixelGunWeekActive::~GUIPixelGunWeekActive()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUIPixelGunWeekActive::onLoad()
	{
		initChests();
		setWeekActiveValue(0);
		getWindow("PixelGunWeekActive-Title")->SetText(getString(LanguageKey::GUI_WEEK_ACTIVE));
		m_countdown = getWindow<GUIStaticText>("PixelGunWeekActive-Countdown");
		m_subscriptionGuard.add(ChangeWeekActiveValueEvent::subscribe(std::bind(&GUIPixelGunWeekActive::setWeekActiveValue, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ChangeWeekActiveTimeEvent::subscribe(std::bind(&GUIPixelGunWeekActive::setWeekActiveTime, this, std::placeholders::_1)));
	}

	void GUIPixelGunWeekActive::onUpdate(ui32 nTimeElapse)
	{
		m_last_time -= nTimeElapse;
		if (!isShown())
			return;

		if (m_last_time > 0)
		{
			int seconds = m_last_time / 1000;
			int second = seconds % 60;
			int minute = (seconds % 3600) / 60;
			int hour = seconds / 3600;
			int day = hour / 24;
			if (day > 0)
			{
				m_countdown->SetText(StringUtil::Format("%d %s", day, getString(LanguageKey::GUI_BIRD_ACTIVITY_DAY).c_str()).c_str());
			}
			else
			{
				m_countdown->SetText(StringUtil::Format("%s:%s:%s",
					hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
					minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
					second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
			}
		}
	}

	void GUIPixelGunWeekActive::initChests()
	{
		auto Chests = getWindow("PixelGunWeekActive-Chests");
		Chests->CleanupChildren();
		list<TaskActive*>::type actives = TaskSystemSetting::getWeekActiveList();
		static unsigned week_chest_index = 0;
		float AllDistance = Math::Min(0.2f * actives.size(), 0.75f);
		float Distance = AllDistance / (actives.size() - 1);
		float PositionX = (1.0f - AllDistance) / 2 - 0.05f;
		float Width = 65.0f;
		for (auto active : actives)
		{
			week_chest_index++;
			String ChestName = StringUtil::Format("PixelGunWeekActive-Chests-%d", week_chest_index).c_str();
			GUIPixelGunActiveChest* Chest = (GUIPixelGunActiveChest*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_PIXEL_GUN_ACTIVE_CHEST, ChestName.c_str());
			Chest->SetSize({ {0.0f, Width}, {1.0f, 0.0f} });
			Chest->SetXPosition({ PositionX, 0.0f });
			PositionX += Distance;
			Chest->SetTaskActive(active);
			Chests->AddChildWindow(Chest);
		}
	}

	bool GUIPixelGunWeekActive::setWeekActiveValue(int value)
	{
		auto ActiveValue = getWindow("PixelGunWeekActive-Active-Value");
		ActiveValue->SetText(StringUtil::ToString(value).c_str());
		list<TaskActive*>::type actives = TaskSystemSetting::getWeekActiveList();
		for (auto active : actives)
		{
			if (active->ActiveValue > value)
			{
				continue;
			}
			if (active->Status == ACTIVE_STATUS_UNDONE)
			{
				active->Status = ACTIVE_STATUS_DONE;
				active->HasChange = true;
			}
		}
		return true;
	}

	bool GUIPixelGunWeekActive::setWeekActiveTime(int seconds)
	{
		if (seconds * 1000 > m_last_time + 3600000)
		{
			list<TaskActive*>::type actives = TaskSystemSetting::getWeekActiveList();
			for (auto active : actives)
			{
				active->Status = ACTIVE_STATUS_UNDONE;
				active->HasChange = true;
			}
		}
		m_last_time = seconds * 1000;
		return true;
	}
}
