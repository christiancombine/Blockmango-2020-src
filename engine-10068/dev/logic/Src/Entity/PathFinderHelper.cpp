#include "PathFinderHelper.h"

PathFinderHelper::PathFinderHelper()
{
	//
}

PathFinderHelper::~PathFinderHelper()
{
	//
}

PathPoint * PathFinderHelper::alloc(const BlockPos& pos)
{
	return m_memoryPool.newElement(pos);
}

void PathFinderHelper::free(PathPoint * ptr)
{
	m_memoryPool.deleteElement(ptr);
}
