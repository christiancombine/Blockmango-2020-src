#pragma once
#ifndef __GUI_SKYBLOCK_PRIVILEGE_SHOP_HEADER__
#define __GUI_SKYBLOCK_PRIVILEGE_SHOP_HEADER__
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIGridView.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIRadioButton.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/SkyBlockPrivilegeShopSetting.h"
#include "UI/GUIStaticImage.h"
#include "GuiSkyBlockSupemePrivilege.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_skyBlockPrivilegeShop : public  gui_layout
	{
	private:
		GuiSkyBlockSupemePrivilege* m_skyblock_supeme_privilege = nullptr;
		GUILayout* m_tabLayout = nullptr;
		GUILayout* m_normalLayout = nullptr;

		GUILayout* m_itemListLayout = nullptr;
		GUIGridView* m_itemGridView = nullptr;

		GUILayout* m_rewardListLayout = nullptr;
		GUIGridView* m_rewardGridView = nullptr;

		SubscriptionGuard m_subscriptionGuard;

		//GUIStaticText* m_privilege_name = nullptr;
		GUIStaticText* m_item_name = nullptr;
		GUIStaticText* m_item_des = nullptr;
		GUIStaticText* m_buy_text = nullptr;
		GUIStaticImage* m_normal_title = nullptr;
		GUIStaticImage* m_supeme_title = nullptr;
		GUIButton* m_buy_btn = nullptr;
		ui32 m_showTipTime = 0;
		String m_tipMessage = "";

	public:
		gui_skyBlockPrivilegeShop();
		~gui_skyBlockPrivilegeShop();

		enum ClickType
		{
			CLOSE_PANEL = 0,
			BUY_ITEM,
		};

		enum OperateType
		{
			BUY_GOOD,
			UPDATE_DATA,
		};

		bool isForceUpdate() { return true; }
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onRadioChange(const EventArgs & events, int groupId, GUIRadioButton* radio);
		bool onButtonClick(const EventArgs & events, ClickType clickType);
		bool onBuyGoodsResult(const String& msg);
		bool updateShopInfo(int srotType, int level, const String& priData);
		bool updateShopDetail(const SkyBlockPrivilegeGoods goodData, int index);
		bool updateShopItemDetail(const SkyBlockPrivilegeReward item, int index);
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

		void initTabsAndPanels();
		int m_cur_select = SKYBLOCK_PRI_SUPREME;

		SkyBlockPrivilegeGoods m_good;
		map<int, GUILayout*>::type m_tab_group;

	private:
		void addTabView(SkyBlockPrivilegeGoodGroup group, int index);
		//void updataShopSort(GUIGridView* gridView, SkyBlockPrivilegeGoodGroup group, int index);
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif