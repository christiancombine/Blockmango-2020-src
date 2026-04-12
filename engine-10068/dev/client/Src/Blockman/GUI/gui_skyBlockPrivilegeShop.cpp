#include "gui_skyBlockPrivilegeShop.h"
#include "Util/LanguageKey.h"
#include "GUI/GuiSkyBlockPrivilegeShopItem.h"
#include "GUI/GuiSkyBlockPrivilegeRewardItem.h"
#include "GUI/GuiDef.h"
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
#include "UI/GUIFontManager.h"

namespace BLOCKMAN
{

	gui_skyBlockPrivilegeShop::gui_skyBlockPrivilegeShop() :
		gui_layout("SkyBlockPrivilegeShop.json")
	{
		m_skyblock_supeme_privilege = m_skyblock_supeme_privilege ? m_skyblock_supeme_privilege : LordNew GuiSkyBlockSupemePrivilege;
		LordNew SkyBlockPrivilegeShopSetting();

	}

	gui_skyBlockPrivilegeShop::~gui_skyBlockPrivilegeShop()
	{
		m_subscriptionGuard.unsubscribeAll();

		SkyBlockPrivilegeShopSetting::Instance()->unloadSetting();
		LordDelete SkyBlockPrivilegeShopSetting::Instance();
		LordSafeDelete(m_skyblock_supeme_privilege);
	}

	void gui_skyBlockPrivilegeShop::onLoad()
	{
		SkyBlockPrivilegeShopSetting::Instance()->loadSetting();

		setText("SkyBlockPrivilegeShop-Normal-Name", "gui_skyblock_normal_pri_title");
		setText("SkyBlockPrivilegeShop-Supeme-Name", "gui_skyblock_supeme_pri_title");
		setText("SkyBlockPrivilegeShop-Reward-Text", "gui_sky_block_pri_reward_text");

		m_normal_title = getWindow<GUIStaticImage>("SkyBlockPrivilegeShop-Normal-Title");
		m_supeme_title = getWindow<GUIStaticImage>("SkyBlockPrivilegeShop-Supeme-Title");
		m_tabLayout = getWindow<GUILayout>("SkyBlockPrivilegeShop-Content-Tabs");
		m_normalLayout = getWindow<GUILayout>("SkyBlockPrivilegeShop-Content");
		m_itemListLayout = getWindow<GUILayout>("SkyBlockPrivilegeShop-Content-ItemList");
		getWindow<GUIButton>("SkyBlockPrivilegeShop-BtnClose")->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockPrivilegeShop::onButtonClick, this, std::placeholders::_1, ClickType::CLOSE_PANEL));
		m_itemGridView = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "SkyBlockPrivilegeShop-Content-ItemList-GridView");
		m_itemGridView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_itemGridView->InitConfig(0, 8.f, 1);
		m_itemListLayout->AddChildWindow(m_itemGridView);

		m_rewardListLayout = getWindow<GUILayout>("SkyBlockPrivilegeShop-Reward-List");
		m_rewardGridView = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "SkyBlockPrivilegeReward-Content-ItemList-GridView");
		m_rewardGridView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_rewardGridView->InitConfig(4.f, 4.f, 3);
		m_rewardListLayout->AddChildWindow(m_rewardGridView);


		m_buy_btn = getWindow<GUIButton>("SkyBlockPrivilegeShop-Buy-Btn");
		m_buy_text = getWindow<GUIStaticText>("SkyBlockPrivilegeShop-Buy-Text");

		m_item_name = getWindow<GUIStaticText>("SkyBlockPrivilegeShop-Item-Name");
		m_item_des = getWindow<GUIStaticText>("SkyBlockPrivilegeShop-Destext");
		//m_privilege_name = getWindow<GUIStaticText>("SkyBlockPrivilegeShop-Pri-Name");

		m_buy_btn->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockPrivilegeShop::onButtonClick, this, std::placeholders::_1, ClickType::BUY_ITEM));
		m_subscriptionGuard.add(SkyBlockUpdataShopInfoEvent::subscribe(std::bind(&gui_skyBlockPrivilegeShop::updateShopInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(SkyBlockPrivilegeShopClickItemEvent::subscribe(std::bind(&gui_skyBlockPrivilegeShop::updateShopDetail, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(SkyBlockPrivilegeRewardClickItemEvent::subscribe(std::bind(&gui_skyBlockPrivilegeShop::updateShopItemDetail, this, std::placeholders::_1, std::placeholders::_2)));
		initTabsAndPanels();
	}

	bool gui_skyBlockPrivilegeShop::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct);
	}

	void gui_skyBlockPrivilegeShop::onUpdate(ui32 nTimeElapse)
	{
		
	}

	void gui_skyBlockPrivilegeShop::onGetCommonData(String key, String data)
	{
		if (key == "SupemePrivilegeImg")
		{
			SkyBlockPrivilegeShopSetting::Instance()->parseUpdatePriImgData(data);
			if (m_cur_select == SKYBLOCK_PRI_SUPREME)
			{
				vector<SkyBlockPrivilegeGoods>::type cur_Goods;
				SkyBlockPrivilegeShopSetting::Instance()->getShopItemByTabId(cur_Goods, SKYBLOCK_PRI_SUPREME);
				for (auto supemePri : cur_Goods)
				{
					m_skyblock_supeme_privilege->setGood(supemePri);
					break;
				}
			}
			return;
		}
	}

	void gui_skyBlockPrivilegeShop::addTabView(SkyBlockPrivilegeGoodGroup group, int index)
	{
		String strTabName = StringUtil::Format("SkyBlockPrivilegeShop-Content-Tabs-Item-%d", index).c_str();
		String imgName = StringUtil::Format("SkyBlockPrivilegeShop-Content-Tabs-Red-Img-%d", index).c_str();
		String textName = StringUtil::Format("SkyBlockPrivilegeShop-Content-Tabs-Red-Text-%d", index).c_str();
		if (GUIWindowManager::Instance()->GetGUIWindow(strTabName.c_str()))
		{
			return;
		}
		GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, strTabName.c_str());
		radioItem->SetArea(UDim(0, 0.f), UDim(0, index * 70.f), UDim(1.f, 0), UDim(0, 56.f));
		radioItem->SetNormalImage("set:skyblock_privilege_shop.json image:btn_normal");
		radioItem->SetPushedImage("set:skyblock_privilege_shop.json image:tab_select");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		radioItem->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "10 25 25 25");

		GUIString tabName = "";
		GUIString imageRes = "";

		radioItem->SetTextColor(Color(0.921569f, 0.807843f, 0.643137f));
		radioItem->SetProperty(GWP_TEXT_BORDER, "true");
		radioItem->SetText(getString(group.m_name.c_str()));

		if (index == 1)
		{
			GUIStaticImage* red_img = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, imgName.c_str());
			GUIStaticText* red_text = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, textName.c_str());
			red_img->SetImage("set:skyblock_privilege_shop.json image:tab_red");
			red_img->SetArea(UDim(0, 0), UDim(0, -5.f), UDim(0.8f, 0), UDim(0, 16.f));
			red_text->SetArea(UDim(0, 0), UDim(0, -10.f), UDim(0.8f, 0), UDim(0, 16.f));
			red_text->SetText(getString("gui_privilege_red_text"));
			red_text->SetTextHorzAlign(HA_CENTRE);
			red_text->SetFont(GUIFontManager::Instance()->GetFont("HT12"));
			radioItem->AddChildWindow(red_img);
			radioItem->AddChildWindow(red_text);
		}

		radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_skyBlockPrivilegeShop::onRadioChange, this, std::placeholders::_1, group.m_id, radioItem));
		m_tabLayout->AddChildWindow(radioItem);
		if (index == 0)
		{
			radioItem->SetSelected(true);
		}
	}

	bool gui_skyBlockPrivilegeShop::onRadioChange(const EventArgs & events, int groupId, GUIRadioButton * radio)
	{
		playSoundByType(ST_Click);
		radio->SetWidth(UDim(1.f, 0));
		if (radio->IsSelected())
		{
			m_cur_select = groupId;
			radio->SetWidth(UDim(1.1f, 0));
			vector<SkyBlockPrivilegeGoods>::type cur_Goods;
			SkyBlockPrivilegeShopSetting::Instance()->getShopItemByTabId(cur_Goods, groupId);
			if (groupId == SKYBLOCK_PRI_SUPREME)
			{
				for (auto supemePri : cur_Goods)
				{
					m_normal_title->SetVisible(false);
					m_supeme_title->SetVisible(true);
					m_normalLayout->SetVisible(false);
					m_skyblock_supeme_privilege->show();
					m_skyblock_supeme_privilege->setGood(supemePri);
					break;
				}
			}
			else
			{
				m_normal_title->SetVisible(true);
				m_supeme_title->SetVisible(false);
				m_normalLayout->SetVisible(true);
				m_skyblock_supeme_privilege->hide();

				static unsigned goodsCount = 0;
				m_itemGridView->RemoveAllItems();
				m_itemGridView->SetScrollOffset(0);
				auto cur_count = 0;
				for (SkyBlockPrivilegeGoods item : cur_Goods)
				{
					goodsCount++;
					GUIString strTabName = StringUtil::Format("SkyBlockPrivilegeShop-Content-ItemList-GridView-Item-%d", goodsCount).c_str();
					GuiSkyBlockPrivilegeShopItem* goodsItem = (GuiSkyBlockPrivilegeShopItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_PRIVILEGE_SHOP_ITEM, strTabName);
					goodsItem->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0), UDim(0.36f, 0));
					goodsItem->setGoods(item, cur_count);
					m_itemGridView->AddItem(goodsItem);
					if (cur_count == 0)
					{
						updateShopDetail(item, 0);
					}

					cur_count++;
				}
			}
		}
		return true;
	}

	bool gui_skyBlockPrivilegeShop::onButtonClick(const EventArgs & events, ClickType clickType)
	{
		switch (clickType)
		{
		case ClickType::CLOSE_PANEL:
			playSoundByType(ST_ClosePanel);
			getParent()->showMainControl();
			break;
		case ClickType::BUY_ITEM:
			if (!Blockman::Instance()->m_pPlayer->inventory->SkyBlockisCanBuyPrivilege(m_good.haveItem, 1))
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_STR_APP_SHOP_INVENTORY_IS_FULL);
				return false;
			}

			i32 price = m_good.coinNum;
			UICommon::checkMoney(price, m_good.coinId, [=](bool isSuccess) {
				if (isSuccess)
				{
					if (m_good.coinId == SKYBLOCK_DIAMOND && price >= 100)
					{
						ShowSkyBlockCostMuchMoneyEvent::emit("gui_cost_much_money_tip", m_good.coinId, price, m_good.id, 1, m_good.name);
					}
					else
					{
						GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSkyBlockBuyGoods(1, m_good.id, BUY_GOOD);
					}
				}
			});
		}
		return true;
	}

	bool gui_skyBlockPrivilegeShop::onBuyGoodsResult(const String& msg)
	{
		m_showTipTime = 1;
		m_tipMessage = msg;
		return true;
	}

	bool gui_skyBlockPrivilegeShop::updateShopInfo(int sortType, int level, const String& priData)
	{
		vector<SkyBlockPrivilegeGoods>::type m_goods;

		SkyBlockPrivilegeShopSetting::Instance()->parseUpdateShopData(m_goods, priData);

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
			if (good.tabId == SKYBLOCK_PRI_SUPREME)
			{
				m_skyblock_supeme_privilege->setGood(good);
			}
			else
			{
				auto count = m_itemGridView->GetItemCount();
				for (size_t i = 0; i < count; i++)
				{
					GuiSkyBlockPrivilegeShopItem* shopItem = (GuiSkyBlockPrivilegeShopItem*)m_itemGridView->GetItem(i);
					if (shopItem->m_goods.id == good.id)
					{
						shopItem->setGoods(good, shopItem->m_index);
						break;
					}
				}
			}
		}
		return true;
	}

	bool gui_skyBlockPrivilegeShop::updateShopDetail(const SkyBlockPrivilegeGoods goodData, int index)
	{
		m_good = goodData;
		//m_privilege_name->SetText(LanguageManager::Instance()->getString(goodData.name).c_str());
		static unsigned privilegeCount = 0;
		m_rewardGridView->RemoveAllItems();
		m_rewardGridView->SetScrollOffset(0);
		vector<SkyBlockPrivilegeReward>::type goods;
		SkyBlockPrivilegeShopSetting::Instance()->getPrivilegeRewardItem(goods, m_good.rewards);
		auto cur_count = 0;
		for (SkyBlockPrivilegeReward item : goods)
		{
			privilegeCount++;
			GUIString strTabName = StringUtil::Format("SkyBlockPrivilegeShop-Content-RewardList-GridView-Item-%d", privilegeCount).c_str();
			GuiSkyBlockPrivilegeRewardItem* goodsItem = (GuiSkyBlockPrivilegeRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_PRIVILEGE_REWARD_ITEM, strTabName);
			goodsItem->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.32f, 0), UDim(0.32f, 0));
			goodsItem->setGoods(item, cur_count, SKYBLOCK_PRI_REWARD_NORNAL);
			m_rewardGridView->AddItem(goodsItem);
			if (cur_count == 0)
			{
				updateShopItemDetail(item, 0);
			}
			cur_count++;
		}

		m_buy_btn->SetVisible(true);
		m_buy_text->SetVisible(true);
		m_buy_text->SetText(getString(LanguageKey::SHOP_BTN_BUY));
		if (goodData.isHave)
		{
			m_buy_btn->SetVisible(false);
			m_buy_text->SetText(getString(LanguageKey::GUI_SHOP_IS_HAVE));
		}

		if (index >= 0)
		{
			auto count = m_itemGridView->GetItemCount();
			for (size_t i = 0; i < count; i++)
			{
				GuiSkyBlockPrivilegeShopItem* shopItem = (GuiSkyBlockPrivilegeShopItem*)m_itemGridView->GetItem(i);
				if (shopItem)
				{
					shopItem->m_select_img->SetVisible(false);
					if (i == index)
					{
						shopItem->m_select_img->SetVisible(true);
					}
				}
			}
		}
		return true;
	}
	bool gui_skyBlockPrivilegeShop::updateShopItemDetail(const SkyBlockPrivilegeReward item, int index)
	{
		auto count = m_rewardGridView->GetItemCount();
		for (size_t i = 0; i < count; i++)
		{
			GuiSkyBlockPrivilegeRewardItem* shopItem = (GuiSkyBlockPrivilegeRewardItem*)m_rewardGridView->GetItem(i);
			if (shopItem)
			{
				shopItem->m_select_img->SetVisible(false);
				if (i == index)
				{
					m_item_name->SetText(LanguageManager::Instance()->getString(item.name).c_str());
					m_item_des->SetText(LanguageManager::Instance()->getString(item.detail).c_str());

					shopItem->m_select_img->SetVisible(true);
				}
			}
		}
		return true;
	}

	void gui_skyBlockPrivilegeShop::initTabsAndPanels()
	{
		GUILayout* SupemePrivilegePanel = getWindow<GUILayout>("SkyBlockPrivilegeShop-Supeme-Privilege");
		if (SupemePrivilegePanel == NULL)
			return;
		m_skyblock_supeme_privilege->attachTo(this, SupemePrivilegePanel);

		vector<SkyBlockPrivilegeGoodGroup>::type cur_Goods;
		if (SkyBlockPrivilegeShopSetting::Instance()->getShopItem(cur_Goods))
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
		}
	}
}
