#pragma once
#ifndef __GUI_TASK_SYSTEM_HEADER__
#define __GUI_TASK_SYSTEM_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUICheckBox.h"
#include "GUI/RootGuiLayout.h"
#include "GUITaskDailyPanel.h"
#include "GUITaskMainLinePanel.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_taskSystem : public gui_layout
	{
	private:
		GUITaskDailyPanel* m_task_daily_panel = nullptr;
		GUITaskMainLinePanel* m_task_mainline_panel = nullptr;

		GUICheckBox* m_task_tab_daily = nullptr;
		GUICheckBox* m_task_tab_mainline = nullptr;

	public:
		gui_taskSystem();
		virtual ~gui_taskSystem();

		void onLoad() override;
		bool isNeedLoad() override;
		bool isForceUpdate() { return true; }
		void onGetCommonData(String key, String data);

	private:
		bool onCloseClick(const EventArgs & events);
		void initTabsAndPanels();
		bool onDailyTabSelected(const EventArgs & events);
		bool onMainLineTabSelected(const EventArgs & events);
		bool updateMainLineRedPoint(bool show);
		bool updateDailyRedPoint(bool show);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}
#endif 
