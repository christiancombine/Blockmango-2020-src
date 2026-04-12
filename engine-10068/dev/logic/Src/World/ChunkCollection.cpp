#include "ChunkCollection.h"

#include "World.h"
#include "Chunk/Chunk.h"

#include "Block/BM_Material.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Chunk/ChunkService.h"

namespace BLOCKMAN
{

ChunkCollection::ChunkCollection(World* pWorld, const BlockPos& min, const BlockPos& max, int extend)
{
	worldObj = pWorld;
	chunkX = (min.x - extend) >> 4;
	chunkZ = (min.z - extend) >> 4;
	int chunkEndX = (max.x + extend) >> 4;
	int chunkEndZ = (max.z + extend) >> 4;

	arrayColumn = chunkEndZ - chunkZ + 1;
	arrayRow = chunkEndX - chunkX + 1;
	chunkArray.resize(arrayRow * arrayColumn);

	isEmpty = true;
	m_isInAsync = false;
	ChunkPtr pChunk;

	for (int x = chunkX; x <= chunkEndX; ++x)
	{
		for (int z = chunkZ; z <= chunkEndZ; ++z)
		{
			auto chunkService = pWorld->getChunkService();
			if (chunkService->chunkInCache(x, z))
			{
				chunkArray[(x - chunkX) * arrayColumn + (z - chunkZ)] = chunkService->getChunk(x, z);
			}
		}
	}

	for (int x = min.x >> 4; x <= max.x >> 4; ++x)
	{
		for (int z = min.z >> 4; z <= max.z >> 4; ++z)
		{
			pChunk = chunkArray[(x - chunkX) * arrayColumn + (z - chunkZ)];

			if (pChunk == NULL)
				m_isInAsync = true;

			if (pChunk != NULL && !pChunk->getAreLevelsEmpty(min.y, max.y))
			{
				isEmpty = false;
			}
		}
	}
}

int ChunkCollection::getBlockId(const BlockPos& pos)
{
	if (pos.y < 0 || pos.y >= 256)
		return 0;
	
	int chunkidx_x = (pos.x >> 4) - chunkX;
	int chunkidx_z = (pos.z >> 4) - chunkZ;

	if (chunkidx_x >= 0 && chunkidx_x < arrayRow &&
		chunkidx_z >= 0 && chunkidx_z < arrayColumn)
	{
		ChunkPtr pChunk = chunkArray[chunkidx_x * arrayColumn + chunkidx_z];
		return pChunk == NULL ? 0 : pChunk->getBlockID(pos.getChunkPos());
	}

	return 0;
}

int ChunkCollection::getBlockIDAndMeta(const BlockPos& pos)
{
	if (pos.y < 0 || pos.y >= 256)
		return 0;

	int chunkidx_x = (pos.x >> 4) - chunkX;
	int chunkidx_z = (pos.z >> 4) - chunkZ;

	if (chunkidx_x >= 0 && chunkidx_x < arrayRow &&
		chunkidx_z >= 0 && chunkidx_z < arrayColumn)
	{
		ChunkPtr pChunk = chunkArray[chunkidx_x * arrayColumn + chunkidx_z];
		return pChunk == NULL ? 0 : pChunk->getBlockIDAndMeta(pos.getChunkPos());
	}

	return 0;
}

float ChunkCollection::getBrightness(const BlockPos& pos, int minBrightness)
{
	int light = getLightValue(pos);

	if (light < minBrightness)
	{
		light = minBrightness;
	}

	return worldObj->lightBrightnessTable[light];
}

int ChunkCollection::getLightBrightnessForSkyBlocks(const BlockPos& pos, int minR, int minG, int minB)
{
	int bs = getSkyBlockTypeBrightness(LC_SKY, pos);
	int br = getSkyBlockTypeBrightness(LC_BLOCK_R, pos);
	int bg = getSkyBlockTypeBrightness(LC_BLOCK_G, pos);
	int bb = getSkyBlockTypeBrightness(LC_BLOCK_B, pos);

	if (br < minR) br = minR;
	if (bg < minG) bg = minG;
	if (bb < minB) bb = minB;

	return (bs << 12) | (bb << 8) | (bg << 4) | br;
}

float ChunkCollection::getLightBrightness(const BlockPos& pos)
{
	return worldObj->lightBrightnessTable[getLightValue(pos)];
}

bool ChunkCollection::checkBlockPos(const BlockPos& pos)
{
	if (pos.x >= -30000000 &&
		pos.z >= -30000000 &&
		pos.x < 30000000 &&
		pos.z <= 30000000)
		return true;
	return false;
}

int ChunkCollection::getLightValueExt(const BlockPos& pos, bool flag)
{
	if (checkBlockPos(pos))
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
				BlockPos bp_up = pos;
				bp_up.y++;
				int light_up = getLightValueExt(bp_up, false);
				BlockPos bp_east = pos; 
				bp_east.x++;
				int light_east = getLightValueExt(bp_east, false);
				BlockPos bp_west = pos;
				bp_west.x--;
				int light_west = getLightValueExt(bp_west, false);
				BlockPos bp_south = pos; 
				bp_south.z++;
				int light_south = getLightValueExt(bp_south, false);
				BlockPos bp_north = pos; 
				bp_north.z--;
				int light_north = getLightValueExt(bp_north, false);

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

		if (pos.y < 0)
		{
			return 0;
		}
		else if (pos.y >= 256)
		{
			int light = 15 - worldObj->m_skylightSubtracted;

			if (light < 0)
			{
				light = 0;
			}

			return light;
		}
		else
		{
			int cx = (pos.x >> 4) - chunkX;
			int cz = (pos.z >> 4) - chunkZ;
			return chunkArray[cx * arrayColumn + cz]->getBlockLightValue(pos.getChunkPos(), worldObj->m_skylightSubtracted);
		}
	}
	else
	{
		return 15;
	}
}

int ChunkCollection::getBlockMeta(const BlockPos& pos)
{
	if (pos.y < 0)
	{
		return 0;
	}
	else if (pos.y >= 256)
	{
		return 0;
	}
	else
	{
		int cx = (pos.x >> 4) - chunkX;
		int cz = (pos.z >> 4) - chunkZ;
		return chunkArray[cx * arrayColumn + cz]->getBlockMeta(pos.getChunkPos());
	}
}

const BM_Material& ChunkCollection::getBlockMaterial(const BlockPos& pos)
{
	int blockID = getBlockId(pos);
	return blockID == 0 ? BM_Material::BM_MAT_air : BlockManager::sBlocks[blockID]->getMaterial();
}

bool ChunkCollection::isBlockOpaqueCube(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL ? false : pBlock->isOpaqueCube();
}

bool ChunkCollection::isBlockNormalCube(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL ? false : pBlock->getMaterial().blocksMovement() && pBlock->renderAsNormalBlock();
}

bool ChunkCollection::doesBlockHaveSolidTopSurface(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return worldObj->isBlockTopFacingSurfaceSolid(pBlock, getBlockMeta(pos));
}

bool ChunkCollection::isAirBlock(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL;
}

int ChunkCollection::getSkyBlockTypeBrightness(LIGHTCHANNEL channel, BlockPos pos)
{
	if (pos.y < 0)
	{
		pos.y = 0;
	}

	if (pos.y >= 256)
	{
		pos.y = 255;
	}

	if (checkBlockPos(pos))
	{
		if (channel == LC_SKY)
		{
			return 0;
		}
		else
		{
			if (BlockManager::sUseNeighborBrightness[getBlockId(pos)])
			{
				BlockPos bp_neighbor = pos; bp_neighbor.y++;
				int light_up = getSpecialBlockBrightness(channel, bp_neighbor);
				bp_neighbor = pos; bp_neighbor.x++;
				int light_east = getSpecialBlockBrightness(channel, bp_neighbor);
				bp_neighbor = pos; bp_neighbor.x--;
				int light_west = getSpecialBlockBrightness(channel, bp_neighbor);
				bp_neighbor = pos; bp_neighbor.z++;
				int light_south = getSpecialBlockBrightness(channel, bp_neighbor);
				bp_neighbor = pos; bp_neighbor.z--;
				int light_north = getSpecialBlockBrightness(channel, bp_neighbor);

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
				int cx = (pos.x >> 4) - chunkX;
				int cz = (pos.z >> 4) - chunkZ;
				return chunkArray[cx * arrayColumn + cz]->getSavedLightValue(channel, pos.getChunkPos());
			}
		}
	}
	else
	{
		return channel == LC_SKY ? 0xF : 0;
	}
}

int ChunkCollection::getSpecialBlockBrightness(LIGHTCHANNEL channel, BlockPos pos)
{
	if (pos.y < 0)
		pos.y = 0;
	if (pos.y >= 256)
		pos.y = 255;

	if (checkBlockPos(pos))
	{
		int cx = (pos.x >> 4) - chunkX;
		int cz = (pos.z >> 4) - chunkZ;
		return chunkArray[cx * arrayColumn + cz]->getSavedLightValue(channel, pos.getChunkPos());
	}
	else
	{
		return channel == LC_SKY ? 0xF : 0;
	}
}

int ChunkCollection::isBlockProvidingPowerTo(const BlockPos& pos, int side)
{
	int blockID = getBlockId(pos);
	return blockID == 0 ? 0 : BlockManager::sBlocks[blockID]->isProvidingStrongPower(this, pos, side);
}

TileEntity* ChunkCollection::getBlockTileEntity(const BlockPos& pos)
{
	int cx = (pos.x >> 4) - chunkX;
	int cz = (pos.z >> 4) - chunkZ;
	return chunkArray[cx*arrayColumn+cz]->getChunkBlockTileEntity(pos.getChunkPos());
}


}