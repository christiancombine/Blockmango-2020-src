#ifndef __GUI_BLOCKCITY_CHARM_RANK_HEADER__
#define __GUI_BLOCKCITY_CHARM_RANK_HEADER__

#include "GUI/gui_layout.h"


namespace LORD {
	class GUIListBox;
}

namespace BLOCKMAN
{
	class gui_blockCityCharmRank : public gui_layout
	{
	public:
		gui_blockCityCharmRank();
		~gui_blockCityCharmRank();

		void onLoad() override;
		void onShow() override;
		void onHide() override;
		bool isNeedLoad() override;

		bool onClose(const EventArgs& events);
		void onSetDatas();
		bool onDataChange();

	private:
		GUIListBox* m_list = nullptr;
		bool m_dataChange = false;
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
