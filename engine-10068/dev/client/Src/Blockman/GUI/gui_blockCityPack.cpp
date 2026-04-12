#include "gui_blockCityPack.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiSlotTable.h"
#include "Inventory/InventoryPlayer.h"
#include "game.h"
#include "Util/UICommon.h"
#include "GUI/GuiBlockCityTigerLotteryItem.h"
#include "GuiDef.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/BlockCitySetting.h"
#include "UI/GUIGridView.h"
#include "UI/GUIRadioButton.h"
#include "Util/TipType.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUILayout.h"
#include "UI/GUIGridView.h"
#include "GUI/GuiSlotTable.h"
#include "UI/GUIButton.h"


namespace BLOCKMAN
{
	i32 gui_blockCityPack::replaceIndex = -1;

	gui_blockCityPack::gui_blockCityPack()
		: gui_layout("BlockCityPack.json")
	{
	}

	gui_blockCityPack::~gui_blockCityPack()
	{
		m_radioMap.clear();
		m_tabMap.clear();
	}

	void gui_blockCityPack::onLoad()
	{
 		setText("BlockCityPack-Title-Name", "gui_blockcity_pack");

		m_labs = getWindow<GUILayout>("BlockCityPack-Label");
		m_items = getWindow<GUIGridView>("BlockCityPack-Items");
		m_items->InitConfig(4.f, 6.f, 5);
		m_image = getWindow<GUIStaticImage>("BlockCityPack-Info-Image");
		m_status = getWindow<GUIButton>("BlockCityPack-Change-Status");
		m_timedTip = getWindow<GUIStaticText>("BlockCityPack-Timed-Tip");
 		m_hotbar = getWindow<GuiSlotTable>("BlockCityPack-SlotTable");
 		m_hotbar->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_blockCityPack::onItemStackTouchUp, this));
		m_transferEffect = getWindow<GUIStaticImage>("BlockCityPack-Transfer-Effect");

		getWindow("BlockCityPack-Timed-Tip-Layout")->SetVisible(false);

		getWindow("BlockCityPack-ToggleInventoryButton")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_blockCityPack::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BlockCityPack-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityPack::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		m_status->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityPack::onClick, this, std::placeholders::_1, ViewId::CHANGE_STATUS));
		getWindow("BlockCityPack-Transfer")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityPack::onClick, this, std::placeholders::_1, ViewId::TRANSFER));

 		m_subscriptionGuard.add(UpdateBlockCityPlayerPackEvent::subscribe(std::bind(&gui_blockCityPack::onDataChange, this)));

 		m_subscriptionGuard.add(Blockman::Instance()->m_pPlayer->inventory->onInventoryContentSync([this] {
 			m_isInventoryChanged = true;
 		}));

 		m_subscriptionGuard.add(BlockCityPackItemClickEvent::subscribe(std::bind(&gui_blockCityPack::onItemClick, this, std::placeholders::_1)));
 		m_subscriptionGuard.add(BlockCityStorageOperationEvent::subscribe(std::bind(&gui_blockCityPack::onOperation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
	}

	void gui_blockCityPack::onShow()
	{
		if (m_hotbar)
		{
			m_hotbar->selectItemSlot(-1);
		}
		if (m_dataChange)
		{
			onPackUpdate();
			m_dataChange = false;
		}
		else
		{
			if (m_curItem.itemId > 0)
			{
				m_needTakeItem = BlockCityHouseShopItem();
				BlockCityPackItemClickEvent::emit(m_curItem);
			}
		}

		m_transferEffectTime = 2000;
	}

	void gui_blockCityPack::onUpdate(ui32 nTimeElapse)
	{
 		m_itemAnimator.update(nTimeElapse);

		if (!isShown() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
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

		m_dataChangeTime += nTimeElapse;
		if (m_limitItemLeftTime > 0)
		{
			m_limitItemLeftTime = (m_limitItemLeftTime - nTimeElapse) > 0 ? (m_limitItemLeftTime - nTimeElapse) : 0;
			if (m_limitItemLeftTime >= 0)
			{
				setText(m_timedTip, timeFormat(m_limitItemLeftTime));
			}
		}
 
		checkTransferEffect(nTimeElapse);
	}

	bool gui_blockCityPack::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	void gui_blockCityPack::onPackUpdate()
	{
		if (!UICommon::checkBlockCityParam())
		{
			return;
		}
		auto pack = Blockman::Instance()->m_pPlayer->m_blockCity->getHouseShop();
		
		static int count = 0;
		m_radioMap.clear();
		if (pack.size() != m_labs->GetChildCount())
		{
			m_labs->CleanupChildren();
			for (size_t i = 0; i < pack.size(); i++)
			{
				GUIString strTabName = StringUtil::Format("BlockCityPack-Label-%d", count++).c_str();
				GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, strTabName.c_str());
				radioItem->SetArea(UDim(0, 0.f), UDim(0, i * 105.f), UDim(1.f, 0.f), UDim(0.f, 100.f));
				radioItem->SetNormalImage("set:blockcity_store.json image:col_lable_nor");
				radioItem->SetPushedImage("set:blockcity_store.json image:col_lable_pre");
				radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityPack::onRadioChange, this, std::placeholders::_1, pack[i]));

				GUIString strTabImageName = StringUtil::Format("BlockCityPack-Label-Image-%d", count).c_str();
				GUIStaticImage* radioItemImage = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strTabImageName.c_str());
				radioItemImage->SetArea(UDim(0, 0.f), UDim(0, 0.f), UDim(1.f, 0.f), UDim(1.f, 0.f));
				radioItemImage->SetImage(pack[i].shopClassifyImage.c_str());
				radioItemImage->SetTouchable(false);
				radioItem->AddChildWindow(radioItemImage);

				GUIString strTabTextName = StringUtil::Format("BlockCityPack-Label-Text-%d", count).c_str();
				GUIStaticText* radioItemText = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strTabTextName.c_str());
				radioItemText->SetArea(UDim(0, 0.f), UDim(0, 0.f), UDim(1.f, 0.f), UDim(1.f, 0.f));
				radioItemText->SetTextHorzAlign(HA_CENTRE);
				radioItemText->SetTextVertAlign(VA_BOTTOM);
				radioItemText->SetText(getString(pack[i].shopClassifyName).c_str());
				radioItemText->SetTouchable(false);
				radioItemText->SetTextColor(Color::BLACK);
				radioItem->AddChildWindow(radioItemText);

				m_radioMap.insert({ pack[i].shopClassifyId, radioItem });
				m_labs->AddChildWindow(radioItem);
			}
			
		}
		else
		{
			for (size_t i = 0; i < m_labs->GetChildCount(); i++)
			{
				auto radioItem = dynamic_cast<GUIRadioButton*>(m_labs->GetChildByIndex(i));
				radioItem->removeAllEvents();
				radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityPack::onRadioChange, this, std::placeholders::_1, pack[i]));
				m_radioMap.insert({ pack[i].shopClassifyId, radioItem });
			}
		}

		i32 tabId = m_curTabId > 0 ? m_curTabId : pack[0].shopClassifyId;
		if (m_radioMap[tabId])
		{
			if (m_radioMap[tabId]->IsSelected())
			{
				WindowEventArgs args(m_radioMap[tabId]);
				m_radioMap[tabId]->fireEvent(EventRadioStateChanged, args);
			}
			else 
			{
				m_radioMap[tabId]->SetSelected(true);
			}
		}
	}

	bool gui_blockCityPack::onRadioChange(const EventArgs & events, BlockCityHouseShopClassify tool)
	{
		if(m_radioMap[tool.shopClassifyId] && m_radioMap[tool.shopClassifyId]->IsSelected())
		{
			if (m_hotbar)
			{
				m_hotbar->selectItemSlot(-1);
			}

			m_needTakeItem = BlockCityHouseShopItem();

			float itemX = (m_items->GetPixelSize().x - 4 * 4.f) / 5;
			float itemY = 125.f / 135.f * itemX;
			static int toolsCount = 0;
			m_items->RemoveAllItems();
			m_items->SetScrollOffset(0);
			vector<BlockCityHouseShopItem>::type items = Blockman::Instance()->m_pPlayer->m_blockCity->getShopItems(tool.shopClassifyId);
			bool isHaveItem = false;
			for (auto item : items)
			{
				if (item.itemStatus > 1)
				{
					isHaveItem = true;
					GUIString strItemName = StringUtil::Format("BlockCityPack-Items-%d", toolsCount++).c_str();
					GuiBlockCityTigerLotteryItem* itemView = (GuiBlockCityTigerLotteryItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_TIGER_LOTTERY_ITEM, strItemName);
					itemView->setPackItem(item);
					itemView->setShowType(3);
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, itemX), UDim(0, itemY));
					m_items->AddItem(itemView);
				}
			}

			getWindow<GUILayout>("BlockCityPack-Null-Items-Info")->SetVisible(!isHaveItem);
			getWindow<GUILayout>("BlockCityPack-Info")->SetVisible(isHaveItem);

			BlockCityHouseShopItem  defaultItem;

			int num = 0;
			for (size_t i = 0; i < items.size(); i++)
			{
				if (items[i].itemStatus > 1)
				{
					if (num == 0)
					{
						defaultItem = items[i];
					}
					if (items[i].itemId == m_tabMap[tool.shopClassifyId] && items[i].itemId != 0)
					{
						defaultItem = items[i];
						break;
					}
					num++;
				}
				
			}
			if (defaultItem.itemId != 0)
			{
				m_curItem = defaultItem;
				BlockCityPackItemClickEvent::emit(m_curItem);
			}
			m_curTabId = tool.shopClassifyId;
		}
		return true;
	}

	bool gui_blockCityPack::onItemClick(const BlockCityHouseShopItem & item)
	{
		auto tool = BlockCitySetting::getItem(item.itemId);
		i32 shopId = Blockman::Instance()->m_pPlayer->m_blockCity->getTabIdByItemId(item.itemId);
		m_tabMap.erase(shopId);
		m_tabMap.insert({ shopId, item.itemId });

		if (isShown() && m_needTakeItem.itemId == item.itemId && UICommon::checkPlayerParam() && !m_dataChange)
		{
			autoTake();
			m_dataChange = false;
		}

		m_curItem = item;
		m_needTakeItem = item;
		m_dataChange = false;

		m_image->SetImage(tool->icon.c_str());
		setText("BlockCityPack-Info-Name", tool->name.c_str());
		setText("BlockCityPack-Introduce", tool->desc.c_str());
		setText(m_status, item.itemStatus > 2 ? "gui_blockcity_pack_demount" : "gui_blockcity_pack_use");
		m_status->SetNormalImage(item.itemStatus > 2 ? "set:blockcity_pack.json image:demount" : "set:blockcity_pack.json image:use");
		m_status->SetPushedImage(item.itemStatus > 2 ? "set:blockcity_pack.json image:demount" : "set:blockcity_pack.json image:use");

		getWindow("BlockCityPack-Timed-Tip-Layout")->SetVisible(item.limitTime > 0);

		m_limitItemLeftTime = item.limitTime - m_dataChangeTime;

		for (size_t i = 0; i < m_items->GetItemCount(); i++)
		{
			auto itemView = dynamic_cast<GuiBlockCityTigerLotteryItem*>(m_items->GetItem(i));
			itemView->setSelectId(item.itemId);
		}

		dropItem(getItemPosition());
		return true;
	}

	bool gui_blockCityPack::onOperation(i32 hotbarIndex, i32 itemId, i32 meta, bool isEditMode)
	{
		if (!UICommon::checkPlayerParam() || isEditMode)
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

	bool gui_blockCityPack::onClick(const EventArgs& events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::DEL:
			ClientNetwork::Instance()->getCommonSender()->sendBlockCityOperation(4);
			break;
		case ViewId::CHANGE_STATUS:
		{
			InventoryPlayer* inventory = Blockman::Instance()->m_pPlayer->inventory;
			i32 index = inventory->getInventorySlotContainItem(m_curItem.itemId);
			if (index == -1)
			{
				index = (m_curTabId == i32(ShopClassifyId::Wings)) ? inventory->getInventoryIndexOfCurrentItem() : inventory->getFirstEmptyStackInHotbar();
				index = index != -1 ? replaceIndex = index : ++replaceIndex < 9 ? replaceIndex : replaceIndex = 0;
			}
			ClientNetwork::Instance()->getCommonSender()->sendBlockCityChangeToolStatus(m_curTabId, m_curItem.itemId, index);
			Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = index;
			break;
		}
		case ViewId::TRANSFER:
		{
			vector<BlockCityPayment>::type pays;
			if (Blockman::Instance()->m_pPlayer->isOnDecoration())
			{
				RootGuiLayout::Instance()->showMainControl();
				ShowBlockCityTipEvent::emit("gui_call_on_in_decoration");
				RootGuiLayout::Instance()->hideWindowByLayoutId(RootGuiLayout::LayoutId::BLOCK_CITY_PACK);
				break;
			}
			else if (BLOCKMAN::Blockman::Instance()->m_pPlayer->isOnVehicle())
			{
				RootGuiLayout::Instance()->showMainControl();
				ShowBlockCityTipEvent::emit("gui_manor_call_tp_in_vehicle");
				RootGuiLayout::Instance()->hideWindowByLayoutId(RootGuiLayout::LayoutId::BLOCK_CITY_PACK);
				break;
			}
			else if (BLOCKMAN::Blockman::Instance()->m_pPlayer->isGlide())
			{
				RootGuiLayout::Instance()->showMainControl();
				ShowBlockCityTipEvent::emit("gui_call_on_in_glide");
				RootGuiLayout::Instance()->hideWindowByLayoutId(RootGuiLayout::LayoutId::BLOCK_CITY_PACK);
				break;
			}
			else if (BLOCKMAN::Blockman::Instance()->m_pPlayer->isUseElevator())
			{
				RootGuiLayout::Instance()->showMainControl();
				ShowBlockCityTipEvent::emit("gui_call_on_use_elevator");
				RootGuiLayout::Instance()->hideWindowByLayoutId(RootGuiLayout::LayoutId::BLOCK_CITY_PACK);
				break;
			}
			else
			{
				ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_GO_SHOP, "gui_blockcity_transfer_shop");
			}
			break;
		}
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockCityPack::onDataChange()
	{
		m_dataChange = true;
		m_haveData = true;
		m_dataChangeTime = 0;
		if (isShown())
		{
			onShow();
		}
		return true;
	}

	bool gui_blockCityPack::onItemStackTouchUp(const EventArgs & events)
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
 
	void gui_blockCityPack::takeItem(const UVector2& pos)
	{
		int index = m_hotbar->getSelectItemSlot();
		if (index >= 0 && m_needTakeItem.itemId > 0)
		{
			ItemStackPtr item = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			if (item)
			{
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityStorageOperation(false, Blockman::Instance()->m_pPlayer->inventory->currentItemIndex, item->itemID, item->itemDamage, false);
			}

			if ((!item || item->itemID != m_needTakeItem.itemId) && m_needTakeItem.itemStatus > 0)
			{
				UVector2 startPos = UVector2(UDim(0.5f, 0), UDim(0.5f, 0));
				startPos = getItemPosition();
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityStorageOperation(true, Blockman::Instance()->m_pPlayer->inventory->currentItemIndex, m_needTakeItem.itemId, 0, false);
				m_itemAnimator.startAnimation(m_hotbar, m_needTakeItem.itemId, 0, startPos, pos);
				m_needTakeItem = BlockCityHouseShopItem();
				m_hotbar->selectItemSlot(-1);
			}
		}
	}

	void gui_blockCityPack::dropItem(const UVector2& endPos)
	{
		int index = m_hotbar->getSelectItemSlot();
		if (index >= 0)
		{
			ItemStackPtr item = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			if (item)
			{
				UVector2 startPos = m_hotbar->getItemPosition();
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityStorageOperation(false, Blockman::Instance()->m_pPlayer->inventory->currentItemIndex, item->itemID, item->subItemID, false);
				m_itemAnimator.startAnimation(m_hotbar, item->itemID, item->subItemID, startPos, endPos);
				Blockman::Instance()->m_pPlayer->inventory->removeCurrentItemFromHotbar();
			}
		}
		m_hotbar->selectItemSlot(-1);
	}

	void gui_blockCityPack::autoTake()
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (m_needTakeItem.itemStatus > 1)
		{
			for (int i = 0; i < InventoryPlayer::HOTBAR_COUNT; i++)
			{
				ItemStackPtr itmeStack = inventory->hotbar[i];
				if (itmeStack && itmeStack->itemID == m_needTakeItem.itemId)
				{
					return;
				}
			}
			
			int index = inventory->getFirstEmptyStackInHotbar();
			
			if (index >= 0)
			{
				UVector2 startPos = UVector2(UDim(0.5f, 0), UDim(0.5f, 0));
				startPos = getItemPosition();
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityStorageOperation(true, index, m_needTakeItem.itemId, 0,false);
				m_itemAnimator.startAnimation(m_hotbar, m_needTakeItem.itemId, 0, startPos, m_hotbar->getItemPosition(index));
				m_needTakeItem = BlockCityHouseShopItem();
				m_hotbar->selectItemSlot(-1);
			}
		}
	}

	UVector2 gui_blockCityPack::getItemPosition()
	{
		for (size_t i = 0; i < m_items->GetItemCount(); i++)
		{
			auto itemView = dynamic_cast<GuiBlockCityTigerLotteryItem*>(m_items->GetItem(i));
			if (itemView->isSelected())
			{
				return UVector2(UDim(0, itemView->GetRenderArea().left + itemView->GetRenderArea().getWidth() / 2), UDim(0, itemView->GetRenderArea().top + itemView->GetRenderArea().getHeight() / 2));
			}
		}
		return UVector2(UDim(0.5f, 0), UDim(0.5f, 0));
	}

	String gui_blockCityPack::timeFormat(i32 time)
	{
		i32 seconds = time / 1000;
		i32 day = seconds / 86400;
		String d = StringUtil::ToString(day);
		String h = StringUtil::ToString((seconds % 86400) / 3600);
		h = (h.length() == 1 ? ("0" + h) : h);
		String m = StringUtil::ToString((seconds % 3600) / 60);
		m = (m.length() == 1 ? ("0" + m) : m);
		String s = StringUtil::ToString(seconds % 60);
		s = (s.length() == 1 ? ("0" + s) : s);

		return StringUtil::Format(getString("gui_blockcity_timed_tip").c_str(), d.c_str(), h.c_str(), m.c_str(), s.c_str()).c_str();
	}

	void gui_blockCityPack::checkTransferEffect(i32 nTimeElapse)
	{
		m_transferEffectTime = (m_transferEffectTime - nTimeElapse) > 0 ? (m_transferEffectTime - nTimeElapse) : 0;
		m_transferEffect->SetVisible(m_transferEffectTime > 0);
	}
}