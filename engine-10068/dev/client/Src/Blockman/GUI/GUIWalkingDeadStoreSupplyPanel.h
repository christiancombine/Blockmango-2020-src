#pragma once
#ifndef __GUI_WALKING_DEAD_STORE_SUPPLY_PANEL_HEADER__
#define __GUI_WALKING_DEAD_STORE_SUPPLY_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIGridView.h"
#include "Setting/WalkingDeadStoreSetting.h"
using namespace LORD;

namespace BLOCKMAN
{
	class GUIWalkingDeadStoreSupplyPanel : public gui_layout
	{
	private:
		int m_cur_tab_id = 0;
		GUIGridView* m_supply_list = nullptr;

	public:
		GUIWalkingDeadStoreSupplyPanel();
		~GUIWalkingDeadStoreSupplyPanel();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void updateSupplyByTabId();

	private:
		void initGoodsList();
	};
}

#endif
