#include "SectionClient.h"

/** header files from common. */
#include "World/Section.h"
#include "World/NibbleArray.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Render/VisualGraph.h"
#include "cChunk/ChunkClient.h"
#include "cWorld/WorldClient.h"

namespace BLOCKMAN
{

vector<ui16>::type SectionClient::dirtyBlockBuffer;
bool SectionClient::vizited[0xFFFF] = { 0 };
const Vector3i offsetTable_6Neighbours[6] =
{
	Vector3i(1, 0, 0),	//Side_Right = 0,
	Vector3i(-1, 0, 0),	//Side_Left,
	Vector3i(0, 0, 1),	//Side_Front,
	Vector3i(0, 0, -1),	//Side_Back,
	Vector3i(0, 1, 0),	//Side_Top,
	Vector3i(0, -1, 0),	//Side_Bottom,
};

SectionClient::SectionClient(int yBase)
	: Section(yBase)
	, m_blocklightR(NULL)
	, m_blocklightG(NULL)
	, m_blocklightB(NULL)
	, m_skylightArray(NULL)
	, m_isVisSetBuild(false)
{
	m_setVisibility = new SetVisibility();
	m_blocklightR = LordNew NibbleArray(Section::BLOCK_ARRAY_SIZE, 4);
	m_blocklightG = LordNew NibbleArray(Section::BLOCK_ARRAY_SIZE, 4);
	m_blocklightB = LordNew NibbleArray(Section::BLOCK_ARRAY_SIZE, 4);
	m_skylightArray = LordNew NibbleArray(Section::BLOCK_ARRAY_SIZE, 4);
	m_redFlags.reset();
	memset(m_nearSections, 0, sizeof(SectionClient*) * 27);
}

SectionClient::~SectionClient()
{
	delete m_setVisibility;
	LordSafeDelete(m_blocklightR);
	LordSafeDelete(m_blocklightG);
	LordSafeDelete(m_blocklightB);
	LordSafeDelete(m_skylightArray);
}

void SectionClient::setExtSkylightValue(const BlockPos& pos, int skylightvalue)
{
	LordAssertX(m_skylightArray, "m_skylightArray is NULL!");
	m_skylightArray->set(pos, skylightvalue);
}

int SectionClient::getExtSkylightValue(const BlockPos& pos)
{
	LordAssertX(m_skylightArray, "m_skylightArray is NULL!");
	return m_skylightArray->get(pos);
}

void SectionClient::setExtBlocklightValue(const BlockPos& pos, int blocklightvalue)
{
	LordAssertX(m_blocklightR, "m_blocklightR is NULL!");
	LordAssertX(m_blocklightG, "m_blocklightG is NULL!");
	LordAssertX(m_blocklightB, "m_blocklightB is NULL!");
	ui32 br = blocklightvalue & 0xF;
	ui32 bg = (blocklightvalue >> 4) & 0xF;
	ui32 bb = (blocklightvalue >> 8) & 0xF;
	m_blocklightR->set(pos, br);
	m_blocklightG->set(pos, bg);
	m_blocklightB->set(pos, bb);
}

void SectionClient::setExtBlockLightRGB(const BlockPos& pos, int r, int g, int b)
{
	LordAssertX(m_blocklightR, "m_blocklightR is NULL!");
	LordAssertX(m_blocklightG, "m_blocklightG is NULL!");
	LordAssertX(m_blocklightB, "m_blocklightB is NULL!");
	m_blocklightR->set(pos, r);
	m_blocklightG->set(pos, g);
	m_blocklightB->set(pos, b);
}

int SectionClient::getExtBlocklightValue(const BlockPos& pos)
{
	LordAssertX(m_blocklightR, "m_blocklightR is NULL!");
	LordAssertX(m_blocklightG, "m_blocklightG is NULL!");
	LordAssertX(m_blocklightB, "m_blocklightB is NULL!");
	ui32 br = m_blocklightR->get(pos);
	ui32 bg = m_blocklightG->get(pos);
	ui32 bb = m_blocklightB->get(pos);
	return (bb << 8) | (bg << 4) | br;
}

void SectionClient::getExtBlocklightRGB(const BlockPos& pos, int& r, int& g, int& b)
{
	LordAssertX(m_blocklightR, "m_blocklightR is NULL!");
	LordAssertX(m_blocklightG, "m_blocklightG is NULL!");
	LordAssertX(m_blocklightB, "m_blocklightB is NULL!");
	r = m_blocklightR->get(pos);
	g = m_blocklightG->get(pos);
	b = m_blocklightB->get(pos);
}

void SectionClient::setBlockID(const BlockPos& pos, int id)
{
	m_lock.Lock();
	Section::setBlockID(pos, id);
	m_isVisSetBuild = false;
	m_lock.Unlock();
}

void SectionClient::setBlockMeta(const BlockPos& pos, int meta)
{
	m_lock.Lock();
	Section::setBlockMeta(pos, meta);
	m_isVisSetBuild = false;
	m_lock.Unlock();
}

void SectionClient::reBuildVisibility()
{
	if (m_isVisSetBuild)
		return;

	Block** pBlocks = BlockManager::sBlocks;
	VisualGraph visGraph;
	m_lock.Lock();

	for (int x = 0; x < 16; ++x)
	{
		for (int z = 0; z < 16; ++z)
		{
			for (int y = 0; y < 16; ++y)
			{
				BlockPos pos(x, y, z);
				int blockID = getBlockID(pos);
				if (blockID > 0 && pBlocks[blockID]->isOpaqueCube())
				{
					visGraph.setOpaque(pos);
				}
			}
		}
	}
	m_lock.Unlock();
	visGraph.computeVisibility(m_setVisibility);
	m_isVisSetBuild = true;
}

bool SectionClient::changeBlockRedStatusInThisSection(const BlockPos& pos, bool isRed)
{
	bool result = false;
	if (pos.x >= 0 && pos.x < 16 && pos.y >= 0 && pos.y < 16 && pos.z >= 0 && pos.z < 16)
	{
		int index = pos.y << 8 | pos.z << 4 | pos.x;
		bool currRedStatus = m_redFlags[index];
		if (currRedStatus != isRed)
		{
			if (isRed)
			{
				m_redFlags.set(index);
			}
			else
			{
				m_redFlags.reset(index);
			}
			result = true;
		}
	}
	return result;
}

bool SectionClient::getBlockRedStatusInThisSection(const BlockPos & pos)
{
	if (pos.x >= 0 && pos.x < 16 && pos.y >= 0 && pos.y < 16 && pos.z >= 0 && pos.z < 16)
	{
		int index = pos.y << 8 | pos.z << 4 | pos.x;
		return m_redFlags[index];
	}
	return false;
}

void SectionClient::injectLight(int* heightmap, int maxHeight)
{
	for (int z = 0; z < 16; ++z)
	{
		for (int x = 0; x < 16; ++x)
		{
			int h = heightmap[(z << 4) | x];

			for (int y = 0; y < 16; ++y)
			{
				int worldy = m_yBase * 16 + y;
				int blockID = getBlockID(BlockPos(x, y, z));

				ui32 skylight = (worldy <= h) ? 0 : 15;
				ui32 blockLight = BlockManager::sLightValue[blockID];
				ui32 br = blockLight & 0xF;
				ui32 bg = (blockLight >> 4) & 0xF;
				ui32 bb = (blockLight >> 8) & 0xF;

				m_skylightArray->set(x, y, z, skylight);

				m_blocklightR->set(x, y, z, br);
				m_blocklightG->set(x, y, z, bg);
				m_blocklightB->set(x, y, z, bb);

				if (skylight == 0 && BlockManager::sLightOpacity[blockID] != 15)
				{
					ui16 biasCompressed = (x << 8) | (y << 4) | (z);
					m_dirtyBlocksIdx.enqueue(biasCompressed);
				}
				if (blockLight > 0)
					markNeighborLightDirty(x, y, z);
			}
		}
	}
}

void SectionClient::onModifyLight(const BlockPos& bias, ui32 prevID, ui32 newID)
{
	ui32 skylight = 0, br = 0, bg = 0, bb = 0;
	estimateLight(bias, newID, skylight, br, bg, bb);

	// lightvalue: skylight[4-bit] blocklightB[4-bit] blocklightG[4-bit] blocklightR[4-bit]
	ui16 correctLightValue = ui8(skylight << 12) | (bb << 8) | (bg << 4) | br;
	ui16 originLightValue = getLightUI16Data(bias);
	
	setExtSkylightValue(bias, skylight);
	setExtBlockLightRGB(bias, br, bg, bb);

	bool alreadyChanged = (correctLightValue != originLightValue);
	markBlockLightDirty(bias.x, bias.y, bias.z, alreadyChanged);

	ui32 prevBlockLight = BlockManager::sLightValue[prevID];
	ui32 newBLockLight = BlockManager::sLightValue[newID];
	int prevR = prevBlockLight & 0xF;
	int prevG = (prevBlockLight >> 4) & 0xF;
	int prevB = (prevBlockLight >> 8) & 0xF;

	int newR = newBLockLight & 0xF;
	int newG = (newBLockLight >> 4) & 0xF;
	int newB = (newBLockLight >> 8) & 0xF;

	if (prevR < newR || prevG < newG || prevB < newB)
		markNeighborLightDirty(bias.x, bias.y, bias.z);
}

void SectionClient::estimateLight(const BlockPos& bias, ui32 newID, ui32& skylight, ui32& br, ui32& bg, ui32& bb)
{
	i32 wldy = m_yBase * 16 + bias.y;

	ui8 h = m_chunk->getHeightValue(bias.x, bias.z);

	skylight = (wldy <= h) ? 0 : 15;
	int blocklight = BlockManager::sLightValue[newID];
	br = blocklight & 0xF;
	bg = (blocklight >> 4) & 0xF;
	bb = (blocklight >> 8) & 0xF;
}

void SectionClient::updateLights()
{
	if (m_dirtyBlocksIdx.size_approx() <= 0)
		return;

	dirtyBlockBuffer.clear();

	ui16 tmpIdx;
	size_t maxCntOnce = 2000;
	size_t cnt = 0;
	while (m_dirtyBlocksIdx.try_dequeue(tmpIdx)) {
		dirtyBlockBuffer.push_back(tmpIdx);
		++cnt;
		if (cnt >= maxCntOnce) {
			break;
		}
	}

	memset(vizited, 0, sizeof(vizited));
	int lightChangedBlocks = 0;
	ui8 sideFlag[6] = { 0, 0, 0, 0, 0, 0 };

	for (size_t i = 0; i < dirtyBlockBuffer.size(); i++)
	{
		ui16 cur = dirtyBlockBuffer[i];

		if (!vizited[cur])
		{
			bool alreadyChanged;
			Vector3i bias = blockBiasDecompress(cur, alreadyChanged);

			ui32 skylight, br, bg, bb;
			caculateLight(bias, skylight, br, bg, bb);

			// lightvalue: skylight[4-bit] blocklightB[4-bit] blocklightG[4-bit] blocklightR[4-bit]
			ui16 correctLightValue = ui16(skylight << 12) | (bb << 8) | (bg << 4) | br;
			ui16 originLightValue = getLightUI16Data(bias);

			//if ((correctLightValue & 0x000000ff) != (originLightValue & 0x000000ff) || alreadyChanged)
			if ((originLightValue != correctLightValue) || alreadyChanged)
			{
				lightChangedBlocks++;
				markNeighborLightDirty(bias.x, bias.y, bias.z);
				setExtSkylightValue(bias, skylight);
				setExtBlockLightRGB(bias, br, bg, bb);
				if (bias.x == 15) sideFlag[0] = 1;
				if (bias.x == 0 ) sideFlag[1] = 1;
				if (bias.z == 15) sideFlag[2] = 1;
				if (bias.z == 0 ) sideFlag[3] = 1;
				if (bias.y == 15) sideFlag[4] = 1;
				if (bias.y == 0 ) sideFlag[5] = 1;
			}
		}

		vizited[cur] = true;
	}

	if (lightChangedBlocks > 0)
	{
		m_world->markSectionForUpdate(BlockPos(m_posX, m_yBase, m_posZ));
		for (int i = 0; i < 6; ++i)
		{
			if (sideFlag[i] == 1)
			{
				int ox = offsetTable_6Neighbours[i].x;
				int oy = offsetTable_6Neighbours[i].y;
				int oz = offsetTable_6Neighbours[i].z;
				m_world->markSectionForUpdate(BlockPos(m_posX + ox, m_yBase + oy, m_posZ + oz));
				//RequestUpdateMesh(tileblock->GetNearTile(ox, oy, oz), EMeshGroup(eDG_All & (~eDG_Fx)));
			}
		}
	}
}

void SectionClient::caculateLight(const BlockPos& bias, ui32& skylight, ui32& blr, ui32& blg, ui32& blb)
{
	i32 baserelx = bias.x + 16;
	i32 baserelz = bias.z + 16;
	i32 basewldy = bias.y + m_yBase * 16;

	int blockid = getBlockID(bias);
	int absorb = BlockManager::sLightOpacity[blockid];
	int luminance = BlockManager::sLightValue[blockid];
	ui32 luminanceR = luminance & 0xF;
	ui32 luminanceG = (luminance >> 4) & 0xF;
	ui32 luminanceB = (luminance >> 8) & 0xF;
	if (absorb < 15 || luminance != 0)
	{
		ui32 nearskylight = 0;
		ui32 nearpointlightR = 0;
		ui32 nearpointlightG = 0;
		ui32 nearpointlightB = 0;

		for (int i = 0; i < 6; i++)
		{
			Vector3i offset = offsetTable_6Neighbours[i];

			i32 relx = baserelx + offset.x;
			i32 relz = baserelz + offset.z;
			i32 wldy = basewldy + offset.y;

			ui32 sky = 0;
			int br = 0, bg = 0, bb = 0;

			ChunkClient* nearchunk = m_chunk->getNearChunk((int)(relx >> 4) - 1, (int)(relz >> 4) - 1);
			if (nearchunk != nullptr && wldy >= 0 && wldy < 256)
			{
				i32 biasx = relx & 0x0f;
				i32 biasy = wldy & 0x0f;
				i32 biasz = relz & 0x0f;

				SectionClient* sectionlight = (SectionClient*)(nearchunk->m_pSections[wldy >> 4]);
				if (sectionlight)
				{
					sectionlight->getLightUI32Data(BlockPos(biasx, biasy, biasz), sky, br, bg, bb);
				}
				else
				{
					skylight = 15;
					br = 0;
					bg = 0;
					bb = 0;
				}
			}
			else
			{
				sky = 0;
				br = 0;
				bg = 0;
				bb = 0;
			}

			nearskylight = (sky > nearskylight) ? (sky) : (nearskylight);
			nearpointlightR = (br > nearpointlightR) ? (br) : (nearpointlightR);
			nearpointlightG = (bg > nearpointlightG) ? (bg) : (nearpointlightG);
			nearpointlightB = (bb > nearpointlightB) ? (bb) : (nearpointlightB);
		}

		ui8 h = m_chunk->getHeightValue(bias.x, bias.z);

		ui32 selfskylight = (ui32)((basewldy <= h) ? 0 : 15);

		ui32 skylightfromnear = nearskylight > absorb ? nearskylight - absorb : 0;
		ui32 pointlightfromnearR = nearpointlightR > absorb ? nearpointlightR - absorb : 0;
		ui32 pointlightfromnearG = nearpointlightG > absorb ? nearpointlightG - absorb : 0;
		ui32 pointlightfromnearB = nearpointlightB > absorb ? nearpointlightB - absorb : 0;

		skylight = Math::Max(selfskylight, skylightfromnear);	// MULTIPLY BY HBAO
		blr = Math::Max(luminanceR, pointlightfromnearR);
		blg = Math::Max(luminanceG, pointlightfromnearG);
		blb = Math::Max(luminanceB, pointlightfromnearB);
	}
	else
	{
		skylight = 0;
		blr = 0;
		blg = 0;
		blb = 0;
	}
}


void SectionClient::markBlockLightDirty(int biasx, int biasy, int biasz, bool alreadyChanged)
{
	ui16 biasCompressed = (biasx << 8) | (biasy << 4) | (biasz);
	biasCompressed |= alreadyChanged ? 0x8000 : 0;
	m_dirtyBlocksIdx.enqueue(biasCompressed);
}

void SectionClient::markNeighborLightDirty(int biasx, int biasy, int biasz)
{
	i32 baserelx = biasx + 16;
	i32 baserelz = biasz + 16;
	i32 basewldy = biasy + m_yBase * 16;

	for (int i = 0; i < 6; i++)
	{
		Vector3i offset = offsetTable_6Neighbours[i];

		i32 relx = baserelx + offset.x;
		i32 relz = baserelz + offset.z;
		i32 wldy = basewldy + offset.y;

		if (wldy >= 0 && wldy < 256)
		{
			Vector2i neighborId((int)(relx >> 4) - 1, (int)(relz >> 4) - 1);
			ChunkClient* nearchunk = m_chunk->getNearChunk(neighborId.x, neighborId.y);
			if (nearchunk != nullptr)
			{
				Vector3i curbias((relx & 0x0f), (wldy & 0x0f), (relz & 0x0f));
				SectionClient* pSection = (SectionClient*)(nearchunk->m_pSections[int(wldy >> 4)]);
				if(pSection)
					pSection->markBlockLightDirty(curbias.x, curbias.y, curbias.z);
			}
			else
			{
				Vector3i nid = Vector3i(m_chunk->m_posX, m_yBase, m_chunk->m_posZ) + Vector3i(neighborId.x, 0, neighborId.y);
				ui32 id = sectionIDCompress(nid.x, nid.y, nid.z);
				auto& dirtys = m_world->m_preLightDirtMap[id];
				Vector3i curbias((relx & 0x0f), (wldy & 0x0f), (relz & 0x0f));
				ui16 biasCompress = blockBiasCompress(curbias, false);
				dirtys.push_back(biasCompress);
			}
		}
	}
}

}
