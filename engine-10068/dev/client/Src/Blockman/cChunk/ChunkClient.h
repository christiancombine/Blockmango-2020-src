/********************************************************************
filename: 	ChunkClient.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-06-08
*********************************************************************/
#ifndef __CHUNK_CLIENT_HEADER__
#define __CHUNK_CLIENT_HEADER__

/** header files from common. */
#include "Chunk/Chunk.h"
#include "Functional/StaticCastOp.h"
#include "Functional/AssignOp.h"
#include "cWorld/SectionClient.h"

namespace BLOCKMAN
{
	
class SectionClient;
class SetVisibility;

class ChunkClient : public Chunk
{
public:
	void initialize2();

	ChunkClient* m_nearChunks[9];

protected:
	virtual Section* CreateSection(int x, int z, int yBase);
	int getSectionCnt();

public:
	ChunkClient() : ChunkClient(nullptr, 0, 0) {};
	ChunkClient(World* pWorld, int x, int z);
	virtual ~ChunkClient();

	SetVisibility* getSetVisibility(int idx);
	//void initDefaut();
	inline ChunkClient* getNearChunk(int offsetX, int offsetZ) { return m_nearChunks[(offsetX+1) + (offsetZ+1) * 3]; }
	inline ChunkClient* getNearChunk_Raw(int offsetX, int offsetZ) { return m_nearChunks[offsetX + offsetZ * 3]; }
	void updateNearSectionRef();
	void invalidNearSectionRef(const Vector2i& chunkid);
	void updateNearSection(int idx);
	void invalidNearSection(const Vector2i& chunkid, int idx);
	bool checkNeighborEmpty(int sectionY);


	void reBuildVisibility();
	/** implement virtual function from Chunk. */
	virtual bool setBlocks(const BlockModifys& modifys) override;
	virtual bool setBlockIDAndMeta(const BlockPos& pos, int blockID, int meta, bool immediate = true) override;
	virtual void generateSkylightMap() override;
	virtual void reBuildVisibility(int idx) override;
	virtual int getSavedLightValue(LIGHTCHANNEL channel, const BlockPos& pos) override;
	virtual int getBlockLightValue(const BlockPos& pos, int l) override;
	virtual bool changeBlockRedStatusInThisChunk(const BlockPos& pos, bool isRed) override;
	virtual bool getBlockRedStatusInThisChunk(const BlockPos& pos) override;
	// virtual bool setBlocks(const BlockModifys& modifys) override;
};

NBTB_BEGIN(ChunkClient, Chunk)
NBTB_ALIAS(m_pSections, "Sections", NBTB_LIST(NBTB_COMPOUND(SectionClient)), NBTB_CONVERTERS(assign_op, static_cast_op<SectionClient*>()))
NBTB_END

}

#endif
