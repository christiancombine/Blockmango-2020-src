#include "ChunkServiceClient.h"
#include "Chunk/Chunk.h"
#include "cChunk/ChunkClient.h"
#include "cWorld/WorldClient.h"
#include "cWorld/BlockChangeRecorderClient.h"
#include "cWorld/SignTextsChangeRecorder.h"

namespace BLOCKMAN
{
	ChunkServiceClient::ChunkServiceClient(World * world, unsigned range)
		: ChunkService(world, range, true)
	{
		m_eventSubscription = ClientBlockChangeRecordClearEvent::subscribe([this](int x, int z) -> bool
		{
			auto chunk = getChunk(x, z);
			saveChunk(x, z, false);
			return true;
		});
	}

	ChunkServiceClient::~ChunkServiceClient()
	{
		m_eventSubscription.unsubscribe();
	}

	void ChunkServiceClient::prepareChunk(const ChunkPtr & chunk)
	{
		ChunkService::prepareChunk(chunk);
		BlockChangeRecorderClient::Instance()->applyChanges(chunk);
		SignTextsChangeRecorder::Instance()->applyChanges(chunk);

		WorldClient* pworld = dynamic_cast<WorldClient*>(m_world);
		pworld->updateNearChunkRef(std::static_pointer_cast<ChunkClient>(chunk).get());

		auto pChunkClient = std::static_pointer_cast<ChunkClient>(chunk);
		// pChunkClient->initDefaut();
		pChunkClient->generateHeightMap();
		pChunkClient->updateNearSectionRef();
		pChunkClient->generateSkylightMap();
	}
}