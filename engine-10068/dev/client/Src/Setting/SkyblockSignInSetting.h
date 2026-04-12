#pragma once
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	enum SkyBlockSignInStatus
	{
		SKYBLOCK_UN_RECEIVED_SIGN,
		SKYBLOCK_ABLE_RECEIVED_SIGN,
		SKYBLOCK_HAVE_RECEIVED_SIGN
	};

	enum SkyBlockSignInItemType
	{
		SKYBLOCK_SIGNIN_ITEM = 1,
		SKYBLOCK_SIGNIN_MONEY = 2
	};


	struct SkyBlockSignInInfo
	{
	public:
		int id = 0;
		int day = 0;
		String title = "";
		int side = 1;
		String background = "";
		String effect = "";
		String image = "";
		int num = 0;
		int rewardId = 0;
		int status = SKYBLOCK_UN_RECEIVED_SIGN;
		int useRoom = 0;
		int rewardType = SKYBLOCK_SIGNIN_MONEY;
		String rewardName = "";
		String desc = "";
		int meta = 0;

	};

	class SkyBlockSignInSetting : public Singleton<SkyBlockSignInSetting>, public ObjectAlloc
	{
	public:

		SkyBlockSignInSetting();
		~SkyBlockSignInSetting();
	public:
		bool loadSetting();
		void unloadSetting();

		bool getSettingByOrder(SkyBlockSignInInfo& info, int goodId);
		bool getShowItems(vector<SkyBlockSignInInfo>::type& goods, int nowOrder, int status);
		vector<SkyBlockSignInInfo>::type m_goods;
	};
}
