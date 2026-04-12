#include "HttpRequest.h"
#include "Util/sha1.h"
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Server.h"
#include "ServerNetwork.h"
#include "World/World.h"
#include "World/Shop.h"
#include <sstream>
#include "Script/GameServerEvents.h"
#include "World/CprCallBackWrap.h"
#include "GameVersionDefine.h"
#include "Network/ClientPeer.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	const String HttpRequest::GAME_TYPE = "g1001";
	const String HttpRequest::WEB_HTTP_BASE_URL = "http://dev.mods.sandboxol.cn";

	const String HttpRequest::GET_MONEY_URL = "{url}/pay/i/api/v1/wealth/users/{userId}";
	const String HttpRequest::PAY_ORDER_URL = "{url}/pay/api/v2/inner/pay/users/purchase/game/props";
	const String HttpRequest::RESUME_ORDER_URL = "{url}/pay/api/v1/pay/users/game/props/billings";
	const String HttpRequest::CANCEL_ORDER_URL = "{url}/pay/api/v1/pay/inner/users/game/props/billings/refund";
	const String HttpRequest::GET_SELECTABLE_ACTION_URL = "{url}/decoration/api/v1/inner/decorations/actions";

	void HttpRequest::asyncLuaGet(const String& url, std::vector<std::vector<String>> params, std::function<void(String, int)> fun)
	{
		auto param = getHttpParameter();
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}
		LordLogInfo("http get request:%s?%s", url.c_str(), param.content.c_str());
		auto callback = [=](cpr::Response response)
		{
			int code = response.status_code;
			if (code != 200)
			{
				std::string message = response.error.message;
				std::string text = response.text;
				auto foo = [=]()
				{
					fun("{}", code);
					if (code != 404 && code != 0)
					{
						LordLogError("[SCRIPT_EXCEPTION]Server Http Request async get error(%s?%s), http status code : %d, error message : %s, text : %s",
							url.c_str(), param.content.c_str(), code, message.c_str(), text.c_str());
					}
					else
					{
						LordLogError("Server Http Request async get error(%s?%s), http status code : %d, error message : %s, text : %s",
							url.c_str(), param.content.c_str(), code, message.c_str(), text.c_str());
					}
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [=]()
			{
				fun(result, code);
			};
			Server::Instance()->addAsyncCall(foo);
		};
		CprCallBackWrap::Instance()->GetCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::asyncLuaPost(const String& url, std::vector<std::vector<String>> params, const String& body, std::function<void(String, int)> fun)
	{
		auto param = getHttpParameter();
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}
		LordLogInfo("http post request:%s?%s", url.c_str(), param.content.c_str());
		auto callback = [=](cpr::Response response)
		{
			int code = response.status_code;
			if (code != 200)
			{
				std::string message = response.error.message;
				std::string text = response.text;
				auto foo = [=]()
				{
					fun("{}", code);
					if (code != 404 && code != 0)
					{
						LordLogError("[SCRIPT_EXCEPTION]Server Http Request async post error(%s?%s), http status code : %d, error message : %s, text : %s",
							url.c_str(), param.content.c_str(), code, message.c_str(), text.c_str());
					}
					else
					{
						LordLogError("Server Http Request async post error(%s?%s), http status code : %d, error message : %s, text : %s",
							url.c_str(), param.content.c_str(), code, message.c_str(), text.c_str());
					}
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [=]()
			{
				fun(result, code);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}

	void BLOCKMAN::HttpRequest::asyncLuaPut(const String& url, std::vector<std::vector<String>> params, const String& body, std::function<void(String, int)> fun)
	{
		auto param = getHttpParameter();
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}
		LordLogInfo("http put request:%s?%s", url.c_str(), param.content.c_str());
		auto callback = [=](cpr::Response response)
		{
			int code = response.status_code;
			if (code != 200)
			{
				std::string message = response.error.message;
				std::string text = response.text;
				auto foo = [=]()
				{
					fun("{}", code);
					if (code != 404 && code != 0)
					{
						LordLogError("[SCRIPT_EXCEPTION]Server Http Request async put error(%s?%s), http status code : %d, error message : %s, text : %s",
							url.c_str(), param.content.c_str(), code, message.c_str(), text.c_str());
					}
					else
					{
						LordLogError("Server Http Request async put error(%s?%s), http status code : %d, error message : %s, text : %s",
							url.c_str(), param.content.c_str(), code, message.c_str(), text.c_str());
					}
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [=]()
			{
				fun(result, code);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->PutCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}

	void BLOCKMAN::HttpRequest::asyncLuaDelete(const String& url, std::vector<std::vector<String>> params, const String& body, std::function<void(String, int)> fun)
	{
		auto param = getHttpParameter();
		for (auto pr : params)
		{
			cpr::Parameter cprp(pr[0].c_str(), pr[1].c_str());
			param.AddParameter(cprp);
		}
		LordLogInfo("http delete request:%s?%s", url.c_str(), param.content.c_str());
		auto callback = [=](cpr::Response response)
		{
			int code = response.status_code;
			if (code != 200)
			{
				std::string message = response.error.message;
				std::string text = response.text;
				auto foo = [=]()
				{
					fun("{}", code);
					if (code != 404 && code != 0)
					{
						LordLogError("[SCRIPT_EXCEPTION]Server Http Request async delete error(%s?%s), http status code : %d, error message : %s, text : %s",
							url.c_str(), param.content.c_str(), code, message.c_str(), text.c_str());
					}
					else
					{
						LordLogError("Server Http Request async delete error(%s?%s), http status code : %d, error message : %s, text : %s",
							url.c_str(), param.content.c_str(), code, message.c_str(), text.c_str());
					}
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [=]()
			{
				fun(result, code);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		CprCallBackWrap::Instance()->DeleteCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			param,
			cpr::Body{ body.c_str() },
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}


	void HttpRequest::loadUserMoney(String url, i64 userId, std::function<void(i64, String)> fun)
	{
		String finalUrl = StringUtil::Replace(GET_MONEY_URL, "{url}", url.c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString(userId).c_str());
		LordLogInfo("[loadUserMoney] Url=%s", finalUrl.c_str());
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				auto foo = [=]()
				{
					LordLogError("load user money error, http status code : %d", response.status_code);
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [=]()
			{
				fun(userId, result);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		CprCallBackWrap::Instance()->GetCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::resumeOrder(String url, String orderId)
	{
		LordLogInfo("HttpRequest::resumeOrder orderId [%s]", orderId.c_str());

		String finalUrl = StringUtil::Replace(RESUME_ORDER_URL, "{url}", url.c_str());
		auto callback = [](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				auto foo = [=]()
				{
					LordLogError("resume order error, http status code : %d", response.status_code);
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
		};

		auto parameters = getHttpParameter();
		parameters.AddParameter({ "orderId", orderId.c_str() });

		CprCallBackWrap::Instance()->PutCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::cancelOrder(String url, String orderId, ui64 userId)
	{
		LordLogInfo("HttpRequest::cancelOrder orderId [%s]", orderId.c_str());
		String finalUrl = StringUtil::Replace(CANCEL_ORDER_URL, "{url}", url.c_str());
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				auto foo = [=]()
				{
					LordLogError("resume order error, http status code : %d", response.status_code);
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}

			auto foo = [=]()
			{
				LordLogInfo("HttpRequest::cancelOrder text [%s]", response.text.c_str());
				auto pree = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
				if (pree)
				{
					pree->loadUserMoney();
				}
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		parameters.AddParameter({ "orderId", orderId.c_str() });

		CprCallBackWrap::Instance()->PutCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::pay(String url, i32 currencyType, i32 uniqueId, i32 price, i64 platformUserId, std::function<void(i64, String, bool)> fun)
	{
        if(currencyType == 1)
            currencyType = 0;
		String finalUrl = StringUtil::Replace(PAY_ORDER_URL, "{url}", url.c_str());
		auto doc = LORD::make_shared<Document>();
		doc->SetObject();
		Document::AllocatorType &allocator = doc->GetAllocator();
		doc->AddMember("currency", currencyType, allocator);
		doc->AddMember("gameId", StringRef(Server::Instance()->getGameType().c_str()), allocator);
		doc->AddMember("propsId", uniqueId, allocator);
		doc->AddMember("quantity", price, allocator);
		Value userId(kNumberType);
		userId.SetInt64(platformUserId);
		doc->AddMember("userId", userId, allocator);

		int engineVersion = EngineVersionSetting::getEngineVersion();
		doc->AddMember("engineVersion", engineVersion, allocator);	

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (peer)
		{
			doc->AddMember("packageName", StringRef(peer->getPackageName().c_str()), allocator);
			doc->AddMember("appVersion", StringRef(peer->getVersionCode().c_str()), allocator);
		}
		else
		{
			doc->AddMember("packageName", "com.sandboxol.blockymods", allocator);
			doc->AddMember("appVersion", "1", allocator);
		}
#endif

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		doc->Accept(writer);
		const char* json = buffer.GetString();
		
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				auto foo = [=]()
				{
					LordLogError("pay error, http status code : %d", response.status_code);
					fun(platformUserId, "{}", false);
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [=]()
			{
				fun(platformUserId, result, true);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Body{ json },
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::getUserAction(String url, i64 userId, std::function<void(i64, String)> fun)
	{
		String finalUrl = StringUtil::Replace(GET_SELECTABLE_ACTION_URL, "{url}", url.c_str());
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				auto foo = [=]()
				{
					LordLogError("get user action error, http status code : %d", response.status_code);
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [=]()
			{
				fun(userId, result);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();
		parameters.AddParameter({ "userId", StringUtil::ToString(userId).c_str() });
		CprCallBackWrap::Instance()->GetCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}

	void HttpRequest::checkUserRelation(String url, i64 userId, vector<i64>::type users, std::function<void(i64, String)> fun)
	{
		String finalUrl = StringUtil::Replace("{url}/friend/api/v1/inner/friends/filter", "{url}", url.c_str());
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				auto foo = [=]()
				{
					LordLogError("check user relation error, http status code : %d", response.status_code);
				};
				Server::Instance()->addAsyncCall(foo);
				return;
			}
			String result = String(response.text.c_str());
			auto foo = [=]()
			{
				fun(userId, result);
			};
			Server::Instance()->addAsyncCall(foo);
		};

		auto parameters = getHttpParameter();

		String userArray = "";
		for (int i = 0; i <users.size(); i++)
		{
			if (i < users.size() - 1)
			{
				userArray += StringUtil::ToString(users[i]) + ",";
			}
			else
			{
				userArray += StringUtil::ToString(users[i]);
			}
		}

		cpr::Parameter cprp("friendIds", userArray.c_str());
		cpr::Parameter cprp2("userId", StringUtil::ToString(userId).c_str());
		parameters.AddParameter(cprp);
		parameters.AddParameter(cprp2);

		CprCallBackWrap::Instance()->GetCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ finalUrl.c_str() },
			parameters,
			cpr::Header{ { "Content-Type", CprCallBackWrap::CONTENT_TYPE.c_str() } }
		);
	}


	cpr::Parameters HttpRequest::getHttpParameter()
	{
		LORD::ui32 timestamp = LORD::Root::Instance()->getCurrentTime();
		char ibuf[10] = { 0 };
		snprintf(ibuf, sizeof(ibuf) - 1, "%d", timestamp);
		cpr::Parameters parameters =
		{
			cpr::Parameter{ "timestamp", StringUtil::ToString(timestamp).c_str() },
			cpr::Parameter{ "nonce", ibuf },
			cpr::Parameter{ "signature", parameterSignature(timestamp, ibuf).c_str() }
		};

		return parameters;
	}

	String HttpRequest::parameterSignature(ui32 timestamp, String nonce)
	{
		String info = Server::WEB_HTTP_SECRET + nonce + StringUtil::ToString(timestamp);
		SHA1 sha1;
        sha1.update(info.c_str());
        const std::string hash = sha1.final();
		return hash.c_str();
	}

}
