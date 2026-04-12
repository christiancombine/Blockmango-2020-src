#include "RankManager.h"
#include "Util/ClientEvents.h"
#include "FriendManager.h"

using namespace rapidjson;

namespace BLOCKMAN 
{
	void RankManager::parseGlobalRank(const String & data)
	{
		Document doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
		{
			vector<UserRank>::type ranks = {};
			for (const auto& item : doc.GetArray())
			{
				UserRank rank = UserRank();
				if (item.HasMember("userId"))
				{
					rank.userId = item.FindMember("userId")->value.GetInt64();
				}

				if (item.HasMember("userName") && !item.FindMember("userName")->value.IsNull())
				{
					rank.userName = item.FindMember("userName")->value.GetString();
				}

				if (item.HasMember("pic") && !item.FindMember("pic")->value.IsNull())
				{
					rank.picUrl = item.FindMember("pic")->value.GetString();
				}

				if (item.HasMember("integral"))
				{
					rank.score = item.FindMember("integral")->value.GetInt();
				}

				if (item.HasMember("rank"))
				{
					rank.rank = item.FindMember("rank")->value.GetInt();
				}

				ranks.push_back(rank);
			}

			sort(ranks.begin(), ranks.end(), [](const UserRank &rank1, const UserRank &rank2) {
				return rank1.rank < rank2.rank;
			});

			UpdateBlockCityGolbalRankEvent::emit(ranks);
		}
	}
	/**
	 * @fromType 1 friend 2 request
	 */
	void RankManager::parseUsersRank(const String & data, i32 fromType)
	{
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
		{
			vector<UserRank>::type ranks = {};
			for (const auto& item : doc.GetArray())
			{
				UserRank rank = UserRank();
				if (item.HasMember("userId"))
				{
					rank.userId = item.FindMember("userId")->value.GetInt64();
				}

				if (item.HasMember("userName") && !item.FindMember("userName")->value.IsNull())
				{
					rank.userName = item.FindMember("userName")->value.GetString();
				}

				if (item.HasMember("pic") && !item.FindMember("pic")->value.IsNull())
				{
					rank.picUrl = item.FindMember("pic")->value.GetString();
				}

				if (item.HasMember("integral"))
				{
					rank.score = item.FindMember("integral")->value.GetInt();
				}

				if (item.HasMember("rank"))
				{
					rank.rank = item.FindMember("rank")->value.GetInt();
				}
				ranks.push_back(rank);
			}
			sort(ranks.begin(), ranks.end(), [](const UserRank &rank1, const UserRank &rank2) {
				return rank1.rank < rank2.rank;
			});

			for (const auto& rank : ranks)
			{
				if (auto pal = FriendManager::findFriendById(rank.userId))
				{
					pal->score = rank.score;
				}
			}

			switch (fromType)
			{
			case 1:
			{
				vector<Friend>::type frends = {};
				for (const auto& pal : FriendManager::getFriends())
				{
					frends.push_back(*pal);
				}
				UpdateUserFriendEvent::emit(frends);
				UpdateBlockCityFriendRankEvent::emit(ranks);
			}
				break;
			case 2:
			{
				UpdateBlockCityUserRequestRankEvent::emit(ranks);
			}
				break;
			}
		}
	}

	void RankManager::parseMyRank(const String & data)
	{
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
		{
			UserRank myRank = UserRank();

			if (doc.HasMember("userId"))
			{
				myRank.userId = doc.FindMember("userId")->value.GetInt64();
			}

			if (doc.HasMember("userName") && !doc.FindMember("userName")->value.IsNull())
			{
				myRank.userName = doc.FindMember("userName")->value.GetString();
			}

			if (doc.HasMember("pic") && !doc.FindMember("pic")->value.IsNull())
			{
				myRank.picUrl = doc.FindMember("pic")->value.GetString();
			}

			if (doc.HasMember("integral"))
			{
				myRank.score = doc.FindMember("integral")->value.GetInt();
			}

			if (doc.HasMember("rank"))
			{
				myRank.rank = doc.FindMember("rank")->value.GetInt();
			}

			UpdateBlockCityMyRankEvent::emit(myRank);
		}
	}

	void RankManager::parseAppRank(const String & data, String type)
	{
		Document doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
		{
			if (doc.HasMember("pageInfo"))
			{
				auto pageInfo = doc.FindMember("pageInfo");// ->value["data"].GetArray();
				if (!pageInfo->value.HasMember("data"))
				{
					return;
				}
				auto rankData = pageInfo->value.FindMember("data")->value.GetArray();
				vector<UserRank>::type ranks = {};
				for (const auto& item : rankData)
				{
					UserRank rank = UserRank();
					if (item.HasMember("userId"))
					{
						rank.userId = item.FindMember("userId")->value.GetInt64();
					}

					if (item.HasMember("nickName") && !item.FindMember("nickName")->value.IsNull())
					{
						rank.userName = item.FindMember("nickName")->value.GetString();
					}

					if (item.HasMember("headPic") && !item.FindMember("headPic")->value.IsNull())
					{
						rank.picUrl = item.FindMember("headPic")->value.GetString();
					}

					if (item.HasMember("integral"))
					{
						rank.score = item.FindMember("integral")->value.GetInt();
					}

					if (item.HasMember("rank"))
					{
						rank.rank = item.FindMember("rank")->value.GetInt();
					}
					ranks.push_back(rank);
				}

				sort(ranks.begin(), ranks.end(), [](const UserRank &rank1, const UserRank &rank2) {
					return rank1.rank < rank2.rank;
				});

				UpdateBlockCityGolbalCharmRankEvent::emit(ranks, type);
			}
		}
	}

	void RankManager::parseUsersAppRank(const String & data, String type)
	{
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
		{
			vector<UserRank>::type ranks = {};
			for (const auto& item : doc.GetArray())
			{
				UserRank rank = UserRank();
				if (item.HasMember("userId"))
				{
					rank.userId = item.FindMember("userId")->value.GetInt64();
				}

				if (item.HasMember("nickName") && !item.FindMember("nickName")->value.IsNull())
				{
					rank.userName = item.FindMember("nickName")->value.GetString();
				}

				if (item.HasMember("headPic") && !item.FindMember("headPic")->value.IsNull())
				{
					rank.picUrl = item.FindMember("headPic")->value.GetString();
				}

				if (item.HasMember("integral"))
				{
					rank.score = item.FindMember("integral")->value.GetInt();
				}

				if (item.HasMember("rank"))
				{
					rank.rank = item.FindMember("rank")->value.GetInt();
				}
				ranks.push_back(rank);
			}

			sort(ranks.begin(), ranks.end(), [](const UserRank &rank1, const UserRank &rank2) {
				return rank1.rank < rank2.rank;
			});

			UpdateBlockCityFriendCharmRankEvent::emit(ranks, type);
		}
	}

	void RankManager::parseMyAppRank(const String & data, String type, i32 lastWeek)
	{
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
		{
			UserRank myRank = UserRank();

			if (doc.HasMember("userId"))
			{
				myRank.userId = doc.FindMember("userId")->value.GetInt64();
			}

			if (doc.HasMember("nickName") && !doc.FindMember("nickName")->value.IsNull())
			{
				myRank.userName = doc.FindMember("nickName")->value.GetString();
			}

			if (doc.HasMember("headPic") && !doc.FindMember("headPic")->value.IsNull())
			{
				myRank.picUrl = doc.FindMember("headPic")->value.GetString();
			}

			if (doc.HasMember("integral"))
			{
				myRank.score = doc.FindMember("integral")->value.GetInt();
			}

			if (doc.HasMember("rank"))
			{
				myRank.rank = doc.FindMember("rank")->value.GetInt();
			}

			if (lastWeek == 1)
			{
				//todo
			}
			else
			{
				UpdateBlockCityMyCharmRankEvent::emit(myRank, type);
			}
		}
	}

}

