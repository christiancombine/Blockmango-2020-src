/********************************************************************
filename: 	World.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-4
*********************************************************************/
#ifndef __WORLD_HEADER__
#define __WORLD_HEADER__

#include "IBlockAccess.h"
#include "Section.h"
#include "WorldInfo.h"
#include "WorldSettings.h"
#include "BM_Container_def.h"
#include "IWorldEventListener.h"
#include "RayTracyResult.h"
#include "Shop.h"

#include "Util/Random.h"
#include "Util/ChunkPosition.h"
#include "Util/ChunkCoordIntPair.h"
#include "Audio/SoundDef.h"
#include "Game/Manor.h"
#include "Setting/RanchSetting.h"
#include "Core.h"
#include <future>

using namespace LORD;

class CprCallBackWrap;
namespace BLOCKMAN
{

class ChunkService;
class BM_Material;
class Entity;
class EntityCreature;
class Chunk;
class Block;
class TileEntity;
class Explosion;
class EntityPlayer;
class PathEntity;
class EntityAircraft;
class EntityBird;
class EntityVehicle;
class EntityActorNpc;
class EntitySessionNpc;
class EntityFlyingMount;
class EntityFloatingMount;
class EntitySkillThrowable;
class EntityBlockmanEmpty;
class EntityCreatureBullet;
class SimpleEffect;
class SchematicTaskQueue;

using ChunkPtr = std::shared_ptr<Chunk>;

class World : public IBlockAccess, public ObjectAlloc
{
	friend class Chunk;
public:
	typedef list<IWorldEventListener*>::type WorldListeners;
	typedef set<TileEntity*>::type	TileEntitiesArr;
	typedef std::future<void> CprHttpFuture;
public:
	/** bool; if true updates scheduled by scheduleBlockUpdate happen immediately */
	bool m_scheduledUpdatesAreImmediate = false;
	/** RNG for World. */
	Random m_Rand;
	/** This is set to true for client worlds, and false for server worlds. */
	bool m_isClient = true;
	ChunkService* m_pChunkService = nullptr;
	/** Boolean that is set to true when trying to find a spawn point */
	bool m_findingSpawnPoint = false;
	/** How much light is subtracted from full daylight */
	int m_skylightSubtracted = 0;
	/** Set to 2 whenever a lightning bolt is generated in SSP. Decrements if > 0 in updateWeather(). Value appears to be unused. */
	int m_lastLightningBolt = 0;
	/** Option > Difficulty setting (0 - 3) */
	int m_difficultySetting = 0;

	Shop* m_shop = nullptr;
	ManorControl* m_manorControl = nullptr;
	float lightBrightnessTable[16];
	float colorsSunriseSunset[4];
	float moonPhaseFactors[8];

protected:
	/** A list of all Entities in all currently-loaded chunks */
	EntityMap m_loadedEntityMap;
	EntityList m_unloadedEntityList;

	/** A list of all TileEntities in all currently-loaded chunks */
	TileEntitiesArr m_loadedTileEntityList;
	TileEntitiesArr m_addedTileEntityList;
	/** Entities marked for removal. */
	TileEntitiesArr m_entityRemoval;

	/** Array list of players in the world. */
	EntityPlayers m_playerEntities;

	/** a list of all the lightning entities */
	// vector<Entity*>	m_weatherEffects;

	WorldListeners m_worldListeners;
	/** Handles chunk operations and caching */

	//ISaveHandler saveHandler;

	/**
	* holds information about a world (size on disk, time, spawn point, seed, ...)
	*/
	WorldInfo m_WorldInfo;

	// MapStorage mapStorage;
	// VillageCollection villageCollectionObj;
	// VillageSiege villageSiegeObj = new VillageSiege(this);
	// Profiler theProfiler;

	/** The world-local pool of vectors */
	// private final Vec3Pool vecPool = new Vec3Pool(300, 2000);
	// Calendar theCalendar = Calendar.getInstance();
	// Scoreboard worldScoreboard;

	/** The log agent for this world. */
	// final ILogAgent worldLogAgent;
	AABBList m_collidingAABBs;
	bool m_scanningTileEntities = false;

	/** indicates if enemies are spawned or not */
	// protected boolean spawnHostileMobs = true;

	/** A flag indicating whether we should spawn peaceful mobs. */
	// protected boolean spawnPeacefulMobs = true;

	// whether has knockback effect when attack no harm
	bool m_knockbackNoHarm = true;

private:
	CprCallBackWrap* m_cprCallBackWrap = NULL;
	std::list<CprHttpFuture> m_cprHttpFutureList;
	void checkCprHttpFuture();
	SchematicTaskQueue* m_schematicTaskQueue = NULL;

protected:
	void onEntityAdded(Entity* par1Entity);
	void onEntityRemoved(Entity* par1Entity);

public:
	void init();
	World(const String& name, const WorldSettings& settings, int loadRange);
	virtual ~World();

	virtual float getFrameDeltaTime() = 0;
	virtual void createShop();
	Shop* getShop() { return m_shop; }

	bool knockbackNoHarm() { return m_knockbackNoHarm; }
	void setKnockbackNoHarm(bool value) { m_knockbackNoHarm = value; }

	/** Creates the chunk provider for this world. Called in the constructor. Retrieves provider from worldProvider? */
	virtual ChunkService* createChunkService(int loadRange) = 0;

	void initialize(const WorldSettings& par1WorldSettings) { m_WorldInfo.setServerInitialized(true); }

	void reset();
	/**  Sets a new spawn location by finding an uncovered block at a random (x,z) location in the chunk. */
	void setSpawnLocation() { setSpawnLocation({ 8, 64, 8 }); }

	/**  Returns the block ID of the first block at this (x,z) location with air above it, searching from sea level upwards. */
	int getFirstUncoveredBlock(int x, int z);
		
	void updateComparatorOutputLevel(const BlockPos& pos, int blockID);

	/** Checks if a block at a given position should have a tile entity. */
	bool blockHasTileEntity(const BlockPos& pos);

	void updateAllPlayersSleepingFlag() {}
	
	/** Returns whether a block exists at world coordinates x, y, z */
	bool blockExists(const BlockPos& pos);

	/** Checks if any of the chunks within distance (argument 4) blocks of the given block exist */
	bool doChunksNearChunkExist(const BlockPos& pos, int dis);

	/** Checks between a min and max all the chunks in between actually exist. */
	bool checkChunksExist(const BlockPos& min, const BlockPos& max);

	/** Returns whether a chunk exists at chunk coordinates x, y */
	bool chunkExists(int x, int z);

	/** Returns a chunk looked up by block coordinates. Args: x, z */
	ChunkPtr getChunkFromBlockCoords(int x, int z);

	/** Returns back a chunk looked up by chunk coordinates Args: x, y */
	ChunkPtr getChunkFromChunkCoords(int x, int z);

	bool canLightningStrikeAt(const BlockPos& pos);

	EntityPlayers& getPlayers() { return m_playerEntities; }

	/** Returns true if the given Entity can be placed on the given side of the given block position. */
	bool canPlaceEntityOnSide(int blockID, const BlockPos& pos, bool par5, int side, Entity* pEntity, ItemStackPtr pStack);

	/** Sets the block ID and metadata at a given location. Args: X, Y, Z, new block ID, new metadata, flags. Flag 1 will
	* cause a block update. Flag 2 will send the change to clients (you almost always want this). Flag 4 prevents the
	* block from being re-rendered, if this is a client world. Flags can be added together. */
	bool needNotifyNeighborBlock(int flag) { return (flag & 1) != 0; }
	bool needNotifyListeners(int flag) { return (flag & 2) != 0; }
	bool preventRerendered(int flag) { return (flag & 4) != 0; }
	virtual bool setBlock(const BlockPos& pos, int blockID, int metadata, int flag, bool immediate = true);
		
	/** Set a all the blocks data. */
	virtual bool setBlocks(int x, int z, const BlockModifys& modifys);

	/**
	* Sets the blocks metadata and if set will then notify blocks that this block changed, depending on the flag. Args:
	* x, y, z, metadata, flag. See setBlock for flag description
	*/
	bool setBlockMetadataWithNotify(const BlockPos& pos, int metadata, int flag);

	/** Sets a block to 0 and notifies relevant systems with the block change  Args: x, y, z */
	bool setBlockToAir(const BlockPos& pos) { return setBlock(pos, 0, 0, 3); }

	/** Destroys a block and optionally drops items. Args: X, Y, Z, dropItems */
	bool destroyBlock(const BlockPos& pos, bool isDropItem);

	/** Sets a block and notifies relevant systems with the block change  Args: x, y, z, blockID */
	bool setBlock(const BlockPos& pos, int blockID)	{ return setBlock(pos, blockID, 0, 3); }

	void addTileEntity(TileEntity* tileEntity);

	/**
	* On the client, re-renders the block. On the server, sends the block to the client (which will re-render it only
	* if the ID or MD changes), including the tile entity description packet if applicable. Args: x, y, z
	*/
	void markBlockForUpdate(const BlockPos& pos);

	void markSectionForUpdate(const BlockPos& sectionID);

	/** The block type change and need to notify other systems  Args: x, y, z, blockID */
	void notifyBlockChange(const BlockPos& pos, int blockID) { notifyBlocksOfNeighborChange(pos, blockID); }
	
	/** On the client, re-renders all blocks in this range, inclusive. On the server, does nothing. */
	void markBlockRangeForRenderUpdate(const BlockPos& min, const BlockPos& max);

	/** Notifies neighboring blocks that this specified block changed */
	void notifyBlocksOfNeighborChange(const BlockPos& pos, int blockID);

	/** Calls notifyBlockOfNeighborChange on adjacent blocks, except the one on the given side. */
	void notifyBlocksOfNeighborChange(const BlockPos& pos, int changeBlockID, int side);

	/** Notifies a block that one of its neighbor change to the specified type Args: x, y, z, blockID */
	void notifyBlockOfNeighborChange(const BlockPos& pos, int changeBlockID);

	/** Returns true if the given block will receive a scheduled tick in this tick. Args: X, Y, Z, blockID */
	virtual bool isBlockTickScheduledThisTick(const BlockPos& pos, int blockID) { return false; }

	/** Checks if the specified block is able to see the sky */
	bool canBlockSeeTheSky(const BlockPos& pos);

	/** Returns the y coordinate with a block in it at this x, z coordinate */
	int getHeightValue(int x, int z);

	/** On the client, re-renders this block. On the server, does nothing. Used for lighting updates. */
	void markBlockForRenderUpdate(const BlockPos& pos);

	/** Checks whether its daytime by seeing if the light subtracted from the skylight is less than 4 */
	bool isDaytime() { return m_skylightSubtracted < 4; }

	EntityMap* getLoadedEntityMap() { return &m_loadedEntityMap; }
	const TileEntitiesArr& getLoadedTileEntities() const { return m_loadedTileEntityList; }
	/** Called to place all entities as part of a world	*/
	virtual bool spawnEntityInWorld(Entity* pEntity);
	
	//
	int spawnItemInWorld(int id, int num, int meta, float life, Vector3 position, Vector3 motion, bool isCombine, bool isEverLife = false);

	/** Schedule the entity for removal during the next tick. Marks the entity dead in anticipation. */
	virtual void removeEntity(Entity* pEntity);
	void removeEntity(int entityId);

	EntityAircraft* getAircraft();
	Entity* getEntity(int entityId);
	EntityCreature* getEntityCreature(int entityId);
	EntityPlayer* getPlayerEntity(int entityId);
	EntityPlayer* getPlayerByPlatformUserId(ui64 userId);
	EntityPlayer* getFirstPlayerEntity();
	EntityArr getEntityCreatureAI(String loadBtTree);
	Entity* getEntityCreatureAIByType(CRETURE_AI_TYPE CreatureAIType);
	EntityArr getEntityCreatureMonsterAI();
	EntityArr getEntityCreatureBaseAI();
	EntityArr getEntityCreatureBossAI();
	EntityArr getBulletEntity(int shootingEntityId);

	/** Do NOT use this method to remove normal entities- use normal removeEntity */
	void removePlayerEntityDangerously(EntityPlayer* pPlayer);

	void addWorldListener(IWorldEventListener* listener) { m_worldListeners.push_back(listener); }
	void removeWorldListener(IWorldEventListener* listener) { m_worldListeners.remove(listener); }

	/** Returns a list of bounding boxes that collide with aabb excluding the passed in entity's collision. Args: entity, aabb*/
	AABBList& getCollidingBoundingBoxes(Entity* pEntity, const Box& aabb);
	AABBList& getCollidingBlockBounds(const Box& aabb);

	EntityArr getEntitiesWithinAABBExcludingEntity(Entity* pEntity, const Box& aabb, bool includeWatchModePlayer = false);
	EntityArr getEntitiesWithinAABB(ui32 EntityType, const Box& aabb, bool includeWatchModePlayer = false);
	EntityArr getEntitiesWithinRayTracy(Entity* pEntity, int type, const Vector3& beginPos, const Vector3& dir, float len);

	bool isEntityHeadWithinRayTracy(const Box& aabb, const Vector3& beginPos, const Vector3& dir, float len);

	/** Creates an explosion. Args: entity, x, y, z, strength */
	Explosion* createExplosion(Entity* exploder, EntityLivingBase* explosivePlacedBy, const Vector3& pos, float strength, bool bSmoking, int sourceItemId = 0);

	/** returns a new explosion. Does initiation (at time of writing Explosion is not finished) */
	Explosion* newExplosion(Entity* exploder, EntityLivingBase* explosivePlacedBy, const Vector3& pos, float strength, bool bFlaming, bool bSmoking, float custom_explosionDamageFactor = 1.0f, int sourceItemId = 0);

	/** Gets the percentage of real blocks within within a bounding box, along a specified vector. */
	float getBlockDensity(const Vector3& vec, const Box& aabb);

	/** If the block in the given direction of the given coordinate is fire, extinguish it. Args: Player, X,Y,Z, blockDirection */
	bool extinguishFire(EntityPlayer* pPlayer, const BlockPos& pos, int face);

	RayTraceResult rayTraceBlocks(Vector3 start, Vector3 end);

	RayTraceResult rayTraceBlocks(Vector3 start, Vector3 end, bool stopOnLiquid);

	/** Performs a raycast against all blocks in the world */
	RayTraceResult rayTraceBlocks(Vector3 start, Vector3 end, bool stopOnLiquid, bool ignoreBlockWithoutBoundingBox);

	/** Returns the amount of skylight subtracted for the current time */
	int calculateSkylightSubtracted(float rdt);

	/**	calls calculateCelestialAngle */
	float getCelestialAngle(float rdt);
	float* calcSunriseSunsetColors(float angle, float rdt);
		
	/** Return getCelestialAngle()*2*PI */
	float getCelestialAngleRadians(float rdt);
	
	/** Gets the height to which rain/snow will fall. Calculates it if not already stored.	*/
	int getPrecipitationHeight(int x, int z);

	/** Finds the highest block on the x, z coordinate that is solid and returns its y coord. Args x, z	*/
	int getTopSolidOrLiquidBlock(int x, int z);

	/** Schedules a tick to a block with a delay (Most commonly the tick rate) */
	virtual void scheduleBlockUpdate(const BlockPos& pos, int blockID, int tickrate) {}

	virtual void scheduleBlockUpdateWithPriority(const BlockPos& pos, int blockID, int tickrate, int priority) {}

	/** Schedules a block update from the saved information in a chunk. Called when the chunk is loaded. */
	virtual void scheduleBlockUpdateFromLoad(const BlockPos& pos, int blockID, int tickrate, int priority) {}

	/** Returns true if there are no solid, live entities in the specified AxisAlignedBB */
	bool checkNoEntityCollision(const Box& aabb) { return checkNoEntityCollision(aabb, NULL); }

	/** Returns true if there are no solid, live entities in the specified AxisAlignedBB, excluding the given entity */
	bool checkNoEntityCollision(const Box& aabb, Entity* pEntity);

	/** Returns true if there are any blocks in the region constrained by an AxisAlignedBB	*/
	bool checkBlockCollision(const Box& aabb);

	/** Returns true if the given bounding box contains the given material */
	bool isMaterialInBB(const Box& bb, const BM_Material& mat);

	/** handles the acceleration of an object whilst in water. Not sure if it is used elsewhere. */
	bool handleMaterialAcceleration(const Box& bb, const BM_Material& mat, Entity* pEntity);

	/** checks to see if a given block is both water and is cold enough to freeze */
	bool isBlockFreezable(const BlockPos& pos) { return canBlockFreeze(pos, false); }

	/** checks to see if a given block is both water, and cold enough to freeze - if the par4 boolean is set, this will
	* only return true if there is a non-water block immediately adjacent to the specified block */
	bool canBlockFreeze(const BlockPos& pos, bool flag);

	/** Tests whether or not snow can be placed at a given location	*/
	bool canSnowAt(const BlockPos& pos);

	/** Returns if any of the blocks within the aabb are liquids. Args: aabb */
	bool isAnyLiquid(const Box& aabb);

	/** Returns whether or not the given bounding box is on fire or not */
	bool isBoundingBoxBurning(const Box& aabb);
		
	/** Sets the TileEntity for a given block in X, Y, Z coordinates */
	void setBlockTileEntity(const BlockPos& pos, TileEntity* pTileEntity, const ChunkPtr& ownChunk = nullptr);
	/*** Removes the TileEntity for a given block in X,Y,Z coordinates */
	void removeBlockTileEntity(const BlockPos& pos);

	bool isBlockFullCube(const BlockPos& pos);
		
	/**
	* Performs check to see if the block is a normal, solid block, or if the metadata of the block indicates that its
	* facing puts its solid side upwards. (inverted stairs, for example)
	*/
	bool isBlockTopFacingSurfaceSolid(const Block* par1Block, int metadata);

	/** Checks if the block is a solid, normal cube. If the chunk does not exist, or is not loaded, it returns the boolean parameter. */
	bool isBlockNormalCubeDefault(const BlockPos& pos, bool par4);

	/** Called on construction of the World class to setup the initial skylight values */
	void calculateInitialSkylight();

	/** Runs a single tick for the world */
	virtual void tick();
	
	/** Runs through the list of updates to run and ticks them */
	virtual bool tickUpdates(bool par1);
	
	/**  Returns the highest redstone signal strength powering the given block. Args: X, Y, Z. */
	int getBlockPowerInput(const BlockPos& pos);

	/**	* Returns the indirect signal strength being outputted by the given block in the *opposite* of the given direction.	*/
	bool getIndirectPowerOutput(const BlockPos& pos, int dir);

	/** Gets the power level from a certain block face.  Args: x, y, z, direction */
	int getIndirectPowerLevelTo(const BlockPos& pos, int dir);

	/**
	* Used to see if one of the blocks next to you or your block is getting power from a neighboring block. Used by
	* items like TNT or Doors so they don't have redstone going straight into them.  Args: x, y, z
	*/
	bool isBlockIndirectlyGettingPowered(const BlockPos& pos);

	int getStrongestIndirectPower(const BlockPos& pos);

	EntityArr getClosestCreatureBossToPlayer(Entity* par1Entity, float distance);

	EntityArr getClosestCreatureBoss(const Vector3& pos, float distance);

	/** Gets the closest player to the entity within the specified distance (if distance is less than 0 then ignored). */
	EntityPlayer* getClosestPlayerToEntity(Entity* par1Entity, float distance);

	/** Gets the closest player to the point within the specified distance (distance can be set to less than 0 to not limit the distance). */
	EntityPlayer* getClosestPlayer(const Vector3& pos, float distance);

	EntityPlayer* getClosestPlayerIgnoreYPos(const Vector3&pos, float distance);

	/** Returns the closest vulnerable player to this entity within the given radius, or null if none is found */
	EntityPlayer* getClosestVulnerablePlayerToEntity(Entity* entity, float radius);

	/** Returns the closest vulnerable player within the given radius, or null if none is found. */
	EntityPlayer* getClosestVulnerablePlayer(const Vector3& pos, float radius);

	/** If on MP, sends a quitting packet. */
	void sendQuittingDisconnectingPacket() {}

	/**  Retrieve the world seed from level.dat	*/
	i64 getSeed() const { return m_WorldInfo.getSeed(); }

	i64 getTotalWorldTime() const { return m_WorldInfo.getWorldTotalTime(); }

	i64 getWorldTime() const { return m_WorldInfo.getWorldTime(); }

	virtual void setWorldTime(i64 time) { m_WorldInfo.setWorldTime(time); }

	float getCurrWorldTime(int& hour, int& minute);

	void setWorldWeather(String weather, int time);

	void setSpawnLocation(const Vector3i& position);

	BlockPos getSpawnPoint()
	{
		return BlockPos(m_WorldInfo.getSpawnX(), m_WorldInfo.getSpawnY(), m_WorldInfo.getSpawnZ());
	}

	/** Called when checking if a certain block can be mined or not. The 'spawn safe zone' check is located here. */
	bool canMineBlock(EntityPlayer* pPlayer, const BlockPos& pos) { return true; }

	/** sends a Packet 38 (Entity Status) to all tracked players of that entity */
	virtual void setEntityState(Entity* pEntity, i8 state) { }

	/** gets the ChunkService this world uses. */
	ChunkService* getChunkService() { return m_pChunkService; }

	virtual void addBlockDoor(const BlockPos& pos, int maxTick) {}
	virtual void removeBlockDoor(const BlockPos& pos) {}
	/**
	* Adds a block event with the given Args to the blockEventCache. During the next tick(), the block specified will
	* have its onBlockEvent handler called with the given parameters. Args: X,Y,Z, BlockID, EventID, EventParameter
	*/
	virtual void addBlockEvent(const BlockPos& pos, int blockID, int id, int param);

	/** Gets the World's WorldInfo instance */
	const WorldInfo& getWorldInfo() const { return m_WorldInfo; }
	void setTimeStopped(bool stopped) { m_WorldInfo.setTimeStopped(stopped); }

	/** Gets the GameRules instance. */
	// GAME_R getGameRules() const { return m_orldInfo.getGameRulesInstance(); }

	/** Checks to see if the biome rainfall values for a given x,y,z coordinate set are extremely high*/
	bool isBlockHighHumidity(const BlockPos& pos);

	/** Returns an unique new data id from the MapStorage for the given prefix and saves the idCounts map to the 'idcounts' file. */
	int getUniqueDataId(const String& par1Str);
	
	/** Returns current world height. */
	int getActualHeight() const;

	/** puts the World Random seed to a specific state dependant on the inputs */
	Random& setRandomSeed(int s1, int s2, int s3);
		
	/** Returns horizon height for use in rendering the sky. */
	float getHorizon() const { return m_WorldInfo.getTerrainType() == TERRAIN_TYPE_FLAT ? 0.0f : 63.0f; }

	/** Starts (or continues) destroying a block with given ID at the given coordinates for the given partially destroyed  value */
	void destroyBlockInWorldPartially(int breakerID, const BlockPos& pos, int damage);


	void updateTileEntityChunkAndDoNothing(const BlockPos& pos, TileEntity* pTileEntity);

	PathEntity* getEntityPathToXYZ(Entity* pEntity, const BlockPos& pos, float mindis, bool passOpenWoodDoor, bool passCloseWoodDoor, bool avoidsWater, bool canSwim);
	PathEntity* getPathEntityToEntity(Entity* start, Entity* end, float mindis, bool passOpenWoodDoor, bool passCloseWoodDoor, bool avoidsWater, bool canSwim);

	void removeEntityFromChunk(Entity* pEntity);

	/*** Updates (and cleans up) entities and tile entities */
	void updateEntities();
	void updateEntity(Entity* pEntity);
	/** Will update the entity in the world if the chunk the entity is in is currently loaded or its forced to update. */
	void updateEntityWithOptionalForce(Entity* pEntity, bool forceUpdate);
	void markTileEntityForDespawn(TileEntity* pTileEntity) { m_entityRemoval.insert(pTileEntity); }
	/** Adds a list of entities to be unloaded on the next pass of World.updateEntities() */
	void unloadEntities(const EntityList& entities) { m_unloadedEntityList.insert(entities.begin(), entities.end()); }

	/** Does the same as getBlockLightValue_do but without checking if its not a normal block */
	virtual int getFullBlockLightValue(const BlockPos& pos) { return 0; }
	/** Returns saved light value without taking into account the time of day.  Either looks in the sky light map or
	* block light map based on the enumSkyBlock arg. */
	virtual int getSavedLightValue(LIGHTCHANNEL channel, const BlockPos& pos) { return 0; }

	/** Plays a sound at the entity's position. Args: entity, sound, volume and frequency (relative to 1.0) */
	void playSoundAtEntityByType(Entity* pEntity, SoundType soundType);

	/** Plays sound to all near players except the player reference given */
	void playSoundToNearExceptByType(EntityPlayer* pPlayer, SoundType soundType);

	/** Play a sound effect. Many many parameters for this function. */
	void playSoundEffectByType(const Vector3& pos, SoundType soundType);
	/** par8 is loudness, all pars passed to blockmanInstance.sndManager.playSound */
	virtual void playSoundOfPlaceBlock(const BlockPos& pos, int blockId) {}
	virtual void playSoundOfStepOnBlock(const BlockPos& pos, int blockId) {}
	virtual void playSoundByType(const Vector3& pos, SoundType soundType) {}
	/** Plays a record at the specified coordinates of the specified name. Args: recordName/music, x, y, z */
	void playRecord(const String& record, const BlockPos& pos);
	/** Spawns a particle.  Args particleName, x, y, z, velX, velY, velZ */
	virtual void spawnParticle(const String& name, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter = nullptr) {}
	/** play auxSFX (sound, particles, etc) */
	void playAuxSFX(int type, const BlockPos& pos, int data) { playAuxSFXAtEntity(NULL, type, pos, data); }
	/** play auxSFX (sound, particles, etc) on player. */
	virtual void playAuxSFXAtEntity(EntityPlayer* pPlayer, int type, const BlockPos& pos, int data);

	/** Gets the light value of a block location */
	virtual int getBlockLightValue(const BlockPos& pos) { return 0; }
	/** Brightness for SkyBlock.Sky is clear white and (through color computing it is assumed) DEPENDENT ON DAYTIME.
	Brightness for SkyBlock.Block is yellowish and independent. */
	virtual int getSkyBlockTypeBrightness(LIGHTCHANNEL channel, const BlockPos& pos) { return 0; }

	virtual bool canPlaySpecialSound(int sst, int max_tick = 0) { return false; }

	/** implement virtual function from IBlockAccess. */
	virtual int getBlockId(const BlockPos& pos);
	virtual int getBlockMeta(const BlockPos& pos);
	virtual int getBlockIDAndMeta(const BlockPos& pos);
	virtual TileEntity* getBlockTileEntity(const BlockPos& pos);
	virtual int getLightBrightnessForSkyBlocks(const BlockPos& pos, int minr, int ming, int minb) { return 0; }
	virtual float getBrightness(const BlockPos& pos, int minBrightness) { return 0.f; }
	virtual float getLightBrightness(const BlockPos& pos) { return 0.f; }
	virtual const BM_Material& getBlockMaterial(const BlockPos& pos);
	virtual bool isBlockOpaqueCube(const BlockPos& pos);
	virtual bool isBlockNormalCube(const BlockPos& pos);
	virtual bool isAirBlock(const BlockPos& pos) { return getBlockId(pos) == 0; }
	virtual int getHeight() const { return MAX_WORLD_Y; }
	virtual bool extendedLevelsInChunkCollection() { return false; }
	virtual bool doesBlockHaveSolidTopSurface(const BlockPos& pos);
	virtual int isBlockProvidingPowerTo(const BlockPos& pos, int dir);
	virtual void resetSignText(Vector3i vec3, int line, const char *text, bool isWordWrap = false);
	virtual void changePlayerActor(EntityPlayer* pPlayer, const String actorName, const String bodyId);
	virtual void changeEntityActor(Entity* entity, const String actorName);
	virtual void restorePlayerActor(EntityPlayer* player);

	virtual void addGoodsGroupToShop(i32 category, String icon, String name);
	virtual void addGoodsToShop(i32 category, Goods goods);
	virtual void addRespawnGoodsToShop(RespawnGoods goods);
	virtual void addRespawnGoodsToShopByUniqueId(RespawnGoods goods);

	virtual void resetVehiclePosition(int entityId, Vector3 pos);
	virtual void resetVehiclePositionAndYaw(int entityId, Vector3 pos, float yaw);
	virtual bool hasPlayerByVehicle(int entityId);

	virtual int tryAllPlayerTakeAircraft(Vector3 startPos, Vector3 endPos, float speed);
	virtual void tryAllPlayerOffAircraft();

	virtual void setPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2, float speed) {}
	virtual vector<Vector2>::type getPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2);

	virtual void fillAreaByBlockIdAndMate(const BlockPos & start_pos, const BlockPos & end_pos, int block_id, int mate = 0) {}
	virtual void handleLargeBlocks(const BlockModifys& blocks_info) {}
	virtual bool setCropsBlock(ui64 platformId, const BlockPos& pos, int blockID, int stage, int curStageTime, std::time_t lastUpdateTime, int residueHarvestNum =1) { return true; }
	virtual void updateManorOwner(ui64 rakssid, std::vector<ManorOwner>  owners);
	virtual Vector3 getPositionByEntityId(int entityId);
	void addCprFutureToList(CprHttpFuture cprFuture);
	virtual void updateLandNpc(i32 entityId, i32 status, i32 price, i32 totalTime, i32 timeLeft, std::vector<RanchCommon> recipe, std::vector<RanchCommon> reward) {}
	virtual bool rangeCheckBlock(i32 blockId, const BlockPos& startPos , const BlockPos& endPos);
	virtual bool rangeCheckBlockByEntityId(i32 entityId, i32 blockId) { return false;}
	virtual vector<EntityBird*>::type getPlayerBirds(ui64 userId);
	virtual void updateDecorationYaw(i32 entityId, float yaw);
	virtual bool getBlockRedStatusInThisWorld(const BlockPos& pos) { return false; }
	void setActorRotateSpeed(i32 entityId, float speed);
	//virtual bool hasEntitiesInPos(Vector3 beginPos, Vector3 endPos);
	virtual std::vector<int> hasEntitiesInPos(Vector3 beginPos, Vector3 endPos);

	virtual void createSchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
		bool reversalX = false, bool reversalZ = false, bool coverOldBlock = true,
		int placeBlockId = 0, int placeOffset = 0, int extraParam = 0);
	virtual void destroySchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
		bool reversalX = false, bool reversalZ = false, int placeOffset = 0, int extraParam = 0);

public:
	virtual int addAircraft(Vector3 pos, float yaw);
	virtual int addMerchant(Vector3 pos, float yaw, String nameLang);
	virtual int addVehicle(Vector3 pos, int carId, float yaw);
	virtual int addRankNpc(Vector3 pos, float yaw, String nameLang);
	virtual EntityActorNpc* addActorNpc(Vector3 pos, float yaw, String actorName);
	virtual EntitySessionNpc* addSessionNpc(Vector3 pos, float yaw, i32 sessionType, String actorName);
	virtual EntityCreature* addCreature(Vector3 pos, int monsterId, float yaw, String actorName);
	virtual EntityBlockmanEmpty* addRailCar(Vector3 pos, float yaw, String actorName, bool isRun);
	virtual int addActorCannon(Vector3 beginPos, Vector3 fallOnPos, String actionName, String body, String bodyId, float heightIncrement);
	virtual int addBuildNpc(Vector3& pos, float yaw, ui64 userId, i32 actorId);
	virtual int addLandNpc(Vector3 pos, float yaw, ui64 userId, i32 landCode, String nameLang, String actorName, String actorBody, String actorBodyId);
	virtual int addEntityBulletin(Vector3 pos, float yaw, i32 bulletinId, String name);
	virtual int addDecoration(Vector3& pos, float yaw, ui64 userId, i32 decorationId);
	virtual EntityCreatureBullet* addCreatureBullet(int throwerId, int targetId);
	virtual EntityFlyingMount* addFlyingMount(Vector3 pos, float yaw, int settingId);
	virtual EntityFloatingMount* addFloatingMount(Vector3 pos, float yaw, int settingId);
	virtual EntitySkillThrowable* addSkillThrowable(EntityLivingBase* pShooting);

	void setBlockResistance(int blockId, float resistance);

private:
	virtual EntityVehicle* newEntityVehicle(const Vector3& pos, int carId, float yaw);
};

}

#endif
