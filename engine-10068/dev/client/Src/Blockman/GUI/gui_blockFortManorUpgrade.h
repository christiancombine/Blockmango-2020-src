#ifndef __GUI_BLOCKFORT_MANOR_UPGRADE_HEADER__
#define __GUI_BLOCKFORT_MANOR_UPGRADE_HEADER__

#include "GUI/gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"

namespace BLOCKMAN
{
	class gui_blockFortManorUpgrade : public gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			BUY
		};

	public:
		gui_blockFortManorUpgrade();
		~gui_blockFortManorUpgrade();

		void onLoad() override;
		void onShow() override;
		bool isNeedLoad() override;
		bool onClick(const EventArgs& events, ViewId viewId);
		bool onDataChange();
		void onSetData();

	private:
		GUIStaticText* m_curManorLevel = nullptr;
		GUIStaticText* m_nextManorLevel = nullptr;
		GUIStaticImage* m_curManorImage = nullptr;
		GUIStaticImage* m_nextManorImage = nullptr;
		GUIStaticText* m_curManorArea = nullptr;
		GUIStaticText* m_nextManorArea = nullptr;
		GUIListHorizontalBox* m_unlockItems = nullptr;
		GUIStaticText* m_price = nullptr;
		GUIStaticImage* m_currency = nullptr;

		GUILayout* m_curManorLayout = nullptr;
		GUILayout* m_nextManorLayout = nullptr;
		GUILayout* m_unlockItemsLayout = nullptr;
		GUIStaticImage* m_arrow = nullptr;
		GUIStaticText* m_maxLevel = nullptr;

		bool m_dataChange = false;
		bool m_canBuy = false;
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
