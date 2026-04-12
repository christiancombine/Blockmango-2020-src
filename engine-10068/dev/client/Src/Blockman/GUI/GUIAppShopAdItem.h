#pragma once
#ifndef __GUI_APP_SHOP_AD_ITEM_HEADER__
#define __GUI_APP_SHOP_AD_ITEM_HEADER__

#include "GUI/CustomGuiWindow.h"

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
	class GUIAppShopAdItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_itemName = nullptr;
		GUIStaticText* m_itemNum = nullptr;
		GUIStaticImage* m_itemIcon = nullptr;
		GUIButton* m_showAd = nullptr;
		GUIStaticImage* m_adIcon = nullptr;

		int m_vedioAdTimes = 0;
		int m_vedioAdTime = 0;

	private:
		void removeComponents();

	public:
		GUIAppShopAdItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIAppShopAdItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onButtonClick(const EventArgs & events);
		void setGoods(Goods goods);
		void setAdInfo(int times, int time);
	};

}
#endif
