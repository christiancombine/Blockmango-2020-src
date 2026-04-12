#include "ServerPacketSender.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityRankNpc.h"
#include "Entity/EntityMerchant.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntityAircraft.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Item/Potion.h"
#include "Script/GameServerEvents.h"
#include "CommodityManager.h"
#include "Network/protocol/S2CPackets5.h"
#include "Blockman/World/BlockChangeRecorderServer.h"

using namespace BLOCKMAN;

std::shared_ptr<DataPacket> ServerPacketSender::constructAddPotionEffectPacket(int entityId, PotionEffect* effect)
{
	auto packet = LORD::make_shared<S2CPacketAddEntityEffect>();
	packet->entityId = entityId;
	packet->potionId = effect->getPotionID();
	packet->amplifier = effect->getAmplifier();
	packet->duration = effect->getDuration();
	packet->isAmbient = effect->getIsAmbient();
	packet->isSplashPotion = effect->getIsSplashPotion();
	packet->isPotionDurationMax = effect->getIsPotionDurationMax();

	return packet;
}

void ServerPacketSender::sendRemoveEntityEffectToTrackingPlayers(int entityId, int potionId)
{
	auto packet = LORD::make_shared<S2CPacketRemoveEntityEffect>();
	packet->entityId = entityId;
	packet->potionId = potionId;

	//LordLogInfo("sendRemoveEntityEffectToTrackingPlayers: entityId: %d, potionId: %d", packet->entityId, packet->potionId);

	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::sendSyncWorldTime(ui64 rakssid, int64_t time)
{
	auto packet = LORD::make_shared<S2CPacketSyncWorldTime>();
	packet->time = time;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::broadcastSyncWorldTime(int64_t time)
{
	auto packet = LORD::make_shared<S2CPacketSyncWorldTime>();
	packet->time = time;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSyncWorldWeather(ui64 rakssid, String weather, int time)
{
	auto packet = LORD::make_shared<S2CPacketSyncWorldWeather>();
	packet->weather = weather;
	packet->time = time;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::broadcastSyncWorldWeather(String weather, int time)
{
	auto packet = LORD::make_shared<S2CPacketSyncWorldWeather>();
	packet->weather = weather;
	packet->time = time;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

std::shared_ptr<DataPacket> ServerPacketSender::constructPlaySoundPacket(uint soundType, i32 entityId)
{
	auto packet = LORD::make_shared<S2CPacketPlaySound>();
	packet->m_soundType = soundType;
	packet->m_entityId = entityId;
	return packet;
}

void ServerPacketSender::sendAllSetSignTexts(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketSetSignTexts>();
	for (auto tileEntity : Server::Instance()->getWorld()->getLoadedTileEntities())
	{
		if (auto tileEntitySign = dynamic_cast<TileEntitySign*>(tileEntity))
		{
			S2CPacketSetSignTexts::SignData data;
			data.position = tileEntitySign->m_pos;
			data.isWordWrap = tileEntitySign->isWordWrap();
			for (size_t i = 0; i < BLOCKMAN::TileEntitySign::SIGN_COUNT; ++i)
			{
				data.texts[i] = tileEntitySign->getText(i);
			}
			packet->signData.push_back(data);
		}
	}
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadcastSetSignTexts(BLOCKMAN::TileEntitySign* tileEntitySign)
{
	auto packet = LORD::make_shared<S2CPacketSetSignTexts>();
	S2CPacketSetSignTexts::SignData data;
	data.position = tileEntitySign->m_pos;
	data.isWordWrap = tileEntitySign->isWordWrap();
	for (size_t i = 0; i < BLOCKMAN::TileEntitySign::SIGN_COUNT; ++i)
	{
		data.texts[i] = tileEntitySign->getText(i);
	}
	packet->signData.push_back(data);
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSetSignTexts(BLOCKMAN::TileEntitySign* tileEntitySign, ui64 rakssId)
{
	if (!tileEntitySign) return;

	auto packet = LORD::make_shared<S2CPacketSetSignTexts>();
	S2CPacketSetSignTexts::SignData data;
	data.position = tileEntitySign->m_pos;
	data.isWordWrap = tileEntitySign->isWordWrap();
	for (size_t i = 0; i < BLOCKMAN::TileEntitySign::SIGN_COUNT; ++i)
	{
		data.texts[i] = tileEntitySign->getText(i);
	}
	packet->signData.push_back(data);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendChangePlayerInfo(ui64 rakssId, BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketChangePlayerInfo>();
	packet->m_playerId = player->entityId;
	packet->m_playerShowName = player->getShowName();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastChangePlayerInfo(BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketChangePlayerInfo>();
	packet->m_playerId = player->entityId;
	packet->m_playerShowName = player->getShowName();
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendUpdateMerchant(ui64 rakssid, int entityId, int index)
{
	auto merchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (merchant)
	{
		auto cList = CommodityManager::Instance()->getCommodityList(index);
		if (!cList)
			return;
		auto packet = LORD::make_shared<S2CPacketUpdateMerchant>();
		packet->id = merchant->entityId;
		packet->nameLang = merchant->getNameLang();
		packet->name = merchant->getName();
		packet->commodities = cList->getCommodities();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
	}
}

void ServerPacketSender::sendUpdateMerchantInfo(ui64 rakssid, int entityId)
{
	auto merchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (merchant)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateMerchantInfo>();
		packet->id = merchant->entityId;
		packet->nameLang = merchant->getNameLang();
		packet->name = merchant->getName();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
	}
}

void ServerPacketSender::sendUpdateMerchantCommodities(ui64 rakssid, int entityId, int index)
{
	auto merchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (merchant)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateMerchantCommodities>();
		packet->id = merchant->entityId;
		auto cList = CommodityManager::Instance()->getCommodityList(index);
		if (!cList)
			return;
		packet->commodities = cList->getCommodities();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
	}
}

void ServerPacketSender::sendSetFlying(ui64 rakssId, BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketSetFlying>();
	packet->playerId = player->entityId;
	packet->isFlying = player->capabilities.isFlying;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSetFlyingToTrackingPlayers(BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketSetFlying>();
	packet->playerId = player->entityId;
	packet->isFlying = player->capabilities.isFlying;
	sendPacketToTrackingPlayers(player->entityId, packet);
}

void ServerPacketSender::sendExplosion(BLOCKMAN::EntityPlayer* player, const Vector3& pos, float size, const BLOCKMAN::BlockPosArr& poss, const Vector3& motion, bool isSmoke, float custom_size)
{
	ui64   rakssId = dynamic_cast<EntityPlayerMP*>(player)->getRaknetID();
	auto packet = LORD::make_shared<S2CPacketExplosion>();
	packet->m_size = size;
	packet->m_pos = pos;
	packet->m_motion = motion;
	packet->m_customsize = custom_size;
	packet->m_smoke = isSmoke;
	for (auto p : poss)
	{
		packet->m_poss.push_back(p);
	}
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendOpenCraftTable(BLOCKMAN::EntityPlayer* player)
{
	auto mp = dynamic_cast<EntityPlayerMP*> (player);
	if (mp)
	{
		auto packet = LORD::make_shared<S2CPacketOpenCraftTable>();
		Server::Instance()->getNetwork()->sendPacket(packet, mp->getRaknetID());
	}
}
void ServerPacketSender::sendBroadCastFurnaceTime(BLOCKMAN::EntityPlayer* player, Vector3i blockPos, int time, int maxTime)
{
	auto packet = LORD::make_shared<S2CPacketUpdateFurnaceTime>();
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->time = time;
	packet->maxTime = maxTime;

	auto mp = dynamic_cast<EntityPlayerMP*> (player);
	if (mp)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, mp->getRaknetID());
	}
	else
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
}

void ServerPacketSender::sendUsePropResults(ui64 rakssId, String propId, int results)
{
	auto packet = LORD::make_shared<S2CPacketUsePropResults>();
	packet->m_propId = propId;
	packet->m_results = results;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);

}

void ServerPacketSender::sendUserIn(ui64 rakssId, ui64 platformUserId, String nickName, int teamId, String teamName, int curPlayer, int maxPlayer, const UserAttrInfo& attrInfo)
{
	if (attrInfo.followEnterType == 2) // in watch follow mode
		return;
	auto packet = LORD::make_shared<S2CPacketUserIn>();
	packet->m_nickName = nickName;
	packet->m_platformUserId = platformUserId;
	packet->m_teamId = teamId;
	packet->m_teamName = teamName;
	packet->m_curPlayer = curPlayer;
	packet->m_maxPlayer = maxPlayer;
	packet->m_picUrl = attrInfo.picUrl.c_str();
	packet->m_vip = attrInfo.vip;
	packet->m_country = attrInfo.country.c_str();
	packet->m_lang = attrInfo.lang.c_str();
	packet->m_sex = attrInfo.sex;

	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}

}

void ServerPacketSender::sendUserOut(ui64 platformUserId)
{
	auto packet = LORD::make_shared<S2CPacketUserOut>();
	packet->m_platformUserId = platformUserId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendLoginResult(ui64 rakssId, int32_t resultCode)
{
	auto packet = LORD::make_shared<S2CPacketLoginResult>();
	packet->m_resultCode = resultCode;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPing(ui64 rakssId, ui32 clientSendPingTime)
{
	auto packet = LORD::make_shared<S2CPacketPing>();
	packet->m_clientSendPingTime = clientSendPingTime;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSettlement(ui64 rakssId, String gameType, String gameResult)
{
	auto packet = LORD::make_shared<S2CPacketSettlement>();
	packet->m_gameType = gameType;
	packet->m_gameResult = gameResult;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendGameover(ui64 rakssId, String overMessage, int code)
{
	auto packet = LORD::make_shared<S2CPacketGameover>();
	packet->m_overMessage = overMessage;
	packet->m_code = code;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCloseContainer(int playerID, bool close, const Vector3i& containerPos)
{
	auto packet = LORD::make_shared<S2CPacketCloseContainer>();
	packet->m_close = close;
	packet->m_playerID = playerID;
	packet->m_containerPosX = (i16)containerPos.x;
	packet->m_containerPosY = (i16)containerPos.y;
	packet->m_containerPosZ = (i16)containerPos.z;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSetAllowFlying(ui64 rakssId, bool allowFlying)
{
	auto packet = LORD::make_shared<S2CPacketSetAllowFlying>();
	packet->allowFlying = allowFlying;

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendGameStatus(ui64 rakssId, int status)
{
	auto packet = LORD::make_shared<S2CPacketGameStatus>();
	packet->m_status = status;
	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}


void ServerPacketSender::sendSetWatchMode(ui64 rakssId, int entityId, bool isWatchMode)
{
	auto packet = LORD::make_shared<S2CPacketSetWatchMode>();
	packet->playerId = entityId;
	packet->isWatchMode = isWatchMode;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSetWatchModeToTrackingPlayers(int entityId, bool isWatchMode)
{
	auto packet = LORD::make_shared<S2CPacketSetWatchMode>();
	packet->playerId = entityId;
	packet->isWatchMode = isWatchMode;
	sendPacketToTrackingPlayers(entityId, packet);
}

void ServerPacketSender::sendPlayerSettlement(ui64 rakssId, String result, bool8 isNextServer)
{
	auto packet = LORD::make_shared<S2CPacketPlayerSettlement>();
	packet->m_result = result;
	packet->m_isNextServer = isNextServer;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendGameSettlement(ui64 rakssId, String result, bool8 isNextServer)
{
	auto packet = LORD::make_shared<S2CPacketGameSettlement>();
	packet->m_result = result;
	packet->m_isNextServer = isNextServer;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPlayerLifeStatus(ui64 platformUserId, bool isLife)
{
	auto packet = LORD::make_shared<S2CPacketPlayerLifeStatus>();
	packet->m_platformUserId = platformUserId;
	packet->m_isLife = isLife;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastBedDestroy(int teamId)
{
	auto packet = LORD::make_shared<S2CPacketBedDestroy>();
	packet->m_teamId = teamId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendRespawnCountdown(ui64 rakssId, ui32 second)
{
	auto packet = LORD::make_shared<S2CPacketRespawnCountdown>();
	packet->m_second = second;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadAllPlayerTeamInfo(String result)
{
	auto packet = LORD::make_shared<S2CPacketAllPlayerTeamInfo>();
	packet->m_result = result;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChangePlayerTeam(ui64 rakssId, ui64 platformUserId, int teamId)
{
	auto packet = LORD::make_shared<S2CPacketChangePlayerTeam>();
	packet->m_platformUserId = platformUserId;
	packet->m_teamId = teamId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastChangePlayerTeam(ui64 platformUserId, int teamId)
{
	auto packet = LORD::make_shared<S2CPacketChangePlayerTeam>();
	packet->m_platformUserId = platformUserId;
	packet->m_teamId = teamId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendPlayerChangeActor(ui64 rakssId, const BLOCKMAN::EntityPlayerMP* pPlayer)
{
	auto packet = LORD::make_shared<S2CPacketPlayerChangeActor>();
	packet->m_entityId = pPlayer->entityId;
	packet->m_actorName = pPlayer->m_actorName;
	packet->m_bodyId = pPlayer->m_actorBodyId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	/*LordLogDebug("sendPlayerChangeActor entityId:%d, actorName:%s, bodyId:%s, rakssId:%s",
		packet->m_entityId, packet->m_actorName.c_str(), packet->m_bodyId.c_str(), StringUtil::ToString(rakssId).c_str());*/
}

void ServerPacketSender::syncChangePlayerActorInfo(ui64 rakssId, int changeActortCount, int needMoneyCount)
{
	auto packet = LORD::make_shared<S2CSyncChangePlayerActorInfo>();
	packet->changeActorCount = changeActortCount;
	packet->needMoneyCount = needMoneyCount;
	packet->actorName = "";
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendNpcActorInfo(i64 rakssId, EntityActorNpc* pNpc)
{
	auto packet = LORD::make_shared<S2CSyncNpcActorInfo>();
	packet->entityId = pNpc->entityId;
	packet->actorName = pNpc->getActorName();
	packet->headName = pNpc->getHeadName();
	packet->skillName = pNpc->getSkillName();
	packet->haloEffectName = pNpc->getHaloEffectName();
	packet->isCanObstruct = pNpc->isCanObstruct();
	packet->content = pNpc->getContent();
	packet->isCanCollided = pNpc->canBeCollidedWith();
	packet->isSuspended = pNpc->getSuspended();
	packet->scale = pNpc->getScale();
	packet->height = pNpc->getHeight();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPlayerChangeActor(EntityPlayer* pPlayer)
{
	auto packet = LORD::make_shared<S2CPacketPlayerChangeActor>();
	packet->m_entityId = pPlayer->entityId;
	packet->m_actorName = pPlayer->m_actorName;
	packet->m_bodyId = pPlayer->m_actorBodyId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	/*LordLogDebug("broadCastPlayerChangeActor entityId:%d, actorName:%s, bodyId:%s",
	packet->m_entityId, packet->m_actorName.c_str(), packet->m_bodyId.c_str());*/
}

void ServerPacketSender::broadCastPlayerRestoreActor(int entityId)
{
	auto packet = LORD::make_shared<S2CPacketPlayerRestoreActor>();
	packet->m_entityId = entityId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendUpdateShop(ui64 rakssId)
{
	Shop* shop = Server::Instance()->getWorld()->getShop();
	if (shop)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateShop>();
		packet->m_goods = shop->getGoods();
		packet->m_respawnGoods = shop->getRespawnGoods();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::broadCastUpdateShop()
{
	Shop* shop = Server::Instance()->getWorld()->getShop();
	if (shop)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateShop>();
		packet->m_goods = shop->getGoods();
		packet->m_respawnGoods = shop->getRespawnGoods();
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
}

void ServerPacketSender::broadCastUpdateShopGoods(int groupIndex, int goodsIndex, int limit)
{
	auto packet = LORD::make_shared<S2CPacketUpdateShopGoods>();
	packet->groupIndex = groupIndex;
	packet->goodsIndex = goodsIndex;
	packet->limit = limit;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendShoppingResult(ui64 rakssId, int code, String result)
{
	auto packet = LORD::make_shared<S2CPacketShoppingResult>();
	packet->code = code;
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendInitMoney(ui64 rakssId, ui64 diamondBlues, ui64 diamondGolds, ui64 golds)
{
	auto packet = LORD::make_shared<S2CPacketInitMoney>();
	packet->diamondBlues = diamondBlues;
	packet->diamondGolds = diamondGolds;
	packet->golds = golds;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendReloadBullet(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketReloadBullet>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPlayerGunGire(i32 entityId, i32 gunId, const PACKET_DETAIL::Vector3& pos, const PACKET_DETAIL::Vector3& dir,
	ui32 tracyType, const PACKET_DETAIL::Vector3i& blockPos, const PACKET_DETAIL::Vector3& hitpos, ui32 hitEntityID, bool isHeadshot)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncGunFireResult>();
	s2cPacket->sourceEntityId = entityId;
	s2cPacket->gunId = gunId;
	s2cPacket->pos = pos;
	s2cPacket->dir = dir;
	s2cPacket->tracyType = tracyType;
	s2cPacket->blockPos = blockPos;
	s2cPacket->hitpos = hitpos;
	s2cPacket->hitEntityID = hitEntityID;
	s2cPacket->isHeadshot = isHeadshot;
	sendPacketToTrackingPlayers(entityId, s2cPacket, true);
}

void ServerPacketSender::broadCastSyncTakeVehicle(int playerId, bool onoff, bool isDriver, int vehicleId)
{
	auto packet = LORD::make_shared<S2CPacketSyncTakeVehicle>();

	packet->playerId = playerId;
	packet->onoff = onoff;
	packet->vehicleId = vehicleId;
	packet->isDriver = isDriver;
	sendPacketToTrackingPlayers(playerId, packet, true);
}

void ServerPacketSender::broadCastChangePlayerMaxHealth(i32 entityId, float health, bool singleMaxHealth)
{
	auto packet = LORD::make_shared<S2CPacketChangeMaxHealth>();
	packet->entityId = entityId;
	packet->health = health;
	packet->singleMaxHealth = singleMaxHealth;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChangePlayerMaxHealth(ui64 rakssId, i32 entityId, float health, bool singleMaxHealth)
{
	auto packet = LORD::make_shared<S2CPacketChangeMaxHealth>();
	packet->entityId = entityId;
	packet->health = health;
	packet->singleMaxHealth = singleMaxHealth;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerUpdateCurrency(ui64 rakssId, ui64 currency)
{
	auto packet = LORD::make_shared<S2CPacketUpdateCurrency>();
	packet->currency = currency;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSelectRoleData(ui64 rakssId, String result)
{
	auto packet = LORD::make_shared<S2CPacketSelectRoleData>();
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerOwnVehicle(ui64 rakssId, const std::set<int>& ownVehicle)
{
	auto packet = LORD::make_shared<S2CPacketSyncPlayerOwnVehicle>();
	for (auto iter = ownVehicle.cbegin(); iter != ownVehicle.cend(); iter++)
	{
		packet->m_vehicles.push_back(*iter);
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastSyncLockVehicle(int playerId, bool onoff, int vehicleId)
{
	auto packet = LORD::make_shared<S2CPacketSyncLockVehicle>();

	packet->playerId = playerId;
	packet->onoff = onoff;
	packet->vehicleId = vehicleId;
	sendPacketToTrackingPlayers(playerId, packet, true);
}

void ServerPacketSender::sendSyncVehicleState(ui64 rakssId, int entityId)
{
	auto vehicle = dynamic_cast<EntityVehicle*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (!vehicle)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncVehicleState>();
	packet->entityId = entityId;
	packet->velocityX = vehicle->getVelocity().x;
	packet->velocityY = vehicle->getVelocity().y;
	packet->isLocked = vehicle->isLocked();
	packet->hasDriver = vehicle->hasDriver();
	packet->state = (int)vehicle->getActionState();
	packet->driverId = vehicle->getDriverId();
	packet->turningRadius = vehicle->getInput().turningRadius;
	packet->throttle = vehicle->getInput().throttle;
	packet->braking = vehicle->getInput().braking;

	for (int passengerId : vehicle->getPassengersIds())
	{
		packet->m_passengers.push_back(passengerId);
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastVehicleMovement(int playerId, const BLOCKMAN::EntityVehicle* vehicle)
{
	auto packet = LORD::make_shared<S2CPacketSyncVehicleMovement>();

	packet->entityId = vehicle->entityId;
	packet->velocityX = vehicle->getVelocity().x;
	packet->velocityY = vehicle->getVelocity().y;
	packet->posX = vehicle->position.x;
	packet->posY = vehicle->position.y;
	packet->posZ = vehicle->position.z;
	packet->yaw = vehicle->rotationYaw;
	sendPacketToTrackingPlayers(playerId, packet, false);
}

void ServerPacketSender::syncPlayerTakeOnVehicle(ui64 rakssId, int playerId, bool isDriver, int vehicleId)
{
	auto packet = LORD::make_shared<S2CPacketSyncTakeVehicle>();

	packet->playerId = playerId;
	packet->onoff = true;
	packet->vehicleId = vehicleId;
	packet->isDriver = isDriver;

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastVehicleActionState(int playerId, int vehicleId, int state)
{
	auto packet = LORD::make_shared<S2CPacketSyncVehicleActionState>();

	packet->entityId = vehicleId;
	packet->state = state;
	sendPacketToTrackingPlayers(playerId, packet, true);
}

void ServerPacketSender::broadCastVehicleInput(int playerId, int vehicleId, const BLOCKMAN::VehicleInputData& input)
{
	auto packet = LORD::make_shared<S2CPacketSyncVehicleInput>();

	packet->entityId = vehicleId;
	packet->turningRadius = input.turningRadius;
	packet->throttle = input.throttle;
	packet->braking = input.braking;
	sendPacketToTrackingPlayers(playerId, packet, false);
}

void ServerPacketSender::notifyGetMoney(ui64 rakssId, int count)
{
	auto packet = LORD::make_shared<S2CPacketNotifyGetMoney>();
	packet->count = count;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::notifyGetItem(ui64 rakssId, int itemId, int meta, int count)
{
	auto packet = LORD::make_shared<S2CPacketNotifyGetItem>();
	packet->count = count;
	packet->itemId = itemId;
	packet->meta = meta;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::refreshSellItemSuc(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketSynSellItemSuc>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendBuyCommodityResult(ui64 rakssId, int code, String result)
{
	auto packet = LORD::make_shared<S2CPacketBuyCommodityResult>();
	packet->code = code;
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendShowBuyRespawn(ui64 rakssId, i32 times)
{
	auto packet = LORD::make_shared<S2CPacketShowBuyRespawn>();
	packet->times = times;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendShowGoNpcMerchant(ui64 rakssId, i32 entityId, float x, float y, float z, float yaw)
{
	auto packet = LORD::make_shared<S2CPacketShowGoNpcMerchant>();
	packet->x = x;
	packet->y = y;
	packet->z = z;
	packet->entityId = entityId;
	packet->yaw = yaw;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendUpdateBackpack(ui64 rakssId, int capacity, int maxCapacity)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBackpack>();
	packet->capacity = capacity;
	packet->maxCapacity = maxCapacity;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendRankData(ui64 rakssId, i32 entityId, String result)
{
	auto packet = LORD::make_shared<S2CPacketRankData>();
	packet->entityId = entityId;
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendUpdateRankNpc(ui64 rakssId, int entityId)
{
	auto entityRank = dynamic_cast<EntityRankNpc*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (entityRank)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateRankNpc>();
		packet->id = entityRank->entityId;
		packet->nameLang = entityRank->getNameLang();
		packet->name = entityRank->getName();
		packet->rankInfo = entityRank->getRankInfo();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}

}

void ServerPacketSender::sendActivateTrigger(ui64 rakssId, const Vector3i& blockPos, int blockId, int triggerIndex)
{
	auto packet = LORD::make_shared<S2CPacketActivateTrigger>();
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->blockId = blockId;
	packet->triggerIndex = triggerIndex;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadcastTakeAircraft(int playerId, bool onoff, int aircraftId)
{
	auto packet = LORD::make_shared<S2CPacketTakeAircraft>();
	packet->passengerId = playerId;
	packet->entityId = aircraftId;
	packet->isOn = onoff;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::syncPlayerTakeAircraft(ui64 rakssId, int playerId, int aircraftId)
{
	auto packet = LORD::make_shared<S2CPacketTakeAircraft>();
	packet->passengerId = playerId;
	packet->entityId = aircraftId;
	packet->isOn = true;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastAircraftStartFly(int entityId)
{
	auto aircraft = dynamic_cast<EntityAircraft*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (!aircraft)
		return;

	auto packet = LORD::make_shared<S2CPacketAircraftStartFly>();
	packet->entityId = entityId;
	packet->speed = aircraft->getSpeed();
	packet->startPos = aircraft->getAirLineStartPosition();
	packet->endPos = aircraft->getAirLineEndPosition();
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::syncAircraftState(ui64 rakssId, int entityId)
{
	auto aircraft = dynamic_cast<EntityAircraft*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (!aircraft)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncAircraftState>();
	packet->entityId = entityId;
	packet->isFlying = aircraft->isAircraftFlying();
	packet->speed = aircraft->getSpeed();
	packet->flyingTick = aircraft->getFlyingTick();
	packet->startPos = aircraft->getAirLineStartPosition();
	packet->endPos = aircraft->getAirLineEndPosition();
	auto passengers = aircraft->getPassengers();
	for (auto it = passengers.cbegin(); it != passengers.cend(); it++)
	{
		packet->m_passengers.push_back(*it);
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendChangeAircraftUI(ui64 rakssId, bool isShowUI)
{
	auto packet = LORD::make_shared<S2CPacketChangeAircraftUI>();
	packet->isShowUI = isShowUI;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastChangeAircraftUI(bool isShowUI)
{
	auto packet = LORD::make_shared<S2CPacketChangeAircraftUI>();
	packet->isShowUI = isShowUI;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendAirDropPosition(Vector3 position)
{
	auto packet = LORD::make_shared<S2CPacketAirdrop>();
	packet->m_airDropPos = position;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastPoisonCircleRange(vector<Vector2>::type range, float speed)
{
	auto packet = LORD::make_shared<S2CPacketPoisonCircleRange>();
	packet->minSafeRange = Vector3(range[0].x, 0, range[0].y);
	packet->maxSafeRange = Vector3(range[1].x, 0, range[1].y);
	packet->minPoisonRange = Vector3(range[2].x, 0, range[2].y);
	packet->maxPoisonRange = Vector3(range[3].x, 0, range[3].y);
	packet->speed = speed;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendMemberLeftAndKill(ui64 rakssId, int left, int kill)
{
	auto packet = LORD::make_shared<S2CPacketMemberLeftAndKill>();
	packet->m_left = left;
	packet->m_kill = kill;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendBlockDestroyeFailure(ui64 rakssId, bool destroyeStatus, int destroyeFailureType)
{
	auto packet = LORD::make_shared<S2CPacketSyncBlockDestroyeFailure>();
	packet->destroyeStatus = destroyeStatus;
	packet->destroyeFailureType = destroyeFailureType;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSkillType(ui64 rakssId, int skillType, float duration, float coldDownTime)
{
	auto packet = LORD::make_shared<S2CPacketSkillType>();
	packet->skillType = skillType;
	packet->duration = duration;
	packet->coldDownTime = coldDownTime;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendShowHideAndSeekBtnStatus(i64 rakssId, bool isActoBtnStatus, bool isCameraBtnStatus, bool isOtherBtnStatus)
{
	auto packet = LORD::make_shared<S2CPacketHideAndSeekBtnStatus>();
	packet->isShowChangeActorBtn = isActoBtnStatus;
	packet->isShowChangeCameraBtn = isCameraBtnStatus;
	packet->isShowOtherBtn = isOtherBtnStatus;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPickUpItemOrder(ui64 rakssId, i32 itemEntityId, i32 itemId, i32 pickUpPrice, i32 moneyType)
{
	auto packet = LORD::make_shared<S2CPacketPickUpItemOrder>();
	packet->itemId = itemId;
	packet->pickUpPrice = pickUpPrice;
	packet->moneyType = moneyType;
	packet->itemEntityId = itemEntityId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendRequestResult(ui64 rakssId, i32 itemId, BLOCKMAN::ResultCode resultCode, String resultMessage)
{
	auto packet = LORD::make_shared<S2CPacketRequestResult>();
	packet->itemId = itemId;
	packet->resultCode = (int)resultCode;
	packet->resultMessage = resultMessage;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSyncGameTimeShowUi(ui64 rakssId, bool isShowUI, int time)
{
	auto packet = LORD::make_shared<S2CPacketSyncGameTimeShowUi>();
	packet->isShowUI = isShowUI;
	packet->time = time;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastGameTimeShowUi(bool isShowUI, int time)
{
	auto packet = LORD::make_shared<S2CPacketSyncGameTimeShowUi>();
	packet->isShowUI = isShowUI;
	packet->time = time;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);

}


void ServerPacketSender::broadCastCameraLockToClient(bool isCameraLock, int entityId)
{
	auto packet = LORD::make_shared<S2CPacketSyncCameraLock>();
	packet->entityId = entityId;
	packet->isCameraLock = isCameraLock;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::SyncCameraLockStatus(ui64 rakssId, int playerId, bool isCameraLock)
{
	auto packet = LORD::make_shared<S2CPacketSyncCameraLock>();
	packet->entityId = playerId;
	packet->isCameraLock = isCameraLock;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendEntitySkillEffect(Vector3 position, String name, float duration, int width, int height, Vector3 color, int density)
{
	auto packet = LORD::make_shared<S2CPacketEntitySkillEffect>();
	packet->position = position;
	packet->name = name;
	packet->duration = duration;
	packet->width = width;
	packet->height = height;
	packet->color = color;
	packet->density = density;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendPlaySound(ui64 targetId, uint soundType, uint channelId)
{
	auto packet = LORD::make_shared<S2CPacketPlaySound>();
	packet->m_soundType = soundType;
	packet->m_entityId = targetId;
	packet->m_channelId = channelId;
	if (targetId == 0) {
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else {
		Server::Instance()->getNetwork()->sendPacket(packet, targetId);
	}
}

void ServerPacketSender::sendFriendOperation(ui64 rakssid, ui64 userId, i32 operationType)
{
	auto packet = LORD::make_shared<S2CPacketFriendOperation>();
	packet->operationType = operationType;
	packet->userId = userId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}