#ifndef __GAME_ANALYTICS_REQUEST_HEADER__
#define __GAME_ANALYTICS_REQUEST_HEADER__

#include <map>

using namespace LORD;

namespace BLOCKMAN
{
	class GameAnalyticsRequest
	{
	public:
		static void asyncInit(int retryTimes = 3);
		static void asyncEvents(ui64 rakssId, String body, int retryTimes = 3);
		static void asyncPost(String url, String ip, String body, bool gzip, int retryTimes = 3);

	private:
		static std::string getAuthorization(const std::string& src);
		
	};
}

#endif // !__GAME_ANALYTICS_HEADER__
