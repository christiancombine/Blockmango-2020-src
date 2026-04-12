/********************************************************************
filename: 	EntityCreature.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-3
*********************************************************************/
#pragma once

#ifndef __ENTITY_CREATURE_HEADER__
#define __ENTITY_CREATURE_HEADER__

#include "EntityLiving.h"
#include <mutex>

namespace BLOCKMAN
{

class PathEntity;
class AIBase;
class EntityCreature;


class DirectMoveHelper: public ObjectAlloc
{
public:
	DirectMoveHelper(EntityCreature *entity):pEntity(entity)
	{}
	~DirectMoveHelper() = default;
	void addMoveTask(const Vector3 & toPos_, bool stopIfCollision);
	void update();
	bool isMoveOver();
	void stopMove();

private:
	bool checkMoveOnNextBlock();
	bool canMoveOnBlock(const BlockPos & pos);
	int  getBlockMoveInfo(const BlockPos & pos);
	void changeYaw();

	bool checkMoveOnNextBlockForWalkEntity();
	bool checkMoveOnNextBlockForFlyEntity();

	void updateXZ();
	void updateY();

	void updateYaw();
	bool checkCanMoveOnCollisionBlock();
private:
	EntityCreature *pEntity;
	bool needStopIfCollision = false;
	Vector3 toPos = Vector3::INVALID;
	float directYaw = 0.0;

	bool arrivedXZ = true;
	bool arrivedY = true;
	LORD::ui64 lastChangeYawTime = 0;
};


enum CreatureActionState
{
	//�������Ϊ
	CREATURE_AS_IDLE = 0,   // ����
	CREATURE_AS_MOVE,	   // �Ƅ�
	CREATURE_AS_ATTACK,	   // ����
	CREATURE_AS_BE_PLAYER_ATTACK,  // ��player����״̬
	CREATURE_AS_BE_OTHER_ATTACK,  // ����������״̬
	CREATURE_AS_DEATH, // ����
	CREATURE_AS_ALERT, // ����
	CREATURE_AS_RUN  //����
};

class EntityCreature : public EntityLiving
{
public:
	RTTI_DECLARE(EntityCreature);

	ui64 m_userId = 0;
	int m_MonsterId = 0;
	String m_nameLang = "";
	String m_actorName = "";
	bool isEntityAI = true;
	bool canFly = false;
	int m_curUpdateTime = 0;
	int m_lastUpdateTime = 0;
	float m_attackCdTime = 0;
	float m_freeSkillCdTime = 0;
	int m_teamId = 0;
	bool m_isDynamicCreature = true;
	float m_minPlayerDistance = 20.0f;
	CreatureActionState m_prevState;
	CreatureActionState m_curState;
	int m_incPathIndexFailedCount = 0;
	int m_targetEntityId = 0;
	bool m_revengingTarget = false;
	CRETURE_AI_TYPE m_creatureEntityType = CRETURE_AI_TYPE_NONE;
    float m_bloodStripOffset{0.0F};
	CreatureActionState m_prevBaseAction;
	CreatureActionState m_prevUpperAction;
	Listenable<CreatureActionState> m_baseAction;
	Listenable<CreatureActionState> m_upperAction;

protected:
	float m_updateTargetCdTime = 0;
	float m_updatePathCdTime = 0;
	int m_pathTargetId = 0;

public:
	CreatureActionState getPrevBaseAction() { return m_prevBaseAction; }
	CreatureActionState getPrevUpperAction() { return m_prevUpperAction; }
	CreatureActionState getBaseAction() { return m_baseAction; }
	void setBaseAction(CreatureActionState as) { m_prevBaseAction = m_baseAction; m_baseAction = as; }
	CreatureActionState getUpperAction() { return m_upperAction; }
	void setUpperAction(CreatureActionState as) { m_prevUpperAction = m_upperAction; m_upperAction = as; }
	void refreshBaseAction() { m_prevBaseAction = m_baseAction; }
	void refreshUpperAction() { m_prevUpperAction = m_upperAction; }
	void setUserId(ui64 userId) { m_userId = userId; }
	void setActorName(const String& actorName);
	void setCanFly(bool can) { this->canFly = can; }

	String getActorName() const
	{
		return m_actorName;
	}

	void setNameLang(const String& nameLang)
	{
		m_nameLang = nameLang;
	}

	String getNameLang() const
	{
		return m_nameLang;
	}

    int getPathTarget() { return m_pathTargetId; }

	virtual CRETURE_AI_TYPE getMonsterType();
	virtual void setMonsterType(CRETURE_AI_TYPE type);
	virtual String getMonsterBtTree();
	virtual bool isBase();
	virtual int getMonsterId() { return m_MonsterId;}
	virtual float getAttackDistance();
	virtual float getSkillDistance();
	virtual bool isBoss();
	virtual bool isMonster();
	/** Sets the x,y,z of the entity from the given parameters. Also seems to set up a bounding box. */
	virtual void setPosition(const Vector3& pos) override;
	virtual bool changeCreatureAction(CreatureActionState nActionState);
	virtual CreatureActionState getCreatureAction() { return m_curState; }
	virtual CreatureActionState getPreCreatureAction() { return m_prevState; }
	virtual void setScale(float s) override { m_scale = s; }
	virtual void setCurrentTargetEntityId(int EntityId, bool revengingTarget);
	virtual int getCurrentTargetEntityId() { return m_targetEntityId; }
	virtual bool isRevengingTarget() { return m_revengingTarget; }
	virtual bool entityInAttackRange(Entity* entity, float offset = 0.0f);
	virtual bool isCanAttackEntityBySkill(Entity* entity);

	virtual void setActionState(CreatureActionState state) { m_prevState = m_curState; m_curState = state; }

	virtual void setTeamId(int teamId) { m_teamId = teamId; }
	virtual int getTeamId() { return m_teamId; }
	virtual void setSmallAttackCd();
	virtual void setAttackCd();
	virtual float getAttackCd();
	virtual void refreshAttackCdTime();

	virtual void setFreeSkillCd();
	virtual float getFreeSkillCd();
	virtual void refreshFreeSkillCdTime();

	virtual void setUpdateTargetCd();
	virtual float getUpdateTargetCd();
	virtual void refreshUpdateTargetCdTime();


	virtual void setUpdatePathCd(int pathTargetId);
	virtual float getUpdatePathCd(int pathTargetId);
	virtual void refreshUpdatePathCdTime();

	virtual int getAttackCount();

	virtual CRETURE_AI_ATTACK_TYPE getCreatureAttackType();
	virtual void attackPerformance(EntityLivingBase* targeter);

	virtual EntityArr getClosestPlayers();
	virtual bool findCloestPlayerTargetInPatrolDist();
	virtual void collideWithEntity(Entity* pEntity) {};

	virtual bool isFlyEntity();
	virtual void updateFallState(float y, bool onGroundIn);

	bool isHurt() { return m_hurtTime > 0; }
	void setHurtTime(int hurtTime) { m_hurtTime = hurtTime; }
	void decrHurtTime() { m_hurtTime--; }

	bool isDynamicCreature() { return m_isDynamicCreature; }
	bool isImmuneToExplosions() { return true; }

	bool findWayMoveToPos(const Vector3& toPos);
	void directMoveToPos(const Vector3& toPos, bool shoudStopWhenCollision);
	bool hasMoveOver() { return pathToEntity == nullptr && m_moveToPos.isInvalid() && directMoveHelper->isMoveOver(); }
	void stopMove();

	bool hasDeathAction();
	bool hasRunAction();
	void playAlertAction(const Vector3& seePos);
	float detectNextMoveLen();
	virtual void notifyAttackFrom(EntityPlayer *pEnttiy) {};

	virtual bool isForceSyncSpawn() { return true; }

protected:

	PathEntity* pathToEntity = nullptr;

	/** The Entity this EntityCreature is set to attack. */
	Entity* entityToAttack = nullptr;

	/** returns true if a creature has attacked recently only used for creepers and skeletons */
	bool hasAttacked = false;

	/** Used to make a creature speed up and wander away when hit. */
	int fleeingTick = 0;
	
	BlockPos homePosition; // = new ChunkCoordinates(0, 0, 0);

	/** If -1 there is no maximum distance */
	float maximumHomeDistance = -1.f; // = -1.0F;
	AIBase* m_aiMoveTo{nullptr};
	bool m_hasTasks = false;
	int m_hurtTime = 0;
	Vector3 m_moveToPos = Vector3::INVALID;
	Vector3 m_bornPos;
	float   m_bornYaw;
	float moveUpDown = 0.0f;
	std::unique_ptr<DirectMoveHelper> directMoveHelper;
public:
	//EntityCreature(World* pWorld);
	EntityCreature(World* pWorld, int monsterId, Vector3& pos, float yaw, String actorName);
	virtual ~EntityCreature();
	
	/** implement override functions from EntityLiving */
	virtual bool getCanSpawnHere();

	/** implement override functions from EntityLivingBase */
	virtual void onUpdate();
	virtual void onLivingUpdate();
	virtual bool canBeCollidedWith();
	
	/** Takes a coordinate in and returns a weight to determine how likely this creature will try to path to the block.	*/
	virtual float getBlockPathWeight(const BlockPos& pos) { return 0.0F; }

	/** Returns true if entity has a path to follow */
	bool hasPath() { return pathToEntity != nullptr; }

	/** sets the Entities walk path in EntityCreature */
	void setPathToEntity(PathEntity* path) 
	{
		LordSafeDelete(pathToEntity);
		pathToEntity = path; 
	}

	/** Returns current entities target */
	Entity* getEntityToAttack() { return entityToAttack; }

	/** Sets the entity which is to be attacked. */
	void setTarget(Entity* pEntity) { entityToAttack = pEntity; }

	bool isWithinHomeDistanceCurrentPosition() { return isWithinHomeDistanceFromPosition(position.getFloor()); }

	bool isWithinHomeDistanceFromPosition(const BlockPos& pos);

	void setHomePosAndDistance(const BlockPos& pos, int dis) { homePosition = pos; maximumHomeDistance = (float)dis; }

	const BlockPos& getHomePosition() { return homePosition; }

	float getMaximumHomeDistance() { return maximumHomeDistance; }

	void detachHome() { maximumHomeDistance = -1.0F; }

	bool hasHome() { return maximumHomeDistance != -1.0F; }

	void getPathToEntity(Entity* endEntity, float mindis = 100.0f);

	void getPathToHome(float mindis = 100.0f);

	void getPathToPos(const BlockPos& destPos, float mindis = 100.0f);

	Vector3 getBornPos() { return m_bornPos; }

	float getBornYaw() { return m_bornYaw; }

	virtual void setDead();

	virtual PathEntity* getPathEntity() {
		return pathToEntity;
	}

	void decrTicksToFree() {
		if (isDead) {
			--m_delayTicksToFree;
		}
	}

	virtual EntityPlayer* findTargetPlayer();

protected:
	/** Basic mob attack. Default to touch of death in EntityCreature. Overridden by each mob to define their attack. */
	virtual void attackEntity(Entity* pEntity, float damage) {}

	/** Finds the closest player within 16 blocks to attack, or null if this Entity isn't interested in attacking (Animals, Spiders at day, peaceful PigZombies). */
	virtual Entity* findPlayerToAttack() { return NULL; }

	/** implement override functions from EntityLiving*/
	virtual void updateLeashedState();
	/** implement override functions from EntityLivingBase */
	virtual void updateEntityActionState();

	virtual void onLeashDistance(float par1) {}

	// check dynamic creature (Init isDynamicCreature)
	virtual bool checkDynamicCreature();

	// if need change path
	bool needChangePath(const BlockPos& dest);
};


}
#endif