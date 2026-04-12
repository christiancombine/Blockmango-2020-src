#ifndef __GUI_BLOCKCITY_RECEIVE_FLOWER_HEADER__
#define __GUI_BLOCKCITY_RECEIVE_FLOWER_HEADER__

#include "GUI/gui_layout.h"

namespace LORD {
	class GUIListBox;
}

namespace BLOCKMAN
{
	class ThumbUpRecord;

	class gui_blockCityReceiveFlower : public gui_layout
	{
	public:
		gui_blockCityReceiveFlower();
		~gui_blockCityReceiveFlower();

		void onLoad() override;
		void onShow() override;
		bool isNeedLoad() override;

		bool onClose(const EventArgs& events);
		bool onUpdateThumbUpRecord(const vector<ThumbUpRecord>::type& records);
		bool onSuccessDeleteThumbUpRecord();

	private:
		GUIListBox* m_list = nullptr;
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
