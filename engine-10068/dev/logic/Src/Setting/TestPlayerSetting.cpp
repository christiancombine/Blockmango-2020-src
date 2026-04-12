#include "TestPlayerSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	TestPlayer TestPlayerSetting::getStartPlayer()
	{
		CsvReader* pReader = new CsvReader();
		std::string filePath = PathUtil::ConcatPath(".//", "Media", "Setting", "TestPlayer.csv").c_str();
		bool bRet = pReader->LoadFile(filePath);
		if (!bRet)
		{
			LordLogError("Can not open TestPlayerSetting setting file %s", filePath.c_str());
			LordSafeDelete(pReader);
			return TestPlayer();
		}
		pReader->Read(); // discard the second line
		String UserId = "";
		int MaxWeight = 0;
		TestPlayer player;
		while (pReader->Read())
		{
			pReader->GetString("UserId", UserId);
			String lockFile = UserId + ".lock";
			if (PathUtil::IsFileExist(lockFile))
				continue;
			int Weight = 0;
			pReader->GetInt("Weight", Weight);
			if (Weight > MaxWeight)
			{
				MaxWeight = Weight;
				player.userId = StringUtil::ParseUI64(UserId);
				pReader->GetString("UserToken", player.userToken);
				pReader->GetString("LangCode", player.langCode);
			}
		}
		LordSafeDelete(pReader);
		return player;
	}

	list<TestPlayer>::type TestPlayerSetting::getPlayers()
	{
		list<TestPlayer>::type players;
		CsvReader* pReader = new CsvReader();
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		std::string filePath = PathUtil::ConcatPath(".//", "Media", "Setting", "TestPlayer.csv").c_str();
#else
		std::string filePath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "TestPlayer.csv").c_str();
#endif
		bool bRet = pReader->LoadFile(filePath);
		if (!bRet)
		{
			LordLogError("Can not open TestPlayerSetting setting file %s", filePath.c_str());
			LordSafeDelete(pReader);
			return players;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			TestPlayer player;
			String UserId = "";
			pReader->GetString("UserId", UserId);
			player.userId = StringUtil::ParseUI64(UserId);
			pReader->GetString("UserToken", player.userToken);
			pReader->GetString("PicUrl", player.picUrl);
			pReader->GetString("LangCode", player.langCode);
			pReader->GetInt("Team", player.team);
			pReader->GetInt("ManorId", player.manorId);
			pReader->GetInt("Sex", player.sex);
			String TargetUserId = "";
			pReader->GetString("TargetUserId", TargetUserId);
			player.targetUserId = StringUtil::ParseUI64(TargetUserId);
			int robot = 0;
			pReader->GetInt("IsRobot", robot);
			player.isRobot = (robot == 1);
			pReader->GetInt("RobotId", player.robotId);
			pReader->GetInt("FollowEnterType", player.followEnterType);
			players.push_back(player);
		}
		LordSafeDelete(pReader);
		return players;
	}

}