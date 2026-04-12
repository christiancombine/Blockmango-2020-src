#pragma once

#include "Chunk/ChunkReadableStorageFile.h"

namespace BLOCKMAN
{
	class ChunkReadableStorageFileClient : public ChunkReadableStorageFile
	{
	public:
		ChunkReadableStorageFileClient(World* world, const LORD::String& regionDir);

	protected:
		virtual void prepareChunk(const ChunkPtr& chunk) override;
		virtual ChunkPtr readChunkFromAnvil(int x, int z) override;
	};
}