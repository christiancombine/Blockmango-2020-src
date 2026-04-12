#include "gui_blockFortPackInventory.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "Util/ClientEvents.h"
#include "Item/Items.h"
#include "Inventory/InventoryPlayer.h"
#include "GUI/GuiActorWindow.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Actor/ActorManager.h"
#include "UI/GUIStaticImage.h"
#include "Util/LanguageKey.h"
#include "UI/GUIImagesetManager.h"
#include "Setting/GameTypeSetting.h"
#include "Setting/BlockFortItemSetting.h"
#include "Inventory/InventoryPlayer.h"

namespace BLOCKMAN
{
	bool BuildInventory::checkItem(i32 slot)
	{
		if (auto itemStack = m_inventory->getStackInSlot(slot))
		{
			auto item = BlockFortItemSetting::Instance()->findItemByItemId(itemStack->itemID);
			return  item ? item->type < 4 : false;
		}
		return  false;
	}

	bool BuildInventory::isBuilding(i32 itemId)
	{
		return itemId % 2 == 0;
	}
	int BuildInventory::getSizeInventory()
	{
		return  m_inventory->getSizeInventory();
	}
	ItemStackPtr BuildInventory::getStackInSlot(int slot)
	{
		if (checkItem(slot))
		{
			return m_inventory->getStackInSlot(slot);
		}
		return nullptr;
	}

	ItemStackPtr BuildInventory::decrStackSize(int slot, int count)
	{
		return m_inventory->decrStackSize(slot, count);
	}

	ItemStackPtr BuildInventory::getStackInSlotOnClosing(int slot)
	{
		return m_inventory->getStackInSlotOnClosing(slot);
	}

	void BuildInventory::setInventorySlotContents(int slot, ItemStackPtr pStack)
	{
		m_inventory->setInventorySlotContents(slot, pStack);
	}

	bool BuildInventory::isItemValidForSlot(int slot, ItemStackPtr pStack)
	{
		return m_inventory->isItemValidForSlot(slot, pStack);
	}

	//-------------------------------------------------------------------

	const gui_blockFortPackInventory::ViewId  gui_blockFortPackInventory::ALL_VIEW_ID[] = {
		ViewId::ALL,
		ViewId::BUILDING,
		ViewId::ARMOR
	};

	gui_blockFortPackInventory::gui_blockFortPackInventory()
		: gui_layout("BlockFortPackInventory.json")
	{
	}

	gui_blockFortPackInventory::~gui_blockFortPackInventory()
	{
		LordSafeDelete(m_buildingInventory);
	}

	void gui_blockFortPackInventory::onLoad()
	{
		if (!m_buildingInventory)
		{
			m_buildingInventory = LordNew BuildInventory(Blockman::Instance()->m_pPlayer->inventory->getMainInventory());
		}
		m_equipedArmorTable = getWindow<GuiSlotTable>("BlockFortPackInventory-Equiped");
		m_guiScrollableInventory = getWindow<GuiScrollableInventory>("BlockFortPackInventory-Inventory-Items");

		m_pbDefence = getWindow<GUIProgressBar>("BlockFortPackInventory-Defence");


		m_guiScrollableInventory->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_blockFortPackInventory::equipArmor, this));
		m_guiScrollableInventory->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_blockFortPackInventory::onItemStackTouchUp, this));
		m_equipedArmorTable->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_blockFortPackInventory::unequipArmor, this));

		m_actorWindow = getWindow<GuiActorWindow>("BlockFortPackInventory-Actor");
		m_actorTitleName = getWindow<GUIStaticText>("BlockFortPackInventory-Name");


		m_ivItemIcon = getWindow<GUIStaticImage>("BlockFortPackInventory-ItemInfo-Icon");
		m_tvItemDesc = getWindow<GUIStaticText>("BlockFortPackInventory-ItemInfo-Desc");
		m_tvItemName = getWindow<GUIStaticText>("BlockFortPackInventory-ItemInfo-Name");
		m_tvItemName->SetText("");
		m_tvItemDesc->SetText("");

		m_subscriptionGuard.add(ChangeDefenseEvent::subscribe(std::bind(&gui_blockFortPackInventory::updateDefense, this, std::placeholders::_1, std::placeholders::_2)));

		m_radioMap[ViewId::ALL] = getWindow<GUIRadioButton>("BlockFortPackInventory-Inventory-Radio-All");
		m_radioMap[ViewId::ARMOR] = getWindow<GUIRadioButton>("BlockFortPackInventory-Inventory-Radio-Armor");
		m_radioMap[ViewId::BUILDING] = getWindow<GUIRadioButton>("BlockFortPackInventory-Inventory-Radio-Building");

		m_radioMap[ViewId::ALL]->SetText(getString(LanguageKey::INVENTORY_PANEL_ALL));
		m_radioMap[ViewId::ARMOR]->SetText(getString(LanguageKey::INVENTORY_PANEL_EQUIPMENT));
		m_radioMap[ViewId::BUILDING]->SetText(getString("inventory_panel_building"));
		
		for (ViewId id :ALL_VIEW_ID)
		{
			m_radioMap[id]->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockFortPackInventory::onRadioChange, this, std::placeholders::_1, id));
		}
		m_radioMap[ViewId::ALL]->SetSelected(true);
		onShow();
	}

	void gui_blockFortPackInventory::onShow()
	{
		if (GameClient::CGame::Instance()->isEnterBack())
		{
			return;
		}

		String defaultIdle = "idle";
		if (Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_pPlayer->m_defaultIdle > 0)
		{
			defaultIdle = "idle_" +  StringUtil::ToString(Blockman::Instance()->m_pPlayer->m_defaultIdle);
		}
		m_actorWindow->SetActor(Blockman::Instance()->m_pPlayer->m_sex == 2 ? "girl.actor" : "boy.actor", defaultIdle.c_str());
		m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
		m_actorWindow->GetActor()->SetCustomColor(Blockman::Instance()->m_pPlayer->m_skinColor);
	}

	void gui_blockFortPackInventory::onUpdate(ui32)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		m_actorTitleName->SetText(Blockman::Instance()->m_pPlayer->getEntityName().c_str());
		if (inventory == nullptr)
			return;
		auto armorInventory = inventory->getArmorInventory();
		//m_guiScrollableInventory->setInventory(inventory->getArmorFilteredInventory());
		i32 size = m_equipedArmorTable->getColumnCount() * m_equipedArmorTable->getRowCount();
		for (int i = 0; i < size; ++i)
		{
			m_equipedArmorTable->setItemStackArmorType(i, 0, ArmorType(i));
			m_equipedArmorTable->setItemStack(i, 0, armorInventory->getStackInSlot(i));
		}
		updateData();
	}

	bool gui_blockFortPackInventory::equipArmor(const EventArgs & event)
	{

		if (m_showViewId != ViewId::ARMOR)
		{
			return false;
		}

		auto touchEvent = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event);
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		auto itemStack = inventory->getArmorFilteredInventory()->getStackInSlot(touchEvent.index);
		auto index = inventory->findItemStack(itemStack);
		inventory->equipArmor(itemStack);
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendEquipArmor(index);
		Blockman::Instance()->m_pPlayer->m_outLooksChanged = true;
		m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
		return true;
	}

	bool gui_blockFortPackInventory::unequipArmor(const EventArgs & event)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BED_WAR)
		{
			return false;
		}
		auto touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(event);
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		auto itemStack = inventory->getArmorInventory()->getStackInSlot(touchEvent.row * 2 + touchEvent.column);
		auto index = inventory->findItemStack(itemStack);
		if (inventory->unequipArmor(itemStack))
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendUnequipArmor(index);
		}
		Blockman::Instance()->m_pPlayer->m_outLooksChanged = true;
		m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
		return true;
	}

	bool gui_blockFortPackInventory::isNeedLoad()
	{
		return checkGameType({ ClientGameType::BlockCity ,ClientGameType::BlockFort });
	}

	bool gui_blockFortPackInventory::onItemStackTouchUp(const EventArgs & event)
	{
		if (m_showViewId == ViewId::ARMOR)
		{
			return false;
		}

		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr currentItem = inventory->getCurrentItem();
		auto touchIndex = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event).index;
		if (m_selectionIndex == touchIndex)
		{
			m_selectionIndex = -1;
		}
		else if (m_selectionIndex >= 0)
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendSwapItemPosition(m_selectionIndex, touchIndex);
			m_guiScrollableInventory->getInventory()->swapStacksInSlots(m_selectionIndex, touchIndex);
			m_selectionIndex = -1;
		}
		else if (m_guiScrollableInventory->getItemStack(touchIndex) == nullptr)
		{
			inventory->removeCurrentItemFromHotbar();
		}
		else if (m_guiScrollableInventory->getItemStack(touchIndex) == currentItem)
		{
			m_selectionIndex = touchIndex;
		}
		else
		{
			inventory->putItemToHotbar(touchIndex);
		}
		m_ivItemIcon->SetRenderAsBlock(false);
		ItemStackPtr touchStack = inventory->getMainInventory()->getStackInSlot(touchIndex);
		if (touchStack && touchStack->stackSize > 0)
		{
			GuiItemStack::showItemIcon(m_ivItemIcon, touchStack->itemID, touchStack->subItemID);
			if (auto item = BlockFortItemSetting::Instance()->findItemByItemId(touchStack->itemID))
			{
				m_tvItemName->SetText(getString(item->name));
				m_tvItemDesc->SetText(getString(item->desc));
			}
			ShowItemNameEvent::emit(touchStack);
		}
		else
		{
			m_ivItemIcon->SetImage("");
			m_tvItemName->SetText("");
			m_tvItemDesc->SetText("");
		}
		m_guiScrollableInventory->selectItemSlot(m_selectionIndex);

		return true;
	}

	bool gui_blockFortPackInventory::onRadioChange(const EventArgs & events , const ViewId viewId)
	{
		if (m_radioMap[viewId]->IsSelected()) {
			m_showViewId = viewId;
			updateData();
		}
		return true;
	}

	bool gui_blockFortPackInventory::updateDefense(int defense, int maxDefense)
	{
		m_pbDefence->SetProgress(float(defense) / maxDefense);
		getWindow("BlockFortPackInventory-Defence-Value")->SetText(StringUtil::Format( "%d/%d", defense, maxDefense).c_str());
		return false;
	}

	void gui_blockFortPackInventory::updateData()
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (inventory == nullptr)
			return;
		switch (m_showViewId)
		{
		case ViewId::ALL:
			m_guiScrollableInventory->setInventory(inventory->getMainInventory());
			break;
		case ViewId::BUILDING:
			m_guiScrollableInventory->setInventory(m_buildingInventory);
			break;
		case ViewId::ARMOR:
			m_guiScrollableInventory->setInventory(inventory->getArmorFilteredInventory());
			break;
		}
	}
	
}
