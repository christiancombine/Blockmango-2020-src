#include "gui_blockCityTip.h"
#include "Util/LanguageKey.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "game.h"
#include "ShellInterface.h"
#include "rapidjson/document.h"
#include "Entity/EntitySessionNpc.h"
#include "GUI/GuiBlockCityTigerLotteryItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Setting/BlockCitySetting.h"
#include "GUI/GuiBlockCityBuyFlyItem.h"
#include "Network/protocol/CommonDataBuilder.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_blockCityTip::gui_blockCityTip()
		: gui_layout("BlockCityTip.json")
	{
	}

	gui_blockCityTip::~gui_blockCityTip()
	{
	}

	void gui_blockCityTip::onLoad()
	{

		m_tvTitle = getWindow<GUIStaticText>("BlockCityTip-Title-Name");
		m_tvPanelMsg = getWindow<GUIStaticText>("BlockCityTip-Panel");
		m_btnLeft = getWindow<GUIButton>("BlockCityTip-Left-Button");
		m_btnRight = getWindow<GUIButton>("BlockCityTip-Right-Button");
		m_leftIcon = getWindow<GUIStaticImage>("BlockCityTip-Left-Button-Money-Icon");
		m_tvLeftValue = getWindow<GUIStaticText>("BlockCityTip-Left-Button-Money-Num");

		m_buyLayout = getWindow<GUILayout>("BlockCityTip-Buy");
		m_tvBuyOneName = getWindow<GUIStaticText>("BlockCityTip-Buy-One-Msg");
		m_buyOneIcon = getWindow<GUIStaticImage>("BlockCityTip-Buy-One-Icon");
		m_tvBuyOneValue = getWindow<GUIStaticText>("BlockCityTip-Buy-One-Num");
		m_tvBuyManyName = getWindow<GUIStaticText>("BlockCityTip-Buy-Many-Msg");
		m_buyManyIcon = getWindow<GUIStaticImage>("BlockCityTip-Buy-Many-Icon");
		m_tvBuyManyValue = getWindow<GUIStaticText>("BlockCityTip-Buy-Many-Num");

		m_timeLayout = getWindow<GUILayout>("BlockCityTip-TimeTip");
		m_tvTime = getWindow<GUIStaticText>("BlockCityTip-TimeTip-Time");
		m_timeMoneyIcon = getWindow<GUIStaticImage>("BlockCityTip-TimeTip-Money-Icon");
		m_tvTimeMoneyValue = getWindow<GUIStaticText>("BlockCityTip-TimeTip-Money-Num");

		m_list = getWindow<GUIListHorizontalBox>("BlockCityTip-List");

		m_watchAdBtn = getWindow<GUIButton>("BlockCityTip-Tigger-AD");
		m_watchAdBtn->SetVisible(false);

		setText("BlockCityTip-TimeTip-Msg", "gui_blockcity_tip_time_msg");
		setText("BlockCityTip-Tigger-AD-Text", "gui_cd_free");

		getWindow<GUIButton>("BlockCityTip-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTip::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow<GUIButton>("BlockCityTip-Buy-One")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTip::onClick, this, std::placeholders::_1, ViewId::BUY_ONE));
		getWindow<GUIButton>("BlockCityTip-Buy-Many")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTip::onClick, this, std::placeholders::_1, ViewId::BUY_MANY));
		getWindow<GUIButton>("BlockCityTip-Buy-Cancel")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTip::onClick, this, std::placeholders::_1, ViewId::BUY_CANCEL));
		m_btnLeft->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTip::onClick, this, std::placeholders::_1, ViewId::LEFT));
		m_btnRight->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTip::onClick, this, std::placeholders::_1, ViewId::RIGHT));
		m_watchAdBtn->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTip::onClick, this, std::placeholders::_1, ViewId::WATCH_AD));

		m_subscriptionGuard.add(ShowBlockCityTigerLotteryTipEvent::subscribe(std::bind(&gui_blockCityTip::showTigerLotteryTip, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowBlockCityTipEvent::subscribe(std::bind(&gui_blockCityTip::showTip, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowBlockCityShowCommonTipEvent::subscribe(std::bind(&gui_blockCityTip::showCommonTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowBlockCityLockItemsEvent::subscribe(std::bind(&gui_blockCityTip::showBlockCityLockItems, this)));
		m_subscriptionGuard.add(BlockCitySendDeleteFriendIdEvent::subscribe(std::bind(&gui_blockCityTip::setDeleteFriendId, this, std::placeholders::_1)));	
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_blockCityTip::onVideoAdVisible, this, std::placeholders::_1)));
	}

	bool gui_blockCityTip::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	void gui_blockCityTip::onUpdate(ui32 nTimeElapse)
	{
		if (isShown() && m_tipType == BlockCityTipType::BLOCKCITY_TIGER_LOTTERY && m_leftTime > 0)
		{
			m_leftTime = (m_leftTime - nTimeElapse > 0) ? m_leftTime - nTimeElapse : 0;
			m_tvTime->SetText(UICommon::timeFormat(m_leftTime).c_str());
			m_timeMoneyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
			m_tvTimeMoneyValue->SetText(StringUtil::ToString(m_cubePrice).c_str());
			if (m_leftTime == 0)
			{
				RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			}
		}
	}

	bool gui_blockCityTip::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId) {
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			break;
		case ViewId::LEFT:
			if (m_tipType == BlockCityTipType::BLOCKCITY_TIGER_LOTTERY)
			{
				RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			}
			else
			{
				btnSure();
			}
			break;
		case ViewId::RIGHT:
			if (m_tipType == BlockCityTipType::BLOCKCITY_TIGER_LOTTERY)
			{
				btnSure();
			}
			else
			{
				RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			}
			break;
		case ViewId::BUY_ONE:
			if (m_tipType == BlockCityTipType::BLOCKCITY_LACK_GOODS)
			{
				UICommon::checkMoney(m_price, m_currencyType, [=](bool isSuccess) {
					if (isSuccess)
					{
						ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyLackingItems(1, i32(m_currencyType));
						RootGuiLayout::Instance()->setBlockCityTipVisible(false);
					}
					else
					{
						vector<BlockCityPayment>::type pays;
						BlockCityPayment pay;
						pay.num = 1;
						pay.price = m_cubePrice;
						pay.moneyType = 1;
						pays.push_back(pay);
						ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_LACK_MONEY_LACK_ITEMS, "gui_blockcity_lock_money_tip");
					}
				});
			}
			break;
		case ViewId::BUY_MANY:
			if (m_tipType == BlockCityTipType::BLOCKCITY_LACK_GOODS)
			{
				UICommon::checkMoney(m_price * 64, m_currencyType, [=](bool isSuccess) {
					if (isSuccess)
					{
						ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyLackingItems(2, i32(m_currencyType));
						RootGuiLayout::Instance()->setBlockCityTipVisible(false);
					}
					else
					{
						vector<BlockCityPayment>::type pays;
						BlockCityPayment pay;
						pay.num = 2;
						pay.price = (i32)Math::Ceil(m_price * 64 / 100.f);
						pay.moneyType = 1;
						pays.push_back(pay);
						ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_LACK_MONEY_LACK_ITEMS, "gui_blockcity_lock_money_tip");
					}
				});
			}
			break;
		case ViewId::BUY_CANCEL:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			break;
		case ViewId::WATCH_AD:
			onWatchAd();
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			break;
		}
		return false;
	}

	void gui_blockCityTip::btnSure()
	{
		switch (m_tipType)
		{
		case BlockCityTipType::BLOCKCITY_TIGER_LOTTERY:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			UICommon::checkMoney(m_cubePrice, m_currencyType, [=](bool isSuccess) {
				if (isSuccess)
				{
					SetBlockCityLotteryByNpcEvent::emit(m_tigerLotteryEntityId);
				}
			});
			break;
		case BlockCityTipType::BLOCKCITY_DELETE_MANOR:
			ClientNetwork::Instance()->getCommonSender()->sendBlockCityManorBuyOperation(1, i32(CurrencyType::GAME_MONEY));
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			RootGuiLayout::Instance()->showMainControl();
			playSoundByType(ST_clear_manor);
			break;
		case BlockCityTipType::BLOCKCITY_GO_HOME:
			if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer)
			{
				return;
			}
			playSoundByType(ST_go_home);
			ClientNetwork::Instance()->getSender()->sendCallOnManor(Blockman::Instance()->m_pPlayer->getPlatformUserId());
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			break;
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_STORE:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			UICommon::checkMoney(m_price, m_currencyType, [=](bool isSuccess) {
				if (isSuccess)
				{
					ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyGoods(m_storTabId, m_storItemId, m_storeBuyType, i32(m_currencyType));
				}
			});
			break;
		case BlockCityTipType::BLOCKCITY_LACK_GOODS:
			if (m_currencyType == CurrencyType::PLATFORM_MONEY)
			{
				RootGuiLayout::Instance()->setBlockCityTipVisible(false);
				UICommon::checkMoney(m_cubePrice, m_currencyType, [=](bool isSuccess) {
					if (isSuccess)
					{
						ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyLackingItems(1, i32(m_currencyType));
					}
				});
			}
			else
			{
				UICommon::checkMoney(m_price, m_currencyType, [=](bool isSuccess) {
					if (isSuccess)
					{
						RootGuiLayout::Instance()->setBlockCityTipVisible(false);
						ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyLackingItems(1, i32(m_currencyType));
					}
					else
					{
						vector<BlockCityPayment>::type pays;
						BlockCityPayment pay;
						pay.num = 1;
						pay.price = m_cubePrice;
						pay.moneyType = 1;
						pays.push_back(pay);
						ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_LACK_MONEY_LACK_ITEMS, "gui_blockcity_lock_money_tip");
					}
				});
			}			
			break;
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_LACK_ITEMS:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			UICommon::checkMoney(m_price, m_currencyType, [=](bool isSuccess) {
				if (isSuccess)
				{
					ClientNetwork::Instance()->getCommonSender()->sendBlockCityBuyLackingItems(m_storeBuyType, i32(m_currencyType));
				}
			});
			break;
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_DELETE:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			UICommon::checkMoney(m_price, m_currencyType, [=](bool isSuccess) {
				if (isSuccess)
				{
					ClientNetwork::Instance()->getCommonSender()->sendBlockCityManorBuyOperation(1, i32(m_currencyType));
					RootGuiLayout::Instance()->showMainControl();
				}
			});
			break;
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_UPGRADE:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			UICommon::checkMoney(m_price, m_currencyType, [=](bool isSuccess) {
				if (isSuccess)
				{
					ClientNetwork::Instance()->getCommonSender()->sendBlockCityManorBuyOperation(2, i32(m_currencyType));
				}
			});
			break;
		case BlockCityTipType::BLOCKCITY_DELETE_FRIEND:
			BlockCityDeleteFriendEvent::emit(m_deleteFriendId);
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			break;
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_HOUSE_SHOP:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			UICommon::checkMoney(m_price, m_currencyType, [=](bool isSuccess) {
				if (isSuccess)
				{
					ClientNetwork::Instance()->getCommonSender()->sendBlockCityHouseShopBuyGoods(m_storTabId, m_storItemId, i32(m_currencyType), m_index);
					Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = m_index;
					Blockman::Instance()->m_pPlayer->inventory->onInventoryChanged();
				}
			});
			break;
		case BlockCityTipType::BLOCKCITY_GO_SHOP:
			if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer)
			{
				return;
			}
			playSoundByType(ST_go_home);
			ClientNetwork::Instance()->getCommonSender()->sendBlockCityTeleportToShop();
			RootGuiLayout::Instance()->showMainControl();
			break;
		case BlockCityTipType::BLOCKCITY_DELETE_ARCHIVE:
			BlockCityArchiveSureEvent::emit(1);
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			break;
		case BlockCityTipType::BLOCKCITY_ARCHIVE_BUY_SAVE:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			UICommon::checkMoney(m_price, CurrencyType::PLATFORM_MONEY, [=](bool isSuccess) {
				if (isSuccess)
				{
					ClientNetwork::Instance()->getCommonSender()->sendBlockCitylUnlockNewArchive();
					BlockCityArchiveSureEvent::emit(2);
				}
			});
			break;
		case BlockCityTipType::BLOCKCITY_ARCHIVE_COVER:
			BlockCityArchiveSureEvent::emit(2);
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			break;
		case BlockCityTipType::BLOCKCITY_ARCHIVE_READ:
			BlockCityArchiveSureEvent::emit(3);
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			break;
		case  BlockCityTipType::BLOCKCITY_ARCHIVE_BUY:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			UICommon::checkMoney(m_price, CurrencyType::PLATFORM_MONEY, [=](bool isSuccess) {
				if (isSuccess)
				{
					ClientNetwork::Instance()->getCommonSender()->sendBlockCitylUnlockNewArchive();
				}
			});
			break;
		default:
			RootGuiLayout::Instance()->setBlockCityTipVisible(false);
			break;
		}
	}

	void gui_blockCityTip::onWatchAd()
	{
		int entityId = ClientNetwork::Instance()->getDataCache()->getServerId(m_tigerLotteryEntityId);
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::BLOCK_CITY_TIGER, StringUtil::ToString(entityId));
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("realm_chest_cd_ad_click", GameClient::CGame::Instance()->getGameType());
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("click_reward_ad", GameClient::CGame::Instance()->getGameType());
	}

	bool gui_blockCityTip::showTip(const String & msg)
	{
		showCommonTip(vector<BlockCityPayment>::type() , (int)BlockCityTipType::COMMON, msg);
		return true;
	}

	bool gui_blockCityTip::showCommonTip(const vector<BlockCityPayment>::type& pays, i32 tipType, const String & msg)
	{
		m_watchAdBtn->SetVisible(false);
		m_tipType = (BlockCityTipType)tipType;
		m_tvPanelMsg->SetVisible(true);
		m_timeLayout->SetVisible(false);
		m_list->SetVisible(false);
		m_tvPanelMsg->SetText(getString(msg));
		switch (pays.size())
		{
		case 0:
			setText(m_tvTitle, "gui_blockcity_tip_title");

			m_btnLeft->SetVisible(false);
			m_btnRight->SetVisible(true);
			m_buyLayout->SetVisible(false);
			m_btnRight->SetHorizontalAlignment(HA_CENTRE);
			setText(m_btnRight, "gui_blockcity_tip_sure");
			m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:sure");
			m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:sure");
			break;
		case 1:
			m_buyLayout->SetVisible(false);

			m_btnLeft->SetVisible(true);
			m_btnRight->SetVisible(true);
			m_btnRight->SetHorizontalAlignment(HA_RIGHT);
			m_leftIcon->SetVisible(true);
			m_tvLeftValue->SetVisible(true);
			setText(m_btnLeft, "");
			setText(m_btnRight, "gui_blockcity_tip_cancel");
			m_btnLeft->SetNormalImage("set:blockcity_main_tip.json image:sure");
			m_btnLeft->SetPushedImage("set:blockcity_main_tip.json image:sure");
			m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:cancel");
			m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:cancel");

			m_currencyType = (CurrencyType)pays[0].moneyType;
			m_price = pays[0].price;
			m_leftIcon->SetImage(UICommon::getCurrencyIconByType(pays[0].moneyType).c_str());
			m_tvLeftValue->SetText(StringUtil::ToString(pays[0].price).c_str());
			break;
		case 2:
			m_btnLeft->SetVisible(false);
			m_btnRight->SetVisible(false);
			m_buyLayout->SetVisible(true);
			setText("BlockCityTip-Buy-Cancel", "gui_blockcity_tip_cancel");
			break;
		}
		switch ((BlockCityTipType)tipType)
		{
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_STORE:
			setText(m_tvTitle, "gui_blockcity_lock_money");
			m_storeBuyType = pays[0].num;
			m_storTabId = pays[0].tabId;
			m_storItemId = pays[0].itemId;
			break;
		case BlockCityTipType::BLOCKCITY_DELETE_MANOR:
			setText(m_tvTitle, "gui_blockcity_delete_title");

			m_btnLeft->SetVisible(true);
			m_btnRight->SetHorizontalAlignment(HA_RIGHT);
			m_leftIcon->SetVisible(false);
			m_tvLeftValue->SetVisible(false);
			setText(m_btnLeft, "gui_blockcity_delete");
			setText(m_btnRight, "gui_blockcity_tip_cancel");
			m_btnLeft->SetNormalImage("set:blockcity_main_tip.json image:sure");
			m_btnLeft->SetPushedImage("set:blockcity_main_tip.json image:sure");
			m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:cancel");
			m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:cancel");			
			break;
		case BlockCityTipType::BLOCKCITY_GO_HOME:
			setText(m_tvTitle, "gui_blockcity_transfer_title");
			m_btnLeft->SetVisible(true);
			m_btnRight->SetHorizontalAlignment(HA_RIGHT);
			m_leftIcon->SetVisible(false);
			m_tvLeftValue->SetVisible(false);
			setText(m_btnLeft, "gui_blockcity_transfer");
			setText(m_btnRight, "gui_blockcity_tip_cancel");
			m_btnLeft->SetNormalImage("set:blockcity_main_tip.json image:sure");
			m_btnLeft->SetPushedImage("set:blockcity_main_tip.json image:sure");
			m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:cancel");
			m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:cancel");
			break;
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_LACK_ITEMS:
			setText(m_tvTitle, "gui_blockcity_lock_money");
			m_storeBuyType = pays[0].num;
			break;
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_DELETE:
			setText(m_tvTitle, "gui_blockcity_lock_money");
			break;
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_UPGRADE:
			setText(m_tvTitle, "gui_blockcity_lock_money");
			break;
		case BlockCityTipType::BLOCKCITY_DELETE_FRIEND:
			m_btnLeft->SetVisible(true);
			m_btnRight->SetHorizontalAlignment(HA_RIGHT);
			m_leftIcon->SetVisible(false);
			m_tvLeftValue->SetVisible(false);
			setText(m_btnLeft, "gui_blockcity_tip_sure");
			setText(m_btnRight, "gui_blockcity_tip_cancel");
			m_btnLeft->SetNormalImage("set:blockcity_main_tip.json image:sure");
			m_btnLeft->SetPushedImage("set:blockcity_main_tip.json image:sure");
			m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:cancel");
			m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:cancel");
			break;
		case BlockCityTipType::BLOCKCITY_LACK_MONEY_HOUSE_SHOP:
			setText(m_tvTitle, "gui_blockcity_lock_money");
			m_storeBuyType = pays[0].num;
			m_storTabId = pays[0].tabId;
			m_storItemId = pays[0].itemId;
			m_index = pays[0].index;
			break;
		case BlockCityTipType::BLOCKCITY_GO_SHOP:
			setText(m_tvTitle, "gui_blockcity_transfer_title");
			m_btnLeft->SetVisible(true);
			m_btnRight->SetHorizontalAlignment(HA_RIGHT);
			m_leftIcon->SetVisible(false);
			m_tvLeftValue->SetVisible(false);
			setText(m_btnLeft, "gui_blockcity_transfer");
			setText(m_btnRight, "gui_blockcity_tip_cancel");
			m_btnLeft->SetNormalImage("set:blockcity_main_tip.json image:sure");
			m_btnLeft->SetPushedImage("set:blockcity_main_tip.json image:sure");
			m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:cancel");
			m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:cancel");
			break;
		case BlockCityTipType::BLOCKCITY_ARCHIVE_COVER:
		case BlockCityTipType::BLOCKCITY_DELETE_ARCHIVE:
		case BlockCityTipType::BLOCKCITY_ARCHIVE_READ:
			m_btnLeft->SetVisible(true);
			m_btnRight->SetHorizontalAlignment(HA_RIGHT);
			m_leftIcon->SetVisible(false);
			m_tvLeftValue->SetVisible(false);
			setText(m_btnLeft, "gui_blockcity_tip_sure");
			setText(m_btnRight, "gui_blockcity_tip_cancel");
			m_btnLeft->SetNormalImage("set:blockcity_main_tip.json image:sure");
			m_btnLeft->SetPushedImage("set:blockcity_main_tip.json image:sure");
			m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:cancel");
			m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:cancel");
			break;
		default:
			break;
		}
		RootGuiLayout::Instance()->setBlockCityTipVisible(true);
		return true;
	}

	bool gui_blockCityTip::showTigerLotteryTip(i32 entityId)
	{
		m_tipType = BlockCityTipType::BLOCKCITY_TIGER_LOTTERY;
		m_currencyType = CurrencyType::PLATFORM_MONEY;
		auto entity = dynamic_cast<EntitySessionNpc*>(Blockman::Instance()->m_pWorld->getEntity(entityId));
		if (!entity)
		{
			return false;
		}

		m_tigerLotteryEntityId = entityId;
		m_leftTime = entity->getTimeLeft();

		auto data = entity->getSessionContent();
		if (data.length() == 0)
			return false;
		rapidjson::Document doc = rapidjson::Document();
		doc.Parse(data.c_str());

		if (doc.HasParseError())
		{
			LordLogError("The lottery data content HasParseError.");
			return false;
		}

		if (doc.HasMember("cubeMoney"))
		{
			auto doc_cubePrice = doc.FindMember("cubeMoney");
			m_cubePrice = doc_cubePrice->value.GetInt();
		}

		setText(m_tvTitle, "gui_blockcity_tip_title");
		m_timeLayout->SetVisible(true);
		m_tvPanelMsg->SetVisible(false);
		m_list->SetVisible(false);
		m_btnLeft->SetVisible(true);
		m_btnRight->SetVisible(true);
		m_btnRight->SetHorizontalAlignment(HA_RIGHT);
		m_leftIcon->SetVisible(false);
		m_tvLeftValue->SetVisible(false);
		m_buyLayout->SetVisible(false);
		setText(m_btnLeft, "gui_blockcity_tip_cancel");
		setText(m_btnRight, "gui_blockcity_tip_sure");
		m_btnLeft->SetNormalImage("set:blockcity_main_tip.json image:cancel");
		m_btnLeft->SetPushedImage("set:blockcity_main_tip.json image:cancel");
		m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:sure");
		m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:sure");

		m_watchAdBtn->SetVisible(false);
		if (entity->getActorBodyId() == "1")
		{
			if (m_canShowTiger2Ad && m_isAdReady)
			{
				m_watchAdBtn->SetVisible(true);
				GameClient::CGame::Instance()->getShellInterface()->
					dataReport("realm_chest_cd_ad_show", GameClient::CGame::Instance()->getGameType());
			}
		}
		else
		{
			if (m_canShowTiger1Ad && m_isAdReady)
			{
				m_watchAdBtn->SetVisible(true);
				GameClient::CGame::Instance()->getShellInterface()->
					dataReport("realm_chest_cd_ad_show", GameClient::CGame::Instance()->getGameType());
			}
		}

		RootGuiLayout::Instance()->setBlockCityTipVisible(true);
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("realm_chest_cd_win", GameClient::CGame::Instance()->getGameType());
		return true;
	}

	bool gui_blockCityTip::showBlockCityLockItems()
	{
		m_watchAdBtn->SetVisible(false);
		auto lacks = Blockman::Instance()->m_pPlayer->m_blockCity->getBuyLackItems();
		if (lacks)
		{
			m_timeLayout->SetVisible(false);
			switch (lacks->type)
			{
			case 1:
			{
				m_tipType = BlockCityTipType::BLOCKCITY_LACK_GOODS;
				setText(m_tvTitle, "gui_blockcity_lock_material");
				m_tvPanelMsg->SetVisible(false);
				m_buyLayout->SetVisible(false);
				m_list->SetVisible(true);
				m_btnLeft->SetVisible(true);
				m_leftIcon->SetVisible(true);
				m_tvLeftValue->SetVisible(true);
				m_btnRight->SetVisible(true);
				m_btnRight->SetHorizontalAlignment(HA_RIGHT);
				setText(m_btnRight, "gui_blockcity_tip_cancel");
				setText(m_btnLeft, "");
				m_btnLeft->SetNormalImage("set:blockcity_main_tip.json image:sure");
				m_btnLeft->SetPushedImage("set:blockcity_main_tip.json image:sure");
				m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:cancel");
				m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:cancel");

				m_price = lacks->price;
				m_cubePrice = lacks->priceCube;
				m_currencyType = CurrencyType::GAME_MONEY;
				m_leftIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
				m_tvLeftValue->SetText(StringUtil::ToString(lacks->price).c_str());
				static int count = 0;
				m_list->ClearAllItem();
				for (const auto& item : lacks->items)
				{
					String iterLayout = StringUtil::Format("BlockCityTip-List-Items-%d", count++).c_str();
					GuiBlockCityTigerLotteryItem* itemView = (GuiBlockCityTigerLotteryItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_TIGER_LOTTERY_ITEM, iterLayout.c_str());
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 65.f), UDim(1.f, 0));
					itemView->setLackItem(item);
					m_list->AddItem(itemView, false);
				}
			}
				break;
			case  2:
			{
				m_tipType = BlockCityTipType::BLOCKCITY_LACK_GOODS;
				setText(m_tvTitle, "gui_blockcity_lock_item_title");
				m_tvPanelMsg->SetVisible(true);
				m_buyLayout->SetVisible(true);
				m_list->SetVisible(false);
				m_btnLeft->SetVisible(false);
				m_btnRight->SetVisible(false);
				setText("BlockCityTip-Buy-Cancel", "gui_blockcity_tip_cancel");
				if (lacks->items.size() > 0)
				{
					m_price = lacks->price;
					m_cubePrice = lacks->priceCube;
					m_currencyType = CurrencyType::GAME_MONEY;
					ItemStackPtr itemStack = LORD::make_shared<ItemStack>(lacks->items[0].itemId, 1, lacks->items[0].itemMeta);
					String itemName = LanguageManager::Instance()->getItemName(itemStack->getItemName());
					m_tvPanelMsg->SetText(StringUtil::Format(getString("gui_blockcity_lock_item").c_str(), itemName.c_str()).c_str());
					setText(m_tvBuyOneName, "gui_blockcity_lock_item_buy_one");
					setText(m_tvBuyManyName, "gui_blockcity_lock_item_buy_gcroup");
					m_buyOneIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
					m_buyManyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
					m_tvBuyOneValue->SetText(StringUtil::ToString(lacks->price).c_str());
					m_tvBuyManyValue->SetText(StringUtil::ToString(lacks->price * 64).c_str());
				}
			}
				break;
			case 3:
			{
				m_tipType = BlockCityTipType::BLOCKCITY_LACK_GOODS;
				setText(m_tvTitle, "gui_blockcity_lock_item_title");
				m_tvPanelMsg->SetVisible(true);
				m_btnLeft->SetVisible(true);
				m_leftIcon->SetVisible(true);
				m_tvLeftValue->SetVisible(true);
				m_btnRight->SetVisible(true);
				m_btnRight->SetHorizontalAlignment(HA_RIGHT);
				setText(m_btnRight, "gui_blockcity_tip_cancel");
				setText(m_btnLeft, "");
				m_btnLeft->SetNormalImage("set:blockcity_main_tip.json image:sure");
				m_btnLeft->SetPushedImage("set:blockcity_main_tip.json image:sure");
				m_btnRight->SetNormalImage("set:blockcity_main_tip.json image:cancel");
				m_btnRight->SetPushedImage("set:blockcity_main_tip.json image:cancel");
				
				m_list->SetVisible(false);
				m_buyLayout->SetVisible(false);
				if (lacks->items.size() == 1)
				{
					m_price = lacks->price;
					m_cubePrice = lacks->priceCube;
					String itemName = BlockCitySetting::getItemName(lacks->items[0].id);
					i32 moneyType = Blockman::Instance()->m_pPlayer->m_blockCity->getShopItemCurrencyTypeByLackItemId(lacks->items[0].id);
					m_currencyType = CurrencyType(moneyType);
					i32 price = moneyType == i32(CurrencyType::PLATFORM_MONEY) ? m_cubePrice : m_price;
					m_tvPanelMsg->SetText(StringUtil::Format(getString("gui_blockcity_lock_item").c_str(), getString(itemName).c_str()).c_str());
					m_leftIcon->SetImage(UICommon::getCurrencyIconByType(moneyType).c_str());
					m_tvLeftValue->SetText(StringUtil::ToString(price).c_str());
				}
			}
				break;
			default:
				return false;
				break;
			}
		}

		RootGuiLayout::Instance()->setBlockCityTipVisible(true);
		return true;
	}

	bool gui_blockCityTip::setDeleteFriendId(i64 userId)
	{
		m_deleteFriendId = userId;
		return true;
	}

	bool gui_blockCityTip::onVideoAdVisible(bool visible)
	{
		m_isAdReady = visible;
		return true;
	}

	void gui_blockCityTip::onGetCommonData(String key, String data)
	{
		if (key == "TigerAd1")
		{
			auto builder = CommonDataBuilder().fromData(data);
			bool isShow = builder.getBoolParam("isShow");
			m_canShowTiger1Ad = isShow;
			return;
		}

		if (key == "TigerAd2")
		{
			auto builder = CommonDataBuilder().fromData(data);
			bool isShow = builder.getBoolParam("isShow");
			m_canShowTiger2Ad = isShow;
			return;
		}
	}

}
