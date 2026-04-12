#include "gui_taskSystem.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "Setting/TaskSystemSetting.h"
#include "Network/protocol/CommonDataBuilder.h"

namespace BLOCKMAN
{
	gui_taskSystem::gui_taskSystem() :
		gui_layout("TaskSystem.json")
	{
		m_task_daily_panel = m_task_daily_panel ? m_task_daily_panel : LordNew GUITaskDailyPanel;
		m_task_mainline_panel = m_task_mainline_panel ? m_task_mainline_panel : LordNew GUITaskMainLinePanel;
	}

	gui_taskSystem::~gui_taskSystem()
	{
		m_subscriptionGuard.unsubscribeAll();
		TaskSystemSetting::unloadSetting();
		LordSafeDelete(m_task_daily_panel);
		LordSafeDelete(m_task_mainline_panel);
	}

	void gui_taskSystem::onLoad()
	{
		TaskSystemSetting::loadSetting();

		auto Close = getWindow<GUIButton>("TaskSystem-Close-Button");
		Close->subscribeEvent(EventButtonClick, std::bind(&gui_taskSystem::onCloseClick, this, std::placeholders::_1));

		auto Title = getWindow("TaskSystem-Title-Text");
		Title->SetText(getString(LanguageKey::GUI_TASKSYSTEM_TITLE));

		initTabsAndPanels();
	}

	bool gui_taskSystem::isNeedLoad()
	{
		return checkGameType(ClientGameType::PixelGunHall);
	}

	void gui_taskSystem::onGetCommonData(String key, String data)
	{
		if (key == "MainLineTaskData")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int level = builder.getIntParam("level");
			String c_data = builder.getStringParam("data");
			m_task_mainline_panel->onUpdateMainLineTaskData(level, c_data);
			return;
		}

		if (key == "DailyTaskData")
		{
			m_task_daily_panel->onUpdateDailyTaskData(data);
			return;
		}

		if (key == "TaskActiveValue")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int type = builder.getIntParam("type");
			int value = builder.getIntParam("value");
			if (type == (int)TaskActiveType::ACTIVE_TYPE_DAY)
				ChangeDayActiveValueEvent::emit(value);
			else
				ChangeWeekActiveValueEvent::emit(value);
			return;
		}

		if (key == "TaskActiveTime")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int type = builder.getIntParam("type");
			int seconds = builder.getIntParam("seconds");
			if (type == (int)TaskActiveType::ACTIVE_TYPE_DAY)
				ChangeDayActiveTimeEvent::emit(seconds);
			else
				ChangeWeekActiveTimeEvent::emit(seconds);
			return;
		}

		if (key == "TaskActiveStatus")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int activeId = builder.getIntParam("activeId");
			int status = builder.getIntParam("status");
			auto active = TaskSystemSetting::findTaskActiveById(activeId);
			if (active == nullptr)
				return;
			active->Status = (TaskActiveStatus)status;
			active->HasChange = true;
			return;
		}
	}

	void gui_taskSystem::initTabsAndPanels()
	{
		GUILayout* TaskPanels = getWindow<GUILayout>("TaskSystem-Panels");
		if (TaskPanels == NULL)
			return;

		m_task_tab_daily = getWindow<GUICheckBox>("TaskSystem-Tab-Daily");
		m_task_tab_mainline = getWindow<GUICheckBox>("TaskSystem-Tab-MainLine");

		m_task_daily_panel->attachTo(this, TaskPanels);
		m_task_mainline_panel->attachTo(this, TaskPanels);

		m_task_tab_daily->SetText(LanguageManager::Instance()->getString("task_tab_daily").c_str());
		m_task_tab_mainline->SetText(LanguageManager::Instance()->getString("task_tab_mainline").c_str());

		m_task_tab_daily->subscribeEvent(EventCheckStateChanged, std::bind(&gui_taskSystem::onDailyTabSelected, this, std::placeholders::_1));
		m_task_tab_mainline->subscribeEvent(EventCheckStateChanged, std::bind(&gui_taskSystem::onMainLineTabSelected, this, std::placeholders::_1));

		m_subscriptionGuard.add(UpdateMainLineRedPoiontEvent::subscribe(std::bind(&gui_taskSystem::updateMainLineRedPoint, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateDailyRedPoiontEvent::subscribe(std::bind(&gui_taskSystem::updateDailyRedPoint, this, std::placeholders::_1)));

		m_task_tab_daily->SetChecked(true);
	}

	bool gui_taskSystem::onDailyTabSelected(const EventArgs & events)
	{	
		bool checked = m_task_tab_daily->GetChecked();
		m_task_tab_daily->SetTouchable(!checked);
		m_task_tab_mainline->SetTouchable(checked);
		m_task_tab_mainline->SetChecked(!checked);
		if (checked)
		{
			m_task_daily_panel->show();
			m_task_mainline_panel->hide();
		}
		return true;
	}

	bool gui_taskSystem::onMainLineTabSelected(const EventArgs & events)
	{
		bool checked = m_task_tab_mainline->GetChecked();
		m_task_tab_mainline->SetTouchable(!checked);
		m_task_tab_daily->SetTouchable(checked);
		m_task_tab_daily->SetChecked(!checked);
		if (checked)
		{
			m_task_mainline_panel->show();
			m_task_daily_panel->hide();
		}
		return true;
	}

	bool gui_taskSystem::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_taskSystem::updateMainLineRedPoint(bool show)
	{
		auto RedPoint = getWindow<GUIStaticImage>("TaskSystem-Tab-MainLine-RedPoint");
		if (RedPoint == NULL)
		{
			return false;
		}
		RedPoint->SetVisible(show);
		return true;
	}

	bool gui_taskSystem::updateDailyRedPoint(bool show)
	{
		auto RedPoint = getWindow<GUIStaticImage>("TaskSystem-Tab-Daily-RedPoint");
		if (RedPoint == NULL)
		{
			return false;
		}
		RedPoint->SetVisible(show);
		return true;
	}

}