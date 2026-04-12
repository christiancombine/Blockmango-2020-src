#ifndef __GUI_PLAYER_INVENTORY_CONTROL_HEADER__
#define __GUI_PLAYER_INVENTORY_CONTROL_HEADER__

#include "GUI/gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUICheckBox.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiSlotTable.h"

namespace BLOCKMAN
{
	class gui_playerInventoryControl : public gui_layout
	{
	public:
		enum InventoryTabId
		{
			ARMOR,
            CRAFTING_TYPE1,
            CRAFTING_TYPE2,
            CRAFTING_TYPE3,
            CRAFTING_TYPE4,
            CRAFTING_TYPE5,
			CRAFTING,
			DESIGNATION,
			MAIN_INVENTORY,
		};
		static const InventoryTabId ALL_INVENTORY_TAB_ID[];

		gui_playerInventoryControl();
		~gui_playerInventoryControl();

		void showTab(int tab);

		bool onItemStackTouchUp(const EventArgs& events);
		void onLoad() override;
		void onShow() override;
		void onHide() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		bool isInCraftingTable()
		{
			return m_isInCraftingTable;
		}
		void setInCraftingTable(bool isInCraftingTable);
		void notifyUpdateCraft();
		bool isCraftingTabSelected()
		{
			return m_selectedTab == InventoryTabId::CRAFTING;
		}

		void armorPanelSellItem();

	private:

		map<int, GUIStaticText*>::type m_radioNameMap;
		map<int, GUIRadioButton*>::type m_tabs;
		map<int, gui_layout*>::type m_panels;
		GuiSlotTable* m_hotbar = nullptr;
		int m_selectedTab = InventoryTabId::ARMOR;
		bool m_isInCraftingTable = false;
		bool m_isCraftAllSwitch = true;

		bool onClickOutside(const EventArgs& events);
		bool onCloseInventory(const EventArgs& events);
		bool onClick(const EventArgs& events);
		bool onRadioStateChanged(const EventArgs & events, int tabId);
		bool onCraftAllSwitchChanged(bool all_switch);
		bool m_isInventoryChanged = false;
		SubscriptionGuard m_subscriptionGuard;
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
