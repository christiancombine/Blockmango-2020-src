#include "gui_skyblock_productUpdate.h"
#include "game.h"
#include "Util/LanguageKey.h"
#include "UI/GUIButton.h"
#include "Util/LanguageManager.h"
#include "ShellInterface.h"
#include "Setting/LogicSetting.h"
#include "Util/UICommon.h"
#include "GUI/GuiItemStack.h"
#include "Item/Item.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_skyblock_productUpdate::gui_skyblock_productUpdate() :
		gui_layout("SkyblockUpdateTip.json")
	{

	}

	gui_skyblock_productUpdate::~gui_skyblock_productUpdate()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_skyblock_productUpdate::onLoad()
	{
		getWindow("SkyblockUpdateTip-SkyBlock-Title-Name")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_TITLE_NAME));
		getWindow("SkyblockUpdateTip-Ad-Name")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_AD_NAME));
		m_current_text = getWindow<GUIStaticText>("SkyblockUpdateTip-Current-Text");
		m_current_text->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_CURRENT_NAME));
		m_next_text = getWindow<GUIStaticText>("SkyblockUpdateTip-Next-Text");
		m_next_text->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_NEXT_NAME));
		m_capacity = getWindow<GUIStaticText>("SkyblockUpdateTip-Capacity-Text");
		m_capacity ->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_CAPACITY_NAME));
		getWindow("SkyblockUpdateTip-Time-Text")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_TIME_TITLE));
		getWindow("SkyblockUpdateTip-Time-Text_4")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_TIME_TITLE));

		m_one = getWindow<GUIStaticText>("SkyblockUpdateTip-Time-Text_2");
		m_two = getWindow<GUIStaticText>("SkyblockUpdateTip-Time-Text_3");
		m_third = getWindow<GUIStaticText>("SkyblockUpdateTip-Time-Text");
		m_four = getWindow<GUIStaticText>("SkyblockUpdateTip-Time-Text_4");
		m_five= getWindow<GUIStaticText>("SkyblockUpdateTip-Time-Text_5");
		m_six = getWindow<GUIStaticText>("SkyblockUpdateTip-Time-Text_6");
		m_current_capacity = getWindow<GUIStaticText>("SkyblockUpdateTip-Current-Capacity");
		m_next_capacity = getWindow<GUIStaticText>("SkyblockUpdateTip-Next-Capacity");

		m_first_current = getWindow<GUIStaticText>("SkyblockUpdateTip-First-Current-Item");
		m_first_current_Icon = getWindow<GUIStaticImage>("SkyblockUpdateTip-First-Current-Icon");
		m_first_next = getWindow<GUIStaticText>("SkyblockUpdateTip-First-Next-Item");
		m_first_next_Icon = getWindow<GUIStaticImage>("SkyblockUpdateTip-First-Next-Icon");
		m_second_current = getWindow<GUIStaticText>("SkyblockUpdateTip-Second-Current-Item");
		m_second_current_Icon = getWindow<GUIStaticImage>("SkyblockUpdateTip-Second-Current-Icon");
		m_second_next = getWindow<GUIStaticText>("SkyblockUpdateTip-Second-Next-Item");
		m_second_next_Icon = getWindow<GUIStaticImage>("SkyblockUpdateTip-Second-Next-Icon");
		m_third_current = getWindow<GUIStaticText>("SkyblockUpdateTip-Third-Current-Item");
		m_third_current_Icon = getWindow<GUIStaticImage>("SkyblockUpdateTip-Third-Current-Icon");
		m_third_next = getWindow<GUIStaticText>("SkyblockUpdateTip-Third-Next-Item");
		m_third_next_Icon = getWindow<GUIStaticImage>("SkyblockUpdateTip-Third-Next-Icon");
		m_first_current_num = getWindow<GUIStaticText>("SkyblockUpdateTip-First-Current-Num");
		m_first_next_num = getWindow<GUIStaticText>("SkyblockUpdateTip-First-Next-Num");
		m_second_current_num = getWindow<GUIStaticText>("SkyblockUpdateTip-Second-Current-Num");
		m_second_next_num = getWindow<GUIStaticText>("SkyblockUpdateTip-Second-Next-Num");
		m_third_current_num = getWindow<GUIStaticText>("SkyblockUpdateTip-Third-Current-Num");
		m_third_next_num = getWindow<GUIStaticText>("SkyblockUpdateTip-Third-Next-Num");
		m_price = getWindow<GUIStaticText>("SkyblockUpdateTip-SkyBlock-Content-Currency-Value");
		m_discount = getWindow<GUIStaticText>("SkyblockUpdateTip-Discount-Text");
		m_adprice = getWindow<GUIStaticText>("SkyblockUpdateTip-Cost-Text");
		m_price_png = getWindow<GUIStaticImage>("SkyblockUpdateTip-SkyBlock-Content-Currency-Icon");

		auto btnClose = getWindow<GUIButton>("SkyblockUpdateTip-SkyBlock-Title-Btn-Close");
		btnClose->subscribeEvent(EventButtonClick, std::bind(&gui_skyblock_productUpdate::onCloseClick, this, std::placeholders::_1));

	    btnSure = getWindow<GUIButton>("SkyblockUpdateTip-SkyBlock-Btn-Sure");
		btnSure->subscribeEvent(EventButtonClick, std::bind(&gui_skyblock_productUpdate::onSureClick, this, std::placeholders::_1));

		btnCost = getWindow<GUIButton>("SkyblockUpdateTip-SkyBlock-Btn-Cost");
		btnCost->SetVisible(false);
		btnCost->subscribeEvent(EventButtonClick, std::bind(&gui_skyblock_productUpdate::onCostClick, this, std::placeholders::_1));

		btnAd = getWindow<GUIButton>("SkyblockUpdateTip-SkyBlock-Btn-Ad");
		btnAd->SetVisible(false);
		btnAd->subscribeEvent(EventButtonClick, std::bind(&gui_skyblock_productUpdate::onAdClick, this, std::placeholders::_1));

		m_arrow = getWindow<GUIStaticImage>("SkyblockUpdateTip-Arrowhead");

		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_skyblock_productUpdate::onCubeAdVisible, this, std::placeholders::_1)));
	}
	bool gui_skyblock_productUpdate::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockProduct);
	}

	bool gui_skyblock_productUpdate::onCubeAdVisible(bool visible)
	{
		LordLogInfo("gui_skyblock_productUpdate::onCubeAdVisible %d", visible ? 1 : 0);
		m_have_cube_ad = visible;
		return true;
	}
	
	void gui_skyblock_productUpdate::onUpdate(ui32 nTimeElapse)
	{
		
	}

	void gui_skyblock_productUpdate::onGetCommonData(String key, String data)
	{
		if (key == "UpdateProduct")
		{
			showUpdateProductMessage(data);
		}
	}

	void gui_skyblock_productUpdate::showUpdateProductMessage(const String& data)
	{
		if (data.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}

		if (!doc->HasMember("adPrice") || !doc->HasMember("adType") || !doc->HasMember("coins") || !doc->HasMember("prices") || !doc->HasMember("current_item1") || !doc->HasMember("current_num1") ||
			!doc->HasMember("current_item2") || !doc->HasMember("current_num2") || !doc->HasMember("current_item3") || !doc->HasMember("current_num3") || !doc->HasMember("next_item1") ||
			!doc->HasMember("next_num1") || !doc->HasMember("next_item2") || !doc->HasMember("next_num2") || !doc->HasMember("next_item3") || !doc->HasMember("next_num3") || !doc->HasMember("npcid") ||
			!doc->HasMember("npclevel")|| !doc->HasMember("isMax") || !doc->HasMember("current_capacity") || !doc->HasMember("next_capacity"))
		{
			LordLogError("gui_skyblock_productUpdate::getItemInfo The game result content player_info missed some field.");
			LordDelete(doc);
			return;
		}

		m_first_next_num->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_first_current_num->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_second_next_num->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_second_current_num->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_third_next_num->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_third_current_num->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));

		m_one->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_two->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_third->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_four->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_five->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_six->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_current_capacity->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));
		m_next_capacity->SetTextColor(Color(0.560784f, 0.427451f, 0.2352941f));

		int isMax = doc->FindMember("isMax")->value.GetInt();
		if (isMax == 1)
		{
			btnAd->SetVisible(false);
			btnCost->SetVisible(false);
			btnSure->SetVisible(false);
			m_arrow->SetVisible(false);
			m_next_text->SetVisible(false);
			m_first_next->SetVisible(false);
			m_first_next_Icon->SetVisible(false);
			m_first_next_num->SetVisible(false);
			m_four->SetVisible(false);
			m_price_png->SetVisible(false);
			m_price->SetVisible(false);
			m_next_capacity->SetVisible(false);
			m_current_text->SetText(getString("Lv.MAX"));
			m_current_text->SetArea({ 0.425f , 0.0f }, { 0.030f, 0.0f }, { 0.209f, 0.0f }, { 0.147f, 0.0f });
			m_first_current_Icon->SetArea({ 0.661f , 0.0f }, { 0.0f, 0.0f }, { 0.194f, 0.0f }, { 1.0f, 0.0f });
			m_first_current_num->SetArea({ 0.866f , 0.0f }, { 0.0f, 0.0f }, { 0.388f, 0.0f }, { 1.0f, 0.0f });
			m_third->SetArea({ 1.35f , 0.0f }, { 0.0f, 0.0f }, { 0.333f, 0.0f }, { 1.0f, 0.0f });
			m_second_current_Icon->SetArea({ 0.661f , 0.0f }, { 0.0f, 0.0f }, { 0.194f, 0.0f }, { 1.0f, 0.0f });
			m_second_current_num->SetArea({ 0.866f , 0.0f }, { 0.0f, 0.0f }, { 0.388f, 0.0f }, { 1.0f, 0.0f });
			m_one->SetArea({ 1.35f , 0.0f }, { 0.0f, 0.0f }, { 0.333f, 0.0f }, { 1.0f, 0.0f });
			m_third_current_Icon->SetArea({ 0.661f , 0.0f }, { 0.0f, 0.0f }, { 0.194f, 0.0f }, { 1.0f, 0.0f });
			m_third_current_num->SetArea({ 0.866f , 0.0f }, { 0.0f, 0.0f }, { 0.388f, 0.0f }, { 1.0f, 0.0f });
			m_five->SetArea({ 1.35f , 0.0f }, { 0.0f, 0.0f }, { 0.333f, 0.0f }, { 1.0f, 0.0f });
			m_current_capacity->SetArea({ 0.545f , 0.0f }, { 0.595f, 0.0f }, { 0.096f, 0.0f }, { 0.116f, 0.0f });
			m_capacity->SetArea({ 0.349f , 0.0f }, { 0.595f, 0.0f }, { 0.164f, 0.0f }, { 0.116f, 0.0f });
		}
		else 
		{
			btnAd->SetVisible(true);
			btnCost->SetVisible(true);
			btnSure->SetVisible(true);
			m_arrow->SetVisible(true);
			m_next_text->SetVisible(true);
			m_first_next->SetVisible(true);
			m_first_next_Icon->SetVisible(true);
			m_first_next_num->SetVisible(true);
			m_four->SetVisible(true);
			m_price_png->SetVisible(true);
			m_price->SetVisible(true);
			m_next_capacity->SetVisible(true);
			m_current_text->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_CURRENT_NAME));
			m_current_text->SetArea({ 0.187f , 0.0f }, { 0.028f, 0.0f }, { 0.209f, 0.0f }, { 0.149f, 0.0f });
			m_first_current_Icon->SetArea({ 0.0f , 0.0f }, { 0.0f, 0.0f }, { 0.2f, 0.0f }, { 1.0f, 0.0f });
			m_first_current_num->SetArea({ 0.411f , 0.0f }, { 0.028f, 0.0f }, { 0.194f, 0.0f }, { 1.0f, 0.0f });
			m_third->SetArea({ 0.666f , 0.0f }, { 0.0f, 0.0f }, { 0.333f, 0.0f }, { 1.0f, 0.0f });
			m_second_current_Icon->SetArea({ 0.0f , 0.0f }, { 0.0f, 0.0f }, { 0.2f, 0.0f }, { 1.0f, 0.0f });
			m_second_current_num->SetArea({ 0.411f , 0.0f }, { 0.057f, 0.0f }, { 0.194f, 0.0f }, { 1.0f, 0.0f });
			m_one->SetArea({ 0.666f , 0.0f }, { 0.0f, 0.0f }, { 0.333f, 0.0f }, { 1.0f, 0.0f });
			m_third_current_Icon->SetArea({ 0.0f , 0.0f }, { 0.0f, 0.0f }, { 0.2f, 0.0f }, { 1.0f, 0.0f });
			m_third_current_num->SetArea({ 0.411f , 0.0f }, { 0.057f, 0.0f }, { 0.194f, 0.0f }, { 1.0f, 0.0f });
			m_five->SetArea({ 0.666f , 0.0f }, { 0.0f, 0.0f }, { 0.333f, 0.0f }, { 1.0f, 0.0f});
			m_current_capacity->SetArea({ 0.342f , 0.0f }, { 0.595f, 0.0f }, { 0.096f, 0.0f }, { 0.116f, 0.0f });
			m_capacity->SetArea({ 0.149f , 0.0f }, { 0.595f, 0.0f }, { 0.164f, 0.0f }, { 0.116f, 0.0f });
		}

		int first_current = doc->FindMember("current_item1")->value.GetInt();
		Item* pItem = Item::getItemById(first_current);
		if (pItem)
		{
// 			String name = pItem->getUnlocalizedName().c_str();
// 			m_first_current->SetText(LanguageManager::Instance()->getItemName(name.c_str()).c_str());
			GuiItemStack::showItemIcon(m_first_current_Icon, first_current, 0);
		}
		
		int first_next = doc->FindMember("next_item1")->value.GetInt();
		Item* pItem1 = Item::getItemById(first_next);
		if (pItem1)
		{
// 			String name = pItem1->getUnlocalizedName().c_str();
// 			m_first_next->SetText(LanguageManager::Instance()->getItemName(name.c_str()).c_str());
			GuiItemStack::showItemIcon(m_first_next_Icon, first_next,0);
		}
		
		int second_current = doc->FindMember("current_item2")->value.GetInt();
		if (second_current == 0)
		{
			m_second_current->SetVisible(false);
			m_one->SetVisible(false);
		}
		else
		{

			Item* pItem2 = Item::getItemById(second_current);
			if (pItem2)
			{
// 				String name = pItem2->getUnlocalizedName().c_str();
// 				m_second_current->SetText(LanguageManager::Instance()->getItemName(name.c_str()).c_str());
				GuiItemStack::showItemIcon(m_second_current_Icon, second_current, 0);
			}
			m_second_current->SetVisible(true);
			m_one->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_TIME_TITLE));
			m_one->SetVisible(true);
			
		}

		int third_current = doc->FindMember("current_item3")->value.GetInt();
		if (third_current == 0)
		{
			m_third_current->SetVisible(false);
			m_five->SetVisible(false);
		}
		else
		{

			Item* pItem2 = Item::getItemById(third_current);
			if (pItem2)
			{
// 				String name = pItem2->getUnlocalizedName().c_str();
// 				m_third_current->SetText(LanguageManager::Instance()->getItemName(name.c_str()).c_str());
				GuiItemStack::showItemIcon(m_third_current_Icon, third_current, 0);
			}
			m_third_current->SetVisible(true);
			m_five->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_TIME_TITLE));
			m_five->SetVisible(true);

		}
		
		int second_next = doc->FindMember("next_item2")->value.GetInt();
		if (second_next == 0)
		{
			m_second_next->SetVisible(false);
			m_two->SetVisible(false);
		}
		else
		{
			Item* pItem3 = Item::getItemById(second_next);
			if (pItem3)
			{
// 				String name = pItem3->getUnlocalizedName().c_str();
// 				m_second_next->SetText(LanguageManager::Instance()->getItemName(name.c_str()).c_str());
				GuiItemStack::showItemIcon(m_second_next_Icon, second_next, 0);
			}
			m_second_next->SetVisible(true);
			m_two->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_TIME_TITLE));
			m_two->SetVisible(true);
		}

		int third_next = doc->FindMember("next_item3")->value.GetInt();
		if (third_next == 0)
		{
			m_third_next->SetVisible(false);
			m_six->SetVisible(false);
		}
		else
		{
			Item* pItem3 = Item::getItemById(third_next);
			if (pItem3)
			{
// 				String name = pItem3->getUnlocalizedName().c_str();
// 				m_third_next->SetText(LanguageManager::Instance()->getItemName(name.c_str()).c_str());
				GuiItemStack::showItemIcon(m_third_next_Icon, third_next, 0);
			}
			m_third_next->SetVisible(true);
			m_six->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UPDATE_TIME_TITLE));
			m_six->SetVisible(true);
		}
		
		int first_current_num = doc->FindMember("current_num1")->value.GetInt();
		m_first_current_num->SetText(StringUtil::Format("%d", first_current_num).c_str());
		int first_next_num = doc->FindMember("next_num1")->value.GetInt();
		m_first_next_num->SetText(StringUtil::Format("%d", first_next_num).c_str());
		if (first_current!= first_next || first_current_num != first_next_num)
		{
			m_first_next_num->SetTextColor(Color::RED);
			m_first_current_num->SetTextColor(Color::RED);
			m_four->SetTextColor(Color::RED);
			m_third->SetTextColor(Color::RED);
		}
		
		int second_current_num = doc->FindMember("current_num2")->value.GetInt();
		if (second_current_num == 0)
		{
			m_second_current_num->SetVisible(false);
		}
		else
		{
			m_second_current_num->SetText(StringUtil::Format("%d", second_current_num).c_str());
			m_second_current_num->SetVisible(true);
		}

		int second_next_num = doc->FindMember("next_num2")->value.GetInt();
		if (second_next_num == 0)
		{
			m_second_next_num->SetVisible(false);
		}
		else
		{
			m_second_next_num->SetText(StringUtil::Format("%d", second_next_num).c_str());
			m_second_next_num->SetVisible(true);
		}

		if (second_current != second_next || second_current_num != second_next_num)
		{
			m_second_next_num->SetTextColor(Color::RED);
			m_second_current_num->SetTextColor(Color::RED);
			m_one->SetTextColor(Color::RED);
			m_two->SetTextColor(Color::RED);
		}

		int third_current_num = doc->FindMember("current_num3")->value.GetInt();
		if (third_current_num == 0)
		{
			m_third_current_num->SetVisible(false);
		}
		else
		{
			m_third_current_num->SetText(StringUtil::Format("%d", third_current_num).c_str());
			m_third_current_num->SetVisible(true);
		}

		int third_next_num = doc->FindMember("next_num3")->value.GetInt();
		if (third_next_num == 0)
		{
			m_third_next_num->SetVisible(false);
		}
		else
		{
			m_third_next_num->SetText(StringUtil::Format("%d", third_next_num).c_str());
			m_third_next_num->SetVisible(true);
		}

		if (third_current != third_next || third_current_num != third_next_num)
		{
			m_third_next_num->SetTextColor(Color::RED);
			m_third_current_num->SetTextColor(Color::RED);
			m_five->SetTextColor(Color::RED);
			m_six->SetTextColor(Color::RED);
		}

		int current_capacity = doc->FindMember("current_capacity")->value.GetInt();
		m_current_capacity->SetText(StringUtil::Format("%d", current_capacity).c_str());
		int next_capacity = doc->FindMember("next_capacity")->value.GetInt();
		if (next_capacity == 0)
		{
			m_next_capacity->SetVisible(false);
		}
		else
		{
			m_next_capacity->SetText(StringUtil::Format("%d", next_capacity).c_str());
			m_next_capacity->SetVisible(true);
		}
		
		if (current_capacity != next_capacity)
		{
			m_current_capacity->SetTextColor(Color::RED);
			m_next_capacity->SetTextColor(Color::RED);
		}
	
		int price = doc->FindMember("prices")->value.GetInt();
		m_price->SetText(StringUtil::Format("%d", price).c_str());
	    m_unlockCost = price;

		int adPrice = doc->FindMember("adPrice")->value.GetInt();
		m_costprice = adPrice;

		int coinType = doc->FindMember("coins")->value.GetInt();
		m_unlockType = coinType;
		int adType = doc->FindMember("adType")->value.GetInt();

		int npcid = doc->FindMember("npcid")->value.GetInt();
		m_npcid = npcid;
		int npclevel = doc->FindMember("npclevel")->value.GetInt();
		m_npclevel = npclevel;
		

		if (coinType == 1)
		{
			m_price_png->SetImage("set:app_shop.json image:app_shop_diamonds_mods");
		}
		if (coinType == 3)
		{
			m_price_png->SetImage("set:items.json image:currency_money");
		}

		if (adType == 0)
		{
			int percent = (int)(((price - adPrice) * 1.0 / price) * 100);
			m_discount->SetText(StringUtil::Format("-%d%%", percent).c_str());
			if (m_have_cube_ad)
			{
				btnAd->SetVisible(true);
			}
			else
			{
				btnAd->SetVisible(false);
			}
			btnCost->SetVisible(false);
		}
		if (adType == 1)
		{
			m_adprice ->SetText(StringUtil::Format("%d", adPrice).c_str());
			btnCost->SetVisible(true);
			btnAd->SetVisible(false);
		}

		if (adType == 2)
		{
			btnAd->SetVisible(false);
			btnCost->SetVisible(false);
		}
		
		LordDelete(doc);
		getParent()->showWindowByLayoutId(RootGuiLayout::LayoutId::UPDATE_PRODUCT_INFO);
	}

	bool gui_skyblock_productUpdate::onCostClick(const EventArgs & events)
	{
		UICommon::checkMoney(m_costprice, CurrencyType::PLATFORM_MONEY, [=](bool isSuccess) {
			if (isSuccess)
			{
				GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendClickBuyUpdate(RIGHT_MAGIC, m_npcid);
				getParent()->showMainControl();
			}
		});
		return true;
	}

	bool gui_skyblock_productUpdate::onAdClick(const EventArgs & events)
	{
		String str = StringUtil::Format("%s", "npcLevelUp#0").c_str();
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd(104802, str);
		GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendWatchAdUpdateInfo(m_npcid, m_npclevel);
		return true;
	}

	bool gui_skyblock_productUpdate::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_skyblock_productUpdate::onSureClick(const EventArgs & events)
	{
		switch (m_unlockType)
		{
		case 1:
			UICommon::checkMoney(m_unlockCost, CurrencyType::PLATFORM_MONEY, [=](bool isSuccess) {
				if (isSuccess)
				{
					GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendClickBuyUpdate(LEFT_MAGIC, m_npcid);
					getParent()->showMainControl();
				}
			});
			break;
		case 3:
			GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendClickBuyUpdate(LEFT_MONEY, m_npcid);
			getParent()->showMainControl();
			break;
		default:
			break;
		}
		return true;
	}

}


	

	

