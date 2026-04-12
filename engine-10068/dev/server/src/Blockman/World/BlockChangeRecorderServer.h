#ifndef __BLOCK_CHANGE_RECORDER_SERVER_HEADER__
#define __BLOCK_CHANGE_RECORDER_SERVER_HEADER__

#include "Core.h"
#include "Server.h"
#include "Blockman/World/ServerWorld.h"
#include "Network/ServerNetwork.h"
#include "Blockman/Chunk/ChunkServer.h"
using namespace LORD;

namespace BLOCKMAN
{
    class BlockChangeRecorderServer : public Singleton<BlockChangeRecorderServer>, public ObjectAlloc
    {
    private:
        using BlockPos = Vector3i;

		struct ChunkRecord
		{
			i16 x;
			i16 z;
			ChunkRecord() {};
			ChunkRecord(int posX, int posZ) : x(posX >> 4), z(posZ >> 4){ }
			bool operator == (const ChunkRecord& rhs) const
			{
				return  (x == rhs.x && z == rhs.z);
			}
		};

		struct RecordItem
		{
			ui8 xz;
			ui8 y;
			ui16 idAndMeta;
			RecordItem() {}
			RecordItem(int posX, int posY, int posZ,int blockId, int meta )
				: xz(ui8(posX << 4 | posZ & 0xF))
				,y(ui8(posY))
				, idAndMeta(meta << 12 | blockId & 0xFFF)
			{}

			ui16 getId() const { return idAndMeta & 0xFFF; }
			ui8 getMeta() const { return (idAndMeta >> 12) & 15; }

			ui8 getX() const { return (xz >> 4) & 0xF; }
			ui8 getZ() const { return xz & 0xF; }
			BlockPos getChunkPos() { return BlockPos(getX(), y, getZ()); }

			bool operator == (const RecordItem& rhs) const
			{
				return  (xz == rhs.xz && y == rhs.y);
			}
		};

        struct BlockPosCompare
        {
            bool operator()(const RecordItem& lhs, const RecordItem& rhs) const
            {
                if (lhs.xz < rhs.xz)
                {
                    return true;
                }
                if (lhs.xz > rhs.xz)
                {
                    return false;
                }
                if (lhs.y < rhs.y)
                {
                    return true;
                }
                if (lhs.y > rhs.y)
                {
                    return false;
                }
                return false;
            }
        };


		struct ChunkCompare
		{
			bool operator()(const ChunkRecord& lhs, const ChunkRecord& rhs) const
			{
				if (lhs.x < rhs.x)
				{
					return true;
				}
				if (lhs.x > rhs.x)
				{
					return false;
				}

				if (lhs.z < rhs.z)
				{
					return true;
				}

				return false;
			}
		};

        using Records = std::set<RecordItem, BlockPosCompare>;
		std::map<ChunkRecord, Records, ChunkCompare> m_changedPositions;

    public:
        bool record(int x, int y, int z, int oldBlockId, int oldBlockMeta, int newBlockId, int newBlockMeta)
        {
            return record(BlockPos(x, y, z), oldBlockId, oldBlockMeta, newBlockId, newBlockMeta);
        }

		bool record(const BlockPos& blockPos, int oldBlockId, int oldBlockMeta, int newBlockId, int newBlockMeta)
        {
			ChunkRecord chunk(blockPos.x, blockPos.z);
			RecordItem item = RecordItem(blockPos.x & 0xF, blockPos.y, blockPos.z & 0xF, oldBlockId, oldBlockMeta);

			auto iter = m_changedPositions.find(chunk);
			if (iter != m_changedPositions.end())
			{
				auto blockIter = iter->second.find(item);
				if (blockIter != iter->second.end())
				{
					if (blockIter->getId() == newBlockId && blockIter->getMeta() == newBlockMeta)
					{
						iter->second.erase(blockIter);
						if (iter->second.empty())
						{
							m_changedPositions.erase(iter);
						}
						return false;
					}		
				}
				else
				{
					iter->second.insert(item);
				}
			}
			else
			{
				Records blocks;
				blocks.insert(item);
				m_changedPositions[chunk] = blocks;
			}
			return true;
        }

        const std::vector<BlockPos> getChangedPositions()
        {
			std::vector<BlockPos> blocks;
			for (const auto& chunk : m_changedPositions)
			{
				for (const auto& block : chunk.second)
				{
					blocks.emplace_back(BlockPos( int(chunk.first.x) << 4 | block.getX(), block.y, int(chunk.first.z) << 4 | block.getZ()));
				}
			}
			LordLogInfo("Records m_changedPositions size = [%7d]", blocks.size());
            return blocks;
        }

		int getChangesBlockSize() 
		{
			int count = 0;
			for (const auto& chunk : m_changedPositions)
			{
				for (const auto & block : chunk.second) {
					count++;
				}
			}
			return count;
		}

		void reset()
		{
			for (auto &chunk : m_changedPositions)
			{
				for (auto& item : chunk.second)
				{
					BlockPos pos = BlockPos(int(chunk.first.x) << 4 | item.getX(), item.y, int(chunk.first.z) << 4 | item.getZ());
					Server::Instance()->getWorld()->getChunkFromBlockCoords(pos.x, pos.z)->setBlockIDAndMetaWithoutSync(pos.getChunkPos(), item.getId(), item.getMeta());
				}
			}

			Server::Instance()->getNetwork()->getSender()->resetUpdateBlocks();
			this->m_changedPositions.clear();
		}

		bool isInChangeSet(const BlockPos& blockPos)
		{
			ChunkRecord chunk(blockPos.x, blockPos.z);
			auto iter = m_changedPositions.find(chunk);
			if (iter != m_changedPositions.end())
			{
				RecordItem item = RecordItem(blockPos.x & 0xF, blockPos.y, blockPos.z & 0xF, 0, 0);
				return iter->second.find(item) != iter->second.end();
			}
			return false;
		}

		void removePosition(const BlockPos& blockPos)
		{
			ChunkRecord chunk(blockPos.x, blockPos.z);
			auto chunkIter = m_changedPositions.find(chunk);
			if (chunkIter != m_changedPositions.end())
			{
				RecordItem item = RecordItem(blockPos.x & 0xF, blockPos.y, blockPos.z & 0xF, 0, 0);

				auto blockIter = chunkIter->second.find(item);
				if (blockIter != chunkIter->second.end())
				{
					chunkIter->second.erase(blockIter);
					if (chunkIter->second.empty())
					{
						m_changedPositions.erase(chunkIter);
					}
				}
			}
		}
    };
}

#endif // !__BLOCK_CHANGE_RECORDER_HEADER__
