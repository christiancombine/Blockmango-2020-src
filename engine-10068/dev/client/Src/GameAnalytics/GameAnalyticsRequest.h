#ifndef __GAME_ANALYTICS_REQUEST_HEADER__
#define __GAME_ANALYTICS_REQUEST_HEADER__

#include <map>

using namespace LORD;

namespace BLOCKMAN
{
	class GameAnalyticsRequest : public Singleton<GameAnalyticsRequest>, public ObjectAlloc
	{
	public:
		void asyncInit(int retryTimes = 3);
		void asyncEvents(String body, int retryTimes = 3);
		void asyncPost(String url, String ip, String body, bool gzip, int retryTimes = 3);

	private:
		std::string getAuthorization(const std::string& src);
		
	};
}

#endif // !__GAME_ANALYTICS_HEADER__
