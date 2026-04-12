#include "GUIPixelGunDayActive.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIProgressBar.h"
#include "GUI/GUIPixelGunActiveChest.h"
#include "GUI/GuiDef.h"
#include "Setting/TaskSystemSetting.h"

namespace BLOCKMAN
{
	GUIPixelGunDayActive::GUIPixelGunDayActive() :
		gui_layout("PixelGunDayActive.json")
	{
	}

	GUIPixelGunDayActive::~GUIPixelGunDayActive()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUIPixelGunDayActive::onLoad()
	{
		initChests();
		setDayActiveValue(0);
		getWindow("PixelGunDayActive-Title")->SetText(getString(LanguageKey::GUI_DAY_ACTIVE));
		m_countdown = getWindow<GUIStaticText>("PixelGunDayActive-Countdown");
		m_subscriptionGuard.add(ChangeDayActiveValueEvent::subscribe(std::bind(&GUIPixelGunDayActive::setDayActiveValue, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ChangeDayActiveTimeEvent::subscribe(std::bind(&GUIPixelGunDayActive::setDayActiveTime, this, std::placeholders::_1)));
	}

	void GUIPixelGunDayActive::onUpdate(ui32 nTimeElapse)
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
			m_countdown->SetText(StringUtil::Format("%s:%s:%s",
				hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
				minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
				second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
		}
	}

	void GUIPixelGunDayActive::initChests()
	{
		auto Chests = getWindow("PixelGunDayActive-Chests");
		Chests->CleanupChildren();
		list<TaskActive*>::type actives = TaskSystemSetting::getDayActiveList();
		static unsigned day_chest_index = 0;
		float PositionX = 0.07f;
		float Distance = 0.78f / (actives.size() - 1);
		float Width = 65.0f;
		for (auto active : actives)
		{
			day_chest_index++;
			String ChestName = StringUtil::Format("PixelGunDayActive-Chests-%d", day_chest_index).c_str();
			GUIPixelGunActiveChest* Chest = (GUIPixelGunActiveChest*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_PIXEL_GUN_ACTIVE_CHEST, ChestName.c_str());
			Chest->SetSize({ {0.0f, Width}, {1.0f, 0.0f} });
			Chest->SetXPosition({ PositionX, 0.0f });
			PositionX += Distance;
			Chest->SetTaskActive(active);
			Chests->AddChildWindow(Chest);
		}
	}

	bool GUIPixelGunDayActive::setDayActiveValue(int value)
	{
		auto ActiveValue = getWindow("PixelGunDayActive-Active-Value");
		ActiveValue->SetText(StringUtil::ToString(value).c_str());

		auto ProgressBar = getWindow<GUIProgressBar>("PixelGunDayActive-Active-Progress");
		float Width = ProgressBar->GetPixelSize().x;
		float FristProgress = 0.07f + 40.0f / Width;
		list<TaskActive*>::type actives = TaskSystemSetting::getDayActiveList();
		float Distance = 0.78f / (actives.size() - 1);

		float progress = 0.0f;
		bool is_first = true;
		int last_active_value = 0;
		size_t cur_pos = 0;

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

		for (auto active : actives)
		{
			cur_pos++;
			if (cur_pos == actives.size())
			{
				Distance = Distance + 0.15f - 40.0f / Width;
			}
			if (value >= active->ActiveValue - last_active_value)
			{
				progress += is_first ? FristProgress : Distance;
			}
			else
			{
				if (is_first)
				{
					progress += FristProgress * (float)value / (float)active->ActiveValue;
				}
				else
				{
					progress += Distance * (float)value / (float)(active->ActiveValue - last_active_value);
				}
				break;
			}	
			value -= (active->ActiveValue - last_active_value);
			last_active_value = active->ActiveValue;
			is_first = false;
		}
		ProgressBar->SetProgress(progress);
		return true;
	}

	bool GUIPixelGunDayActive::setDayActiveTime(int seconds)
	{
		if (seconds * 1000 > m_last_time + 3600000)
		{
			list<TaskActive*>::type actives = TaskSystemSetting::getDayActiveList();
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
