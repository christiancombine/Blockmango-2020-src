#ifndef __GUI_ARMOR_PANEL_CONTROL_HEADER__
#define __GUI_ARMOR_PANEL_CONTROL_HEADER__

#include "Core.h"
#include "GUI/gui_layout.h"
#include "GUI/GuiScrollableInventory.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIRadioButton.h"
#include "Inventory/InventoryProxy.h"

namespace BLOCKMAN
{
	class IInventory;
	class GuiActorWindow;
}
namespace BLOCKMAN
{
	class BuildInventory : public InventoryProxy
	{
	private:
		bool checkItem(i32 slot);
		bool isBuilding(i32 itemId);

	public:
		BuildInventory(IInventory* inventory)
			: InventoryProxy(inventory) {}
		int getSizeInventory() override;
		ItemStackPtr getStackInSlot(int slot) override;
		ItemStackPtr decrStackSize(int slot, int count) override;
		ItemStackPtr getStackInSlotOnClosing(int slot) override;
		void setInventorySlotContents(int slot, ItemStackPtr pStack) override;
		bool isItemValidForSlot(int slot, ItemStackPtr pStack) override;
	};

	class gui_blockFortPackInventory : public gui_layout
	{
	private:
		enum class ViewId
		{
			ALL,
			BUILDING,
			ARMOR,
		};

	public:
		gui_blockFortPackInventory();
		~gui_blockFortPackInventory();
		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32) override;
		bool equipArmor(const EventArgs& event);
		bool unequipArmor(const EventArgs& event);
		bool isNeedLoad() override;

	private:
		bool onItemStackTouchUp(const EventArgs& event);
		bool onRadioChange(const EventArgs& events, const ViewId viewId);
		bool updateDefense(int defense, int maxDefense);
		void updateData();

	private:

		int m_selectionIndex = -1;
		ViewId m_showViewId = ViewId::ALL;
		static const ViewId ALL_VIEW_ID[];
		map<ViewId, GUIRadioButton*>::type m_radioMap;

		GuiScrollableInventory* m_guiScrollableInventory = nullptr;
		GuiSlotTable* m_equipedArmorTable = nullptr;
		GuiActorWindow* m_actorWindow = nullptr;
		GUIStaticText* m_actorTitleName = nullptr;
		GUIStaticText* m_tvItemName = nullptr;
		GUIStaticText* m_tvItemDesc = nullptr;
		GUIStaticImage* m_ivItemIcon = nullptr;
		GUIProgressBar* m_pbDefence = nullptr;

		IInventory* m_buildingInventory = nullptr;
	};
}

#endif // !__GUI_MAIN_INVENTORY_PANEL_HEADER__

