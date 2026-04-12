/********************************************************************
filename: 	SectionClient.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-06-08
*********************************************************************/
#ifndef __SECTION_CLIENT_HEADER__
#define __SECTION_CLIENT_HEADER__

/** header files from common. */
#include "World/Section.h"
#include "Util/BitSet.h"
#include "Util/Rwq/concurrentqueue.h"

namespace BLOCKMAN
{

class WorldClient;
class ChunkClient;

class SectionClient : public Section
{
	NBTB_DECLARE_FRIEND(SectionClient);
public:
	SectionClient(int yBase = 0);
	~SectionClient();

	void injectLight(int* heightmap, int maxHeight);
	void setWorld(WorldClient* w) { m_world = w; }
	void setChunk(ChunkClient* c) { m_chunk = c; }

	// implement virtual function from Section.
	virtual void setBlockID(const BlockPos& pos, int id) override;
	virtual void setBlockMeta(const BlockPos& pos, int meta) override;
	virtual void setExtSkylightValue(const BlockPos& pos, int skyLightValue) override;
	virtual int getExtSkylightValue(const BlockPos& pos) override;
	virtual void setExtBlocklightValue(const BlockPos& pos, int blockLightValue) override;
	virtual void setExtBlockLightRGB(const BlockPos& pos, int r, int g, int b) override;
	virtual int getExtBlocklightValue(const BlockPos& pos) override;
	virtual void getExtBlocklightRGB(const BlockPos& pos, int& r, int& g, int& b) override;
	virtual void reBuildVisibility() override;
	virtual SetVisibility* getVisibility()  override { return m_setVisibility; }
	virtual bool changeBlockRedStatusInThisSection(const BlockPos& pos, bool isRed) override;
	virtual bool getBlockRedStatusInThisSection(const BlockPos& pos) override;
	NibbleArray* getSkyLightArray() { return m_skylightArray; }
	bool isVisibilityBuild() const { return m_isVisSetBuild; }

	inline ui32 getBlockUI32Data(const BlockPos& pos);
	inline ui8 getLightUI8Data(const BlockPos& pos);
	inline void getLightUI32Data(const BlockPos& pos, ui32& skyLight, int& br, int& bg, int& bb);
	inline ui16  getLightUI16Data(const BlockPos& pos);

	inline SectionClient* getNearSection(int offsetX, int offsetY, int offsetZ) { return m_nearSections[(offsetX + 1) + (offsetY + 1) * 3 + (offsetZ + 1) * 9]; }
	inline static ui16 blockBiasCompress(const BlockPos& bias, bool alreadyChanged) { ui16 t = (bias.x << 8) | (bias.y << 4) | (bias.z); t |= alreadyChanged ? 0x8000 : 0; return t; }
	inline static ui16 blockBiasCompress(int x, int y, int z, bool alreadyChanged) { ui16 t = (x << 8) | (y << 4) | (z); t |= alreadyChanged ? 0x8000 : 0; return t; }
	inline static BlockPos blockBiasDecompress(ui16 v, bool& alreadyChanged) { alreadyChanged = ((v & 0x8000) != 0); return Vector3i(((v >> 8) & 0x0f), ((v >> 4) & 0x0f), (v & 0x0f)); }
	inline static ui32 secitonIDCompress(const BlockPos& sectionid) { return (sectionid.y << 28) | ((sectionid.x + 4096) << 14) | (sectionid.z + 4096); }
	inline static ui32 sectionIDCompress(int x, int y, int z) { return (y << 28) | ((x + 4096) << 14) | (z + 4096); }
	inline static BlockPos sectionIDDecompress(ui32 v) { return BlockPos(((v >> 14) & 0x3FFF) - 4096, ((v >> 28) & 0x0f), (v & 0x3FFF) - 4096);	}

	void updateLights();
	bool isLightingDirty() const {
		return m_dirtyBlocksIdx.size_approx() > 0;
	}
	void onModifyLight(const BlockPos& bias, ui32 prevID, ui32 newID);
	void estimateLight(const BlockPos& bias, ui32 newID, ui32& skylight, ui32& br, ui32& bg, ui32& bb);
	void caculateLight(const BlockPos& bias, ui32& skylight, ui32& blr, ui32& blg, ui32& lbb);
	void markBlockLightDirty(int biasx, int biasy, int biasz, bool alreadyChanged = false);
	void markNeighborLightDirty(int biasx, int biasy, int biasz);

public:
	SectionClient* m_nearSections[27];

protected:
	static vector<ui16>::type dirtyBlockBuffer;  // safe when engine restart!!!!
	static bool vizited[0xFFFF];  // safe when engine restart!!!!

	/** Set true if the section's visibility is build */
	bool m_isVisSetBuild = false;
	SetVisibility* m_setVisibility = nullptr;
	Mutex m_lock;

	moodycamel::ConcurrentQueue<ui16> m_dirtyBlocksIdx;

	ChunkClient* m_chunk = nullptr;
	WorldClient* m_world = nullptr;

	// The NibbleArray containing a block of Block-light data.
	NibbleArray* m_blocklightR = nullptr;
	NibbleArray* m_blocklightG = nullptr;
	NibbleArray* m_blocklightB = nullptr;
	// The NibbleArray containing a block of Sky-light data. 
	NibbleArray* m_skylightArray = nullptr;

	BitSet<4096> m_redFlags;
};

inline ui32 SectionClient::getBlockUI32Data(const BlockPos& pos)
{
	int id = getBlockID(pos);
	int meta = id == 0 ? 0 : getBlockMeta(pos);
	int blockLight = getExtBlocklightValue(pos);
	int skyLight = getExtSkylightValue(pos);
	// 32bit-data: skylight[4-bit] blocklightB[4-bit] blocklightG[4-bit] blocklightR[4-bit] meta[4bit] blockid[12bit]
	ui32 ret = id; 
	ret |= ui32(meta) << 12;
	ret |= ui32(blockLight) << 16; 
	ret |= ui32(skyLight) << 28;
	return ret;
}

inline ui8 SectionClient::getLightUI8Data(const BlockPos& pos)
{
	return getExtSkylightValue(pos);
}

inline void SectionClient::getLightUI32Data(const BlockPos& pos, ui32& skyLight, int& br, int& bg, int& bb)
{
	skyLight = getExtSkylightValue(pos);
	getExtBlocklightRGB(pos, br, bg, bb);
}

inline ui16 SectionClient::getLightUI16Data(const BlockPos& pos)
{
	int skyLight = getExtSkylightValue(pos);
	int blockLight = getExtBlocklightValue(pos);
	// lightvalue: skylight[4-bit] blocklightB[4-bit] blocklightG[4-bit] blocklightR[4-bit]
	return ui16((skyLight << 12) | blockLight);
}

NBTB_BEGIN(SectionClient, Section)
NBTB_END

}

#endif
