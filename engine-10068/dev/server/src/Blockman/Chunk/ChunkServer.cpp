#include "ChunkServer.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Blockman/World/BlockChangeRecorderServer.h"

#include "Script/GameServerEvents.h"
namespace BLOCKMAN
{

	bool ChunkServer::setBlockIDAndMetaWithoutSync(const BlockPos & pos, int blockID, int meta)
	{
		return Chunk::setBlockIDAndMeta(pos, blockID, meta);
	}

	bool ChunkServer::setBlockMetaWithoutSync(const BlockPos & pos, int meta)
	{
		return Chunk::setBlockMeta(pos, meta);
	}

	bool ChunkServer::setBlockIDAndMeta(const BlockPos & pos, int blockID, int meta, bool immediate)
	{
		int oldBlockId = getBlockID(pos);
		int oldBlockMeta = getBlockMeta(pos);

		auto absoluteBlockPos = toAbsoluteBlockPos(pos);
		bool isRecordAdd = BlockChangeRecorderServer::Instance()->record(absoluteBlockPos, oldBlockId, oldBlockMeta, blockID, meta);

		if (Chunk::setBlockIDAndMeta(pos, blockID, meta))
		{
			if (immediate)
			{
				auto sender = Server::Instance()->getNetwork()->getSender();
				Server::Instance()->getWorld()->addSingleUpdateBlock(absoluteBlockPos, blockID, meta);
			}
			return true;
		}

		if (isRecordAdd)
		{
			BlockChangeRecorderServer::Instance()->removePosition(absoluteBlockPos);
		}

		return false;
	}

	bool ChunkServer::setBlockMeta(const BlockPos & pos, int meta)
	{
		int oldBlockId = getBlockID(pos);
		int oldBlockMeta = getBlockMeta(pos);
		if (Chunk::setBlockMeta(pos, meta))
		{
			auto absoluteBlockPos = toAbsoluteBlockPos(pos);
			auto sender = Server::Instance()->getNetwork()->getSender();
			BlockChangeRecorderServer::Instance()->record(absoluteBlockPos, oldBlockId, oldBlockMeta, oldBlockId, meta);
			Server::Instance()->getWorld()->addSingleUpdateBlock(absoluteBlockPos, oldBlockId, meta);
			// sender->broadCastUpdateBlock(absoluteBlockPos);
			return true;
		}
		return false;
	}

}
