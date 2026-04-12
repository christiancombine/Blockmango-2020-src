#include "GUITaskMainLinePanel.h"
#include "Util/ClientEvents.h"
#include "UI/GUILayout.h"
#include "Setting/TaskSystemSetting.h"
#include "GUI/GuiDef.h"
#include "GUITaskMainLineTab.h"
#include "GUITaskPanel.h"
#include "Util/MacroUtil.h"

namespace BLOCKMAN
{
	GUITaskMainLinePanel::GUITaskMainLinePanel() :
		gui_layout("TaskMainLinePanel.json")
	{

	}

	GUITaskMainLinePanel::~GUITaskMainLinePanel()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUITaskMainLinePanel::onLoad()
	{

		map<int, TaskGroup*>::type MainLineTasks = TaskSystemSetting::getMainLineTasks();
		if (MainLineTasks.empty())
			return;

		ShowPixelGunTaskIconEvent::emit();
		m_subscriptionGuard.add(TaskMainLineTabChangeEvent::subscribe(std::bind(&GUITaskMainLinePanel::onTabChange, this, std::placeholders::_1)));
		m_subscriptionGuard.add(CheckMainLineRedPointEvent::subscribe(std::bind(&GUITaskMainLinePanel::updateRedPoint, this)));

		GUILayout* Tabs = getWindow<GUILayout>("TaskMainLinePanel-Tabs");
		GUILayout* Panels = getWindow<GUILayout>("TaskMainLinePanel-TaskPanels");
		if (Tabs == NULL || Panels == NULL)
			return;

		GUIListBox* TabList = (GUIListBox*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LIST, "TaskMainLinePanel-Tabs-List");
		TabList->SetSize({ {1.0f, -40.0f}, {1.0f, -12.0f} });
		TabList->SetXPosition({ 0.0f, 6.0f });
		TabList->SetVerticalAlignment(VA_CENTRE);
		TabList->SetInterval(4.0f);
		Tabs->AddChildWindow(TabList);

		static unsigned mainline_tab_index = 0;
		for (auto group : MainLineTasks)
		{
			mainline_tab_index++;
			GUIString TabName = StringUtil::Format("TaskMainLinePanel-Tabs-%d", mainline_tab_index).c_str();
			GUITaskMainLineTab* pMainLineTab = (GUITaskMainLineTab*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_TASK_MAINLINE_TAB, TabName);
			pMainLineTab->SetSize({ {1.0f, 0.0f}, {0.0f, 70.0f} });
			TabList->AddItem(pMainLineTab, false);
			pMainLineTab->InitMainLineTab(group.second);
			m_tabs.push_back(pMainLineTab);

			GUIString PanelName = StringUtil::Format("TaskMainLinePanel-Panels-%d", mainline_tab_index).c_str();
			GUITaskPanel* pTaskPanel = (GUITaskPanel*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_TASK_PANEL, PanelName);
			pTaskPanel->SetSize({ {1.0f, -12.0f}, {1.0f, -12.0f} });
			pTaskPanel->SetPosition({ {0.0f, 6.0f}, {0.0f, 6.0f} });
			Panels->AddChildWindow(pTaskPanel);
			pTaskPanel->InitMainLineTasks(group.second->Id);
			pTaskPanel->SetVisible(false);
			m_panels.push_back(pTaskPanel);
		}

		GUITaskMainLineTab* tab = *m_tabs.begin();
		tab->SetSelectTab(true);
	}

	bool GUITaskMainLinePanel::onTabChange(int id)
	{
		auto tab = getTabById(id);
		if (!tab)
		{
			return false;
		}
		if (m_current_tab && m_current_tab->GetTabId() != id)
		{
			m_current_tab->SetSelectTab(false);
		}
		for (auto panel : m_panels)
		{
			NULL_CONTINUE(panel);

			panel->SetVisible(false);
			if (panel->GetPanelId() == id)
			{							
				panel->SetVisible(true);
				TaskGroup* group = TaskSystemSetting::findTaskGroupByGroupId(id);
				if (!group)
				{
					return false;
				}
				panel->ShowMainLineTasks(group);
				m_current_tab = tab;
				m_current_panel = panel;
			}
		}
		return true;
	}

	GUITaskMainLineTab* GUITaskMainLinePanel::getTabById(int id)
	{
		for (auto tab : m_tabs)
		{
			if (tab->GetTabId() == id)
			{
				return tab;
			}
		}
		return nullptr;
	}

	GUITaskPanel* GUITaskMainLinePanel::getPanelById(int id)
	{
		for (auto panel : m_panels)
		{
			if (panel && panel->GetPanelId() == id)
			{
				return panel;
			}
		}
		return nullptr;
	}

	bool GUITaskMainLinePanel::onUpdateMainLineTaskData(int lv, const String & data)
	{
		map<int, TaskGroup*>::type result = TaskSystemSetting::parseUpdateMainLineTaskData(data);
		if (result.empty())
		{
			return false;
		}
		ShowPixelGunTaskIconEvent::emit();

		if (m_current_tab && m_current_panel)
		{
			TaskGroup* group = TaskSystemSetting::findTaskGroupByGroupId(m_current_tab->GetTabId());
			if (!group)
			{
				return false;
			}
			m_current_panel->ShowMainLineTasks(group);
		}

		for (auto group : result)
		{
			auto tab = getTabById(group.second->Id);
			if (tab != nullptr)
			{
				if (group.second->Lv <= lv)
				{
					tab->UnLockTab();
				}
				tab->UpdateRedPoint();
			}
		}
		updateRedPoint();
		return true;
	}

	bool GUITaskMainLinePanel::updateRedPoint()
	{
		for (auto tab : m_tabs)
		{
			if (tab->m_has_new)
			{
				UpdateMainLineRedPoiontEvent::emit(true);
				return true;
			}
		}
		UpdateMainLineRedPoiontEvent::emit(false);
		return true;
	}
}
