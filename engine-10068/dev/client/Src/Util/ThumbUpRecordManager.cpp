#include "ThumbUpRecordManager.h"
#include "Util/ClientEvents.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	void ThumbUpRecordManager::parseThumbRecordData(const String & data)
	{
		Document doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
		{
			if (doc.HasMember("data"))
			{
				auto recordData = doc.FindMember("data")->value.GetArray();
				vector<ThumbUpRecord>::type records = {};
				for (const auto& item : recordData)
				{
					ThumbUpRecord record = ThumbUpRecord();
					if (item.HasMember("id"))
					{
						record.messageId = item.FindMember("id")->value.GetInt64();
					}
					if (item.HasMember("activeUserId"))
					{
						record.targetId = item.FindMember("activeUserId")->value.GetInt64();
					}
					if (item.HasMember("activeUserNickName"))
					{
						record.userName = item.FindMember("activeUserNickName")->value.GetString();
					}

					if (item.HasMember("activeUserPicUrl"))
					{
						record.picUrl = item.FindMember("activeUserPicUrl")->value.GetString();
					}
					records.push_back(record);
				}

				UpdateBlockCityThumbUpRecordEvent::emit(records);
			}
		}
	}
}
