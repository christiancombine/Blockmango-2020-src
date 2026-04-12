#include "WorldClient.h"
#include "cChunk/ChunkClient.h"
#include "cChunk/ChunkServiceClient.h"
#include "cChunk/ChunkReadableStorageFileClient.h"
#include "Blockman.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Object/Root.h"

/** header files from Logic. */
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Chunk/Chunk.h"

#include "Render/RenderGlobal.h"
#include "Audio/SoundSystem.h"
#include "game.h"
#include "WorldGenerator/SchematicModel.h"
#include "cWorld/BlockChangeRecorderClient.h"

#include "Util/SkillEffectManager.h"
#include "Entity/EntityPlayer.h"
#include "Util/PlayerPrefs.h"
#include "Setting/LogicSetting.h"
#include "Network/protocol/S2CPackets.h"
#include "cEntity/EntityVehicleClient.h"
#include "WorldGenerator/AnvilManager.h"
#include "Util/CPUTimer.h"

namespace BLOCKMAN
{

void WorldClient::init2()
{
	m_cloudColour = 0xFFFFFF;
	m_prevRainingStrength = 0;
	m_rainingStrength = 0;
	m_prevThunderingStrength = 0;
	m_thunderingStrength = 0;
	m_lightUpdateBlockList = NULL;
	memset(m_special_sound_tick, 0, sizeof(m_special_sound_tick));
}

WorldClient::WorldClient(const String& name, const WorldSettings& settings, int loadRange)
: World(name, settings, loadRange)
{
	m_isClient = true;

	init2();
	createShop();
	m_lightUpdateBlockList = (int*)LordMalloc(sizeof(int)* 32768);
	memset(m_lightUpdateBlockList, 0, sizeof(int)* 32768);
	
	// par3WorldProvider.registerWorld(this);
	calculateInitialWeather();
}

WorldClient::~WorldClient()
{
	m_isDestroying = true;
	LordSafeFree(m_lightUpdateBlockList);
}

ChunkService* WorldClient::createChunkService(int loadRange)
{
	m_pChunkService = LordNew ChunkServiceClient(this, loadRange);
	auto mapDir = PathUtil::ConcatPath(Root::Instance()->getMapPath(), m_WorldInfo.getMapId() + "/");
	PathUtil::FormatPath(mapDir);
	m_pChunkService->useReadableStorage<ChunkReadableStorageFileClient>(this, mapDir);
	return m_pChunkService;
}

int WorldClient::blockGetRenderType(const BlockPos& pos)
{
	int blockID = getBlockId(pos);

	cBlock* pcBlock = cBlockManager::scBlocks[blockID];

	return pcBlock != NULL ? pcBlock->getRenderType() : -1;
}

int WorldClient::getFullBlockLightValue(const BlockPos& pos)
{
	int y = pos.y;
	if (y < 0)
		return 0;

	if (y >= 256)
		y = 255;

	ChunkPtr pChunck = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
	LordAssert(pChunck);

	return pChunck->getBlockLightValue(pos.getChunkPos(), 0);
}

int WorldClient::getBlockLightValue_impl(const BlockPos& pos, bool flag)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;
	if (!(x >= -MAX_WORLD_X && z >= -MAX_WORLD_Z && x < MAX_WORLD_X && z < MAX_WORLD_Z))
		return 15;

	if (flag)
	{
		int blockID = getBlockId(pos);

		if (BlockManager::sUseNeighborBrightness[blockID])
		{
			int lihgtValue_up = getBlockLightValue_impl(BlockPos(x, y + 1, z), false);
			int lightValue_right = getBlockLightValue_impl(BlockPos(x + 1, y, z), false);
			int lightValue_left = getBlockLightValue_impl(BlockPos(x - 1, y, z), false);
			int lightValue_forward = getBlockLightValue_impl(BlockPos(x, y, z + 1), false);
			int lightValue_backword = getBlockLightValue_impl(BlockPos(x, y, z - 1), false);

			if (lightValue_right > lihgtValue_up)
				lihgtValue_up = lightValue_right;
			if (lightValue_left > lihgtValue_up)
				lihgtValue_up = lightValue_left;
			if (lightValue_forward > lihgtValue_up)
				lihgtValue_up = lightValue_forward;
			if (lightValue_backword > lihgtValue_up)
				lihgtValue_up = lightValue_backword;

			return lihgtValue_up;
		}
	}

	if (y < 0)
		return 0;

	if (y >= 256)
		y = 255;

	ChunkPtr pChunk = getChunkFromChunkCoords(x >> 4, z >> 4);
	LordAssert(pChunk);

	BlockPos chunkPos = pos.getChunkPos();
	chunkPos.y = y;
	return pChunk->getBlockLightValue(chunkPos, m_skylightSubtracted);
}

int WorldClient::getSkyBlockTypeBrightness(LIGHTCHANNEL channel, const BlockPos& pos)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;
	if (y < 0)
	{
		y = 0;
	}

	if (y >= 256)
	{
		return channel == LC_SKY ? 0xF : 0;
	}
	else if (x >= -MAX_WORLD_X && z >= -MAX_WORLD_Z && x < MAX_WORLD_X && z < MAX_WORLD_Z)
	{
		int xinChunk = x >> 4;
		int zinChunk = z >> 4;

		if (!chunkExists(xinChunk, zinChunk))
		{
			return channel == LC_SKY ? 0xF : 0;
		}
		else if (BlockManager::sUseNeighborBrightness[getBlockId(BlockPos(x, y, z))])
		{
			int lightValue_up = getSavedLightValue(channel, BlockPos(x, y + 1, z));
			int lightValue_right = getSavedLightValue(channel, BlockPos(x + 1, y, z));
			int lightValue_left = getSavedLightValue(channel, BlockPos(x - 1, y, z));
			int lightValue_forward = getSavedLightValue(channel, BlockPos(x, y, z + 1));
			int lightValue_backward = getSavedLightValue(channel, BlockPos(x, y, z - 1));

			if (lightValue_right > lightValue_up)
				lightValue_up = lightValue_right;
			if (lightValue_left > lightValue_up)
				lightValue_up = lightValue_left;
			if (lightValue_forward > lightValue_up)
				lightValue_up = lightValue_forward;
			if (lightValue_backward > lightValue_up)
				lightValue_up = lightValue_backward;

			return lightValue_up;
		}
		else
		{
			ChunkPtr pChunk = getChunkFromChunkCoords(xinChunk, zinChunk);
			LordAssert(pChunk);
			return pChunk->getSavedLightValue(channel, BlockPos(x & 15, y, z & 15));
		}
	}
	else
	{
		return channel == LC_SKY ? 0xF : 0;
	}
}

int WorldClient::getSavedLightValue(LIGHTCHANNEL channel, const BlockPos& pos)
{
	int y = pos.y;
	if (y < 0)
		y = 0;
	if (y >= 256)
		y = 255;

	if (pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z)
	{
		int xinChunk = pos.x >> 4;
		int zinChunk = pos.z >> 4;

		if (!chunkExists(xinChunk, zinChunk))
		{
			return channel == LC_SKY ? 0xF : 0;
		}
		else
		{
			ChunkPtr pChunk = getChunkFromChunkCoords(xinChunk, zinChunk);
			if (pChunk)
				return pChunk->getSavedLightValue(channel, BlockPos(pos.x & 15, y, pos.z & 15));
			else
				return LC_SKY ? 0xF : 0;
		}
	}
	else
		return LC_SKY ? 0xF : 0;
}

int WorldClient::getLightBrightnessForSkyBlocks(const BlockPos& pos, int minR, int minG, int minB)
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

float WorldClient::getBrightness(const BlockPos& pos, int minBrightness)
{
	int brightness = getBlockLightValue(pos);

	if (brightness < minBrightness)
	{
		brightness = minBrightness;
	}

	return lightBrightnessTable[brightness];
}

float WorldClient::getLightBrightness(const BlockPos& pos)
{
	return lightBrightnessTable[getBlockLightValue(pos)];
}

bool WorldClient::setBlocks(int x, int z, const BlockModifys& modifys)
{
	vector<ui32>::type prevIds;
	vector<ui32>::type prevMetas;
	prevIds.resize(modifys.size());
	prevMetas.resize(modifys.size());
	for (size_t i = 0; i < modifys.size(); ++i)
	{
		prevIds[i] = getBlockId(modifys[i].pos);
		prevMetas[i] = getBlockMeta(modifys[i].pos);
	}
	bool ret = World::setBlocks(x, z, modifys);

	//     for (auto block : modifys)
	//     {
	//         BlockChangeRecorderClient::Instance()->record(block.pos, block.id, (uint8_t)block.meta);
	//     }
	BlockChangeRecorderClient::Instance()->recordBlocks(x, z, modifys);

	if (ret)
	{
		for (size_t i = 0; i < modifys.size(); ++i)
		{
			const BlockPos& m = modifys[i].pos;
			Vector3i sid(m.x >> 4, m.y >> 4, m.z >> 4);
			int sectionid = ((sid.x + 1024) << 18) | ((sid.z + 1024) << 4) | sid.y;
			m_dirtySectionVisibility.enqueue(sectionid);

			if (prevIds[i] != modifys[i].id || prevMetas[i] != modifys[i].meta)
			{
				ChunkPtr pChunk = getChunkFromChunkCoords(m.x >> 4, m.z >> 4);
				SectionClient* psection = (SectionClient*)(pChunk->m_pSections[m.y >> 4]);
				psection->onModifyLight(m, prevIds[i], modifys[i].id);
				markSectionForUpdate(BlockPos(x, m.y >> 4, z));
			}
		}
	}

	return ret;
}

bool WorldClient::setBlock(const BlockPos& pos, int blockID, int metadata, int flag, bool immediate)
{
	//LordLogInfo("setBlick[%d,%d,%d] id:%d meta:%d", pos.x, pos.y, pos.z, blockID, metadata);
	if (!(pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z))
		return false;

	if (pos.y < 0)
	{
		return false;
	}
	else if (pos.y >= 256)
	{
		return false;
	}

	ChunkPtr pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
	int tblockID = 0;

	if ((flag & 1) != 0)
	{
		tblockID = pChunk->getBlockID(pos.getChunkPos());
	}

	BlockPos chunkpos = pos.getChunkPos();
	ui32 prevID = pChunk->getBlockID(chunkpos);
	bool modifyed = pChunk->setBlockIDAndMeta(chunkpos, blockID, metadata);

	if (modifyed)
	{
		// need change height map?
		pChunk->updateHeightMap(chunkpos);

		SectionClient* psection = (SectionClient*)(pChunk->m_pSections[pos.y >> 4]);
		if (psection)
			psection->onModifyLight(pos, prevID, blockID);

		if ((flag & 2) != 0 && (!m_isClient || (flag & 4) == 0))
		{
			markBlockForUpdate(pos);
		}

		if (!m_isClient && (flag & 1) != 0)
		{
			notifyBlockChange(pos, tblockID);
			Block* pBlock = BlockManager::sBlocks[blockID];

			if (pBlock != NULL && pBlock->hasComparatorInputOverride())
			{
				updateComparatorOutputLevel(pos, blockID);
			}
		}

		Vector3i sid(pos.x >> 4, pos.y >> 4, pos.z >> 4);
		int sectionid = ((sid.x + 1024) << 18) | ((sid.z + 1024) << 4) | sid.y;
		m_dirtySectionVisibility.enqueue(sectionid);
	}

	return modifyed;
}

void WorldClient::applyRebuildVisibility()
{
	// rebuild visibility
	DirtySectionVisibility dirtySections;

	int tmpSectionId;
	size_t maxCntOnce = 100;
	size_t cnt = 0;
	while (m_dirtySectionVisibility.try_dequeue(tmpSectionId)) {
		++cnt;
		dirtySections.insert(tmpSectionId);
		if (cnt >= maxCntOnce) {
			break;
		}
	}

	for (auto& sid : dirtySections)
	{
		int secx = (sid >> 18) - 1024;
		int secz = ((sid >> 4) & 0x3FFF)  - 1024;
		int secy = sid & 0x0000000F;
		if (getChunkService()->chunkInCache(secx, secz))
			getChunkService()->getChunk(secx, secz)->reBuildVisibility(secy);
	}
}

void WorldClient::updateNearChunkRef(ChunkClient* pchunk)
{
	for (int zOffset = -1; zOffset <= 1; zOffset++)
	{
		for (int xOffset = -1; xOffset <= 1; xOffset++)
		{
			int nearIndex = (xOffset + 1) + (zOffset + 1) * 3;

			Vector2i nearchunkid(pchunk->m_posX + xOffset, pchunk->m_posZ + zOffset);

			ChunkClient* nearChunk = nullptr;
			if (m_pChunkService->chunkInCache(nearchunkid.x, nearchunkid.y))
				nearChunk = std::static_pointer_cast<ChunkClient>(m_pChunkService->getChunk(nearchunkid.x, nearchunkid.y)).get();

			pchunk->m_nearChunks[nearIndex] = nearChunk;

			if (nearChunk != nullptr)
			{
				nearChunk->m_nearChunks[(-xOffset + 1) + (-zOffset + 1) * 3] = pchunk;
			}
		}
	}

	// set the section's near
	for (int i = 0; i < 9; ++i)
	{
		if (pchunk->m_nearChunks[i] != nullptr)
			pchunk->m_nearChunks[i]->updateNearSectionRef();
	}
}

void WorldClient::invalidateNearChunkRef(const Vector2i& chunkid)
{
	if (m_isDestroying)
		return;

	for (int zOffset = -1; zOffset <= 1; zOffset++)
	{
		for (int xOffset = -1; xOffset <= 1; xOffset++)
		{
			int nearIndex = (xOffset + 1) + (zOffset + 1) * 3;

			Vector2i nearchunkid(chunkid.x + xOffset, chunkid.y + zOffset);

			ChunkClient* nearChunk = nullptr;
			if (m_pChunkService->chunkInCache(nearchunkid.x, nearchunkid.y))
				nearChunk = std::static_pointer_cast<ChunkClient>(m_pChunkService->getChunk(nearchunkid.x, nearchunkid.y)).get();

			if (nearChunk != nullptr)
			{
				nearChunk->invalidNearSectionRef(chunkid);
				nearChunk->m_nearChunks[(-xOffset + 1) + (-zOffset + 1) * 3] = nullptr;
			}
		}
	}

}

void WorldClient::asyncLoadChunks(const Vector3& playerPos, int chunkRange)
{
	if (!SceneManager::Instance())
		return;
	Camera* pcam = SceneManager::Instance()->getMainCamera();
	if (!pcam)
		return;

	Vector3 caemraDir = pcam->getDirection();

	Vector2i center((int)Math::Floor(playerPos.x / 16.f), (int)Math::Floor(playerPos.z / 16.f));

	Vector2 cameraDir2D(caemraDir.x, caemraDir.z);
	cameraDir2D.normalize();

	int radius = (chunkRange / 2) + 1;
	int length = radius;
	int radius2 = radius * radius;

	for (int i = -length; i <= length; i++)
	{
		for (int j = -length; j <= length; j++)
		{
			Vector2i chunkPos = center + Vector2i(i, j);
			ui32 sectionid = ((chunkPos.x + 1024) << 16) | (chunkPos.y + 1024);
			
			Vector2 dir2Point((float)i, (float)j); 
			dir2Point.normalize();

			// InlcudeAngle of view cone
			float includeAngleCos = dir2Point.x * cameraDir2D.x + dir2Point.y * cameraDir2D.y;

			// Visibility Conditions 
			float distance2 = static_cast<float>(i * i + j * j);

			if (distance2 <= radius2 && (distance2 <= ((float)radius2 * 0.5f) || includeAngleCos > 0.0f))
			{
				if (m_pChunkService->chunkInCache(chunkPos.x, chunkPos.y))
					continue;
				if (m_pChunkService->ayncGetExists(chunkPos.x, chunkPos.y))
					continue;
				m_pChunkService->getChunkAsync(chunkPos.x, chunkPos.y);
			}
		}
	}
}

void WorldClient::playSoundByType(const Vector3& pos, SoundType soundType)
{
	if (!LogicSetting::Instance()->canPlayGameSound())
	{
		return;
	}

	NULL_RETURN_VOID(Blockman::Instance()->renderViewEntity);

	float dis = 16.0F;
	float volume = 1.0F; // ����Ĭ����1
	if (volume > 1.0F)
		dis *= volume;

	float disSqr = Blockman::Instance()->renderViewEntity->getDistanceSq(pos);
	if (disSqr > (dis * dis))
		return;
	SoundSystem::Instance()->playEffectByType(soundType, pos);
}

void WorldClient::playSoundOfPlaceBlock(const BlockPos & pos, int blockId)
{
	auto block = cBlockManager::scBlocks[blockId];
	//playSound(Vector3(pos) + 0.5f, block->getStepSound().getPlaceSound(), (block->getStepSound().volume + 1.0F) / 2.0F, block->getStepSound().pitch * 0.8F, false);
	playSoundByType(Vector3(pos) + 0.5f, block->getStepSound().placeSound);
}

void WorldClient::playSoundOfStepOnBlock(const BlockPos & pos, int blockId)
{
	auto block = cBlockManager::scBlocks[blockId];
	//playSound(Vector3(pos) + 0.5f, block->getStepSound().getStepSound(), (block->getStepSound().volume + 1.0F) / 2.0F, block->getStepSound().pitch * 0.8F, false);
	playSoundByType(Vector3(pos) + 0.5f, block->getStepSound().stepSound);
}

void WorldClient::doVoidFogParticles(BlockPos pos)
{
	PROFILE_CPU("WorldClient::doVoidFogParticles");
	int bound = 16;

	BlockPos checkpos;
	for (int i = 0; i < 1000; ++i)
	{
		checkpos = pos;
		checkpos.x += m_Rand.nextInt(bound) - m_Rand.nextInt(bound);
		checkpos.y += m_Rand.nextInt(bound) - m_Rand.nextInt(bound);
		checkpos.z += m_Rand.nextInt(bound) - m_Rand.nextInt(bound);
		if (!checkChunksExist(checkpos, checkpos))
		{
			continue;
		}
		int blockId = getBlockId(checkpos);

		if (blockId == 0 && m_Rand.nextInt(8) > checkpos.y /*&& provider->getWorldHasVoidParticles()*/)
		{
			Vector3 particlePos = checkpos;
			particlePos.x += m_Rand.nextFloat();
			particlePos.y += m_Rand.nextFloat();
			particlePos.z += m_Rand.nextFloat();
			spawnParticle("depthsuspend", particlePos, BlockPos::ZERO);
		}
		else if (blockId > 0)
		{
			cBlockManager::scBlocks[blockId]->randomDisplayTick(this, checkpos, m_Rand);
		}
	}
}

void WorldClient::createSchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
	bool reversalX, bool reversalZ, bool coverOldBlock, int placeBlockId, int placeOffset, int extraParam)
{
	processSchematic(schematicName, startPos, placeRakssid, true, reversalX, reversalZ,
		coverOldBlock, placeBlockId, placeOffset, extraParam);
}

void WorldClient::destroySchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
	bool reversalX, bool reversalZ, int placeOffset, int extraParam)
{
	processSchematic(schematicName, startPos, placeRakssid, false, reversalX, reversalZ,
		true, 0, placeOffset, extraParam);
}

void WorldClient::processSchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
	bool createOrDestroy, bool reversalX, bool reversalZ, bool coverOldBlock, int placeBlockId,
	int placeOffset, int extraParam)
{
	// parser .Schematic file
	auto schematicModel = LordNewT(SchematicModel);
	schematicModel->resetData();

	String path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "dynamic", schematicName);
	if (schematicModel->deserialize(path.c_str()))
	{
		int element_count = 0;
		i16 width = schematicModel->getWidth();
		i16 height = schematicModel->getHeight();
		i16 length = schematicModel->getLength();

		std::map< ChunkInfo, BlockModifys > chunk_blocks;
		chunk_blocks.clear();

		auto&& placeCoverResult = PlayerPrefs::Instance()->GetProperty<Vector3iBoolMap>("SchematicPlaceCoverResult");

		// from the bottom to top
		for (i16 y = 0; y < height; ++y)
		{
			for (i16 x = 0; x < width; ++x)
			{
				for (i16 z = 0; z < length; ++z)
				{
					int index = (y * length + z) * width + x;

					ui8 block_id = (placeBlockId != 0 ? (schematicModel->getBlockId(index) != 0 ?
						placeBlockId : 0) : schematicModel->getBlockId(index));

					if (block_id <= 0)
						continue;

					ui8 mate = schematicModel->getMate(index);

					if (BlockManager::isStairsBlock((int)block_id))
					{
						mate = BlockManager::StairsBlockRotate(mate, reversalX, reversalZ);
					}
					else if ((int)block_id == BLOCK_ID_LADDER)
					{
						mate = BlockManager::LadderBlockRotate(mate, reversalX, reversalZ);
					}
					else if (BlockManager::isTorchWoodBlock((int)block_id))
					{
						mate = BlockManager::TorchWoodBlockRotate(mate, reversalX, reversalZ);
					}
					else if (BlockManager::isWoodBlock((int)block_id))
					{
						mate = BlockManager::WoodBlockRotate(mate, reversalX, reversalZ);
					}
					else if ((int)block_id == BLOCK_ID_VINE)
					{
						mate = BlockManager::VineBlockRotate(mate, reversalX, reversalZ);
					}
					else if (BlockManager::isDoorBlock((int)block_id))
					{
						mate = BlockManager::DoorBlockRotate(mate, reversalX, reversalZ);
					}
					else if (BlockManager::isFenceGateBlock((int)block_id))
					{
						mate = BlockManager::FenceGateBlockRotate(mate, reversalX, reversalZ);
					}
					else if ((int)block_id == BLOCK_ID_TRAP_DOOR)
					{
						mate = BlockManager::TrapDoorBlockRotate(mate, reversalX, reversalZ);
					}

					element_count++;

					int pos_x = (int)x;
					int pos_y = (int)y;
					int pos_z = (int)z;

					int pos_x_old = pos_x;
					int pos_y_old = pos_y;
					int pos_z_old = pos_z;

					// 绕点旋转 270 180 90算法
					if (reversalX && !reversalZ)
					{
						pos_x = length - pos_z_old - placeOffset;
						pos_z = pos_x_old;
					}
					else if (reversalX && reversalZ)
					{
						pos_x = -pos_x_old;
						pos_z = length - pos_z_old - placeOffset;

						pos_x += width - placeOffset;
					}
					else if (!reversalX && reversalZ)
					{
						pos_x = -(length - pos_z_old) + placeOffset;
						pos_z = -pos_x_old;

						pos_z += width - placeOffset;
						pos_x += length - placeOffset;
					}
					else
					{

					}

					pos_x += startPos.x;
					pos_y += startPos.y;
					pos_z += startPos.z;

					Vector3i position(pos_x, pos_y, pos_z);

					if (!createOrDestroy)
					{
						block_id = 0;
						mate = 0;
					}

					bool bRet{ true };
					findPlaceCoverResult(placeCoverResult, pos_x, pos_y, pos_z, bRet);

					if (!coverOldBlock)
					{
						bool bHasBlock = getBlockId(BlockPos(pos_x, pos_y, pos_z)) > 0;
						if (bHasBlock && bRet)
						{
							continue;
						}
					}

					BlockModifyInfo chunk_block;
					chunk_block.pos = position;
					chunk_block.id = block_id;
					chunk_block.meta = mate;
					ChunkInfo chunk_info(pos_x >> 4, pos_z >> 4);

					auto iter = chunk_blocks.find(chunk_info);
					if (iter != chunk_blocks.end())
					{
						iter->second.push_back(chunk_block);
					}
					else
					{
						BlockModifys block_info;
						block_info.push_back(chunk_block);
						chunk_blocks[chunk_info] = block_info;
					}
				}
			}
		}

		for (auto chunk : chunk_blocks)
		{
			setBlocks(chunk.first.chunkX, chunk.first.chunkZ, chunk.second);
		}
		chunk_blocks.clear();
		//放置模块成功
		SchemeticPlaceBlockFinishEvent::emit();
	}
	else
	{
		LordLogError("WorldClient::processSchematic failed to deserialize schematic: %s", path.c_str());
	}
	LordSafeDelete(schematicModel);
}

void WorldClient::findPlaceCoverResult(Vector3iBoolMap& placeCoverResult, int pos_x, int pos_y, int pos_z, bool &bRet)
{
	for (auto it = placeCoverResult.begin();it != placeCoverResult.end();++it)
	{
		if (it->first.x == pos_x && it->first.y == pos_y && it->first.z == pos_z)
		{
			bRet = it->second;
			it = placeCoverResult.erase(it);
			return;
		}
	}
}

void WorldClient::fillAreaByBlockIdAndMate(const BlockPos & start_pos, const BlockPos & end_pos, int block_id, int mate)
{
	// if (block_id <= 0) return;

	int begin_x = start_pos.x < end_pos.x ? start_pos.x : end_pos.x;
	int begin_y = start_pos.y < end_pos.y ? start_pos.y : end_pos.y;
	int begin_z = start_pos.z < end_pos.z ? start_pos.z : end_pos.z;
	int end_x = start_pos.x < end_pos.x ? end_pos.x : start_pos.x;
	int end_y = start_pos.y < end_pos.y ? end_pos.y : start_pos.y;
	int end_z = start_pos.z < end_pos.z ? end_pos.z : start_pos.z;

	std::map< ChunkInfo, BlockModifys > chunk_blocks;
	chunk_blocks.clear();

	for (int y = begin_y; y <= end_y; ++y)
	{
		for (int x = begin_x; x <= end_x; ++x)
		{
			for (int z = begin_z; z <= end_z; ++z)
			{
				Vector3i position(x, y, z);

				BlockModifyInfo chunk_block;
				chunk_block.pos = position;
				chunk_block.id = (ui16)block_id;
				chunk_block.meta = (ui16)mate;
				ChunkInfo chunk_info(x >> 4, z >> 4);

				auto iter = chunk_blocks.find(chunk_info);
				if (iter != chunk_blocks.end())
				{
					iter->second.push_back(chunk_block);
				}
				else
				{
					BlockModifys block_info;
					block_info.push_back(chunk_block);
					chunk_blocks[chunk_info] = block_info;
				}
			}
		}
	}

	for (auto chunk : chunk_blocks)
	{
		setBlocks(chunk.first.chunkX, chunk.first.chunkZ, chunk.second);
	}

	chunk_blocks.clear();

	LordLogInfo("WorldClient::fillAreaByBlockIdAndMate start_pos x:%d y:%d z:%d end_pos x:%d y:%d z:%d block_id:%d mate:%d", start_pos.x, start_pos.y, start_pos.z, end_pos.x, end_pos.y, end_pos.z, block_id, mate);
}

void WorldClient::handleLargeBlocks(const BlockModifys& blocks_info)
{
	std::map< ChunkInfo, BlockModifys > chunk_blocks;
	chunk_blocks.clear();

	for (const auto& blockInfo : blocks_info)
	{
		Vector3i position(blockInfo.pos.x, blockInfo.pos.y, blockInfo.pos.z);

		BlockModifyInfo chunk_block;
		chunk_block.pos = position;
		chunk_block.id = blockInfo.id;
		chunk_block.meta = blockInfo.meta;
		ChunkInfo chunk_info(blockInfo.pos.x >> 4, blockInfo.pos.z >> 4);

		auto iter = chunk_blocks.find(chunk_info);
		if (iter != chunk_blocks.end())
		{
			iter->second.push_back(chunk_block);
		}
		else
		{
			BlockModifys block_info;
			block_info.push_back(chunk_block);
			chunk_blocks[chunk_info] = block_info;
		}
	}

	for (auto chunk : chunk_blocks)
	{
		setBlocks(chunk.first.chunkX, chunk.first.chunkZ, chunk.second);
	}

	chunk_blocks.clear();
}

bool WorldClient::checkRangeBlockIsAir(const Vector3 & beginPos, const Vector3 & endPos)
{
	EntityArr entities = getEntitiesWithinAABB(ENTITY_CLASS_ENTITY, Box(beginPos, endPos));
	if (!entities.empty())
	{
		return false;
	}

	for (int x = (int)beginPos.x	; x < endPos.x; x ++ )
	{
		for (int y = (int)beginPos.y; y < endPos.y; y++)
		{
			for (int z = (int)beginPos.z; z < endPos.z; z++)
			{
				i32 blockId = getBlockId(Vector3i(x, y, z));
				if (blockId != 0 )
				{
					return false;
				}
			}
		}
	}
	return true;
}

void WorldClient::setPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2, float speed)
{
	String image = "";
	if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunChicken)
		image = "poisionCircle_1.png";
	else
		image = "poisionCircle.png";

	Blockman::Instance()->m_globalRender->setPoisonCircleRangeAndSpeed(getPoisonCircleRange(safeRange1, safeRange2, poisonRange1, poisonRange2), speed, image);
}

float WorldClient::getFrameDeltaTime()
{
	//return GameClient::CGame::Instance()->getFrameTime() / 1000.0f;
	return Blockman::s_tickFPS;
}

bool WorldClient::getBlockRedStatusInThisWorld(const BlockPos & pos)
{
	ChunkPtr pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
	if (pChunk)
	{
		return pChunk->getBlockRedStatusInThisChunk(pos.getChunkPos());
	}
	return false;
}

void WorldClient::spawnParticle(const String& name, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter)
{
	for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
	{
		(*it)->spawnParticle(name, pos, vel, emmiter);
	}
}

void WorldClient::playSoundAtEntityByType(Entity* pEntity, SoundType soundType)
{
	if (pEntity != NULL && soundType > ST_Invalid)
	{
		for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
		{
			(*it)->playSound(pEntity->position + Vector3(0.f, -pEntity->yOffset, 0.f), soundType);
		}
	}
}

float WorldClient::getSunBrightness(float rdt)
{
	float celestail = getCelestialAngle(rdt);
	float brightness = 1.0f - (Math::Cos_tab(celestail * Math::PI_2) * 2.0f + 0.2f);

	if (brightness < 0.0f)
	{
		brightness = 0.0f;
	}

	if (brightness > 1.0f)
	{
		brightness = 1.0f;
	}

	brightness = 1.0f - brightness;
	brightness = brightness * (1.0f - (getRainStrength(rdt) * 5.0f) / 16.f);
	brightness = brightness * (1.0f - (getWeightedThunderStrength(rdt) * 5.0f) / 16.f);
	return brightness * 0.8F + 0.2F;
}

Color WorldClient::getSkyColor(Entity* pEntity, float rdt)
{
	float celestial = getCelestialAngle(rdt);
	float brightness = Math::Cos_tab(celestial * Math::PI_2) * 2.0F + 0.5F;

	if (brightness < 0.0F)
	{
		brightness = 0.0F;
	}

	if (brightness > 1.0F)
	{
		brightness = 1.0F;
	}

	int x = int(Math::Floor(pEntity->getPosition().x));
	int z = int(Math::Floor(pEntity->getPosition().z));
	float temperature = 0.5f / 3.f;
	int skyColor = Color::HSBtoRGB(0.62222224F - temperature * 0.05F, 0.5F + temperature * 0.1F, 1.0F);
	float r = (float)(skyColor >> 16 & 255) / 255.0F;
	float g = (float)(skyColor >> 8 & 255) / 255.0F;
	float b = (float)(skyColor & 255) / 255.0F;
	r *= brightness;
	g *= brightness;
	b *= brightness;
	float rainStrength = getRainStrength(rdt);

	if (rainStrength > 0.0F)
	{
		float f1 = (r * 0.3F + g * 0.59F + b * 0.11F) * 0.6F;
		float f2 = 1.0F - rainStrength * 0.75F;
		r = r * f2 + f1 * (1.0F - f2);
		g = g * f2 + f1 * (1.0F - f2);
		b = b * f2 + f1 * (1.0F - f2);
	}

	float thunderStrength = getWeightedThunderStrength(rdt);

	if (thunderStrength > 0.0F)
	{
		float f1 = (r * 0.3F + g * 0.59F + b * 0.11F) * 0.2F;
		float f2 = 1.0F - thunderStrength * 0.75F;
		r = r * f2 + f1 * (1.0F - f2);
		g = g * f2 + f1 * (1.0F - f2);
		b = b * f2 + f1 * (1.0F - f2);
	}

	if (m_lastLightningBolt > 0)
	{
		float f1 = (float)m_lastLightningBolt - rdt;

		if (f1 > 1.0F)
		{
			f1 = 1.0F;
		}

		f1 *= 0.45F;
		r = r * (1.0F - f1) + 0.8F * f1;
		g = g * (1.0F - f1) + 0.8F * f1;
		b = b * (1.0F - f1) + 1.0F * f1;
	}

	return Color(r, g, b);
}

Color WorldClient::getCloudColour(float rdt)
{
	float celestial = getCelestialAngle(rdt);
	float brightness = Math::Cos(celestial * Math::PI_2) * 2.0F + 0.5F;

	if (brightness < 0.0F)
	{
		brightness = 0.0F;
	}

	if (brightness > 1.0F)
	{
		brightness = 1.0F;
	}

	float r = (float)(m_cloudColour >> 16 & 255L) / 255.0F;
	float g = (float)(m_cloudColour >> 8 & 255L) / 255.0F;
	float b = (float)(m_cloudColour & 255L) / 255.0F;
	float rainStrength = getRainStrength(rdt);

	if (rainStrength > 0.0F)
	{
		float f1 = (r * 0.3F + g * 0.59F + b * 0.11F) * 0.6F;
		float f2 = 1.0F - rainStrength * 0.95F;
		r = r * f2 + f1 * (1.0F - f2);
		g = g * f2 + f1 * (1.0F - f2);
		b = b * f2 + f1 * (1.0F - f2);
	}

	r *= brightness * 0.9F + 0.1F;
	g *= brightness * 0.9F + 0.1F;
	b *= brightness * 0.85F + 0.15F;
	float thunderStrength = getWeightedThunderStrength(rdt);

	if (thunderStrength > 0.0F)
	{
		float f1 = (r * 0.3F + g * 0.59F + b * 0.11F) * 0.2F;
		float f2 = 1.0F - thunderStrength * 0.95F;
		r = r * f2 + f1 * (1.0F - f2);
		g = g * f2 + f1 * (1.0F - f2);
		b = b * f2 + f1 * (1.0F - f2);
	}

	return Color(r, g, b);
}

Color WorldClient::getFogColor(float rdt)
{
	float angle = getCelestialAngle(rdt);
	float var3 = Math::Cos_tab(angle * (float)Math::PI * 2.0F) * 2.0F + 0.5F;
	if (var3 < 0.0F)
	{
		var3 = 0.0F;
	}

	if (var3 > 1.0F)
	{
		var3 = 1.0F;
	}

	float var4 = 0.7529412F;
	float var5 = 0.84705883F;
	float var6 = 1.0F;
	var4 *= var3 * 0.94F + 0.06F;
	var5 *= var3 * 0.94F + 0.06F;
	var6 *= var3 * 0.91F + 0.09F;
	return Color(var4, var5, var6);
}

float WorldClient::getStarBrightness(float rdt)
{
	float celestial = getCelestialAngle(rdt);
	float brightness = 1.0f - (Math::Cos_tab(celestial * Math::PI_2) * 2.0f + 0.25f);

	if (brightness < 0.0f)
	{
		brightness = 0.0f;
	}

	if (brightness > 1.0f)
	{
		brightness = 1.0f;
	}

	return brightness * brightness * 0.5f;
}

void WorldClient::calculateInitialWeather()
{
	if (m_WorldInfo.isRaining())
	{
		m_rainingStrength = 1.0F;

		if (m_WorldInfo.isThundering())
		{
			m_thunderingStrength = 1.0F;
		}
	}
}

void WorldClient::tick()
{
	PROFILE_CPU("WorldClient::tick");
	updateWeather();
	World::tickUpdates(false);
	SkillEffectManager::Instance()->onUpdate();
	updateSpecialSound();
}

void WorldClient::updateWeather()
{
	int thunderTime = m_WorldInfo.getThunderTime();

	if (thunderTime <= 0)
	{
		if (m_WorldInfo.isThundering())
		{
			m_WorldInfo.setThunderTime(m_Rand.nextInt(12000) + 3600);
		}
		else
		{
			m_WorldInfo.setThunderTime(m_Rand.nextInt(168000) + 12000);
		}
	}
	else
	{
		--thunderTime;
		m_WorldInfo.setThunderTime(thunderTime);

		if (thunderTime <= 0)
		{
			m_WorldInfo.setThundering(!m_WorldInfo.isThundering());
		}
	}

	int rainTime = m_WorldInfo.getRainTime();

	if (rainTime <= 0)
	{
		if (m_WorldInfo.isRaining())
		{
			m_WorldInfo.setRainTime(m_Rand.nextInt(12000) + 12000);
		}
		else
		{
			m_WorldInfo.setRainTime(m_Rand.nextInt(168000) + 12000);
		}
	}
	else
	{
		--rainTime;
		m_WorldInfo.setRainTime(rainTime);

		if (rainTime <= 0)
		{
			m_WorldInfo.setRaining(!m_WorldInfo.isRaining());
		}
	}

	m_prevRainingStrength = m_rainingStrength;

	if (m_WorldInfo.isRaining())
	{
		m_rainingStrength = (float)((double)m_rainingStrength + 0.01);
	}
	else
	{
		m_rainingStrength = (float)((double)m_rainingStrength - 0.01);
	}

	if (m_rainingStrength < 0.0F)
	{
		m_rainingStrength = 0.0F;
	}

	if (m_rainingStrength > 1.0F)
	{
		m_rainingStrength = 1.0F;
	}

	m_prevThunderingStrength = m_thunderingStrength;

	if (m_WorldInfo.isThundering())
	{
		m_thunderingStrength = (float)((double)m_thunderingStrength + 0.01);
	}
	else
	{
		m_thunderingStrength = (float)((double)m_thunderingStrength - 0.01);
	}

	if (m_thunderingStrength < 0.0F)
	{
		m_thunderingStrength = 0.0F;
	}

	if (m_thunderingStrength > 1.0F)
	{
		m_thunderingStrength = 1.0F;
	}
}

bool WorldClient::canPlaySpecialSound(int sst, int max_tick)
{
	if (sst < 0 || sst >= SST_COUNT || sst >= MAX_SPECIAL_SOUND_TYPE) return false;

	int limit_max_tick = MAX_SPECIAL_SOUND_TICK;
	if (max_tick > 0)
	{
		limit_max_tick = max_tick;
	}
	if (m_special_sound_tick[sst] > limit_max_tick)
	{
		m_special_sound_tick[sst] = 0;
		return true;
	}

	return false;
}

void WorldClient::updateSpecialSound()
{
	for (int i = 0; i < MAX_SPECIAL_SOUND_TYPE; ++i)
	{
		m_special_sound_tick[i]++;
		if (m_special_sound_tick[i] > MAX_SPECIAL_SOUND_TICK_LIMIT)
		{
			m_special_sound_tick[i] = 0;
		}
	}
}

float WorldClient::getMoonPhaseFactor()
{
	return moonPhaseFactors[(int)(m_WorldInfo.getWorldTime() / 24000L) % 8];
}

ui64 WorldClient::findManorByPlayerPosition(EntityPlayer * player)
{
	if (m_manorControl && player)
	{
		Vector3 position = player->position;
		for (auto  item : m_manorControl->getOwners())
		{
			if (item.origin.x < position.x && item.origin.z < position.z && item.termini.x > position.x && item.termini.z > position.z && item.ownerId != player->getPlatformUserId())
			{
				return item.ownerId;
			}
		}
	}
	return 0;
}

EntityVehicle* WorldClient::newEntityVehicle(const Vector3& pos, int carId, float yaw)
{
	return LordNew EntityVehicleClient(this, carId, pos, yaw);
}

}
