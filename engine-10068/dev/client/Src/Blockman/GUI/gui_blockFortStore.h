#ifndef __GUI_BLOCK_FORT_STORE_HEADER__
#define __GUI_BLOCK_FORT_STORE_HEADER__

#include "GUI/gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "UI/GUIListBox.h"
#include "UI/GUIGridView.h"
#include "UI/GUISystem.h"
#include "UI/GUIListHorizontalBox.h"
#include "GUI/GuiSlotTable.h"

#include "GUI/GUIWindowMoveAnimator.h"
#include "GUI/GuiCommonTabGridView.h"
#include "UI/GUIRadioButton.h"
#include "Game/BlockFort.h"
namespace BLOCKMAN
{
	class BlockFortItemInfo;
};

namespace BLOCKMAN
{
	class gui_blockFortStore : public gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			BUY,
			MAIN,
		};

	public:

		gui_blockFortStore();
		~gui_blockFortStore();

		void onLoad() override;
		void onShow() override;
		bool isNeedLoad() override;
		bool onClick(const EventArgs& events, ViewId viewId);
		bool onDataChange(i32 tabId);

	private:
		map<i32, GuiCommonTabGridView*>::type m_radioPanelMap;
		map<i32, GUIRadioButton*>::type m_radioTabs;

		GUILayout* m_tabs = nullptr;
		GUIStaticImage* m_ivIcon = nullptr;
		GUIButton* m_tvName = nullptr;
		GUIStaticText* m_tvDesc = nullptr;
		GUIListBox* m_lvInfos = nullptr;
		GUIButton* m_btnBuy = nullptr;
		GUIStaticText* m_tvMoneyValue = nullptr;
		GUIStaticImage* m_ivMoneyIcon = nullptr;

		BlockFortStoreItem m_curItem;
		bool m_dataChange = false;

		i32 m_curTabId = -1;
		i32 m_curGroupId = -1;
		i32 m_changeTabId = -1;

	private:
		void initGridView();
		void initTabView();

		void initGridViewTab(i32 tabId);
		void initGridViewItems(i32 tabId, i32 groupId);
		void initItemInfo(const vector<BlockFortItemInfo>::type& items);
		bool onRadioChange(const EventArgs & events, i32 tabId);
		bool onGroupRadioChange(const EventArgs & events, GUIRadioButton* view, i32 tabId, i32 groupId);
		bool onItemClick(const EventArgs & events, i32 tabId, i32 groupId, const BlockFortStoreItem & item);

	};
}

#endif // !__GUI_BLOCK_FORT_STORE_HEADER__
