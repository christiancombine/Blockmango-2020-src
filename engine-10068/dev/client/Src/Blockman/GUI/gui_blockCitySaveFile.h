#ifndef __GUI_BLOCKCITY_SAVE_FILE_HEADER__
#define __GUI_BLOCKCITY_SAVE_FILE_HEADER__

#include "GUI/gui_layout.h"

namespace LORD {
	class GUIListBox;
}

namespace BLOCKMAN
{
	class BlockCityArchiveItem;
	class gui_blockCitySaveFile : public gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			SAVE,
			READ,
			DEL,
		};

	public:
		gui_blockCitySaveFile();
		~gui_blockCitySaveFile();

		void onLoad() override;
		void onShow() override;
		void onHide() override;
		bool isNeedLoad() override;

		bool onClick(const EventArgs& events, ViewId viewId);
		bool onDataChange();
 		void onSetDatas();
		bool onItemClick(i32 index);
		bool tipSure(i32 type);

	private:
		GUIListBox* m_list = nullptr;
		bool m_dataChange = false;
		map<i32, BlockCityArchiveItem>::type m_mapItem;
		i32 m_curItemIndex = -1;
		bool m_isSureDelete = false;
		bool m_isSureSave = false;
		bool m_isSureRead = false;
		i32 m_nextPrice = 0;
		i32 m_maxNum = 0;
		i32 m_curNum = 0;
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
