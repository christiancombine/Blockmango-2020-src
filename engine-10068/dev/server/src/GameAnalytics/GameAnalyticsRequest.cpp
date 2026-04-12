#include "Core.h"
#include "GameAnalyticsRequest.h"
#include "GameAnalyticsSetting.h"
#include "cpr/cpr.h"
#include "Util/Sha256.h"
#include "Util/HMAC.h"
#include "Util/Base64.h"
#include "Util/Gzip.h"
#include "World/CprCallBackWrap.h"
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "GameVersionDefine.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Network/ServerNetwork.h"
#include "Server.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	std::string GameAnalyticsRequest::getAuthorization(const std::string& src)
	{
		uint8_t digest[SHA256::HASH_SIZE];
		HMAC<SHA256>::calc(src, GameAnalyticsSetting::SECRET_KEY.c_str(), digest);
		return Base64::encode(digest, sizeof(digest));
	}

	void GameAnalyticsRequest::asyncInit(int retryTimes)
	{
		auto doc = Document();
		doc.SetObject();
		Document::AllocatorType &allocator = doc.GetAllocator();
		Value data(kObjectType);
		data.AddMember("platform", "server", allocator);
		data.AddMember("os_version", StringRef(StringUtil::ToString(EngineVersionSetting::getEngineVersion()).c_str()), allocator);
		data.AddMember("sdk_version", "rest api v2", allocator);
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		data.Accept(writer);
		const char* json = buffer.GetString();

		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				if (retryTimes > 0)
				{
					auto foo = [=]()
					{
						asyncInit(retryTimes - 1);
					};
					Server::Instance()->addAsyncCall(foo);
				}
			}
			else
			{
				auto doc = Document();
				doc.Parse(response.text.c_str());
				if (doc.HasParseError())
				{
					return;
				}
				bool Enabled = doc.HasMember("enabled") ? doc.FindMember("enabled")->value.GetBool() : false;
				int64_t ServerTs = doc.HasMember("server_ts") ? doc.FindMember("server_ts")->value.GetInt64() : 0;
				auto foo = [=]()
				{
					GameAnalyticsSetting::ENABLE_ANALYTICS = Enabled;
					SCRIPT_EVENT::GameAnalyticsInitEvent::invoke(Enabled, ServerTs, GameAnalyticsSetting::CUSTOM_01, GameAnalyticsSetting::CUSTOM_02);
				};
				Server::Instance()->addAsyncCall(foo);
			}
		};
		std::string body{ json };

		cpr::Header header{
			{ "Content-Type", "application/json" },
			{ "Authorization", getAuthorization(body) }
		};

		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ GameAnalyticsSetting::INIT_URL.c_str() },
			cpr::Parameters{ },
			cpr::Body{ body },
			header
		);
	}

	void GameAnalyticsRequest::asyncEvents(ui64 rakssId, String body, int retryTimes)
	{
		String ip = ServerNetwork::Instance()->getPlayerIp(rakssId);
		asyncPost(GameAnalyticsSetting::EVENT_URL, ip, body, false, retryTimes);
	}

	void GameAnalyticsRequest::asyncPost(String url, String ip, String data, bool gzip, int retryTimes)
	{
		auto callback = [=](cpr::Response response)
		{
			if (response.status_code != 200)
			{
				if (retryTimes > 0)
				{
					auto foo = [=]()
					{
						asyncPost(url, ip, data, gzip, retryTimes - 1);
					};
					Server::Instance()->addAsyncCall(foo);
				}
			}
		};

		std::string body { data.c_str() };

		cpr::Header header{
			{ "Content-Type", "application/json" }
		};
		

		if (ip.length() > 0)
		{
			header["X-Forwarded-For"] = ip.c_str();
		}

		if (gzip)
		{
			std::string temp(compressBound(body.size()), '\0');
			unsigned long length = temp.size();
			auto *src = reinterpret_cast<unsigned char*>(&body[0]);
			if (gzCompress(src, body.size(), reinterpret_cast<unsigned char *>(&temp[0]), length) == Z_OK)
			{
				body = std::string(temp.data(), length);
				header["Content-Encoding"] = "gzip";
			}
		}

		header["Authorization"] = getAuthorization(body);

		CprCallBackWrap::Instance()->PostCallback(callback,
			cpr::ConnectTimeout{ std::chrono::milliseconds(CprCallBackWrap::HTTP_CONNECT_TIMEOUT) },
			cpr::Timeout{ CprCallBackWrap::HTTP_TIMEOUT },
			cpr::Url{ url.c_str() },
			cpr::Parameters{ },
			cpr::Body{ data.c_str() },
			header
		);
	}

}