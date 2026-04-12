#include "S2CEntityPacketHandles.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/EntityOtherPlayerMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "Item/ItemStack.h"
#include "Entity/EntityPlayer.h"
#include "Entity/DamageSource.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityBullet.h"
#include "Entity/EntityPotion.h"
#include "World/World.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/IInventory.h"
#include "Inventory/Container.h"
#include "Util/ClientEvents.h"
#include "Item/Potion.h"
#include "Item/Items.h"
#include "Block/BlockManager.h"
#include "Block/Blocks.h"
#include "cWorld/BlockChangeRecorderClient.h"
#include "ShellInterface.h"
#include "Object/Root.h"
#include "Entity/Enchantment.h"
#include "Entity/EntitySnowball.h"
#include "Entity/EntityTNTPrimed.h"
#include "Entity/EntityGrenade.h"
#include "Entity/EntityTNTThrowable.h"
#include "Entity/Explosion.h"
#include "cEntity/EntityVehicleClient.h"
#include "Entity/EntityRankNpc.h"
#include "TileEntity/TileEntitys.h"
#include "cWorld/SignTextsChangeRecorder.h"
#include "Entity/EntityMerchant.h"
#include "BattleSummary.h"
#include "Blockman/GUI/RootGuiLayout.h"
#include "Util/LanguageKey.h"
#include "Util/LanguageManager.h"
#include "Util/ClientDataReport.h"
#include "cItem/cItem.h"
#include "Item/ItemStack.h"
#include "Setting/GunSetting.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityActorNpc.h"
#include "Blockman/EntityRenders/EntityRenderManager.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntityCreature.h"
#include "Entity/EntityCreatureBullet.h"
#include "Entity/EntitySkillThrowable.h"
#include "Entity/EntityFireball.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Entity/EntityBlockman.h"
#include "cEntity/EntityActorCannonClient.h"
#include "cEntity/PlayerControlller.h"
#include "cEntity/EntityFishHookClient.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityLandNpc.h"
#include "Entity/EntityBulletin.h"
#include "Entity/EntityBird.h"
#include "Entity/EntityDecoration.h"
#include "Render/RenderGlobal.h"
#include "Setting/MultiLanTipSetting.h"
#include "Plugins/GameClientEvents.h"
#include "cWorld/WorldClient.h"
#include "cEntity/EntityFlyingMountClient.h"
#include "cEntity/EntityFloatingMountClient.h"
#include "Entity/TeleportPositionConverter.h"

using namespace BLOCKMAN;


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketAddEntityEffect>& packet) 
{

	LordLogInfo("_handlePacket_S2CPacketAddEntityEffect_, packet->entityId=%d, packet->potionId=%d",
		(int)packet->entityId, (int)packet->potionId);

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pPlayer = dataCache->getPlayerByServerId(packet->entityId);
	if (!pPlayer) {
		LordLogError("recv S2CPacketAddEntityEffect but can not find player, player remote id=%d\n", packet->entityId);
		return;
	}

	PotionEffect* effect = LordNew PotionEffect(packet->potionId, packet->duration, packet->amplifier, packet->isAmbient);
	effect->setSplashPotion(packet->isSplashPotion);
	effect->setPotionDurationMax(packet->isPotionDurationMax);
	pPlayer->addPotionEffect(effect);

	if (packet->potionId == Potion::invisibility->getId()) {
		if (dataCache->getSelfServerId() != packet->entityId) {
			pPlayer->setInvisible(true);
		}
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketRemoveEntityEffect>& packet)
{

	LordLogInfo("_handlePacket_S2CPacketRemoveEntityEffect_, packet->entityId=%d", (int)packet->entityId);

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pPlayer = dataCache->getPlayerByServerId(packet->entityId);
	if (!pPlayer) {
		LordLogError("recv S2CPacketRemoveEntityEffect but can not find player, player remote id=%d\n", packet->entityId);
		return;
	}
	pPlayer->removePotionEffect(packet->potionId);

	if (packet->potionId == Potion::invisibility->getId()) {
		if (dataCache->getSelfServerId() != packet->entityId) {
			pPlayer->setInvisible(false);
		}
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketRemoveEntity>&packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	if (packet->m_entityID == dataCache->getSelfServerId())
	{
		LordLogError("It is impossible to receive a S2CPacketRemoveEntity to delete self Player.");
		return;
	}
	// if the entity is snowBall just remove the relesionship of the 
	int clientID = dataCache->getClientId(packet->m_entityID);
	dataCache->removeEntityByServerId(packet->m_entityID, false);
	dataCache->removeClientServerIdPair(clientID);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityMovement>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityId);
	if (!pEntity)
	{
		return;
	}

	auto pBlockman = dataCache->getBlockman();
	if (pEntity->isClass(ENTITY_CLASS_VEHICLE) && pBlockman->m_pPlayer->getVehicle() == pEntity && pBlockman->m_pPlayer->isDriver())
		return;

	if (pEntity)
	{
		float yaw = packet->m_rotating ? (packet->m_yaw) : pEntity->rotationYaw;
		float pitch = packet->m_rotating ? (packet->m_pitch) : pEntity->rotationPitch;
		Vector3 lastPos = Vector3(pEntity->serverPos.x / 32.f, pEntity->serverPos.y / 32.f, pEntity->serverPos.z / 32.f);
		pEntity->serverPos.x += packet->m_x;
		pEntity->serverPos.y += packet->m_y;;
		pEntity->serverPos.z += packet->m_z;
		Vector3 toPos = Vector3(pEntity->serverPos.x / 32.f, pEntity->serverPos.y / 32.f, pEntity->serverPos.z / 32.f);
		float distance = lastPos.distanceTo(toPos);
		int increment = 3;
		EntityLivingBase* moveEntity = dynamic_cast<EntityLivingBase*>(pEntity);
		if (moveEntity)
			increment = distance / (moveEntity->getAIMoveSpeed() + moveEntity->getSpeedAddition());
		else
			increment = distance / 0.1f;
		increment = Math::Max(increment, 3);
		pEntity->setPositionAndRotation2(toPos, yaw, pitch, increment);
	}
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityTeleport>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityId);
	if (!pEntity) {
		/************************************************************************/
		// when player died, all entity and clientId serverId map cleared
		/************************************************************************/
		return;
	}

	auto pBlockman = dataCache->getBlockman();
	if (pEntity->isClass(ENTITY_CLASS_VEHICLE) && pBlockman->m_pPlayer->getVehicle() == pEntity && pBlockman->m_pPlayer->isDriver())
		return;

	pEntity->serverPos.x = packet->m_x;
	pEntity->serverPos.y = packet->m_y;
	pEntity->serverPos.z = packet->m_z;
	Vector3 toPos = Vector3(pEntity->serverPos.x / 32.f, pEntity->serverPos.y / 32.f, pEntity->serverPos.z / 32.f);
	toPos.y = pEntity->serverPos.y / 32.f + 0.015625f + pEntity->yOffset;
	pEntity->setPositionAndRotation(toPos, packet->m_yaw, packet->m_pitch);
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityVelocity>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityID);

	if (pEntity)
	{
		float velx = packet->m_motionX / 8000.f;
		float vely = packet->m_motionY / 8000.f;
		float velz = packet->m_motionZ / 8000.f;
		/*if (dynamic_cast<EntityPlayer*>(pEntity)) {
		LordLogInfo("PacketEntityVelocity id[%d] Velocity(%f, %f, %f)", packet->m_entityID, velx, vely, velz);
		}*/
		pEntity->setVelocity(Vector3(velx, vely, velz));
	}
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnExpOrb>& packet)
{
	// nothing todo. 
	// this protocol is client to server.
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnMob>& packet)
{
	// nothing todo. 
	// this protocol is client to server.
	LordLogInfo("handlePacket>>S2CPacketSpawnMob");
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnPlayer>& packet)
{
	auto pWorld = BLOCKMAN::Blockman::Instance()->getWorld();

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto pBlockman = dataCache->getBlockman();
	auto pWorldClient = dataCache->getWorldClient();

	EntityOtherPlayerMP* pPlayer = dynamic_cast<EntityOtherPlayerMP*>(dataCache->getPlayerByServerId(packet->m_entityID));
	if (pPlayer)
	{
		int clientID = dataCache->getClientId(packet->m_entityID);
		dataCache->removeEntityByServerId(packet->m_entityID, false);
		dataCache->removeClientServerIdPair(clientID);
		pPlayer = nullptr;
	}

	float x = packet->m_posX / 32.f;
	float y = packet->m_posY / 32.f;
	float z = packet->m_posZ / 32.f;
	// float yaw = packet->m_yaw * 360.f / 256.0F;
	// float pitch = packet->m_pitch * 360.f / 256.0F;
	float yaw = packet->m_yaw;
	float pitch = packet->m_pitch;
	pPlayer = LordNew EntityOtherPlayerMP(pBlockman->m_pWorld, packet->m_name);
	pPlayer->prevPos.x = pPlayer->lastTickPos.x = float(pPlayer->serverPos.x = packet->m_posX);
	pPlayer->prevPos.y = pPlayer->lastTickPos.y = float(pPlayer->serverPos.y = packet->m_posY);
	pPlayer->prevPos.z = pPlayer->lastTickPos.z = float(pPlayer->serverPos.z = packet->m_posZ);
	pPlayer->m_holdItemActor = packet->m_holdItemActor;
	pPlayer->m_holdItemChanged = true;
	pPlayer->m_isConcealPower = packet->m_isConcealPower;
	pPlayer->setPositionAndRotation(Vector3(x, y, z), yaw, pitch);
	pPlayer->setRotationYawHead(yaw);
	pPlayer->setInvisible(packet->m_invisible);
	pPlayer->setShowName(packet->m_showName);
	pPlayer->m_sex = packet->m_sex;
	pPlayer->m_vip = packet->m_vip;
	pPlayer->m_outLooksChanged = true;
	pPlayer->setTeamId(packet->m_teamId);
	pPlayer->setTeamName(packet->m_teamName);
	pPlayer->setPlatformUserId(packet->m_platformUserId);
	pPlayer->m_defaultIdle = packet->m_defaultIdle;
	pPlayer->spYaw = packet->m_isSpYaw;
	pPlayer->spYawRadian = packet->m_SpYawRadian;
	LordLogInfo("spawnPlayer get name %s getshowName %s", packet->m_name.c_str(), packet->m_showName.c_str());

	// add entity to world
	dataCache->addClientServerIdPair(pPlayer->entityId, packet->m_entityID);
	pWorldClient->spawnEntityInWorld(pPlayer);
	LordLogInfo("recv S2CPacketSpawnPlayer server id[%d] sex[%d] name[%s] pos(%f, %f, %f)",
		packet->m_entityID, packet->m_sex, packet->m_name.c_str(), x, y, z);

	SCRIPT_EVENT::OtherPlayerSpawnEvent::invoke(pPlayer);
}

Entity* S2CEntityPacketHandles::createEntityVehicle(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	int carID = packet->m_throwerID; //use throwerID as carID
	EntityVehicle* pVehicle = LordNew EntityVehicleClient(world, carID, packet->m_pos, packet->m_yaw);
	//LordLogInfo("createEntityVehicle carId %d, clientId %d, serverId %d", carID, pVehicle->entityId, packet->m_entityID);
	return pVehicle;
}

Entity* S2CEntityPacketHandles::createEntityBullet(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityBullet) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return nullptr;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityBullet* pBullet = LordNew EntityBullet(world);
	pBullet->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	pBullet->setPosition(packet->m_pos);
	pBullet->yOffset = 0.0f;
	pBullet->setVelocity(packet->m_motion);
	pBullet->m_shootingEntityId = shooter->entityId;
	/*LordLogInfo("S2CEntitySpawnCommonPacket(EntityArrow): clientId[%d],serverId[%d] x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f]",
	entityArrow->entityId, packet->m_entityID, entityArrow->position.x, entityArrow->position.y,entityArrow->position.z,
	entityArrow->motion.x, entityArrow->motion.y, entityArrow->motion.z, entityArrow->rotationYaw, entityArrow->rotationPitch);*/
	return pBullet;
}

Entity* S2CEntityPacketHandles::createEntityArrow(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityArrow) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return nullptr;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityArrow* entityArrow = LordNew EntityArrow(world);
	entityArrow->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityArrow->setPosition(packet->m_pos);
	entityArrow->yOffset = 0.0f;
	entityArrow->setVelocity(packet->m_motion);
	entityArrow->m_shootingEntityId = shooter->entityId;
	entityArrow->setOnFire(packet->m_onFire);

	if (packet->m_itemDamage > 0)
	{
		entityArrow->setArrowID(packet->m_itemDamage);
	}

	if (packet->potionId != 0)
	{
		entityArrow->m_potionEffect = LordNew PotionEffect(packet->potionId, packet->duration, packet->amplifier, packet->amplifier);
	}
	/*LordLogInfo("S2CEntitySpawnCommonPacket(EntityArrow): clientId[%d],serverId[%d] x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f]",
	entityArrow->entityId, packet->m_entityID, entityArrow->position.x, entityArrow->position.y,entityArrow->position.z,
	entityArrow->motion.x, entityArrow->motion.y, entityArrow->motion.z, entityArrow->rotationYaw, entityArrow->rotationPitch);*/
	return entityArrow;
}


Entity* S2CEntityPacketHandles::createEntitySnowball(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntitySnowball) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return nullptr;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntitySnowball* entitySnowball = LordNew EntitySnowball(world, shooter);
	entitySnowball->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entitySnowball->setPosition(packet->m_pos);
	entitySnowball->setHeadingFromThrower(shooter, shooter->rotationPitch, shooter->rotationYaw, 0.0F, 1.5F, 1.0F);
	entitySnowball->setVelocity(packet->m_motion);
	entitySnowball->setMeta(packet->m_itemDamage);
	return entitySnowball;
}

Entity* S2CEntityPacketHandles::createEntityTNTThrowable(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityTNTThrowable) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return nullptr;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityTNTThrowable* entityTNTThrowable = LordNew EntityTNTThrowable(world, shooter);
	entityTNTThrowable->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityTNTThrowable->setHeadingFromThrower(shooter, shooter->rotationPitch, shooter->rotationYaw, 0.0F, 1.5F, 1.0F);
	entityTNTThrowable->setVelocity(packet->m_motion);
	return entityTNTThrowable;
}

Entity* S2CEntityPacketHandles::createEntityGrenade(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityGrenade) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return nullptr;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityGrenade* entityGrenade = LordNew EntityGrenade(world, shooter, packet->m_itemID);
	entityGrenade->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityGrenade->setHeadingFromThrower(shooter, shooter->rotationPitch, shooter->rotationYaw, 0.0F, 1.5F, 1.0F);
	entityGrenade->setVelocity(packet->m_motion);
	return entityGrenade;
}

Entity* S2CEntityPacketHandles::createEntityMerchant(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityMerchant = LordNew EntityMerchant(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	// float yaw = packet->m_yaw * 360.f / 256.0F;
	// float pitch = packet->m_pitch * 360.f / 256.0F;
	float yaw = packet->m_yaw;
	float pitch = packet->m_pitch;
	entityMerchant->prevPos = entityMerchant->lastTickPos = packet->m_pos;
	entityMerchant->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityMerchant->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityMerchant->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityMerchant->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityMerchant->forceSpawn = true;
	return entityMerchant;
}

Entity* S2CEntityPacketHandles::createEntityRank(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityRank = LordNew EntityRankNpc(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	// float yaw = packet->m_yaw * 360.f / 256.0F;
	// float pitch = packet->m_pitch * 360.f / 256.0F;
	float yaw = packet->m_yaw;
	float pitch = packet->m_pitch;
	entityRank->prevPos = entityRank->lastTickPos = packet->m_pos;
	entityRank->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityRank->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityRank->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityRank->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityRank->forceSpawn = true;
	return entityRank;
}


Entity* S2CEntityPacketHandles::createEntityAircraft(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	int aircraftID = packet->m_throwerID;

	EntityAircraft* pAircraft = LordNew EntityAircraft(world, aircraftID, packet->m_pos, packet->m_yaw);
	//LordLogInfo("createEntityVehicle carId %d, clientId %d, serverId %d", carID, pVehicle->entityId, packet->m_entityID);
	return pAircraft;
}

Entity* S2CEntityPacketHandles::createEntityTNTPrimed(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	EntityTNTPrimed* tntPrimed = LordNew EntityTNTPrimed(world, packet->m_pos, shooter);
	tntPrimed->setLocationAndAngles(packet->m_pos, packet->m_yaw, packet->m_pitch);
	tntPrimed->setVelocity(packet->m_motion);
	return tntPrimed;
}

Entity* S2CEntityPacketHandles::createEntityPotion(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* thrower = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!thrower) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityPotion) can not find thrower, throwerID[%d]", packet->m_throwerID);
		return nullptr;
	}

	EntityPotion* entityPotion = LordNew EntityPotion(world, thrower, thrower->inventory->getCurrentItem());
	entityPotion->setPosition(packet->m_pos);
	entityPotion->setVelocity(packet->m_motion);
	/*LordLogInfo("S2CEntitySpawnCommonPacket(EntityPotion): clientId[%d],serverId[%d] x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f]",
	entityPotion->entityId, packet->m_entityID, entityPotion->position.x, entityPotion->position.y, entityPotion->position.z,
	entityPotion->motion.x, entityPotion->motion.y, entityPotion->motion.z, entityPotion->rotationYaw, entityPotion->rotationPitch);*/
	return entityPotion;
}

Entity* S2CEntityPacketHandles::createEntityActortNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityActorNpc = LordNew EntityActorNpc(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entityActorNpc->prevPos = entityActorNpc->lastTickPos = packet->m_pos;
	entityActorNpc->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityActorNpc->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityActorNpc->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityActorNpc->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	entityActorNpc->setWeight(packet->weight);
	entityActorNpc->setScale(packet->m_scale);
	entityActorNpc->setHeight(packet->height);
	entityActorNpc->setFloatProperty("textScale", packet->m_textScale);
	// TODO: add display name
	entityActorNpc->forceSpawn = true;
	return entityActorNpc;
}

Entity* S2CEntityPacketHandles::createEntityCreature(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	int monsterID = packet->m_throwerID;

	Vector3 pos = packet->m_pos;
	EntityCreature* pCreature = LordNew EntityCreature(world, monsterID, pos, packet->m_yaw, "");
	pCreature->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	pCreature->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	pCreature->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	pCreature->prevPos = pCreature->lastTickPos = packet->m_pos;
	pCreature->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	pCreature->setScale(packet->m_scale);
	return pCreature;
}

Entity* S2CEntityPacketHandles::createEntityBuildNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityBuildNpc = LordNew EntityBuildNpc(world, packet->m_pos);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entityBuildNpc->prevPos = entityBuildNpc->lastTickPos = packet->m_pos;
	entityBuildNpc->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityBuildNpc->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityBuildNpc->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityBuildNpc->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityBuildNpc->forceSpawn = true;
	return entityBuildNpc;
}

Entity* S2CEntityPacketHandles::createEntityLandNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityLandNpc = LordNew EntityLandNpc(world, packet->m_pos);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entityLandNpc->prevPos = entityLandNpc->lastTickPos = packet->m_pos;
	entityLandNpc->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityLandNpc->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityLandNpc->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityLandNpc->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityLandNpc->forceSpawn = true;
	return entityLandNpc;
}

Entity* S2CEntityPacketHandles::createEntityBulletin(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = Blockman::Instance()->getWorld();
	auto entityBulletin = LordNew EntityBulletin(world, packet->m_pos);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entityBulletin->prevPos = entityBulletin->lastTickPos = packet->m_pos;
	entityBulletin->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityBulletin->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityBulletin->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityBulletin->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityBulletin->forceSpawn = true;
	return entityBulletin;
}

Entity* S2CEntityPacketHandles::createEntityBird(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = Blockman::Instance()->getWorld();
	auto entity = LordNew EntityBird(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entity->prevPos = entity->lastTickPos = packet->m_pos;
	entity->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entity->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entity->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entity->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entity->forceSpawn = true;
	return entity;
}

Entity* S2CEntityPacketHandles::createEntityDecoration(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = Blockman::Instance()->getWorld();
	auto entity = LordNew EntityDecoration(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entity->prevPos = entity->lastTickPos = packet->m_pos;
	entity->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entity->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entity->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entity->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entity->forceSpawn = true;
	return entity;
}

void S2CEntityPacketHandles::updateEntityBirdAction(std::shared_ptr<S2CPacketSyncEntityAction>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityBird*>(dataCache->getEntityByServerId(packet->entityId));
	if (!entity)
	{
		LordLogError("entityBird does not exist, id: %d", packet->entityId);
		return;
	}
	if (entity->getBaseAction() == BIRD_AS_ATTACK)
		entity->setBaseAction((BirdActionState)packet->preBaseAction);
	entity->setBaseAction((BirdActionState)packet->actionStatus);
	entity->changeBirdAction((BirdActionState)packet->actionStatus);
}

Entity* S2CEntityPacketHandles::createEntityFireball(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityFireball) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return nullptr;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityFireball* entityFireball = LordNew EntityFireball(world, shooter);
	entityFireball->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityFireball->setHeadingFromThrower(shooter, shooter->rotationPitch, shooter->rotationYaw, 0.0F, 1.5F, 1.0F);
	entityFireball->setVelocity(packet->m_motion);
	return entityFireball;
}

Entity* S2CEntityPacketHandles::createEntityBlockman(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	EntityBlockmanEmpty* blockman = LordNew EntityBlockmanEmpty(world, packet->m_pos, packet->m_yaw, true);
	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	blockman->prevPos = blockman->lastTickPos = packet->m_pos;
	blockman->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	blockman->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	blockman->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	blockman->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	blockman->forceSpawn = true;
	return blockman;
}

Entity* S2CEntityPacketHandles::createEntityFishHook(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* angler = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!angler) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityFishHookClient) can not find angler, anglerID[%d]", packet->m_throwerID);
		return nullptr;
	}

	Vector3 originalPos = angler->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityFishHookClient* entityFishHook = LordNew EntityFishHookClient(world, angler);
	entityFishHook->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityFishHook->setPosition(packet->m_pos);
	entityFishHook->setHeadingFromThrower(angler, angler->rotationPitch, angler->rotationYaw, 0.0f, 2.0f, 1.0f);
	entityFishHook->setVelocity(packet->m_motion);

	angler->m_fishEntity = entityFishHook;
	return entityFishHook;
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	/*LordLogInfo("recv S2CEntitySpawnCommonPacket, entityId[%d], type[%d], throwId[%d]",
	packet->m_entityID, packet->m_type, packet->m_throwerID);*/
	Entity * entity = nullptr;
	switch (packet->m_type)
	{
	case ENTITY_CREATE_TYPE_BOAT:
		LordLogError("recv S2CEntitySpawnCommonPacket with unknown type[%d]", packet->m_type);
		//TODO
		break;
	case ENTITY_CREATE_TYPE_ARROE:
		entity = S2CEntityPacketHandles::createEntityArrow(packet);
		break;
	case ENTITY_CREATE_TYPE_BULLET:
		entity = S2CEntityPacketHandles::createEntityBullet(packet);
		break;
	case ENTITY_CREATE_TYPE_VEHICLE:
		entity = S2CEntityPacketHandles::createEntityVehicle(packet);
		break;
	case ENTITY_CREATE_TYPE_POTION:
		entity = S2CEntityPacketHandles::createEntityPotion(packet);
		break;
	case ENTITY_CREATE_TYPE_TNT_PRIMED:
		entity = S2CEntityPacketHandles::createEntityTNTPrimed(packet);
		break;
	case ENTITY_CREATE_TYPE_SNOWBALL:
		entity = S2CEntityPacketHandles::createEntitySnowball(packet);
		break;
	case ENTITY_CREATE_TYPE_GRENADE:
		entity = S2CEntityPacketHandles::createEntityGrenade(packet);
		break;
	case ENTITY_CREATE_TYPE_TNT_THROWABLE:
		entity = S2CEntityPacketHandles::createEntityTNTThrowable(packet);
		break;
	case ENTITY_CREATE_TYPE_MERCHANT:
		entity = S2CEntityPacketHandles::createEntityMerchant(packet);
		break;
	case ENTITY_CREATE_TYPE_RANK_NPC:
		entity = S2CEntityPacketHandles::createEntityRank(packet);
		break;
	case ENTITY_CREATE_TYPE_AIRCRAFT:
		entity = S2CEntityPacketHandles::createEntityAircraft(packet);
		break;
	case ENTITY_CREATE_TYPE_ACTOR_NPC:
		entity = S2CEntityPacketHandles::createEntityActortNpc(packet);
		break;
	case ENTITY_CREATE_TYPE_SESSION_NPC:
		entity = S2CEntityPacketHandles::createEntitySessionNpc(packet);
		break;
	case ENTITY_CREATE_TYPE_CREATURE:
		entity = S2CEntityPacketHandles::createEntityCreature(packet);
		break;
	case ENTITY_CREATE_TYPE_FIREBALL:
		entity = S2CEntityPacketHandles::createEntityFireball(packet);
		break;
	case ENTITY_CREATE_TYPE_BLOCKMAN:
	case ENTITY_CREATE_TYPE_BLOCKMAN_EMPTY:
		entity = S2CEntityPacketHandles::createEntityBlockman(packet);
		break;
	case ENTITY_CREATE_TYPE_FISHHOOK:
		entity = S2CEntityPacketHandles::createEntityFishHook(packet);
		break;
	case ENTITY_CREATE_TYPE_BUILD_NPC:
		entity = S2CEntityPacketHandles::createEntityBuildNpc(packet);
		break;
	case ENTITY_CREATE_TYPE_LAND_NPC:
		entity = S2CEntityPacketHandles::createEntityLandNpc(packet);
		break; 
	case ENTITY_CREATE_TYPE_BULLETIN:
		entity = S2CEntityPacketHandles::createEntityBulletin(packet);
		break; 
	case ENTITY_CREATE_TYPE_BIRD:
		entity = S2CEntityPacketHandles::createEntityBird(packet);
		break;	
	case ENTITY_CREATE_TYPE_DECORATION:
		entity = S2CEntityPacketHandles::createEntityDecoration(packet);
		break;
	default:
		LordLogError("recv S2CEntitySpawnCommonPacket with unknown type[%d]", packet->m_type);
		return;
	}
	if (entity == nullptr)
		return;
	entity->setCustomProperties(packet->m_custom_properties);
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	dataCache->addClientServerIdPair(entity->entityId, packet->m_entityID);
	world->spawnEntityInWorld(entity);
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityHeadRotation>& packet)
{

	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityID);

	if (pEntity)
	{
		//float headYaw = packet->m_headYaw * 360.f / 256.0F;
		float headYaw = packet->m_headYaw;
		//LordLogInfo("PacketEntityHeadRotation id[%d] headYaw(%f)", packet->m_entityID, headYaw);
		pEntity->setRotationYawHead(headYaw);
	}
}


Entity* S2CEntityPacketHandles::createEntitySessionNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entitySessionNpc = LordNew EntitySessionNpc(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	// float yaw = packet->m_yaw * 360.f / 256.0F;
	// float pitch = packet->m_pitch * 360.f / 256.0F;
	float yaw = packet->m_yaw;
	float pitch = packet->m_pitch;
	entitySessionNpc->prevPos = entitySessionNpc->lastTickPos = packet->m_pos;
	entitySessionNpc->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entitySessionNpc->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entitySessionNpc->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entitySessionNpc->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entitySessionNpc->forceSpawn = true;
	return entitySessionNpc;
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityPlayerTeleport>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	Vector3i relativePos(packet->m_x, packet->m_y, packet->m_z);
	auto toPos = TeleportPositionConverter::ConvertToAbsoluteAndUpdateAnchor(packet->m_entityId, relativePos);

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityId);
	if (!pEntity) {
		return;
	}

	float yaw = pEntity->rotationYaw;
	float pitch = pEntity->rotationPitch;
	pEntity->serverPos.x = (int)(toPos.x * 32);
	pEntity->serverPos.y = (int)(toPos.y * 32);
	pEntity->serverPos.z = (int)(toPos.z * 32);
	if (dynamic_cast<EntityPlayerSPMP*>(pEntity))
	{
		toPos.y = toPos.y + pEntity->yOffset + 1 / 32.0f; // 1 / 32.0f is for the deviation of float, prevent the player dropping down the block
		pEntity->setPositionAndRotation(toPos, yaw, pitch);
	}
	else if (auto otherPlayer = dynamic_cast<EntityOtherPlayerMP*>(pEntity))
	{
		toPos.y = toPos.y + 1 / 32.0f;
		yaw = otherPlayer->getNextYaw();
		pitch = otherPlayer->getNextPitch();
		pEntity->setPositionAndRotation2(toPos, yaw, pitch, 3);
	}
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnCreatureBullet>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* thrower = dataCache->getEntityByServerId(packet->throwerId);
	Entity* targeter = dataCache->getEntityByServerId(packet->targetId);
	if (thrower == nullptr || targeter == nullptr)
		return;
	EntityCreatureBullet* pBullet = LordNew EntityCreatureBullet(world, dataCache->getClientId(packet->throwerId), dataCache->getClientId(packet->targetId));
	pBullet->setYawY(packet->yawY);
	pBullet->setSpeedFactor(packet->speedFactor);
	//EntityCreatureBullet* pBullet = LordNew EntityCreatureBullet(world, thrower, targeter, CreatureBulletType(packet->type),
	//packet->bombEffect, packet->speedFactor);
	//pBullet->setBombEffect(packet->bombEffect);
	dataCache->addClientServerIdPair(pBullet->entityId, packet->entityId);
	world->spawnEntityInWorld(pBullet);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityCreature>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto entityCreature = dynamic_cast<EntityCreature*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!entityCreature)
	{
		LordLogError("entityCreature does not exist, id: %d", entityId);
		return;
	}
	entityCreature->setActorName(packet->actorName);
	entityCreature->setMonsterType((CRETURE_AI_TYPE)packet->creatureEntityType);
	entityCreature->setNameLang(packet->nameLang);
	entityCreature->setCanFly(packet->canFly);
	entityCreature->m_keepJumping = packet->keepJumping;
	entityCreature->onGround = packet->onGround;
	EntityRenderManager::Instance()->refreshEntityActor(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId), entityCreature->getActorName());
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncCreatureEntityAction>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto entityCreature = dynamic_cast<EntityCreature*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!entityCreature)
	{
		LordLogError("entityCreature does not exist, id: %d", packet->entityId);
		return;
	}

	if (entityCreature->getBaseAction() == CREATURE_AS_ATTACK)
		entityCreature->setBaseAction((CreatureActionState)packet->preBaseAction);

	entityCreature->setBaseAction((CreatureActionState)packet->actionStatus);

	entityCreature->changeCreatureAction((CreatureActionState)packet->actionStatus);
	if (entityCreature->getCreatureAction() == CREATURE_AS_BE_PLAYER_ATTACK)
		entityCreature->setHurtTime(5);
}

void  S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpwanSkillItemThrowable>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->throwerId);
	EntityLivingBase* thrower = dynamic_cast<EntityLivingBase*>(dataCache->getEntityByServerId(packet->throwerId));
	if (!thrower)
		return;
	SkillItem* pSkillItem = LogicSetting::Instance()->getSkillItem(packet->skillId);
	if (pSkillItem)
	{
		EntitySkillThrowable* entitySkillThrowable = LordNew EntitySkillThrowable(world, thrower);
		entitySkillThrowable->setHeadingFromThrower(thrower, thrower->rotationPitch, thrower->rotationYaw, 0.0, 2.0, 0.0);
		entitySkillThrowable->setPosition(packet->pos);
		entitySkillThrowable->setSkillId(pSkillItem->SkillId);
		entitySkillThrowable->setRenderActor(pSkillItem->IsActor);
		entitySkillThrowable->setSkillDuration(pSkillItem->SkillDuration);
		entitySkillThrowable->setReleaseDistance(pSkillItem->ItemSkillDistace);
		entitySkillThrowable->setMoveSpeed(pSkillItem->MoveSpeed);
		entitySkillThrowable->setDropSpeed(pSkillItem->DropSpeed);
		entitySkillThrowable->setIsParabola(pSkillItem->Parabola);
		entitySkillThrowable->setAtlasName(pSkillItem->AtlasName);
		entitySkillThrowable->setItemIconName(pSkillItem->ItemIconName);
		entitySkillThrowable->setItemSkillId(pSkillItem->ItemId);
		entitySkillThrowable->setEffectName(pSkillItem->EffectName);
		entitySkillThrowable->setNotCustomSpeed(packet->notCustomSpeed);
		entitySkillThrowable->setVelocity(packet->motion);
		world->spawnEntityInWorld(entitySkillThrowable);
	}
}

void  S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketActorNpcContent>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityActorNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (!entity)
		return;
	entity->setContent(packet->content);
}

void  S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketActorNpcSuspended>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityActorNpc = dynamic_cast<EntityActorNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (entityActorNpc)
	{
		entityActorNpc->setSuspended(packet->suspended, packet->pos);
		return;
	}
	
	auto entitySessionNpc = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (entitySessionNpc)
	{
		entitySessionNpc->setSuspended(packet->suspended, packet->pos);
		return;
	}
}

void  S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSessionNpcHeadInfoMultiLang>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (!entity)
		return;

	entity->setHeadNameMulTickInfo(packet->msg, packet->lv, packet->time * 1000);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityDecoration>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityDecoration*>(dataCache->getEntityByServerId(packet->entityId));
	if (entity)
	{
		entity->m_userId = packet->userId;
		entity->setDecorationId(packet->decorationId);
		entity->setPlayAction(packet->isPlayAction);
		for (int i = 0 ; i < packet->nodeSize; i++)
		{
			entity->changeNode(packet->nodeKeys[i], packet->nodeValues[i]);
		}
		Blockman::Instance()->getWorld()->changeEntityActor(entity, entity->getActorName());
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnFlyingMount>& packet)
{
	auto world = Blockman::Instance()->getWorld();
	// TODO: use FlyingMountOption
	auto entity = LordNew EntityFlyingMountClient(world, packet->position, packet->mountSettingId);

	Quaternion orient;
	orient.w = packet->bodyOrientW;
	orient.x = packet->bodyOrientX;
	orient.y = packet->bodyOrientY;
	orient.z = packet->bodyOrientZ;
	entity->setOrientation(orient);

	orient.w = packet->viewOrientW;
	orient.x = packet->viewOrientX;
	orient.y = packet->viewOrientY;
	orient.z = packet->viewOrientZ;
	entity->setViewOrientation(orient);
	entity->forceSpawn = true;
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	if (auto player = dynamic_cast<EntityPlayer*>(dataCache->getEntityByServerId(packet->riderId)))
	{
		player->getOnFlyingMount(entity);
	}
	dataCache->addClientServerIdPair(entity->entityId, packet->entityId);
	world->spawnEntityInWorld(entity);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketFlyingMountMovement>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityFlyingMount*>(dataCache->getEntityByServerId(packet->entityId));
	if (!entity)
		return;
	Quaternion orient;
	entity->setPosition(packet->position);

	orient.w = packet->bodyOrientW;
	orient.x = packet->bodyOrientX;
	orient.y = packet->bodyOrientY;
	orient.z = packet->bodyOrientZ;
	entity->setOrientation(orient);

	orient.w = packet->viewOrientW;
	orient.x = packet->viewOrientX;
	orient.y = packet->viewOrientY;
	orient.z = packet->viewOrientZ;
	entity->setViewOrientation(orient);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnFloatingMount>& packet)
{
	auto world = Blockman::Instance()->getWorld();
	auto entity = LordNew EntityFloatingMountClient(world, packet->position, packet->mountSettingId);
	entity->rotationYaw = packet->bodyYaw;
	entity->rotationPitch = packet->pitch;
	entity->forceSpawn = true;
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	if (auto player = dynamic_cast<EntityPlayer*>(dataCache->getEntityByServerId(packet->riderId)))
	{
		player->getOnFloatingMount(entity);
		player->rotationYaw = packet->viewYaw;
		player->rotationPitch = packet->pitch;
	}
	dataCache->addClientServerIdPair(entity->entityId, packet->entityId);
	world->spawnEntityInWorld(entity);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketFloatingMountMovement>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityFloatingMount*>(dataCache->getEntityByServerId(packet->entityId));
	if (!entity)
		return;
	entity->rotationYaw = packet->bodyYaw;
	entity->rotationPitch = packet->pitch;
	if (auto rider = entity->getRider())
	{
		rider->rotationYaw = packet->viewYaw;
		rider->rotationPitch = packet->pitch;
	}
	entity->setPosition(packet->position);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEnableMovement>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto player = dynamic_cast<EntityPlayer*>(dataCache->getPlayerByServerId(packet->entityId));
	if (!player)
		return;
	player->enableMovement();
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityBlockmanInfo>& packet)
{
	LordLogInfo("S2CPacketSyncEntityBlockmanInfo blockmanType  = %d ", packet->blockmanType);
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto blockman = dynamic_cast<EntityBlockman*>(dataCache->getEntityByServerId(packet->entityId));
	if (blockman)
	{
		blockman->setNameLang(packet->nameLang);
		blockman->setActorName(packet->actorName);
		blockman->setActorBody(packet->actorBody);
		blockman->setActorBodyId(packet->actorBodyId);
		blockman->setCurrentMaxSpeed(packet->maxSpeed);
		BLOCKMAN::Blockman::Instance()->getWorld()->changeEntityActor(blockman, packet->actorName);
		if (auto entity = dynamic_cast<EntityBlockmanEmpty*>(blockman))
		{
			entity->setCurSpeed(packet->curSpeed);
			entity->setSpeedAddtion(packet->addSpeed);
			entity->setNeedRecordRoute(packet->isRecordRoute);
		}
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketBlockmanMoveAttr>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto blockman = dynamic_cast<EntityBlockmanEmpty*>(dataCache->getEntityByServerId(packet->entityId));
	if (blockman)
	{
		blockman->pushX = packet->pushX;
		blockman->pushZ = packet->pushZ;
		blockman->motion = packet->motion;
		blockman->setPositionAndRotation2(packet->position, packet->yaw, blockman->rotationPitch, 3);
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityFrozenTime>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	auto player = dynamic_cast<EntityPlayer*>(entity);
	if (player)
	{
		if (player == BLOCKMAN::Blockman::Instance()->m_pPlayer)
		{
			BLOCKMAN::Blockman::Instance()->m_playerControl->setDisableMovement(packet->time > 0);
		}
		player->setOnFrozen(packet->time);
		return;
	}
	auto blockman = dynamic_cast<EntityBlockmanEmpty*>(entity);
	if (blockman)
	{
		blockman->setOnFrozen(packet->time);
		return;
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityPlayerTeleportWithMotion>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityId);
	if (!pEntity)
	{
		return;
	}

	pEntity->serverPos.x = (int)(packet->m_x * 32);
	pEntity->serverPos.y = (int)(packet->m_y * 32);
	pEntity->serverPos.z = (int)(packet->m_z * 32);

	pEntity->motion.x = packet->m_mx;
	pEntity->motion.y = packet->m_my;
	pEntity->motion.z = packet->m_mz;
	pEntity->onGround = packet->m_isOnGround;

	float x = packet->m_x;
	float y = packet->m_y;
	float z = packet->m_z;
	float yaw = pEntity->rotationYaw;
	float pitch = pEntity->rotationPitch;

	if (dynamic_cast<EntityPlayerSPMP*>(pEntity))
	{
		y = packet->m_y + pEntity->yOffset + 1 / 32.0f; // 1 / 32.0f is for the deviation of float, prevent the player dropping down the block
	}
	else if (auto otherPlayer = dynamic_cast<EntityOtherPlayerMP*>(pEntity))
	{
		y = packet->m_y + 1 / 32.0f;
		yaw = otherPlayer->getNextYaw();
		pitch = otherPlayer->getNextPitch();
	}
	Vector3 toPos = Vector3(x, y, z);
	pEntity->setPositionAndRotation(toPos, yaw, pitch);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnCannon>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityActorCannon = LordNew EntityActorCannonClient(world, packet->m_pos, packet->m_fallOnPos);

	entityActorCannon->prevPos = entityActorCannon->lastTickPos = packet->m_pos;
	entityActorCannon->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityActorCannon->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityActorCannon->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityActorCannon->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	entityActorCannon->forceSpawn = true;
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entityActorCannon->entityId, packet->m_entityID);
	world->spawnEntityInWorld(entityActorCannon);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityBuildNpc>& packet)
{
	LordLogInfo("S2CPacketSyncEntityBuildNpc actorId  = %d ", packet->m_actorId);
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto buildNpc = dynamic_cast<EntityBuildNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (buildNpc)
	{
		buildNpc->m_userId = packet->m_userId;
		buildNpc->setActorId(packet->m_actorId);
		buildNpc->m_maxQueueNum = packet->m_maxQueueNum;
		buildNpc->m_productCapacity = packet->m_productCapacity;
		buildNpc->m_queueUnlockPrice = packet->m_queueUnlockPrice;
		buildNpc->m_queueUnlockCurrencyType = packet->m_queueUnlockCurrencyType;

		buildNpc->setRecipes(packet->m_recipes);
		buildNpc->setProducts(packet->m_products);
		buildNpc->setUnlockQueues(packet->m_unlockQueues);
		buildNpc->m_isDataChange = true;

		BLOCKMAN::Blockman::Instance()->getWorld()->changeEntityActor(buildNpc, buildNpc->getActorName());

		if (buildNpc->getBuildType() == BuildType::BUILD_FARM)
		{
			RanchBuildFarmUpdateEvent::emit(buildNpc->entityId);
		}

		if (buildNpc->getBuildType() == BuildType::BUILD_PLANT)
		{
			RanchBuildFactoryUpdateEvent::emit(buildNpc->entityId);
		}
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityLandNpc>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto landNpc = dynamic_cast<EntityLandNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (landNpc)
	{
		LordLogInfo("S2CPacketSyncEntityLandNpc entityId  = %d ", packet->entityId);
		landNpc->setName(packet->name);
		landNpc->setNameLang(packet->nameLang);
		landNpc->setActorName(packet->actorName);
		landNpc->setActorBody(packet->actorBody);
		landNpc->setActorBodyId(packet->actorBodyId);
		landNpc->setStatus(packet->status);
		landNpc->setTimeLeft(packet->timeLeft);
		landNpc->setRecipe(packet->recipe);
		landNpc->setReward(packet->reward);
		landNpc->setUserId(packet->userId);
		landNpc->setLandCode(packet->landCode);
		landNpc->setPrice(packet->price);
		landNpc->setTotalTime(packet->totalTime);
		BLOCKMAN::Blockman::Instance()->getWorld()->changeEntityActor(landNpc, landNpc->getActorName());
	}

}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityBulletin>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityBulletin = dynamic_cast<EntityBulletin*>(dataCache->getEntityByServerId(packet->entityId));
	if (entityBulletin)
	{
		entityBulletin->setRanks(packet->ranks);
		entityBulletin->setRank(packet->userRank);
		entityBulletin->setNameLang(packet->name);
		entityBulletin->setBulletinId( packet->bulletinId);
		Blockman::Instance()->getWorld()->changeEntityActor(entityBulletin, entityBulletin->getActorName());
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityHurtTime>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	auto player = dynamic_cast<EntityPlayer*>(entity);
	if (player)
	{
		player->setOnHurt(packet->time);
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityColorfulTime>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	auto player = dynamic_cast<EntityPlayer*>(entity);
	if (player)
	{
		player->setOnColorful(packet->time);
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntitySelected>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	if (entity)
		entity->setOnSelected(packet->type);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityDisableMoveTime>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	auto player = dynamic_cast<EntityPlayer*>(entity);
	if (player)
	{
		if (player == Blockman::Instance()->m_pPlayer)
		{
			Blockman::Instance()->m_playerControl->setDisableMovement(true);
		}
		player->setDisableMove(packet->time);
		return;
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityBird>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityBird*>(dataCache->getEntityByServerId(packet->entityId));
	if (entity)
	{
		LordLogInfo("S2CPacketSyncEntityBird entityId  = %d ", packet->entityId);
		entity->m_userId = packet->userId;
		entity->m_birdId = packet->birdId;
		entity->setNameLang(packet->nameLang);
		entity->setActorName(packet->actorName);
		entity->setActorBody(packet->actorBody);
		entity->setActorBodyId(packet->actorBodyId);
		entity->setDressGlasses(packet->dressGlasses);
		entity->setDressHat(packet->dressHat);
		entity->setDressBeak(packet->dressBeak);
		entity->setDressWing(packet->dressWing);
		entity->setDressTail(packet->dressTail);
		entity->setDressEffect(packet->dressEffect);
		Blockman::Instance()->getWorld()->changeEntityActor(entity, entity->getActorName());
	}

}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityAction>& packet)
{
	switch (packet->entityClass)
	{
	case ENTITY_CLASS_BIRD:
	case ENTITY_CLASS_BIRDAI:
		updateEntityBirdAction(packet);
		break;
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityHealth>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityLivingBase*>(dataCache->getEntityByServerId(packet->entityId));
	if (!entity)
	{
		LordLogError("S2CPacketSyncEntityHealth entity does not exist, id: %d", packet->entityId);
		return;
	}
	entity->setEntityMaxHealth(packet->maxHealth);
	entity->setEntityHealth(packet->health);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangeGuideArrowStatus>& packet)
{
	if (packet->flag)
	{
		Blockman::Instance()->m_pPlayer->addGuideArrow(Vector3(packet->arrowPos[0], packet->arrowPos[1], packet->arrowPos[2]));
	}
	else
	{
		Blockman::Instance()->m_pPlayer->deleteGuideArrow(Vector3(packet->arrowPos[0], packet->arrowPos[1], packet->arrowPos[2]));
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketAddWallText>& packet)
{
	Blockman::Instance()->m_globalRender->addWallText(packet->type, packet->timeLeft, packet->text, Vector3(packet->textPos[0], packet->textPos[1], packet->textPos[2]), 
		packet->scale, packet->yaw, packet->pitch, packet->r, packet->g, packet->b, packet->a);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketDeleteWallText>& packet)
{
	Blockman::Instance()->m_globalRender->deleteWallText(Vector3(packet->textPos[0], packet->textPos[1], packet->textPos[2]));
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncCreatureTargetId>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->EntityId);
	auto entityCreature = dynamic_cast<EntityCreature*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!entityCreature)
	{
		LordLogError("entityCreature does not exist, id: %d", entityId);
		return;
	}
	entityCreature->m_targetEntityId = dataCache->getClientId(packet->TargetId);
	//EntityRenderManager::Instance()->refreshEntityActor(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId), entityCreature->getActorName());
	//Blockman::Instance()->m_globalRender->deleteWallText(Vector3(packet->textPos[0], packet->textPos[1], packet->textPos[2]));
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBindEntity>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	int parentId = dataCache->getClientId(packet->parentEntityId);
	int childId = dataCache->getClientId(packet->childEntityId);
	Entity* parentEntity = Blockman::Instance()->getWorld()->getEntity(parentId);
	Entity* childEntity = Blockman::Instance()->getWorld()->getEntity(childId);
	if (!parentEntity || !childEntity)
		return;

	childEntity->bindParentEntity(parentId, packet->nodeName, packet->idleActionId);

	SCRIPT_EVENT::SyncBindEntityEvent::invoke(parentEntity, childEntity);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncUnbindEntity>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	int childId = dataCache->getClientId(packet->childEntityId);
	Entity* childEntity = Blockman::Instance()->getWorld()->getEntity(childId);
	if (!childEntity)
		return;

	childEntity->unbindParentEntity();

	SCRIPT_EVENT::SyncUnbindEntityEvent::invoke(childEntity);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityConnection>& packet)
{
	auto pWorld = Blockman::Instance()->getWorld();
	if (!pWorld)
		return;

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	Entity* pEntity = pWorld->getEntity(dataCache->getClientId(packet->myEntityId));
	if (!pEntity)
		return;

	Entity* parentEntity = pWorld->getEntity(dataCache->getClientId(packet->parentEntityId));
	if (parentEntity)
	{
		pEntity->bindParentEntity(parentEntity->entityId, packet->nodeName, packet->idleActionId);
	}
	else
	{
		pEntity->setParentActorConnectiveBone(packet->nodeName);
		pEntity->setIdleActionId(packet->idleActionId);
	}

	for (auto childId : packet->childrenEntityId)
	{
		Entity* childEntity = pWorld->getEntity(dataCache->getClientId(childId));
		if (childEntity)
		{
			childEntity->bindParentEntity(pEntity->entityId, childEntity->getParentActorConnectiveBone(), childEntity->getIdleActionId());
		}
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnEntityItem>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	EntityItem* entityItem = LordNew EntityItem(world, packet->position, LORD::make_shared<ItemStack>(packet->itemId, packet->itemCount, packet->itemMeta));
	if (packet->enchanted)
	{
		// add any enchantment to turn on the rendering of enchantment effect on the item model
		entityItem->getEntityItem()->addEnchantment(Enchantment::protection, 1);
	}
	entityItem->delayBeforeCanPickup = 40;
	entityItem->serverPos.x = int(Math::Floor(packet->position[0] * 32.f));
	entityItem->serverPos.y = int(Math::Floor(packet->position[1] * 32.f));
	entityItem->serverPos.z = int(Math::Floor(packet->position[2] * 32.f));
	entityItem->forceSpawn = true;
	entityItem->motion = packet->motion;

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	dataCache->addClientServerIdPair(entityItem->entityId, packet->entityId);
	world->spawnEntityInWorld(entityItem);
}
