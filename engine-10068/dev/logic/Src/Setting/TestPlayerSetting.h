#pragma once
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class TestPlayer
	{
	public:
		ui64 userId{ 0 };
		String userToken{ "" };
		String langCode{ "zh_CN" };
		String picUrl{ "" };
		int team{ 0 };
		int manorId{ 0 };
		int sex{ 0 };
		ui64 targetUserId{ 0 };
		bool isRobot = { false };
		int32_t robotId = { 0 };
		int32_t followEnterType = { 0 };
	};

	class TestPlayerSetting
	{
	public:
		TestPlayer getStartPlayer();
		list<TestPlayer>::type getPlayers();
	};
}