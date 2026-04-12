#ifndef __CHUNK_SERVICE_SERVER_HEADER__
#define __CHUNK_SERVICE_SERVER_HEADER__

#include "Chunk/ChunkService.h"
#include <limits>

namespace BLOCKMAN
{
	class ChunkServiceServer : public ChunkService
	{
	public:
		ChunkServiceServer(World* world)
			: ChunkService(world, 650, false)
		{}
		virtual bool chunkInCache(int x, int z) override
		{
			int emptyKey = (std::numeric_limits<int>::max)();
			int deletedKey = (std::numeric_limits<int>::min)();

			if (x != x || x == deletedKey || x == emptyKey) {
				LordLogError("nan occur, ChunkServiceServer::chunkInCache(x)  exception, x=%d, isnan(x)=%s",
					x, (x != x ? "true" : "false"));
				return false;
			}

			if (z != z || z == deletedKey || z == emptyKey) {
				LordLogError("nan occur, ChunkServiceServer::chunkInCache(z) exception, z=%d, isnan(z)=%s",
					z, (z != z ? "true" : "false"));
				return false;
			}

			return getChunk(x, z) != nullptr;
		}

	};
}

#endif