#include "S2CInitPacketHandles.h"
#include "Network/ClientNetwork.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/EntityOtherPlayerMP.h"
#include "cWorld/Blockman.h"
#include "game.h"

#include "Entity/Entity.h"
#include "Entity/EntityPlayer.h"
#include "World/World.h"

#include "Network/DataCache.h"
#include "Network/ClientNetwork.h"

#include "Util/ClientEvents.h"
#include "Util/ClientDataReport.h"

#include "Inventory/CraftingManager.h"
#include "Inventory/CoinManager.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/InventoryEnderChest.h"

#include "Setting/LogicSetting.h"
#include "Setting/AppItemsSetting.h"
#include "Setting/SceneSetting.h"
#include "World/GameSettings.h"
#include "cWorld/Blockman.h"
#include "cWorld/WorldClient.h"
#include "Render/RenderGlobal.h"
#include "Blockman/GUI/RootGuiLayout.h"
#include "Plugins/GameClientEvents.h"


void S2CInitPacketHandles::handlePacket(std::shared_ptr<S2CPacketGameInfo>& packet)
{
	LordLogInfo("receive game info packet");

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	dataCache->setSelfServerId(packet->m_playerRuntimeId);

	auto bm = BLOCKMAN::Blockman::Instance();
	auto world = dynamic_cast<BLOCKMAN::WorldClient*>(bm->m_pWorld);
	world->setTimeStopped(packet->isTimeStopped);
	world->setSpawnLocation(packet->spawnPos);
	bm->generateWorld(packet->sex, packet->m_defaultIdle);
	auto craftingManager = BLOCKMAN::CraftingManager::Instance();
	for (const auto& recipeName : packet->recipes)
	{
		if (!craftingManager->loadRecipe(recipeName))
		{
			LordLogError("recipe %s is not usable", recipeName.c_str());
		}
	}
	dataCache->setBlockmanWorld(bm, world);

	GameClient::CGame::Instance()->getNetwork()->setStatus(ClientNetworkCore::NetStatus::emLoginSuc);
	GameClient::CGame::Instance()->setGameType(packet->m_gameType);
	GameClient::CGame::Instance()->setGetServerInfo(true);

	BLOCKMAN::CoinManager::Instance()->clearCoinMapping();
	for (size_t i = 0; i < std::size(packet->m_coinMapping); ++i)
	{
		BLOCKMAN::CoinManager::Instance()->addCoinMapping(packet->m_coinMapping[i].coinId, packet->m_coinMapping[i].itemId);
	}

	if (bm->m_pPlayer)
	{
		bm->m_pPlayer->m_vip = packet->vip;
		bm->m_pPlayer->m_actionLockedInfoMap.clear();
		for (int i = 0; i < packet->m_selectableActions.size(); ++i)
		{
			bm->m_pPlayer->m_actionLockedInfoMap[packet->m_selectableActions[i]] = false;
		}
	}
}

void S2CInitPacketHandles::handlePacket(std::shared_ptr<S2CPacketCheckCSVersionResult>& packet)
{
	GameClient::CGame::Instance()->setSendLoginTime(0);
	if (packet->m_success)
	{
		BLOCKMAN::CheckCSVersionSuccEvent::emit();
		ClientNetwork::Instance()->getSender()->sendLogin();
		LordLogInfo("recv S2CPacketCheckCSVersionResult, check succ...");
	}
	else
	{
		BLOCKMAN::CheckCSVersionFailEvent::emit();
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_ENGINE_VERSION_VALIDATION_FAILS);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_ENGINE_VERSION_VALIDATION_FAILS);
		LordLogError("recv S2CPacketCheckCSVersionResult, check version fail, server version = %d", packet->m_serverVersion);
	}
}

void S2CInitPacketHandles::handlePacket(std::shared_ptr<S2CPacketLoginResult>& packet)
{
	GameClient::CGame::Instance()->setSendLoginTime(0);
	if (packet->m_resultCode == (int32_t)NETWORK_DEFINE::PacketLoginResult::emSucc) {
		BLOCKMAN::LoginSuccEvent::emit();
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::ENTER_GAME_SUCCESS);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS , BLOCKMAN::ClientDataReport::ERROR_CODE_ENTER_GAME_SUCCESS);
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_ENTER_GAME_SUCCESS);
		GameClient::CGame::Instance()->recordRecvedLoginResultTime();
		GameClient::CGame::Instance()->reportEnterGameTime();
		LordLogInfo("recv S2CPacketLoginResult, login succ");
	}
	else if (packet->m_resultCode == (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrTokenWrong)
	{
		BLOCKMAN::LoginTokenErrorEvent::emit();
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_VALIDATION_FAILS);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_VALIDATION_FAILS);
		LordLogError("recv S2CPacketLoginResult, login fail, result code=%d", packet->m_resultCode);
	}
	else if (packet->m_resultCode == (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrBeKicked) {
		BLOCKMAN::BeKickedEvent::emit();
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_BE_KICKED);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_BE_KICKED);
		LordLogError("recv S2CPacketLoginResult, login fail, result code=%d", packet->m_resultCode);
	}
	else if (packet->m_resultCode == (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrGetUserAttrFail) {
		BLOCKMAN::LoginGetUserAttrFailEvent::emit();
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_ENGINE_GET_USER_ATTR_FAIL);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_ENGINE_GET_USER_ATTR_FAIL);
		LordLogError("recv S2CPacketLoginResult, login fail, result code=%d", packet->m_resultCode);
	}
	else if (packet->m_resultCode == (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrWaitRoleAttrFail) {
		BLOCKMAN::LoginWaitRoleAttrFailEvent::emit();
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_ENGINE_WAIT_ROLE_ATTR_FAIL);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_ENGINE_WAIT_ROLE_ATTR_FAIL);
		LordLogError("recv S2CPacketLoginResult, login fail, result code=%d", packet->m_resultCode);
	}
	else if (packet->m_resultCode == (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrServerQuitting) {
		BLOCKMAN::LoginServerQuittingEvent::emit();
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_SERVER_QUITTING);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_SERVER_QUITTING);
		LordLogError("recv S2CPacketLoginResult, login fail, result code=%d", packet->m_resultCode);
	}
	else {
		// should not go here
		BLOCKMAN::LoginFailureEvent::emit();
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_UNKNOWN);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_UNKNOWN);
		LordLogError("recv S2CPacketLoginResult, login fail, unknown result code=%d", packet->m_resultCode);
	}
}

void S2CInitPacketHandles::handlePacket(std::shared_ptr<S2CPacketDBDataReady>& packet)
{
	LordLogInfo("---------S2CPacketDBDataReady---------");
	GameClient::CGame::Instance()->setDbDataReady();
	BLOCKMAN::Blockman::Instance()->onGameReady();
	SCRIPT_EVENT::DBDataReadyEvent::invoke(true);
}

void S2CInitPacketHandles::handlePacket(std::shared_ptr<S2CPacketEnterOtherGame>& packet)
{
	GameClient::CGame::Instance()->resetGame(packet->gameType, packet->targetId, packet->mapId);
}

void S2CInitPacketHandles::handlePacket(std::shared_ptr<S2CPacketAppExpResult>& packet)
{
	GameClient::CGame::Instance()->onExpResult(packet->curLv, packet->toLv, packet->addExp, packet->curExp, packet->toExp, packet->upExp, packet->status);
}

void S2CInitPacketHandles::handlePacket(std::shared_ptr<S2CPacketLogicSettingInfo>& packet)
{
	LordLogInfo("S2CPacketLogicSettingInfo");
	BLOCKMAN::LogicSetting::Instance()->setMaxInventorySize(packet->m_mainInventorySize);
	BLOCKMAN::LogicSetting::Instance()->setMaxEnderInventorySize(packet->m_enderInventorySize);
	BLOCKMAN::LogicSetting::Instance()->setCanEatWhenever(packet->m_canEatSetting);
	BLOCKMAN::LogicSetting::Instance()->setGunPluginSetting(packet->m_gunSetting);
	BLOCKMAN::LogicSetting::Instance()->setBulletClipPluginSetting(packet->m_bulletClipSetting);
	BLOCKMAN::LogicSetting::Instance()->setBlockDynamicAttr(packet->m_blockDynamicAttr);
	BLOCKMAN::LogicSetting::Instance()->setToolItemDynamicAttr(packet->m_toolItemDynamicAttr);
	BLOCKMAN::LogicSetting::Instance()->setSneakShowName(packet->m_sneakShowName);
	BLOCKMAN::LogicSetting::Instance()->setBreakBlockSoon(packet->m_breakBlockSoon);
	BLOCKMAN::LogicSetting::Instance()->setCanDamageItem(packet->m_isCanDamageItem);
	BLOCKMAN::LogicSetting::Instance()->setDisableSelectEntity(packet->m_disableSelectEntity);
	BLOCKMAN::LogicSetting::Instance()->setGunIsNeedBulletStatus(packet->m_gunIsNeedBullet);
	BLOCKMAN::LogicSetting::Instance()->setSkillItemSetting(packet->m_skillItemSetting);
	BLOCKMAN::LogicSetting::Instance()->setHideClouds(packet->m_hideClouds);
	BLOCKMAN::LogicSetting::Instance()->setCanCloseChest(packet->m_canCloseChest);
	BLOCKMAN::LogicSetting::Instance()->setShowGunEffectWithSingleStatus(packet->m_showGunEffectWithSingle);
	BLOCKMAN::LogicSetting::Instance()->setAllowHeadshotStatus(packet->m_allowHeadshot);
	BLOCKMAN::LogicSetting::Instance()->setThirdPersonDistance(packet->m_thirdPersonDistance);
	BLOCKMAN::LogicSetting::Instance()->setQuicklyBuildBlockCost(packet->m_quicklyBuildBlockCost);
	BLOCKMAN::LogicSetting::Instance()->setQuicklyBuildBlockNum(packet->m_quicklyBuildBlockNum);
	BLOCKMAN::LogicSetting::Instance()->setSwordBreakBlock(packet->m_swordCanBreakBlock);
	BLOCKMAN::LogicSetting::Instance()->setToolDurable(packet->m_need_durable);
	BLOCKMAN::LogicSetting::Instance()->setRenderYawThreshold(packet->m_renderYawThreshold);
	BLOCKMAN::LogicSetting::Instance()->setRenderYawScale(packet->m_renderYawScale);
	BLOCKMAN::LogicSetting::Instance()->setDeathAnimEffect(packet->m_deathAnimEffect);
	BLOCKMAN::LogicSetting::Instance()->setUseMovingEnderChest(packet->m_isUseMovingEnderChest);
	BLOCKMAN::LogicSetting::Instance()->setHideArmor(packet->m_hideArmor);
	BLOCKMAN::LogicSetting::Instance()->setParachuteMoveSpeed(packet->m_parachuteMoveSpeed);
	BLOCKMAN::LogicSetting::Instance()->setParachuteDownSpeed(packet->m_parachuteDownSpeed);
	BLOCKMAN::LogicSetting::Instance()->setMaxChangeInChunk(packet->m_max_changes_in_on_chunk);
	BLOCKMAN::LogicSetting::Instance()->setSeparateCameraControl(packet->m_isSeparateCameraControl);
	BLOCKMAN::LogicSetting::Instance()->setUseLocalScaleInHand(packet->m_useLocalScaleInHand);
	BLOCKMAN::LogicSetting::Instance()->setGameWeather(packet->m_gameWeatherType);
	Blockman::Instance()->m_pPlayer->getInventoryEnderChest()->setSlotsCount(packet->m_enderInventorySize);
	RootGuiLayout::Instance()->showCameraDistanceSlider(packet->m_isSeparateCameraControl);
}

void S2CInitPacketHandles::handlePacket(std::shared_ptr<S2CPacketAppItemsSettingInfo>& packet)
{
	LordLogInfo("S2CPacketAppItemsSettingInfo");
	BLOCKMAN::AppItemsSetting::Instance()->setActionInfoList(packet->m_actionInfoList);
	UpdateSelectableActionEvent::emit();
}

void S2CInitPacketHandles::handlePacket(std::shared_ptr<S2CPacketSceneSettingInfo>& packet)
{
	LordLogInfo("S2CPacketSceneSettingInfo");
	BLOCKMAN::SceneSetting::Instance()->setSkyInfoList(packet->skyInfoList);
	BLOCKMAN::SceneSetting::Instance()->setSunAndMoonName(packet->sunName, packet->moonName);

	int skyType = (packet->skyType >= 0 && packet->skyType <= 2) ? packet->skyType : 1;
	int cloudType = (packet->cloudType >= 0 && packet->cloudType <= 2) ? packet->cloudType : 1;
	if (BLOCKMAN::SceneSetting::Instance()->changeSceneSwitch((SceneSetting::RenderSkyType)skyType, (SceneSetting::RenderCloudType)cloudType, packet->showSunAndMoon, packet->showMeteor, packet->showStarEffect))
	{
		BLOCKMAN::Blockman::Instance()->m_globalRender->applySceneSwitchChanges();
	}
}
