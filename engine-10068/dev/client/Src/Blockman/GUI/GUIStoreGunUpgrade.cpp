#include "GUIStoreGunUpgrade.h"
#include "GUI/GUIStorePropertyItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{
	GUIStoreGunUpgrade::GUIStoreGunUpgrade() :
		gui_layout("StoreGunUpgrade.json")
	{
	}

	GUIStoreGunUpgrade::~GUIStoreGunUpgrade()
	{

	}

	void GUIStoreGunUpgrade::onLoad()
	{
		auto Title = getWindow<GUIStaticText>("StoreGunUpgrade-Title");
		Title->SetText(getString("gui_upgrade_title").c_str());

		auto GoldName = getWindow<GUIStaticText>("StoreGunUpgrade-Gold-Name");
		GoldName->SetText(getString("gui_gold_name").c_str());
		m_gold_icon = getWindow<GUIStaticImage>("StoreGunUpgrade-Gold-Image");
		m_gold_has = getWindow<GUIStaticText>("StoreGunUpgrade-Gold-Has");
		m_gold_price = getWindow<GUIStaticText>("StoreGunUpgrade-Gold-Price");
		m_gold_time = getWindow<GUIStaticText>("StoreGunUpgrade-Gold-Time");

		auto TicketName = getWindow<GUIStaticText>("StoreGunUpgrade-Ticket-Name");
		TicketName->SetText(getString("gui_ticket_name_%d").c_str());
		m_ticket_icon = getWindow<GUIStaticImage>("StoreGunUpgrade-Ticket-Image");
		m_ticket_has = getWindow<GUIStaticText>("StoreGunUpgrade-Ticket-Has");
		m_ticket_price = getWindow<GUIStaticText>("StoreGunUpgrade-Ticket-Price");
		m_ticket_time = getWindow<GUIStaticText>("StoreGunUpgrade-Ticket-Time");

		auto Close = getWindow<GUIButton>("StoreGunUpgrade-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&GUIStoreGunUpgrade::onCloseClick, this, std::placeholders::_1));
		auto Buy = getWindow<GUIButton>("StoreGunUpgrade-Ticket-Buy");
		Buy->subscribeEvent(EventButtonClick, std::bind(&GUIStoreGunUpgrade::onBuyClick, this, std::placeholders::_1));
		auto GoldUpgrade = getWindow<GUIButton>("StoreGunUpgrade-Gold-Upgrade");
		GoldUpgrade->SetText(getString("gui_upgrade").c_str());
		GoldUpgrade->subscribeEvent(EventButtonClick, std::bind(&GUIStoreGunUpgrade::onUpgradeClick, this, std::placeholders::_1, true));
		auto TicketUpgrade = getWindow<GUIButton>("StoreGunUpgrade-Ticket-Upgrade");
		TicketUpgrade->SetText(getString("gui_upgrade").c_str());
		TicketUpgrade->subscribeEvent(EventButtonClick, std::bind(&GUIStoreGunUpgrade::onUpgradeClick, this, std::placeholders::_1, false));
	}

	void GUIStoreGunUpgrade::onShowGunUpgrade(int gunId)
	{
		StoreGun* gun = GunStoreSetting::findStoreGunById(gunId);
		
		if (!gun )
		{
			hide();
			return;
		}
		StoreGunLevel* level = GunStoreSetting::findStoreGunLevelByGunIdAndLevel(gun->Id, gun->Level);

		m_gold_icon->SetImage(GunStoreSetting::getPriceIcon(level->UpgradePriceType).c_str());
		
		m_gold_price->SetText(StringUtil::Format("%s:%s",
			getString("gui_upgrade_need").c_str(),
			GunStoreSetting::getPriceText(level->UpgradePriceType, gun->Chip, level->UpgradePrice).c_str()).c_str());
		m_gold_time = getWindow<GUIStaticText>("StoreGunUpgrade-Gold-Time");
		setTimeText(level->UpgradeTime);

		auto player = Blockman::Instance() ? Blockman::Instance()->m_pPlayer : nullptr;
		if (player)
		{
			Wallet * wallet = player->m_wallet;
			if (wallet)
			{
				m_gold_has->SetText(StringUtil::Format("%s:%s",
					getString("gui_upgrade_has").c_str(),
					StringUtil::ToString(wallet->getCurrency()).c_str()).c_str());
			}
		}

		auto TicketName = getWindow<GUIStaticText>("StoreGunUpgrade-Ticket-Name");
		TicketName->SetText(getString(StringUtil::Format("gui_ticket_name_%d", gun->TabType)).c_str());
		m_ticket_icon->SetImage(StringUtil::Format("set:gun_upgrade.json image:gun_%d", gun->TabType).c_str());
		m_ticket_has->SetText(StringUtil::Format("%s:%s",
			getString("gui_upgrade_has").c_str(),
			StringUtil::ToString(GunStoreSetting::findTicketNumByType(gun->TabType)).c_str()).c_str());
		m_ticket_price->SetText(StringUtil::Format("%s:%s",
			getString("gui_upgrade_need").c_str(),
			StringUtil::ToString(level->TicketPrice).c_str()).c_str());
		m_ticket_time = getWindow<GUIStaticText>("StoreGunUpgrade-Ticket-Time");
		m_ticket_time->SetText(getString("gui_ticket_time").c_str());

		return;
	}

	bool GUIStoreGunUpgrade::onCloseClick(const EventArgs & events)
	{
		hide();
		return true;
	}

	bool GUIStoreGunUpgrade::onBuyClick(const EventArgs & events)
	{
		hide();
		RootGuiLayout::Instance()->showPixelGunGiftBagStore(true);
		return true;
	}

	bool GUIStoreGunUpgrade::onUpgradeClick(const EventArgs & events, bool needWait)
	{
		OnGunUpgradeEvent::emit(needWait);
		hide();
		return false;
	}

	void GUIStoreGunUpgrade::setTimeText(int time)
	{
		int seconds = time;
		int second = seconds % 60;
		int minute = (seconds % 3600) / 60;
		int hour = seconds / 3600;
		m_gold_time->SetText(StringUtil::Format("%s%s:%s:%s",
			getString("gui_gold_time").c_str(),
			hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
			minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
			second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
	}

}
