#pragma once
#ifndef _WALKINGDEAD_STORE_SUPPLY_POPUP_HEADER_
#define _WALKINGDEAD_STORE_SUPPLY_POPUP_HEADER_

#include "gui_layout.h"
#include "RootGuiLayout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUIGridView.h"


using namespace LORD;

namespace BLOCKMAN
{
	class GUIWalkingDeadStoreSupplyPopup : public gui_layout
	{
	private:
		int m_cur_supply_id = 0;
		GUIGridView* m_supply_goods_list = nullptr;
		GUILayout* m_content = nullptr;
		GUIStaticText* m_title = nullptr;
		GUIButton* m_close_button = nullptr;
		GUIStaticImage* m_box_image = nullptr;
	public:
		GUIWalkingDeadStoreSupplyPopup();
		~GUIWalkingDeadStoreSupplyPopup();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		bool isForceUpdate() { return true; }

		void updateSupplyItemBySupplyId(int supplyId);
	private:

		void initSupplyGoodsList();
		bool showSupplyPopup(int supplyId);
		bool onCloseWalkingDeadStoreSupplyPopup(const EventArgs & events);
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}

#endif