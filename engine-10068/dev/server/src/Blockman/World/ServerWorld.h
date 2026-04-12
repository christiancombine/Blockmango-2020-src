/********************************************************************
filename: 	ServerWorld
file path:	h:\sandboxol\client\blockmango-client\dev\server\src\World\ServerWorld.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __SERVERWORLD_H__
#define __SERVERWORLD_H__

#include "Global.h"
#include "World/World.h"
#include "World/WorldSettings.h"
#include "World/GameSettings.h"
#include "Util/SubscriptionGuard.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityBulletin.h"
#include "LuaRegister/Template/TableVisitor.h"
#include "Inventory/IInventory.h"
#include <unordered_map>
#include "../Entity/EntityRobotAI.h"

namespace BLOCKMAN
{

class EntityTracker;
class EntityPlayerAI;

struct BlockEventData
{
	BlockPos m_pos;
	int		 m_blockID;
	int		 m_eventID;
	int		 m_eventParam;

	BlockEventData(const BlockPos& pos, int blockid, int eventid, int eventparam)
		: m_pos(pos)
		, m_blockID(blockid)
		, m_eventID(eventid)
		, m_eventParam(eventparam)
	{}

	bool operator == (const BlockEventData& rhs) const
	{
		return m_pos == rhs.m_pos && m_blockID == rhs.m_blockID && m_eventID == rhs.m_eventID && m_eventParam == rhs.m_eventParam;
	}
};

typedef list<BlockEventData>::type BlockEventList;
using EntityRobotAIMap = std::unordered_map<int, EntityRobotAI*>;

struct TickEntry
{
	struct TickEntryHasher {
		size_t operator() (const TickEntry& te) const {
			return size_t((((te.m_pos.x << 20) + (te.m_pos.z << 10) + te.m_pos.y) << 8) + te.blockID);
		}
	};

	static i64 nextTickEntryID;
	BlockPos m_pos;
	int blockID;
	int priority;
	i64 scheduledTime;
	i64 tickEntryID;

	TickEntry()
	{}

	TickEntry(const BlockPos& pos, int id)
		: m_pos(pos), blockID(id)
	{
		tickEntryID = nextTickEntryID++;
		priority = 0;
		scheduledTime = 0; 
	}

	bool operator == (const TickEntry& rhs) const;
	bool operator > (const TickEntry& rhs) const;
	bool operator < (const TickEntry& rhs) const;

	void setScheduledTime(i64 t) { scheduledTime = t; }
	void setPriority(int p) { priority = p; }
};

typedef list<TickEntry>::type TickEntryLst;
typedef set<TickEntry>::type TickEntryTreeSet;
typedef std::unordered_set<TickEntry, TickEntry::TickEntryHasher> TickEntryHashSet;

class ServerWorld : public World
{
private:
	SubscriptionGuard m_subscriptionGuard;
	int m_quickChangeTimeTicks = 0;
	int m_quickChangeTimeSpeed = 0;
	int	m_quickChangeEndTime = 0;
	EntityRobotAIMap m_entityRobotAIMap;
public:
	ServerWorld(const String& name, const WorldSettings& settings, int loadRange);
	virtual ~ServerWorld(void);

	//static std::shared_ptr<ServerWorld> createWorld(const String & path);
	static ServerWorld * createWorld(const String & name);
	void destroy();

	ChunkService* createChunkService(int loadRange) override;

	/** implement override functions from World. */
	virtual void tick();
	virtual bool tickUpdates(bool par1) override;
	virtual void addBlockEvent(const BlockPos& pos, int blockID, int id, int param);
	virtual bool isBlockTickScheduledThisTick(const BlockPos& pos, int blockID) override;
	virtual void scheduleBlockUpdate(const BlockPos& pos, int blockID, int tickrate) override;
	virtual void scheduleBlockUpdateWithPriority(const BlockPos& pos, int blockID, int tickrate, int priority) override;
	virtual void scheduleBlockUpdateFromLoad(const BlockPos& pos, int blockID, int tickrate, int priority) override;
	virtual float getFrameDeltaTime();
	virtual bool setBlock(const BlockPos& pos, int blockID, int metadata, int flag, bool immediate = true) override;
	/** Called to place all entities as part of a world	*/
	virtual bool spawnEntityInWorld(Entity* pEntity) override;
	virtual void removeEntity(Entity* pEntity) override;

	void setQuickWorldTime(int quickChangeTimeTicks, int quickChangeTimeSpeed, int quickChangeEndTime);
	virtual void setWorldTime(i64 time) override;
	void createShop();
	void addBlockDoor(const BlockPos& pos, int maxTick);
	void removeBlockDoor(const BlockPos& pos);
	void showBuyRespawnToPlayer(ui64 rakssId, int times);
	void setPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2, float speed);
	int tryAllPlayerTakeAircraft(Vector3 startPos, Vector3 endPos, float speed);
	void generate();
	void fireTNT(Vector3i tntPos, int entityId = 0);
	void addAirDrop(float x, float y, float z);

	void killCreature(int entityId);
	void changeCreatureAttackTarget(int entityId, int targetId);
	bool setCropsBlock(ui64 platformId, const BlockPos& pos, int blockID, int metadata, int curStageTime, int stealCount, std::time_t lastUpdateTime, int residueHarvestNum = 1);
	bool setFruitsBlock(const BlockPos& pos, int blockID);
	//强制怪物移动到一个位置
	void forceCreatureToPos(int entityId, const Vector3& pos);

	// fill blocks from start_pos to end_pos.
	void fillAreaByBlockIdAndMate(const BlockPos& start_pos, const BlockPos& end_pos, int block_id, int mate = 0) override;
	void updateManorOwner(ui64 rakssid, std::vector<ManorOwner> owners) override;
	// block is in record
	bool isInChangeRecord(const BlockPos& blockPos);

	void addSingleUpdateBlock(BlockPos pos, int block_id, int meta);

	void addSkillEffect(int skillEffectId, Vector3 position);
	void updateActorNpcContent(int entityId, String content);
	void updateActorNpcSuspended(int entityId, bool suspended, Vector3 pos);
	void updateSessionNpcHeadInfoMultiLang(int entityId, String msg, int lv, int time);
	void updateSessionNpcHeadInfoTime(ui64 rakssId, int entityId, String msg, int lv, int time);
	void updateBuildNpc(i32 entityId, i32 maxQueueNum, i32 productCapacity, i32 queueUnlockPrice, i32 queueUnlockCurrencyType, std::vector<RanchCommon> products,  std::vector<ProductQueue> unlockQueues, std::vector<ProductRecipe> recipes);
	void updateLandNpc(i32 entityId, i32 status, i32 price, i32 totalTime, i32 timeLeft, std::vector<RanchCommon> recipe, std::vector<RanchCommon> reward) override;

	void addSimpleEffect(String effectName, Vector3 position, float yaw, int duration, int targetId = 0, float scale = 1.0f);
	void removeSimpleEffect(String effectName);
	void setRailSpeedInfo(int entityId, float maxSpeed, float curSpeed, float addSpeed);
	void setRailCarMaxSpeed(int entityId, float maxSpeed);
	void setRailCarCurSpeed(int entityId, float curSpeed);
	void setRailCarAddSpeed(int entityId, float addSpeed);
	void setRailCarRecordRoute(int entityId, bool isRecord);
	void setRailCarActor(int entityId, String actorName);
	void setRailCarName(int entityId, String nameLang);
	void startRailCarRun(int entityId, float angle);
	void changeRailCarDirection(int entityId);
	void setRailCarFrozen(int entityId, float frozenTime);
	bool rangeCheckBlockByEntityId(i32 entityId, i32 blockId) override;
	void playCloseup(const Vector3& position, float farDistance, float nearDistance, float velocity, float duration, float yaw, float pitch);
	void setEntitySelected(ui64 rakssId, int entityId, int type);
	void updateEntityBulletin(ui64 rakssid, i32 entityId, std::vector<BulletinRank> ranks);
	void updateEntityBulletinUserRank(ui64 rakssid, i32 entityId, BulletinRank userRanks);
	void setBirdDress(i32 entityId, String dressGlasses, String dressHat, String dressBeak, String dressWing, String dressTail, String dressEffect);
	void updateBirdDress(i32 entityId, String  masterName, String  slaveName);
	void updateSessionNpc(i32 entityId, ui64 rakssid, String nameLang, String actorName, String actorBody, String actorBodyId, String sessionContent, String actorAction, i32 timeLeft = 0, bool isCanCollided = true);
	void setEntityHealth(i32 entityId, float health, float maxHealth);
	void setCreatureHome(i32 entityId, Vector3i pos,  int dis = 0);
	IInventory* getInventory(int blockId, const BlockPos& pos);

public:
	virtual int addRobot(Vector3 pos, int monsterId, float yaw, String actorName);
	virtual int addWalkingDead(Vector3 pos, int monsterId, float yaw, String actorName);
	virtual EntityCreature* addCreature(Vector3 pos, int monsterId, float yaw, String actorName);
	virtual int addEntityBird(Vector3 pos, TableVisitor birdMgr, Vector3i homePos, ui64 userId, i64 birdId, String actorName, String actorBody, String actorBodyId);
	virtual EntityPlayerAI* addEntityPlayerAI(Vector3i pos, String name, const ui64 rakssid);
protected:
	void sendAndApplyBlockEvents();

	/** Called to apply a pending BlockEvent to apply to the current world. */
	bool onBlockEventReceived(const BlockEventData& data);

public:
	int m_blockEventIndex;

	/** pending tick list entries this tick. */
	TickEntryLst m_tickEntryList;
	TickEntryTreeSet m_tickEntryTreeSet;
	TickEntryHashSet m_tickEntryHashSet;

	/** Double buffer of ServerBlockEventList[] for holding pending BlockEventData's */
	BlockEventList m_blockEventCache[2];

	GameSettings * m_gamesSetting = nullptr;
};

}
#endif
