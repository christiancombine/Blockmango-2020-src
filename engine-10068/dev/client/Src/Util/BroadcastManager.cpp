#include "BroadcastManager.h"
#include "Common.h"
#include "Util/ClientEvents.h"
#include <sstream>
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "Util/FriendManager.h"
#include "Enums/Enums.h"
#include "Util/LanguageManager.h"
#include "game.h"

using namespace rapidjson;

namespace BLOCKMAN
{

	void BroadcastManager::parseBroadcastData(i32 type, const String & content)
	{
		switch ((BroadcastType)type)
		{
		case BroadcastType::RANCH_ORDER_HELP:
			praseRanchOrderHelp(content);
			break;
		case BroadcastType::RANCH_ORDER_FINISH:
			praseRanchOrderFinish(content);
			break;
		case BroadcastType::RANCH_RANK_ENTER_GAME:
			praseRanchEnterGame(content);
			break;
		case BroadcastType::NOTIFICATION:
			ReceiveNotificationEvent::emit(1, content);
			break;
		case BroadcastType::FRIEND_INVITE:
		case BroadcastType::RACE_INVITE:
			//case Bro
			parseFriendInvite(content);
			break;
		case BroadcastType::PRAISE:
			BlockCityGetPraiseEvent::emit();
			break;
		case BroadcastType::RACE_RESULE:
			parseRaceVehicle(content);
		default:
			praseCommon(content);
			break;
		}

	}

	void BroadcastManager::praseRanchOrderHelp(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError())
		{
			if (doc.HasMember("userId") && doc.HasMember("id"))
			{
				i64 userId = doc.FindMember("userId")->value.GetInt64();
				i64 helpId = doc.FindMember("id")->value.GetInt64();
				const auto& pal = FriendManager::findUserById(userId);
				if (pal && LanguageManager::Instance())
				{
					String msg = StringUtil::Format(LanguageManager::Instance()->getString("gui_ranch_broadcast_order_help").c_str(), pal->nickName.c_str());
					BroadcastReceiveEvent::emit(BroadcastMessage(msg, 5000, 5000));
				}
			}
		}
		FriendUpdateEvent::emit();
	}

	void BroadcastManager::praseRanchOrderFinish(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError())
		{
			if (doc.HasMember("helperUserId") && doc.HasMember("id"))
			{
				i64 userId = doc.FindMember("helperUserId")->value.GetInt64();
				i64 helpId = doc.FindMember("id")->value.GetInt64();
				const auto& pal = FriendManager::findUserById(userId);
				if (pal && LanguageManager::Instance())
				{
					String msg = StringUtil::Format(LanguageManager::Instance()->getString("gui_ranch_broadcast_order_help_done").c_str(), pal->nickName.c_str());
					BroadcastReceiveEvent::emit(BroadcastMessage(msg, 5000, 5000));
				}
			}
		}
	}

	void BroadcastManager::praseRanchEnterGame(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError())
		{
			if (doc.HasMember("name ") && doc.HasMember("msg") && doc.HasMember("stayTime") && doc.HasMember("rollTime"))
			{
				String name = doc.FindMember("name")->value.GetString();
				String msg = doc.FindMember("msg")->value.GetString();
				i32 stayTime = doc.FindMember("stayTime")->value.GetInt();
				i32 rollTime = doc.FindMember("rollTime")->value.GetInt();
				String message = StringUtil::Format(LanguageManager::Instance()->getString(msg).c_str(), name.c_str());
				BroadcastReceiveEvent::emit(BroadcastMessage(message, rollTime, stayTime));
			}
		}
	}

	void BroadcastManager::praseCommon(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError())
		{

		}
	}

	void BroadcastManager::parseFriendInvite(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError() && doc.HasMember("userId") && doc.HasMember("picUrl") && doc.HasMember("nickName") && doc.HasMember("showTime"))
		{
			i64 userId = 0;
			if (doc.FindMember("userId")->value.IsInt64())
			{
				userId = doc.FindMember("userId")->value.GetInt64();
			}

			if (doc.FindMember("userId")->value.IsString())
			{
				userId = StringUtil::ParseI64(doc.FindMember("userId")->value.GetString());
			}

			i32 type = 0;
			if (doc.FindMember("type")->value.IsInt())
			{
				type = doc.FindMember("type")->value.GetInt();
			}

			String nickName = doc.FindMember("nickName")->value.GetString();
			String picUrl = doc.FindMember("picUrl")->value.GetString();
			i32 showTime = doc.FindMember("showTime")->value.GetInt() * 1000;
			if (userId != GameClient::CGame::Instance()->getPlatformUserId())
			{
				BlockCityInviteEvent::emit(userId, nickName, picUrl, showTime, type);
			}
		}
	}

	void BroadcastManager::parseRaceVehicle(const String & content)
	{
		Document  doc = Document();
		doc.Parse(content.c_str());
		if (!doc.HasParseError() && doc.HasMember("winner") && doc.HasMember("loser"))
		{
			String winner = "";
			if (doc.FindMember("winner")->value.IsString())
			{
				winner = doc.FindMember("winner")->value.GetString();
			}

			String loser = "";
			if (doc.FindMember("loser")->value.IsString())
			{
				loser = doc.FindMember("loser")->value.GetString();
			}

			BroadcastRaceVehicleResultEvent::emit(winner, loser);
		}
	}
}
