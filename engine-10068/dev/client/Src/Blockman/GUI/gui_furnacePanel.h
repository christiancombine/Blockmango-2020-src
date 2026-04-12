#pragma once
#ifndef __GUI_FURNACE_PANEL_HEADER__
#define __GUI_FURNACE_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"
#include "GUI/GuiScrollableInventory.h"
#include "GUI/GuiItemSlot.h"
#include <vector>

using namespace LORD;

namespace BLOCKMAN
{
	class gui_furnacePanel : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_furnacePanel();
		virtual ~gui_furnacePanel();
		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32) override;
		bool isNeedLoad() override;

		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

 		bool onCloseClick(const EventArgs & events);

		bool onMaterialClick(const EventArgs & events);
		bool onMaterialLongClickStart(const EventArgs & events);
		bool onMaterialLongClickEnd(const EventArgs & events);

		bool onFuelClick(const EventArgs & events);
		bool onFuelLongClickStart(const EventArgs & events);
		bool onFuelLongClickEnd(const EventArgs & events);

		bool onResultClick(const EventArgs & events);
		bool onResultLongClickStart(const EventArgs & events);
		bool onResultLongClickEnd(const EventArgs & events);

		bool placeMeterial(const EventArgs& events);
		bool onMaterialItemStackLongTouchStart(const EventArgs& events);
		bool onMaterialItemStackLongTouchEnd(const EventArgs& events);

		bool placeFuel(const EventArgs& events);
		bool onFuelItemStackLongTouchStart(const EventArgs& events);
		bool onFuelItemStackLongTouchEnd(const EventArgs& events);

		void updateData(ui32);
		void setInventory(IInventory* inventory);
		void setInventory(IInventory* inventory, const Vector3i & blockPos, int face, const Vector3 & hisPos);
		void exchangeItem(bool isPlayerInventoryToChest, ui32 touchTime, int index, int targetIndex = -1);
		void setBurnTime(int time, int maxTime) {
			m_burnTime = time * 1000;
			m_burnMaxTime = maxTime * 1000;
		}
		ItemStackPtr  getClickItem(IInventory* inventory, const EventArgs& event);

	private:
		const int m_closeFurnaceDis = 10;
		ui32 m_touchTime = 0;
		IInventory* m_furnaceInventory = nullptr;
		int m_face = 0;
		Vector3 m_hisPos;
		Vector3i m_blockPos;
		ItemStackPtr m_selectStackPtr = nullptr;
		int m_burnTime = 0;
		int m_burnMaxTime = 0;

		GuiScrollableInventory* m_guiMeterialScrollableInventory = nullptr;
		GuiScrollableInventory* m_guiFuelScrollableInventory = nullptr;
 		GUIButton* m_closeBtn = nullptr;
		GuiItemSlot* m_materialSlot = nullptr;
		GuiItemSlot* m_fuelSlot = nullptr;
		GuiItemSlot* m_resultSlot = nullptr;
		GUIStaticText* m_burnTimeTxt = nullptr;
		GUIStaticImage* m_fire_img = nullptr;
		GUIProgressBar* m_fire_progress = nullptr;
	};
}

#endif

