#include "gui_tipDialog.h"
#include "game.h"
#include "Util/LanguageKey.h"
#include "Setting/CarSetting.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "UI/GUIButton.h"
#include "Entity/EntityMerchant.h"
#include "Entity/EntityItem.h"
#include "Util/LanguageManager.h"
#include "Item/Item.h"
#include "Item/ItemStack.h"
#include "GuiItemStack.h"
#include "Inventory/CoinManager.h"
#include "ShellInterface.h"
#include "Object/Root.h"
#include "Util/UICommon.h"
#include "Inventory/InventoryPlayer.h"
#include "Setting/GameTypeSetting.h"
#include "Setting/LogicSetting.h"
#include "Setting/AppItemsSetting.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Plugins/GameClientEvents.h"
#include "Util/PlayerPrefs.h"
#include "Audio/SoundSystem.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_tipDialog::gui_tipDialog() :
		gui_layout("TipDialog.json")
	{

	}


	gui_tipDialog::~gui_tipDialog()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_tipDialog::onLoad()
	{
		auto defaultPanel = getWindow("TipDialog-Panel");
		defaultPanel->SetVisible(false);
		auto bedWarPanel = getWindow("TipDialog-BedWar");
		bedWarPanel->SetVisible(false);
		auto skyBlockPanel = getWindow("TipDialog-SkyBlock");
		skyBlockPanel->SetVisible(false);
		auto pixelGunPanel = getWindow("TipDialog-Pixel-Gun-Panel");
		pixelGunPanel->SetVisible(false);
		m_nolongerRemind = getWindow("TipDialog-Nolonger_Remind");
		m_nolongerRemindCheck = getWindow<GUICheckBox>("TipDialog-Nolonger_Remind_Check");
		m_nolongerRemindText = getWindow<GUIStaticText>("TipDialog-Nolonger_Remind-Text");
		if (m_nolongerRemind)
		{
			m_nolongerRemind->SetVisible(false);
		}
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::PixelGunHall:
		case ClientGameType::PixelGunGameTeam:
		case ClientGameType::PixelGunGamePerson:
		case ClientGameType::PixelGunGame1v1:
			pixelGunPanel->SetVisible(true);
			m_titleText = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Title-Name");
			m_messageText = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-Vehicle-Message");
			m_valueText = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-Currency-Value");
			m_otherMsgText = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-Other-Message");
			m_iconImage = getWindow<GUIStaticImage>("TipDialog-Pixel-Gun-Content-Currency-Icon");
			m_cancelBtn = getWindow<GUIButton>("TipDialog-Pixel-Gun-Btn-Cancel");
			m_sureBtn = getWindow<GUIButton>("TipDialog-Pixel-Gun-Btn-Sure");
			m_contentWindow = getWindow("TipDialog-Pixel-Gun-Content-Vehicle");
			m_otherContentWindow = getWindow("TipDialog-Pixel-Gun-Content-Other");
			m_otherLackOfMoneyWindow = getWindow("TipDialog-Pixel-Gun-Content-LackOfMoney");
			m_lackOfMoneyTip = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-LackOfMoney-Currency-Tip");
			m_lackOfMoneyIcon = getWindow<GUIStaticImage>("TipDialog-Pixel-Gun-Content-LackOfMoney-Currency-Icon");
			m_lackOfMoneyValue = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-LackOfMoney-Currency-Value");
			m_lackOfMoneyMsg = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-LackOfMoney-Message");
			m_watchAdBtn = getWindow<GUIButton>("TipDialog-Pixel-Gun-Btn-Watch-Ad");
			m_adHint = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-Ad-Hint");
			getWindow<GUIButton>("TipDialog-Pixel-Gun-Title-Btn-Close")->SetVisible(false);
			getWindow<GUIButton>("TipDialog-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClose, this, std::placeholders::_1));
			break;
		case ClientGameType::BED_WAR_HALL:
			bedWarPanel->SetVisible(true);
			m_titleText = getWindow<GUIStaticText>("TipDialog-BedWar-Title-Name");
			m_messageText = getWindow<GUIStaticText>("TipDialog-BedWar-Content-Vehicle-Message");
			m_valueText = getWindow<GUIStaticText>("TipDialog-BedWar-Content-Currency-Value");
			m_otherMsgText = getWindow<GUIStaticText>("TipDialog-BedWar-Content-Other-Message");
			m_iconImage = getWindow<GUIStaticImage>("TipDialog-BedWar-Content-Currency-Icon");
			m_cancelBtn = getWindow<GUIButton>("TipDialog-BedWar-Btn-Cancel");
			m_sureBtn = getWindow<GUIButton>("TipDialog-BedWar-Btn-Sure");
			m_contentWindow = getWindow("TipDialog-BedWar-Content-Vehicle");
			m_otherContentWindow = getWindow("TipDialog-BedWar-Content-Other");
			m_otherLackOfMoneyWindow = getWindow("TipDialog-BedWar-Content-LackOfMoney");
			m_lackOfMoneyTip = getWindow<GUIStaticText>("TipDialog-BedWar-Content-LackOfMoney-Currency-Tip");
			m_lackOfMoneyIcon = getWindow<GUIStaticImage>("TipDialog-BedWar-Content-LackOfMoney-Currency-Icon");
			m_lackOfMoneyValue = getWindow<GUIStaticText>("TipDialog-BedWar-Content-LackOfMoney-Currency-Value");
			m_lackOfMoneyMsg = getWindow<GUIStaticText>("TipDialog-BedWar-Content-LackOfMoney-Message");
			m_watchAdBtn = getWindow<GUIButton>("TipDialog-BedWar-Btn-Watch-Ad");
			m_adHint = getWindow<GUIStaticText>("TipDialog-BedWar-Content-Ad-Hint");
			getWindow<GUIButton>("TipDialog-BedWar-Title-Btn-Close")->SetVisible(false);
			// getWindow<GUIButton>("TipDialog-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClose, this, std::placeholders::_1));
			break;
		case ClientGameType::SkyBlockMineCave:
		case ClientGameType::SkyBlockProduct:
		case ClientGameType::SkyBlockMain:
			skyBlockPanel->SetVisible(true);
			m_titleText = getWindow<GUIStaticText>("TipDialog-SkyBlock-Title-Name");
			m_messageText = getWindow<GUIStaticText>("TipDialog-SkyBlock-Content-Vehicle-Message");
			m_valueText = getWindow<GUIStaticText>("TipDialog-SkyBlock-Content-Currency-Value");
			m_otherMsgText = getWindow<GUIStaticText>("TipDialog-SkyBlock-Content-Other-Message");
			m_iconImage = getWindow<GUIStaticImage>("TipDialog-SkyBlock-Content-Currency-Icon");
			m_cancelBtn = getWindow<GUIButton>("TipDialog-SkyBlock-Btn-Cancel");
			m_sureBtn = getWindow<GUIButton>("TipDialog-SkyBlock-Btn-Sure");
			m_contentWindow = getWindow("TipDialog-SkyBlock-Content-Vehicle");
			m_otherContentWindow = getWindow("TipDialog-SkyBlock-Content-Other");
			m_otherLackOfMoneyWindow = getWindow("TipDialog-SkyBlock-Content-LackOfMoney");
			m_lackOfMoneyTip = getWindow<GUIStaticText>("TipDialog-SkyBlock-Content-LackOfMoney-Currency-Tip");
			m_lackOfMoneyIcon = getWindow<GUIStaticImage>("TipDialog-SkyBlock-Content-LackOfMoney-Currency-Icon");
			m_lackOfMoneyValue = getWindow<GUIStaticText>("TipDialog-SkyBlock-Content-LackOfMoney-Currency-Value");
			m_lackOfMoneyMsg = getWindow<GUIStaticText>("TipDialog-SkyBlock-Content-LackOfMoney-Message");
			m_watchAdBtn = getWindow<GUIButton>("TipDialog-SkyBlock-Btn-Watch-Ad");
			m_adHint = getWindow<GUIStaticText>("TipDialog-SkyBlock-Content-Ad-Hint");
			getWindow<GUIButton>("TipDialog-SkyBlock-Title-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClose, this, std::placeholders::_1));
			break;
		case ClientGameType::WalkingDead:
			getWindow("TipDialog-WalkingDead-Panel")->SetVisible(true);
			getWindow("TipDialog-Panel")->SetVisible(false);
			getWindow("TipDialog-BedWar")->SetVisible(false);
			getWindow("TipDialog-Pixel-Gun-Panel")->SetVisible(false);
			getWindow("TipDialog-SkyBlock")->SetVisible(false);
			m_titleText = getWindow<GUIStaticText>("TipDialog-WalkingDead-Title-Name");
			m_messageText = getWindow<GUIStaticText>("TipDialog-WalkingDead-Content-Vehicle-Message");
			m_otherMsgText = getWindow<GUIStaticText>("TipDialog-WalkingDead-Content-Other-Message");
			m_contentWindow = getWindow("TipDialog-WalkingDead-Content-Vehicle");
			m_otherContentWindow = getWindow("TipDialog-WalkingDead-Content-Other");
			m_otherLackOfMoneyWindow = getWindow("TipDialog-WalkingDead-Content-LackOfMoney");
			m_lackOfMoneyTip = getWindow<GUIStaticText>("TipDialog-WalkingDead-Content-LackOfMoney-Currency-Tip");
			m_lackOfMoneyIcon = getWindow<GUIStaticImage>("TipDialog-WalkingDead-Content-LackOfMoney-Currency-Icon");
			m_lackOfMoneyValue = getWindow<GUIStaticText>("TipDialog-WalkingDead-Content-LackOfMoney-Currency-Value");
			m_lackOfMoneyMsg = getWindow<GUIStaticText>("TipDialog-WalkingDead-Content-LackOfMoney-Message");
			m_valueText = getWindow<GUIStaticText>("TipDialog-WalkingDead-Content-Currency-Value");
			m_iconImage = getWindow<GUIStaticImage>("TipDialog-WalkingDead-Content-Currency-Icon");
			m_sureBtn = getWindow<GUIButton>("TipDialog-WalkingDead-Btn-Sure");
			m_cancelBtn = getWindow<GUIButton>("TipDialog-WalkingDead-Btn-Cancel");
			m_btnClose = getWindow<GUIButton>("TipDialog-WalkingDead-Title-Btn-Close");
			m_watchAdBtn = getWindow<GUIButton>("TipDialog-WalkingDead-Btn-Watch-Ad");
            m_adHint = getWindow<GUIStaticText>("TipDialog-WalkingDead-Content-Ad-Hint");
			m_btnClose->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClose, this, std::placeholders::_1));
			break;
		default:
			defaultPanel->SetVisible(true);
			m_titleText = getWindow<GUIStaticText>("TipDialog-Title-Name");
			m_messageText = getWindow<GUIStaticText>("TipDialog-Content-Vehicle-Message");
			m_otherMsgText = getWindow<GUIStaticText>("TipDialog-Content-Other-Message");
			m_contentWindow = getWindow("TipDialog-Content-Vehicle");
			m_otherContentWindow = getWindow("TipDialog-Content-Other");
			m_otherLackOfMoneyWindow = getWindow("TipDialog-Content-LackOfMoney");
			m_lackOfMoneyTip = getWindow<GUIStaticText>("TipDialog-Content-LackOfMoney-Currency-Tip");
			m_lackOfMoneyIcon = getWindow<GUIStaticImage>("TipDialog-Content-LackOfMoney-Currency-Icon");
			m_lackOfMoneyValue = getWindow<GUIStaticText>("TipDialog-Content-LackOfMoney-Currency-Value");
			m_lackOfMoneyMsg = getWindow<GUIStaticText>("TipDialog-Content-LackOfMoney-Message");
			m_valueText = getWindow<GUIStaticText>("TipDialog-Content-Currency-Value");
			m_iconImage = getWindow<GUIStaticImage>("TipDialog-Content-Currency-Icon");
			m_sureBtn = getWindow<GUIButton>("TipDialog-Btn-Sure");
			m_cancelBtn = getWindow<GUIButton>("TipDialog-Btn-Cancel");
			m_watchAdBtn = getWindow<GUIButton>("TipDialog-Btn-Watch-Ad");
			m_adHint = getWindow<GUIStaticText>("TipDialog-Content-Ad-Hint");
			getWindow<GUIButton>("TipDialog-Title-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClose, this, std::placeholders::_1));
			switch (GameClient::CGame::Instance()->GetGameType())
			{
			case ClientGameType::BED_WAR:
				m_sureBtn->SetWidth({ 0.38f, 0.0f });
				m_sureBtn->SetXPosition({ -0.10f, 0.0f });
				m_cancelBtn->SetWidth({ 0.38f, 0.0f });
				m_cancelBtn->SetXPosition({ 0.10f, 0.0f });
				break;
			}
			break;
		}

		m_watchAdBtn->SetVisible(false);
		m_adHint->SetVisible(false);
		m_cancelBtn->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClick, this, std::placeholders::_1, VIewId::BTN_CANCEL));
		m_sureBtn->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClick, this, std::placeholders::_1, VIewId::BTN_SURE));
		m_watchAdBtn->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClick, this, std::placeholders::_1, VIewId::BTN_WATCH_AD));

		m_subscriptionGuard.add(ShowVehicleTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showVehicleTipDialog, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(showUnlockCommodityTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showUnlockCommodityTipDialog, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)));
		m_subscriptionGuard.add(ShowGoAppShopTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showLackOfMoneyGoAppShop, this)));
		m_subscriptionGuard.add(ShowReviveEvent::subscribe(std::bind(&gui_tipDialog::showRevive, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
		m_subscriptionGuard.add(ShowGoNpcMerchantTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showGoNpcMerchantTipDialog, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
		m_subscriptionGuard.add(SyncChangePlayerActortEvent::subscribe(std::bind(&gui_tipDialog::showChangePlayerActortTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowPickUpItemTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showPickUpItemTipDialog, this, 
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowResultTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showResultTipDialog, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowGetSellManorTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showGetOrSellManor, this)));
		m_subscriptionGuard.add(ShowGetManorTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showGetManor, this)));
		m_subscriptionGuard.add(ShowSellManorTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showSellManor, this)));
		m_subscriptionGuard.add(ShowLackOfMoneyTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showLackOfMoney, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowUpgradeResourceEvent::subscribe(std::bind(&gui_tipDialog::showUpgradeResourceTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowCustomTipMsgEvent::subscribe(std::bind(&gui_tipDialog::showCustomTipMsg, this, std::placeholders::_1,std::placeholders::_2)));
		m_subscriptionGuard.add(ShowManorBuildTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showManorBuildHouse, this)));
		m_subscriptionGuard.add(ShowBuildWarGuessTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showBuildWarGuessTip, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowConsumeCoinTipEvent::subscribe(std::bind(&gui_tipDialog::showConsumeCoinTipMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowRanchExBeginTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showRanchExBeginTip, this)));
		m_subscriptionGuard.add(ShowRanchExTaskFinishTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showRanchExTaskFinish, this)));
		m_subscriptionGuard.add(ShowKeepItemTipEvent::subscribe(std::bind(&gui_tipDialog::showKeepItemTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowEnchantMentQuickEvent::subscribe(std::bind(&gui_tipDialog::showEnchantmentQuickTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowPixelGunHallNotOpenEvent::subscribe(std::bind(&gui_tipDialog::showPixelGunHallNotOpen, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowPixelGunHallLvNotEnoughEvent::subscribe(std::bind(&gui_tipDialog::showPixelGunHallLvNotEnough, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowPixelGunHallLvMapLockDiamondEvent::subscribe(std::bind(&gui_tipDialog::showPixelGunHallMapLockDiamond, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowMainLineTaskLvNotEnoughEvent::subscribe(std::bind(&gui_tipDialog::showMainLineTaskLvNotEnough, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowSkyBlockMainLineTaskLvNotEnoughEvent::subscribe(std::bind(&gui_tipDialog::showSkyBlockMainLineTaskLvNotEnough, this, std::placeholders::_1)));
		m_subscriptionGuard.add(DisableRechargeEvent::subscribe(std::bind(&gui_tipDialog::disableRecharge, this)));
		m_subscriptionGuard.add(ShowBedWarReclaimTipEvent::subscribe(std::bind(&gui_tipDialog::showBedWarReclaimTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowBedWarReclaimAllTipEvent::subscribe(std::bind(&gui_tipDialog::showBedWarReclaimAllTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowBedWarRuneUnlockTipEvent::subscribe(std::bind(&gui_tipDialog::showBedWarRuneUnlockTip, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_tipDialog::onVideoAdVisible, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowSkyBlockCostMuchMoneyEvent::subscribe(std::bind(&gui_tipDialog::onShowSkyBlockCostMuchMoneyMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)));
	}

	void gui_tipDialog::onShow()
	{
		//
	}

	void gui_tipDialog::onGetCommonData(String key, String data)
	{
		if (key == "SkyRoyaleRespawnAd")
		{
			auto builder = CommonDataBuilder().fromData(data);
			bool isShow = builder.getBoolParam("isShow");
			m_showAd = isShow;
		}
		else if(key == "HungryGameRespawnAd")
		{
			auto builder = CommonDataBuilder().fromData(data);
			bool isShow = builder.getBoolParam("isShow");
			m_showAd = isShow;
		}
	}

	void gui_tipDialog::refreshUpgradeResourceUI()
	{
		m_titleText->SetText(getString(LanguageKey::GUI_SHOP_TAB_UPGRADE).c_str());
		m_messageText->SetText(m_tipMessage.c_str());

		if (m_vehicleId < 0)
		{
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_cancelBtn->SetVisible(false);
			m_sureBtn->SetVisible(false);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			return;
		}

		m_contentWindow->SetVisible(true);
		m_otherContentWindow->SetVisible(false);
		m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
		m_valueText->SetText(StringUtil::ToString(m_vehiclePrice).c_str());
		GuiItemStack::showItemIcon(m_iconImage, m_vehicleId, 0);
		m_contentWindow->SetVisible(true);
		m_otherContentWindow->SetVisible(false);
		m_sureBtn->SetEnabled(false);
		m_sureBtn->SetText(getString(LanguageKey::LACK_OF_MONEY));
		if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
		{
			Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
			int coinId = CoinManager::Instance()->itemIdToCoinId(m_vehicleId);
			if (wallet && wallet->getCoinNum(coinId) >= m_vehiclePrice)
			{
				m_sureBtn->SetEnabled(true);
				m_sureBtn->SetText(getString(LanguageKey::GUI_SHOP_TAB_UPGRADE));
			}
		}
	}

	void gui_tipDialog::upgradeResource()
	{
		ClientNetwork::Instance()->getSender()->sendUpgradeResource(resourceId);
	}

	bool gui_tipDialog::disableRecharge()
	{
		m_enable_recharge = false;
		return true;
	}

	bool gui_tipDialog::onVideoAdVisible(bool visible)
	{
		m_haveVideoAd = visible;
		return true;
	}

	void gui_tipDialog::onWatchVehicleAd()
	{
		String params = CommonDataBuilder().addIntParam("EntityId", m_vehicleEntityId).getData();
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::JAIL_BREAK_UNLOCK_VEHICLE, params);
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("car_free_unlock", GameClient::CGame::Instance()->getGameType());
		getParent()->hideTipDialog();
	}

	void gui_tipDialog::onWatchRespawnAd()
	{
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("click_reward_ad", GameClient::CGame::Instance()->getGameType());

		if (checkGameType(ClientGameType::Sky_Royale))
		{
			GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::SKY_ROYALE_RESPAWN);
			m_showAd = false;

			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("skyroyale_die_revive_ad_click", GameClient::CGame::Instance()->getGameType());
			ClientNetwork::Instance()->getLuaCommonSender()->sendLuaCommonData("ClickWatchRespawnAd");
		}
		else if (checkGameType(ClientGameType::HungryGame))
		{
			GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::HUNGRY_GAME_RESPAWN);
			m_showAd = false;

			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("survive_die_revive_ad_click", GameClient::CGame::Instance()->getGameType());

		}
		getParent()->hideTipDialog();
	}

	void gui_tipDialog::refreshUI()
	{
		String msg{};

		m_cancelBtn->SetVisible(true);
		m_sureBtn->SetVisible(true);
		m_watchAdBtn->SetVisible(false);
		m_nolongerRemind->SetVisible(false);

		switch (m_showTipType)
		{
		case TipType::VEHICLE_TIP:
		{
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_VEHICLE));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_UNLOCK));
			m_valueText->SetText(StringUtil::ToString(m_vehiclePrice).c_str());
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_sureBtn->SetEnabled(true);
			m_iconImage->SetImage(UICommon::getCurrencyIcon().c_str());
			m_iconImage->SetVisible(true);
			m_valueText->SetVisible(true);
			bool showAd = m_haveVideoAd && checkGameType(ClientGameType::Jail_Break);
			m_watchAdBtn->SetVisible(showAd);
			m_adHint->SetVisible(showAd);
			if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
			{
				Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
				if (wallet && wallet->getCurrency() >= m_vehiclePrice)
				{
					m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_UNLOCK));
				}
				else
				{
					m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_GET_CURRENCY));
				}
			}
			break;
		}
		case TipType::UNLOCK_COMMODITY:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_cancelBtn->SetEnabled(true);
			m_sureBtn->SetEnabled(true);
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_valueText->SetText(StringUtil::ToString(m_unlockCost).c_str());
			m_iconImage->SetImage(UICommon::getCurrencyIcon().c_str());
			m_iconImage->SetVisible(true);
			m_valueText->SetVisible(true);
			break;

		case TipType::GO_APP_SHOP:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;

		case TipType::REVIVE:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_REVIVE_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_REVIVE_SURE));
			msg = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_REVIVE_MSG).c_str(), (int)m_countDown);
			m_messageText->SetText(msg.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_is_showAd = m_showAd && m_haveVideoAd;
			if (checkGameType({ ClientGameType::Sky_Royale, ClientGameType::HungryGame }))
			{
				m_watchAdBtn->SetVisible(m_is_showAd);
				m_adHint->SetVisible(m_is_showAd);

			}
			break;
		case TipType::GO_NPC_MERCHANT:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_BTN_UPGRADE_BAG));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::CHANGE_ACTORT_TIP:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_CHANGE_ACTORT));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CHANGE_ACTORT));
			m_valueText->SetText(StringUtil::ToString(needMoneyCount).c_str());
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_sureBtn->SetEnabled(true);
			m_iconImage->SetImage(UICommon::getCurrencyIconByType(CurrencyType::PLATFORM_MONEY).c_str());
			m_valueText->SetVisible(true);
			if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
			{
				Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
				if (wallet && wallet->getDiamondsGolds() >= needMoneyCount)
				{
					m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CHANGE_ACTORT));
				}
				else
				{
					m_sureBtn->SetEnabled(false);
					m_sureBtn->SetText(getString(LanguageKey::LACK_OF_MONEY));
				}
			}
			break;

		case TipType::PICK_UP_ITEM:
			m_titleText->SetText(StringUtil::Format(getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_TITLE).c_str(), m_itemName.c_str()).c_str());
			m_messageText->SetText(m_tipMessage.c_str());
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_BTN_SURE));
			m_valueText->SetText(StringUtil::ToString(m_pickUpItemPrice).c_str());
			m_iconImage->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
			m_iconImage->SetVisible(true);
			m_valueText->SetVisible(true);
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::GET_MANOR:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE_GET_MANOR));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case  TipType::LACK_OF_PLATFORM_MONEY:
			m_titleText->SetText(getString(LanguageKey::LACK_OF_MONEY));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_BACK_PLATFORM_TOP_UP));

			m_lackOfMoneyTip->SetText(getString(LanguageKey::GUI_TIP_NEED_CURRENCY));
			m_lackOfMoneyMsg->SetText(getString(LanguageKey::GUI_NEED_BACK_PLATFORM_TOP_UP));
			m_lackOfMoneyValue->SetText(StringUtil::ToString(needMoneyCount).c_str());
			m_lackOfMoneyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());;
			
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(true);
			m_sureBtn->SetEnabled(true);

			break;

		case TipType::UPGRADE_RESOURCE:
			refreshUpgradeResourceUI();
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::MANOR_BUILD_HOUSE:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_MANOR_HOUSE_BTN_BUILD));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::CUSTOM_TIP:
			refreshCustomTipUI();
			break;
		case TipType::BUILD_WAR_GUESS_TIP:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::CONSUME_COIN_TIP:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_valueText->SetText(StringUtil::ToString(needMoneyCount).c_str());
			m_contentWindow->SetVisible(true);
			m_iconImage->SetVisible(true);
			m_valueText->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(false);
			break;
		case TipType::KEEP_ITEM:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_SURE));
			msg = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_MSG).c_str(), (int)m_countDown);
			m_messageText->SetText(msg.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::ENCHANTMENT_QUICK:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::PIXEL_GUNHALL_LOCK_MAP_DIAMOND:
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MAP_DIAMOND_LOCK_TIP_TITLE));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_iconImage->SetVisible(true);
			m_valueText->SetVisible(true);
			break;
		case TipType::PIXEL_GUNHALL_LOCK_MODE:
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_iconImage->SetVisible(false);
			m_valueText->SetVisible(false);
			m_cancelBtn->SetVisible(false);
			m_sureBtn->SetVisible(false);
			break;
		case TipType::PIXEL_GUNHALL_MODE_NOT_OPEN:
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_iconImage->SetVisible(false);
			m_valueText->SetVisible(false);
			m_cancelBtn->SetVisible(false);
			m_sureBtn->SetVisible(false);
			break;
		case TipType::QUICKLY_BUILD_BLOCK:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_QUICKLY_BUILD_BLOCK));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_cancelBtn->SetVisible(true);
			m_cancelBtn->SetEnabled(true);
			m_sureBtn->SetVisible(true);
			m_sureBtn->SetEnabled(true);
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_iconImage->SetImage("set:app_shop.json image:app_shop_diamonds_mods");
			m_valueText->SetText(StringUtil::ToString(LogicSetting::Instance()->getQuicklyBuildBlockCost()).c_str());
			m_iconImage->SetVisible(true);
			m_valueText->SetVisible(true);
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::MAIN_LINE_TASK_LV_LOCK:
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_iconImage->SetVisible(false);
			m_valueText->SetVisible(false);
			m_cancelBtn->SetVisible(false);
			m_sureBtn->SetVisible(false);
			break;
		case TipType::SKYBLOCK_MAIN_LINE_TASK_LV_LOCK:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_iconImage->SetVisible(false);
			m_valueText->SetVisible(false);
			m_cancelBtn->SetVisible(false);
			m_sureBtn->SetVisible(false);
			break;
		case TipType::UNLOCK_SELECTABLE_ACTION:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_titleText->SetVisible(true);
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_cancelBtn->SetEnabled(true);
			m_sureBtn->SetEnabled(true);
			m_cancelBtn->SetVisible(true);
			m_sureBtn->SetVisible(true);
			m_messageText->SetText(m_tipMessage.c_str());
			m_otherContentWindow->SetVisible(false);
			m_contentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::BEDWAR_RECLAIM_TIP:
			m_titleText->SetText(getString(LanguageKey::GUI_BEDWAR_RECLAIM_TITLE_TIP));
			m_contentWindow->SetVisible(true);
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_valueText->SetVisible(false);
			m_iconImage->SetVisible(false);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::BEDWAR_RECLAIM_ALL_TIP:
			m_titleText->SetText(getString(LanguageKey::GUI_BEDWAR_RECLAIM_ALL_TITLE_TIP));
			m_contentWindow->SetVisible(true);
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_valueText->SetVisible(false);
			m_iconImage->SetVisible(false);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::BED_WALL_HALL_UNLOCK_RUNE:
			m_titleText->SetText(getString(LanguageKey::GUI_BEDWAR_UNLOCK_RUNE_TITLE_TIP));
			m_contentWindow->SetVisible(true);
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_valueText->SetVisible(true);
			m_iconImage->SetVisible(true);
			m_messageText->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::SKYBLOCK_COST_MUCH_MONEY:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_messageText->SetVisible(true);
			m_valueText->SetVisible(true);
			m_iconImage->SetVisible(true);
			m_valueText->SetText(StringUtil::ToString(needMoneyCount).c_str());
			m_contentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(false);
			m_cancelBtn->SetVisible(true);
			m_sureBtn->SetVisible(true);
			break;
		case TipType::DIAMOND_BLUE_LACK_TIPS:
		{
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_sureBtn->SetEnabled(true);
			m_cancelBtn->SetVisible(true);
			m_sureBtn->SetVisible(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			if (m_nolongerRemind && m_nolongerRemindCheck && m_nolongerRemindText)
			{
				m_nolongerRemind->SetVisible(true);
				m_nolongerRemindText->SetText(getString("gui_nolonger_remind"));
			}
			break;
		}
		default:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_sureBtn->SetEnabled(true);
			m_cancelBtn->SetVisible(true);
			m_sureBtn->SetVisible(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		}
	}

	void gui_tipDialog::onUpdate(ui32 nTimeElapse)
	{
		if (m_showTipType == TipType::REVIVE)
		{
			if (m_countDown > 0)
			{
				m_countDown -= (float)nTimeElapse / 1000;
				String msg = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_REVIVE_MSG).c_str(), (int)m_countDown);
				m_messageText->SetText(msg.c_str());
				if (m_countDown < 0)
				{
					closeRevive();
				}
			}
		}
		else if (m_showTipType == TipType::KEEP_ITEM)
		{
			if (m_countDown > 0)
			{
				m_countDown -= (float)nTimeElapse / 1000;
				String msg = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_MSG).c_str(), (int)m_countDown);
				m_messageText->SetText(msg.c_str());
				if (m_countDown < 0)
				{
					closeKeepItemTip();
				}
			}

		}
	}

	bool gui_tipDialog::onBtnClick(const EventArgs, VIewId viewId)
	{
		switch (viewId)
		{
		case gui_tipDialog::VIewId::BTN_SURE:
			switch (m_showTipType) {
			case TipType::VEHICLE_TIP:
				if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
				{
					Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
					if (wallet && wallet->getCurrency() >= m_vehiclePrice)
					{
						GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyVehicle(m_vehicleId);
						getParent()->hideTipDialog();
					}
					else
					{
						getParent()->showAppShop();
					}
				}
				break;
			case TipType::UNLOCK_COMMODITY:
				if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
				{
					Wallet* wallet = Blockman::Instance()->m_pPlayer->m_wallet;
					if (wallet && wallet->getCurrency() >= m_unlockCost)
					{
						GameClient::CGame::Instance()->getNetwork()->getSender()->sendUnlockCommodity(m_merchantId, m_groupIndex, m_commodityIndex, m_commodityId);
						getParent()->hideTipDialog();
					}
					else
					{
						getParent()->showAppShop();
					}
				}
				break;
			case TipType::GO_NPC_MERCHANT:
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendGoNpcMerchant(m_npc_x, m_npc_y, m_npc_z, m_player_yaw);
				break;
			case TipType::GO_APP_SHOP:
				getParent()->showAppShop();
				break;
			case TipType::REVIVE:
				UICommon::checkMoney(m_revive_price, m_revive_type, [=](bool isSuccess) {
					if (isSuccess)
					{
						m_countDown = 0.0f;
						getParent()->hideTipDialog();
						GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyRespawn(true, m_uniqueId, m_index);
					}
				});

				break;
			case TipType::CHANGE_ACTORT_TIP:
				if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
				{
					UICommon::checkMoney(needMoneyCount, 1, [=](bool isSuccess) {
						if (isSuccess)
						{
							GameClient::CGame::Instance()->getNetwork()->getSender()->sendChangePlayerActor();
							getParent()->hideTipDialog();
						}
					});
				}
				break;
			case TipType::PICK_UP_ITEM:
				pickUpItem();
				break;
			case TipType::GET_MANOR:
				//TODO
				ClientNetwork::Instance()->getSender()->sendGetManor();
				getParent()->hideTipDialog();
				break;
			case TipType::SELL_MANOR:
				if (Blockman::Instance()->m_pPlayer->m_manor->getInfo() && Blockman::Instance()->m_pPlayer->m_manor->getInfo()->manorId.length() > 0)
				{
					showSureSellManor(Blockman::Instance()->m_pPlayer->m_manor->getInfo()->value);
				}
				break;
			case TipType::SURE_SELL_MANOR:
				//TODO
				ClientNetwork::Instance()->getSender()->sendSellManor();
				getParent()->hideTipDialog();
				break;
			case TipType::LACK_OF_PLATFORM_MONEY:
				GameClient::CGame::Instance()->getShellInterface()->onRecharge((int)RechargeType::RECHARGE_TYPE_CUBE);
				break;
			case TipType::UPGRADE_RESOURCE:
				upgradeResource();
				getParent()->hideTipDialog();
				break;
			case TipType::CUSTOM_TIP:
				SyncCustomTipResult(true);
				getParent()->hideTipDialog();
				break;
			case TipType::MANOR_BUILD_HOUSE:
				SureBuildManorHouseEvent::emit();
				getParent()->hideTipDialog();
				break;
			case TipType::BUILD_WAR_GUESS_TIP:
				SoundSystem::Instance()->playEffectByType(ST_Click);
				ClientNetwork::Instance()->getSender()->sendBuildWarGuessSuc(m_buildWarGuessId);
				getParent()->hideTipDialog();
				break;
			case TipType::CONSUME_COIN_TIP:
				sendConsumeCoinTipResult(true);
				break;
			case TipType::RANCHERS_EXPLORE_BEGIN:
				ClientNetwork::Instance()->getSender()->sendRanchExBegin();
				getParent()->hideTipDialog();
				break;
			case TipType::RANCHERS_TASK_FINISH:
				if (Blockman::Instance() 
					&& Blockman::Instance()->m_pPlayer 
					&& Blockman::Instance()->m_pPlayer->inventory
					&& Blockman::Instance()->m_pPlayer->inventory->getCurrentItem())
				{
					ClientNetwork::Instance()->getSender()->sendRanchExTaskFinish(Blockman::Instance()->m_pPlayer->inventory->getCurrentItem()->itemID, 
						Blockman::Instance()->m_pPlayer->inventory->getCurrentItem()->getItemStackInfo().num);
				}
				
				getParent()->hideTipDialog();
				break;
			case TipType::KEEP_ITEM:
				m_countDown = 0.0f;
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendKeepItem(true, KeepItemType::KEEP_ITEM);
				break;
			case TipType::ENCHANTMENT_QUICK:
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendEnchantmentQuick(m_enchantment_equip_id, m_enchantment_effect_index);
				break;
			case TipType::PIXEL_GUNHALL_LOCK_MAP_DIAMOND:
				GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendPixelGunUnlockMap(m_pixelhall_map_id);
				getParent()->hideTipDialog();
				break;
			case TipType::PIXEL_GUNHALL_LOCK_MODE:
				getParent()->hideTipDialog();
				break;
			case TipType::PIXEL_GUNHALL_MODE_NOT_OPEN:
				getParent()->hideTipDialog();
				break;
			case TipType::QUICKLY_BUILD_BLOCK:
				getParent()->hideTipDialog();
				UICommon::checkMoney(LogicSetting::Instance()->getQuicklyBuildBlockCost(), CurrencyType::PLATFORM_MONEY, [=](bool isSuccess) {
					if (isSuccess)
					{
						ClientNetwork::Instance()->getSender()->sendBuyQuicklyBuildBlock();
					}
				});
				break;
			case TipType::MAIN_LINE_TASK_LV_LOCK:
				getParent()->hideTipDialog();
                break;
			case TipType::UNLOCK_SELECTABLE_ACTION:
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getShellInterface()->buyAction(m_toUnlockActionId);
				break;
			case TipType::BEDWAR_RECLAIM_TIP:
				getParent()->hideTipDialog();
				ClientNetwork::Instance()->getCommonSender()->sendBedWarReclaimRune(m_bedWar_reclaim_rune_id);
				break;
			case TipType::BEDWAR_RECLAIM_ALL_TIP:
				getParent()->hideTipDialog();
				ClientNetwork::Instance()->getCommonSender()->sendBedWarOneKeyReclaimRune(m_bedWar_reclaim_all_type);
				break;
			case TipType::BED_WALL_HALL_UNLOCK_RUNE:
				getParent()->hideTipDialog();
				ClientNetwork::Instance()->getCommonSender()->sendBedWarUnlockRune(m_bedWar_rune_unlock_type);
				break;
			case TipType::PIXEL_GUN_STORE_LACK_TICKET:
				getParent()->hideTipDialog();
				getParent()->showPixelGunGiftBagStore(true);
				break;
			case TipType::SKYBLOCK_COST_MUCH_MONEY:
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSkyBlockBuyGoods(m_cost_much_num, m_cost_much_id, 0);
				break;
			case TipType::DIAMOND_BLUE_LACK_TIPS:
			{
				getParent()->hideTipDialog();
				if (m_sureFunc)
				{
					m_sureFunc(EventArgs(), VIewId::BTN_SURE);
				}
				break;
			}
			default:
				getParent()->hideTipDialog();
				break;
			}
			break;
		case gui_tipDialog::VIewId::BTN_CANCEL:
			switch (m_showTipType) {
			case TipType::GO_NPC_MERCHANT:
			{
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendGoNpcMerchant(m_npc_x, m_npc_y, m_npc_z, m_player_yaw);
				auto dataCache = ClientNetwork::Instance()->getDataCache();
				auto entityId = dataCache->getClientId(m_merchantId);
				auto merchant = dynamic_cast<EntityMerchant*>(Blockman::Instance()->getWorld()->getEntity(entityId));
				if (merchant)
				{
					if (!merchant->getHasUpdated())
					{
						merchant->setHasUpdated(true);
						ClientNetwork::Instance()->getSender()->sendNeedUpdateMerchantCommodities(m_merchantId);
					}
					MerchantActivateEvent::emit(merchant);
				}
			}
			break;
			case TipType::CUSTOM_TIP:
				SyncCustomTipResult(false);
				getParent()->hideTipDialog();
				break;
			case TipType::CONSUME_COIN_TIP:
				sendConsumeCoinTipResult(false);
				break;
			case TipType::KEEP_ITEM:
				closeKeepItemTip();
				break;
			case TipType::DIAMOND_BLUE_LACK_TIPS:
				getParent()->hideTipDialog();
				break;
			default:
				getParent()->hideTipDialog();
				if (m_showTipType == TipType::REVIVE)
					closeRevive();
				break;
			}
			break;
		case VIewId::BTN_WATCH_AD:
			switch (m_showTipType)
			{
			case TipType::VEHICLE_TIP:
				onWatchVehicleAd();
				break;
			case TipType::REVIVE:
				onWatchRespawnAd();
				break;
			}
			break;
		}
		return true;
	}

	bool gui_tipDialog::onBtnClose(const EventArgs)
	{
		getParent()->hideTipDialog();
		if (m_showTipType == TipType::REVIVE)
			closeRevive();
		else if (m_showTipType == TipType::KEEP_ITEM)
			closeKeepItemTip();
        else if (m_showTipType == TipType::DIAMOND_BLUE_LACK_TIPS)
        {
            getParent()->hideTipDialog();
        }
		return true;
	}

	bool gui_tipDialog::showVehicleTipDialog(int vehicleId, int entityId)
	{
		this->m_showTipType = TipType::VEHICLE_TIP;
		CarSetting* carSetting = CarSetting::getCarSetting(vehicleId);
		if (!CarSetting::m_sbIsCarFree && carSetting)
		{
			getParent()->showTipDialog();
			m_vehicleId = carSetting->carId;
			m_vehicleEntityId = ClientNetwork::Instance()->getDataCache()->getServerId(entityId);
			m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_UNLOCK_VEHICLE).c_str(), getString(carSetting->name).c_str());
			m_vehiclePrice = carSetting->price;
			m_watchAdBtn->SetText(getString("unlock.vehicle.free.use"));
			m_adHint->SetText(getString("unlock.vehicle.free.use.hint"));
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("car_unlock", GameClient::CGame::Instance()->getGameType());	
			if (m_haveVideoAd)
			{
				GameClient::CGame::Instance()->getShellInterface()->
					dataReport("car_unlock_show_ad", GameClient::CGame::Instance()->getGameType());
			}
			refreshUI();
		}
		return true;
	}

	bool gui_tipDialog::showUnlockCommodityTipDialog(int merchantId, int groupIndex, int commodityIndex, int commodityId, int unlockCost, const String & commodityName)
	{
		this->m_showTipType = TipType::UNLOCK_COMMODITY;
		m_merchantId = merchantId;
		m_groupIndex = groupIndex;
		m_commodityIndex = commodityIndex;
		m_commodityId = commodityId;
		m_unlockCost = unlockCost;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_UNLOCK_COMMODITY).c_str(), getString(commodityName).c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showLackOfMoneyGoAppShop()
	{
		this->m_showTipType = TipType::GO_APP_SHOP;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_GO_APP_SHOP).c_str();
		getParent()->showTipDialog();
		refreshUI();
		return false;
	}

	bool gui_tipDialog::showGoNpcMerchantTipDialog(int merchantId, float x, float y, float z, float yaw)
	{
		this->m_merchantId = merchantId;
		this->m_npc_x = x;
		this->m_npc_y = y;
		this->m_npc_z = z;
		this->m_player_yaw = yaw;
		this->m_showTipType = TipType::GO_NPC_MERCHANT;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_GO_NPC_MERCHANT).c_str();
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showChangePlayerActortTip(int changeCount, int needMoneyCount, const String& actortName)
	{
		this->m_showTipType = TipType::CHANGE_ACTORT_TIP;
		getParent()->showTipDialog();
		ActortName = actortName;
		this->needMoneyCount = needMoneyCount;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_CHANGE_ACTORT).c_str(), StringUtil::ToString(changeCount).c_str());
		m_vehiclePrice = needMoneyCount;
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showUpgradeResourceTip(int costItemId, int costItemNum, int resId,const String & tipStr)
	{
		this->m_showTipType = TipType::UPGRADE_RESOURCE;
		getParent()->showTipDialog();
		m_vehicleId = costItemId;
		m_tipMessage = tipStr;
		m_vehiclePrice = costItemNum;
		resourceId = resId;
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showPickUpItemTipDialog(i32 itemEntityId, i32 itemId, i32 price, CurrencyType moneyType)
	{

		if (isShown() && itemEntityId == m_pickUpItemId)
		{
			return false;
		}

		ItemStackPtr itemStack = LORD::make_shared<ItemStack>(itemId, 1, 0);
		this->m_showTipType = TipType::PICK_UP_ITEM;
		getParent()->showTipDialog();
		this->m_pickUpItemId = itemId;
		this->m_pickUpItemPrice = price;
		this->m_itemName = LanguageManager::Instance()->getItemName(itemStack->getItemName()).c_str();
		this->m_pickUpItemEntityId = itemEntityId;
		this->m_currencyType = moneyType;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_CONTENT).c_str(), m_itemName.c_str());
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showResultTipDialog(ResultCode code, String resultMsg)
	{
		this->m_showTipType = TipType::OTHER_TIP;
		switch (code)
		{
		case ResultCode::ITEM_NOT_EXIST:
			m_tipMessage = getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_NOT_EXIST).c_str();
			
			break;
		case ResultCode::LACK_MONEY:
			m_tipMessage = getString(LanguageKey::LACK_OF_MONEY).c_str();
			break;
		case ResultCode::INVENTORY_FULL:
			m_tipMessage = getString(LanguageKey::GUI_STR_APP_SHOP_INVENTORY_IS_FULL).c_str();
			break;
		case ResultCode::PAY_FAILURE:
			m_tipMessage = getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_PAY_FAILURE).c_str();
			break;
		case ResultCode::PIXEL_GUN_STORE_LACK_TICKET:
			this->m_showTipType = TipType::PIXEL_GUN_STORE_LACK_TICKET;
			m_tipMessage = getString(resultMsg).c_str();
			break;
		default:
			m_tipMessage = getString(resultMsg).c_str();
			break;
		}
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher)
		{
			ShowRanchCommonTipEvent::emit(m_tipMessage);
			return false;
		}
		
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			ShowBirdCommonTipEvent::emit(m_tipMessage);
			return false;
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity)
		{
			ShowBlockCityTipEvent::emit(m_tipMessage);
			return false;
		}

		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showGetOrSellManor()
	{
		auto manor = Blockman::Instance()->m_pPlayer->m_manor->getInfo();

		if (manor && manor->manorId.length() > 0)
		{
			showSellManor();
		}
		else
		{
			showGetManor();
		}

		return true;
	}

	bool gui_tipDialog::showGetManor()
	{
		auto manor = Blockman::Instance()->m_pPlayer->m_manor->getInfo();

		if (manor && manor->manorId.length() > 0)
		{
			showResultTipDialog(ResultCode::COMMON, LanguageKey::GUI_MANOR_GET_ALREADY_EXISTS);
		}
		else
		{
			this->m_showTipType = TipType::GET_MANOR;
			m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_GET_MANOR).c_str();
			m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), Blockman::Instance()->m_pPlayer->getEntityName().c_str());
			getParent()->showTipDialog();
			refreshUI();
		}
		return true;
	}

	bool gui_tipDialog::showSellManor()
	{
		auto manor = Blockman::Instance()->m_pPlayer->m_manor->getInfo();

		if (manor && manor->manorId.length() > 0)
		{
			if (manor->isMaster)
			{
				this->m_showTipType = TipType::SELL_MANOR;
				m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_SELL_MANOR).c_str();
				m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), Blockman::Instance()->m_pPlayer->getEntityName().c_str(), (i32)(manor->value * 0.8f));
				getParent()->showTipDialog();
				refreshUI();
			}
			else
			{
				showResultTipDialog(ResultCode::COMMON, LanguageKey::GUI_MANOR_IS_NOT_LOAD);
			}
		}
		else
		{
			showResultTipDialog(ResultCode::COMMON, LanguageKey::GUI_MANOR_SELL_NOT_EXISTS);
		}
		return true;
	}

	bool gui_tipDialog::showSureSellManor(i32 price)
	{
		this->m_showTipType = TipType::SURE_SELL_MANOR;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_SURE_SELL_MANOR).c_str();
		m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), (i32)(price * 0.8f));
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showLackOfMoney(i32 difference, CurrencyType type)
	{
	    if (type == CurrencyType::PLATFORM_MONEY)
    	{
    		if (!m_enable_recharge)
    		{
    			showResultTipDialog(ResultCode::LACK_MONEY, "");
    			return false;
    		}
    	}
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher)
		{
			ShowRanchLockOfMoneyEvent::emit(difference, (int)type);
			return false;
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			ShowBirdLockOfMoneyEvent::emit(difference, (int)type);
			return false;
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockFort)
		{
			ShowBlockFortLackOfMoneyTipEvent::emit(difference, (i32)type);
			return false;
		}

		if (type == CurrencyType::PLATFORM_MONEY)
		{
			if (checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct))
			{
				GameClient::CGame::Instance()->getShellInterface()->onRecharge((int)RechargeType::RECHARGE_TYPE_CUBE);
				return false;
			}

            this->m_showTipType = TipType::LACK_OF_PLATFORM_MONEY;
			this->m_currencyType = type;
			this->needMoneyCount = difference;
			getParent()->showTipDialog();
			refreshUI();
		}
		else
		{
			if (checkGameType(ClientGameType::BlockCity))
			{
				return false;
			}

			if (checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct))
			{
				SCRIPT_EVENT::SkyBlockShowLackMoney::invoke(true);
				return false;
			}
			showLackOfMoneyGoAppShop();
		}
		return true;
	}

	bool gui_tipDialog::showManorBuildHouse()
	{
		this->m_showTipType = TipType::MANOR_BUILD_HOUSE;
		this->m_tipMessage = getString(LanguageKey::GUI_MANOR_BUILD_HOUSE_TIP).c_str();
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showBuildWarGuessTip(int guessRoomId, String name)
	{
		m_buildWarGuessId = guessRoomId;
		m_showTipType = TipType::BUILD_WAR_GUESS_TIP;
		m_tipMessage = getString(LanguageKey::GUI_BUILD_WAR_GUESS_TIP).c_str();
		m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), name.c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showConsumeCoinTipMsg(const String & message, int coinId, int price, const String & extra)
	{
		m_showTipType = TipType::CONSUME_COIN_TIP;
		m_tipExtra = extra;
		m_tipMessage = getString(message).c_str();
		m_coinId = coinId;
		needMoneyCount = price;
		if (StringUtil::Find(m_tipMessage, "%d"))
			m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), price);
		m_iconImage->SetImage(UICommon::getCurrencyIconByType(coinId).c_str());
		m_iconImage->SetVisible(true);
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	void gui_tipDialog::sendConsumeCoinTipResult(bool isSure)
	{
		getParent()->hideTipDialog();
		if (!isSure)
		{
			ClientNetwork::Instance()->getSender()->sendConsumeTipResult(this->m_tipExtra, isSure);
			return;
		}
		UICommon::checkMoney(needMoneyCount, m_coinId, [=](bool isSuccess) {
			if (isSuccess)
			{
				ClientNetwork::Instance()->getSender()->sendConsumeTipResult(this->m_tipExtra, isSure);
			}
		});
	}

	bool gui_tipDialog::showRanchExBeginTip()
	{
		this->m_showTipType = TipType::RANCHERS_EXPLORE_BEGIN;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_RANCHER_EXPLORE_BEGIN).c_str();
		getParent()->showTipDialog();
		refreshUI();

		return true;
	}

	bool gui_tipDialog::showRanchExTaskFinish()
	{
		this->m_showTipType = TipType::RANCHERS_TASK_FINISH;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_RANCHER_EXPLORE_TASK_FINISH).c_str();
		getParent()->showTipDialog();
		refreshUI();

		return true;
	}

	void gui_tipDialog::pickUpItem()
	{
		auto dataCache = ClientNetwork::Instance()->getDataCache();
		auto entityId = dataCache->getClientId(m_pickUpItemEntityId);
		auto entityItem = dynamic_cast<EntityItem*>(Blockman::Instance()->getWorld()->getEntity(entityId));
		if (!entityItem)
		{
			LordLogInfo("Pick up item do not exist");
			showResultTipDialog(ResultCode::ITEM_NOT_EXIST, "");
			return;
		}

		NULL_RETURN_VOID(Blockman::Instance());
		NULL_RETURN_VOID(Blockman::Instance()->m_pPlayer);

		if (!Blockman::Instance()->m_pPlayer->inventory->isCanAddItem(m_pickUpItemId, entityItem->getEntityItem()->stackSize,
			entityItem->getEntityItem()->getItem()->getMetadata(entityItem->getEntityItem()->getItemDamage())))
		{
			LordLogInfo("pick up item inventory full");
			showResultTipDialog(ResultCode::INVENTORY_FULL, "");
			return;
		}

		UICommon::checkMoney(m_pickUpItemPrice, m_currencyType, [=](bool isSuccess) {
			if (isSuccess)
			{
				ClientNetwork::Instance()->getSender()->sendPickUpItemPay(m_pickUpItemEntityId, m_pickUpItemId);
				getParent()->hideTipDialog();
			}
		});
	}

	bool gui_tipDialog::showRevive(int moneyType, int moneyCost, int countDown, int uniqueId, int index)
	{
		m_uniqueId = uniqueId;
		m_index = index;
		m_revive_type = moneyType;
		m_revive_price = moneyCost;

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::HungryGame)
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("survive_die_revive_win", GameClient::CGame::Instance()->getGameType());
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Sky_Royale)
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("skyroyale_die_revive_win", GameClient::CGame::Instance()->getGameType());
		}

		m_countDown = (float)countDown;
		this->m_showTipType = TipType::REVIVE;
		getParent()->showTipDialog();
		refreshUI();
		m_valueText->SetText(StringUtil::ToString(moneyCost).c_str());
		Wallet *wallet = Blockman::Instance()->m_pPlayer->m_wallet;
		m_iconImage->SetImage(UICommon::getCurrencyIconByType((CurrencyType)moneyType).c_str());
		m_iconImage->SetVisible(true);
		m_valueText->SetVisible(true);
		if (wallet)
		{
			ui64 money = wallet->getMoneyByCoinId(moneyType);
			m_sureBtn->SetEnabled(money >= moneyCost);
			m_sureBtn->SetText(getString(money >= moneyCost ? LanguageKey::GUI_DIALOG_TIP_REVIVE_SURE : LanguageKey::LACK_OF_MONEY));
		}
		else
		{
			m_sureBtn->SetEnabled(false);
		}

		if (m_is_showAd && checkGameType(ClientGameType::Sky_Royale))
		{
			m_watchAdBtn->SetText(getString("g1027.free.revive"));
			m_adHint->SetText(getString("g1027.watch.ad.free.revive"));
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("skyroyale_die_revive_ad_show", GameClient::CGame::Instance()->getGameType());
		}
		else if (m_is_showAd && checkGameType(ClientGameType::HungryGame))
		{
			m_watchAdBtn->SetText(getString("g1001.free.revive"));
			m_adHint->SetText(getString("g1001.watch.ad.free.revive"));
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("survive_die_revive_ad_show", GameClient::CGame::Instance()->getGameType());
		}
		return true;
	}

	void gui_tipDialog::closeRevive()
	{
		m_countDown = 0.0f;
		getParent()->hideTipDialog();
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyRespawn(false, -1, -1);
		LordLogInfo("C2SPacketBuyRespawn:%d", Root::Instance()->getCurrentTime());
	}

	void gui_tipDialog::closeKeepItemTip()
	{
		m_countDown = 0.0f;
		getParent()->hideTipDialog();
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendKeepItem(false, KeepItemType::KEEP_ITEM);
	}

	TipType gui_tipDialog::getShowTipType()
	{
		return m_showTipType;
	}

	bool gui_tipDialog::showCustomTipMsg(const String&  messageLang, const String&  extra)
	{
		this->m_showTipType = TipType::CUSTOM_TIP;
		this->m_tipExtra = extra;
		m_tipMessage = getString(messageLang).c_str();
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showKeepItemTip(int coinType, int coin, int time)
	{
		m_countDown = (float)time;
		this->m_showTipType = TipType::KEEP_ITEM;
		getParent()->showTipDialog();
		refreshUI();
		m_valueText->SetText(StringUtil::ToString(coin).c_str());
		Wallet *wallet = Blockman::Instance()->m_pPlayer->m_wallet;
		m_iconImage->SetImage(UICommon::getCurrencyIconByType((CurrencyType)coinType).c_str());
		m_iconImage->SetVisible(true);
		m_valueText->SetVisible(true);
		if (wallet)
		{
			ui64 money = wallet->getMoneyByCoinId(coinType);
			m_sureBtn->SetEnabled(money >= coin);
			m_sureBtn->SetText(getString(money >= coin ? LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_SURE : LanguageKey::LACK_OF_MONEY));
		}
		else
		{
			m_sureBtn->SetEnabled(false);
		}
		return true;
	}

	bool gui_tipDialog::showEnchantmentQuickTip(int diamond, int equipId, int effectIndex)
	{
		this->m_showTipType = TipType::ENCHANTMENT_QUICK;
		m_iconImage->SetImage(UICommon::getCurrencyIconByType(CurrencyType::PLATFORM_MONEY).c_str());
		m_valueText->SetText(StringUtil::ToString(diamond).c_str());
		m_iconImage->SetVisible(true);
		m_valueText->SetVisible(true);
		m_enchantment_equip_id = equipId;
		m_enchantment_effect_index = effectIndex;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_ENCHANTMENT_QUICK).c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showPixelGunHallNotOpen(int type)
	{
		this->m_showTipType = TipType::PIXEL_GUNHALL_MODE_NOT_OPEN;
		m_tipMessage = getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_NOT_OPEN_TIP_TXT).c_str();

		if (type == 0)
		{
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_NOT_OPEN_TIP_TITLE));
		}
		else if (type == 1)
		{
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MAP_NOT_OPEN_TIP_TITLE));
		}
		
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showPixelGunHallLvNotEnough(int lv, int type)
	{
		this->m_showTipType = TipType::PIXEL_GUNHALL_LOCK_MODE;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_LOCK_TIP_TXT).c_str(), lv);

		if (type == 0)
		{
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_LOCK_TIP_TITLE));
		}
		else
		{
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MAP_LOCK_TIP_TITLE));
		}

		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showPixelGunHallMapLockDiamond(int num, int mapId)
	{
		this->m_showTipType = TipType::PIXEL_GUNHALL_LOCK_MAP_DIAMOND;
		m_pixelhall_map_id = mapId;
		m_iconImage->SetImage(UICommon::getCurrencyIconByType(CurrencyType::PLATFORM_MONEY).c_str());
		m_valueText->SetText(StringUtil::ToString(num).c_str());
		m_iconImage->SetVisible(true);
		m_valueText->SetVisible(true);
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_PIXEL_GUNHALL_MAP_DIAMOND_LOCK_TIP_TXT).c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showQuicklyBuildBlock()
	{
		this->m_showTipType = TipType::QUICKLY_BUILD_BLOCK;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_QUICKLY_BUILD_BLOCK).c_str(), LogicSetting::Instance()->getQuicklyBuildBlockNum());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showMainLineTaskLvNotEnough(int lv)
	{
		this->m_showTipType = TipType::MAIN_LINE_TASK_LV_LOCK;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::MAIN_LINE_TASK_LV_LOCK_TIP_TEXT).c_str(), lv);
		m_titleText->SetText(getString(LanguageKey::MAIN_LINE_TASK_LV_LOCK_TIP_TITLE));
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showSkyBlockMainLineTaskLvNotEnough(const String msg)
	{
		this->m_showTipType = TipType::SKYBLOCK_MAIN_LINE_TASK_LV_LOCK;
		m_tipMessage = msg;
		m_titleText->SetText(getString(LanguageKey::MAIN_LINE_TASK_LV_LOCK_TIP_TITLE));
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showUnlockSelectableActionTip(int actionId)
	{
		this->m_showTipType = TipType::UNLOCK_SELECTABLE_ACTION;
		SelectableActionInfo actionInfo;
		bool hasActionInfo = AppItemsSetting::Instance()->getActionInfoFromMap(actionId, actionInfo);
		if (hasActionInfo)
		{
			m_iconImage->SetVisible(true);
			m_valueText->SetVisible(true);
			m_iconImage->SetImage(actionInfo.currency == 2 ? "set:summary.json image:coin" : "set:app_shop.json image:app_cube_icon");
			m_valueText->SetText(StringUtil::ToString(actionInfo.price).c_str());
			m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_UNLOCK_MSG).c_str(), actionInfo.currency == 2 ? getString(LanguageKey::GUI_DIALOG_TIP_UNLOCK_MONEY_TYPE_GOLD_COINS).c_str() : getString(LanguageKey::GUI_DIALOG_TIP_UNLOCK_MONEY_TYPE_BCUBES).c_str());
			m_toUnlockActionId = actionInfo.buyId;
			getParent()->showTipDialog();
			refreshUI();
		}
		else
		{
			getParent()->hideTipDialog();
		}
		return true;
	}

	bool gui_tipDialog::showBedWarReclaimTip(int left_num, const String & name, int rune_id)
	{
		this->m_showTipType = TipType::BEDWAR_RECLAIM_TIP;
		m_bedWar_reclaim_rune_id = rune_id;
		String msg = StringUtil::Format(getString(LanguageKey::GUI_BEDWAR_RECLAIM_MSG_TIP).c_str(), getString(name.c_str()).c_str(), left_num);
		m_messageText->SetText(msg.c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showBedWarReclaimAllTip(int sum, int key_sum, int type)
	{
		this->m_showTipType = TipType::BEDWAR_RECLAIM_ALL_TIP;
		m_bedWar_reclaim_all_type = type;
		String msg = StringUtil::Format(getString(LanguageKey::GUI_BEDWAR_RECLAIM_ALL_MSG_TIP).c_str(), sum, key_sum);
		m_messageText->SetText(msg.c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showBedWarRuneUnlockTip(int num, int type)
	{
		this->m_showTipType = TipType::BED_WALL_HALL_UNLOCK_RUNE;
		String msg = StringUtil::Format("%d", num);
		m_bedWar_rune_unlock_type = type;
		m_messageText->SetText(getString(LanguageKey::GUI_BEDWAR_UNLOCK_RUNE_MSG_TIP).c_str());
		m_valueText->SetText(msg.c_str());
		m_iconImage->SetImage(UICommon::getCurrencyIconByType(KEY_MONEY).c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	void  gui_tipDialog::refreshCustomTipUI()
	{
		m_titleText->SetText(getString(""));
		m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
		m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
		m_sureBtn->SetEnabled(true);
		m_sureBtn->SetVisible(true);
		m_cancelBtn->SetVisible(true);
		m_otherMsgText->SetText(m_tipMessage.c_str());
		m_contentWindow->SetVisible(false);
		m_otherContentWindow->SetVisible(true);
		m_otherLackOfMoneyWindow->SetVisible(false);
	}

	void gui_tipDialog::SyncCustomTipResult(bool bResult)
	{
		ClientNetwork::Instance()->getSender()->sendCustomTipResult(this->m_tipExtra, bResult);
	}


	bool gui_tipDialog::onShowSkyBlockCostMuchMoneyMsg(const String & message, int coinId, int price, int id, int num, const String& Name)
	{
		m_showTipType = TipType::SKYBLOCK_COST_MUCH_MONEY;
		m_tipMessage = StringUtil::Format(getString(message).c_str(), num, getString(Name).c_str()).c_str();
		m_coinId = coinId;
		needMoneyCount = price;
		m_cost_much_id = id;
		m_cost_much_num = num;
		m_iconImage->SetImage(UICommon::getCurrencyIconByType(coinId).c_str());
		m_iconImage->SetVisible(true);
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}
}
