#pragma once
#ifndef __GUI_SKYBLOCK_APP_SHOP_HEADER__
#define __GUI_SKYBLOCK_APP_SHOP_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIGridView.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIRadioButton.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/SkyBlockAppShopSetting.h"
#include "UI/GUIEditBox.h"
#include "UI/GUIStaticImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_skyBlockAppShop : public  gui_layout
	{
	private:
		GUILayout* m_tabLayout = nullptr;

		GUILayout* m_itemListLayout = nullptr;
		GUIGridView* m_itemGridView = nullptr;
		GUIStaticText* m_textTip = nullptr;
		SubscriptionGuard m_subscriptionGuard;

		
		GUIStaticText* m_item_name = nullptr;
		GUIStaticText* m_des = nullptr;
		GUIStaticText* m_one_coin = nullptr;
		GUIStaticText* m_num_des = nullptr;
		GUIStaticText* m_num_text = nullptr;
		GUIStaticText* m_total_coin = nullptr;
		GUIStaticText* m_time_text = nullptr;
		GUIStaticText* m_hot_text = nullptr;
		GUIStaticText* m_have_text = nullptr;
		GUIStaticText* m_buy_text = nullptr;
		GUIStaticText* m_lock_text = nullptr;
		GUIButton* m_sub_btn = nullptr;
		GUIButton* m_add_btn = nullptr;
		GUIButton* m_buy_btn = nullptr;
		GUIStaticImage* m_item_icon = nullptr;
		GUIStaticImage* m_coin_img = nullptr;
		GUIStaticImage* m_hot_img = nullptr;
		GUIStaticImage* m_have_img = nullptr;
		GUIStaticImage* m_one_icon = nullptr;
		GUILayout* m_time_bg = nullptr;
		GUIEditBox* m_input_num = nullptr;

		ui32 m_showTipTime = 0;
		String m_tipMessage = "";
		int m_cur_num = 1;

	public:
		gui_skyBlockAppShop();
		~gui_skyBlockAppShop();

		enum ClickType
		{
			CLOSE_PANEL = 0,
			ADD_NUM,
			SUB_NUM,
			BUY_ITEM,
			MAX_NUM,
			MIN_NUM,
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
		bool onLackOfMoney(int msgtype);
		bool onShopUpdate();
		bool onBuyGoodsResult(const String& msg);
		bool updateShopInfo(int srotType, int level, const String& priData);
		bool updateShopDetail(const SkyBlockGoods goodData, int index);
		bool isNeedLoad() override;
		bool changePageToIndex(i32 pageId);

		bool onInputChanged(const EventArgs & events);
		void initTabsAndPanels();

		SkyBlockGoods m_good;

		int m_sortType = 0;
		int m_level = 0;

		long long m_time = 0;
		int m_selectTabId = 0;

		map<int, GUIGridView*>::type m_panel_map;
		map<int, GUIRadioButton*>::type m_shop_tabs;

		map<int, GUILayout*>::type m_tab_group;

	private:
		void addTabView(SkyBlockGoodGroup group, int index);
		void updataTabGroup(SkyBlockGoodGroup group, int index);
		void updataShopSort(GUIGridView* gridView, SkyBlockGoodGroup group, int index);

		bool onTabGroupRadioChange(const EventArgs & events, SkyBlockGoodGroup group, i32 groupId, GUIRadioButton* radio);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif