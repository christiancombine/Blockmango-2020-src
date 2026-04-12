/********************************************************************
filename: 	WorldClient.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-4
*********************************************************************/
#ifndef __WORLD_CLIENT_HEADER__
#define __WORLD_CLIENT_HEADER__

/** header files from common. */
#include "World/World.h"
#include "Util/Concurrent/Future.h"
#include "cChunk/ChunkClient.h"
#include "Util/Rwq/concurrentqueue.h"

namespace BLOCKMAN
{

using Vector3iBoolMap = std::vector<std::pair<Vector3i, bool>>;

class WorldClient : public World
{
	friend class BLOCKMAN::ChunkClient;
public:
	typedef set<int>::type DirtySectionVisibility;
	typedef map<ui32, vector<ui16>::type >::type PreLightDirtMap;
	typedef vector<Future<ChunkPtr> >::type ChunkClientArr;
	PreLightDirtMap m_preLightDirtMap;

protected:
	int m_cloudColour = 0; //  = 0xFFFFFF

	float m_prevRainingStrength = 0.f;
	float m_rainingStrength = 0.f;
	float m_prevThunderingStrength = 0.f;
	float m_thunderingStrength = 0.f;

	bool m_isDestroying = false;

	/**
	* is a temporary list of blocks and light values used when updating light levels. Holds up to 32x32x32 blocks (the
	* maximum influence of a light source.) Every element is a packed bit value: 0000000000LLLLzzzzzzyyyyyyxxxxxx. The
	* 4-bit L is a light level used when darkening blocks. 6-bit numbers x, y and z represent the block's offset from
	* the original block, plus 32 (i.e. value of 31 would mean a -1 offset
	*/
	int* m_lightUpdateBlockList = nullptr;
	// int* m_lightUpdateBlockList_thread;

	moodycamel::ConcurrentQueue<int> m_dirtySectionVisibility;

	ChunkClientArr chunkAsyncArr;
public:
	void init2();
	WorldClient(const String& name, const WorldSettings& settings, int loadRange);
	virtual ~WorldClient();

	virtual ChunkService* createChunkService(int loadRange) override;

	/** Returns the render type of the block at the given coordinate. */
	int blockGetRenderType(const BlockPos& pos);

	float getWeightedThunderStrength(float rdt) { return (m_prevThunderingStrength + (m_thunderingStrength - m_prevThunderingStrength) * rdt) * getRainStrength(rdt); }
	float getRainStrength(float rdt) { return m_prevRainingStrength + (m_rainingStrength - m_prevRainingStrength) * rdt; }
	void setRainStrength(float rdt) { m_prevRainingStrength = rdt; m_rainingStrength = rdt; }
	
	void applyRebuildVisibility();
	void updateNearChunkRef(BLOCKMAN::ChunkClient* pchunk);
	void invalidateNearChunkRef(const Vector2i& chunkid);
	void asyncLoadChunks(const Vector3& playerPos, int chunkRange);

	/**
	* Gets the light value of a block location. This is the actual function that gets the value and has a bool flag
	* that indicates if its a half step block to get the maximum light value of a direct neighboring block (left,
	* right, forward, back, and up)
	*/
	int getBlockLightValue_impl(const BlockPos& pos, bool flag);
				
	/** Plays a sound at the entity's position. Args: entity, sound, volume, and pitch. */
	void playSoundAtEntityByType(Entity* pEntity, SoundType soundTyp);
	
	/** Returns the sun brightness - checks time of day, rain and thunder */
	float getSunBrightness(float rdt);

	/** Calculates the color for the skybox */
	Color getSkyColor(Entity* pEntity, float rdt);
	Color getSkyColor(float rdt) { return Color(0.4f, 0.6f, 0.9f); }

	Color getCloudColour(float rdt);

	/** Returns vector(ish) with R/G/B for fog	*/
	Color getFogColor(float rdt);
	
	/** How bright are stars in the sky */
	float getStarBrightness(float rdt);

	void calculateInitialWeather();

	/** Runs a single tick for the world */
	virtual void tick();

	/*** Updates all weather states. */
	void updateWeather();

	float getMoonPhaseFactor();

	void doVoidFogParticles(BlockPos pos);
	ui64 findManorByPlayerPosition(EntityPlayer* player);

	struct ChunkInfo
	{
		ChunkInfo(int x, int z) :chunkX(x), chunkZ(z) {}
		int chunkX = 0;
		int chunkZ = 0;

		bool operator< (const ChunkInfo& c) const
		{
			if (chunkX != c.chunkX)
			{
				return chunkX < c.chunkX;
			}
			else if (chunkZ != c.chunkZ)
			{
				return chunkZ < c.chunkZ;
			}
			return chunkZ < c.chunkZ;
		}
	};

	static const int MAX_SPECIAL_SOUND_TYPE = 4;
	const unsigned int MAX_SPECIAL_SOUND_TICK = 100;
	const unsigned int MAX_SPECIAL_SOUND_TICK_LIMIT = 1000;
	int m_special_sound_tick[MAX_SPECIAL_SOUND_TYPE];

	enum SpecialSoundType
	{
		SST_WATER = 0,
		SST_FIRE = 1,
		SST_SWIM = 2,
		SST_LIQUID_SPLASH = 3,
		SST_COUNT,
	};

	bool canPlaySpecialSound(int sst, int max_tick = 0);
	void updateSpecialSound();

	/** implement virtual function from World*/
	virtual int  getFullBlockLightValue(const BlockPos& pos) override;
	virtual int  getSavedLightValue(LIGHTCHANNEL channel, const BlockPos& pos) override;
	virtual void spawnParticle(const String& name, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter = nullptr) override;
	virtual int  getBlockLightValue(const BlockPos& pos) override { return getBlockLightValue_impl(pos, true); }
	virtual int  getSkyBlockTypeBrightness(LIGHTCHANNEL channel, const BlockPos& pos) override;
	virtual int  getLightBrightnessForSkyBlocks(const BlockPos& pos, int minR, int minG, int minB) override;
	virtual float getBrightness(const BlockPos& pos, int minBrightness) override;
	virtual float getLightBrightness(const BlockPos& pos) override;
	virtual bool setBlock(const BlockPos& pos, int blockID, int metadata, int flag, bool immediate = true) override;
	virtual bool setBlocks(int x, int z, const BlockModifys& modifys) override;
	virtual void playSoundOfPlaceBlock(const BlockPos& pos, int blockId) override;
	virtual void playSoundOfStepOnBlock(const BlockPos& pos, int blockId) override;
	virtual void playSoundByType(const Vector3& pos, SoundType soundType) override;
	virtual void setPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2, float speed) override;
	virtual float getFrameDeltaTime() override;
	virtual bool getBlockRedStatusInThisWorld(const BlockPos& pos) override;

	void createSchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
		bool reversalX = false, bool reversalZ = false, bool coverOldBlock = true,
		int placeBlockId = 0, int placeOffset = 0, int extraParam = 0) override;
	void destroySchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
		bool reversalX = false, bool reversalZ = false, int placeOffset = 0, int extraParam = 0) override;
	void processSchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
		bool createOrDestroy, bool reversalX = false, bool reversalZ = false, bool coverOldBlock = true,
		int placeBlockId = 0, int placeOffset = 0, int extraParam = 0);
	
	void findPlaceCoverResult(Vector3iBoolMap& placeCoverResult, int pos_x, int pos_y, int pos_z, bool &bRet);
	// fill blocks from start_pos to end_pos.
	void fillAreaByBlockIdAndMate(const BlockPos& start_pos, const BlockPos& end_pos, int block_id, int mate = 0) override;
	// handle large blocks
	void handleLargeBlocks(const BlockModifys& blocks_info) override;
	
	bool checkRangeBlockIsAir(const Vector3& beginPos, const Vector3& endPos);

private:
	EntityVehicle* newEntityVehicle(const Vector3& pos, int carId, float yaw) override;
};

}

#endif
