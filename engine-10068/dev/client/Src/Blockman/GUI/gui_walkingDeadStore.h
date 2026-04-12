#pragma once
#ifndef _WALKINGDEAD_STORE_HEADER_
#define _WALKINGDEAD_STORE_HEADER_

#include "gui_layout.h"
#include "RootGuiLayout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "GUI/GUIWalkingDeadStoreGoodsPanel.h"
#include "GUI/GUIWalkingDeadStoreTab.h"
#include "GUI/GUIWalkingDeadStoreSupplyPanel.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_walkingDeadStore : public gui_layout
	{
	private:
		int m_store_id = 0;

		GUIWalkingDeadStoreGoodsPanel* m_goods_panel = nullptr;
		GUIWalkingDeadStoreSupplyPanel* m_supply_panel = nullptr;


		GUILayout* m_tabs = nullptr;
		GUILayout* m_ren = nullptr;
		GUIStaticText* m_store = nullptr;
		list<GUIWalkingDeadStoreTab*>::type m_store_tabs;

	public:
		gui_walkingDeadStore();
		~gui_walkingDeadStore();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		bool isForceUpdate() { return true; }
		void onGetCommonData(String key, String data);

	private:
		void initTabsUI(list<WalkingDeadStoreTab*>::type tabs);
		bool onCloseWalkingDeadStore(const EventArgs & events);
		bool showStoreById(int storeId);
		bool showStoreByTraveller(const String& data);
		bool onUpdateGoods(int goodsId,int limit);

		bool onWalkingDeadStoreVip(int vip, int64_t vipTime);
		bool onUpdateWalkingDeadSupply(const String & data);
		bool onTabChange(int type);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}













#endif