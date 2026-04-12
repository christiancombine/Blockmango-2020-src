#ifndef __GUI_BLOCKCITY_CHOOSE_PAPER_HEADER__
#define __GUI_BLOCKCITY_CHOOSE_PAPER_HEADER__

#include "GUI/gui_layout.h"

namespace LORD {
	class GUILayout;
}

namespace BLOCKMAN
{
	class BlockCityStoreItem;

	class gui_blockCityChoosePaper : public gui_layout
	{
	public:
		gui_blockCityChoosePaper();
		~gui_blockCityChoosePaper();

		void onLoad() override;
		void onShow() override;
		void onHide() override;
		bool isNeedLoad() override;
		bool onSure(const EventArgs& events);
		bool open(i32 guideId, const vector<i32>::type& items);
		void initItems(const vector<BlockCityStoreItem>::type& items);
		bool onItemClick(i32 propId);

	private:
		i32 m_propId = 0;
		i32 m_guideId = 0;
		
		GUILayout* m_items = nullptr;
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
