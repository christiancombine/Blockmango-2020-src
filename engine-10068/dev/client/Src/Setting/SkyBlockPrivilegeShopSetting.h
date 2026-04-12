#pragma once
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	enum SkyBlockPrivilegeTabType
	{
		SKYBLOCK_PRI_SUPREME = 2,
	};

	enum SkyBlockPrivilegeGoodsStatus
	{
		SKYBLOCK_PRIVILEGE_LOCK = 1,
		SKYBLOCK_PRIVILEGE_UN_LOCK = 2
	};

	enum SkyBlockPrivilegeRewardPanel
	{
		SKYBLOCK_PRI_REWARD_SUPREME = 1,
		SKYBLOCK_PRI_REWARD_NORNAL = 2,
		SKYBLOCK_RECIVE_NEW_PRI = 3
	};

	struct SkyBlockPrivilegeGoods
	{
	public:
		i32	id = 0;
		i32	tabId = 0;
		i32	itemId = 0;
		String haveItem = "";
		i32 coinId = 0;
		i32 coinNum = 0;
		i32 status = SKYBLOCK_PRIVILEGE_LOCK;
		String icon = "";
		String name = "";
		String detail = "";
		String coinImg = "";
		String rewards = "";
		bool forever = false;
		bool isHave = false;
	};

	struct SkyBlockPrivilegeGoodGroup
	{
	public:
		i32 m_id;
		String m_name;
		vector<SkyBlockPrivilegeGoods>::type m_goods;
	};

	struct SkyBlockPrivilegeReward
	{
	public:
		i32	id = 0;
		String icon = "";
		String name = "";
		String detail = "";
	};

	class SkyBlockPrivilegeShopSetting : public Singleton<SkyBlockPrivilegeShopSetting>, public ObjectAlloc
	{
	public:

		SkyBlockPrivilegeShopSetting();
		~SkyBlockPrivilegeShopSetting();
		bool loadSetting();
		bool loadShopItemSetting();
		bool loadShopTabSetting();
		bool loadRewardItemSetting();
		void unloadSetting();

 		bool getShopItem(vector< struct SkyBlockPrivilegeGoodGroup>::type& goods);
 		void parseUpdateShopData(vector<SkyBlockPrivilegeGoods>::type& goods, const String& priData);
		void parseUpdateData(const String& priData, vector<SkyBlockPrivilegeGoods>::type& goods);
		bool getPrivilegeRewardItem(vector<SkyBlockPrivilegeReward>::type& goods, String rewards);
		bool getShopItemByTabId(vector<SkyBlockPrivilegeGoods>::type& goods, int tabId);
		bool getPrivilegeRewardDataByPriId(SkyBlockPrivilegeReward& goods, int priId);
		void parseUpdatePriImgData(const String& priData);
		bool isHavePriImg(int id);

		int m_sortType = 1;
		int m_level = 0;
		int old_sortType = 0;
		int old_level = 0;


		vector<int>::type m_priImg;

		vector<SkyBlockPrivilegeGoodGroup>::type m_goodGrops;
		vector<SkyBlockPrivilegeReward>::type m_reward;

	};
}