#include "gui_blockCityStore.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiItemStack.h"
#include "GUI/GuiSlotTable.h"

#include "Inventory/InventoryPlayer.h"
#include "Util/LanguageKey.h"
#include "game.h"
#include "ShellInterface.h"
#include "Setting/GameTypeSetting.h"
#include "Util/UICommon.h"
#include "GUI/GuiBlockCityTigerLotteryItem.h"
#include "GuiDef.h"
#include "GUI/GuiItemStack.h"
#include "GUI/gui_blockCityStorePaper.h"
#include "GUI/gui_blockCityStoreBlock.h"
#include "GUI/gui_blockCityStoreFurniture.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/BlockCitySetting.h"
#include "Util/TipType.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUILayout.h"
#include "GUI/GuiSlotTable.h"

namespace BLOCKMAN
{
	gui_blockCityStore::gui_blockCityStore()
		: gui_layout("BlockCityStore.json")
	{
		m_radioPanelMap[ViewId::PARPER] = m_radioPanelMap[ViewId::PARPER] ? m_radioPanelMap[ViewId::PARPER] : LordNew gui_blockCityStorePaper;
		m_radioPanelMap[ViewId::BLOCK] = m_radioPanelMap[ViewId::BLOCK] ? m_radioPanelMap[ViewId::BLOCK] : LordNew gui_blockCityStoreBlock;
		m_radioPanelMap[ViewId::FURNITURE] = m_radioPanelMap[ViewId::FURNITURE] ? m_radioPanelMap[ViewId::FURNITURE] : LordNew gui_blockCityStoreFurniture;
	}

	gui_blockCityStore::~gui_blockCityStore()
	{
		map<ViewId, gui_layout*>::iterator  iter;
		for (iter = m_radioPanelMap.begin(); iter != m_radioPanelMap.end(); iter++)
		{
			LordSafeDelete(iter->second);
		}
	}

	void gui_blockCityStore::onLoad()
	{
		setText("BlockCityStore-Frame-Name", "gui_blockcity_store");
		setText("BlockCityStore-Col-Lable-Parper-Name", "gui_blockcity_store_parper");
		setText("BlockCityStore-Col-Lable-Block-Name", "gui_blockcity_store_block");
		setText("BlockCityStore-Col-Lable-Furniture-Name", "gui_blockcity_store_furniture");
		setText("BlockCityStore-Info-Score-Name", "gui_blockcity_store_score");

		m_radioColTabs[ViewId::PARPER] = getWindow<GUIRadioButton>("BlockCityStore-Col-Lable-Paper");
		m_radioColTabs[ViewId::BLOCK] = getWindow<GUIRadioButton>("BlockCityStore-Col-Lable-Block");
		m_radioColTabs[ViewId::FURNITURE] = getWindow<GUIRadioButton>("BlockCityStore-Col-Lable-Furniture");

		m_itemIcon = getWindow<GUIStaticImage>("BlockCityStore-Info-Image");
		m_tvItemName = getWindow<GUIStaticText>("BlockCityStore-Info-Name");
		m_tvItemScore = getWindow<GUIStaticText>("BlockCityStore-Info-Score-Value");
		m_tvBuyOne = getWindow<GUIStaticText>("BlockCityStore-Buy-One-Name");
		m_tvBuyGroup = getWindow<GUIStaticText>("BlockCityStore-Buy-Group-Name");
		m_currencyBuyOne = getWindow<GUIStaticImage>("BlockCityStore-One-Money-Icon");
		m_currencyBuyGroup = getWindow<GUIStaticImage>("BlockCityStore-Group-Money-Icon");
		m_hotbar = getWindow<GuiSlotTable>("BlockCityStore-SlotTable");
		m_hotbar->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_blockCityStore::onItemStackTouchUp, this));

		m_tvItemDescName = getWindow<GUIStaticText>("BlockCityStore-Info-Desc-Name");
		m_tvItemDescVal = getWindow<GUIStaticText>("BlockCityStore-Info-Desc-Value");

		m_tvItemInteractLayout = getWindow<GUILayout>("BlockCityStore-Info-Interact");
		m_tvItemInteractName = getWindow<GUIStaticText>("BlockCityStore-Info-Interact-Name");
		m_tvItemInteractVal = getWindow<GUIStaticText>("BlockCityStore-Info-Interact-Value");
		m_tvItemInteractName->SetText(getString("gui_blockCity_store_interact").c_str());
		m_tvItemInteractLayout->SetVisible(false);

		
		getWindow("BlockCityStore-ToggleInventoryButton")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_blockCityStore::onCloseInventory, this, std::placeholders::_1));
		getWindow("BlockCityStore-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityStore::onCloseInventory, this, std::placeholders::_1));
		getWindow("BlockCityStore-Buy-One")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityStore::onClick, this, std::placeholders::_1, ViewId::BUY_ONE));
		getWindow("BlockCityStore-Buy-Group")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityStore::onClick, this, std::placeholders::_1, ViewId::BUY_GROUP));
		getWindow("BlockCityStore-Del")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityStore::onClick, this, std::placeholders::_1, ViewId::DEL));
		getWindow("BlockCityStore-Frame")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_blockCityStore::onClick, this, std::placeholders::_1, ViewId::MAIN));
		m_subscriptionGuard.add(UpdateBlockCityStoreEvent::subscribe(std::bind(&gui_blockCityStore::onDataChange, this)));
		m_subscriptionGuard.add(Blockman::Instance()->m_pPlayer->inventory->onInventoryContentSync([this] {
			m_isInventoryChanged = true;
		}));

		

		auto radioPanel = getWindow<GUILayout>("BlockCityStore-Panel");
		map<ViewId, GUIRadioButton*>::iterator  iter;
		for (iter = m_radioColTabs.begin(); iter != m_radioColTabs.end(); iter++)
		{
			if (m_radioPanelMap[iter->first])
			{
				m_radioPanelMap[iter->first]->attachTo(this, radioPanel);
				m_radioPanelMap[iter->first]->hide();
			}

			iter->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityStore::onRadioChange, this, std::placeholders::_1, iter->first));
		}
		m_radioColTabs[ViewId::PARPER]->SetSelected(true);

		m_subscriptionGuard.add(BlockCityStoreItemClickEvent::subscribe(std::bind(&gui_blockCityStore::onItemClick, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BlockCityStorageOperationEvent::subscribe(std::bind(&gui_blockCityStore::onOperation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));

	}

	void gui_blockCityStore::onShow()
	{
		if (m_hotbar)
		{
			m_hotbar->selectItemSlot(-1);
		}
	}

	void gui_blockCityStore::onUpdate(ui32 nTimeElapse)
	{
		m_itemAnimator.update(nTimeElapse);

		if (!isShown() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		if (isShown() && !UICommon::checkBlockCityIsInManor())
		{
			RootGuiLayout::Instance()->showMainControl();
		}

 		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
 		for (int i = 0; i < 9; ++i)
 		{
 			m_hotbar->setItemStack(0, i, inventory->hotbar[i]);
 		}
 
 		if (RootGuiLayout::Instance()->isPlayerInventoryShown() && m_isInventoryChanged) {
 			Blockman::Instance()->m_pPlayer->inventory->notifyUpdateToProxy();
 			m_isInventoryChanged = false;
 		}
 
	}

	bool gui_blockCityStore::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityStore::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioColTabs[viewId]->IsSelected())
		{
			if (m_hotbar)
			{
				m_hotbar->selectItemSlot(-1);
			}

			for (auto iter : m_radioColTabs)
			{
				if (m_radioPanelMap[iter.first] && m_radioPanelMap[iter.first]->isShown())
				{
					m_radioPanelMap[iter.first]->hide();
				}
			}

			if (m_radioPanelMap[viewId])
			{
				m_curTabId = viewId;
				m_radioPanelMap[viewId]->show();
			}
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_blockCityStore::onCloseInventory(const EventArgs& events)
	{
		RootGuiLayout::Instance()->showMainControl();
		return true;
	}

	bool gui_blockCityStore::onItemClick(const BlockCityStoreItem & item)
	{
		if (isShown() && m_needTakeItem.id == item.id && UICommon::checkPlayerParam() && !m_dataChange)
		{
			autoTake();
			m_dataChange = false;
			return false;
		}

		m_curItem = item;
		m_needTakeItem = item;
		m_dataChange = false;
		m_tvItemInteractLayout->SetVisible(false);
		if (m_radioPanelMap[m_curTabId])
		{
			dropItem(m_radioPanelMap[m_curTabId]->getItemPosition());
		}
		if (item.itemMate >= 0)
		{
			ItemStackPtr itemStack = LORD::make_shared<ItemStack>(item.itemId, 1, item.itemMate);
			GuiItemStack::showItemIcon(m_itemIcon, item.itemId, item.itemMate);
			m_tvItemName->SetText(LanguageManager::Instance()->getItemName(itemStack->getItemName()).c_str());
			m_tvItemDescName->SetText(getString("gui_blockCity_store_feature").c_str());
		}
		else
		{
			m_itemIcon->SetRenderAsBlock(false);
			m_itemIcon->SetImage(BlockCitySetting::getItemIcon(item.id).c_str());
			m_tvItemName->SetText(getString(BlockCitySetting::getItemName(item.id).c_str()));
			m_tvItemDescName->SetText(getString("gui_blockcity_store_vol").c_str());

			if (item.itemMate == -2)
			{
				m_tvItemInteractLayout->SetVisible(true);
				m_tvItemInteractVal->SetText(getString(BlockCitySetting::getItemDesc(item.id).c_str()));
			}
		}

		m_tvItemScore->SetText(StringUtil::ToString(item.score).c_str());
		setText(m_tvItemDescVal, item.volume.c_str());

		if (item.groupPrice > 0)
		{
			getWindow("BlockCityStore-Buy-One")->SetVisible(true);
			getWindow("BlockCityStore-Buy-Group")->SetEnabled(item.limit == 0);
			i32 onePrice = item.moneyType == CurrencyType::PLATFORM_MONEY ? item.onePriceCube : item.onePrice;
			i32 groupPrice = item.moneyType == CurrencyType::PLATFORM_MONEY ? item.groupPriceCube : item.groupPrice;
			m_tvBuyOne->SetText(StringUtil::Format(getString("gui_blockcity_store_one").c_str(), onePrice).c_str());
			m_tvBuyGroup->SetText(StringUtil::Format(getString("gui_blockcity_store_gcroup").c_str(), groupPrice).c_str());
			m_currencyBuyOne->SetImage(UICommon::getCurrencyIconByType(item.moneyType).c_str());
			m_currencyBuyGroup->SetImage(UICommon::getCurrencyIconByType(item.moneyType).c_str());
		}
		else
		{
			getWindow("BlockCityStore-Buy-One")->SetVisible(false);
			getWindow("BlockCityStore-Buy-Group")->SetEnabled(item.limit == 0);
			i32 price = item.moneyType == CurrencyType::PLATFORM_MONEY ? item.onePriceCube : item.onePrice;
			m_tvBuyGroup->SetText(StringUtil::Format(getString("gui_blockcity_store_one").c_str(), price).c_str());
			m_currencyBuyGroup->SetImage(UICommon::getCurrencyIconByType(item.moneyType).c_str());
		}
		return true;
	}

	bool gui_blockCityStore::onOperation(i32 hotbarIndex, i32 itemId, i32 meta, bool isEditMode)
	{
		if (!UICommon::checkPlayerParam() || !isEditMode)
		{
			return false;
		}
		if (itemId > 0)
		{
			auto inventory =Blockman::Instance()->m_pPlayer->inventory;
			int index = -1;
			for (int i = 0; i < InventoryPlayer::MAIN_INVENTORY_COUNT; i++ )
			{
				ItemStackPtr info = inventory->mainInventory[i];
				if (info && info->itemID == itemId && info->subItemID == meta)
				{
					index = i;
					break;
				}
			}

			int nSlot = -1;
			for (int i = 0; i < InventoryPlayer::HOTBAR_COUNT; i++)
			{
				ItemStackPtr tmp = inventory->hotbar[i];
				if (tmp && tmp->itemID == itemId && tmp->subItemID == meta) {
					nSlot = i;
					break;
				}
			}
			
			inventory->removeItemFromHotbar(nSlot);
			if (index >= 0)
			{
				if (hotbarIndex >= 0)
				{
					Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = hotbarIndex;
				}
				inventory->putItemToHotbar(index);
			}
		}
		return true;
	}

	bool gui_blockCityStore::onClick(const EventArgs& events, ViewId viewId)
	{
		if (m_curItem.id == 0)
		{
			return false;
		}

		switch (viewId)
		{
		case ViewId::BUY_ONE:
			if (CurrencyType(m_curItem.moneyType) == CurrencyType::PLATFORM_MONEY)
			{
				UICommon::checkMoney(m_curItem.onePriceCube, m_curItem.moneyType, [=](bool isSuccess) {
					if (isSuccess)
					{
						m_needTakeItem = m_curItem;
						autoTake();
						ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyGoods((i32)m_curTabId, m_curItem.id, 1, m_curItem.moneyType);
					}
				});
			}
			else
			{
				UICommon::checkMoney(m_curItem.onePrice, m_curItem.moneyType, [=](bool isSuccess) {
					if (isSuccess)
					{
						m_needTakeItem = m_curItem;
						autoTake();
						ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyGoods((i32)m_curTabId, m_curItem.id, 1, m_curItem.moneyType);
					}
					else
					{
						vector<BlockCityPayment>::type pays;
						BlockCityPayment pay;
						pay.num = 1;
						pay.price = m_curItem.onePriceCube;
						pay.moneyType = 1;
						pay.tabId = (i32)m_curTabId;
						pay.itemId = m_curItem.id;
						pays.push_back(pay);
						ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_LACK_MONEY_STORE, "gui_blockcity_lock_money_tip");
					}
				});
			}
			break;
		case ViewId::BUY_GROUP:
		{
			i32 price = 0;
			i32 priceCube = 0;
			if (m_curItem.groupPrice > 0)
			{
				price = m_curItem.groupPrice;
				priceCube = m_curItem.groupPriceCube;
			}
			else
			{
				price = m_curItem.onePrice;
				priceCube = m_curItem.onePriceCube;
			}

			if (CurrencyType(m_curItem.moneyType) == CurrencyType::PLATFORM_MONEY)
			{
				UICommon::checkMoney(priceCube, m_curItem.moneyType, [=](bool isSuccess) {
					if (isSuccess)
					{
						m_needTakeItem = m_curItem;
						autoTake();
						ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyGoods((i32)m_curTabId, m_curItem.id, 2, m_curItem.moneyType);
					}
				});
			}
			else
			{
				UICommon::checkMoney(price, m_curItem.moneyType, [=](bool isSuccess) {
					if (isSuccess)
					{
						m_needTakeItem = m_curItem;
						autoTake();
						ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyGoods((i32)m_curTabId, m_curItem.id, 2, m_curItem.moneyType);
					}
					else
					{
						vector<BlockCityPayment>::type pays;
						BlockCityPayment pay;
						pay.num = 2;
						pay.price = priceCube;
						pay.moneyType = 1;
						pay.tabId = (i32)m_curTabId;
						pay.itemId = m_curItem.id;
						pays.push_back(pay);
						ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_LACK_MONEY_STORE, "gui_blockcity_lock_money_tip");
					}
				});
			}
		}
			break;
		case ViewId::DEL:
			ClientNetwork::Instance()->getCommonSender()->sendBlockCityOperation(4);
			break;
		case ViewId::MAIN:
		{
			auto touchEvent = dynamic_cast<const MouseEventArgs&>(events);
			if (touchEvent.window)
			{
				dropItem(UVector2({ 0, touchEvent.position.x }, { 0, touchEvent.position.y }));
			}
		}
			break;
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockCityStore::onDataChange()
	{
		m_dataChange = true;
		return true;
	}

	bool gui_blockCityStore::onItemStackTouchUp(const EventArgs & events)
	{
		auto touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(events);
		LordAssert(touchEvent.row == 0);
		if (Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_pPlayer->inventory)
		{
			Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = touchEvent.column;
			ItemStackPtr touchStack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			if (touchStack && touchStack->stackSize > 0)
			{
				ShowItemNameEvent::emit(touchStack);
			}
			m_hotbar->selectItemSlot(Blockman::Instance()->m_pPlayer->inventory->currentItemIndex);
			takeItem(touchEvent.pos);
		}
		return false;
	}

	void gui_blockCityStore::takeItem(const UVector2& pos)
	{
		int index = m_hotbar->getSelectItemSlot();
		if (index >= 0 && m_needTakeItem.itemId > 0)
		{
			ItemStackPtr item = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			int mate = m_needTakeItem.itemMate >= 0 ? m_needTakeItem.itemMate : 0;
			if (item)
			{
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityStorageOperation(false, Blockman::Instance()->m_pPlayer->inventory->currentItemIndex, item->itemID, item->itemDamage, true);
			}

			if ((!item || item->itemID != m_needTakeItem.itemId || item->itemDamage != mate) && m_needTakeItem.limit <= 0)
			{
				UVector2 startPos = UVector2(UDim(0.5f, 0), UDim(0.5f, 0));
				if (m_radioPanelMap[m_curTabId])
				{
					startPos = m_radioPanelMap[m_curTabId]->getItemPosition();
				}
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityStorageOperation(true, Blockman::Instance()->m_pPlayer->inventory->currentItemIndex, m_needTakeItem.itemId, mate, true);
				m_itemAnimator.startAnimation(m_hotbar, m_needTakeItem.itemId, mate, startPos, pos);
				m_needTakeItem = BlockCityStoreItem();
				m_hotbar->selectItemSlot(-1);
			}
		}
	}

	void gui_blockCityStore::dropItem(const UVector2& endPos)
	{
		int index = m_hotbar->getSelectItemSlot();
		if (index >= 0)
		{
			ItemStackPtr item = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			if (item)
			{
				UVector2 startPos = m_hotbar->getItemPosition();
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityStorageOperation(false, Blockman::Instance()->m_pPlayer->inventory->currentItemIndex, item->itemID, item->subItemID, true);
				m_itemAnimator.startAnimation(m_hotbar, item->itemID, item->subItemID, startPos, endPos);
				Blockman::Instance()->m_pPlayer->inventory->removeCurrentItemFromHotbar();
			}
		}
		m_hotbar->selectItemSlot(-1);
	}

	void gui_blockCityStore::autoTake()
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		int mate = m_needTakeItem.itemMate >= 0 ? m_needTakeItem.itemMate : 0;
		if (m_needTakeItem.limit <= 0)
		{
			for (int i = 0; i < InventoryPlayer::HOTBAR_COUNT; i++)
			{
				ItemStackPtr itmeStack = inventory->hotbar[i];
				if (itmeStack && itmeStack->itemID == m_needTakeItem.itemId && itmeStack->subItemID == mate)
				{
					return;
				}
			}

			int index = inventory->getFirstEmptyStackInHotbar();
			if (index >= 0)
			{
				UVector2 startPos = UVector2(UDim(0.5f, 0), UDim(0.5f, 0));
				if (m_radioPanelMap[m_curTabId])
				{
					startPos = m_radioPanelMap[m_curTabId]->getItemPosition();
				}
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityStorageOperation(true, index, m_needTakeItem.itemId, mate, true);
				m_itemAnimator.startAnimation(m_hotbar, m_needTakeItem.itemId, mate, startPos, m_hotbar->getItemPosition(index));
				m_needTakeItem = BlockCityStoreItem();
				m_hotbar->selectItemSlot(-1);
			}
		}
	}
}