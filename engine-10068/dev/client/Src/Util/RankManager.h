#pragma once

#ifndef __RANK_MANAGER_HEADER__
#define __RANK_MANAGER_HEADER__
#include "Core.h"
#include <sstream>
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
using namespace LORD;

namespace BLOCKMAN {

	class UserRank
	{
	public:
		i32 rank = 0;
		i64 score = 0;
		ui64 userId = 0;
		String picUrl = "";
		String userName = "";
		UserRank() {}
	};

	class RankManager : public ObjectAlloc
	{
	public:
		static void parseGlobalRank(const String & data);
		static void parseUsersRank(const String & data, i32 fromType);
		static void parseMyRank(const String & data);

		static void parseAppRank(const String & data, String type);
		static void parseUsersAppRank(const String & data, String type);
		static void parseMyAppRank(const String & data, String type, i32 lastWeek);
	};
}

#endif