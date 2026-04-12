#ifndef __DYNAMIC_CAST_CLIENT_ENTITY_H__
#define __DYNAMIC_CAST_CLIENT_ENTITY_H__

#include "Blockman/cEntity/EntityActorCannonClient.h"
#include "Blockman/cEntity/EntityBulletClient.h"
#include "Blockman/cEntity/EntityFishHookClient.h"
#include "Blockman/cEntity/EntityFlyingMountClient.h"
#include "Blockman/cEntity/EntityGunFlame.h"
#include "Blockman/cEntity/EntityOtherPlayerMP.h"
#include "Blockman/cEntity/EntityPlayerSP.h"
#include "Blockman/cEntity/EntityPlayerSPMP.h"
#include "Blockman/cEntity/EntityVehicleClient.h"

using namespace BLOCKMAN;

struct DynamicCastClientEntity
{
	static EntityActorCannonClient* dynamicCastActorCannonClient(Entity* entity)
	{
		return dynamic_cast<EntityActorCannonClient*>(entity);
	}

	static EntityBulletClient* dynamicCastBulletClient(Entity* entity)
	{
		return dynamic_cast<EntityBulletClient*>(entity);
	}

	static EntityFishHookClient* dynamicCastFishHookClient(Entity* entity)
	{
		return dynamic_cast<EntityFishHookClient*>(entity);
	}

	static EntityFlyingMountClient* dynamicCastFlyingMountClient(Entity* entity)
	{
		return dynamic_cast<EntityFlyingMountClient*>(entity);
	}

	static EntityGunFlame* dynamicCastGunFlame(Entity* entity)
	{
		return dynamic_cast<EntityGunFlame*>(entity);
	}

	static EntityOtherPlayerMP* dynamicCastOtherPlayerMP(Entity* entity)
	{
		return dynamic_cast<EntityOtherPlayerMP*>(entity);
	}

	static EntityPlayerSP* dynamicCastPlayerSP(Entity* entity)
	{
		return dynamic_cast<EntityPlayerSP*>(entity);
	}

	static EntityPlayerSPMP* dynamicCastPlayerSPMP(Entity* entity)
	{
		return dynamic_cast<EntityPlayerSPMP*>(entity);
	}

	static EntityVehicleClient* dynamicCastVehicleClient(Entity* entity)
	{
		return dynamic_cast<EntityVehicleClient*>(entity);
	}
};


#endif