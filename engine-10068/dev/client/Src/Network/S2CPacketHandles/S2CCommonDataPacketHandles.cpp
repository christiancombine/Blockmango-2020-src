#include "S2CCommonDataPacketHandles.h"
#include "Network/CommonDataDispatch.h"
#include "GUI/RootGuiLayout.h"
#include "cWorld/Blockman.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Plugins/GameClientEvents.h"
#include "game.h"
#include "ShellInterface.h"
#include "Entity/EntityVehicle.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/PlayerPrefs.h"
#include "Setting/ClassificationSetting.h"
#include "GameAnalytics/GameAnalyticsSetting.h"
#include "Blockman/EntityRenders/EntityRenderManager.h"

static void onSyncEntityPlayerInfo(ui64 userId, String name, String key, String data)
{
	auto build = CommonDataBuilder().fromData(data);

	auto pPlayer = Blockman::Instance()->getWorld()->getPlayerByPlatformUserId(build.getUI64Param("userId"));
	NULL_RETURN_VOID(pPlayer);

	SCRIPT_EVENT::cSyncEntityPlayerInfoEvent::invoke(&build, pPlayer);
}

static void onSyncEntityActorNpcInfo(ui64 userId, String name, String key, String data)
{
	auto build = CommonDataBuilder().fromData(data);

	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(build.getIntParam("entityId"));

	auto pActorNpc = dynamic_cast<EntityActorNpc*>(pEntity);
	NULL_RETURN_VOID(pActorNpc);

	if (build.hasParam("textScale"))
	{
		auto textScale = build.getStringParam("textScale");
		pActorNpc->setFloatProperty("textScale", StringUtil::ParseFloat(textScale));
	}

	if (build.hasParam("actorName"))
	{
		auto actorName = build.getStringParam("actorName");
		pActorNpc->setActorName(actorName.c_str());
	}
	if (build.hasParam("headName"))
	{
		auto headName = build.getStringParam("headName");
		pActorNpc->setHeadName(headName.c_str());
	}
	if (build.hasParam("headNameMultiLeft") && build.hasParam("headNameMultiRight"))
	{
		auto headNameMultiLeft = build.getStringParam("headNameMultiLeft");
		auto headNameMultiRight = build.getStringParam("headNameMultiRight");
		auto headNameMulti = StringUtil::Format("%s=%s", headNameMultiLeft.c_str(), headNameMultiRight.c_str());
		pActorNpc->setHeadName(headNameMulti.c_str());
	}

	if (build.hasParam("scale"))
	{
		auto scale = build.getFloatParam("scale");
		pActorNpc->setScale(scale);
	}

	if (build.hasParam("height"))
	{
		auto height = build.getFloatParam("height");
		pActorNpc->setHeight(height);
	}

	EntityRenderManager::Instance()->refreshEntityActor(pActorNpc, pActorNpc->getActorName());
}

void ClientCommonDataHelper::init()
{
	CommonDataDispatch::Instance()->unregisterAll();

	CommonDataDispatch::Instance()->registerHandle("Lua", &S2CCommonDataPacketHandles::onLuaCommonEvent);
	CommonDataDispatch::Instance()->registerHandle("DispatchInfo", &S2CCommonDataPacketHandles::onDispatchInfo);

	CommonDataDispatch::Instance()->registerHandle("GameAnalytics", &S2CCommonDataPacketHandles::onGameAnalyticsData);
	CommonDataDispatch::Instance()->registerHandle("ShopData", &S2CCommonDataPacketHandles::onGetShopData);
	CommonDataDispatch::Instance()->registerHandle("AppShopData", &S2CCommonDataPacketHandles::onGetAppShopData);
	CommonDataDispatch::Instance()->registerHandle("VideoAdTipData", &S2CCommonDataPacketHandles::onGetVideoAdTipData);
	CommonDataDispatch::Instance()->registerHandle("MainControlData", &S2CCommonDataPacketHandles::onGetMainControlData);
	CommonDataDispatch::Instance()->registerHandle("ToolBarData", &S2CCommonDataPacketHandles::onGetToolBarData);
	CommonDataDispatch::Instance()->registerHandle("OpenMovingEnderChest", &S2CCommonDataPacketHandles::onOpenMovingEnderChest);
	CommonDataDispatch::Instance()->registerHandle("PlayerInfoData", &S2CCommonDataPacketHandles::onGetPlayerInfoData);
	CommonDataDispatch::Instance()->registerHandle("TipDialogData", &S2CCommonDataPacketHandles::onGetTipDialogData);

	CommonDataDispatch::Instance()->registerHandle("TycoonData", &S2CCommonDataPacketHandles::onGetTycoonData);
	CommonDataDispatch::Instance()->registerHandle("BedWarHallData", &S2CCommonDataPacketHandles::onGetBedWarHallData);
	CommonDataDispatch::Instance()->registerHandle("BedWarData", &S2CCommonDataPacketHandles::onGetBedWarData);
	CommonDataDispatch::Instance()->registerHandle("PixelGunHallData", &S2CCommonDataPacketHandles::onGetPixelGunHallData);
	CommonDataDispatch::Instance()->registerHandle("PixelGunGameData", &S2CCommonDataPacketHandles::onGetPixelGunGameData);
    CommonDataDispatch::Instance()->registerHandle("BlockCityStorageOperation", &S2CCommonDataPacketHandles::onBlockCityStorageOperation);
	CommonDataDispatch::Instance()->registerHandle("PlayingPraiseAction", &S2CCommonDataPacketHandles::onPlayingPraiseAction);
	CommonDataDispatch::Instance()->registerHandle("WalkingDeadGameData",&S2CCommonDataPacketHandles::onGetWalkingDeadGameData);

	CommonDataDispatch::Instance()->registerHandle("SkyBlockMiningAreaData", &S2CCommonDataPacketHandles::onGetSkyBlockMiningAreaData);
	CommonDataDispatch::Instance()->registerHandle("SkyBlockProductData", &S2CCommonDataPacketHandles::onGetSkyBlockProductData);
	CommonDataDispatch::Instance()->registerHandle("SkyBlockMainData", &S2CCommonDataPacketHandles::onGetSkyBlockMainData);

    CommonDataDispatch::Instance()->registerHandle("BlockCityData", &S2CCommonDataPacketHandles::onGetBlockCityData);
    CommonDataDispatch::Instance()->registerHandle("syncBlockCityCDRewardData", &S2CCommonDataPacketHandles::onSyncBlockCityCDRewardData);
	CommonDataDispatch::Instance()->registerHandle("BirdSimulatorData", &S2CCommonDataPacketHandles::onGetBirdSimulatorData);
	CommonDataDispatch::Instance()->registerHandle("DataReport", &S2CCommonDataPacketHandles::onGetShellInterfaceDataReport);
	CommonDataDispatch::Instance()->registerHandle("BlockCityTigerAdShowUI", &S2CCommonDataPacketHandles::onGetBlockCityTigerAdShowUI);
	CommonDataDispatch::Instance()->registerHandle("syncBlockCityRaceToGetOnCarOrNot", &S2CCommonDataPacketHandles::onBlockCityToGetOnCarOrNotRace);
	CommonDataDispatch::Instance()->registerHandle("BlockCityRaceVehicleResult", &S2CCommonDataPacketHandles::onBlockCityRaceVehicleResulr);
	CommonDataDispatch::Instance()->registerHandle("showBlockCityRaceFailMas", &S2CCommonDataPacketHandles::onShowBlockCityRaceFailMsg);

    CommonDataDispatch::Instance()->registerHandle("PixelAdData", &S2CCommonDataPacketHandles::onGetPixelAdData);
	CommonDataDispatch::Instance()->registerHandle("doHurtMsg", &S2CCommonDataPacketHandles::onDoHurtMsg);
    CommonDataDispatch::Instance()->registerHandle("doAttackMsg", &S2CCommonDataPacketHandles::onDoAttackMsg);
    CommonDataDispatch::Instance()->registerHandle("syncChestInfoMsg", &S2CCommonDataPacketHandles::onSyncChestInfoMsg);
	CommonDataDispatch::Instance()->registerHandle("syncDiePlayerMsg", &S2CCommonDataPacketHandles::onSyncDiePlayerMsg);
	CommonDataDispatch::Instance()->registerHandle("BlockData", &S2CCommonDataPacketHandles::onGetBlockData);
	CommonDataDispatch::Instance()->registerHandle("SkyWarLuckyBlockData", &S2CCommonDataPacketHandles::onGetSkyWarLuckyBlockData);
	//CommonDataDispatch::Instance()->registerHandle("ShowDiamondBlueLackTips", &S2CCommonDataPacketHandles::onShowDiamondBlueLackTips);
	CommonDataDispatch::Instance()->registerHandle("SyncEntityPlayerInfo", &onSyncEntityPlayerInfo);
	CommonDataDispatch::Instance()->registerHandle("SyncEntityActorNpcInfo", &onSyncEntityActorNpcInfo);
}

void S2CCommonDataPacketHandles::handlePacket(std::shared_ptr<S2CPacketServerCommonData>& packet)
{
	CommonDataDispatch::Instance()->handleCommonData(GameClient::CGame::Instance()->getPlatformUserId(), packet->name, packet->key, packet->data);
}

void S2CCommonDataPacketHandles::onLuaCommonEvent(ui64 userId, String& name, String& key, String& data)
{
	if (SCRIPT_EVENT::ServerCommonDataEvent::invoke(key, data))
	{
		return;
	}
}

void S2CCommonDataPacketHandles::onDispatchInfo(ui64 userId, String & name, String & key, String & data)
{
	GameClient::CGame::Instance()->resetGame(data);
}

void S2CCommonDataPacketHandles::onGetAppShopData(ui64 userId, String& name, String& key, String& data)
{
	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::APP_SHOP, key, data);
}

void S2CCommonDataPacketHandles::onGetMainControlData(ui64 userId, String& name, String& key, String& data)
{
	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::MAIN, key, data);
}

void S2CCommonDataPacketHandles::onGetToolBarData(ui64 userId, String& name, String& key, String& data)
{
	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::TOOL_BAR, key, data);
}

void S2CCommonDataPacketHandles::onGetPlayerInfoData(ui64 userId, String& name, String& key, String& data)
{
	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PLAYER_INFO, key, data);
}

void S2CCommonDataPacketHandles::onOpenMovingEnderChest(ui64 userId, String& name, String& key, String& data)
{
	auto m_pPlayer = Blockman::Instance()->m_pPlayer;
	IInventory* pInventory = (IInventory*)m_pPlayer->getInventoryEnderChest();
	if (pInventory != NULL)
	{
		ChestActivateEvent::emit(pInventory, m_pPlayer->getPosition().getFloor(), 1, Vector3::ZERO);
	}
}

void S2CCommonDataPacketHandles::onGetVideoAdTipData(ui64 userId, String& name, String& key, String& data)
{
	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::VIDEO_AD_TIP, key, data);
}

void S2CCommonDataPacketHandles::onGameAnalyticsData(ui64 userId, String & name, String & key, String & data)
{
	GameAnalyticsSetting::Instance()->setGameAnalytics(data);
}

void S2CCommonDataPacketHandles::onGetShopData(ui64 userId, String& name, String& key, String& data)
{
	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::SHOP, key, data);
}

void S2CCommonDataPacketHandles::onGetBedWarData(ui64 userId, String& name, String& key, String& data)
{

}

void S2CCommonDataPacketHandles::onGetTycoonData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "VideoAd")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::TYCOON_VEDIO_AD, key, data);
		return;
	}
}

void S2CCommonDataPacketHandles::onGetBedWarHallData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "HallInfo" || key == "CurrentSeasonInfo")
	{
		onLuaCommonEvent(userId, name, key, data);
	}
	if (key == "ShowSeasonRank" || key == "CurrentSeasonInfo")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::BEDWAR_SEASON_RANK, key, data);
		return;
	}
	if (key == "LastSeasonInfo")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::BEDWAR_SEASON_REWARD, key, data);
		return;
	}
	if (key == "RuneData" || key == "RuneNextTrough")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::BEDWAR_RUNE, key, data);
		return;
	}
}

void S2CCommonDataPacketHandles::onGetPixelGunHallData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "DailyTaskData" || key == "MainLineTaskData" || key == "TaskActiveValue" || key == "TaskActiveTime" ||
		key == "TaskActiveStatus")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::TASK_SYSTEM, key, data);
		return;
	}
	if (key == "HallInfo")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUN_INDIE_MAIN, key, data);
		return;
	}
	if (key == "RewardResult")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::CHEST_REWARD_RESULT, key, data);
		return;
	}
	if (key == "RewardResults")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::CHEST_REWARD_RESULT, key, data);
		return;
	}
	if (key == "ShowGunStore" || key == "StoreGunData" || key == "StorePropData" || key == "StoreModuleData" ||
		key == "StoreTicketData")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::GUN_STORE, key, data);
		return;
	}
	if (key == "ShowChestLottery" || key == "ChestLotteryData")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::CHEST_LOTTERY, key, data);
		return;
	}
	if (key == "ShowSelectMode" || key == "SelectModeData")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUNHALL_MODE_SELECT, key, data);
		return;
	}
	if (key == "ShowArmorUpgrade" || key == "ArmorUpgradeData")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE, key, data);
		return;
	}
	if (key == "ShowSeasonRank" || key == "CurrentSeasonInfo")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::SEASON_RANK, key, data);
		return;
	}
	if (key == "LastSeasonInfo")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::SEASON_REWARD, key, data);
		return;
	}
	if (key == "SignInData")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUN_SIGN_IN, key, data);
		return;
	}
	if (key == "ShowLevelUp")
	{
		RootGuiLayout::Instance()->showLevelUpPanel(true);
		return;
	}
	if (key == "GiftBagAd" || key == "GiftBagData" || key == "GiftBagRefreshTime")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUN_GIFT_BAG_STORE, key, data);
		return;
	}
}

void S2CCommonDataPacketHandles::onGetPixelGunGameData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "Show1v1" || key == "1v1Data")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUN_1V1, key, data);
		return;
	}
	if (key == "ShowRevive" || key == "ReviveData")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUN_REVIVE, key, data);
		return;
	}
	if (key == "ShowGameResult" || key == "GameResultExtra" || key == "1v1ResultData" || key == "PersonResultData" ||
		key == "TeamResultData")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUN_RESULT, key, data);
		return;
	}
	if (key == "LevelUpData")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUN_GAME_LVUP, key, data);
		return;
	}
	if (key == "ChangeWeapon" || key == "ChickenGameData" || key == "ChickenWarningSetting" || key == "HideChickenWarning" ||
		key == "ShowChickenWarning" || key == "ShowPoisonMask" || key == "WeaponInfo")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_CHICKEN_MAIN, key, data);
		return;
	}
	if (key == "ChickenResultData" || key == "ShowChickenResult")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_CHICKEN_AWARD, key, data);
		return;
	}
}

void S2CCommonDataPacketHandles::onBlockCityStorageOperation(ui64 userId, String& name, String& key, String& data)
{
	auto build = CommonDataBuilder().fromData(data);
	i32 itemId = build.getIntParam("itemId");
	i32 meta = build.getIntParam("meta");
	i32 hotbarIndex = build.getIntParam("hotbarIndex");
	bool isEditMode = build.getBoolParam("isEditMode");

	if (Blockman::Instance()->m_pPlayer)
	{
		BlockCityStorageOperationEvent::emit(hotbarIndex, itemId, meta, isEditMode);
	}
}

void S2CCommonDataPacketHandles::onPlayingPraiseAction(ui64 userId, String& name, String& key, String& data)
{
	auto build = CommonDataBuilder().fromData(data);
	auto entityId = build.getIntParam("entityId");
	auto duration = build.getIntParam("duration");

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto player = dynamic_cast<EntityPlayer*>(dataCache->getEntityByServerId(entityId));
	if (!player)
		return;
	player->m_needUpdateUpperAction = true;
	player->setInsertUpperAction(ActionState::AS_PRAISE_ACTION, duration);
}

void S2CCommonDataPacketHandles::onGetSkyBlockMiningAreaData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "NotifySellItemFail")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::BUY_GOODS_TIP, key, data);
		return;
	}
}

void S2CCommonDataPacketHandles::onGetSkyBlockProductData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "UnlockTip")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::UNLOCK_PRODUCT_INFO, key, data);
		return;
	}

	if (key == "UpdateProduct")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::UPDATE_PRODUCT_INFO, key, data);
		return;
	}

	if (key == "SkyBlockProductProcess")
	{
		auto builder = CommonDataBuilder().fromData(data);
		i32 curProcess = builder.getIntParam("curProcess");
		i32 maxProcess = builder.getIntParam("maxProcess");
		int entityId = builder.getIntParam("entityId");

		auto dataCache = ClientNetwork::Instance()->getDataCache();
		auto npc = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(entityId));

		if (npc)
		{
			npc->setCurProcess(curProcess);
			npc->setMaxProcess(maxProcess);
		}
		return;
	}

	if (key == "GuideArrow")
	{
		auto builder = CommonDataBuilder().fromData(data);
		bool isShow = builder.getBoolParam("isShow");
		auto posX = builder.getIntParam("posX");
		auto posY = builder.getIntParam("posY");
		auto posZ = builder.getIntParam("posZ");
		if (isShow)
		{
			Blockman::Instance()->m_pPlayer->addGuideArrow(Vector3(posX, posY, posZ));
		}
		else
		{
			Blockman::Instance()->m_pPlayer->deleteGuideArrow(Vector3(posX, posY, posZ));
		}
	}
}

void S2CCommonDataPacketHandles::onSyncBlockCityCDRewardData(ui64 userId, String& name, String& key, String& data)
{
	auto build = CommonDataBuilder().fromData(data);
	i32 cdTime = build.getIntParam("CDTime");
	String cdImg = build.getStringParam("CDImg");

	if (Blockman::Instance()->m_pPlayer)
	{
		BlockCitySyncCDRewardEvent::emit(cdTime, cdImg);
	}
}

void S2CCommonDataPacketHandles::onGetBlockCityData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "TigerAd1" || key == "TigerAd2")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::BLOCK_CITY_TIP, key, data);
		return;
	}

	if (key == "FlyingAd")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::BLOCK_CITY_BUY_FLY, key, data);
		return;
	}
}

void S2CCommonDataPacketHandles::onGetSkyBlockMainData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "SignInData")
	{
		onLuaCommonEvent(userId, name, key, data);
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::SKYBLOCK_SIGN_IN, key, data);
		return;
	}
	if (key == "SignInRewardResult")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::SKYBLOCK_REWARD_RESULT, key, data);
		return;
	}
	if (key == "SkyBlockLoading")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::MAIN, key, data);
		return;
	}

	if (key == "ReviveNewPri")
	{
		onLuaCommonEvent(userId, name, key, data);
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::SKYBLOCK_REVIVE_NEW_PRI, key, data);
		return;
	}

	if (key == "SupemePrivilegeImg")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::SKYBLOCK_PRIVILEGE_SHOP, key, data);
		return;
	}

	if (key == "ShowLackDiamondPanel")
	{
		GameClient::CGame::Instance()->getShellInterface()->onRecharge((int)RechargeType::RECHARGE_TYPE_CUBE);
		return;
	}
}

void S2CCommonDataPacketHandles::onGetBirdSimulatorData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "moneyAd" || key == "eggTicketAd" || key == "chestCDAd" || key == "vipChestAd")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::BIRD_TIP, key, data);
		return;
	}

	if (key == "feedAd")
	{

		auto builder = CommonDataBuilder().fromData(data);
		bool isShow = builder.getBoolParam("isShow");
		i32 feedAdReward = builder.getIntParam("feedAdReward");

		if (Blockman::Instance()->m_pPlayer)
		{
			SendBirdShowFeedAdEvent::emit(isShow, feedAdReward);
		}
		return;
	}
}

void S2CCommonDataPacketHandles::onGetShellInterfaceDataReport(ui64 userId, String& name, String& key, String& data)
{
	auto build = CommonDataBuilder().fromData(data);
	auto mainEvent = build.getStringParam("mainEvent");
	auto childEvent = build.getStringParam("childEvent");
	GameClient::CGame::Instance()->getShellInterface()->dataReport(mainEvent, childEvent);
}

void S2CCommonDataPacketHandles::onGetBlockCityTigerAdShowUI(ui64 userId, String& name, String& key, String& data)
{
	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::BLOCK_CITY_TIGER_LOTTERY, key, data);
}

//void S2CCommonDataPacketHandles::onShowDiamondBlueLackTips(ui64 userId, String& name, String& key, String& data)
//{
//	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::TIP_DIALOG, key, data);
//}

void S2CCommonDataPacketHandles::onDoHurtMsg(ui64 userId, String& name, String& key, String& data)
{
	NULL_RETURN_VOID(Blockman::Instance());
	NULL_RETURN_VOID(Blockman::Instance()->m_pPlayer);
	Blockman::Instance()->m_pPlayer->doHurt(0, false);
}

void S2CCommonDataPacketHandles::onSyncChestInfoMsg(ui64 userId, String& name, String& key, String& data)
{
    //TODO
    auto build = CommonDataBuilder().fromData(data);
    auto posX = build.getIntParam("posX");
    auto posY = build.getIntParam("posY");
    auto posZ = build.getIntParam("posZ");
    auto headName = build.getStringParam("headName");
    auto action = build.getBoolParam("action");

    auto chestInfo = PlayerPrefs::Instance()->GetProperty<ChestInfoMap>("ChestInfo");
    if (action)
    {
        ChestInfo info;
        info.pos = BlockPos{ posX, posY, posZ };
        info.headName = headName;
        chestInfo[StringUtil::ToString(info.pos).c_str()] = info;
    }
    else
    {
        auto it = chestInfo.find(StringUtil::ToString(BlockPos{ posX, posY, posZ }).c_str());
        if (it != chestInfo.end())
            chestInfo.erase(it);
    }
    PlayerPrefs::Instance()->SetProperty<ChestInfoMap>("ChestInfo", chestInfo);
}

void S2CCommonDataPacketHandles::onDoAttackMsg(ui64 userId, String& name, String& key, String& data)
{
	auto build = CommonDataBuilder().fromData(data);
	ui64 otherUserId = build.getIntParam("userId");
	if (key == "attack")
	{
		WalkingDeadAttackedPlayerEvent::emit(otherUserId);
	}
	else if (key == "attacked")
	{
		WalkingDeadAttackPlayerEvent::emit(otherUserId);
	}
}

void S2CCommonDataPacketHandles::onGetTipDialogData(ui64 userId, String& name, String& key, String& data)
{
	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::TIP_DIALOG, key, data);
}

void S2CCommonDataPacketHandles::onSyncDiePlayerMsg(ui64 userId, String& name, String& key, String& data)
{
	auto build = CommonDataBuilder().fromData(data);
	ui64 otherUserId = build.getIntParam("userId");
	float x = build.getFloatParam("x");
	float y = build.getFloatParam("y");
	float z = build.getFloatParam("z");
	WalkingDeadDiePlayerEvent::emit(otherUserId, x, y, z);
}

void S2CCommonDataPacketHandles::onBlockCityToGetOnCarOrNotRace(ui64 userId, String& name, String& key, String& data)
{
	auto pPlayer = Blockman::Instance()->m_pPlayer;
	auto build = CommonDataBuilder().fromData(data);
	auto vehicleId = build.getIntParam("vehicle_entityId");
	auto isAgree = build.getBoolParam("isAgree");
	auto targetUId = build.getI64Param("competitorUId");
	auto targetName = build.getStringParam("name");

	if (isAgree)
	{
		auto vehicle = dynamic_cast<EntityVehicle*>(ClientNetwork::Instance()->getDataCache()->getEntityByServerId(vehicleId));
		if (!vehicle || !pPlayer)
		{
			return;
		}
		setRaceVehileCountdownEvent::emit(true);
		pPlayer->askGetOnVehicle(vehicle, false);
	}
	else if (!targetName.empty())
	{
		String msg = StringUtil::Format(LanguageManager::Instance()->getString("gui_blockcity_disagree_race").c_str(), targetName.c_str()).c_str();
		CenterSystemNotificationEvent::emit(msg, 3);
		BlockCityRaceBtnEvent::emit(targetUId);
	}
}

void S2CCommonDataPacketHandles::onBlockCityRaceVehicleResulr(ui64 userId, String name, String key, String data)
{
	auto build = CommonDataBuilder().fromData(data);
	bool isWin = build.getBoolParam("isWin");
	setRaceVehileResultEvent::emit(isWin);
}



void S2CCommonDataPacketHandles::onGetPixelAdData(ui64 userId, String& name, String& key, String& data)
{
	RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::PIXEL_GUN_INDIE_MAIN, key, data);
}

void S2CCommonDataPacketHandles::onGetWalkingDeadGameData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "ShowStoreId")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::WALKINGDEAD_STORE, key, data);
		return;
	}
	if (key == "UpdateGoods")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::WALKINGDEAD_STORE, key, data);
		return;
	}
	if (key == "SupplyData")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::WALKINGDEAD_STORE, key, data);
		return;
	}
	if (key == "ShowVipInfo")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::WALKINGDEAD_STORE, key, data);
		return;
	}
	if (key == "DeathSettlement")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::WALKINGDEAD_DEATHSETTLEMENT, key, data);
		return;
	}
	if (key == "DeathItemsDetail")
	{
		ClassificationSetting::setDetails(data);
		return;
	}
	if (key == "DesignationDetail")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::WALKINGDEAD_DESIGNATIONPANEL, key, data);
		return;
	}
	if (key == "ShowStoreByTraveller")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::WALKINGDEAD_STORE, key, data);
		return;
	}
}

void S2CCommonDataPacketHandles::onShowBlockCityRaceFailMsg(ui64 uiserId, String& name, String& key, String& data)
{
	//showType = 1,	--û�п����ܵ�
	//		   = 2,	--�Է��ڵȴ�����
	//		   = 3, --��Ҫ�ȴ���������ҵĻظ�
	//		   = 4, --��Ҫ�ظ�������

	auto build = CommonDataBuilder().fromData(data);
	auto showType = build.getIntParam("showType");
	auto targetName = build.getStringParam("name");

	switch (showType)
	{
	case 1:
	case 2:
	{
		String str = showType == 1 ? "gui_blockcity_notFreeLines" : "gui_blockcity_targetWaitAns";
		CenterSystemNotificationEvent::emit(LanguageManager::Instance()->getString(str).c_str(), 3);
	}
		break;
	case 3:
	case 4:
	{
		if (!targetName.empty())
		{
			String str = showType == 3 ? "gui_blockcity_waitTargetAns" : "gui_blockcity_ansInvitor";
			String msg = StringUtil::Format(LanguageManager::Instance()->getString(str).c_str(), targetName.c_str()).c_str();
			CenterSystemNotificationEvent::emit(msg, 3);
		}
	}
		break;
	default:
		break;
	}
}

void S2CCommonDataPacketHandles::onGetBlockData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "BlockElasticity")
	{
		auto build = CommonDataBuilder().fromData(data);
		auto id = build.getIntParam("id");
		auto elasticity = build.getFloatParam("elasticity");
		BLOCKMAN::LogicSetting::Instance()->setBlockElasticity(id, elasticity);
	}
}

void S2CCommonDataPacketHandles::onGetSkyWarLuckyBlockData(ui64 userId, String& name, String& key, String& data)
{
	if (key == "LuckyBlockGeneralDeathTip")
	{
		RootGuiLayout::Instance()->onDispatchCommonData(RootGuiLayout::LayoutId::KILL_PANEL, key, data);
		return;
	}
}