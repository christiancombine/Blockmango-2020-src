/********************************************************************
filename: 	TessMeshCollection.h
file path:	dev\client\Src\Blockman\Render

version:	1
author:		ajohn
company:	supernano
date:		2016-12-2
*********************************************************************/
#ifndef __TESS_MESH_COLLECTION_HEADER__
#define __TESS_MESH_COLLECTION_HEADER__

#include "World/IBlockAccess.h"
#include "World/World.h"

namespace BLOCKMAN
{

class ChunkClient;
class WorldClient;
class SectionClient;

class TessMeshCollection : public IBlockAccess, public ObjectAlloc
{
public:
	TessMeshCollection(WorldClient* pWorld);
	bool reSetSection(const BlockPos& sectionPos);
	bool buildData();

	/** implement virtual function from IBlockAccess. */
	virtual int getBlockId(const BlockPos& pos);
	virtual int getBlockMeta(const BlockPos& pos);
	virtual int getBlockIDAndMeta(const BlockPos& pos);
	virtual TileEntity* getBlockTileEntity(const BlockPos& pos);
	virtual int getLightBrightnessForSkyBlocks(const BlockPos& pos, int minr, int ming, int minb);
	virtual float getBrightness(const BlockPos& pos, int minBrightness);
	virtual float getLightBrightness(const BlockPos& pos);
	virtual const BM_Material& getBlockMaterial(const BlockPos& pos);
	virtual bool isBlockOpaqueCube(const BlockPos& pos);
	virtual bool isBlockNormalCube(const BlockPos& pos);
	virtual bool isAirBlock(const BlockPos& pos);
	virtual int getHeight() { return 256; }
	virtual bool extendedLevelsInChunkCollection() { return isEmpty; }
	virtual bool doesBlockHaveSolidTopSurface(const BlockPos& pos);
	virtual int isBlockProvidingPowerTo(const BlockPos& pos, int side);
	virtual bool isRedBlock(const BlockPos& pos);

	int getLightValue(const BlockPos& pos) { return getLightValueExt(pos, true); }
	int getLightValueExt(const BlockPos& pos, bool flag);
	int getBrightnessMixed(LIGHTCHANNEL channel, BlockPos pos);
	int getSpecialBlockBrightness(LIGHTCHANNEL channel, BlockPos pos);
	bool isInAsync() { return m_isInAsync; }


	ui32 blocks[20][20][20];

protected:
	bool checkBlockPos(const BlockPos& pos);

protected:
	int chunkX = 0;
	int chunkZ = 0;
	int chunkY = 0;

	BlockPos startPos;

	/** True if the chunk cache is empty. */
	bool isEmpty = false;
	/** True if the chunk is in async provide */
	bool m_isInAsync = false;

	/** Reference to the World object. */
	WorldClient* worldObj = nullptr;
	ChunkPtr chunkObj;
	SectionClient* sectionObj = nullptr;
};

}

#endif