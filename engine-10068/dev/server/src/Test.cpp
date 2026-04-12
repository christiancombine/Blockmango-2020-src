#include "Test.h"
#include <iostream>
#include "Entity/PathNavigate.h"
#include "Util/MemoryPool.h"
//#include "Util/MemoryPool.h"
using namespace std;
using namespace BLOCKMAN;

Test::Test()
{
	testMemoryPool();
}

void Test::testMemoryPool()
{
	//cout<<"Test::testMemoryPool "<<sizeof(PathPoint)<<endl;

	//PathPoint pp;

	//MemoryPool<PathPoint> mp;

	/*MemoryPool<PathPoint> pool(64);
	auto pPathPoint = pool.alloc();*/

	/*MemoryPool<PathPoint, 100000> pool;
	for (int i = 0;i < 100000;++i)
	{
		auto ptr = pool.newElement(BlockPos(0, 0, 0));
		*ptr;
		pool.deleteElement(ptr);
	}*/

	/*for (int i = 0; i < 100000; ++i)
	{
		auto ptr = LordNew PathPoint(BlockPos(0, 0, 0));
		*ptr;
		LordDelete ptr;
	}*/
}
