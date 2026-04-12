#include "ClientHttpRequest.h"
#include "Util/sha1.h"
#include <sstream>
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "World/CprCallBackWrap.h"
#include "Util/FriendManager.h"
#include "Util/MailManager.h"
#include "Util/ClientEvents.h"
#include "Util/RankManager.h"
#include "Util/ThumbUpRecordManager.h"
#include "game.h"
#include "ShellInterface.h"
#include "GameVersionDefine.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	const bool ClientHttpRequest::ENABLED_LOG = true;

	String ClientHttpRequest::packageName = "";
	String ClientHttpRequest::versionCode = "";

	void ClientHttpRequest::asyncGet(String url, map<String, String>::type params, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		std::stringstream ss;
		ss << url << "?";
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr.first.c_str(), pr.second.c_str());
			param.AddParameter(cprp);
			ss << pr.first.c_str() << "=" << pr.second.c_str() << "&";
		}

		LordLogInfo("Client http get request:%s", ss.str().c_str());

		String stringUrl(ss.str().c_str());
		auto callback = [fun, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string message = response.error.message;
				auto foo = [fun, code, stringUrl, message]()
				{
					fun("{}", false);
					LordLogError("Client Http Request async get error(%s), http status code : %d, error message : %s", stringUrl.c_str(), code, message.c_str());
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->GetCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			getHttpHeader()
		);
	}

	void ClientHttpRequest::asyncPost(String url, map<String, String>::type params, String body, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		std::stringstream ss;
		ss << url << "?";
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr.first.c_str(), pr.second.c_str());
			param.AddParameter(cprp);
			ss << pr.first.c_str() << "=" << pr.second.c_str() << "&";
		}
		LordLogInfo("Client http post request:%s", ss.str().c_str());

		String stringUrl(ss.str().c_str());
		auto callback = [fun, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string message = response.error.message;
				auto foo = [fun, code, stringUrl, message]()
				{
					fun("{}", false);
					LordLogError("Client Httpa sync post error(%s), http status code : %d, error message : %s", stringUrl.c_str(), code, message.c_str());
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			getHttpHeader()
		);
	}

	void ClientHttpRequest::asyncPut(String url, map<String, String>::type params, String body, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		std::stringstream ss;
		ss << url << "?";
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr.first.c_str(), pr.second.c_str());
			param.AddParameter(cprp);
			ss << pr.first.c_str() << "=" << pr.second.c_str() << "&";
		}
		LordLogInfo("Client http put request:%s", ss.str().c_str());

		String stringUrl(ss.str().c_str());
		auto callback = [fun, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string message = response.error.message;
				auto foo = [fun, code, stringUrl, message]()
				{
					fun("{}", false);
					LordLogError("Client Httpa sync put error(%s), http status code : %d, error message : %s", stringUrl.c_str(), code, message.c_str());
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->PutCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			getHttpHeader()
		);
	}

	void ClientHttpRequest::asyncDelete(String url, map<String, String>::type params, String body, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		std::stringstream ss;
		ss << url << "?";
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr.first.c_str(), pr.second.c_str());
			param.AddParameter(cprp);
			ss << pr.first.c_str() << "=" << pr.second.c_str() << "&";
		}
		LordLogInfo("Client http delete request:%s", ss.str().c_str());

		String stringUrl(ss.str().c_str());
		auto callback = [fun, stringUrl](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string message = response.error.message;
				auto foo = [fun, code, stringUrl, message]()
				{
					fun("{}", false);
					LordLogError("Client Httpa sync delete error(%s), http status code : %d, error message : %s", stringUrl.c_str(), code, message.c_str());
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->DeleteCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			getHttpHeader()
		);
	}

	void BLOCKMAN::ClientHttpRequest::asyncLuaGet(String url, std::vector<std::vector<String>> params, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}
		LordLogInfo("http get request:%s?%s", url.c_str(), param.content.c_str());
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string message = response.error.message;
				auto foo = [=]()
				{
					fun("{}", false);
					LordLogError("Client Http Request async get error(%s?%s), http status code : %d, error message : %s", url.c_str(), param.content.c_str(), code, message.c_str());
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->GetCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			getHttpHeader()
		);
	}

	void BLOCKMAN::ClientHttpRequest::asyncLuaPost(String url, std::vector<std::vector<String>> params, String body, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}
		LordLogInfo("http post request:%s?%s", url.c_str(), param.content.c_str());
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string message = response.error.message;
				auto foo = [=]()
				{
					fun("{}", false);
					LordLogError("Client Http Request async post error(%s?%s), http status code : %d, error message : %s", url.c_str(), param.content.c_str(), code, message.c_str());
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			getHttpHeader()
		);
	}

	void BLOCKMAN::ClientHttpRequest::asyncLuaPut(String url, std::vector<std::vector<String>> params, String body, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}
		LordLogInfo("http put request:%s?%s", url.c_str(), param.content.c_str());
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string message = response.error.message;
				auto foo = [=]()
				{
					fun("{}", false);
					LordLogError("Client Http Request async put error(%s?%s), http status code : %d, error message : %s", url.c_str(), param.content.c_str(), code, message.c_str());
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->PutCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			getHttpHeader()
		);
	}

	void BLOCKMAN::ClientHttpRequest::asyncLuaDelete(String url, std::vector<std::vector<String>> params, String body, std::function<void(String, bool)> fun)
	{
		cpr::Parameters param;
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}
		LordLogInfo("http delete request:%s?%s", url.c_str(), param.content.c_str());
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				int code = response.status_code;
				std::string message = response.error.message;
				auto foo = [=]()
				{
					fun("{}", false);
					LordLogError("Client Http Request async delete error(%s?%s), http status code : %d, error message : %s", url.c_str(), param.content.c_str(), code, message.c_str());
				};
				GameClient::CGame::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [result, fun]()
			{
				fun(result, true);
			};
			GameClient::CGame::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->DeleteCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			getHttpHeader()
		);
	}

	ClientHttpResponse ClientHttpRequest::parseResponse(bool isSuccess, String logTag, String response)
	{
		ClientHttpResponse result = ClientHttpResponse();
		Document doc = Document();
		doc.Parse(response.c_str());
		if (isSuccess && !doc.HasParseError() && doc.HasMember("code"))
		{
			int code = doc.FindMember("code")->value.GetInt();
			result.code = code;
			if (code == 1 && doc.HasMember("data"))
			{
				auto data = doc.FindMember("data");
				StringBuffer buffer;
				Writer<StringBuffer> writer(buffer);
				data->value.Accept(writer);
				result.data = String(buffer.GetString());
			}
		}

		if (result.code == 1)
		{
			if (ENABLED_LOG)
			{
				LordLogInfo("Client http  request: %s parse response succeed, result: %s", logTag.c_str(), result.data.length() < 1000 ? result.data.c_str() : "result.length > 1000");
			}
		}
		else
		{
			LordLogError("Client http  request: %s parse response error, response: %s", logTag.c_str(), response.c_str());
		}
		return result;
	}

	void ClientHttpRequest::refreshFriend()
	{
		loadFriend();
		loadClanMember();
	}

	void ClientHttpRequest::loadFriend()
	{
		String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends", "{url}", getBaseUrl().c_str());
		map<String, String>::type params = {
			{ "pageNo","0"},
			{ "pageSize","50" },
		};
		asyncGet(finalUrl, params, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadFriend", result);
			if (response.code == 1)
			{
				FriendManager::parseFriendData(response.data);
				if (getGameType() == "g1031")
				{
					loadUserRanchInfo(true);
				}
			}
		});
	}

	void ClientHttpRequest::loadClanInfo()
	{
		String finalUrl = StringUtil::Replace("{url}/clan/api/v1/clan/tribe/base", "{url}", getBaseUrl().c_str());
		asyncGet(finalUrl, map<String, String>::type(), [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadClanInfo", result);
			if (response.code == 1)
			{
				FriendManager::parseClanData(response.data);
			}
		});

	}

	void ClientHttpRequest::loadClanMember()
	{
		String finalUrl = StringUtil::Replace("{url}/clan/api/v1/clan/tribe/member", "{url}", getBaseUrl().c_str());
		asyncGet(finalUrl, map<String, String>::type(), [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadClanMember", result);
			if (response.code == 1)
			{
				FriendManager::parseClanMember(response.data);
				loadUserRanchInfo(false);
			}
		});
	}

	void ClientHttpRequest::loadUserRanchInfo(bool isFriend)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/farm/user-data", "{url}", getBaseUrl().c_str());
		map<String, String>::type p = map<String, String>::type();
		String str = "";
		int i = 0;
		const auto& friends = isFriend ? FriendManager::getFriends() : FriendManager::getMembers();
		for (auto pal : friends)
		{
			i++;
			if (i < (int)friends.size())
			{
				str += StringUtil::ToString(pal->userId) + ",";
			}
			else
			{
				str += StringUtil::ToString(pal->userId);
			}

		}
		p["ids"] = str;
		p["userId"] = getUserId();
		asyncGet(finalUrl, p, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadUserRanchInfo", result);
			if (response.code == 1)
			{
				FriendManager::parseRanchData(isFriend, response.data);
			}
		});
	}

	void ClientHttpRequest::loadMails()
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/mails", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameType"] = getGameType();
		asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadMails", result);
			if (response.code == 1)
			{
				MailManager::parseMailData(response.data);
			}
		});
	}

	void ClientHttpRequest::loadUserInfo(i64 userId)
	{
		String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends/{userId}", "{url}", getBaseUrl().c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString(userId).c_str());
		asyncGet(finalUrl, map<String, String>::type(), [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadUserInfo", result);
			if (response.code == 1)
			{
				FriendManager::parseUserInfo(userId, response.data);
				loadUserDecoration(userId);
			}
		});
	}

	void ClientHttpRequest::loadSeasonRule(int retry)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/segment/integral", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameId"] = getGameType();
		asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadSeasonRule", result);
			if (response.code == 1)
			{
				UpdateSeasonRuleEvent::emit(response.data);
			}
			else
			{
				if (retry > 0)
				{
					loadSeasonRule(retry - 1);
				}
			}
		});
	}

	void ClientHttpRequest::loadSeasonRank(int type, int highRank, int page, int size, int retry)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/segment/integral/rank", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameId"] = getGameType();
		param["highRank"] = StringUtil::ToString(highRank);
		param["pageNo"] = StringUtil::ToString(page);
		param["pageSize"] = StringUtil::ToString(size);
		asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadSeasonRank", result);
			if (response.code == 1)
			{
				UpdateSeasonRankDataEvent::emit(type, response.data);
			}
			else
			{
				if (retry > 0)
				{
					loadSeasonRank(type, highRank, page, size, retry - 1);
				}
			}
		});
	}


	void ClientHttpRequest::loadBedWarSeasonRule(const String &gameId, int retry)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/segment/integral", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameId"] = gameId;
		asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadBedWarSeasonRule", result);
			if (response.code == 1)
			{
				UpdateSeasonRuleEvent::emit(response.data);
			}
			else
			{
				if (retry > 0)
				{
					loadBedWarSeasonRule(gameId, retry - 1);
				}
			}
		});
	}

	void ClientHttpRequest::loadUserRequests()
	{
		String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends/requests", "{url}", getBaseUrl().c_str());
		map<String, String>::type params = {
			{ "pageNo","0"},
			{ "pageSize","50" },
		};
		asyncGet(finalUrl, params, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadUserRequests", result);
			if (response.code == 1)
			{
				FriendManager::parseUserRequests(response.data);
			}
		});
	}
	
	/**
	 * @type 1 = agree 2 refuse 3 delete
	 */
	void ClientHttpRequest::friendOperation(i32 type, i64 userId)
	{
		switch (type)
		{
		case 1:
			{
				String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends/{otherId}/agreement", "{url}", getBaseUrl().c_str());
				finalUrl = StringUtil::Replace(finalUrl, "{otherId}", StringUtil::ToString(userId).c_str());
				asyncPut(finalUrl, map<String, String>::type(), "", [userId](String result, bool isSuccess) {
					ClientHttpResponse response = parseResponse(isSuccess, "friendOperationAgreeAdd", result);
					if (response.code == 1)
					{
						FriendOperationForAppHttpResultEvent::emit(3, userId);
						loadUserRequests();
						loadFriend();
					}
				});
			}
			break;
		case 2:
			{
				String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends/{otherId}/rejection", "{url}", getBaseUrl().c_str());
				finalUrl = StringUtil::Replace(finalUrl, "{otherId}", StringUtil::ToString(userId).c_str());
				asyncPut(finalUrl, map<String, String>::type(), "", [=](String result, bool isSuccess) {
					ClientHttpResponse response = parseResponse(isSuccess, "friendOperationRefuseAdd", result);
					if (response.code == 1)
					{
						loadUserRequests();
					}
				});
			}
			break;
		case 3:
			{
				String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends", "{url}", getBaseUrl().c_str());
				map<String, String>::type param = {
					{ "friendId" , StringUtil::ToString(userId) }
				};
				asyncDelete(finalUrl, param, "", [=](String result, bool isSuccess) {
					ClientHttpResponse response = parseResponse(isSuccess, "friendOperationDelete", result);
					if (response.code == 1)
					{
						FriendOperationForAppHttpResultEvent::emit(5, userId);
						loadFriend();
					}
				});
			}
			break;
		case 4:
		{
			String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends", "{url}", getBaseUrl().c_str());
			
			map<String, String>::type param = {};

			Document doc = Document();
			doc.SetObject();
			Document::AllocatorType &allocator = doc.GetAllocator();
			Value friendId(kNumberType);
			friendId.SetInt64(userId);
			doc.AddMember("friendId", friendId, allocator);
			doc.AddMember("msg", "", allocator);
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			doc.Accept(writer);
			asyncPost(finalUrl, param, buffer.GetString(), [=](String result, bool isSuccess) {
				ClientHttpResponse response = parseResponse(isSuccess, "friendOperationAdd", result);
				if (response.code == 1)
				{
					FriendOperationForAppHttpResultEvent::emit(4, userId);
				}
				else
				{
					FriendOperationForAppHttpResultEvent::emit(1001, userId);
				}
			});
		}
		break;
		}
	}

	 void ClientHttpRequest::loadGlobalRank(String type, i32 pageNo, i32 pageSize)
	 {
		 String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/rank/top/{gameId}", "{url}", getBaseUrl().c_str());
		 finalUrl = StringUtil::Replace(finalUrl, "{gameId}", getGameType().c_str());
		 map<String, String>::type param = map<String, String>::type();
		 param["userId"] = getUserId();
		 param["gameId"] = getGameType();
		 param["type"] = type;
		 param["pageNo"] = StringUtil::ToString(pageNo);
		 param["pageSize"] = StringUtil::ToString(pageSize);

		 asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			 ClientHttpResponse response = parseResponse(isSuccess, "loadGlobalRank", result);
			 if (response.code == 1)
			 {
				RankManager::parseGlobalRank(response.data);
			 }
		 });
	 }

	 void ClientHttpRequest::loadMyRank(String type)
	 {
		 String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/rank/user/{gameId}", "{url}", getBaseUrl().c_str());
		 finalUrl = StringUtil::Replace(finalUrl, "{gameId}", getGameType().c_str());
		 map<String, String>::type param = map<String, String>::type();
		 param["userId"] = getUserId();
		 param["gameId"] = getGameType();
		 param["type"] = type;

		 asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			 ClientHttpResponse response = parseResponse(isSuccess, "loadMyRank", result);
			 if (response.code == 1)
			 {
				 RankManager::parseMyRank(response.data);
			 }
		 });
	 }

	 void ClientHttpRequest::loadUsersRank(i32 fromType, vector<i64>::type userIds, String type)
	 {
		 String users = "";
		 for (size_t i = 0; i < userIds.size(); i++)
		 {
			 users += StringUtil::ToString(userIds[i]);
			 if (i < userIds.size() - 1)
			 {
				 users += ",";
			 }
		 }

		 String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/rank/users/{gameId}", "{url}", getBaseUrl().c_str());
		 finalUrl = StringUtil::Replace(finalUrl, "{gameId}", getGameType().c_str());
		 map<String, String>::type param = map<String, String>::type();
		 param["userId"] = getUserId();
		 param["gameId"] = getGameType();
		 param["type"] = type;
		 param["userIds"] = users;

		 asyncGet(finalUrl, param, [fromType](String result, bool isSuccess) {
			 ClientHttpResponse response = parseResponse(isSuccess, "loadUsersRank", result);
			 if (response.code == 1)
			 {
				 RankManager::parseUsersRank(response.data, fromType);
			 }
		 });
	 }

	void ClientHttpRequest::loadBedWarSeasonRank(int type, int highRank, int page, int size, const String &gameId, int retry)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/segment/integral/rank", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameId"] = gameId;
		param["highRank"] = StringUtil::ToString(highRank);
		param["pageNo"] = StringUtil::ToString(page);
		param["pageSize"] = StringUtil::ToString(size);
		asyncGet(finalUrl, param, [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadBedWarSeasonRank", result);
			if (response.code == 1)
			{
				UpdateBedWarSeasonRankDataEvent::emit(type, response.data);
			}
			else
			{
				if (retry > 0)
				{
					loadBedWarSeasonRank(type, highRank, page, size, gameId, retry - 1);
				}
			}
		});
	}


	void ClientHttpRequest::loadUserDecoration(i64 userId)
	{
		String finalUrl = StringUtil::Replace("{url}/decoration/api/v1/decorations/{otherId}/using", "{url}", getBaseUrl().c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{otherId}", StringUtil::ToString(userId).c_str());
		asyncGet(finalUrl, map<String, String>::type(), [=](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadUserDecoration", result);
			if (response.code == 1)
			{
				FriendManager::parseUserDress(userId, response.data);
			}
		});
	}

	void ClientHttpRequest::loadAppRank(String type, i32 pageNo, i32 pageSize)
	{
		String finalUrl = StringUtil::Replace("{url}/game/api/v1/games/{gameId}/rank", "{url}", getBaseUrl().c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{gameId}", getGameType().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameId"] = getGameType();
		param["type"] = type;
		param["pageNo"] = StringUtil::ToString(pageNo);
		param["pageSize"] = StringUtil::ToString(pageSize);

		asyncGet(finalUrl, param, [type](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadAppRank", result);
			if (response.code == 1)
			{
				RankManager::parseAppRank(response.data, type);
			}
		});
	}

	void ClientHttpRequest::loadUsersAppRank(String type, vector<i64>::type userIds)
	{
		String users = "";
		for (size_t i = 0; i < userIds.size(); i++)
		{
			users += StringUtil::ToString(userIds[i]);
			if (i < userIds.size() - 1)
			{
				users += ",";
			}
		}

		String finalUrl = StringUtil::Replace("{url}/game/api/v1/games/{gameId}/rank/users", "{url}", getBaseUrl().c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{gameId}", getGameType().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["gameId"] = getGameType();
		param["type"] = type;
		param["userIds"] = users;

		asyncGet(finalUrl, param, [type](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadUsersAppRank", result);
			if (response.code == 1)
			{
				RankManager::parseUsersAppRank(response.data, type);
			}
		});
	}

	void ClientHttpRequest::loadMyAppRank(String type, i32 lastWeek)
	{
		String finalUrl = StringUtil::Replace("{url}/game/api/v1/games/{gameId}/uses/rank", "{url}", getBaseUrl().c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{gameId}", getGameType().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameId"] = getGameType();
		param["type"] = type;
		param["lastWeek"] = lastWeek;

		asyncGet(finalUrl, param, [type, lastWeek](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "loadMyAppRank", result);
			if (response.code == 1)
			{
				RankManager::parseMyAppRank(response.data, type, lastWeek);
			}
		});
	}

	void ClientHttpRequest::sendThumbUpRecord(i64 targetId, i32 status, String gameId)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/insert/thumbs-up-record", "{url}", getBaseUrl().c_str());
		Document doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		Value activeUserId(kNumberType);
		Value passiveUserId(kNumberType);
		activeUserId.SetInt64(StringUtil::ParseI64(getUserId()));
		passiveUserId.SetInt64(targetId);
		doc.AddMember("activeUserId", activeUserId, allocator);
		doc.AddMember("passiveUserId", passiveUserId, allocator);
		doc.AddMember("gameId", StringRef(gameId.c_str()), allocator);
		doc.AddMember("status", status, allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc.Accept(writer);
		const char* json = buffer.GetString();
		map<String, String>::type params;
		asyncPost(finalUrl, params, json, [](String result, bool isSuccess) {
			ClientHttpResponse response =  parseResponse(isSuccess, "sendThumbUpRecord", result);
			if (response.code == 1)
			{
				// todo:

			}
		});
	}

	void ClientHttpRequest::getThumbUpRecord(i32 pageNo, i32 pageSize)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/get/thumbs-up-record", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["userId"] = getUserId();
		param["gameId"] = getGameType();
		param["pageNo"] = StringUtil::ToString(pageNo);
		param["pageSize"] = StringUtil::ToString(pageSize);

		asyncGet(finalUrl, param, [](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "getThumbUpRecord", result);
			if (response.code == 1)
			{
				ThumbUpRecordManager::parseThumbRecordData(response.data);
			}
		});
	}

	void BLOCKMAN::ClientHttpRequest::deleteThumbUpRecord(i64 messageId)
	{
		String finalUrl = StringUtil::Replace("{url}/gameaide/api/v1/delete/thumbs-up-record", "{url}", getBaseUrl().c_str());
		map<String, String>::type param = map<String, String>::type();
		param["id"] = StringUtil::ToString(messageId);
		asyncPost(finalUrl, param, "", [](String result, bool isSuccess) {
			ClientHttpResponse response = parseResponse(isSuccess, "deleteThumbUpRecord", result);
			if (response.code == 1)
			{
				DeleteBlockCityThumbUpRecordEvent::emit();
			}
		});
	}

	String ClientHttpRequest::getBaseUrl()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return "http://dev.mods.sandboxol.cn";
#else
		return GameClient::CGame::Instance()->getBaseUrl();
#endif
	}

	void BLOCKMAN::ClientHttpRequest::initClientInfo()
	{
		String clientInfo = GameClient::CGame::Instance()->getShellInterface()->getClientInfo();
		Document doc = Document();
		doc.Parse(clientInfo.c_str());
		if (doc.HasParseError())
		{
			versionCode = "1";
			packageName = "com.sandboxol.blockymods";
			return;
		}
		versionCode = doc.HasMember("version_code") ? StringUtil::ToString(doc.FindMember("version_code")->value.GetInt()) : "1";
		packageName = doc.HasMember("package_name") ? doc.FindMember("package_name")->value.GetString() : "com.sandboxol.blockymods";
	}

	String BLOCKMAN::ClientHttpRequest::getPackageName()
	{
		if (packageName.empty())
		{
			initClientInfo();
		}
		return packageName;
	}

	String BLOCKMAN::ClientHttpRequest::getVersionCode()
	{
		if (versionCode.empty())
		{
			initClientInfo();
		}
		return versionCode;
	}

	String ClientHttpRequest::getLanguage()
	{
		return GameClient::CGame::Instance()->getLanguage();
	}

	String ClientHttpRequest::getUserId()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return "1440";
#else
		i64 userId = GameClient::CGame::Instance()->getPlatformUserId();
		return StringUtil::ToString(userId);
#endif
	}

	String ClientHttpRequest::getUserToken()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiIxNzYiLCJpYXQiOjE1NTYyNDczMjgsInN1YiI6IjIwMTktMDQtMjYgMTA6NTU6MjgiLCJpc3MiOiJTYW5kYm94LVNlY3VyaXR5LUJhc2ljIiwiZXhwIjoxNTU3MTM2MzYxfQ.Nf3sad0xwDtbtKyGQhjTlcVZaxnfEjON96GplOl7_D0";
#else
		return GameClient::CGame::Instance()->getUserToken();
#endif
	}

	String ClientHttpRequest::getGameType()
	{
		return GameClient::CGame::Instance()->getGameType();
	}

	cpr::Header ClientHttpRequest::getHttpHeader()
	{
		return cpr::Header {
			{ "userId", getUserId().c_str() },
			{ "language", getLanguage().c_str() },
			{ "Access-Token", getUserToken().c_str() },
			{ "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() },
			{ "engineVersion", StringUtil::ToString(EngineVersionSetting::getEngineVersion()).c_str() },
			{ "packageName", getPackageName().c_str() },
			{ "appVersion", getVersionCode().c_str() },
		};
	}

}
