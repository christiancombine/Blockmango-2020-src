#pragma once
#ifndef __GUI_SKYBLOCK_APP_SHOP_ITEM_HEADER__
#define __GUI_SKYBLOCK_APP_SHOP_ITEM_HEADER__

#include "GUI/CustomGuiWindow.h"
#include "GUI/gui_renderItems.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "Setting/SkyBlockAppShopSetting.h"
#include "Util/SubscriptionGuard.h"



using namespace LORD;
namespace BLOCKMAN
{
	class GuiSkyBlockAppShopItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_item_icon = nullptr;
		GUIStaticImage* m_lock_img = nullptr;

	public:
		SkyBlockGoods m_goods;
		SubscriptionGuard m_subscriptionGuard;


		GUIStaticImage* m_select_img = nullptr;
		GUIStaticImage* m_have_img = nullptr;
		GUIStaticImage* m_hot_img = nullptr;

		GUIStaticText* m_hot_text = nullptr;
		GUIStaticText* m_have_text = nullptr;

		int m_oldItemId = -2;
		int m_oldItemDamage = -2;
		int m_groupIndex = -1;

		int m_cur_level = 0;
		int m_index = 1;

	private:
		void removeComponents();

	public:
		GuiSkyBlockAppShopItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiSkyBlockAppShopItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onButtonClick(const EventArgs & events);
		void setGoods(SkyBlockGoods goods, int groupIndex);
		void setIndex(int index);
	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
