#include "SynBlockStrategy.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "TileEntity/TileEntitys.h"
#include "Util/PlayerPrefs.h"

namespace BLOCKMAN
{

	void SyncBlockStrategyVector::init(const Vector3& playPosition, std::vector<BlockPos>& blocks)
	{
		auto world = Server::Instance()->getWorld();

		if (world == nullptr) return;

		m_updateBlocks.reserve(blocks.size());

		for (auto block : blocks)
		{
			Vector3 block_pos((float)block.x, (float)block.y, (float)block.z);
			float distance = getDistance(block_pos, playPosition);
			m_updateBlocks.emplace_back(block, world->getBlockId(block), world->getBlockMeta(block), distance);
		}

		std::sort(m_updateBlocks.begin(), m_updateBlocks.end());

		m_moveNextUpdateDistanceTick = 0;
	}

	void SyncBlockStrategyVector::add(const Vector3& playPosition, BlockPos& pos, int blockId, int meta)
	{
		float distance = getDistance(pos, playPosition);
		m_updateBlocks.emplace_back(pos, blockId, meta, distance);

		std::sort(m_updateBlocks.begin(), m_updateBlocks.end());
	}

	void SyncBlockStrategyVector::move(const Vector3& playPosition)
	{
		if (m_moveNextUpdateDistanceTick < moveUpdateDistanceCd)
			m_moveNextUpdateDistanceTick++;

		if (m_moveNextUpdateDistanceTick >= moveUpdateDistanceCd)
		{
			if ((int)m_updateBlocks.size() <= 0)
				return;

			bool distance_change = false;

			for (std::vector<UpdateBlockItem>::iterator iter = m_updateBlocks.begin(); iter != m_updateBlocks.end(); iter++)
			{
				Vector3 block_pos((float)iter->pos.x, (float)iter->pos.y, (float)iter->pos.z);
				float distance = getDistance(block_pos, playPosition);

				if (Math::Abs(distance - iter->distance) > needChangeDistance)
				{
					distance_change = true;
					iter->distance = distance;
				}
			}

			if (distance_change)
			{
				std::sort(m_updateBlocks.begin(), m_updateBlocks.end());
			}

			m_moveNextUpdateDistanceTick = 0;
		}
	}

	void SyncBlockStrategyVector::sync(ui64 raknetID)
	{
		currentWaitSyncTime += Server::Instance()->getServerFramTime();
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		if (currentWaitSyncTime > 5.0f)
		{
			currentWaitSyncTime = 0;
			currentSyncBlockNum = 0;
		}
#else
		if (currentWaitSyncTime > 0.5f)
		{
			currentWaitSyncTime = 0;
			currentSyncBlockNum = 0;
		}
#endif // LORD_PLATFORM == LORD_PLATFORM_WINDOWS

		if (currentSyncBlockNum >= onceSyncBlockNum)
			return;

		if ((int)m_updateBlocks.size() <= 0) 
			return;

		auto world = Server::Instance()->getWorld();

		if (world == nullptr) 
			return;

		std::vector<BlockInfo> updatesBlock;
		std::vector<TileEntitySign*> titleEntitySignVec;

		// once update to client some blocks
		for (std::vector<UpdateBlockItem>::iterator iter = m_updateBlocks.begin(); iter != m_updateBlocks.end(); )
		{
			if ((int)updatesBlock.size() >= onceSyncBlockNum)
			{
				break;
			}

			if (iter->distance < PlayerPrefs::Instance()->getIntPrefs("BlockNeedUpdateDistance"))
			{
				Vector3i position(iter->pos.x, iter->pos.y, iter->pos.z);
				BlockInfo block_info(position, world->getBlockId(position), world->getBlockMeta(position));
				updatesBlock.push_back(block_info);

				auto tileEntity = dynamic_cast<TileEntitySign*>(Server::Instance()->getWorld()->getBlockTileEntity(position));
				if (tileEntity)
				{
					titleEntitySignVec.push_back(tileEntity);
				}

				iter = m_updateBlocks.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		if (updatesBlock.size() <= 0) 
			return;

		currentSyncBlockNum += (int)updatesBlock.size();

		auto sender = Server::Instance()->getNetwork()->getSender();

		if (!sender) return;

		sort(updatesBlock.begin(), updatesBlock.end(), [](const BlockInfo& first, const BlockInfo& second) {
			return first.y < second.y;
		});

		sender->sendUpdateBlocksWithDistance(raknetID, updatesBlock);

		for (auto tileEntity : titleEntitySignVec)
		{
			sender->sendSetSignTexts(tileEntity, raknetID);
		}
	}

	void SyncBlockStrategySection::init(const Vector3& playPosition, std::vector<BlockPos>& blocks)
	{
		auto world = Server::Instance()->getWorld();

		if (world == nullptr) return;

		for (auto block : blocks)
		{
			add(playPosition, block, world->getBlockId(block), world->getBlockMeta(block));
		}
	}

	void SyncBlockStrategySection::add(const Vector3& playPosition, BlockPos& pos, int blockId, int meta)
	{
		// LordLogInfo("SyncBlockStrategySection::add %d %d %d %d %d", pos.x, pos.y, pos.z, blockId, meta);
		SectionInfo sectionInfo;
		sectionInfo.absoluteX = i16(aPosToRPos(pos.x));
		sectionInfo.absoluteY = ui8(aPosToRPos(pos.y));
		sectionInfo.absoluteZ = i16(aPosToRPos(pos.z));

		i16 playerSectionX = i16(aPosToRPos((int)playPosition.x));
		i16 playerSectionY = i16(aPosToRPos((int)playPosition.y));
		i16 playerSectionZ = i16(aPosToRPos((int)playPosition.z));

		BlockPosInSection blockPosInSection;
		blockPosInSection.relativeX = i8(pos.x % 16);
		blockPosInSection.relativeY = i8(pos.y % 16);
		blockPosInSection.relativeZ = i8(pos.z % 16);

		BlockInfoInSection blockInfoSection;
		blockInfoSection.id = (ui16)blockId;
		blockInfoSection.meta = (ui8)meta;

		bool needSync = false;
		int distance = 0;
		int needSyncSectionDistance = PlayerPrefs::Instance()->getIntPrefs("BlockNeedSyncSectionDistance");
		int distanceChunkX = Math::Abs(sectionInfo.absoluteX - playerSectionX);
		int distanceChunkY = Math::Abs(sectionInfo.absoluteY - playerSectionY);
		int distanceChunkZ = Math::Abs(sectionInfo.absoluteZ - playerSectionZ);
		if (distanceChunkX <= needSyncSectionDistance &&
			distanceChunkY <= needSyncSectionDistance && 
			distanceChunkZ <= needSyncSectionDistance)
		{
			needSync = true;
			isNeedSync = true;
		}

		// LordLogInfo("mNearBySection playPosition %d %d %d", (int)playPosition.x, (int)playPosition.y, (int)playPosition.z);
		// LordLogInfo("mNearBySection sectionInfo.absoluteX %d %d %d", sectionInfo.absoluteX, sectionInfo.absoluteY, sectionInfo.absoluteZ);
		// LordLogInfo("mNearBySection playerSection %d %d %d", playerSectionX, playerSectionY, playerSectionZ);

		auto sectionIter = mNearBySection.find(sectionInfo);

		if (sectionIter != mNearBySection.end())
		{
			auto blockIter = sectionIter->second.blockInfoInSectionMap.find(blockPosInSection);

			if (blockIter != sectionIter->second.blockInfoInSectionMap.end())
			{
				blockIter->second = blockInfoSection;
			}
			else
			{
				sectionIter->second.blockInfoInSectionMap[blockPosInSection] = blockInfoSection;
				blockNum++;
			}

			sectionIter->second.isNeedSync = needSync;
		}
		else
		{
			BlockInfoInSectionAll blockInfoInSectionAll;
			blockInfoInSectionAll.isNeedSync = needSync;
			blockInfoInSectionAll.distance = Math::Sqrt(distanceChunkX * distanceChunkX +
				distanceChunkY * distanceChunkY + distanceChunkZ * distanceChunkZ);
			blockInfoInSectionAll.blockInfoInSectionMap[blockPosInSection] = blockInfoSection;
			mNearBySection[sectionInfo] = blockInfoInSectionAll;
			blockNum++;
		}
	}

	void SyncBlockStrategySection::move(const Vector3& playPosition)
	{
		// still in the same section
		if (aPosToRPos((int)playPosition.x) == aPosToRPos((int)mLastSection.absoluteX)
			&& aPosToRPos((int)playPosition.y) == aPosToRPos((int)mLastSection.absoluteY)
			&& aPosToRPos((int)playPosition.z) == aPosToRPos((int)mLastSection.absoluteZ))
		{
			return;
		}

		// LordLogInfo("SyncBlockStrategySection::move %d %d %d", (int)playPosition.x, (int)playPosition.y, (int)playPosition.z);
		mLastSection.absoluteX = (i16)playPosition.x;
		mLastSection.absoluteY = (ui8)playPosition.y;
		mLastSection.absoluteZ = (i16)playPosition.z;

		// section has changed
		isNeedSync = false;
		int needSyncSectionDistance = PlayerPrefs::Instance()->getIntPrefs("BlockNeedSyncSectionDistance");
		for (SectionInfoMap::iterator sectionIter = mNearBySection.begin(); sectionIter != mNearBySection.end(); sectionIter++)
		{
			int distanceChunkX = Math::Abs(((int)sectionIter->first.absoluteX) - aPosToRPos((int)playPosition.x));
			int distanceChunkY = Math::Abs(((int)sectionIter->first.absoluteY) - aPosToRPos((int)playPosition.y));
			int distanceChunkZ = Math::Abs(((int)sectionIter->first.absoluteZ) - aPosToRPos((int)playPosition.z));
			if (distanceChunkX <= needSyncSectionDistance && 
				distanceChunkY <= needSyncSectionDistance && 
				distanceChunkZ <= needSyncSectionDistance)
			{
				sectionIter->second.distance = Math::Sqrt(distanceChunkX * distanceChunkX +
					distanceChunkY * distanceChunkY + distanceChunkZ * distanceChunkZ);
				sectionIter->second.isNeedSync = true;
				isNeedSync = true;
				currentWaitSyncTime = 0;
				currentSyncBlockNum = 0;
				// LordLogInfo("isNeedSync x = true %d %d %d %d", (int)sectionIter->first.absoluteX, (int)playPosition.x, aPosToRPos((int)playPosition.x), aPosToRPos(((int)sectionIter->first.absoluteX) - aPosToRPos((int)playPosition.x)));
				// LordLogInfo("isNeedSync y = true %d %d %d %d", (int)sectionIter->first.absoluteY, (int)playPosition.y, aPosToRPos((int)playPosition.y), aPosToRPos(((int)sectionIter->first.absoluteY) - aPosToRPos((int)playPosition.y)));
				// LordLogInfo("isNeedSync z = true %d %d %d %d", (int)sectionIter->first.absoluteZ, (int)playPosition.z, aPosToRPos((int)playPosition.z), aPosToRPos(((int)sectionIter->first.absoluteZ) - aPosToRPos((int)playPosition.z)));
			}
			else
			{
				// LordLogInfo("isNeedSync x = false %d %d %d %d", (int)sectionIter->first.absoluteX, (int)playPosition.x, aPosToRPos((int)playPosition.x), aPosToRPos(((int)sectionIter->first.absoluteX) - aPosToRPos((int)playPosition.x)));
				// LordLogInfo("isNeedSync y = false %d %d %d %d", (int)sectionIter->first.absoluteY, (int)playPosition.y, aPosToRPos((int)playPosition.y), aPosToRPos(((int)sectionIter->first.absoluteY) - aPosToRPos((int)playPosition.y)));
				// LordLogInfo("isNeedSync z = false %d %d %d %d", (int)sectionIter->first.absoluteZ, (int)playPosition.z, aPosToRPos((int)playPosition.z), aPosToRPos(((int)sectionIter->first.absoluteZ) - aPosToRPos((int)playPosition.z)));
				sectionIter->second.isNeedSync = false;
			}
		}
	}

	void SyncBlockStrategySection::sync(ui64 raknetID)
	{
		currentWaitSyncTime += Server::Instance()->getServerFramTime();
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		if (currentWaitSyncTime > 5.0f)
		{
			currentWaitSyncTime = 0;
			currentSyncBlockNum = 0;
		}
#else
		if (currentWaitSyncTime > 0.5f)
		{
			currentWaitSyncTime = 0;
			currentSyncBlockNum = 0;
		}
#endif // LORD_PLATFORM == LORD_PLATFORM_WINDOWS

		if (currentSyncBlockNum >= onceSyncBlockNum)
			return;

		if (mNearBySection.empty())
			return;

		if (!isNeedSync) 
			return;	

		std::vector<BlockInfo> updatesBlock;
		std::vector<TileEntitySign*> titleEntitySignVec;

		int closestDistance = 1000000;
		int syncAbsoluteX = 0;
		int syncAbsoluteY = 0;
		int syncAbsoluteZ = 0;
		BlockInfoInSectionAll* closestSection = NULL;

		// find closest section
		for (SectionInfoMap::iterator sectionIter = mNearBySection.begin(); sectionIter != mNearBySection.end(); sectionIter++)
		{
			if (!sectionIter->second.isNeedSync)
				continue;

			if (closestDistance <= sectionIter->second.distance)
				continue;

			closestDistance = sectionIter->second.distance;
			closestSection = &sectionIter->second;
			syncAbsoluteX = rPosToaPos((int)sectionIter->first.absoluteX);
			syncAbsoluteY = rPosToaPos((int)sectionIter->first.absoluteY);
			syncAbsoluteZ = rPosToaPos((int)sectionIter->first.absoluteZ);
		}

		if (closestSection == NULL)
		{
			isNeedSync = false;
			return;
		}
	
		if (closestSection->blockInfoInSectionMap.size() > onceSyncBlockNum) // big size section, need wait
		{
			currentWaitSyncTime = 0;
		}

		// sync closest section blocks
		for (BlockInfoInSectionMap::iterator blockIter = closestSection->blockInfoInSectionMap.begin(); 
			blockIter != closestSection->blockInfoInSectionMap.end(); )
		{
			int absoluteX = (int)blockIter->first.relativeX + syncAbsoluteX;
			int absoluteY = (int)blockIter->first.relativeY + syncAbsoluteY;
			int absoluteZ = (int)blockIter->first.relativeZ + syncAbsoluteZ;

			Vector3i position(absoluteX, absoluteY, absoluteZ);
			BlockInfo block_info(position, (int)blockIter->second.id, (int)blockIter->second.meta);
			// LordLogInfo("SyncBlockStrategySection::sync %d %d %d %d %d", position.x, position.y, position.z, (int)blockIter->second.id, (int)blockIter->second.meta);
			updatesBlock.push_back(block_info);
			blockNum--;

			auto tileEntity = dynamic_cast<TileEntitySign*>(Server::Instance()->getWorld()->getBlockTileEntity(position));
			if (tileEntity)
			{
				titleEntitySignVec.push_back(tileEntity);
			}

			blockIter = closestSection->blockInfoInSectionMap.erase(blockIter);
		}

		// delete empty section
		for (SectionInfoMap::iterator sectionIter = mNearBySection.begin(); sectionIter != mNearBySection.end(); sectionIter++)
		{
			if (sectionIter->second.blockInfoInSectionMap.empty())
			{
				mNearBySection.erase(sectionIter);
				break;
			}
		}

		if (updatesBlock.size() <= 0)
		{
			isNeedSync = false;
			return;
		}
		
		currentSyncBlockNum += (int)updatesBlock.size();

		auto sender = Server::Instance()->getNetwork()->getSender();

		if (!sender) return;

		sort(updatesBlock.begin(), updatesBlock.end(), [](const BlockInfo& first, const BlockInfo& second) {
			return first.y < second.y;
		});
		sender->sendUpdateBlocksWithDistance(raknetID, updatesBlock);

		for (auto tileEntity : titleEntitySignVec)
		{
			sender->sendSetSignTexts(tileEntity, raknetID);
		}
	}

	int SyncBlockStrategySection::aPosToRPos(int a)
	{
		return (int)(a * 1.0 / 16);
	}

	int SyncBlockStrategySection::rPosToaPos(int r)
	{
		return r * 16;
	}
	
	bool SyncBlockContext::checkSync()
	{
		return mStrategy->checkSync();
	}

}
