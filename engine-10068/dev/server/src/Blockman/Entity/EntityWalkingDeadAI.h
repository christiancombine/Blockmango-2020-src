#pragma once

#include "EntityCreatureAI.h"
#include "World/World.h"

namespace BLOCKMAN
{
	class EntityWalkingDeadAI final : public EntityCreatureAI
	{
		RTTI_DECLARE(EntityWalkingDeadAI);

	public:
		EntityWalkingDeadAI(World* pWorld, int monsterId, Vector3& pos, float yaw, String actorName);
		~EntityWalkingDeadAI() = default;

		virtual bool findCloestPlayerTargetInPatrolDist() override;
		virtual void onUpdate() override;
		virtual void moveEntity(const Vector3& vel) override;
		virtual bool canBeCollidedWith() override;
		//virtual float getAIMoveSpeed() { return 0.1F; }
		virtual void setCurrentTargetEntityId(int EntityId, bool revengingTarget) override;
		void doForceMove(const BlockPos& pos, bool bForce = false);
		void doGoHome();

	protected:
		virtual bool loadAgent() override;
		virtual void collideWithNearbyEntities() override;
	private:
		//检测强制移动结束
		bool checkForceMoveFinish(int distance = 2);

		//检测回场状态
		bool checkReturnState();
	private:
		bool m_isForceMoving{false}; //是否强制移动
		BlockPos m_forceMoveDest; //强制移动目的地
		bool m_bCannotGoHome{ false }; //是否能回城
	};
}