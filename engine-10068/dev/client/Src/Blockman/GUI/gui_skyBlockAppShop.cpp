#include "gui_skyBlockAppShop.h"
#include "World/World.h"
#include "cWorld/Blockman.h"
#include "Util/LanguageKey.h"
#include "GUI/GuiSkyBlockAppShopItem.h"
#include "GUI/GUIAppShopAdItem.h"
#include "GUI/GuiDef.h"
#include "Util/AboutShopping.h"
#include "game.h"
#include "Setting/GameTypeSetting.h"
#include "Setting/FilterWord.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/InventoryPlayer.h"
#include "Util/UICommon.h"
#include "GUI/GuiItemStack.h"
#include "Block/BlockUtil.h"
#include "Item/Item.h"
#include "Audio/SoundSystem.h"

namespace BLOCKMAN
{

	gui_skyBlockAppShop::gui_skyBlockAppShop() :
		gui_layout("SkyBlockAppShop.json")
	{
		LordNew SkyBlockAppShopSetting();
		
	}

	gui_skyBlockAppShop::~gui_skyBlockAppShop()
	{
		m_subscriptionGuard.unsubscribeAll();

		SkyBlockAppShopSetting::Instance()->unloadSetting();
		LordDelete SkyBlockAppShopSetting::Instance();
	}

	void gui_skyBlockAppShop::onLoad()
	{
		SkyBlockAppShopSetting::Instance()->loadSetting();
		getWindow("SkyBlockAppShop-TitleName")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_APPSHOP_TITLE));
		m_tabLayout = getWindow<GUILayout>("SkyBlockAppShop-Content-Tabs");
		m_textTip = getWindow<GUIStaticText>("SkyBlockAppShop-Content-Tip");
		m_itemListLayout = getWindow<GUILayout>("SkyBlockAppShop-Content-ItemList");
		getWindow<GUIButton>("SkyBlockAppShop-BtnClose")->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockAppShop::onButtonClick, this, std::placeholders::_1, ClickType::CLOSE_PANEL));
		m_itemGridView = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "SkyBlockAppShop-Content-ItemList-GridView");
		m_itemGridView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_itemGridView->InitConfig(15.f, 15.f, 5);
		m_itemListLayout->AddChildWindow(m_itemGridView);


		m_item_name = getWindow<GUIStaticText>("SkyBlockAppShop-Item-Name");
		m_des = getWindow<GUIStaticText>("SkyBlockAppShop-Destext");
		m_one_coin = getWindow<GUIStaticText>("SkyBlockAppShop-One-CoinNum");
		m_num_des = getWindow<GUIStaticText>("SkyBlockAppShop-Count-Text");
		m_num_text = getWindow<GUIStaticText>("SkyBlockAppShop-Buy-Num-Text");
		m_total_coin = getWindow<GUIStaticText>("SkyBlockAppShop-Total-CoinNum");
		m_sub_btn = getWindow<GUIButton>("SkyBlockAppShop-Sub-Btn");
		m_add_btn = getWindow<GUIButton>("SkyBlockAppShop-Add-Btn");
		m_buy_btn = getWindow<GUIButton>("SkyBlockAppShop-Buy-Btn");
		m_item_icon = getWindow<GUIStaticImage>("SkyBlockAppShop-Detail-Icon");
		m_coin_img = getWindow<GUIStaticImage>("SkyBlockAppShop-Coin-Icon");
		m_time_text = getWindow<GUIStaticText>("SkyBlockAppShop-Time");
		m_hot_img = getWindow<GUIStaticImage>("SkyBlockAppShop-Hot");
		m_have_img = getWindow<GUIStaticImage>("SkyBlockAppShop-Have");
		m_hot_text = getWindow<GUIStaticText>("SkyBlockAppShop-Hot-Text");
		m_have_text = getWindow<GUIStaticText>("SkyBlockAppShop-Have-Text");
		m_buy_text = getWindow<GUIStaticText>("SkyBlockAppShop-Buy-Text");
		m_one_icon = getWindow<GUIStaticImage>("SkyBlockAppShop-One-Coin-Icon");
		m_time_bg = getWindow<GUILayout>("SkyBlockAppShop-Time-Bg");

		m_lock_text = getWindow<GUIStaticText>("SkyBlockAppShop-Lock-Tip");
		m_lock_text->SetVisible(false);

		m_time_bg->SetVisible(false);
		m_have_img->SetVisible(false);
		m_hot_img->SetVisible(false);
		m_num_des->SetText(getString(LanguageKey::GUI_SHOP_NUM_TEXT));
		m_buy_text->SetText(getString(LanguageKey::SHOP_BTN_BUY));
		m_have_text->SetText(getString(LanguageKey::GUI_SHOP_IS_HAVE));
		
		m_add_btn->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockAppShop::onButtonClick, this, std::placeholders::_1, ClickType::ADD_NUM));
		m_sub_btn->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockAppShop::onButtonClick, this, std::placeholders::_1, ClickType::SUB_NUM));
		m_buy_btn->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockAppShop::onButtonClick, this, std::placeholders::_1, ClickType::BUY_ITEM));
		
		auto btnMin = getWindow<GUIButton>("SkyBlockAppShop-Num-Min");
		btnMin->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockAppShop::onButtonClick, this, std::placeholders::_1, ClickType::MIN_NUM));
		auto strMin = getString("sky.block.app.shop.min");
		if (strMin != "sky.block.app.shop.min")
			btnMin->SetText(strMin);
		
		auto btnMax = getWindow<GUIButton>("SkyBlockAppShop-Num-Max");
		btnMax->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockAppShop::onButtonClick, this, std::placeholders::_1, ClickType::MAX_NUM));
		auto strMax = getString("sky.block.app.shop.max");
		if (strMax != "sky.block.app.shop.max")
			btnMax->SetText(strMax);

		m_input_num = getWindow<GUIEditBox>("SkyBlockAppShop-Buy-Num");
		m_input_num->subscribeEvent(EventWindowTextChanged, std::bind(&gui_skyBlockAppShop::onInputChanged, this, std::placeholders::_1));
		m_input_num->SetBlinkshow(true);
		m_subscriptionGuard.add(SkyBlockUpdataShopInfoEvent::subscribe(std::bind(&gui_skyBlockAppShop::updateShopInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(SkyBlockShopClickItemEvent::subscribe(std::bind(&gui_skyBlockAppShop::updateShopDetail, this, std::placeholders::_1, std::placeholders::_2)));
		initTabsAndPanels();
	}

	bool gui_skyBlockAppShop::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct);
	}

	bool gui_skyBlockAppShop::onInputChanged(const EventArgs & events)
	{
		std::string text = m_input_num->GetText().c_str();
		m_input_num->SetText("");
		m_cur_num = StringUtil::ParseInt(text.c_str());
		if (m_cur_num <= 0)
			m_cur_num = 1;

		if (m_cur_num > 64)
			m_cur_num = 64;

		if (m_good.forever)
			m_cur_num = 1;

		m_num_text->SetText(StringUtil::ToString(m_cur_num).c_str());
		m_total_coin->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_TOTAL_PRICE).c_str(), m_cur_num * m_good.coinNum).c_str());
		return true;
	}

	void gui_skyBlockAppShop::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
			return;
		if (m_showTipTime > 0 && m_showTipTime < 3000)
		{
			m_showTipTime = nTimeElapse + m_showTipTime;
		}
		else
		{
			m_showTipTime = 0;
			m_tipMessage = "";
		}
		m_textTip->SetText(m_tipMessage.c_str());

		if (m_time > 0 && !m_good.forever && (m_good.goodType == SKYBLOCK_PRIVILIGE || m_good.goodType == SKYBLOCK_NPC_UNLOCK))
		{
			m_time_bg->SetVisible(true);
			m_time -= nTimeElapse;
			int second = m_time / 1000;
			int hour = (second % 86400) / 3600 + 1;
			int day = second / 86400;

			if (hour == 24)
			{
				day += 1;
				hour = 0;
			}

			if (hour > 0)
			{
				if (day > 0)
				{

					m_time_text->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_LAST_TIME).c_str(),
						StringUtil::Format(getString(LanguageKey::GUI_SHOP_DAY).c_str(), day).c_str(),
						StringUtil::Format(getString(LanguageKey::GUI_SHOP_HOUR).c_str(), hour).c_str()).c_str());
				}
				else
				{

					m_time_text->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_LAST_TIME).c_str(),
						"", StringUtil::Format(getString(LanguageKey::GUI_SHOP_HOUR).c_str(), hour).c_str()).c_str());
				}
			}
			else
			{

				m_time_text->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_LAST_TIME).c_str(),
					StringUtil::Format(getString(LanguageKey::GUI_SHOP_DAY).c_str(), day).c_str(), "").c_str());
			}
		}
		else
		{
			m_time_bg->SetVisible(false);
		}
	}

	void gui_skyBlockAppShop::addTabView(SkyBlockGoodGroup group, int index)
	{
		String strTabName = StringUtil::Format("SkyBlockAppShop-Content-Tabs-Item-%d", index).c_str();
		String iconName = StringUtil::Format("SkyBlockAppShop-Content-Tabs-Item-Icon-%d", index).c_str();
		if (GUIWindowManager::Instance()->GetGUIWindow(strTabName.c_str()))
		{
			return;
		}
		GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, strTabName.c_str());
		radioItem->SetArea(UDim(0, 0.f), UDim(0, index * 75.f), UDim(1.f, 0), UDim(0, 65.f));
		radioItem->SetNormalImage("set:skyblock_shop.json image:tab_nor");
		radioItem->SetPushedImage("set:skyblock_shop.json image:tab_select");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "20 20 20 20");

		GUIString tabName = "";
		GUIString imageRes = "";

		radioItem->SetTextColor(Color(0.921569f, 0.807843f, 0.643137f));
		radioItem->SetProperty(GWP_TEXT_BORDER, "true");
		radioItem->SetText(getString(group.m_name.c_str()));
		radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_skyBlockAppShop::onRadioChange, this, std::placeholders::_1, group.m_id, radioItem));
		m_tabLayout->AddChildWindow(radioItem);
		m_shop_tabs[group.m_id] = radioItem;

		GUIString ChildsName = StringUtil::Format("gui_skyBlockAppShop-Panels-%d", index).c_str();
		GUIGridView* pChildGV = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, ChildsName);

		pChildGV->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		pChildGV->InitConfig(9.f, 10.f, 5);

		m_itemListLayout->AddChildWindow(pChildGV);
		//updataShopSort(pChildGV, group, index);
		m_panel_map[group.m_id] = pChildGV;
		updataTabGroup(group, index);
	}

	void gui_skyBlockAppShop::updataTabGroup(SkyBlockGoodGroup group, int index)
	{
		map<int, String>::type tabGroups;
		for (auto item : group.m_goods)
		{
			auto groupId = tabGroups.find(item.groupId);
			if (groupId == tabGroups.end())
			{
				tabGroups[item.groupId] = item.groupText;
			}
		}

		GUIString layoutName = StringUtil::Format("gui_skyBlockAppShop-Tab-Group-%d", index).c_str();
		GUILayout* layout = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, layoutName);
		layout->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		layout->SetTouchable(false);
		float wide = (getWindow<GUILayout>("SkyBlockAppShop-Content-Tab-Group")->GetPixelSize().x - 10 * 4) / 5;
		static i32 count = 0;
		int indexCount = 0;
		for (auto tab : tabGroups)
		{
			GUIString name = StringUtil::Format("gui_skyBlockAppShop-Tab-Group-%d-Radio-%d", index, count++).c_str();
			GUIRadioButton* radio = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, name);
			radio->SetArea(UDim(0, (wide + 10) * indexCount), UDim(0, 0), UDim(0, wide), UDim(1.f, 0));
			radio->SetNormalImage("set:skyblock_shop.json image:tip_tab_nor");
			radio->SetPushedImage("set:skyblock_shop.json image:top_tab_select");
			radio->subscribeEvent(EventRadioStateChanged, std::bind(&gui_skyBlockAppShop::onTabGroupRadioChange, this, std::placeholders::_1, group, indexCount + 1, radio));
			layout->AddChildWindow(radio);
			radio->SetText(LanguageManager::Instance()->getString(tab.second).c_str());
			if (indexCount == 0)
			{
				radio->SetSelected(true);
			}
			indexCount++;
		}

		m_tab_group.insert({group.m_id, layout});
		getWindow<GUILayout>("SkyBlockAppShop-Content-Tab-Group")->AddChildWindow(layout);
	}

	void gui_skyBlockAppShop::updataShopSort(GUIGridView* gridView, SkyBlockGoodGroup group, int index)
	{
		static unsigned goodsCount = 0;
		vector<SkyBlockGoods>::type items = group.m_goods;
		float maxWidth = (gridView->GetPixelSize().x - 10.0f * 4) / 5 + 1;
		gridView->RemoveAllItems(); 
		gridView->SetScrollOffset(0);
		auto cur_count = 0;
		for (SkyBlockGoods item : items)
		{
			goodsCount++;
			cur_count++;
			GUIString strTabName = StringUtil::Format("SkyBlockAppShop-Content-ItemList-GridView-Item-%d", goodsCount).c_str();
			GuiSkyBlockAppShopItem* goodsItem = (GuiSkyBlockAppShopItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_APP_SHOP_ITEM, strTabName);
			goodsItem->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, maxWidth), UDim(0, maxWidth));
			goodsItem->setGoods(item, group.m_id);
			goodsItem->setIndex(cur_count);
			gridView->AddItem(goodsItem);
			if (cur_count == 1)
			{
				updateShopDetail(item, 1);
			}
		}
	}

	bool gui_skyBlockAppShop::onTabGroupRadioChange(const EventArgs & events, SkyBlockGoodGroup group, i32 groupId, GUIRadioButton * radio)
	{
		if (radio->IsSelected())
		{
			SkyBlockGoodGroup newGroup;
			if (!SkyBlockAppShopSetting::Instance()->getShopItemByGroupId(group.m_id, groupId, newGroup))
			{
				return false;
			}

			updataShopSort(m_panel_map[group.m_id], newGroup, 0);
		}
		return false;
	}

	bool gui_skyBlockAppShop::onRadioChange(const EventArgs & events, int groupId, GUIRadioButton * radio)
	{
		playSoundByType(ST_Click);
		if (radio->IsSelected())
		{
			m_selectTabId = groupId;
			for (auto group : m_tab_group)
			{
				group.second->SetVisible(group.first == groupId);
			}

			for (auto panel : m_panel_map)
			{
				if (panel.first == groupId)
				{
					if (GuiSkyBlockAppShopItem* item = (GuiSkyBlockAppShopItem*)panel.second->GetItem(0))
						updateShopDetail(item->m_goods, 1);
				}
				panel.second->SetVisible(panel.first == groupId);
			}
		}
		return true;
	}

	bool gui_skyBlockAppShop::onButtonClick(const EventArgs & events, ClickType clickType)
	{
		switch (clickType)
		{
		case ClickType::CLOSE_PANEL:
			playSoundByType(ST_ClosePanel);
			getParent()->showMainControl();
			break;
		case ClickType::SUB_NUM:
			m_cur_num --;
			if (m_cur_num <= 0)
				m_cur_num = 1;

			if (m_good.forever)
				m_cur_num = 1;

			m_num_text->SetText(StringUtil::ToString(m_cur_num).c_str());
			m_total_coin->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_TOTAL_PRICE).c_str(), m_cur_num * m_good.coinNum).c_str());
			break;
		case ClickType::ADD_NUM:
			m_cur_num ++;
			if (m_cur_num > 64)
				m_cur_num = 64;
			if (m_good.forever)
				m_cur_num = 1;

			m_num_text->SetText(StringUtil::ToString(m_cur_num).c_str());
			m_total_coin->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_TOTAL_PRICE).c_str(), m_cur_num * m_good.coinNum).c_str());
			break;
		case ClickType::MAX_NUM:
			m_cur_num = 64;
			if (m_good.forever)
				m_cur_num = 1;
			m_num_text->SetText(StringUtil::ToString(m_cur_num).c_str());
			m_total_coin->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_TOTAL_PRICE).c_str(), m_cur_num * m_good.coinNum).c_str());
			break;
		case ClickType::MIN_NUM:
			m_cur_num = 1;
			m_num_text->SetText(StringUtil::ToString(m_cur_num).c_str());
			m_total_coin->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_TOTAL_PRICE).c_str(), m_cur_num * m_good.coinNum).c_str());
			break;
		case ClickType::BUY_ITEM:
			if (m_good.goodType != SKYBLOCK_NPC_UNLOCK)
			{
				if (m_good.goodType == SKYBLOCK_PRIVILIGE)
				{
					if (!Blockman::Instance()->m_pPlayer->inventory->SkyBlockisCanBuyPrivilege(m_good.haveItem, m_cur_num))
					{
						onLackOfMoney((int)BuyResult::FullInventory);
						return false;
					}
				}
				else
				{
					if (!Blockman::Instance()->m_pPlayer->inventory->isCanAddItem(m_good.itemId, m_good.meta, m_cur_num))
					{
						onLackOfMoney((int)BuyResult::FullInventory);
						return false;
					}
				}
			}
			SoundSystem::Instance()->playEffectByType(ST_Click);
			i32 price = m_good.coinNum * m_cur_num;
			UICommon::checkMoney(price, m_good.coinId, [=](bool isSuccess) {
				if (isSuccess)
				{
					if (m_good.coinId == SKYBLOCK_DIAMOND && price >= 100)
					{
						ShowSkyBlockCostMuchMoneyEvent::emit("gui_cost_much_money_tip", m_good.coinId, price, m_good.id, m_cur_num, m_good.name);
					}
					else
					{
						GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSkyBlockBuyGoods(m_cur_num, m_good.id, BUY_GOOD);
					}
				}
				else
				{
					onLackOfMoney((int)BuyResult::LackOfMoney);
				}
			});
			break;

		}
		return true;
	}

	bool gui_skyBlockAppShop::onLackOfMoney(int msgType)
	{
		m_showTipTime = 1;
		switch (msgType) {
		case (int)BuyResult::LackingGood:
			m_tipMessage = getString(LanguageKey::GUI_STR_APP_SHOP_INSUFFICIENT_INVENTORY).c_str();
			break;
		case (int)BuyResult::LackOfMoney:
			m_tipMessage = getString(LanguageKey::LACK_OF_MONEY).c_str();
			break;
		case (int)BuyResult::FullInventory:
			m_tipMessage = getString(LanguageKey::GUI_STR_APP_SHOP_INVENTORY_IS_FULL).c_str();
			break;
		case (int)BuyResult::HasNoGun:
			m_tipMessage = getString(LanguageKey::HAS_NO_GUN).c_str();
			break;
		case (int)BuyResult::LackOfItem:
			m_tipMessage = getString("gui_shop_lack_of_item").c_str();
			break;
		}
		m_textTip->SetText(m_tipMessage.c_str());
		return true;
	}

	bool gui_skyBlockAppShop::onShopUpdate()
	{
		vector<SkyBlockGoodGroup>::type cur_Goods;
		if (SkyBlockAppShopSetting::Instance()->getShopItem(cur_Goods))
		{
			for (auto &panel : m_panel_map)
			{
				GUIGridView* childs = panel.second;
				childs->RemoveAllItems();
			}
			
			for (auto &panel : m_tab_group)
			{
				GUILayout* childs = panel.second;
				childs->CleanupChildren();
			}

			getWindow("SkyBlockAppShop-Content-Tab-Group")->CleanupChildren();
			m_tab_group.clear();

			for (size_t i = 0; i < cur_Goods.size(); i++)
			{
				auto tab = m_shop_tabs.find(cur_Goods[i].m_id);
				if (tab != m_shop_tabs.end())
				{
					for (auto &panel : m_panel_map)
					{
						if (tab->first == panel.first)
						{
							updataTabGroup(cur_Goods[i], i);
							//updataShopSort(panel.second, cur_Goods[i], i);
							break;
						}
					}
				}
			}

			auto cur_tab = m_shop_tabs.find(m_selectTabId);
			if (cur_tab != m_shop_tabs.end())
			{

				cur_tab->second->SetSelected(false);
				cur_tab->second->SetSelected(true);
			}
		}


		for (auto panel : m_panel_map)
		{
			if (panel.first == m_selectTabId)
			{
				if (GuiSkyBlockAppShopItem* item = (GuiSkyBlockAppShopItem*)panel.second->GetItem(0))
					updateShopDetail(item->m_goods, 1);
			}
		}
		return true;
	}

	bool gui_skyBlockAppShop::onBuyGoodsResult(const String& msg)
	{
		m_showTipTime = 1;
		m_tipMessage = msg;
		m_textTip->SetText(msg.c_str());
		return true;
	}

	bool gui_skyBlockAppShop::updateShopInfo(int sortType, int level, const String& priData)
	{
		auto old_sortType = m_sortType;
		auto old_level = m_level;
		m_sortType = sortType;
		m_level = level;
		vector<SkyBlockGoods>::type m_goods;

		SkyBlockAppShopSetting::Instance()->parseUpdateShopData(m_sortType, m_level, m_goods, priData);
		if (old_sortType != m_sortType)
		{
			onShopUpdate();
			return true;
		}
		
		if (m_goods.empty())
		{
			return true;
		}

		for (auto good : m_goods)
		{

			if (m_good.id == good.id)
			{
				updateShopDetail(good, 0);
			}


			auto tab = m_shop_tabs.find(good.tabId);
			if (tab != m_shop_tabs.end())
			{
				auto panel = m_panel_map.find(tab->first);
				if (panel != m_panel_map.end())
				{
					auto count = panel->second->GetItemCount();
					for (size_t i = 0; i < count; i++)
					{
						GuiSkyBlockAppShopItem* shopItem = (GuiSkyBlockAppShopItem*)panel->second->GetItem(i);
						if (shopItem->m_goods.id == good.id)
						{
							shopItem->setGoods(good, good.tabId);
							break;
						}
					}
				}
			}
		}
		return true;
	}

	bool gui_skyBlockAppShop::updateShopDetail(const SkyBlockGoods goodData, int index)
	{
 		m_good = goodData;
		m_cur_num = 1;
		m_time = (long long)goodData.curTime * 1000;

		m_one_coin->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_ONE_PRICE).c_str(), goodData.coinNum).c_str());
		m_total_coin->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_TOTAL_PRICE).c_str(), m_cur_num * goodData.coinNum).c_str());
		m_num_text->SetText(StringUtil::ToString(m_cur_num).c_str());
		m_item_name->SetText(LanguageManager::Instance()->getString(goodData.name).c_str());
		m_des->SetText(LanguageManager::Instance()->getString(goodData.detail).c_str());



		int cur_blockId = goodData.itemId;
		if (goodData.goodType == SKYBLOCK_BLOCK)
		{
			if (BlockUtil::isValidBlockId(cur_blockId))
			{
				Block* pBlock = BlockManager::sBlocks[cur_blockId];
				if (pBlock != NULL)
				{
					if (!BlockUtil::isBasicBlock(cur_blockId))
					{
						cur_blockId = cur_blockId + Item::EC_BLOCK_TO_ITEM_OFFSET;
					}
				}
			}
		}

		GuiItemStack::showItemIcon(m_item_icon, cur_blockId, goodData.meta, goodData.icon);

		m_coin_img->SetImage(UICommon::getCurrencyIconByType(goodData.coinId).c_str());
		m_one_icon->SetImage(UICommon::getCurrencyIconByType(goodData.coinId).c_str());
		m_hot_img->SetVisible(false);
		m_buy_btn->SetVisible(goodData.status == SKYBLOCK_GOOD_UN_LOCK);
		if (goodData.status != SKYBLOCK_GOOD_UN_LOCK)
		{
			m_buy_btn->SetVisible(false);
			m_buy_text->SetVisible(false);
			m_lock_text->SetVisible(true);
			String msg = getString(goodData.lockTip).c_str();
			if (StringUtil::Find(msg, "%d"))
				msg = StringUtil::Format(msg.c_str(), goodData.level);
			m_lock_text->SetText(msg.c_str());
		}
		else
		{
			m_buy_btn->SetVisible(true);
			m_buy_text->SetVisible(true);
			m_lock_text->SetVisible(false);
			m_buy_text->SetText(getString(LanguageKey::SHOP_BTN_BUY));
		}

		if (goodData.hot == 1)
		{
			m_hot_img->SetVisible(true);
			m_hot_img->SetImage("set:skyblock_shop.json image:hot_red");
			m_hot_text->SetText(getString(LanguageKey::GUI_SHOP_HOT));
		}

		m_have_img->SetVisible(goodData.isHave);

		if (goodData.goodType == SKYBLOCK_PRIVILIGE || goodData.goodType == SKYBLOCK_NPC_UNLOCK)
		{
			m_hot_img->SetVisible(true);
			m_hot_img->SetImage("set:skyblock_shop.json image:hot_greey");
			if (goodData.goodType == SKYBLOCK_PRIVILIGE)
			{
				m_hot_img->SetImage("set:skyblock_shop.json image:hot_red");
			}
			if (goodData.forever)
			{
				m_hot_text->SetText(getString(LanguageKey::GUI_SHOP_FOREVER));
				if (goodData.isHave)
				{
					m_buy_btn->SetVisible(false);
					m_buy_text->SetText(getString(LanguageKey::GUI_SHOP_IS_HAVE));
				}
			}
			else
			{
				if (goodData.time > 0)
				{
					int  hour = (goodData.time % 86400) / 3600;
					int  day = goodData.time / 86400;

					if (day > 0)
					{
						m_hot_text->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_DAY).c_str(), day).c_str());
					}
				}
				else
				{
					m_hot_img->SetImage("set:skyblock_shop.json image:hot_red");
				}

				if (goodData.haveForever)
				{
					m_buy_btn->SetVisible(false);
					m_buy_text->SetText(getString(LanguageKey::GUI_SHOP_HAVE_FOREVER));
				}

			}
		}

		if (goodData.goodType == SKYBLOCK_PRIVILIGE && goodData.disCountTips.length() > 2)
		{
			m_hot_img->SetImage("set:skyblock_shop.json image:hot_red");
			m_hot_img->SetVisible(true);
			m_hot_text->SetText(getString(goodData.disCountTips));
		}

		if (index != 0)
		{
			auto panel = m_panel_map.find(m_selectTabId);
			if (panel != m_panel_map.end())
			{
				auto count = panel->second->GetItemCount();
				for (size_t i = 0; i < count; i++)
				{
					GuiSkyBlockAppShopItem* shopItem = (GuiSkyBlockAppShopItem*)panel->second->GetItem(i);
					if (shopItem)
					{
						shopItem->m_select_img->SetVisible(false);
						if (i == index - 1)
						{
							shopItem->m_select_img->SetVisible(true);
						}
					}
				}
			}
		}
		return true;
	}

	void gui_skyBlockAppShop::initTabsAndPanels()
	{
		vector<SkyBlockGoodGroup>::type cur_Goods;
		if (SkyBlockAppShopSetting::Instance()->getShopItem(cur_Goods))
		{
			int mun = m_tabLayout->GetChildCount();
			for (int i = 0; i < mun; i++)
			{
				m_tabLayout->RemoveChildWindow(m_tabLayout->GetChildByIndex(0));
			}
			
			for (size_t i = 0; i < cur_Goods.size(); i++)
			{
				addTabView(cur_Goods[i], i);
			}
			auto radioButton = m_shop_tabs.find(1);
			if (radioButton != m_shop_tabs.end())
				radioButton->second->SetSelected(true);
		}
	}

	bool gui_skyBlockAppShop::changePageToIndex(i32 pageId)
	{
		auto btn = m_shop_tabs.find(pageId);
		if (btn != m_shop_tabs.end())
		{
			btn->second->SetSelected(true);
		}
		return true;
	}
}
