#include "Promise.h"
#include <algorithm>

namespace BLOCKMAN
{
	namespace PROMISE
	{
		thread_local CallbackQueue callbackQueue;

		void runCallbacks()
		{
			for(auto it = callbackQueue.begin(); it!=callbackQueue.end(); )
			{
				const FutureCallback& _callback = *it;
				if (_callback())
				{
					it = callbackQueue.erase(it);
				}
				else
					++it;
			}
		}

		void clearCallbacks()
		{
			callbackQueue.clear();
		}
	}
}
