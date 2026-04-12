#ifndef __GUI_BLOCKCITY_STORE_HEADER__
#define __GUI_BLOCKCITY_STORE_HEADER__

#include "GUI/gui_layout.h"
#include "Game/BlockCity.h"
#include "GUI/GUIWindowMoveAnimator.h"

namespace LORD {
	class GUIStaticText;
	class GUIStaticImage;
	class GUIRadioButton;
	class GUILayout;
}

namespace BLOCKMAN
{
	class GuiSlotTable;
	class gui_blockCityStore : public gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			PARPER = 1,
			BLOCK = 2,
			FURNITURE = 3,
			BUY_ONE = 4,
			BUY_GROUP = 5,
			DEL = 6,
			MAIN = 7,
		};

	public:

		gui_blockCityStore();
		~gui_blockCityStore();


		bool onItemStackTouchUp(const EventArgs& events);
		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		bool isForceUpdate() override { return true; }
		bool onClick(const EventArgs& events, ViewId viewId);
		bool onDataChange();

	private:
		map<ViewId, gui_layout*>::type m_radioPanelMap;

	private:
		map<ViewId, GUIRadioButton*>::type m_radioColTabs;

		GUIStaticImage* m_itemIcon = nullptr;
		GUIStaticText* m_tvItemName = nullptr;
		GUIStaticText* m_tvItemDescVal = nullptr;
		GUIStaticText* m_tvItemScore = nullptr;
		GUIStaticText* m_tvBuyOne = nullptr;
		GUIStaticText* m_tvBuyGroup = nullptr;
		GUIStaticImage* m_currencyBuyOne = nullptr;
		GUIStaticImage* m_currencyBuyGroup = nullptr;
		GuiSlotTable* m_hotbar = nullptr;
		GUIStaticText* m_tvItemInteractVal = nullptr;
		GUIStaticText* m_tvItemInteractName = nullptr;
		GUILayout* m_tvItemInteractLayout = nullptr;
		GUIStaticText* m_tvItemDescName = nullptr;

		BlockCityStoreItem m_curItem;
		BlockCityStoreItem m_needTakeItem;
		bool m_isInventoryChanged = false;
		bool m_dataChange = false;
		//bool m_canBuy = false;

		ViewId m_curTabId = ViewId::PARPER;
		ItemExchangeAnimator m_itemAnimator;

	private:
		bool onCloseInventory(const EventArgs& events);
		bool onRadioChange(const EventArgs & events, ViewId viewId);
		bool onItemClick(const BlockCityStoreItem & item);
		bool onOperation(i32 hotbarIndex, i32 itemId, i32 meta, bool isEditMode);
		void takeItem(const UVector2& pos);
		void dropItem(const UVector2& pos);
		void autoTake();

	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
