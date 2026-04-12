#pragma once

#include "ChunkReadableStorageFileClient.h"
#include "ChunkClient.h"
#include "cWorld/WorldClient.h"
#include "cWorld/AnvilManagerClient.h"


namespace BLOCKMAN
{
	ChunkReadableStorageFileClient::ChunkReadableStorageFileClient(World* world, const LORD::String& regionDir)
		: ChunkReadableStorageFile(world)
	{
		m_anvilManager.reset(new AnvilManagerClient(regionDir));
	}

	void ChunkReadableStorageFileClient::prepareChunk(const ChunkPtr& chunk)
	{
		ChunkReadableStorageFile::prepareChunk(chunk);
		
		auto pChunkClient = std::static_pointer_cast<ChunkClient>(chunk);
		pChunkClient->reBuildVisibility();
	}

	ChunkPtr ChunkReadableStorageFileClient::readChunkFromAnvil(int x, int z)
	{
		auto anvil = m_anvilManager->getAnvil(chunkCoordToAnvilCoord(x), chunkCoordToAnvilCoord(z));
		if (!anvil)
		{
			auto ret = LORD::make_shared<ChunkClient>(m_world, x, z);
			//ret->initDefaut();
			return ret;
		}
		ChunkPtr chunk(anvil->extractChunk(x, z));
		if (!chunk)
		{
			auto ret = LORD::make_shared<ChunkClient>(m_world, x, z);
			//ret->initDefaut();
			return ret;
		}
		return chunk;
	}
}