#pragma once

#include "Core.h"
#include "cpr/cpr.h"

using namespace LORD;
namespace BLOCKMAN 
{
	struct ClientHttpResponse
	{
		i32 code = 0;
		String data = "";
	};

	class ClientHttpRequest
	{
	private:
		static String packageName;
		static String versionCode;

	public:

		const static bool ENABLED_LOG;

		static void asyncGet(String url, map<String, String>::type params, std::function<void(String, bool)> fun);
		static void asyncPost(String url, map<String, String>::type params, String body, std::function<void(String, bool)> fun);
		static void asyncPut(String url, map<String, String>::type params, String body, std::function<void(String, bool)> fun);
		static void asyncDelete(String url, map<String, String>::type params, String body, std::function<void(String, bool)> fun);

		static void asyncLuaGet(String url, std::vector<std::vector<String>> params, std::function<void(String, bool)> fun);
		static void asyncLuaPost(String url, std::vector<std::vector<String>> params, String body, std::function<void(String, bool)> fun);
		static void asyncLuaPut(String url, std::vector<std::vector<String>> params, String body, std::function<void(String, bool)> fun);
		static void asyncLuaDelete(String url, std::vector<std::vector<String>> params, String body, std::function<void(String, bool)> fun);

		static ClientHttpResponse parseResponse(bool isSuccess, String logTag, String response);

	public:
		static void refreshFriend();
		static void loadFriend();
		static void loadClanInfo();
		static void loadClanMember();
		static void loadUserRanchInfo(bool isFriend);
		static void loadMails();
		static void loadUserInfo(i64 userId);

		static void loadSeasonRule(int retry = 3);
		static void loadSeasonRank(int type, int highRank, int page, int size, int retry = 3);

		static void loadBedWarSeasonRule(const String &gameId, int retry = 3);
		static void loadBedWarSeasonRank(int type, int highRank, int page, int size, const String &gameId, int retry = 3);
		
		static void loadUserRequests();
		static void friendOperation(i32 type , i64 userId);

		//"type" is a self-define value, which is a key in DataBase.
		static void loadGlobalRank(String type, i32 pageNo, i32 pageSize);
		static void loadUsersRank(i32 fromType, vector<i64>::type userIds, String type);
		static void loadMyRank(String type);

		//the value "type" : "all", "month", "week"
		static void loadAppRank(String type, i32 pageNo, i32 pageSize);
		static void loadUsersAppRank(String type, vector<i64>::type userIds);
		//"isLastWeek" : default is 0 represent this week. 1 is last week.
		static void loadMyAppRank(String type, i32 lastWeek = 0);

		// "status" : "0 represent thumb-up, 1 represent send back"
		static void sendThumbUpRecord(i64 targetId, i32 status, String gameId);
		static void getThumbUpRecord(i32 pageNo, i32 pageSize);
		static void deleteThumbUpRecord(i64 messageId);

	private:
		static void loadUserDecoration(i64 userId);

	public:
		static String getBaseUrl();

	private:
		static void initClientInfo();
		static String getPackageName();
		static String getVersionCode();

		static String getUserId();
		static String getLanguage();
		static String getUserToken();
		static String getGameType();
		static cpr::Header getHttpHeader();
		
	};

}
