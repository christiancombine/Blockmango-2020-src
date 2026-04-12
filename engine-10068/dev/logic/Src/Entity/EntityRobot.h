#pragma once

#include "EntityCreature.h"

namespace BLOCKMAN
{
	class EntityRobot final : public EntityCreature
	{
	public:
		RTTI_DECLARE(EntityRobot);

		EntityRobot(World* pWorld, int monsterId, Vector3& pos, float yaw, String actorName);
		~EntityRobot() = default;
	};
}