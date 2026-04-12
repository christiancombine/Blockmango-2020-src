#include "EntityCreature.h"
#include "World/World.h"
#include "AI/AITask.h"
#include "Entity/EntityPlayer.h"
#include "Setting/LogicSetting.h"
#include "Setting/MonsterSetting.h"
#include "Setting/SkillSetting.h"
#include "Setting/ActorSizeSetting.h"
#include "Script/Event/LogicScriptEvents.h"
#include "DamageSource.h"
#include "Enchantment.h"
#include "Object/Root.h"
#include "Memory/LordMemory.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "AI/AIMoveToRestriction.h"
#include "Util/PlayerPrefs.h"

namespace BLOCKMAN
{
	void DirectMoveHelper::addMoveTask(const Vector3 & toPos_, bool stopIfCollision)
	{
		this->pEntity->isCollidedHorizontally = false;

		toPos = toPos_;
		needStopIfCollision = stopIfCollision;
		if (pEntity->isFlyEntity())
			pEntity->motion.y = 0.0;

		arrivedXZ = false;
		if (pEntity->isFlyEntity())
			arrivedY = false;
		else
			arrivedY = true;
	}


	void DirectMoveHelper::update()
	{
		if (!arrivedXZ)
		{
			if(needStopIfCollision  || LORD::Root::Instance()->getCurrentTime()-lastChangeYawTime > 2000)
				updateYaw();
			updateXZ();
		}

		if (!arrivedY)
			updateY();
	}

	void DirectMoveHelper::updateYaw()
	{
		int baseY = int(Math::Floor(pEntity->boundingBox.vMin.y + 0.5f));
		auto position = pEntity->position;
		Vector3 dirVec = toPos - Vector3(position.x, float(baseY), position.z);
		directYaw = (Math::ATan2(dirVec.z, dirVec.x) * Math::RAD2DEG) - 90.0F;
	}

	void DirectMoveHelper::updateY()
	{
		if (pEntity->isFlyEntity())
		{
			auto yStep = toPos.y - pEntity->position.y;
			if (yStep != 0 )
			{
				auto setPos = this->pEntity->position;
				if (Math::Abs(yStep) < 0.3)
				{
					setPos.y = toPos.y;
				}
				else
				{
					auto toPosXZ = toPos;
					auto curPosXZ = pEntity->position;

					float nextYStep = Math::Sgn(yStep) *0.2;

					
					setPos.y += nextYStep;
				}

				Vector3 addVec = setPos - this->pEntity->position;
				Box testBox = pEntity->getBoundingBox();
				testBox.addCoord(addVec);
				auto collisionBox = pEntity->world->getCollidingBlockBounds(testBox);
				if (collisionBox.size() == 0)
					this->pEntity->setPosition(setPos);
				else
					arrivedY = true;

				
			}
			else
			{
				arrivedY = true;
			}

		}
	}

	bool DirectMoveHelper::checkCanMoveOnCollisionBlock()
	{
		float speed_attenuation = 1.0F;
		float slipperiness = pEntity->getSlipperiness(speed_attenuation);

		float f = 0.16277136F / (slipperiness * slipperiness * slipperiness);
		float speed = pEntity->getAIMoveSpeed() * f + pEntity->getSpeedAddition();
		speed = speed * speed_attenuation;

		auto k1 = speed / (1 - slipperiness);

		float f1 = Math::Sin(pEntity->rotationYaw * Math::PI / 180.0F);
		float f2 = Math::Cos(pEntity->rotationYaw * Math::PI / 180.0F);
		float dx = -f1 * k1;
		float dz = f2 * k1;

		auto entityBox = pEntity->getBoundingBox();
		auto collisionBox = pEntity->world->getCollidingBlockBounds(entityBox.addCoord(dx, 0, dz));
		if (collisionBox.size() == 0)
			return false;

		for (auto box : collisionBox)
		{
			auto curBlock = box.vMin.getFloor();
			auto upBlock = curBlock.getPosY();
			int curBlockMoveInfo = getBlockMoveInfo(curBlock);
			int upBlockMoveInfo = getBlockMoveInfo(upBlock);
			if (upBlockMoveInfo != 0 || curBlockMoveInfo < 0)
				return false;
		}

		return true;
	}

	void DirectMoveHelper::updateXZ()
	{
		pEntity->moveForward = 0.0;
		auto toPosXZ = toPos;
		auto curPosXZ = pEntity->position;
		toPosXZ.y = 0;
		curPosXZ.y = 0;

		Vector3 dir;
		dir.x = (-Math::Sin(directYaw* Math::DEG2RAD));
		dir.z = (Math::Cos(directYaw *Math::DEG2RAD));
		
		if (toPosXZ.distanceTo(curPosXZ) <= pEntity->detectNextMoveLen() *1.5)
		{
			arrivedXZ = true;
			auto arrivedPos = toPos;
			arrivedPos.y = this->pEntity->position.y;

			Vector3 addVec = arrivedPos - this->pEntity->position;

			Box testBox = pEntity->getBoundingBox();
			testBox.addCoord(addVec);
			auto collisionBox = pEntity->world->getCollidingBlockBounds(testBox);
			if (collisionBox.size() == 0)
				pEntity->setPosition(arrivedPos);
		}


		if (arrivedXZ)
		{
			auto moveLen = pEntity->detectNextMoveLen();
			pEntity->motion.x = 0;
			pEntity->motion.z = 0;
			return;
		}

		float yaw = Math::WrapDegree(directYaw - pEntity->rotationYaw);

		
		if (yaw > 30)
		{
			pEntity->rotationYaw += 30;
		}
		else if (yaw < -30)
		{
			pEntity->rotationYaw -= 30;
		}
		else if (yaw != 0)
		{
			pEntity->rotationYaw = directYaw;
		}
		else
		{
			this->pEntity->setJumping(false);
			if(!this->pEntity->isCollidedHorizontally)
			{
				pEntity->moveForward = 1.0f;
			}
			else if(checkCanMoveOnCollisionBlock())
			{
				pEntity->moveForward = 1.0f;
				if(!pEntity->isFlyEntity())
					pEntity->setJumping(true);
			}
			else if (pEntity->isOnLadder())
			{
				pEntity->moveForward = 1.0f;
			}
			else 
			{
				pEntity->motion.x = 0;
				pEntity->motion.z = 0;
				pEntity->isCollidedHorizontally = false;
				if (!needStopIfCollision)
				{
					changeYaw();
				}
				else
				{
					arrivedXZ = true;
				}
			}
		}
	}

	bool  DirectMoveHelper::isMoveOver()
	{
		return arrivedXZ && arrivedY;
	}

	void DirectMoveHelper::stopMove()
	{
		this->toPos.invalid();
		this->arrivedXZ = true;
		this->arrivedY = true;
	}

	void DirectMoveHelper::changeYaw()
	{
		//float x = -Math::Sin(pEntity->rotationYaw * Math::DEG2RAD);
		//float z = Math::Cos(pEntity->rotationYaw *Math::DEG2RAD);

		//int xDirect = x / Math::Abs(x);
		//int zDirect = z / Math::Abs(z);
		directYaw = pEntity->rotationYaw + 30;
		
		if (directYaw > 180)
			directYaw = 360 - directYaw;
		else if (directYaw < -180)
			directYaw = 360 + directYaw;

		lastChangeYawTime = LORD::Root::Instance()->getCurrentTime();

	}

	int DirectMoveHelper::getBlockMoveInfo(const BlockPos & ipos)
	{
		int blockID = pEntity->world->getBlockId(ipos);
		int rt = 0;

		if (blockID > 0)
		{
			if (blockID == BLOCK_ID_WATERMOVING
				|| blockID == BLOCK_ID_WATERSTILL)
			{
				return -1;
			}

			Block* pBlock = BlockManager::sBlocks[blockID];
			const BM_Material& mat = pBlock->getMaterial();
			if (mat == BM_Material::BM_MAT_lava)
			{
				return -1;
			}

			if (!pBlock->getBlocksMovement(pEntity->world, ipos))
			{
				//means block can through
				return -1;
			}
			else
			{
				return 1;
			}
		}

		return 0;
	}


	bool DirectMoveHelper::canMoveOnBlock(const BlockPos & pos)
	{
		auto downBlockInfo = getBlockMoveInfo(pos.getNegY());
		auto curBlockInfo = getBlockMoveInfo(pos);

		return curBlockInfo == 0 && downBlockInfo > 0;
	}

	bool DirectMoveHelper::checkMoveOnNextBlock()
	{
		if (this->pEntity->isFlyEntity())
		{
			return this->checkMoveOnNextBlockForFlyEntity();
		}
		else
		{
			return this->checkMoveOnNextBlockForWalkEntity();
		}
	}

	bool DirectMoveHelper::checkMoveOnNextBlockForFlyEntity()
	{
		auto nextPos = pEntity->position;
		nextPos.x += (pEntity->detectNextMoveLen() * (-Math::Sin(pEntity->rotationYaw * Math::DEG2RAD)));
		nextPos.z += (pEntity->detectNextMoveLen() * (Math::Cos(pEntity->rotationYaw *Math::DEG2RAD)));
		
		auto yStep = toPos.y - pEntity->position.y;

		auto toPosXZ = toPos;
		auto curPosXZ = pEntity->position;
		toPosXZ.y = 0;
		curPosXZ.y = 0;

		float nextYStep = yStep / (toPosXZ.distanceTo(curPosXZ) / pEntity->detectNextMoveLen());

		nextPos.y += nextYStep;

		if (nextPos.getFloor() == pEntity->position.getFloor())
		{
			return true;
		}

		auto curBlockInfo = getBlockMoveInfo(nextPos.getFloor());

		if (curBlockInfo != 0)
			return false;

		return true;
	}

	bool DirectMoveHelper::checkMoveOnNextBlockForWalkEntity()
	{
		auto nextPos = pEntity->position;

		nextPos.x += (pEntity->detectNextMoveLen()) * (-Math::Sin(pEntity->rotationYaw * Math::DEG2RAD));
		nextPos.z += (pEntity->detectNextMoveLen()) * (Math::Cos(pEntity->rotationYaw *Math::DEG2RAD));

		BlockPos curBlockPos = pEntity->position.getFloor();
		BlockPos nextBlockPos = nextPos.getFloor();

		if (curBlockPos != nextBlockPos)
		{
			auto nextUpBlockPos = nextBlockPos.getPosY();
			auto nextDownBlockPos = nextBlockPos.getNegY();

			if (canMoveOnBlock(nextBlockPos))
			{
				return true;
			}
			else if (canMoveOnBlock(nextDownBlockPos))
			{
				auto nextPos = pEntity->position;
				nextPos.y -= 1;
				pEntity->setPosition(nextPos);
				return true;
			}
			else if (canMoveOnBlock(nextUpBlockPos))
			{
				auto nextPos = pEntity->position;
				nextPos.y += 1;
				pEntity->setPosition(nextPos);
				return true;
			}

			return false;
		}

		return true;
	}

	EntityCreature::EntityCreature(World* pWorld, int monsterId, Vector3& pos, float yaw, String actorName)
		: EntityLiving(pWorld)
		, pathToEntity(NULL)
		, entityToAttack(NULL)
		, hasAttacked(false)
		, fleeingTick(0)
		, homePosition(BlockPos(pos.x, pos.y, pos.z))
		, maximumHomeDistance(-1.f)
		, m_hasTasks(false)
		, m_MonsterId(monsterId)
		, m_teamId(0)
	{
		m_aiMoveTo = LordNew AIMoveToRestriction(this, 1.f);
		setPosition(pos);
		setActorName(actorName);
		rotationYaw = yaw;
		rotationPitch = 0.0f;
		m_prevState = CREATURE_AS_MOVE;
		m_curState = CREATURE_AS_IDLE;
		m_prevBaseAction = CREATURE_AS_MOVE;
		m_baseAction = CREATURE_AS_IDLE;
		m_prevUpperAction = CREATURE_AS_MOVE;
		m_upperAction = CREATURE_AS_IDLE;
		m_curUpdateTime = Root::Instance()->getCurrentTime();
		m_lastUpdateTime = Root::Instance()->getCurrentTime();
		m_isDynamicCreature = checkDynamicCreature();
		setFreeSkillCd();
		m_bornPos = pos;
		m_bornYaw = yaw;
		directMoveHelper = LORD::make_unique<DirectMoveHelper>(this);

		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			this->canFly = pSetting->canFly;
            m_bloodStripOffset = pSetting->bloodStripOffset;
		}
	}

	EntityCreature::~EntityCreature()
	{
		LordSafeDelete(m_aiMoveTo);
		LordSafeDelete(pathToEntity);
	}

	void EntityCreature::updateEntityActionState()
	{
		if (!isDynamicCreature())
			return;

		if (pathToEntity)
		{
			int baseY = int(Math::Floor(boundingBox.vMin.y + 0.5f));
			rotationPitch = 0.0F;
			Vector3 toward = pathToEntity->getPosition(this);
			float range = (width * 2.0F);
			if (range > 0.5)
				range = 1;

			int lastPathIndex = pathToEntity->getCurrentPathIndex();

			while (toward != Vector3::INVALID && toward.squareDistanceTo(Vector3(position.x, toward.y, position.z)) < range * range)
			{
				pathToEntity->incrementPathIndex();
				if (pathToEntity->isFinished())
				{
					toward = Vector3::INVALID;
					LordSafeDelete(pathToEntity);
				}
				else
				{
					toward = pathToEntity->getPosition(this);
					break;
				}
			}

			if (pathToEntity && lastPathIndex == pathToEntity->getCurrentPathIndex())
			{
				++m_incPathIndexFailedCount;
				if (m_incPathIndexFailedCount >= 10)
				{
					pathToEntity->incrementPathIndex();
					if (pathToEntity->isFinished())
					{
						toward = Vector3::INVALID;
						LordSafeDelete(pathToEntity);
					}
					else
					{
						toward = pathToEntity->getPosition(this);
					}
					m_incPathIndexFailedCount = 0;
				}
			}

			isJumping = false;

			if (toward != Vector3::INVALID)
			{
				Vector3 dirVec = toward - Vector3(position.x, float(baseY), position.z);
				float yaw = (Math::ATan2(dirVec.z, dirVec.x) * Math::RAD2DEG) - 90.0F;
				yaw = Math::WrapDegree(yaw - rotationYaw);
				//moveForward = getEntityAttribute(SharedMonsterAttributes::MOVEMENT_SPEED)->getAttributeValue();
				moveForward = 1.0f;
				if (yaw > 30.0F)
					yaw = 30.0F;
				if (yaw < -30.0F)
					yaw = -30.0F;

				rotationYaw += yaw;


				if (hasAttacked && entityToAttack)
				{
					float dx = entityToAttack->position.x - position.x;
					float dz = entityToAttack->position.z - position.z;
					float tempYaw = rotationYaw;
					rotationYaw = Math::ATan2(dz, dx)  * Math::RAD2DEG - 90.0F;
					yaw = (tempYaw - rotationYaw + 90.0F) * Math::DEG2RAD;
					moveStrafing = -Math::Sin(yaw) * moveForward * 1.0F;
					moveForward = Math::Cos(yaw) * moveForward * 1.0F;
				}

				isJumping = (dirVec.y > 0.0f);
			}

			if (entityToAttack)
			{
				faceEntity(entityToAttack, 30.0F, 30.0F);
			}
		}
		else if (!directMoveHelper->isMoveOver())
		{
			directMoveHelper->update();
		}
		else
		{
			EntityLiving::updateEntityActionState();
			LordSafeDelete(pathToEntity);
		}
	}

	EntityPlayer * EntityCreature::findTargetPlayer()
	{
		return world->getPlayerByPlatformUserId(m_userId);
	}

	void EntityCreature::getPathToHome(float mindis/* = 100.0f*/)
	{
		getPathToPos(homePosition, mindis);
	}

	void EntityCreature::getPathToPos(const BlockPos& destPos, float mindis/* = 100.0f*/)
	{
		if (needChangePath(destPos))
		{
			LordSafeDelete(pathToEntity);
			pathToEntity = world->getEntityPathToXYZ(this, destPos, mindis, true, false, false, true);
			//assert(pathToEntity != nullptr);
		}
	}

	void EntityCreature::getPathToEntity(Entity* endEntity, float mindis/* = 100.0f*/)
	{
		if (endEntity && needChangePath(BlockPos(endEntity->getWidelyPos().x, endEntity->getWidelyPos().y, endEntity->getWidelyPos().z)))
		{
			LordSafeDelete(pathToEntity);
			pathToEntity = world->getPathEntityToEntity(this, endEntity, mindis, true, false, false, true);
		}
	}

	bool EntityCreature::getCanSpawnHere()
	{
		BlockPos pos;
		pos.x = int(Math::Floor(position.x));
		pos.y = int(Math::Floor(boundingBox.vMin.y));
		pos.z = int(Math::Floor(position.z));
		return EntityLiving::getCanSpawnHere() && getBlockPathWeight(pos) >= 0.0F;
	}

	void EntityCreature::onUpdate()
	{
		m_curUpdateTime = Root::Instance()->getCurrentTime();
		EntityLiving::onUpdate();
		refreshAttackCdTime();
		refreshFreeSkillCdTime();
		refreshUpdateTargetCdTime();
		refreshUpdatePathCdTime();
		decrHurtTime();
		m_lastUpdateTime = Root::Instance()->getCurrentTime();
	}

	void EntityCreature::onLivingUpdate()
	{
		if (!isDynamicCreature())
			return;
		EntityLiving::onLivingUpdate();
	}

	bool EntityCreature::canBeCollidedWith()
	{
		return PlayerPrefs::Instance()->getBoolPrefs("IsCreatureCollision");
	}

	void EntityCreature::updateLeashedState()
	{
		if (!isDynamicCreature())
			return;
		EntityLiving::updateLeashedState();
		if (getLeashed() && getLeashedToEntity() && getLeashedToEntity()->world == world)
		{
			Entity* pLeashed = getLeashedToEntity();
			setHomePosAndDistance(BlockPos(int(pLeashed->position.x), int(pLeashed->position.y), int(pLeashed->position.z)), 5);
			float dis = getDistanceToEntity(pLeashed);

			if (!m_hasTasks)
			{
				m_tasks->addTask(2, m_aiMoveTo);
				getNavigator().setAvoidsWater(false);
				m_hasTasks = true;
			}

			onLeashDistance(dis);

			if (dis > 4.0F)
			{
				getNavigator().tryMoveToEntityLiving(pLeashed, 1.0f);
			}

			if (dis > 6.0F)
			{
				Vector3 dir = (pLeashed->position - position) / dis;
				Vector3 absDir;
				Vector3::Abs(absDir, dir);
				motion += dir * absDir * 0.4f;
			}

			if (dis > 10.0F)
			{
				clearLeashed(true, true);
			}
		}
		else if (!getLeashed() && m_hasTasks)
		{
			m_hasTasks = false;
			m_tasks->removeTask(m_aiMoveTo);
			getNavigator().setAvoidsWater(true);
			detachHome();
		}
	}

	bool EntityCreature::isWithinHomeDistanceFromPosition(const BlockPos& pos)
	{
		if (maximumHomeDistance == -1.f)
			return true;

		Vector3i disVec = homePosition - pos;
		return disVec.lenSqr() < maximumHomeDistance * maximumHomeDistance;
	}

	CRETURE_AI_TYPE  EntityCreature::getMonsterType()
	{
		CRETURE_AI_TYPE MonsterType = CRETURE_AI_TYPE_NONE;
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			MonsterType = (CRETURE_AI_TYPE)pSetting->monsterType;
		}
		return MonsterType;
	}

	void EntityCreature::setMonsterType(CRETURE_AI_TYPE type)
	{
		m_creatureEntityType = type;
		m_isDynamicCreature = checkDynamicCreature();
	}

	void EntityCreature::setActorName(const String& actorName)
	{
		m_actorName = actorName;
		ActorSize* pSize{nullptr};
		if (ActorSizeSetting::isHaveActor(m_actorName))
		{
			pSize = ActorSizeSetting::getActorSize(m_actorName);
		}
		else
		{
			pSize = ActorSizeSetting::getActorSize("CreatureNpc");
		}
		setScale(pSize->scale);
		setSize(pSize->width * pSize->scale, pSize->length * pSize->scale, pSize->height * pSize->scale);
		setPosition(position);
	}

	String EntityCreature::getMonsterBtTree()
	{
		String MonsterLoadBtTree = "";
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			MonsterLoadBtTree = pSetting->monsterBtTree;
		}
		return MonsterLoadBtTree;
	}

	bool EntityCreature::isBase()
	{
		return m_creatureEntityType == CRETURE_AI_TYPE_BASE;
	}

	bool EntityCreature::isBoss()
	{
		return m_creatureEntityType == CRETURE_AI_TYPE_MONSTER_BOSS;
	}

	bool EntityCreature::isMonster()
	{
		return m_creatureEntityType == CRETURE_AI_TYPE_MONSTER || m_creatureEntityType == CRETURE_AI_TYPE_G1049;
	}

	bool EntityCreature::hasRunAction()
	{
		return m_creatureEntityType == CRETURE_AI_TYPE_G1049;
	}
	bool EntityCreature::hasDeathAction() 
	{
		return m_creatureEntityType != CRETURE_AI_TYPE_G1049;
	}

	float EntityCreature::detectNextMoveLen()
	{
		float speed_attenuation = 1.0F;
		float slipperiness = getSlipperiness(speed_attenuation);

		float f = 0.16277136F / (slipperiness * slipperiness * slipperiness);
		float speed = getAIMoveSpeed() * f + getSpeedAddition();
		speed = speed * speed_attenuation;

		auto k1 = speed / (1 - slipperiness);

		float f1 = Math::Sin(rotationYaw * Math::PI / 180.0F);
		float f2 = Math::Cos(rotationYaw * Math::PI / 180.0F);
		float dx = - f1*speed;
		float dz = f2*speed;
		
		dx += motion.x;
		dz += motion.z;
		auto k2 = Math::Sqrt(dx * dx + dz * dz);

		return k1;

		
	}

	void EntityCreature::playAlertAction(const Vector3& seePos)
	{
		int baseY = int(Math::Floor(boundingBox.vMin.y + 0.5f));
		Vector3 dirVec = seePos - Vector3(position.x, float(baseY), position.z);
		float yaw = (Math::ATan2(dirVec.z, dirVec.x) * Math::RAD2DEG) - 90.0F;
		yaw = Math::WrapDegree(yaw - rotationYaw);
		this->rotationYaw += yaw;
	}

	float EntityCreature::getAttackDistance()
	{
		float nAttackDistance = 16.0f;
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			nAttackDistance = pSetting->attackDistance;
		}
		return nAttackDistance;
	}

	float EntityCreature::getSkillDistance()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (!pSetting)
			return 0.0f;
		const SkillEffect* pSkill = SkillSetting::getSkillEffect(pSetting->skillId);
		if (!pSkill)
			return 0.0f;
		return Math::Sqrt(float(pSkill->width * pSkill->height));
	}

	void EntityCreature::setDead()
	{
		Entity::setDead();
		//TODO
	}

	void EntityCreature::setPosition(const Vector3 & pos)
	{
		EntityLiving::setPosition(pos);
	}

	bool EntityCreature::changeCreatureAction(CreatureActionState nActionState)
	{
		if (getCreatureAction() == CREATURE_AS_DEATH)
			return false;

		if (nActionState == CREATURE_AS_ATTACK)
			m_curState = CREATURE_AS_IDLE;

		if (m_curState != nActionState ||
			nActionState == CREATURE_AS_BE_PLAYER_ATTACK ||
			nActionState == CREATURE_AS_BE_OTHER_ATTACK ||
			nActionState == CREATURE_AS_DEATH)
		{
			setActionState(nActionState);
			if (getCreatureAction() == CREATURE_AS_DEATH)
			{
				setDead();
				if (world->m_isClient)
					setDelayTicksToFree(PlayerPrefs::Instance()->getIntPrefs("ClientDelayTicksToFree"));
				else
					setDelayTicksToFree(2);
			}
			return true;
		}
		return false;
	}

	void EntityCreature::setCurrentTargetEntityId(int EntityId, bool revengingTarget)
	{
		if (EntityId >= 0) {
			m_targetEntityId = EntityId;
			m_revengingTarget = revengingTarget;
		}
	}

	bool EntityCreature::entityInAttackRange(Entity* entity, float offset/* = 0.0f*/)
	{
		bool bResult = false;
		if (entity)
		{
			float distance = this->getAttackDistance() + offset;
			float dis_sqr = this->getDistanceSq(entity->position);
			if (dis_sqr <= (distance * distance))
			{
				bResult = true;
			}
		}
		return bResult;
	}

	bool EntityCreature::isCanAttackEntityBySkill(Entity * entity)
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (!pSetting)
			return false;
		const SkillEffect* pSkill = SkillSetting::getSkillEffect(pSetting->skillId);
		if (!pSkill)
			return false;
		float minX = position.x - pSkill->width;
		float maxX = position.x + pSkill->width;
		float minZ = position.z - pSkill->height;
		float maxZ = position.z + pSkill->height;
		return entity->position.x >= minX && entity->position.x <= maxX && entity->position.z >= minZ && entity->position.z <= maxZ;
	}

	void EntityCreature::setSmallAttackCd()
	{
		m_attackCdTime = 1000 * m_minPlayerDistance / 8;
	}

	void EntityCreature::setAttackCd()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			m_attackCdTime = pSetting->attackCd * 1000;
		}
	}

	float EntityCreature::getAttackCd()
	{
		return m_attackCdTime;
	}

	void EntityCreature::refreshAttackCdTime()
	{
		if (m_attackCdTime > 0)
		{
			m_attackCdTime = m_attackCdTime - (m_curUpdateTime - m_lastUpdateTime);
		}
		else
		{
			m_attackCdTime = 0;
		}
	}

	void EntityCreature::setFreeSkillCd()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			m_freeSkillCdTime = pSetting->skillCd * 1000;
		}
	}

	float EntityCreature::getFreeSkillCd()
	{
		return m_freeSkillCdTime;
	}

	void EntityCreature::refreshFreeSkillCdTime()
	{
		if (m_freeSkillCdTime > 0)
		{
			m_freeSkillCdTime = m_freeSkillCdTime - (m_curUpdateTime - m_lastUpdateTime);
		}
		else
		{
			m_freeSkillCdTime = 0;
		}
	}

	void EntityCreature::setUpdateTargetCd()
	{
		m_updateTargetCdTime = 1000; // milliseconds
	}

	float EntityCreature::getUpdateTargetCd()
	{
		return m_updateTargetCdTime;
	}

	void EntityCreature::refreshUpdateTargetCdTime()
	{
		if (m_updateTargetCdTime > 0)
		{
			m_updateTargetCdTime = m_updateTargetCdTime - (m_curUpdateTime - m_lastUpdateTime);
		}
		else
		{
			m_updateTargetCdTime = 0;
		}
	}

	void EntityCreature::setUpdatePathCd(int pathTargetId)
	{
		m_updatePathCdTime = 1000; // milliseconds
		m_pathTargetId = pathTargetId;
	}

	float EntityCreature::getUpdatePathCd(int pathTargetId)
	{
		if (m_pathTargetId != pathTargetId) {
			return 0;
		}

		return m_updatePathCdTime;
	}

	void EntityCreature::refreshUpdatePathCdTime()
	{
		if (m_updatePathCdTime > 0)
		{
			m_updatePathCdTime = m_updatePathCdTime - (m_curUpdateTime - m_lastUpdateTime);
		}
		else
		{
			m_updatePathCdTime = 0;
		}
	}

	int EntityCreature::getAttackCount()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			return pSetting->attackCount;
		}
		return 1;
	}

	void EntityCreature::attackPerformance(EntityLivingBase* targeter)
	{

	}

	CRETURE_AI_ATTACK_TYPE EntityCreature::getCreatureAttackType()
	{
		CRETURE_AI_ATTACK_TYPE attackType = CRETURE_AI_ATTACK_TYPE_MELEE;
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			attackType = (CRETURE_AI_ATTACK_TYPE)pSetting->attackType;
		}
		return attackType;
	}

	bool EntityCreature::findCloestPlayerTargetInPatrolDist()
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

	bool EntityCreature::checkDynamicCreature()
	{
		CRETURE_AI_TYPE MonsterType = CRETURE_AI_TYPE_NONE;
		if (!world->m_isClient)
		{
			MonsterType = getMonsterType();
			m_creatureEntityType = MonsterType;
		}
		else
		{
			MonsterType = m_creatureEntityType;
		}
		return MonsterType != CRETURE_AI_TYPE_BASE && MonsterType != CRETURE_AI_TYPE_TOWER && MonsterType != CRETURE_AI_TYPE_NONE;
	}
	
	bool   EntityCreature::isFlyEntity()
	{
		return this->canFly;
	}

	void EntityCreature::updateFallState(float y, bool onGroundIn)
	{
		if (isFlyEntity())
			return;
		else
			EntityLiving::updateFallState(y, onGroundIn);
	}

	void EntityCreature::directMoveToPos(const Vector3 & toPos, bool shoudStopWhenCollision)
	{
		directMoveHelper->addMoveTask(toPos, shoudStopWhenCollision);
	}

	void EntityCreature::stopMove()
	{
		if (pathToEntity != nullptr)
		{
			LordDelete pathToEntity;
			pathToEntity = nullptr;
		}
		m_moveToPos.invalid();
		directMoveHelper->stopMove();
	}

	bool EntityCreature::findWayMoveToPos(const Vector3 &toPos)
	{
		this->directMoveHelper->stopMove();
		if (!isFlyEntity())
		{
			auto newPathToEntity =  this->world->
				getEntityPathToXYZ(this, toPos.getFloor(), 20.0F, true, false, false, true);
			
			if (newPathToEntity == nullptr)
			{
				pathToEntity = nullptr;
				return false;
			}
			else
			{
				if (this->pathToEntity != nullptr)
				{
					LordDelete pathToEntity;
				}
				this->pathToEntity = newPathToEntity;
			}
		}
	
		return true;
	}

	bool EntityCreature::needChangePath(const BlockPos& dest)
	{
		NULL_RETURN(pathToEntity, true);

		auto pFinalPath = pathToEntity->getFinalPathPoint();
		NULL_RETURN(pFinalPath, true);

		return (Vector3(pFinalPath->m_pos) - Vector3(dest)).lenSqr() > getDistanceSq(Vector3(dest));
	}

	EntityArr  EntityCreature::getClosestPlayers()
	{
		float attackDistance = 8.0f;
		int attackCount = 1;
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (pSetting)
		{
			attackDistance = pSetting->attackDistance;
			attackCount = pSetting->attackCount;
		}

		Vector3 & pos = position;

		float min_dis_sqr = attackDistance * attackDistance;
		float min_sqr = 65536.0f;
		EntityArr entityes;

		EntityPlayers& players = world->getPlayers();

		for (EntityPlayers::iterator it = players.begin(); it != players.end(); ++it)
		{
			EntityPlayer* pPlayer = it->second;
			if (!pPlayer)
				continue;

			if (pPlayer->getTeamId() == getTeamId())
				continue;

			if (!pPlayer->capabilities.disableDamage && !pPlayer->capabilities.isWatchMode && pPlayer->isEntityAlive())
			{
				float dis_sqr = pPlayer->getDistanceSq(pos);
				if (dis_sqr < min_dis_sqr)
				{
					entityes.push_back(pPlayer);
				}
				if (dis_sqr < min_sqr)
				{
					min_sqr = dis_sqr;
				}
			}
		}

		m_minPlayerDistance = Math::Sqrt(min_sqr);

		while (entityes.size() > size_t(attackCount))
		{
			float max_sqr = 0.0f;
			auto max_sqr_iterator = entityes.begin();
			bool hasIterator = false;
			for (auto it = entityes.begin(); it != entityes.end(); ++it)
			{
				float dis_sqr = (*it)->getDistanceSq(pos);
				if (dis_sqr > max_sqr)
				{
					max_sqr = dis_sqr;
					max_sqr_iterator = it;
					hasIterator = true;
				}
			}
			if (hasIterator)
				entityes.erase(max_sqr_iterator);
		}

		return entityes;
	}

} 
