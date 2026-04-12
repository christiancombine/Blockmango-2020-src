#include "EntityRobotAI.h"

namespace BLOCKMAN
{
	EntityRobotAI::EntityRobotAI(World* pWorld, int monsterId, Vector3& pos, float yaw, String actorName) : 
		EntityCreatureAI(pWorld, monsterId, pos, yaw, actorName)
	{
		m_keepJumping = false;
		onGround = true;
	}

	void EntityRobotAI::collideWithNearbyEntities()
	{
		//
	}
}
