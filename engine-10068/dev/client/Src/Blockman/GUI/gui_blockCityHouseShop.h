#pragma once
#ifndef __GUI_BLOCKCITY_HOUSE_SHOP_HEADER__
#define __GUI_BLOCKCITY_HOUSE_SHOP_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "Game/BlockCity.h"

using namespace LORD;
namespace LORD {
	class GUIStaticText;
	class GUIStaticImage;
	class GUIButton;

}
namespace BLOCKMAN
{
	class GuiActorWindow;
	class gui_blockCityHouseShop : public  gui_layout
	{
	private:
		enum class ViewId {
			UP,
			NEXT,
			CLOSE,
			BUY
		};

	private:
		GUIStaticText* m_name = nullptr;
		GuiActorWindow* m_actor = nullptr;
		GUIStaticText* m_desc = nullptr;
		GUIStaticText* m_classify = nullptr;
		GUIStaticText* m_price = nullptr;
		GUIButton* m_buy = nullptr;
		GUIStaticImage* m_currencyIcon = nullptr;

		i32 m_itemIndex = 0;
		i32 m_shopClassifyId = 0;
		i32 m_areaId = 0;
		BlockCityHouseShopItem m_shopItem;
		//bool m_canBuy = false;

		BlockCityHouseShopItemInfo m_shopItemInfos;
		static i32 replaceIndex;

	public:
		gui_blockCityHouseShop();
		~gui_blockCityHouseShop();
		void onLoad() override;
		bool isNeedLoad() override;

	private:
		bool onClick(const EventArgs& args, ViewId viewId);
		bool onShopUpdate();
		bool openShop(i32 areaId, i32 itemId, bool isUpdate);
		bool openShopByNpc(const String& msg);
		void changeUI(BlockCityHouseShopItem shopItem, String shopClassify, i32 index);
		i32 findFreeInventoryIndex(i32 itemId);
	};

}
#endif