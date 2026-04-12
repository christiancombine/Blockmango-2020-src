#include "EntityWalkingDeadAI.h"
#include "Setting/MonsterSetting.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Util/MacroUtil.h"
#include "Math/Vector3.h"
#include "Network/ServerNetwork.h"
#include "BtTreeController.h"

namespace BLOCKMAN
{
	EntityWalkingDeadAI::EntityWalkingDeadAI(World * pWorld, int monsterId, Vector3 & pos, float yaw, String actorName) : 
		EntityCreatureAI(pWorld, monsterId, pos, yaw, actorName)
	{
		m_keepJumping = false;
		//onGround = true;
		/*length = 1.0f;
		width = 1.0f;
		height = 2.0f;*/
		maximumHomeDistance = 5.0f;
	}

	bool EntityWalkingDeadAI::findCloestPlayerTargetInPatrolDist()
	{
		if (!isDynamicCreature())
			return false;

		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (!pSetting || !pSetting->isAutoAttack)
			return false;

		EntityPlayer* pPlayer = world->getClosestPlayerToEntity(this, pSetting->patrolDistance);
		if (!pPlayer)
			return false;

		if (pPlayer->isDead || pPlayer->isLogout())
		{
			if (pPlayer->entityId == getCurrentTargetEntityId())
			{
				setCurrentTargetEntityId(0, false);
				changeCreatureAction(CreatureActionState::CREATURE_AS_MOVE);
				return false;
			}
		}
		else
		{
			setCurrentTargetEntityId(pPlayer->entityId, false);
			return true;
		}
		return false;
	}

	void EntityWalkingDeadAI::onUpdate()
	{
		EntityCreatureAI::onUpdate();
		checkForceMoveFinish();
		checkReturnState();
	}

	void EntityWalkingDeadAI::moveEntity(const Vector3 & vel)
	{
		/*if (Math::floatEqual(vel.x, 0.0f) && Math::floatEqual(vel.z, 0.0f))
		{
			return;
		}*/
		EntityCreatureAI::moveEntity(vel);
	}

	bool EntityWalkingDeadAI::canBeCollidedWith()
	{
		return EntityCreatureAI::canBeCollidedWith();
	}

	void EntityWalkingDeadAI::setCurrentTargetEntityId(int EntityId, bool revengingTarget)
	{
		if (m_targetEntityId != EntityId)
		{
			auto players=world->getPlayers();
			for (auto item : players)
			{
				auto player = (EntityPlayerMP*)item.second;
				ServerNetwork::Instance()->getSender()->sendCreatureTargetId(
					player->getRaknetID(), EntityId,this->entityId);
			}
			
		}
		EntityCreatureAI::setCurrentTargetEntityId(EntityId, revengingTarget);
	}

	void EntityWalkingDeadAI::doForceMove(const BlockPos& pos, bool bForce/* = false*/)
	{
		if (m_isForceMoving && !bForce)
		{
			return;
		}

		if (pathToEntity != nullptr && pathToEntity->isDestinationSame(LORD::Vector3(pos.x, pos.y, pos.z)))
		{
			return;
		}

		m_forceMoveDest = BlockPos(pos.x, pos.y, pos.z);
		getPathToPos(m_forceMoveDest, 50.0f);

		if (pathToEntity == nullptr)
		{
			return;
		}

		setCurrentTargetEntityId(0, false);
		changeCreatureAction(CREATURE_AS_MOVE);
		setAIActive(false);
		m_isForceMoving = true;
	}

	void EntityWalkingDeadAI::doGoHome()
	{
		if (m_bCannotGoHome || m_isForceMoving)
		{
			return;
		}

		auto homePos = getHomePosition();
		if (pathToEntity != nullptr && pathToEntity->isDestinationSame(LORD::Vector3(homePos.x, homePos.y, homePos.z)))
		{
			return;
		}

		if (getUpdatePathCd(-1000) > 0)
		{
			return;
		}

		m_forceMoveDest = homePos;
		getPathToHome(50.0f);

		setUpdatePathCd(-1000);

		if (pathToEntity == nullptr)
		{
			m_bCannotGoHome = true;
			return;
		}

		setCurrentTargetEntityId(0, false);
		changeCreatureAction(CREATURE_AS_MOVE);
		setAIActive(false);
		m_isForceMoving = true;
	}

	bool EntityWalkingDeadAI::loadAgent()
	{
		if (m_pAgentPlayer)
		{
			return true;
		}

		m_pAgentPlayer = behaviac::Agent::Create<CommonAgent>();
		m_btTreeId = BtTreeController::Instance()->addBtTreeFromAgent(m_sLoadBTTree.c_str(), m_pAgentPlayer, 1.0f);
		m_pAgentPlayer->setEntity(this);
		return m_btTreeId > 0;
	}

	void EntityWalkingDeadAI::collideWithNearbyEntities()
	{
		//EntityCreatureAI::collideWithNearbyEntities();
	}

	bool EntityWalkingDeadAI::checkForceMoveFinish(int distance)
	{
		/*if (!m_isForceMoving)
		{
			return false;
		}*/

		if (getDistanceSq(Vector3(m_forceMoveDest)) <= distance * distance || pathToEntity == nullptr)
		{
			setAIActive(true);
			m_isForceMoving = false;
		}

		return true;
	}

	bool EntityWalkingDeadAI::checkReturnState()
	{
		MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		NULL_RETURN(pSetting, false);

		auto&& curPos = getPosition();
		if (!Math::checkDistanceLessThan(curPos.x, curPos.y, curPos.z,
			homePosition.x, homePosition.y, homePosition.z, pSetting->chaseRange))
		{
			doGoHome();
		}

		return true;
	}
}
