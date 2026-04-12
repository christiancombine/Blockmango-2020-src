#pragma once

#include "Core.h"
#include "ClientPeer.h"
#include "cpr/cpr.h"

using namespace LORD;
namespace BLOCKMAN 
{
	class HttpRequest
	{
	public:
		const static String GET_MONEY_URL;
		const static String PAY_ORDER_URL;
		const static String RESUME_ORDER_URL;
		const static String CANCEL_ORDER_URL;
		const static String GAME_TYPE;
		const static String WEB_HTTP_BASE_URL;
		const static String GET_SELECTABLE_ACTION_URL;

		static void asyncLuaGet(const String& url, std::vector<std::vector<String>> params, std::function<void(String, int)> fun);
		static void asyncLuaPost(const String& url, std::vector<std::vector<String>> params, const String& body, std::function<void(String, int)> fun);
		static void asyncLuaPut(const String& url, std::vector<std::vector<String>> params, const String& body, std::function<void(String, int)> fun);
		static void asyncLuaDelete(const String& url, std::vector<std::vector<String>> params, const String& body, std::function<void(String, int)> fun);
		
		static void loadUserMoney(String url, i64 userId, std::function<void(i64, String)> fun);
		static void resumeOrder(String url, String orderId);
		static void cancelOrder(String url, String orderId, ui64 userId);
		static void pay(String ur, i32 currencyType, i32 uniqueId, i32 price, i64 platformUserId, std::function<void(i64, String, bool)> fun);
		static void getUserAction(String url, i64 userId, std::function<void(i64, String)> fun);
		static void checkUserRelation(String url, i64 userId, vector<i64>::type users, std::function<void(i64, String)> fun);

		static cpr::Parameters getHttpParameter();


		static String parameterSignature(ui32 timestamp, String nonce);

	private:
	};

}
