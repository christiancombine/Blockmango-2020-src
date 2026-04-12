#include "ServerPacketSender.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Blockman/Entity/EntityTracker.h"
#include "Script/GameServerEvents.h"
#include "Setting/LogicSetting.h"
#include "Network/protocol/S2CPackets4.h"
#include "Entity/EntityBird.h"
#include "Entity/EntityLandNpc.h"

void ServerPacketSender::syncEntityBird(ui64 rakssId, BLOCKMAN::EntityBird* entityBird)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityBird>();
	packet->entityId = entityBird->entityId;
	packet->userId = entityBird->m_userId;
	packet->birdId = entityBird->m_birdId;
	packet->nameLang = entityBird->getNameLang();
	packet->actorName = entityBird->getActorName();
	packet->actorBody = entityBird->getActorBody();
	packet->actorBodyId = entityBird->getActorBodyId();
	packet->dressGlasses = entityBird->getDressGlasses();
	packet->dressHat = entityBird->getDressHat();
	packet->dressBeak = entityBird->getDressBeak();
	packet->dressWing = entityBird->getDressWing();
	packet->dressTail = entityBird->getDressTail();
	packet->dressEffect = entityBird->getDressEffect();

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
	else
	{
		sendPacketToTrackingPlayers(entityBird->entityId, packet);
	}
}

void ServerPacketSender::syncRanch(BLOCKMAN::EntityPlayer* player, int syncType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p && p->m_ranch)
	{
		switch ((BLOCKMAN::SyncRanchType)syncType)
		{
		case SyncRanchType::HOUSE: {
			auto packet = LORD::make_shared<S2CPacketSyncRanchHouse>();
			packet->m_house = *p->m_ranch->getHouse();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
			break;
		}
		case SyncRanchType::INFO: {
			auto packet = LORD::make_shared<S2CPacketSyncRanchInfo>();
			packet->m_info = *p->m_ranch->getInfo();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
			break;
		}

		case SyncRanchType::STORAGE: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchStorage>();
			packet->m_storage = *p->m_ranch->getStorage();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
			break;
		}
		case SyncRanchType::ORDER: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchOrder>();
			packet->m_orders = p->m_ranch->getOrders();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
			break;
		}
		case SyncRanchType::ACHIEVEMENT: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchAchievement>();
			packet->m_achievements = p->m_ranch->getAchievements();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
			break;
		}
		case SyncRanchType::TIME_PRICE: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchTimePrice>();
			packet->m_timePrices = p->m_ranch->getTimePrices();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
			break;
		}
		case SyncRanchType::SHORTCUT_FREE_TIMES: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchShortcutFreeTimes>();
			const auto& items = p->m_ranch->getShortcutFreeTimes();
			for (auto item : items)
			{
				vector<i32>::type newItems = vector<i32>::type();
				newItems.push_back(item.first);
				newItems.push_back(item.second);
				packet->m_shortcutFreeTimes.push_back(newItems);
			}

			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
			break;
		}
		}
	}
}

void ServerPacketSender::syncEntityLandNpc(ui64 rakssId, BLOCKMAN::EntityLandNpc* landNpc)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityLandNpc>();
	packet->entityId = landNpc->entityId;
	packet->name = landNpc->getName();
	packet->nameLang = landNpc->getNameLang();
	packet->actorName = landNpc->getActorName();
	packet->actorBody = landNpc->getActorBody();
	packet->actorBodyId = landNpc->getActorBodyId();
	packet->status = landNpc->getStatus();
	packet->timeLeft = landNpc->getTimeLeft();
	packet->recipe = landNpc->getRecipe();
	packet->reward = landNpc->getReward();
	packet->userId = landNpc->getUserId();
	packet->landCode = landNpc->getLandCode();
	packet->price = landNpc->getPrice();
	packet->totalTime = landNpc->getTotalTime();

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
	else
	{
		sendPacketToTrackingPlayers(landNpc->entityId, packet);
	}
}

void ServerPacketSender::sendSuperShopTip(ui64 rakssId, String tip)
{
	auto packet = LORD::make_shared<S2CPacketSuperShopTip>();
	packet->tip = tip;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendEnterOtherGame(ui64 rakssId, String gameType, ui64 targetId, String mapId)
{
	auto packet = LORD::make_shared<S2CPacketEnterOtherGame>();
	packet->gameType = gameType;
	packet->targetId = targetId;
	packet->mapId = mapId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendKillMsg(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketKillMsg>();
	packet->data = data;
	if (rakssId == 0)
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendShowPersonalShop(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketShowPersonalShop>();
	if (rakssId == 0)
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSetPersonalShopArea(ui64 rakssId, Vector3 startPos, Vector3 endPos)
{
	auto packet = LORD::make_shared<S2CPacketPersonalShopArea>();
	packet->startPos = startPos;
	packet->endPos = endPos;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendAddGunBulletNum(ui64 rakssId, int gunId, int bulletNum)
{
	auto packet = LORD::make_shared<S2CPacketAddGunBulletNum>();
	packet->gunId = gunId;
	packet->bulletNum = bulletNum;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSubGunRecoil(ui64 rakssId, int gunId, float recoil)
{
	auto packet = LORD::make_shared<S2CPacketSubGunRecoil>();
	packet->gunId = gunId;
	packet->recoil = recoil;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCustomPropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketCustomPropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::updateCustomProps(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketUpdateCustomProps>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncRanchRank(BLOCKMAN::EntityPlayer* player, int rankType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p && p->m_ranch)
	{
		auto packet = LORD::make_shared<S2CPacketSyncRanchRank>();
		packet->m_rankType = rankType;
		auto ranks = p->m_ranch->getRankByType(rankType);
		for (auto rank : ranks)
		{
			packet->m_ranks.push_back(rank);
		}
		Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
	}
}

void ServerPacketSender::syncRanchBuild(BLOCKMAN::EntityPlayer* player, int buildType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p && p->m_ranch)
	{
		auto packet = LORD::make_shared<S2CPacketSyncRanchBuild>();
		packet->m_buildType = buildType;
		auto builds = p->m_ranch->getBuildByType(buildType);
		for (auto build : builds)
		{
			packet->m_builds.push_back(build);
		}
		Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
	}
}

void ServerPacketSender::syncEntityBuildNpc(ui64 rakssId, BLOCKMAN::EntityBuildNpc* buildNpc)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityBuildNpc>();
	packet->entityId = buildNpc->entityId;
	packet->m_actorId = buildNpc->m_actorId;
	packet->m_userId = buildNpc->m_userId;
	packet->m_maxQueueNum = buildNpc->m_maxQueueNum;
	packet->m_productCapacity = buildNpc->m_productCapacity;
	packet->m_queueUnlockPrice = buildNpc->m_queueUnlockPrice;
	packet->m_queueUnlockCurrencyType = buildNpc->m_queueUnlockCurrencyType;

	for (auto product : buildNpc->m_products)
	{
		packet->m_products.push_back(product);
	}

	for (auto queue : buildNpc->m_unlockQueues)
	{
		packet->m_unlockQueues.push_back(queue);
	}

	for (auto recipe : buildNpc->m_recipes)
	{
		packet->m_recipes.push_back(recipe);
	}

	if (rakssId == 0)
	{
		sendPacketToTrackingPlayers(buildNpc->entityId, packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendRanchGain(ui64 rakssId, vector<BLOCKMAN::RanchCommon>::type items)
{
	auto packet = LORD::make_shared<S2CPacketRanchGain>();
	packet->items = items;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendBroadcastMessage(ui64 rakssId, i32 type, const String& content)
{
	auto packet = LORD::make_shared<S2CPacketBroadcastMessage>();
	packet->type = type;
	packet->content = content;
	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendRanchUnlockItem(ui64 rakssId, vector<i32>::type items)
{
	auto packet = LORD::make_shared<S2CPacketRanchUnlockItem>();
	packet->items = items;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPlayerMovementWithMotion(int playerEntityId, bool includeSelf)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketEntityPlayerTeleportWithMotion>(pPlayer);

	sendPacketToTrackingPlayers(playerEntityId, packet, includeSelf);
}

void ServerPacketSender::sendSyncCannonFireToTrackingPlayers(int playerEntityId, int cannonEntityId)
{
	auto packet = LORD::make_shared<S2CPacketSyncCannonFire>();
	packet->cannonEntityId = cannonEntityId;
	sendPacketToTrackingPlayers(playerEntityId, packet);
}

void ServerPacketSender::sendCropInfo(i64 userId, Vector3i blockPos, i32 blockId, i32 stage, i32 curStageTime, i32 residueHarvestNum)
{
	auto pree = Server::Instance()->getNetwork()->findPlayerByPlatformUserId(userId);
	if (pree)
	{
		auto packet = LORD::make_shared<S2CPacketCropInfo>();
		packet->blockPos = blockPos;
		packet->blockId = blockId;
		packet->stage = stage;
		packet->curStageTime = curStageTime;
		packet->residueHarvestNum = residueHarvestNum;
		Server::Instance()->getNetwork()->sendPacket(packet, pree->getRakssid());
	}

}

void ServerPacketSender::sendAppExpResult(i64 userId, int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status)
{
	auto pree = Server::Instance()->getNetwork()->findPlayerByPlatformUserId(userId);
	if (pree)
	{
		auto packet = LORD::make_shared<S2CPacketAppExpResult>();
		packet->curLv = curLv;
		packet->toLv = toLv;
		packet->addExp = addExp;
		packet->curExp = curExp;
		packet->toExp = toExp;
		packet->upExp = upExp;
		packet->status = status;
		Server::Instance()->getNetwork()->sendPacket(packet, pree->getRakssid());
	}
}

void ServerPacketSender::sendOccupationUnlock(ui64 rakssId, String occupationInfo)
{
	auto packet = LORD::make_shared<S2CPacketOccupationUnlock>();
	packet->occupationInfo = occupationInfo;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendBuyFlying(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketBuyFlying>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayCloseup(ui64 rakssId, const Vector3& playerPos, const Vector3& closeupPos, float farDistance, float nearDistance, float velocity, float duration, float yaw, float pitch)
{
	auto packet = LORD::make_shared<S2CPacketPlayCloseup>();
	packet->playerPos = playerPos;
	packet->closeupPos = closeupPos;
	packet->farDistance = farDistance;
	packet->nearDistance = nearDistance;
	packet->velocity = velocity;
	packet->duration = duration;
	packet->yaw = yaw;
	packet->pitch = pitch;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendKeepItemTip(i64 rakssId, int coinType, int coin, int tipTime)
{
	auto packet = LORD::make_shared<S2CPacketShowBuyKeepItemTip>();
	packet->coinType = coinType;
	packet->coin = coin;
	packet->tipTime = tipTime;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendOpenEnchantment(i64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketSendOpenEnchantment>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendLogicSettingInfo(ui64 rakssid)
{
	auto packet = LORD::make_shared<S2CPacketLogicSettingInfo>();
	packet->m_mainInventorySize = LogicSetting::Instance()->getMaxInventorySize();
	packet->m_enderInventorySize = LogicSetting::Instance()->getMaxEnderInventorySize();
	packet->m_sneakShowName = LogicSetting::Instance()->isSneakShowName();
	packet->m_breakBlockSoon = LogicSetting::Instance()->isBreakBlockSoon();
	packet->m_isCanDamageItem = LogicSetting::Instance()->isCanDamageItem();
	packet->m_gunSetting = LogicSetting::Instance()->getGunPluginSetting();
	packet->m_bulletClipSetting = LogicSetting::Instance()->getBulletClipPluginSetting();
	packet->m_blockDynamicAttr = LogicSetting::Instance()->getBlockDynamicAttr();
	packet->m_toolItemDynamicAttr = LogicSetting::Instance()->getToolItemDynamicAttr();
	packet->m_disableSelectEntity = LogicSetting::Instance()->isDisableSelectEntity();
	packet->m_canEatSetting = LogicSetting::Instance()->isCanEatWhenever();
	packet->m_gunIsNeedBullet = LogicSetting::Instance()->getGunIsNeedBulletStatus();
	packet->m_skillItemSetting = LogicSetting::Instance()->getSkillItemSetting();
	packet->m_hideClouds = LogicSetting::Instance()->isHideClouds();
	packet->m_canCloseChest = LogicSetting::Instance()->isCanCloseChest();
	packet->m_showGunEffectWithSingle = LogicSetting::Instance()->getShowGunEffectWithSingleStatus();
	packet->m_allowHeadshot = LogicSetting::Instance()->getAllowHeadshotStatus();
	packet->m_thirdPersonDistance = LogicSetting::Instance()->getThirdPersonDistance();
	packet->m_quicklyBuildBlockCost = LogicSetting::Instance()->getQuicklyBuildBlockCost();
	packet->m_quicklyBuildBlockNum = LogicSetting::Instance()->getQuicklyBuildBlockNum();
	packet->m_swordCanBreakBlock = LogicSetting::Instance()->getSwordBreakBlock();
	packet->m_need_durable = LogicSetting::Instance()->getToolDurable();
	packet->m_renderYawThreshold = LogicSetting::Instance()->getRenderYawThreshold();
	packet->m_renderYawScale = LogicSetting::Instance()->getRenderYawScale();
	packet->m_deathAnimEffect = LogicSetting::Instance()->getDeathAnimEffect();
	packet->m_isUseMovingEnderChest = LogicSetting::Instance()->isUseMovingEnderChest();
	packet->m_hideArmor = LogicSetting::Instance()->getHideArmor();
	packet->m_parachuteDownSpeed = LogicSetting::Instance()->getParachuteDownSpeed();
	packet->m_parachuteMoveSpeed = LogicSetting::Instance()->getParachuteMoveSpeed();
	packet->m_max_changes_in_on_chunk = LogicSetting::Instance()->getMaxChangeInChunk();
	packet->m_isSeparateCameraControl = LogicSetting::Instance()->isSeparateCameraControl();
	packet->m_useLocalScaleInHand = LogicSetting::Instance()->getUseLocalScaleInHand();
	packet->m_gameWeatherType = LogicSetting::Instance()->getGameWeather();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendSubGunFireCd(ui64 rakssId, int gunId, float fireCd)
{
	auto packet = LORD::make_shared<S2CPacketSubGunFireCd>();
	packet->gunId = gunId;
	packet->fireCd = fireCd;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncEntityHurtTime(ui64 rakssId, int entityId, float time)
{
	auto packet = LORD::make_shared<S2CPacketEntityHurtTime>();
	packet->entityId = entityId;
	packet->time = time;
	if (rakssId == 0)
		sendPacketToTrackingPlayers(entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendLotteryData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketLotteryData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendLotteryResult(ui64 rakssId, String firstId, String secondId, String thirdId)
{
	auto packet = LORD::make_shared<S2CPacketLotteryResult>();
	packet->firstId = firstId;
	packet->secondId = secondId;
	packet->thirdId = thirdId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendHideAndSeekHallResult(ui64 rakssId, String itemInfo)
{
	auto packet = LORD::make_shared<S2CPacketHideAndSeekHallResult>();
	packet->itemInfo = itemInfo;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncEntityColorfulTime(ui64 rakssId, int entityId, float time)
{
	auto packet = LORD::make_shared<S2CPacketEntityColorfulTime>();
	packet->entityId = entityId;
	packet->time = time;
	if (rakssId == 0)
		sendPacketToTrackingPlayers(entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendEntitySelected(ui64 rakssId, int entityId, int type)
{
	auto packet = LORD::make_shared<S2CPacketEntitySelected>();
	packet->entityId = entityId;
	packet->type = type;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncEntityDisableMoveTime(ui64 rakssId, int entityId, float time)
{
	auto packet = LORD::make_shared<S2CPacketEntityDisableMoveTime>();
	packet->entityId = entityId;
	packet->time = time;
	if (rakssId == 0)
		sendPacketToTrackingPlayers(entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPlayerEffectGunGire(int shootingEntityId, int gunId, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncEffectGunFire>();
	s2cPacket->shootingEntityId = shootingEntityId;
	s2cPacket->gunId = gunId;
	s2cPacket->endPos = endPos;
	s2cPacket->hasBlockTarget = hasBlockTarget;
	s2cPacket->blockTargetPos = blockTargetPos;
	sendPacketToTrackingPlayers(shootingEntityId, s2cPacket, true);
}

void ServerPacketSender::broadCastPlayerEffectGunGireAtEntityResult(int shootingEntityId, int hitEntityId, const Vector3& hitPos, const Vector3& dir, int traceType, bool isHeadshot, int gunId)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncEffectGunFireAtEntityResult>();
	s2cPacket->shootingEntityId = shootingEntityId;
	s2cPacket->hitEntityId = hitEntityId;
	s2cPacket->hitPos = hitPos;
	s2cPacket->dir = dir;
	s2cPacket->traceType = traceType;
	s2cPacket->isHeadshot = isHeadshot;
	s2cPacket->gunId = gunId;
	sendPacketToTrackingPlayers(shootingEntityId, s2cPacket, true);
}

void ServerPacketSender::broadCastPlayerEffectGunGireAtBlockResult(int shootingEntityId, const Vector3i& blockPos, const Vector3& hitPos, const Vector3& dir, bool isRed, int gunId)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncEffectGunFireAtBlockResult>();
	s2cPacket->blockPos = blockPos;
	s2cPacket->hitPos = hitPos;
	s2cPacket->dir = dir;
	s2cPacket->isRed = isRed;
	s2cPacket->gunId = gunId;
	sendPacketToTrackingPlayers(shootingEntityId, s2cPacket, true);
}

void ServerPacketSender::broadCastPlayerEffectGunGireExplosionResult(int shootingEntityId, const Vector3& hitPos, int gunId)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncEffectGunFireExplosionResult>();
	s2cPacket->hitPos = hitPos;
	s2cPacket->gunId = gunId;
	sendPacketToTrackingPlayers(shootingEntityId, s2cPacket, true);
}

void ServerPacketSender::broadCastStopLaserGun(i32 shootingEntityId)
{
	auto packet = LORD::make_shared<S2CPacketStopLaserGun>();
	packet->shootingEntityId = shootingEntityId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastPlayerOpacity(int entityId, float opacityValue)
{
	auto packet = LORD::make_shared<S2CPacketSetPlayerOpacity>();
	packet->entityId = entityId;
	packet->opacityValue = opacityValue;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendShowSkyBlockInfo(i64 rakssId, int lv, i64 cur_exp, i64 max_exp, int island_lv, String area, bool is_max, bool isMaster)
{
	auto packet = LORD::make_shared<S2CPacketShowSkyBlockInfo>();
	packet->lv = lv;
	packet->cur_exp = cur_exp;
	packet->max_exp = max_exp;
	packet->island_lv = island_lv;
	packet->area = area;
	packet->is_max = is_max;
	packet->isMaster = isMaster;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendEnableAutoShoot(i64 rakssId, bool enable)
{
	auto packet = LORD::make_shared<S2CPacketEnableAutoShoot>();
	packet->enable = enable;
	if (rakssId == 0)
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendDisarmament(i64 rakssId, bool isDisarmament)
{
	auto packet = LORD::make_shared<S2CPacketDisarmament>();
	packet->isDisarmament = isDisarmament;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::notifyGetGoods(ui64 rakssId, String icon, int count)
{
	auto packet = LORD::make_shared<S2CPacketNotifyGetGoods>();
	packet->icon = icon;
	packet->count = count;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerNamePerspective(ui64 rakssId, i32 entityId, bool isPerspective)
{
	auto packet = LORD::make_shared<S2CPacketPlayerNamePerspective>();
	packet->entityId = entityId;
	packet->isPerspective = isPerspective;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendEntityAddEntityFeature(i32 entityId, String name, String description, float duration, float scale, int type)
{
	auto packet = LORD::make_shared<S2CPacketAddEntityFeature>();
	packet->entityId = entityId;
	packet->name = name;
	packet->description = description;
	packet->duration = duration;
	packet->scale = scale;
	packet->type = type;
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::syncEntityFeatures(ui64 rakssId, BLOCKMAN::Entity* entity)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityFeature>();
	packet->entityId = entity->entityId;
	packet->entityFeatures = entity->m_entityFeatures;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncEntityBulletin(ui64 rakssId, BLOCKMAN::EntityBulletin* entityBulletin)
{
	auto player = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
	ui64 userId = player ? player->getPlatformUserId() : 0;
	auto packet = LORD::make_shared<S2CPacketSyncEntityBulletin>();
	packet->entityId = entityBulletin->entityId;
	packet->ranks = entityBulletin->getRanks();
	packet->name = entityBulletin->getNameLang();
	packet->bulletinId = entityBulletin->getBulletinId();
	packet->userRank = entityBulletin->getRankByUserId(userId);

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
	else
	{
		sendPacketToTrackingPlayers(entityBulletin->entityId, packet);
	}
}

void ServerPacketSender::syncBirdSimulator(BLOCKMAN::EntityPlayer* player, int syncType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p && p->m_birdSimulator)
	{
		switch ((SyncBirdSimulatorType)syncType)
		{
		case SyncBirdSimulatorType::DEFAULT:
			break;
		case SyncBirdSimulatorType::BAG:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdBag>();
			packet->birdBag = *player->m_birdSimulator->getBag();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::ACTIVITY:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdActivity>();
			packet->activity = *player->m_birdSimulator->getActivity();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::PLAYER_INFO:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdPlayerInfo>();
			packet->playerInfo = *player->m_birdSimulator->getPlayerInfo();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::ATLAS:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdAtlas>();
			packet->atlas = player->m_birdSimulator->getAtlas();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::TAKS:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdTask>();
			packet->tasks = player->m_birdSimulator->getTasks();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::FOOD:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdFood>();
			packet->foods = player->m_birdSimulator->getFoods();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::DRESS:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdDress>();
			packet->dress = player->m_birdSimulator->getDress();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::STORE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdStore>();
			packet->stores = player->m_birdSimulator->getStores();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::PERSONAL_STORE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdPersonalStore>();
			packet->personalStore = player->m_birdSimulator->getPersonalStore();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::SCOPE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdScope>();
			packet->scopes = player->m_birdSimulator->getScopes();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::TIME_PRICE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdTimePrice>();
			packet->timePrices = player->m_birdSimulator->getTimePrices();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBirdSimulatorType::BUFF:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdBuff>();
			packet->buffs = player->m_birdSimulator->getBuffs();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		}
	}
}

void ServerPacketSender::syncPlayerBagInfo(ui64 rakssId, BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketSyncPlayerBagInfo>();
	packet->entityId = player->entityId;
	packet->isConvert = player->m_isBirdConvert;
	packet->curCapacity = player->m_bagCapacity;
	packet->maxCapacity = player->m_bagMaxCapacity;
	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
	else
	{
		sendPacketToTrackingPlayers(player->entityId, packet, true);
	}

}

void ServerPacketSender::sendBirdGain(ui64 rakssId, const vector<BLOCKMAN::BirdGain>::type& items)
{
	auto packet = LORD::make_shared<S2CPacketBirdGain>();
	packet->items = items;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendBirdNestOperation(ui64 rakssId, bool isUnlock, i64 birdId)
{
	auto packet = LORD::make_shared<S2CPacketBirdNestOperation>();
	packet->isUnlock = isUnlock;
	packet->birdId = birdId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendBirdLotteryResult(ui64 rakssId, String eggBodyId, const BLOCKMAN::BirdInfo& info)
{
	auto packet = LORD::make_shared<S2CPacketBirdLotteryResult>();
	packet->eggBodyId = eggBodyId;
	packet->birdInfo = info;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendEntityAction(ui64 rakssId, int entityId, int entityClass, int actionStatus, int baseAction, int uperBaseAction)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityAction>();
	packet->entityId = entityId;
	packet->entityClass = entityClass;
	packet->actionStatus = actionStatus;
	packet->baseAction = baseAction;
	packet->preBaseAction = uperBaseAction;

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
	else
	{
		sendPacketToTrackingPlayers(entityId, packet);
	}
}

void ServerPacketSender::sendBirdAddScore(ui64 rakssId, i32 score, i32 scoreType)
{
	auto packet = LORD::make_shared<S2CPacketBirdAddScore>();
	packet->score = score;
	packet->scoreType = scoreType;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::onBirdSimulatorGather(ui64 userId, i64 birdId, const  BLOCKMAN::BlockPos& pos)
{
	if (Server::Instance()->getBlockFruitsManager())
	{
		Server::Instance()->getBlockFruitsManager()->onGather(userId, birdId, pos);
	}
}

void ServerPacketSender::syncEntityHealth(ui64 rakssId, i32 entityId, float health, float maxHealth)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityHealth>();
	packet->entityId = entityId;
	packet->health = health;
	packet->maxHealth = maxHealth;

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
	else
	{
		sendPacketToTrackingPlayers(entityId, packet);
	}
}