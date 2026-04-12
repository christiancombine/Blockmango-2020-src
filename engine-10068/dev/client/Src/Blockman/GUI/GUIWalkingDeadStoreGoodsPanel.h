#pragma once
#ifndef __GUI_WALKING_DEAD_STORE_GOODS_PANEL_HEADER__
#define __GUI_WALKING_DEAD_STORE_GOODS_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIGridView.h"
#include "Setting/WalkingDeadStoreSetting.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIWalkingDeadStoreGoodsPanel : public gui_layout
	{
	private:
		int m_cur_tab_id = 0;
		int m_goods_id = 0;
		GUIGridView* m_goods_list = nullptr;

 		GUIStaticImage* m_goodsInfoImage = nullptr;
 		GUIStaticImage* m_goodsPriceIcon = nullptr;
 		GUIStaticText* m_goodsInfoName = nullptr;
 		GUIStaticText* m_goodsInfoDesc = nullptr;
 		GUIButton* m_goodsBuy = nullptr;
 		GUIStaticText* m_goodsPrice = nullptr;


		SubscriptionGuard m_subscriptionGuard;
	public:
		GUIWalkingDeadStoreGoodsPanel();
		~GUIWalkingDeadStoreGoodsPanel();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void updateGoodsByTabId(int tabId);
	    bool SyncGoodsInfo(int goodsId);
	private:
		void initGoodsList();


		void initGoodsInfo();
		bool onButtonClick(const EventArgs & events);
		
	};
}

#endif
