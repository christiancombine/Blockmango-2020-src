#ifndef __GUI_BLOCKCITY_RANK_HEADER__
#define __GUI_BLOCKCITY_RANK_HEADER__

#include "GUI/gui_layout.h"

namespace LORD {
	class GUIRadioButton;
}

namespace BLOCKMAN
{
	class gui_blockCityRank : public gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			FRIEND,
			GLOBAL,
			FLOWER,
			AWARD,
		};

	public:
		gui_blockCityRank();
		~gui_blockCityRank();

		void onLoad() override;
		void onShow() override;
		bool isNeedLoad() override;
		bool onClick(const EventArgs& events, ViewId viewId);
		bool isShowCharmButton(bool isShow);

	private:
		map<ViewId, gui_layout*>::type m_radioPanelMap;
		map<ViewId, GUIRadioButton*>::type m_radioTabMap;

	private:
		bool onRadioChange(const EventArgs & events, ViewId viewId);
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
