#ifndef __GUI_SEASON_REWARD_HEADER__
#define __GUI_SEASON_REWARD_HEADER__

#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_seasonReward : public gui_layout
	{
	private:
		int m_show_actor_time = 0;
		int m_cur_honor_id = 0;

	public:
		gui_seasonReward();
		~gui_seasonReward();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		bool onCloseClick(const EventArgs & events);
		bool onUpdateLastSeason(int honorId, int rank, int honor);
		bool onUpdateCurrentSeason(int honorId, int rank, int honor, int endTime);

	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif