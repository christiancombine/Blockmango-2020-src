#include "SchematicTaskQueue.h"
#include "SchematicModel.h"
#include "Object/Root.h"
#include "World/World.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Util/PlayerPrefs.h"
#include "Setting/ModuleBlockSetting.h"
#include "Common.h"

namespace BLOCKMAN
{
	SchematicTaskQueue::SchematicTaskQueue(World* world)
	{
		m_world = world;
	}

	SchematicTaskQueue::~SchematicTaskQueue()
	{
		clearQueue();
		m_world = NULL;
	}

	void SchematicTaskQueue::onUpdate()
	{	
		/*int changeBlockCount = 0;
		while (changeBlockCount <= 1000)
		{
			if (m_queue.empty())
				return;
			auto& front = m_queue.front();
			if (front.isCreate)
				updateCreateSchematic(front, changeBlockCount);
			else
				updateDestroySchematic(front, changeBlockCount);
		}*/
	}

	void SchematicTaskQueue::createSchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
		bool reversalX, bool reversalZ, bool coverOldBlock, int placeBlockId, int placeOffset, int extraParam)
	{
		SchematicTask task;
		task.schematicName = schematicName;
		task.startPos = startPos;
		task.reversalX = reversalX;
		task.reversalZ = reversalZ;
		task.coverOldBlock = coverOldBlock;
		task.placeRakssid = placeRakssid;
		task.placeBlockId = placeBlockId;
		task.placeOffset = placeOffset;
		task.extraParam = extraParam;
		task.isCreate = true;
		int count = 0;
		updateCreateSchematic(task, count);
		//m_queue.push_back(task);
	}

	void SchematicTaskQueue::destroySchematic(const String& schematicName, const BlockPos& startPos, ui64 placeRakssid,
		bool reversalX, bool reversalZ, int placeOffset, int extraParam)
	{
		SchematicTask task;
		task.schematicName = schematicName;
		task.startPos = startPos;
		task.reversalX = reversalX;
		task.reversalZ = reversalZ;
		task.placeRakssid = placeRakssid;
		task.placeOffset = placeOffset;
		task.extraParam = extraParam;
		task.isCreate = false;
		int count = 0;
		updateDestroySchematic(task, count);
		//m_queue.push_back(task);
	}

	bool SchematicTaskQueue::initTaskSchematic(SchematicTask& task)
	{
		if (task.schematic == NULL)
		{
			if (!SCRIPT_EVENT::SchemeticPlaceBlockBeginEvent::invoke(task.placeRakssid, task.startPos, task.placeBlockId, task.extraParam))
			{
				//m_queue.pop_front();
				return false;
			}
			task.schematic = LordNewT(SchematicModel);
			task.schematic->resetData();
			String path = "";
			if (m_world->m_isClient)
				path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "dynamic", task.schematicName);
			else
				path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "dynamic", task.schematicName).c_str();
			if (!task.schematic->deserialize(path.c_str()))
			{
				LordSafeDelete(task.schematic);
				task.schematic = NULL;
				//m_queue.pop_front();
				return false;
			}
		}
		return true;
	}

	void SchematicTaskQueue::updateCreateSchematic(SchematicTask& task, int& changeBlockCount)
	{
		if (!initTaskSchematic(task))
			return;

		bool needSyncPlaceBlock = PlayerPrefs::Instance()->getBoolPrefs("NeedSyncSchematicPlaceBlock");
		int element_count = 0;
		i16 width = task.schematic->getWidth();
		i16 height = task.schematic->getHeight();
		i16 length = task.schematic->getLength();

		for (i16 y = task.processY; y < height; ++y)
		{
			for (i16 x = task.processX; x < width; ++x)
			{
				for (i16 z = task.processZ; z < length; ++z)
				{
					int index = (y * length + z) * width + x;

					ui8 block_id = (task.placeBlockId != 0 ? (task.schematic->getBlockId(index) != 0 ? 
						task.placeBlockId : 0) : task.schematic->getBlockId(index));

					if (block_id <= 0) 
						continue;

					changeBlockCount++;

					if (changeBlockCount > 10000000) // process 10000000 (return when count = 10000001)
					{
						task.processX = x;
						task.processY = y;
						task.processZ = z;
						return;
					}

					ui8 mate = task.schematic->getMate(index);

					element_count++;

					int pos_x = (int)x;
					int pos_y = (int)y;
					int pos_z = (int)z;

					int pos_x_old = pos_x;
					int pos_y_old = pos_y;
					int pos_z_old = pos_z;

					// 绕点旋转 270 180 90算法
					if (task.reversalX && !task.reversalZ)
					{
						pos_x = length - pos_z_old - task.placeOffset;
						pos_z = pos_x_old;
					}
					else if (task.reversalX && task.reversalZ)
					{
						pos_x = -pos_x_old;
						pos_z = length - pos_z_old - task.placeOffset;
						pos_x += width - task.placeOffset;
					}
					else if (!task.reversalX && task.reversalZ)
					{
						pos_x = -(length - pos_z_old) + task.placeOffset;
						pos_z = -pos_x_old;

						pos_z += width - task.placeOffset;
						pos_x += length - task.placeOffset;
					}

					pos_x += task.startPos.x;
					pos_y += task.startPos.y;
					pos_z += task.startPos.z;

					if (!task.coverOldBlock)
					{
						bool bHasBlock = m_world->getBlockId(BlockPos(pos_x, pos_y, pos_z)) > 0;
						bool bRet = SCRIPT_EVENT::SchemeticPlaceCoverEvent::invoke(task.placeRakssid,
							BlockPos(pos_x, pos_y, pos_z), block_id, mate, task.extraParam);
						task.placeCoverResult.emplace(std::make_pair(BlockPos(pos_x, pos_y, pos_z), bRet));
						if (bHasBlock && bRet)
						{
							continue;
						}
					}

					if (BlockManager::isAttachmentBlock((int)block_id))
					{
						NETWORK_DEFINE::S2CPacketUpdateBlockInfo attachment_block(BlockPos(pos_x, pos_y, pos_z), (int)block_id, (int)mate);
						task.subRecordBlocks.push_back(attachment_block);
					}
					else
					{
						if (BlockManager::isStairsBlock((int)block_id))
						{
							mate = BlockManager::StairsBlockRotate(mate, task.reversalX, task.reversalZ);
						}
						else if (BlockManager::isWoodBlock((int)block_id))
						{
							mate = BlockManager::WoodBlockRotate(mate, task.reversalX, task.reversalZ);
						}
						else if (BlockManager::isFenceGateBlock((int)block_id))
						{
							mate = BlockManager::FenceGateBlockRotate(mate, task.reversalX, task.reversalZ);
						}
						else if ((int)block_id == BLOCK_ID_TRAP_DOOR)
						{
							mate = BlockManager::TrapDoorBlockRotate(mate, task.reversalX, task.reversalZ);
						}

						if (SCRIPT_EVENT::SchemeticPlaceBlockEvent::invoke(task.placeRakssid, BlockPos(pos_x, pos_y, pos_z),
							(int)block_id, (int)mate, task.extraParam))
						{
							m_world->setBlock(BlockPos(pos_x, pos_y, pos_z), (int)block_id, (int)mate, 3, needSyncPlaceBlock);
						}
					}
				}
				task.processZ = 0;
			}
			task.processX = 0;
		}

		// special blocks must be created lastly
		for (auto block : task.subRecordBlocks)
		{
			int flag = BlockManager::isDoorBlock((int)block.getBlockId()) ? 2 : 3;

			if (!task.coverOldBlock)
			{
				bool bHasBlock = m_world->getBlockId(BlockPos(block.x, block.y, block.z)) > 0;
				bool bRet = SCRIPT_EVENT::SchemeticPlaceCoverEvent::invoke(task.placeRakssid,
					BlockPos(block.x, block.y, block.z), (int)block.getBlockId(), (int)block.getBlockMeta(), task.extraParam);

				task.placeCoverResult.emplace(std::make_pair(BlockPos(block.x, block.y, block.z), bRet));
				if (bHasBlock && bRet)
				{
					continue;
				}
			}

			int blockMeta = (int)block.getBlockMeta();
			int blockId = (int)block.getBlockId();

			if (blockId == BLOCK_ID_LADDER)
			{
				blockMeta = BlockManager::LadderBlockRotate(blockMeta, task.reversalX, task.reversalZ);
			}
			else if (BlockManager::isTorchWoodBlock((int)blockId))
			{
				blockMeta = BlockManager::TorchWoodBlockRotate(blockMeta, task.reversalX, task.reversalZ);
			}
			else if (blockId == BLOCK_ID_VINE)
			{
				blockMeta = BlockManager::VineBlockRotate(blockMeta, task.reversalX, task.reversalZ);
			}
			else if (BlockManager::isDoorBlock((int)blockId))
			{
				blockMeta = BlockManager::DoorBlockRotate(blockMeta, task.reversalX, task.reversalZ);
			}

			if (SCRIPT_EVENT::SchemeticPlaceBlockEvent::invoke(task.placeRakssid, BlockPos(block.x, block.y, block.z),
				blockId, blockMeta, task.extraParam))
			{
				m_world->setBlock(BlockPos(block.x, block.y, block.z), blockId, blockMeta, flag, needSyncPlaceBlock);
			}
		}
		task.subRecordBlocks.clear();

		if (!needSyncPlaceBlock)
		{
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendCreateHouseFromSchematic(
				task.schematicName, task.startPos, task.placeCoverResult, task.reversalX, task.reversalZ,
				true, task.coverOldBlock, task.placeRakssid, task.placeBlockId, task.placeOffset);
		}

		finishSchematicTask(task);
	}

	void SchematicTaskQueue::updateDestroySchematic(SchematicTask& task, int& changeBlockCount)
	{
		if (!initTaskSchematic(task))
			return;

		bool needSyncPlaceBlock = PlayerPrefs::Instance()->getBoolPrefs("NeedSyncSchematicPlaceBlock");
		int element_count = 0;
		i16 width = task.schematic->getWidth();
		i16 height = task.schematic->getHeight();
		i16 length = task.schematic->getLength();

		// special blocks must be destroyed firstly
		for (i16 y = task.processY; y < height; ++y)
		{
			for (i16 x = task.processX; x < width; ++x)
			{
				for (i16 z = task.processZ; z < length; ++z)
				{
					int index = (y * length + z) * width + x;

					ui8 block_id = task.schematic->getBlockId(index);

					if (block_id <= 0)
						continue;

					changeBlockCount++;

					if (changeBlockCount > 10000000) // process 10000000 (return when count = 10000001)
					{
						task.processX = x;
						task.processY = y;
						task.processZ = z;
						return;
					}

					ui8 mate = task.schematic->getMate(index);

					int pos_x = (int)x;
					int pos_y = (int)y;
					int pos_z = (int)z;

					int pos_x_old = pos_x;
					int pos_y_old = pos_y;
					int pos_z_old = pos_z;

					// 绕点旋转 270 180 90算法
					if (task.reversalX && !task.reversalZ)
					{
						pos_x = length - pos_z_old - task.placeOffset;
						pos_z = pos_x_old;
					}
					else if (task.reversalX && task.reversalZ)
					{
						pos_x = -pos_x_old;
						pos_z = length - pos_z_old - task.placeOffset;
						pos_x += width - task.placeOffset;
					}
					else if (!task.reversalX && task.reversalZ)
					{
						pos_x = -(length - pos_z_old) + task.placeOffset;
						pos_z = -pos_x_old;

						pos_z += width - task.placeOffset;
						pos_x += length - task.placeOffset;
					}

					pos_x += task.startPos.x;
					pos_y += task.startPos.y;
					pos_z += task.startPos.z;

					if (BlockManager::isAttachmentBlock((int)block_id))
					{
						if (SCRIPT_EVENT::SchemeticPlaceBlockEvent::invoke(task.placeRakssid, BlockPos(pos_x, pos_y, pos_z), 
							0, 0,task.extraParam))
						{
							m_world->setBlock(BlockPos(pos_x, pos_y, pos_z), 0, 0, 3, needSyncPlaceBlock);
						}
					}
					else
					{
						NETWORK_DEFINE::S2CPacketUpdateBlockInfo normal_block(BlockPos(pos_x, pos_y, pos_z), 0, 0);
						task.subRecordBlocks.push_back(normal_block);
					}
				}
				task.processZ = 0;
			}
			task.processX = 0;
		}

		for (auto block : task.subRecordBlocks)
		{
			if (SCRIPT_EVENT::SchemeticPlaceBlockEvent::invoke(task.placeRakssid, BlockPos(block.x, block.y, block.z),
				0, 0, task.extraParam))
			{
				m_world->setBlock(BlockPos(block.x, block.y, block.z), (int)block.getBlockId(), 
					(int)block.getBlockMeta(), 3, needSyncPlaceBlock);
			}
		}
		task.subRecordBlocks.clear();

		if (!needSyncPlaceBlock)
		{
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendCreateHouseFromSchematic(
				task.schematicName, task.startPos, task.placeCoverResult, task.reversalX, task.reversalZ,
				false, task.coverOldBlock, task.placeRakssid, task.placeBlockId, task.placeOffset);
		}

		finishSchematicTask(task);
	}

	void SchematicTaskQueue::finishSchematicTask(SchematicTask& task)
	{
		int costBlockNum = 0;
		const auto data = ModuleBlockSetting::getModuleBlockData(task.schematicName.c_str());
		if (data)
			costBlockNum = data->_costBlockNum;
		SCRIPT_EVENT::SchemeticPlaceBlockFinishEvent::invoke(task.placeRakssid, task.placeBlockId, task.extraParam, costBlockNum);
		LordSafeDelete(task.schematic);
		task.schematic = NULL;
		//m_queue.pop_front();
	}

	void SchematicTaskQueue::clearQueue()
	{
		for (auto task : m_queue)
		{
			LordSafeDelete(task.schematic);
			task.schematic = NULL;
		}
		m_queue.clear();
	}
}