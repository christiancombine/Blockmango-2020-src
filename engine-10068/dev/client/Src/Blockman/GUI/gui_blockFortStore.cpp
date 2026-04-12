#include "gui_blockFortStore.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiItemStack.h"
#include "GUI/GuiSlotTable.h"

#include "Inventory/InventoryPlayer.h"
#include "game.h"
#include "Util/UICommon.h"
#include "GuiDef.h"
#include "GUI/GuiItemStack.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/BlockFortItemSetting.h"
#include "Setting/StoreTabSetting.h"
#include "Util/TipType.h"
#include "GUI/GuiScrollableInventory.h"
#include "GUI/GuiBlockFortStoreItem.h"
#include "GUI/GuiBlockFortItemInfo.h"

namespace BLOCKMAN
{

	gui_blockFortStore::gui_blockFortStore()
		: gui_layout("BlockFortStore.json")
	{

	}

	gui_blockFortStore::~gui_blockFortStore()
	{
		m_radioPanelMap.clear();
		m_radioTabs.clear();
	}

	void gui_blockFortStore::onLoad()
	{
		setText("BlockFortStore-Frame-Name", "gui_blockcity_store");
		setText("BlockFortStore-Limit", "buy_limit");
		m_tabs = getWindow<GUILayout>("BlockFortStore-Tabs");
		m_ivIcon = getWindow<GUIStaticImage>("BlockFortStore-Item-Icon");
		m_tvName = getWindow<GUIButton>("BlockFortStore-Item-Name");
		m_tvDesc = getWindow<GUIStaticText>("BlockFortStore-Item-Desc");
		m_lvInfos = getWindow<GUIListBox>("BlockFortStore-Item-Infos");
		m_btnBuy = getWindow<GUIButton>("BlockFortStore-Buy");
		m_tvMoneyValue = getWindow<GUIStaticText>("BlockFortStore-Buy-Money-Value");
		m_ivMoneyIcon = getWindow<GUIStaticImage>("BlockFortStore-Buy-Money-Icon");

		m_btnBuy->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortStore::onClick, this, std::placeholders::_1, ViewId::BUY));
		getWindow("BlockFortStore-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortStore::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BlockFortStore-Frame")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_blockFortStore::onClick, this, std::placeholders::_1, ViewId::MAIN));
		m_subscriptionGuard.add(BlockFortStoreUpdateEvent::subscribe(std::bind(&gui_blockFortStore::onDataChange, this, std::placeholders::_1)));
	}

	void gui_blockFortStore::onShow()
	{
		if (m_dataChange)
		{
			if (m_changeTabId == 0)
			{
				
				initGridView();
				initTabView();
			}
			else
			{
				initGridViewItems(m_curTabId, m_curGroupId);
			}
			m_dataChange = false;
			m_changeTabId = -1;
		}
	}

	bool gui_blockFortStore::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockFort);
	}

	void gui_blockFortStore::initGridView()
	{
		static i32 count = 0;
		auto tabs = Blockman::Instance()->m_pPlayer->m_blockFort->getStore();
		if (tabs.size() != getWindow<GUILayout>("BlockFortStore-Panel")->GetChildCount())
		{
			getWindow<GUILayout>("BlockFortStore-Panel")->CleanupChildren();
			for (size_t i = 0; i < tabs.size(); i++)
			{
				GUIString name = StringUtil::Format("BlockCityFortBlock-Grid-View-%d", count++).c_str();
				GuiCommonTabGridView* view = (GuiCommonTabGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GUI_COMMON_TAB_GRID_VIEW, name);
				view->SetArea(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(1.f, 0.f), UDim(1.f, 0.f));
				view->setTabsArea(URect(UDim(0.f, 0.f), UDim(0.f, -57.f), UDim(1.f, 0.f), UDim(0, 57.f)));
				view->setGridViewArea(URect(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(1.f, -64.f), UDim(1.f, -30.f)));
				view->initGridView(51.f, 29.f, 4);
				view->SetVisible(false);
				m_radioPanelMap.insert({ tabs[i].tabId, view });
				getWindow<GUILayout>("BlockFortStore-Panel")->AddChildWindow(view);
			}
		}

		for (const auto& tab : tabs)
		{
			initGridViewTab(tab.tabId);
		}
	}

	void gui_blockFortStore::initTabView()
	{
		static int count = 0;
		auto tabs = Blockman::Instance()->m_pPlayer->m_blockFort->getStore();
		if (tabs.size() != m_tabs->GetChildCount())
		{
			m_tabs->CleanupChildren();
			m_radioTabs.clear();
			for (size_t i = 0; i < tabs.size(); i++)
			{
				GUIString tabName = StringUtil::Format("BlockFortStoreBlock-Tabs-Item-%d", count++).c_str();
				GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, tabName);
				radioItem->SetArea(UDim(0, 0), UDim(0, 34 + (20 + 74.f) * i), UDim(1.f, 0.f), UDim(0, 74.f));
				radioItem->SetNormalImage("set:blockfort_store.json image:tab_nor");
				radioItem->SetPushedImage("set:blockfort_store.json image:tab_pre");
				radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockFortStore::onRadioChange, this, std::placeholders::_1, tabs[i].tabId));
				/*radioItem->getRenderText().SetBordered(true);
				radioItem->getRenderText().SetBorderColor(Color(0xff7b5d36));*/

				if (auto tabInfo = StoreTabSetting::Instance()->findItem(tabs[i].tabId))
				{
					radioItem->SetText( getString( tabInfo->name));
				}
				m_radioTabs.insert({ tabs[i].tabId, radioItem });
				m_tabs->AddChildWindow(radioItem);

				if (m_curTabId == -1 &&  i == 0 || m_curTabId == tabs[i].tabId)
				{
					if (m_curTabId == -1 && i == 0)
					{
						m_curTabId = tabs[i].tabId;
					}
					radioItem->SetSelected(true);
				}
			}
		}
	}

	void gui_blockFortStore::initGridViewTab(i32 tabId)
	{
		static int count = 0;
		if (m_radioPanelMap[tabId])
		{
			m_radioPanelMap[tabId]->removeAllTabs();
			auto tab = Blockman::Instance()->m_pPlayer->m_blockFort->getStoreTab(tabId);
			for (size_t i = 0; i < tab.groups.size(); i++)
			{
				GUIString tabName = StringUtil::Format("%s-Tabs-%d", m_radioPanelMap[tabId]->GetName().c_str(), count++).c_str();
				GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, tabName);
				radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockFortStore::onGroupRadioChange, this, std::placeholders::_1, radioItem, tabId, tab.groups[i].groupId));
				radioItem->SetArea(UDim(0, 70 + (105.f + 5) * i), UDim(0, 0), UDim(0, 105.f), UDim(0, 57.f));
				radioItem->SetNormalImage("set:blockfort_store.json image:item_tab_nor");
				radioItem->SetPushedImage("set:blockfort_store.json image:item_tab_pre");
				radioItem->SetText(StringUtil::Format("Tab %d", tab.groups[i].groupId).c_str());
				radioItem->SetText(getString(tab.groups[i].groupName.c_str()));
				radioItem->SetTextColor(Color(0xff5c4427));

				m_radioPanelMap[tabId]->addTabView(radioItem);
				if (m_curGroupId == -1 && i == 0 || tab.groups[i].groupId == m_curGroupId)
				{
					if (m_curTabId == -1 && i == 0)
					{
						m_curTabId = tabId;
					}
					radioItem->SetSelected(true);
				}
			}
		}
	}

	void gui_blockFortStore::initGridViewItems(i32 tabId, i32 groupId)
	{
		if (tabId == -1 || groupId == -1 || !m_radioPanelMap[tabId])
		{
			return;
		}
		static int count = 0;
		m_radioPanelMap[tabId]->removeAllItems();
		auto tab = Blockman::Instance()->m_pPlayer->m_blockFort->getStoreTab(tabId);

		bool hasCurItem = false;
		vector<BlockFortStoreItem>::type items = vector<BlockFortStoreItem>::type();
		for (const auto& item : tab.items)
		{
			if (item.groupId == groupId)
			{
				items.push_back(item);
				if (item.id == m_curItem.id)
				{
					hasCurItem = true;
				}
			}
		}

		for (const auto& item : items)
		{
			GUIString itemName = StringUtil::Format("%s-Item-%d", m_radioPanelMap[tabId]->GetName().c_str(), count++).c_str();
			GuiBlockFortStoreItem* itemView = (GuiBlockFortStoreItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GUI_BLOCK_FORT_STORE_ITEM_VIEW, itemName);
			itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 100.f), UDim(0, 100.f));
			itemView->setItem(item);
			itemView->subscribeEvent(EventWindowTouchUp, std::bind(&gui_blockFortStore::onItemClick, this, std::placeholders::_1, tabId, groupId, item));
			itemView->setSelectId(hasCurItem ? m_curItem.id : items[0].id);
			m_radioPanelMap[tabId]->addItemView(itemView, true);
			if (itemView->isSelected())
			{
				auto position = m_radioPanelMap[tabId]->getItemPosition();
				itemView->TouchUp(Vector2(position.d_x.d_offset, position.d_y.d_offset));
			}
		}
	}

	void gui_blockFortStore::initItemInfo(const vector<BlockFortItemInfo>::type & items)
	{
		static i32 count = 0;
		if (!m_lvInfos->getContainerWindow() || m_lvInfos->getContainerWindow()->GetChildCount() != items.size())
		{
			m_lvInfos->ClearAllItem();
			for (const auto& item : items)
			{
				String iterLayout = StringUtil::Format("BlockFortStore-Item-Infos-%d", count++).c_str();
				GuiBlockFortItemInfo* itemView = (GuiBlockFortItemInfo*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKFORT_ITEM_INFO, iterLayout.c_str());
				itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 27.f));
				itemView->setItemInfo(item, BFItemEntrance::Store);
				m_lvInfos->AddItem(itemView, false);
			}
		}
		else
		{
			for (size_t i = 0; i < m_lvInfos->getContainerWindow()->GetChildCount(); i++)
			{
				auto itemView = dynamic_cast<GuiBlockFortItemInfo*>(m_lvInfos->getContainerWindow()->GetChildByIndex(i));
				itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 27.f));
				itemView->setItemInfo(items[i], BFItemEntrance::Store);
			}
		}
	}

	bool gui_blockFortStore::onRadioChange(const EventArgs & events, i32 tabId)
	{
		if (m_radioTabs[tabId]->IsSelected())
		{
			for (auto iter : m_radioTabs)
			{
				if (m_radioPanelMap[iter.first] && m_radioPanelMap[iter.first]->IsVisible())
				{
					m_radioPanelMap[iter.first]->SetVisible(false);
				}
			}

			if (m_radioPanelMap[tabId])
			{
				m_curTabId = tabId;
				m_radioPanelMap[tabId]->SetVisible(true);
				for (size_t i = 0; i < m_radioPanelMap[m_curTabId]->getItemSize(); i++)
				{
					GuiBlockFortStoreItem* itemView = (GuiBlockFortStoreItem*)m_radioPanelMap[tabId]->getItem(i);
					if (itemView->isSelected())
					{
						auto position = m_radioPanelMap[m_curTabId]->getItemPosition();
						itemView->TouchUp(Vector2(position.d_x.d_offset, position.d_y.d_offset));
						break;
					}
				}
			}
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_blockFortStore::onGroupRadioChange(const EventArgs & events, GUIRadioButton* view, i32 tabId, i32 groupId)
	{
		if (view->IsSelected() && m_radioPanelMap[tabId])
		{
			if (m_curTabId == tabId)
			{
				m_curGroupId = groupId;
			}
			initGridViewItems(tabId, groupId);
		}
		return true;
	}

	bool gui_blockFortStore::onItemClick(const EventArgs & events, i32 tabId, i32 groupId, const BlockFortStoreItem & item)
	{
		m_curItem = item;
		m_dataChange = false;
		if (m_radioPanelMap[tabId])
		{
			for (size_t i = 0; i < m_radioPanelMap[tabId]->getItemSize(); i++)
			{
				GuiBlockFortStoreItem* itemView = (GuiBlockFortStoreItem*)m_radioPanelMap[tabId]->getItem(i);
				itemView->setSelectId(item.id);
			}
		}

		m_ivIcon->SetImage(BlockFortItemSetting::Instance()->getItemIcon(item.id).c_str());
		m_tvName->SetText(getString(BlockFortItemSetting::Instance()->getItemName(item.id).c_str()));
		m_tvDesc->SetText(getString(BlockFortItemSetting::Instance()->getItemDesc(item.id).c_str()));
		getWindow<GUIStaticImage>("BlockFortStore-Buy-Money-Icon")->SetImage(UICommon::getCurrencyIconByType(item.moneyType).c_str());
		getWindow("BlockFortStore-Limit")->SetVisible(item.limit > 0);
		initItemInfo(BlockFortItemSetting::Instance()->getItemInfos(item.id));

		bool isCanBuy = (item.limit == 0 || item.itemNum < item.limit);
		m_btnBuy->SetEnabled(isCanBuy);
		m_btnBuy->SetTouchable(isCanBuy);
		m_ivMoneyIcon->SetVisible(isCanBuy);

		if (isCanBuy)
		{
			i32 price = BlockFortItemSetting::Instance()->getItemPrice(item.id, item.moneyType);
			m_tvMoneyValue->SetText(StringUtil::ToString(price).c_str());
			m_tvMoneyValue->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.5f, 10.f), UDim(1.0f, 0.f));
			m_tvMoneyValue->SetHorizontalAlignment(HA_LEFT);
			m_tvMoneyValue->SetTextHorzAlign(HA_RIGHT);
		}
		else
		{
			m_tvMoneyValue->SetText(getString("gui_blockfort_got_item").c_str());
			m_tvMoneyValue->SetTextHorzAlign(HA_CENTRE);
			m_tvMoneyValue->SetHorizontalAlignment(HA_CENTRE);
		}
		return true;
	}

	bool gui_blockFortStore::onClick(const EventArgs& events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::BUY:
			{
				if (m_curItem.id > 0)
				{
					i32 level = Blockman::Instance()->m_pPlayer->m_blockFort->getManorManager()->curManor.level;
					auto item = BlockFortItemSetting::Instance()->findItem(m_curItem.id);
					if (!item || item->level > level)
					{
						ShowBlockFortCommonTipEvent::emit("level_not_enough");
						return false;
					}

					i32 price = BlockFortItemSetting::Instance()->getItemPrice(m_curItem.id, m_curItem.moneyType);
					UICommon::checkMoney(price, m_curItem.moneyType, [=](bool isSuccess) {
						if (isSuccess)
						{
							ClientNetwork::Instance()->getCommonSender()->sendGameTrade(m_curItem.id);
						}
					});
				}
			}
			break;
		case ViewId::MAIN:

			break;
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockFortStore::onDataChange(i32 tabId)
	{
		m_dataChange = true;
		m_changeTabId = tabId;
		if (isShown())
		{
			onShow();
		}
		return true;
	}
}