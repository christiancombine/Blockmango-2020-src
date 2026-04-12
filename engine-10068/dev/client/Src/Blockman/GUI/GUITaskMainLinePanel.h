#ifndef __GUI_TASK_MAINLINE_PANEL_HEADER__
#define __GUI_TASK_MAINLINE_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIListBox.h"
#include "GUITaskMainLineTab.h"
#include "GUITaskPanel.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUITaskMainLinePanel : public gui_layout
	{
	private:
		list<GUITaskMainLineTab*>::type m_tabs;
		list<GUITaskPanel*>::type m_panels;
		GUITaskMainLineTab* m_current_tab = nullptr;
		GUITaskPanel* m_current_panel = nullptr;

	public:
		GUITaskMainLinePanel();
		~GUITaskMainLinePanel();

		void onLoad() override;
		bool onTabChange(int id);
		GUITaskMainLineTab* getTabById(int id);
		GUITaskPanel* getPanelById(int id);
		bool onUpdateMainLineTaskData(int lv, const String & data);
		bool updateRedPoint();
	};
}
#endif 

