#include "ChunkService.h"
#include "Chunk/IChunkProvider.h"
#include "Chunk/IChunkStorage.h"
#include "Chunk/Chunk.h"
#include "World/World.h"
namespace BLOCKMAN
{
	ChunkPtr ChunkService::getChunk(int x, int z)
	{
		ChunkPtr chunk = m_cache.get(x, z);
		if (chunk)
		{
			return chunk;
		}
		auto iter = m_loadingTaskFutures.find({ x,z });
		if (iter == m_loadingTaskFutures.end()
			|| iter->second.cancel()
			|| iter->second.isCanceled())
		{
			{
				std::lock_guard<std::mutex> lk(m_prepareChunkMut);
				chunk = m_cache.get(x, z);
				if (chunk)
				{
					return chunk;
				}

				chunk = m_chunkProvider->provideChunk(x, z);
				LordAssert(chunk);
				m_cache.add(chunk);
			}
			prepareChunk(chunk);

		}
		else
		{
			chunk = iter->second.get();
			LordAssert(chunk);
		}
		
		return chunk;
	}

	// TODO: task cannot be canceled
	Future<ChunkPtr> ChunkService::getChunkAsync(int x, int z)
	{
		if (!m_supportAsync) {
			LordLogError("async unsupported but call getChunkAsync, check it", x, z);
			auto promise = LORD::make_shared<Promise<ChunkPtr>>();
			return promise->getFuture();
		}

		auto promise = LORD::make_shared<Promise<ChunkPtr>>();
		auto iter = m_loadingTaskFutures.find({ x, z });
		if (iter != m_loadingTaskFutures.end())
		{
			LordLogWarning("async get chunk task for %d,%d is already in queue.", x, z);
			// TODO:
			LordAssert(promise->changeToRunning());
			promise->setValue(getChunk(x, z));
		}
		else if (auto chunk = m_cache.get(x, z))
		{
			LordAssert(promise->changeToRunning());
			promise->setValue(chunk);
		}
		else
		{
			//LordLogInfo("workThread add task[%d,%d]", x, z);
			auto future = m_workerThread.addTask(&IChunkProvider::provideChunk, m_chunkProvider, x, z);
			future.onFinish([=] (const ChunkPtr& chunk) {
				m_cache.add(chunk);
				// LordLogInfo("workThread del task[%d,%d]", x, z);
				m_loadingTaskFutures.erase({ x, z });
				prepareChunk(chunk);
				if (promise->changeToRunning())
				{
					promise->setValue(chunk);
				}
			}).onFail([=](const std::exception_ptr& eptr) {
				try
				{
					std::rethrow_exception(eptr);
				}
				catch (std::exception& e)
				{
					LordLogError("Exception occured while loading chunk: %s", e.what());
				}
				catch (...)
				{
					LordLogError("Exception occured while loading chunk");
				}
				m_loadingTaskFutures.erase({ x, z });
				if (promise->changeToRunning())
				{
					promise->setException(eptr);
				}
			}).onCancel([=]() {
				m_loadingTaskFutures.erase({ x, z });
				promise->cancel();
			});
			m_loadingTaskFutures[{x, z}] = std::move(future);
		}
		return promise->getFuture();
	}

	void ChunkService::saveChunk(int x, int z, bool is_flush)
	{
		if (auto chunk = m_cache.get(x, z))
		{
			m_chunkStorage->storeChunk(chunk);
			if (is_flush)
			{
				m_chunkStorage->flushStorage();
			}	
		}
	}

	void ChunkService::saveAllChunks(bool is_flush)
	{
		for (const auto& chunk : m_cache)
		{
			m_chunkStorage->storeChunk(chunk);
		}
		if (is_flush)
		{
			m_chunkStorage->flushStorage();
		}
	}

	void ChunkService::prepareChunk(const ChunkPtr & chunk)
	{
		for (const auto& pair : *m_world->getLoadedEntityMap())
		{
			Entity* entity = pair.second;
			if (int(Math::Floor(entity->position.x / 16.0f)) == chunk->m_posX
				&& int(Math::Floor(entity->position.z / 16.0f)) == chunk->m_posZ)
			{
				if (entity->addedToChunk && chunkInCache(entity->chunkCoord.x, entity->chunkCoord.z))
				{
					m_world->getChunkFromChunkCoords(entity->chunkCoord.x, entity->chunkCoord.z)->removeEntityAtIndex(entity, entity->chunkCoord.y);
				}
				chunk->addEntity(entity);
			}
		}

		for (auto& pair : chunk->m_tileEntityMap)
		{
			m_world->addTileEntity(pair.second);
		}
	}
}