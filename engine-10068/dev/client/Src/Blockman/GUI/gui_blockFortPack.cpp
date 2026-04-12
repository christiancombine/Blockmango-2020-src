#include "gui_blockFortPack.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiItemStack.h"
#include "GUI/GuiSlotTable.h"
#include "GUI/gui_mainInventoryPanelControl.h"
#include "GUI/gui_blockFortPackInventory.h"
#include "Inventory/InventoryPlayer.h"
#include "game.h"
#include "GUI/RootGuiLayout.h"

namespace BLOCKMAN
{

	gui_blockFortPack::gui_blockFortPack()
		: gui_layout("BlockFortPack.json")
	{
	}

	gui_blockFortPack::~gui_blockFortPack()
	{
		for (auto& view : m_panels)
		{
			LordSafeDelete(view.second);
		}
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_blockFortPack::onLoad()
	{
		setText("BlockFortPack-Del-Shortcut-Name", "gui_blockcity_delete");
		m_hotbar = getWindow<GuiSlotTable>("BlockFortPack-Hotbar-SlotTable");
		m_hotbar->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_blockFortPack::onItemStackTouchUp, this));
		getWindow("BlockFortPack-ToggleInventoryButton")->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&gui_blockFortPack::onClose, this));
		getWindow("BlockFortPack-Close")->subscribeEvent(EventButtonClick,SubscriberSlot(&gui_blockFortPack::onClose, this));
		getWindow("BlockFortPack-Del-Shortcut")->subscribeEvent(EventButtonClick, SubscriberSlot(&gui_blockFortPack::onDeleteHotbar, this));

		setText("BlockFortPack-Title", "role_pack_back");
		setText("BlockFortPack-Tab-Inventory", "role_pack_back");

		m_tabs[TabId::INVENTORY] = getWindow<GUIRadioButton>("BlockFortPack-Tab-Inventory");
		m_panels[TabId::INVENTORY] = m_panels[TabId::INVENTORY] ? m_panels[TabId::INVENTORY] : LordNew gui_blockFortPackInventory;

		auto pInventoryPanel = getWindow("BlockFortPack-Panel");
		for (auto& view : m_panels)
		{
			view.second->attachTo(this, pInventoryPanel);
			m_tabs[view.first]->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockFortPack::onRadioChanged, this, std::placeholders::_1, view.first));
		}

		m_subscriptionGuard.add(Blockman::Instance()->m_pPlayer->inventory->onInventoryContentSync([this] {
			m_isInventoryChanged = true;
		}));

		m_tabs[TabId::INVENTORY]->SetSelected(true);
	}

	void gui_blockFortPack::onShow()
	{
		
	}

	void gui_blockFortPack::onHide()
	{
	}

	void gui_blockFortPack::onUpdate(ui32 nTimeElapse)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		for (int i = 0; i < 9; ++i)
		{
			m_hotbar->setItemStack(0, i, inventory->hotbar[i]);
		}
		m_hotbar->selectItemSlot(inventory->currentItemIndex);

		if (m_isInventoryChanged) {
			Blockman::Instance()->m_pPlayer->inventory->notifyUpdateToProxy();
			m_isInventoryChanged = false;
		}
	}

	bool gui_blockFortPack::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockFort);
	}

	bool gui_blockFortPack::onClose(const EventArgs& events)
	{
		RootGuiLayout::Instance()->showMainControl();
		//(dynamic_cast<gui_mainInventoryPanelControl*>(m_panels[TabId::MAIN_INVENTORY]))->resetSelection();
		//playSonud("random.click");
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockFortPack::onRadioChanged(const EventArgs & events, TabId tabId)
	{
		if (m_tabs[tabId] && m_tabs[tabId]->IsSelected())
		{
			for (auto& view : m_panels)
			{
				if (view.first == tabId )
				{
					m_panels[tabId]->show();
				}
				else
				{
					view.second->hide();
				}
			}
			playSoundByType(ST_Click);
		}
		return true;
	}
	

	void gui_blockFortPack::showTab(TabId tab)
	{

	}

	bool gui_blockFortPack::onItemStackTouchUp(const EventArgs & events)
	{
		auto touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(events);
		LordAssert(touchEvent.row == 0);
		Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = touchEvent.column;
		ItemStackPtr touchStack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (touchStack && touchStack->stackSize > 0)
		{
			ShowItemNameEvent::emit(touchStack);
		}
		return false;
	}

	bool gui_blockFortPack::onDeleteHotbar(const EventArgs& events)
	{
		playSoundByType(ST_Click);
		Blockman::Instance()->m_pPlayer->inventory->removeAllItemFromHotBar();
		return true;
	}
}