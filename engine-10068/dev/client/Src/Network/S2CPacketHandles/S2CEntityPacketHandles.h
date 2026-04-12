#pragma once

#ifndef __S2CENTITYPACKETHANDLES_H__
#define __S2CENTITYPACKETHANDLES_H__

#include "Network/protocol/S2CPackets.h"
#include "Entity/Entity.h"

using namespace BLOCKMAN;

class S2CEntityPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketRemoveEntity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityMovement>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityTeleport>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityVelocity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityHeadRotation>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnExpOrb>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnMob>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnPlayer>& packet);
	static void handlePacket(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAddEntityEffect>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketRemoveEntityEffect>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityPlayerTeleport>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnCreatureBullet>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncCreatureEntityAction>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityCreature>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpwanSkillItemThrowable>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketActorNpcContent>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEnableMovement>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityBlockmanInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBlockmanMoveAttr>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityFrozenTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityPlayerTeleportWithMotion>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnCannon>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityBuildNpc>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityLandNpc>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityHurtTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityColorfulTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntitySelected>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityDisableMoveTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityBulletin>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityBird>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityAction>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityHealth>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangeGuideArrowStatus>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAddWallText>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketDeleteWallText>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketActorNpcSuspended>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSessionNpcHeadInfoMultiLang>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityDecoration>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncCreatureTargetId>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBindEntity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncUnbindEntity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityConnection>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnFlyingMount>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketFlyingMountMovement>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnFloatingMount>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketFloatingMountMovement>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnEntityItem>& packet);

private:
	static Entity* createEntityArrow(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityBullet(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityVehicle(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntitySnowball(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityTNTPrimed(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityPotion(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityGrenade(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityTNTThrowable(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityMerchant(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityRank(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityAircraft(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityActortNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntitySessionNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityCreature(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityFireball(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityBlockman(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityFishHook(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityBuildNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityLandNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityBulletin(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityBird(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static Entity* createEntityDecoration(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);

	static void updateEntityBirdAction(std::shared_ptr<S2CPacketSyncEntityAction>& packet);
};

#endif
