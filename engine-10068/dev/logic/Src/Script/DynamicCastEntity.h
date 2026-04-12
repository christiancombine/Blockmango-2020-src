#ifndef __DYNAMIC_CAST_ENTITY_H__
#define __DYNAMIC_CAST_ENTITY_H__

#include "Entity/EntityActorCannon.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityBird.h"
#include "Entity/EntityBlockman.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityBullet.h"
#include "Entity/EntityBulletin.h"
#include "Entity/EntityCreature.h"
#include "Entity/EntityCreatureBullet.h"
#include "Entity/EntityDecoration.h"
#include "Entity/EntityFireball.h"
#include "Entity/EntityFishHook.h"
#include "Entity/EntityFloatingMount.h"
#include "Entity/EntityFlyingMount.h"
#include "Entity/EntityGrenade.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityLandNpc.h"
#include "Entity/EntityLiving.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityMerchant.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityPotion.h"
#include "Entity/EntityRankNpc.h"
#include "Entity/EntityRobot.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntitySkillThrowable.h"
#include "Entity/EntitySnowball.h"
#include "Entity/EntityThrowable.h"
#include "Entity/EntityTNTPrimed.h"
#include "Entity/EntityTNTThrowable.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityXPOrb.h"

using namespace BLOCKMAN;

struct DynamicCastEntity
{
	static EntityActorCannon* dynamicCastActorCannon(Entity* entity)
	{
		return dynamic_cast<EntityActorCannon*>(entity);
	}

	static EntityActorNpc* dynamicCastActorNpc(Entity* entity)
	{
		return dynamic_cast<EntityActorNpc*>(entity);
	}

	static EntityAircraft* dynamicCastAircraft(Entity* entity)
	{
		return dynamic_cast<EntityAircraft*>(entity);
	}

	static EntityArrow* dynamicCastArrow(Entity* entity)
	{
		return dynamic_cast<EntityArrow*>(entity);
	}

	static EntityBird* dynamicCastBird(Entity* entity)
	{
		return dynamic_cast<EntityBird*>(entity);
	}

	static EntityBlockman* dynamicCastBlockman(Entity* entity)
	{
		return dynamic_cast<EntityBlockman*>(entity);
	}

	static EntityBlockmanEmpty* dynamicCastBlockmanEmpty(Entity* entity)
	{
		return dynamic_cast<EntityBlockmanEmpty*>(entity);
	}

	static EntityBuildNpc* dynamicCastBuildNpc(Entity* entity)
	{
		return dynamic_cast<EntityBuildNpc*>(entity);
	}

	static EntityBullet* dynamicCastBullet(Entity* entity)
	{
		return dynamic_cast<EntityBullet*>(entity);
	}

	static EntityBulletin* dynamicCastBulletin(Entity* entity)
	{
		return dynamic_cast<EntityBulletin*>(entity);
	}

	static EntityCreature* dynamicCastCreature(Entity* entity)
	{
		return dynamic_cast<EntityCreature*>(entity);
	}

	static EntityCreatureBullet* dynamicCastCreatureBullet(Entity* entity)
	{
		return dynamic_cast<EntityCreatureBullet*>(entity);
	}

	static EntityDecoration* dynamicCastDecoration(Entity* entity)
	{
		return dynamic_cast<EntityDecoration*>(entity);
	}

	static EntityFireball* dynamicCastFireball(Entity* entity)
	{
		return dynamic_cast<EntityFireball*>(entity);
	}

	static EntityFishHook* dynamicCastFishHook(Entity* entity)
	{
		return dynamic_cast<EntityFishHook*>(entity);
	}

	static EntityFloatingMount* dynamicCastFloatingMount(Entity* entity)
	{
		return dynamic_cast<EntityFloatingMount*>(entity);
	}

	static EntityFlyingMount* dynamicCastFlyingMount(Entity* entity)
	{
		return dynamic_cast<EntityFlyingMount*>(entity);
	}

	static EntityGrenade* dynamicCastGrenade(Entity* entity)
	{
		return dynamic_cast<EntityGrenade*>(entity);
	}

	static EntityItem* dynamicCastItem(Entity* entity)
	{
		return dynamic_cast<EntityItem*>(entity);
	}

	static EntityLandNpc* dynamicCastLandNpc(Entity* entity)
	{
		return dynamic_cast<EntityLandNpc*>(entity);
	}

	static EntityLiving* dynamicCastLiving(Entity* entity)
	{
		return dynamic_cast<EntityLiving*>(entity);
	}

	static EntityLivingBase* dynamicCastLivingBase(Entity* entity)
	{
		return dynamic_cast<EntityLivingBase*>(entity);
	}

	static EntityMerchant* dynamicCastMerchant(Entity* entity)
	{
		return dynamic_cast<EntityMerchant*>(entity);
	}

	static EntityPlayer* dynamicCastPlayer(Entity* entity)
	{
		return dynamic_cast<EntityPlayer*>(entity);
	}

	static EntityPotion* dynamicCastPotion(Entity* entity)
	{
		return dynamic_cast<EntityPotion*>(entity);
	}

	static EntityRankNpc* dynamicCastRankNpc(Entity* entity)
	{
		return dynamic_cast<EntityRankNpc*>(entity);
	}

	static EntityRobot* dynamicCastRobot(Entity* entity)
	{
		return dynamic_cast<EntityRobot*>(entity);
	}

	static EntitySessionNpc* dynamicCastSessionNpc(Entity* entity)
	{
		return dynamic_cast<EntitySessionNpc*>(entity);
	}

	static EntitySkillThrowable* dynamicCastSkillThrowable(Entity* entity)
	{
		return dynamic_cast<EntitySkillThrowable*>(entity);
	}

	static EntitySnowball* dynamicCastSnowball(Entity* entity)
	{
		return dynamic_cast<EntitySnowball*>(entity);
	}

	static EntityThrowable* dynamicCastThrowable(Entity* entity)
	{
		return dynamic_cast<EntityThrowable*>(entity);
	}

	static EntityTNTPrimed* dynamicCastTNTPrimed(Entity* entity)
	{
		return dynamic_cast<EntityTNTPrimed*>(entity);
	}

	static EntityTNTThrowable* dynamicCastTNTThrowable(Entity* entity)
	{
		return dynamic_cast<EntityTNTThrowable*>(entity);
	}

	static EntityVehicle* dynamicCastVehicle(Entity* entity)
	{
		return dynamic_cast<EntityVehicle*>(entity);
	}

	static EntityXPOrb* dynamicCastXPOrb(Entity* entity)
	{
		return dynamic_cast<EntityXPOrb*>(entity);
	}

};

#endif