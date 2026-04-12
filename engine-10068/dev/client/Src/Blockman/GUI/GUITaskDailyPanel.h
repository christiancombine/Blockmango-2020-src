#ifndef __GUI_TASK_DAILY_PANEL_HEADER__
#define __GUI_TASK_DAILY_PANEL_HEADER__

#include "gui_layout.h"
#include "GUIPixelGunDayActive.h"
#include "GUIPixelGunWeekActive.h"
#include "GUITaskPanel.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUITaskDailyPanel : public gui_layout
	{
	private:
		GUIPixelGunDayActive* m_day_active = nullptr;
		GUIPixelGunWeekActive* m_week_active = nullptr;
		GUITaskPanel* m_tasks = nullptr;

	public:
		GUITaskDailyPanel();
		~GUITaskDailyPanel();

		void onLoad() override;
		bool isForceUpdate() { return true; }
		void initDailyTaskPanel();
		bool onUpdateDailyTaskData(const String & data);
		bool updateRedPoint();
	};
}

#endif