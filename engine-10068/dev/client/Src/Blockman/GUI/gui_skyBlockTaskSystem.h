#pragma once
#ifndef __GUI_SKYBLOCK_TASK_SYSTEM_HEADER__
#define __GUI_SKYBLOCK_TASK_SYSTEM_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "GUISkyBlockTaskMainLinePanel.h"
#include "GUISkyBlockTaskDarePanel.h"
#include "UI/GUICheckBox.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_skyBlockTaskSystem : public gui_layout
	{
	private:
		GUISkyBlockTaskMainLinePanel* m_task_mainline_panel = nullptr;
		GUISkyBlockTaskDarePanel* m_task_dare_panel = nullptr;


		GUICheckBox* m_task_tab_mainline = nullptr;
		GUICheckBox* m_task_tab_challenge = nullptr;
		GUIStaticText* m_title = nullptr;
	public:
		gui_skyBlockTaskSystem();
		virtual ~gui_skyBlockTaskSystem();

		void onLoad() override;
		bool isNeedLoad() override;
		bool isForceUpdate() { return true; }
		void showMainTab();

	private:
		bool onCloseClick(const EventArgs & events);
		void initTabsAndPanels();
		bool onMainLineTabSelected(const EventArgs & events);
		bool onChallengeTabSelected(const EventArgs & events);
		bool updateMainLineRedPoint(bool show);
		bool updateDareTaskRedPoint(bool show);
		bool onUnLockDareTaskEvent(bool isUnlock);
		bool onCubeAdVisible(bool visible);

		bool m_isUnLockDare = false;
		bool m_have_cube_ad = false;

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}
#endif 
