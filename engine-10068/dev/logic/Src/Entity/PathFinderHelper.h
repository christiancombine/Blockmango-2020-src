#pragma once

#include "Core.h"
#include "Object/Singleton.h"
#include "../Util/MemoryPool.h"
#include "PathNavigate.h"

using namespace LORD;
using namespace BLOCKMAN;

class PathFinderHelper : public ObjectAlloc, public Singleton<PathFinderHelper>
{
public:
	PathFinderHelper();
	~PathFinderHelper();

	PathPoint* alloc(const BlockPos& pos);
	void free(PathPoint* ptr);

private:
	MemoryPool<PathPoint> m_memoryPool;
};