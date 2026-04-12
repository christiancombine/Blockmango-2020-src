#include "GUITaskDailyPanel.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "GUITaskPanel.h"

namespace BLOCKMAN
{
	GUITaskDailyPanel::GUITaskDailyPanel() :
		gui_layout("TaskDailyPanel.json")
	{
		m_day_active = m_day_active ? m_day_active : LordNew GUIPixelGunDayActive;
		m_week_active = m_week_active ? m_week_active : LordNew GUIPixelGunWeekActive;
	}

	GUITaskDailyPanel::~GUITaskDailyPanel()
	{
		LordSafeDelete(m_day_active);
		LordSafeDelete(m_week_active);
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUITaskDailyPanel::onLoad()
	{
		m_subscriptionGuard.add(CheckDailyRedPointEvent::subscribe(std::bind(&GUITaskDailyPanel::updateRedPoint, this)));

		auto DayActive = getWindow("TaskDailyPanel-Day-Active");
		m_day_active->attachTo(this, DayActive);

		auto TaskContent = getWindow("TaskDailyPanel-Tasks");

		auto WeekActive = getWindow("TaskDailyPanel-Week-Active");
		m_week_active->attachTo(this, WeekActive);

		initDailyTaskPanel();
	}

	void GUITaskDailyPanel::initDailyTaskPanel()
	{
		auto TaskPanel = getWindow("TaskDailyPanel-Tasks");
		if (TaskPanel == NULL)
		{
			return;
		}
		GUIString PanelName = StringUtil::Format("TaskPanel-DailyTasks").c_str();
		GUITaskPanel* pTaskPanel = (GUITaskPanel*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_TASK_PANEL, PanelName);
		pTaskPanel->SetSize({ {1.0f, -12.0f}, {1.0f, -12.0f} });
		pTaskPanel->SetPosition({ {0.0f, 6.0f}, {0.0f, 6.0f} });
		TaskPanel->AddChildWindow(pTaskPanel);
		m_tasks = pTaskPanel;
	}

	bool GUITaskDailyPanel::onUpdateDailyTaskData(const String & data)
	{
		list<Task*>::type result = TaskSystemSetting::parseUpdateDailyTaskData(data);
		if (result.empty() || m_tasks == nullptr)
		{
			return false;
		}
		ShowPixelGunTaskIconEvent::emit();
		m_tasks->ShowDailyTasks(result);
		updateRedPoint();
		return true;
	}
	
	bool GUITaskDailyPanel::updateRedPoint()
	{
		list<Task*>::type result = TaskSystemSetting::getDailyTasks();
		map<int, TaskActive*>::type  active_map = TaskSystemSetting::getTaskActives();
		if (result.empty() || active_map.empty())
		{
			return false;
		}
		for (auto task : result)
		{
			if (task->Status == TASK_STATUS_NOT_GET)
			{
				UpdateDailyRedPoiontEvent::emit(true);
				return true;
			}
		}
		for (auto task : active_map)
		{
			if (task.second->Status == ACTIVE_STATUS_DONE)
			{
				UpdateDailyRedPoiontEvent::emit(true);
				return true;
			}
		}
		UpdateDailyRedPoiontEvent::emit(false);
		return true;
	}
}
