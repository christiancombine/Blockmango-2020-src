#pragma once
#ifndef __GUI_APP_SHOP_ITEM_HEADER__
#define __GUI_APP_SHOP_ITEM_HEADER__

#include "GUI/CustomGuiWindow.h"
#include "GUI/gui_renderItems.h"
#include "World/Shop.h"


using namespace LORD;
namespace LORD {
	class GUILayout;
	class GUIStaticText;
	class GUIStaticImage;
	class GUIButton;
}

namespace BLOCKMAN
{
	class GuiAppShopItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_itemName = nullptr;
		GUILayout* m_quota = nullptr;
		GUIStaticText* m_quotaValue = nullptr;
		GUIStaticText* m_itemNum = nullptr;
		GUIStaticImage* m_itemIcon = nullptr;
		GUIButton* m_btnBuy = nullptr;
		GUIStaticImage* m_currencyIcon = nullptr;
		GUIButton* m_btnVideo = nullptr;
		Goods m_goods;

		int m_videoAdType = 0;
		int m_oldItemId = -2;
		int m_oldItemDamage = -2;
		int m_groupIndex = -1;

		bool m_canShowAd = false;
		bool m_haveAd = false;

	public:
		GuiAppShopItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiAppShopItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setGoods(Goods goods, int groupIndex);
		void setVideoAdInfo(int adType, bool visible);
		int getGoodsItemId() { return m_goods.itemId; };
		void setCanShowAd(bool canShowAd);

	private:
		void removeComponents();
		void updateIcon(int itemId, int damage, String icon);
		bool onButtonClick(const EventArgs & events);
		bool onVideoAdClick(const EventArgs & events);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif