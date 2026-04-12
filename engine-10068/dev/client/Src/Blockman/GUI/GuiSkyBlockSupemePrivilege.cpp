#include "Util/ClientEvents.h"
#include "Network/ClientNetwork.h"
#include "Util/LanguageKey.h"
#include "Util/UICommon.h"
#include "GuiSkyBlockSupemePrivilege.h"

#include "GUI/GuiSkyBlockPrivilegeRewardItem.h"
#include "GUI/GuiDef.h"
#include "UI/GUIStaticText.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/InventoryPlayer.h"
#include "game.h"
#include "GameAnalytics/GameAnalyticsSetting.h"

namespace BLOCKMAN
{
	GuiSkyBlockSupemePrivilege::GuiSkyBlockSupemePrivilege() :
		gui_layout("SkyBlockSupemePrivilege.json")
	{

	}

	GuiSkyBlockSupemePrivilege::~GuiSkyBlockSupemePrivilege()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GuiSkyBlockSupemePrivilege::onLoad()
	{
		auto ListLayout = getWindow<GUILayout>("SkyBlockSupemePrivilege-Reward-List");
		m_buy_btn = getWindow<GUIButton>("SkyBlockSupemePrivilege-Buy-Btn");
		m_buy_btn->subscribeEvent(EventButtonClick, std::bind(&GuiSkyBlockSupemePrivilege::onButtonClick, this, std::placeholders::_1));
		m_reward_gridview = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "SkyBlockSupemePrivilege-Content-ItemList-GridView");
		m_reward_gridview->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_reward_gridview->InitConfig(4.f, 15.f, 9);
		ListLayout->AddChildWindow(m_reward_gridview);

		m_subscriptionGuard.add(SkyBlockSupremeClickItemEvent::subscribe(std::bind(&GuiSkyBlockSupemePrivilege::updateShopItemDetail, this, std::placeholders::_1)));
	}

	void GuiSkyBlockSupemePrivilege::setGood(const SkyBlockPrivilegeGoods good)
	{
		m_good = good;
		getWindow<GUIStaticText>("SkyBlockSupemePrivilege-Title")->SetText(LanguageManager::Instance()->getString(m_good.name).c_str());
		getWindow<GUIStaticText>("SkyBlockSupemePrivilege-Describe")->SetText(LanguageManager::Instance()->getString(m_good.detail).c_str());
		auto money_icon = getWindow<GUIStaticImage>("SkyBlockSupemePrivilege-Coin-Icon");
		auto buy_text = getWindow<GUIStaticText>("SkyBlockSupemePrivilege-Coin-Num");
		auto have_tip = getWindow<GUIStaticText>("SkyBlockSupemePrivilege-Have-Tip");
		money_icon->SetImage(UICommon::getCurrencyIconByType(m_good.coinId).c_str());
		buy_text->SetText(StringUtil::ToString(m_good.coinNum).c_str());
		m_buy_btn->SetVisible(!m_good.isHave);
		have_tip->SetText(getString(LanguageKey::GUI_SHOP_IS_HAVE));
		have_tip->SetVisible(m_good.isHave);

		static unsigned supemePrivilegeCount = 0;
		m_reward_gridview->RemoveAllItems();
		m_reward_gridview->SetScrollOffset(0);
		vector<SkyBlockPrivilegeReward>::type goods;
		SkyBlockPrivilegeShopSetting::Instance()->getPrivilegeRewardItem(goods, m_good.rewards);

		auto cur_count = 0;
		for (SkyBlockPrivilegeReward item : goods)
		{
			supemePrivilegeCount++;
			GUIString strTabName = StringUtil::Format("SkyBlockSupemePrivilege-Content-RewardList-GridView-Item-%d", supemePrivilegeCount).c_str();
			GuiSkyBlockPrivilegeRewardItem* goodsItem = (GuiSkyBlockPrivilegeRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_PRIVILEGE_REWARD_ITEM, strTabName);
			goodsItem->SetArea(UDim(0, 0), UDim(0, 10), UDim(0., 74.f), UDim(0, 74.f));
			goodsItem->setGoods(item, cur_count, SKYBLOCK_PRI_REWARD_SUPREME);
			m_reward_gridview->AddItem(goodsItem);
			cur_count++;
		}

	}

	bool GuiSkyBlockSupemePrivilege::onButtonClick(const EventArgs & events)
	{
		if (GameClient::CGame::Instance()->isEnableIndie())
			GameAnalyticsSetting::Instance()->design(0, { "g1048common_click_indie", "6"});
		else
			GameAnalyticsSetting::Instance()->design(0, { "g1048common_click", "6" });

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
					GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSkyBlockBuyGoods(1, m_good.id, 0);
				}
			}
		});
		return true;
	}

	bool GuiSkyBlockSupemePrivilege::updateShopItemDetail(int index)
	{
		auto count = m_reward_gridview->GetItemCount();
		for (size_t i = 0; i < count; i++)
		{
			GuiSkyBlockPrivilegeRewardItem* shopItem = (GuiSkyBlockPrivilegeRewardItem*)m_reward_gridview->GetItem(i);
			if (shopItem)
			{
				shopItem->m_select_img->SetVisible(false);
				if (i == index)
				{
					shopItem->m_select_img->SetVisible(true);
				}
			}
		}
		return true;
	}
}
