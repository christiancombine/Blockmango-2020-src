#pragma once
#ifndef __GUI_APP_SHOP_HEADER__
#define __GUI_APP_SHOP_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "World/Shop.h"

using namespace LORD;
namespace LORD {
	class GUILayout;
	class GUIGridView;
	class GUIStaticText;
	class GUIRadioButton;
}

namespace BLOCKMAN
{
	class RootGuiLayout;
	class gui_appShop : public  gui_layout
	{
	private:
		GUILayout * m_tabLayout = nullptr;
		GUILayout* m_itemListLayout = nullptr;
		GUIGridView* m_itemGridView = nullptr;
		GUIStaticText* m_textTip = nullptr;
		GUIRadioButton* m_selectTab = nullptr;
		SubscriptionGuard m_subscriptionGuard;

		ui32 m_showTipTime = 0;
		String m_tipMessage = "";

		bool m_canShowAd = false;
		bool m_isVisibleAd = false;
		int m_shopAdTimes = -1;
		int m_shopAdTime = -1;

	public:
		gui_appShop();
		~gui_appShop();
		bool isForceUpdate() { return true; }
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onRadioChange(const EventArgs & events, GoodsGroup group, GUIRadioButton* radio);
		bool onButtonClick(const EventArgs & events);
		bool onLackOfMoney(const EventArgs & events);
		bool onShopUpdate();
		bool onBuyGoodsResult(const String& msg);
		void showPage(int page);
		void onGetCommonData(String key, String data);
		void onShow();

	private:
		void addTabView( GoodsGroup group, int index);
		bool onSyncShopAdInfo(int times, int seconds);
		bool onVideoAdVisible(bool visible);

		RootGuiLayout* getParent();
	};

}
#endif