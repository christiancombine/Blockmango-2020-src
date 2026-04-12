#ifndef __GUI_ARMOR_PANEL_CONTROL_HEADER__
#define __GUI_ARMOR_PANEL_CONTROL_HEADER__

#include "Core.h"
#include "GUI/gui_layout.h"
#include "UI/GUIStaticImage.h"
#include "Item/ItemStack.h"
#include "UI/GUICheckBox.h"

namespace LORD {
	class GUIButton;
	class GUIEditBox;
	class GUIProgressBar;
	class GUILayout;
	class GUIStaticText;
	class GUIRadioButton;
	class GUIListHorizontalBox;
}

namespace BLOCKMAN
{
	class GuiActorWindow;
	class GuiScrollableInventory;
	class GuiItemSlot;
	class GuiSlotTable;
	class gui_armorPanelControl : public gui_layout
	{
	private:
		enum class ViewId
		{
			ALL,
			ARMOR,
			MATERIALS,
			CUSTOM,
		};

	public:
		gui_armorPanelControl();
		~gui_armorPanelControl();
		void onLoad() override;
		void onShow() override;
		void onAttach() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool equipArmor(ItemStackPtr itemStack);
		bool unequipArmor(const EventArgs& event);
		void sellItem();

	private:
		bool onClickEnderChest(const EventArgs& events);
		bool onItemStackTouchUp(const EventArgs& event);
		bool onRadioChange(const EventArgs& events, const ViewId viewId, int tabId);
		bool onRemoveCurrentItem();
		void updateData();
		void updateArmorUI();
		bool refreshSellSelectItem();
		bool onInputChanged(const EventArgs & events);
		bool onButtonNumClick(const EventArgs & events, bool add);
		bool onSellClick(const EventArgs & events);
		bool onAbandonClick(const EventArgs & events);
		void updateIconDefense(int value);
		bool onDressSwitch(const EventArgs & events);

	private:

		int m_selectionIndex = -1;
		// ItemStackPtr m_selectItemStack = nullptr;
		ViewId m_showViewId = ViewId::ALL;
		int m_customTabId = 0;
		static const ViewId ALL_VIEW_ID[];
		bool m_isCraftAllSwitch = false;
		ui32 m_showTipTime = 0;
		String m_tipMessage = "";
		map<int, GUIRadioButton*>::type m_radioMap;

		GuiScrollableInventory* m_guiScrollableInventory = nullptr;
		GuiSlotTable* m_equipedArmorTable = nullptr;
		GuiActorWindow* m_actorWindow = nullptr;
		GUIStaticText* m_actorTitleName = nullptr;
		GUIStaticText* m_actorInventoryName = nullptr;
		GuiItemSlot* m_sellSlot = nullptr;
		GUIStaticText* m_sell_name = nullptr;
		GUIButton* m_sellBtn = nullptr;
		GUIButton* m_sub_btn = nullptr;
		GUIButton* m_add_btn = nullptr;
		GUIButton* m_abandonBtn = nullptr;
		GUIEditBox* m_sell_input_num = nullptr;
		GUIStaticText* m_sell_input_show_num = nullptr;
		GUIStaticText* m_sell_sum_num_txt = nullptr;
		GUIStaticText* m_sell_item_desc = nullptr;
		int m_cur_sell_num = 1;
		int m_cur_sell_money = 0;
		GUILayout* m_attrLayout = nullptr;
		GUICheckBox* m_dressBox = nullptr;
		GUIStaticText* m_textTip = nullptr;
		GUIStaticImage* m_dressBg = nullptr;
		GUIStaticText* m_defence_num = nullptr;
		GUIStaticText* m_attack_num = nullptr;
		GUIStaticText* m_hp_num = nullptr;
		GUIButton* m_enderChest = nullptr;
		GUIStaticImage* m_dressEffect1 = nullptr;
		GUIStaticImage* m_dressEffect2 = nullptr;
		GUIStaticText* m_dress_left = nullptr;
		GUIStaticText* m_dress_right = nullptr;

		GUIListHorizontalBox* m_ic_defence = nullptr;
	};
}

#endif // !__GUI_MAIN_INVENTORY_PANEL_HEADER__

