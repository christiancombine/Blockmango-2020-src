#ifndef __GUI_CRAFTING_PANEL_CONTROL_HEADER__
#define __GUI_CRAFTING_PANEL_CONTROL_HEADER__

#include <memory>
#include "GUI/gui_layout.h"
#include "GUI/GuiScrollableInventory.h"
#include "UI/GUICheckBox.h"

namespace LORD
{
	class GUIButton;
	class GUIStaticText;
}

namespace BLOCKMAN
{
	class CraftingRecipe;
	class gui_playerInventoryControl;

	class gui_craftingPanelControl : public gui_layout
	{
	public:
		gui_craftingPanelControl();
		void onLoad() override;
		void onHide() override;
		void onShow() override;
		void onUpdate(ui32) override;
		bool onItemStackTouchUp(const EventArgs& event);
		bool onCraftButtonClick(const EventArgs& event);
		bool onCheckStateChanged(const EventArgs& event);
		void recipeResetPos();
		inline gui_playerInventoryControl* getParent();

	private:
		size_t m_craftTableSize = 2;
		std::shared_ptr<CraftingRecipe> m_selectedRecipe = nullptr;
		GuiScrollableInventory* m_recipes = nullptr;
		GuiSlotTable* m_craftTable = nullptr;
		// GUIStaticText* m_outputName = nullptr;
		GUIButton* m_craftButton = nullptr;
		GUIStaticText* m_craftButtonText = nullptr;
		GuiItemStack* m_craftButtonIcon = nullptr;
		GUICheckBox* m_craftAllButton = nullptr;
		GUIStaticText* m_switchTxt = nullptr;
		bool m_isCraftAllSwitch = false;

		void updateCraftTableSize(ui32 nTimeElapse);
		void updateCraftTableContent();
		void updateRecipes(ui32 nTimeElapse);
		void updateCraftButton();
		// void updateOutputName();
	};
}

#endif // !__GUI_MAIN_INVENTORY_PANEL_HEADER__

