#pragma once
#ifndef __GUI_SKYBLOCK_PRIVILEGE_SHOP_ITEM_HEADER__
#define __GUI_SKYBLOCK_PRIVILEGE_SHOP_ITEM_HEADER__

#include "GUI/CustomGuiWindow.h"
#include "GUI/gui_renderItems.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "Setting/SkyBlockPrivilegeShopSetting.h"
#include "Util/SubscriptionGuard.h"



using namespace LORD;
namespace BLOCKMAN
{
	class GuiSkyBlockPrivilegeShopItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_item_icon = nullptr;

	public:
		SkyBlockPrivilegeGoods m_goods;
		SubscriptionGuard m_subscriptionGuard;


		GUIStaticImage* m_select_img = nullptr;
		GUIStaticText* m_pri_name = nullptr;
		GUIStaticText* m_money_num = nullptr;
		GUIStaticImage* m_money_icon = nullptr;

		GUILayout* m_discount_panel = nullptr;
		GUIStaticText* m_discount_text = nullptr;


		int m_index = 1;

	private:
		void removeComponents();

	public:
		GuiSkyBlockPrivilegeShopItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiSkyBlockPrivilegeShopItem() = default;
		void Destroy() override;
		bool onButtonClick(const EventArgs & events);
		void setGoods(SkyBlockPrivilegeGoods goods, int groupIndex);
	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
