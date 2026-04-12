#include "gui_skyblockUnlockTip.h"
#include "game.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "UI/GUIButton.h"
#include "Util/LanguageManager.h"
#include "ShellInterface.h"
#include "Setting/GameTypeSetting.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/LogicSetting.h"
#include "GUI/GuiItemStack.h"
#include "Plugins/GameClientEvents.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_skyblockUnlockTip::gui_skyblockUnlockTip():
		gui_layout("SkyblockUnLockTip.json")
	{

	}

	gui_skyblockUnlockTip::~gui_skyblockUnlockTip()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_skyblockUnlockTip::onLoad()
	{
		getWindow("SkyblockUnLockTip-Panel")->SetVisible(true);
		getWindow("SkyblockUnLockTip-Title")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_UNLOCK_TIP));

		m_icon = getWindow<GUIStaticImage>("SkyblockUnLockTip-Icon");
		m_icon_name = getWindow<GUIStaticText>("SkyblockUnLockTip-Icon-Name");
		m_hint_msg = getWindow<GUIStaticText>("SkyblockUnLockTip-Info");
		m_price_icon = getWindow<GUIStaticImage>("SkyblockUnLockTip-Price-Icon");
		m_price_number = getWindow<GUIStaticText>("SkyblockUnLockTip-Price-Number");
		
		auto btnClose = getWindow<GUIButton>("SkyblockUnLockTip-Close");
		btnClose->subscribeEvent(EventButtonClick, std::bind(&gui_skyblockUnlockTip::onCloseClick, this, std::placeholders::_1));

		auto btnSure = getWindow<GUIButton>("SkyblockUnLockTip-Cost-Btn");
		btnSure->subscribeEvent(EventButtonClick, std::bind(&gui_skyblockUnlockTip::onSureClick, this, std::placeholders::_1));

	}
		
	bool gui_skyblockUnlockTip::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockProduct);
	}
	
	void gui_skyblockUnlockTip::onUpdate(ui32 nTimeElapse)
	{
		
	}

	bool gui_skyblockUnlockTip::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	void gui_skyblockUnlockTip::onGetCommonData(String key, String data)
	{
		if (key == "UnlockTip")
		{
			showUnlockTipMessage(data);
		}
	}

	void gui_skyblockUnlockTip::showUnlockTipMessage(const String& data)
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

		if (!doc->HasMember("ItemId") || !doc->HasMember("Icon") || !doc->HasMember("Name") || !doc->HasMember("UnlockDetail") || !doc->HasMember("CoinId") || !doc->HasMember("CoinNum1"))
		{
			LordLogError("gui_skyblockUnlockTip::getItemInfo The game result content player_info missed some field.");
			LordDelete(doc);
			return;
		}

		int itemId = doc->FindMember("ItemId")->value.GetInt();
		m_shopId = itemId;

		String icon = doc->FindMember("Icon")->value.GetString();
		m_icon->SetImage(icon.c_str());

		int coinType = doc->FindMember("CoinId")->value.GetInt();
		m_unlockType = coinType;
		if (coinType == 1)
		{
			m_price_icon->SetImage("set:app_shop.json image:app_shop_diamonds_mods");
		}
		if (coinType == 3)
		{
			m_price_icon->SetImage("set:items.json image:currency_money");
		}

		int price = doc->FindMember("CoinNum1")->value.GetInt();
		m_costprice = price;
		m_price_number->SetText(StringUtil::Format("%d", price).c_str());

		String icon_name = doc->FindMember("Name")->value.GetString();
		m_icon_name->SetText(LanguageManager::Instance()->getString(icon_name).c_str());

		String hint_msg = doc->FindMember("UnlockDetail")->value.GetString();
		m_hint_msg->SetText(LanguageManager::Instance()->getString(hint_msg).c_str());

		getParent()->showWindowByLayoutId(RootGuiLayout::LayoutId::UNLOCK_PRODUCT_INFO);

	}

	bool gui_skyblockUnlockTip::onSureClick(const EventArgs & events)
	{
		if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
		{
			Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
			switch (m_unlockType)
			{
			case 1:
				if (wallet)
				{
					i64 money = wallet->getMoneyByCoinId(CurrencyType::PLATFORM_MONEY);
					if (money >= m_costprice)
					{
						GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSkyBlockBuyGoods(1, m_shopId, 0);
					}
					else
					{
						i32 deltaMoney = i32(m_costprice - money);
						ShowLackOfMoneyTipDialogEvent::emit(deltaMoney, CurrencyType::PLATFORM_MONEY);
						return false;
					}
				}
				break;
			case 3:
				if (wallet)
				{
					i64 money = wallet->getMoneyByCoinId(CurrencyType::GAME_MONEY);
					if (money >= m_costprice)
					{
						GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSkyBlockBuyGoods(1, m_shopId, 0);
					}
					else
					{
						getParent()->showMainControl();
						SCRIPT_EVENT::SkyBlockShowLackMoney::invoke(true);
						return false;
					}
				}
				break;
			default:
				break;
			}
		}
		getParent()->showMainControl();
		return true;
	}
}


	

	

