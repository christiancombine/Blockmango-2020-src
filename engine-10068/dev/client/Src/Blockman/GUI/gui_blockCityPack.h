#ifndef __GUI_BLOCKCITY_PACK_HEADER__
#define __GUI_BLOCKCITY_PACK_HEADER__

#include "GUI/gui_layout.h"
#include "Game/BlockCity.h"
#include "GUI/GUIWindowMoveAnimator.h"

namespace LORD {
	class GUIStaticText;
	class GUIStaticImage;
	class GUIRadioButton;
	class GUILayout;
	class GUIGridView;
	class GuiSlotTable;
	class GUIButton;
}

namespace BLOCKMAN
{
	class gui_blockCityPack : public gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			DEL = 1,
			CHANGE_STATUS = 2,
			TRANSFER = 3,
		};

	public:

		gui_blockCityPack();
		~gui_blockCityPack();


		bool onItemStackTouchUp(const EventArgs& events);
 		void onLoad() override;
 		void onShow() override;
 		void onUpdate(ui32 nTimeElapse) override;
 		bool isNeedLoad() override;
 		bool isForceUpdate() override { return true; }
		UVector2 getItemPosition() override;
 		bool onClick(const EventArgs& events, ViewId viewId);
 		bool onDataChange();
 
 	private:
		GUILayout* m_labs = nullptr;
		GUIGridView* m_items = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIButton* m_status = nullptr;
		GuiSlotTable* m_hotbar = nullptr;
		GUIStaticText* m_timedTip = nullptr;
		GUIStaticImage* m_transferEffect = nullptr;

		map<i32, GUIRadioButton*>::type m_radioMap;
		map<i32, i32>::type m_tabMap;
		BlockCityHouseShopItem m_curItem;
		BlockCityHouseShopItem m_needTakeItem;
		BlockCityHouseShop m_curTab;
 		bool m_isInventoryChanged = false;
 		bool m_dataChange = false; 
 		i32 m_curTabId = -1;
 		ItemExchangeAnimator m_itemAnimator;
		i32 m_limitItemLeftTime = 0;
		bool m_haveData = false;
		i32 m_dataChangeTime = 0;
		i32 m_transferEffectTime = 0;

		static i32 replaceIndex;
 
	private:
		void onPackUpdate();
 		bool onRadioChange(const EventArgs & events, BlockCityHouseShopClassify tool);
 		bool onItemClick(const BlockCityHouseShopItem & item);
 		bool onOperation(i32 hotbarIndex, i32 itemId, i32 meta, bool isEditMode);
		void takeItem(const UVector2& pos);
 		void dropItem(const UVector2& pos);
 		void autoTake();
		String timeFormat(i32 time);
		void checkTransferEffect(i32 nTimeElapse);
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
