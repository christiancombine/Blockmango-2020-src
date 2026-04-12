#ifndef __GUI_SKYBLOCK_TASK_DARE_PANEL_HEADER__
#define __GUI_SKYBLOCK_TASK_DARE_PANEL_HEADER__

#include "gui_layout.h"
#include "GUISkyBlockTaskPanel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISkyBlockTaskDarePanel : public gui_layout
	{
	private:
		GUISkyBlockTaskPanel*  m_panels = nullptr;

	public:
		GUISkyBlockTaskDarePanel();
		~GUISkyBlockTaskDarePanel();

		const int MAX_DARE_TASK = 3;

		void onLoad() override;
		//GUISkyBlockTaskDareTab* getTabById(int id);
		bool onUpdateDareTaskData(int lv, int progressesLevel, const String & data);
		bool onUpdateDareTaskInfo(const String & data);

		bool OnAddTimesButtonClick(const EventArgs & events);
		bool OnRefreshButtonClick(const EventArgs & events);
		bool parseDaretaskInfo(const String & info);
		void onUpdate(ui32 nTimeElapse) override;
		//bool updateRedPoint();
		void updateAdStatus(bool have_cube_ad);

		enum DareTaskOperateType
		{
			ADD_TIMES = 1,
			REFRESH_TASK = 2,
			UP_DATA = 3,
		};

		GUIStaticText* m_nextTimeText = nullptr;
		GUIStaticText* m_nextTime = nullptr;

		GUIButton* m_addTimes = nullptr;
		GUIStaticText* m_timesText = nullptr;
		GUIStaticText* m_timesNum = nullptr;

		GUIButton* m_refresh = nullptr;
		GUIStaticImage* m_refresh_coinImg = nullptr;
		GUIStaticText* m_refresh_coinNum = nullptr;
		GUIStaticText* m_refresh_free_text = nullptr;

		GUIButton* m_refresh_task = nullptr;
		GUIButton* m_refresh_free_task = nullptr;

		int m_cur_tiems = 0;
		int m_max_tiems = 0;
		int m_add_coinId = 0;
		int m_add_coinNum = 0;
		int m_re_coinId = 0;
		int m_re_coinNum = 0;
		int m_next_time = 0;
		int m_max_free_times = 0;
		int m_cur_free_times = 0;
		bool m_have_cube_ad = false;
	};
}
#endif 

