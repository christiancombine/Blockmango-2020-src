#pragma once

#ifndef __DYNAMIC_CAST_SERVER_ENTITY_H__
#define __DYNAMIC_CAST_SERVER_ENTITY_H__

#include "Blockman/Entity/EntityBirdAI.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Blockman/Entity/EntityRobotAI.h"
#include "Blockman/Entity/EntityWalkingDeadAI.h"

using namespace BLOCKMAN;

struct DynamicCastServerEntity
{
	static EntityBirdAI* dynamicCastBirdAI(Entity* entity)
	{
		return dynamic_cast<EntityBirdAI*>(entity);
	}

	static EntityCreatureAI* dynamicCastCreatureAI(Entity* entity)
	{
		return dynamic_cast<EntityCreatureAI*>(entity);
	}

	static EntityPlayerMP* dynamicCastPlayerMP(Entity* entity)
	{
		return dynamic_cast<EntityPlayerMP*>(entity);
	}

	static EntityRobotAI* dynamicCastRobotAI(Entity* entity)
	{
		return dynamic_cast<EntityRobotAI*>(entity);
	}

	static EntityWalkingDeadAI* dynamicCastWalkingDeadAI(Entity* entity)
	{
		return dynamic_cast<EntityWalkingDeadAI*>(entity);
	}
};

#endif