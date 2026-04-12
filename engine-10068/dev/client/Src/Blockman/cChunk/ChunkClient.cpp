#include "ChunkClient.h"
#include "cWorld/SectionClient.h"

/** header files from common. */
#include "World/Section.h"
#include "World/World.h"
#include "World/NibbleArray.h"
#include "World/World.h"
#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Render/VisualGraph.h"
#include "Chunk/ChunkService.h"
#include "cWorld/WorldClient.h"

namespace BLOCKMAN
{

void ChunkClient::initialize2()
{
	memset(m_nearChunks, 0, sizeof(ChunkClient*) * 9);
}

ChunkClient::ChunkClient(World* pWorld, int x, int z)
	: Chunk(pWorld, x, z)
{
	initialize2();
}

ChunkClient::~ChunkClient()
{
	//LordLogInfo("~ChunkClient chunkID(%d,%d)", m_posX, m_posZ);
	WorldClient* pworld = dynamic_cast<WorldClient*>(m_pWorld);
	if(pworld)
		pworld->invalidateNearChunkRef(Vector2i(m_posX, m_posZ));
}

Section* ChunkClient::CreateSection(int x, int z, int yBase)
{
	SectionClient* presult = LordNew SectionClient(yBase);
	m_pSections[yBase] = presult;

	// updateNearSection(yBase);
	return presult;
}

SetVisibility* ChunkClient::getSetVisibility(int idx)
{
	if (idx < 0 || idx >= 16)
		return NULL;
	if (!m_pSections)
		return NULL;
	if (!m_pSections[idx])
	{
		return SetVisibility::s_Empty;
	}
	return m_pSections[idx]->getVisibility();
}

int ChunkClient::getSectionCnt()
{
	int cnt = 0;
	for (int i = 0; i < 16; ++i)
	{
		if (m_pSections[i] != nullptr)
			++cnt;
	}
	return cnt;
}

bool ChunkClient::setBlocks(const BlockModifys& modifys)
{
	// may be some new section created.
	int prevCnt = getSectionCnt();
	bool ret = Chunk::setBlocks(modifys);
	int currectCnt = getSectionCnt();
	
	if (prevCnt != currectCnt)
	{
		initSectionPos();

		WorldClient* clientWorld = (WorldClient*)m_pWorld;
		clientWorld->updateNearChunkRef(this);
	}

	return ret;
}

bool ChunkClient::setBlockIDAndMeta(const BlockPos& pos, int blockID, int meta, bool immediate)
{
	int prevCnt = getSectionCnt();
	bool ret = Chunk::setBlockIDAndMeta(pos, blockID, meta, immediate);
	int currectCnt = getSectionCnt();

	if (prevCnt != currectCnt)
	{
		initSectionPos();

		WorldClient* clientWorld = (WorldClient*)m_pWorld;
		clientWorld->updateNearChunkRef(this);
	}

	return ret;
}

void ChunkClient::generateSkylightMap()
{
	WorldClient* cworld = (WorldClient*)m_pWorld;
	WorldClient::PreLightDirtMap& prelightmap = cworld->m_preLightDirtMap;
	bool changed = false;

	for (int i = 0; i < 16; ++i)
	{
		SectionClient* pSection = (SectionClient*)(m_pSections[i]);
		if (pSection == nullptr)
			continue;
		
		pSection->injectLight(m_heightMap, m_maxHeight);

		Vector3i sectionid(m_posX, i, m_posZ);
		auto it = prelightmap.find(SectionClient::sectionIDCompress(sectionid.x, sectionid.y, sectionid.z));
		if (it != prelightmap.end())
		{
			for (auto ibias : it->second)
			{
				Vector3i bias = SectionClient::blockBiasDecompress(ibias, changed);
				pSection->markBlockLightDirty(bias.x, bias.y, bias.z);
			}
			prelightmap.erase(it);
		}
	}
}

void ChunkClient::updateNearSection(int idx)
{
	SectionClient* pSection = (SectionClient*)m_pSections[idx];
	if (pSection == nullptr)
		return;

	pSection->setChunk(this);
	pSection->setWorld((WorldClient*)(m_pWorld));

	Vector3i sectionID(m_posX, idx, m_posZ);
	for (int zOffset = -1; zOffset <= 1; ++zOffset)
	{
		for (int yOffset = -1; yOffset <= 1; ++yOffset)
		{
			for (int xOffset = -1; xOffset <= 1; ++xOffset)
			{
				int nearIndex = (xOffset + 1) + (yOffset + 1) * 3 + (zOffset + 1) * 9;

				Vector3i nearsecid = sectionID + Vector3i(xOffset, yOffset, zOffset);

				ChunkClient* nearChunk = nullptr;
				if (m_pWorld->m_pChunkService->chunkInCache(nearsecid.x, nearsecid.z))
					nearChunk = std::static_pointer_cast<ChunkClient>(m_pWorld->getChunkService()->getChunk(nearsecid.x, nearsecid.z)).get();

				if (nearChunk)
				{
					if (nearsecid.y >= 0 && nearsecid.y < 16)
						pSection->m_nearSections[nearIndex] = (SectionClient*)(nearChunk->m_pSections[nearsecid.y]);
					else
						pSection->m_nearSections[nearIndex] = nullptr;
				}
				else
					pSection->m_nearSections[nearIndex] = nullptr;
			}
		}
	}
}

void ChunkClient::invalidNearSection(const Vector2i& chunkid, int idx)
{
	SectionClient* pSection = (SectionClient*)m_pSections[idx];
	if (pSection == nullptr)
		return;

	Vector3i sectionID(m_posX, idx, m_posZ);
	for (int zOffset = -1; zOffset <= 1; ++zOffset)
	{
		for (int yOffset = -1; yOffset <= 1; ++yOffset)
		{
			for (int xOffset = -1; xOffset <= 1; ++xOffset)
			{
				int nearIndex = (xOffset + 1) + (yOffset + 1) * 3 + (zOffset + 1) * 9;

				Vector3i nearsecid = sectionID + Vector3i(xOffset, yOffset, zOffset);
				if (chunkid.x == nearsecid.x && chunkid.y == nearsecid.z && nearsecid.y >=0 && nearsecid.y < 16)
				{
					pSection->m_nearSections[nearIndex] = nullptr;
				}
			}
		}
	}
}

void ChunkClient::updateNearSectionRef()
{
	for (int i = 0; i < 16; ++i)
	{
		if (m_pSections[i] == nullptr)
			continue;

		updateNearSection(i);
	}
}

void ChunkClient::invalidNearSectionRef(const Vector2i& chunkid)
{
	for (int i = 0; i < 16; ++i)
	{
		if (m_pSections[i] == nullptr)
			continue;

		invalidNearSection(chunkid, i);
	}
}

bool ChunkClient::checkNeighborEmpty(int sectionY)
{
	for (int i = 0; i < 9; ++i)
	{
		if (m_nearChunks[i] == nullptr)
			return true;
	}

	Section* pSection = m_pSections[sectionY];
	if (!pSection)
		return true;
	return false;
}

int ChunkClient::getSavedLightValue(LIGHTCHANNEL channel, const BlockPos& pos)
{
	Section* pSection = m_pSections[pos.y >> 4];

	if (pSection == NULL)
	{
		return canBlockSeeTheSky(pos) ? 0xF : 0;
	}
	else
	{
		if (channel == LC_SKY)
		{
			return pSection->getExtSkylightValue(pos.getSection());
		}
		else
		{
			int blocklight = pSection->getExtBlocklightValue(pos.getSection());
			return (blocklight >> (channel) * 4) & 0xF;
		}
		return 0;
	}
}

bool ChunkClient::changeBlockRedStatusInThisChunk(const BlockPos& pos, bool isRed)
{
	Section* pSection = m_pSections[pos.y >> 4];
	if (pSection)
	{
		return pSection->changeBlockRedStatusInThisSection(pos.getSection(), isRed);
	}
	return false;
}

bool ChunkClient::getBlockRedStatusInThisChunk(const BlockPos & pos)
{
	Section* pSection = m_pSections[pos.y >> 4];
	if (pSection)
	{
		return pSection->getBlockRedStatusInThisSection(pos.getSection());
	}
	return false;
}

//
//bool ChunkClient::setBlocks(const BlockModifys& modifys)
//{
//	bool ret = Chunk::setBlocks(modifys);
//
//	// need notify all the section to rebuild the meshs.
//
//
//}

int ChunkClient::getBlockLightValue(const BlockPos& pos, int l)
{
	Section* pSection = m_pSections[pos.y >> 4];

	if (pSection == NULL)
	{
		return l < 0xF ? 0xF - l : 0;
	}
	else
	{
		int ret = pSection->getExtSkylightValue(pos.getSection());

		ret -= l;
		int r, g, b;
		pSection->getExtBlocklightRGB(pos.getSection(), r,g,b);
		int brightness = Math::Max(r, Math::Max(g, b));

		if (brightness > ret)
		{
			ret = brightness;
		}

		return ret;
	}
}

void ChunkClient::reBuildVisibility(int idx)
{
	Section* pSection = m_pSections[idx];
	if (!pSection)
		return;
	pSection->reBuildVisibility();
}

void ChunkClient::reBuildVisibility()
{
	// for every section, rebuild the visualGraph
	for (int i = 0; i < 16; ++i)
	{
		Section* pSection = m_pSections[i];
		if (!pSection)
			continue;
		pSection->reBuildVisibility();
	}
}
//
//void ChunkClient::initDefaut()
//{
//	if (m_pSections[0] == nullptr)
//		m_pSections[0] = new SectionClient(0);
//
//	for (int x = 0; x < 16; ++x)
//	{
//		for (int z = 0; z < 16; ++z)
//		{
//			m_pSections[0]->setBlockID(BlockPos(x, 0, z), 7);
//		}
//	}
//}

}
