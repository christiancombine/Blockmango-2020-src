#ifndef __GUI_BLOCK_FORT_PACK_HEADER__
#define __GUI_BLOCK_FORT_PACK_HEADER__

#include "GUI/gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIRadioButton.h"

#include "GUI/GuiSlotTable.h"

namespace BLOCKMAN
{
	class gui_blockFortPack : public gui_layout
	{
	public:
		enum class TabId
		{
			INVENTORY
		};

		gui_blockFortPack();
		~gui_blockFortPack();

		void showTab(TabId tab);

		bool onItemStackTouchUp(const EventArgs& events);
		void onLoad() override;
		void onShow() override;
		void onHide() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad();

	private:
		map<TabId, GUIRadioButton*>::type m_tabs;
		map<TabId, gui_layout*>::type m_panels;
		GuiSlotTable* m_hotbar = nullptr;
		TabId m_selectedTab = TabId::INVENTORY;

		bool onClose(const EventArgs& events);
		bool onDeleteHotbar(const EventArgs& events);
		bool onRadioChanged(const EventArgs & events, TabId tabId);
		bool m_isInventoryChanged = false;
		SubscriptionGuard m_subscriptionGuard;
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
