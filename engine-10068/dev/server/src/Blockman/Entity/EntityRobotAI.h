#pragma once

#include "EntityCreatureAI.h"

namespace BLOCKMAN
{
	class EntityRobotAI final : public EntityCreatureAI
	{
	public:
		EntityRobotAI(World* pWorld, int monsterId, Vector3& pos, float yaw, String actorName);
		~EntityRobotAI() = default;

	private:
		virtual void collideWithNearbyEntities() override;
	};
}