#ifndef __GUI_BLOCKCITY_RANK_AWARD_HEADER__
#define __GUI_BLOCKCITY_RANK_AWARD_HEADER__

#include "GUI/gui_layout.h"

namespace LORD {
	class GUIStaticText;
	class GUIListHorizontalBox;
}

namespace BLOCKMAN
{
	class UserRank;

	class gui_blockCityRankAward : public gui_layout
	{
	public:
		gui_blockCityRankAward();
		~gui_blockCityRankAward();

		void onLoad() override;
		void onShow() override;
		bool isNeedLoad() override;

		bool onSetDatas(i32 myRank);
		bool onCashPrize(const EventArgs& events);

	private:
		GUIListHorizontalBox* m_list = nullptr;
		GUIStaticText* m_rank = nullptr;
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
