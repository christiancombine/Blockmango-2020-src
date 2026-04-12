#include "GUISkyBlockTaskMainLinePanel.h"
#include "Util/ClientEvents.h"
#include "UI/GUILayout.h"
#include "Setting/SkyBlockTaskSystemSetting.h"
#include "GUI/GuiDef.h"
#include "GUISkyBlockTaskMainLineTab.h"
#include "GUISkyBlockTaskPanel.h"

namespace BLOCKMAN
{
	GUISkyBlockTaskMainLinePanel::GUISkyBlockTaskMainLinePanel() :
		gui_layout("SkyBlockTaskMainLinePanel.json")
	{

	}

	GUISkyBlockTaskMainLinePanel::~GUISkyBlockTaskMainLinePanel()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUISkyBlockTaskMainLinePanel::onLoad()
	{
		map<int, SkyBlockTaskGroup>::type MainLineTasks;
		if (!SkyBlockTaskSystemSetting::Instance()->getMainLineTasks(MainLineTasks))
			return;

		m_subscriptionGuard.add(SkyBlockTaskMainLineTabChangeEvent::subscribe(std::bind(&GUISkyBlockTaskMainLinePanel::onTabChange, this, std::placeholders::_1)));
		m_subscriptionGuard.add(SkyBlockUpdateMainLineTaskDataEvent::subscribe(std::bind(&GUISkyBlockTaskMainLinePanel::onUpdateMainLineTaskData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(SkyBlockCheckMainLineRedPointEvent::subscribe(std::bind(&GUISkyBlockTaskMainLinePanel::updateRedPoint, this)));

		GUILayout* Top_Tabs = getWindow<GUILayout>("SkyBlockTaskMainLinePanel-Top-Tabs");
		GUILayout* Panels = getWindow<GUILayout>("SkyBlockTaskMainLinePanel-TaskPanels");
		if (Panels == NULL || Top_Tabs == NULL)
			return;

		auto mainline_tab_index = 0;
		auto ChildWidth = 150.f;
		for (auto group : MainLineTasks)
		{
			mainline_tab_index++;
			GUIString TabName = StringUtil::Format("SkyBlockTaskMainLinePanel-Tabs-%d", mainline_tab_index).c_str();
			GUISkyBlockTaskMainLineTab* pMainLineTab = (GUISkyBlockTaskMainLineTab*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_TASK_MAINLINE_TAB, TabName);
			pMainLineTab->SetArea(UDim(0.f, (mainline_tab_index - 1) * ChildWidth), UDim(0, 0.0f), UDim(0.f, ChildWidth), UDim(1.f, 0));
			Top_Tabs->AddChildWindow(pMainLineTab);
			pMainLineTab->InitMainLineTab(group.second);
			m_tabs.push_back(pMainLineTab);

			GUIString PanelName = StringUtil::Format("SkyBlockTaskMainLinePanel-Panels-%d", mainline_tab_index).c_str();
			GUISkyBlockTaskPanel* pTaskPanel = (GUISkyBlockTaskPanel*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_TASK_PANEL, PanelName);
			pTaskPanel->SetSize({ {1.0f, -12.0f}, {1.0f, -12.0f} });
			pTaskPanel->SetPosition({ {0.0f, 6.0f}, {0.0f, 6.0f} });
			Panels->AddChildWindow(pTaskPanel);
			pTaskPanel->InitMainLineTasks(group.second.Lv);
			pTaskPanel->SetVisible(false);
			m_panels.push_back(pTaskPanel);
		}

		if (!m_tabs.empty())
		{
			GUISkyBlockTaskMainLineTab* tab = dynamic_cast<GUISkyBlockTaskMainLineTab*>(*m_tabs.begin());
			if (tab != nullptr)
				tab->SetSelectTab(true);
		}
		
	}

	bool GUISkyBlockTaskMainLinePanel::onTabChange(int id)
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
			panel->SetVisible(false);
			if (panel->GetPanelId() == id)
			{
				panel->SetVisible(true);
				SkyBlockTaskGroup group;
				if (!SkyBlockTaskSystemSetting::Instance()->findTaskGroupByLevel(id, group))
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

	GUISkyBlockTaskMainLineTab* GUISkyBlockTaskMainLinePanel::getTabById(int id)
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

	GUISkyBlockTaskPanel* GUISkyBlockTaskMainLinePanel::getPanelById(int id)
	{
		for (auto panel : m_panels)
		{
			if (panel->GetPanelId() == id)
			{
				return panel;
			}
		}
		return nullptr;
	}

	bool GUISkyBlockTaskMainLinePanel::onUpdateMainLineTaskData(int lv, int progressesLevel, const String & data)
	{
		map<int, SkyBlockTaskGroup>::type result;
		if (!SkyBlockTaskSystemSetting::Instance()->parseUpdateMainLineTaskData(data, result))
		{
			return false;
		}

		ShowSkyBlockTaskIconEvent::emit();
		if (m_current_tab && m_current_panel)
		{
			SkyBlockTaskGroup group;
			if (!SkyBlockTaskSystemSetting::Instance()->findTaskGroupByLevel(m_current_tab->GetTabId(), group))
			{
				return false;
			}
			m_current_panel->ShowMainLineTasks(group);
		}

		for (auto group : result)
		{
			auto tab = getTabById(group.second.Lv);
			if (tab != nullptr)
			{
				if (group.second.Lv <= lv && group.second.Lv <= progressesLevel)
				{
					tab->UnLockTab();
				}
				tab->UpdateDatra(lv, progressesLevel);
				tab->UpdateRedPoint();
			}
		}
		updateRedPoint();
		return true;
	}

	bool GUISkyBlockTaskMainLinePanel::updateRedPoint()
	{
		for (auto tab : m_tabs)
		{
			if (tab->m_has_new)
			{
				SkyBlockUpdateMainLineRedPoiontEvent::emit(true);
				return true;
			}
		}
		SkyBlockUpdateMainLineRedPoiontEvent::emit(false);
		return true;
	}
	void GUISkyBlockTaskMainLinePanel::showMainTabByLevel()
	{
		int level = 0;
		for (auto tab : m_tabs)
		{
			if (!tab->getTabIsLock())
			{
				if (tab->GetTabId() > level)
					level = tab->GetTabId();
			}
		}

		auto select_tab = getTabById(level);
		if (select_tab)
			select_tab->SetSelectTab(true);
	}


}
