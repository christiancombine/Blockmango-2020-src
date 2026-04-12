#ifndef __GUI_SKYBLOCK_TASK_MAINLINE_PANEL_HEADER__
#define __GUI_SKYBLOCK_TASK_MAINLINE_PANEL_HEADER__

#include "gui_layout.h"
#include "GUISkyBlockTaskMainLineTab.h"
#include "GUISkyBlockTaskPanel.h"
#include "UI/GUIListBox.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISkyBlockTaskMainLinePanel : public gui_layout
	{
	private:
		list<GUISkyBlockTaskMainLineTab*>::type m_tabs;
		list<GUISkyBlockTaskPanel*>::type m_panels;
		GUISkyBlockTaskMainLineTab* m_current_tab = nullptr;
		GUISkyBlockTaskPanel* m_current_panel = nullptr;

	public:
		GUISkyBlockTaskMainLinePanel();
		~GUISkyBlockTaskMainLinePanel();

		void onLoad() override;
		bool onTabChange(int id);
		GUISkyBlockTaskMainLineTab* getTabById(int id);
		GUISkyBlockTaskPanel* getPanelById(int id);
		bool onUpdateMainLineTaskData(int lv, int progressesLevel, const String & data);
		bool updateRedPoint();
		void showMainTabByLevel();
	};
}
#endif 

