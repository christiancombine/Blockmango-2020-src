#include "TessMeshCollection.h"

#include "cWorld/WorldClient.h"
#include "cChunk/ChunkClient.h"

#include "Block/BM_Material.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Chunk/ChunkService.h"

namespace BLOCKMAN
{

TessMeshCollection::TessMeshCollection(WorldClient* pWorld)
{
	worldObj = pWorld;
}

bool TessMeshCollection::reSetSection(const BlockPos& sectionBegin)
{
	chunkX = sectionBegin.x >> 4;
	chunkY = sectionBegin.y >> 4;
	chunkZ = sectionBegin.z >> 4;
	startPos = sectionBegin;
	isEmpty = true;
	m_isInAsync = false;

	ChunkService* chunkService = worldObj->getChunkService();
	if (chunkService->chunkInCache(chunkX, chunkZ))
		chunkObj = chunkService->getChunk(chunkX, chunkZ);
	else
		return false;

	if (chunkObj->m_pSections[chunkY] == nullptr)
		return false;

	/*ChunkClient* pChunk = std::static_pointer_cast<ChunkClient>(chunkObj).get();
	if (pChunk->checkNeighborEmpty(chunkY))
		return false;*/

	isEmpty = chunkObj->m_pSections[chunkY]->isEmpty();
	return true;
}

bool TessMeshCollection::buildData()
{
	if (chunkObj == nullptr)
		return false;

	memset(blocks, 0, sizeof(ui32) * 20 * 20 * 20);
	SectionClient* pMainSection = (SectionClient*)(chunkObj->m_pSections[chunkY]);

	for (int iz = 0; iz < 20; ++iz)
	{
		for (int iy = 0; iy < 20; ++iy)
		{
			for (int ix = 0; ix < 20; ++ix)
			{
				int tilex = (ix + 14) >> 4;
				int tiley = (iy + 14) >> 4;
				int tilez = (iz + 14) >> 4;

				SectionClient* pSection = pMainSection->m_nearSections[tilez * 9 + tiley * 3 + tilex];
				if (pSection)
				{
					int biasx = (ix + 14) - (tilex << 4); // (ix+14) % 16
					int biasy = (iy + 14) - (tiley << 4); // (iy+14) % 16
					int biasz = (iz + 14) - (tilez << 4); // (iz+14) % 16

					blocks[iy][ix][iz] = pSection->getBlockUI32Data(BlockPos(biasx, biasy, biasz));
				}
				else// trade null as all sky-light air blocks-filled section.
				{
					blocks[iy][ix][iz] = 0x00000000;
				}
			}
		}
	}

	return true;
}

int TessMeshCollection::getBlockId(const BlockPos& pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	LordAssert(offset.x >= 0 && offset.x < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);
	
	return blocks[offset.y][offset.x][offset.z] & 0xFFF;
}

int TessMeshCollection::getBlockIDAndMeta(const BlockPos& pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	LordAssert(offset.x >= 0 && offset.x < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);

	ui32 data = blocks[offset.y][offset.x][offset.z];
	ui32 id = data & 0xFFF;
	ui32 meta = (data >> 12) & 0xF;
	return (id << 16) | meta;;
}

float TessMeshCollection::getBrightness(const BlockPos& pos, int minBrightness)
{
	int light = getLightValue(pos);

	if (light < minBrightness)
	{
		light = minBrightness;
	}

	return worldObj->lightBrightnessTable[light];
}

int TessMeshCollection::getLightBrightnessForSkyBlocks(const BlockPos& pos, int minr, int ming, int minb)
{
	int sky = getBrightnessMixed(LC_SKY, pos);
	int br = getBrightnessMixed(LC_BLOCK_R, pos);
	int bg = getBrightnessMixed(LC_BLOCK_G, pos);
	int bb = getBrightnessMixed(LC_BLOCK_B, pos);

	if (br < minr) br = minr;
	if (bg < ming) bg = ming;
	if (bb < minb) bb = minb;

	return (sky << 12) | (bb << 8) | (bg << 4) | br;
}

float TessMeshCollection::getLightBrightness(const BlockPos& pos)
{
	return worldObj->lightBrightnessTable[getLightValue(pos)];
}

bool TessMeshCollection::checkBlockPos(const BlockPos& pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	if (offset.x >= 0 && offset.z >= 0 && offset.y >= 0 &&
		offset.x < 20 && offset.z < 20 && offset.y < 20)
		return true;
	return false;
}

int TessMeshCollection::getLightValueExt(const BlockPos& pos, bool flag)
{
	BlockPos offset = pos - startPos + BlockPos(2, 2, 2);
	if (offset.x >= 0 && offset.z >= 0 && offset.y >= 0 &&
		offset.x < 20 && offset.z < 20 && offset.y < 20)
	{
		if (flag)
		{
			int blockID = getBlockId(pos);

			if (blockID == BLOCK_ID_STONE_SINGLE_SLAB ||
				blockID == BLOCK_ID_WOOD_SINGLE_SLAB ||
				blockID == BLOCK_ID_REDSAND_STONE_SINGLESLAB ||
				blockID == BLOCK_ID_TILLED_FIELD ||
				blockID == BLOCK_ID_STAIRS_WOOD_OAK ||
				blockID == BLOCK_ID_STAIRS_COBBLE_STONE)
			{
				int light_up = getLightValueExt(pos.getPosY(), false);
				int light_east = getLightValueExt(pos.getPosX(), false);
				int light_west = getLightValueExt(pos.getNegX(), false);
				int light_south = getLightValueExt(pos.getPosZ(), false);
				int light_north = getLightValueExt(pos.getNegZ(), false);

				if (light_east > light_up)
					light_up = light_east;
				if (light_west > light_up)
					light_up = light_west;
				if (light_south > light_up)
					light_up = light_south;
				if (light_north > light_up)
					light_up = light_north;

				return light_up;
			}
		}
		return getSpecialBlockBrightness(LC_SKY, pos);
	}
	else
		return 15;
}

int TessMeshCollection::getBlockMeta(const BlockPos& pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	LordAssert(offset.x >= 0 && offset.x < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);

	return (blocks[offset.y][offset.x][offset.z] >> 12) & 0xF;
}

const BM_Material& TessMeshCollection::getBlockMaterial(const BlockPos& pos)
{
	int blockID = getBlockId(pos);
	return blockID == 0 ? BM_Material::BM_MAT_air : BlockManager::sBlocks[blockID]->getMaterial();
}

bool TessMeshCollection::isBlockOpaqueCube(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL ? false : pBlock->isOpaqueCube();
}

bool TessMeshCollection::isBlockNormalCube(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL ? false : pBlock->getMaterial().blocksMovement() && pBlock->renderAsNormalBlock();
}

bool TessMeshCollection::doesBlockHaveSolidTopSurface(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return worldObj->isBlockTopFacingSurfaceSolid(pBlock, getBlockMeta(pos));
}

bool TessMeshCollection::isAirBlock(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL;
}

int TessMeshCollection::getBrightnessMixed(LIGHTCHANNEL channel, BlockPos pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	if (offset.x >= 0 && offset.z >= 0 && offset.y >= 0 &&
		offset.x < 20 && offset.z < 20 && offset.y < 20)
	{

		if (BlockManager::sUseNeighborBrightness[getBlockId(pos)])
		{
			int light_up = getSpecialBlockBrightness(channel, pos.getPosY());
			int light_east = getSpecialBlockBrightness(channel, pos.getPosX());
			int light_west = getSpecialBlockBrightness(channel, pos.getNegX());
			int light_south = getSpecialBlockBrightness(channel, pos.getPosZ());
			int light_north = getSpecialBlockBrightness(channel, pos.getNegZ());

			if (light_east > light_up)
				light_up = light_east;
			if (light_west > light_up)
				light_up = light_west;
			if (light_south > light_up)
				light_up = light_south;
			if (light_north > light_up)
				light_up = light_north;

			return light_up;
		}
		else
		{
			return getSpecialBlockBrightness(channel, pos);
		}
	}
	return channel == LC_SKY ? 0xF : 0;
}

int TessMeshCollection::getSpecialBlockBrightness(LIGHTCHANNEL channel, BlockPos pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	if (offset.x >= 0 && offset.z >= 0 && offset.y >= 0 &&
		offset.x < 20 && offset.z < 20 && offset.y < 20)
	{
		ui32 data = blocks[offset.y][offset.x][offset.z];
		if (channel == LC_BLOCK_R)
			return (data >> 16) & 0xF;
		else if (channel == LC_BLOCK_G)
			return (data >> 20) & 0xF;
		else if (channel == LC_BLOCK_B)
			return (data >> 24) & 0xF;
		else if (channel == LC_SKY)
			return (data >> 28) & 0xF;
		LordAssert(false);
	}
	return channel == LC_SKY ? 0xF : 0;
}

int TessMeshCollection::isBlockProvidingPowerTo(const BlockPos& pos, int side)
{
	int blockID = getBlockId(pos);
	return blockID == 0 ? 0 : BlockManager::sBlocks[blockID]->isProvidingStrongPower(this, pos, side);
}

bool TessMeshCollection::isRedBlock(const BlockPos& pos)
{
	if (worldObj)
	{
		return worldObj->getBlockRedStatusInThisWorld(pos);
	}
	return false;
}

TileEntity* TessMeshCollection::getBlockTileEntity(const BlockPos& pos)
{
	return chunkObj->getChunkBlockTileEntity(pos.getChunkPos());
}


}
