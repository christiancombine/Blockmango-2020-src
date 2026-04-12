#ifndef __GUI_BED_WAR_SEASON_REWARD_HEADER__
#define __GUI_BED_WAR_SEASON_REWARD_HEADER__

#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class RootGuiLayout;
	class gui_bedwarSeasonReward : public gui_layout
	{
	private:
		int m_cur_honor_id = 0;

	public:
		gui_bedwarSeasonReward();
		~gui_bedwarSeasonReward();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		bool onCloseClick(const EventArgs & events);
		bool onUpdateLastSeason(int honorId, int rank, int honor);
		bool onUpdateCurrentSeason(int honorId, int rank, int honor, int endTime);

	private:
		RootGuiLayout * getParent();
	};
}

#endif