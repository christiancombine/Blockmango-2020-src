#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	enum SkyBlockGoodsStatus
	{
		SKYBLOCK_GOOD_LOCK = 1,
		SKYBLOCK_GOOD_UN_LOCK=2
	};

	enum SkyBlockGoodsType
	{
		SKYBLOCK_ITEM = 1,
		SKYBLOCK_PRIVILIGE = 2,
		SKYBLOCK_NPC_UNLOCK = 3,
		SKYBLOCK_MONEY_ITEM = 4,
		SKYBLOCK_BLOCK = 5,
	};

	enum SkyBlockGoodsTabType
	{
		SKYBLOCK_TAB_PRIVILIGE = 1,
		SKYBLOCK_TAB_OLD_PRI = 5,
	};

	enum SkyBlockPriType
	{
		SKYBLOCK_BLOCK_USE_MONEY = 1,
		SKYBLOCK_EXTEND_AREA_HALF = 2,
		SKYBLOCK_PRI_OF_BLOCK = 3,
		SKYBLOCK_GIFT_IRON_THRONE = 4,
		SKYBLOCK_UNLOCK_FLY = 5,
		SKYBLOCK_BUY_BLOCK_HALF = 6,
		SKYBLOCK_PRI_OF_NPC = 7,
		SKYBLOCK_GIFT_MONEY = 8,
		SKYBLOCK_GIFT_EQUIP_TOOL = 9,
		SKYBLOCK_UNLOCK_COMMON_NPC = 10,
		SKYBLOCK_UP_NPC_HALF = 11,
		SKYBLOCK_NPC_EFF_DOUBLE = 12,
		SKYBLOCK_FOREVER_UNLOCK_SKY = 13,
		SKYBLOCK_FOREVER_ALL_PRI = 14,
		SKYBLOCK_FOREVER_NEW_ALL_PRI = 15,
	};

	enum SkyBlockGoodsMoney
	{
		SKYBLOCK_DIAMOND = 1,
		SKYBLOCK_MONEY = 3,
	};

	struct SkyBlockPrivilegeReward;
	struct SkyBlockGoods
	{
	public:
		i32	id = 0;
		i32	tabId = 0;
		i32	itemId = 0;
		String	haveItem = "";
		i32	meta = 0;
		i32 coinId = 0;
		i32 coinNum = 0;
		i32 coinNum1 = 0;
		i32 coinNum2 = 0;
		i32 goodType = SKYBLOCK_ITEM;
		i32 groupId = 0;
		String groupText = "";
		String icon = "";
		String name = "";
		String detail = "";
		String weight = "";
		String coinImg = "";
		String lockTip = "";
		String disCountTips = "";
		StringArray weights;
		i32 level = 1;
		i32 hot = 0;
		i32 status = SKYBLOCK_GOOD_LOCK;
		int time = 0;
		int curTime = 0;
		bool forever = false;
		bool isHave = false; 
		bool isPriBlock = false;
		bool isPriNpc = false;
		bool haveForever = false;
		bool haveHalfPrice = false;
		bool haveUseMoney = false;
	};

	struct SkyBlockGoodGroup
	{
	public:
		i32 m_id;
		String m_icon;
		String m_name;
		vector<SkyBlockGoods>::type m_goods;
	};


	struct SkyBlockPriviLege
	{
	public:
		i32 m_id = 0;
		i32 m_type = 0;
		bool m_have = false;
		String detail = "";
	};


	class SkyBlockAppShopSetting : public Singleton<SkyBlockAppShopSetting>, public ObjectAlloc
	{
	public:

		SkyBlockAppShopSetting();
		~SkyBlockAppShopSetting();
		bool loadSetting();
		bool loadShopItemSetting();
		bool loadShopTabSetting();
		void unloadSetting();

		bool getShopItem(vector<SkyBlockGoodGroup>::type& goods);
		bool getShopItemByGroupId(int tabId, int groupId, SkyBlockGoodGroup& goods);
		void parseUpdateShopData(int SortType, int level, vector<SkyBlockGoods>::type& goods, const String& priData);
		void parseUpdateData(const String& priData, vector<SkyBlockGoods>::type& goods);
		void parseUpdateLevelAndSort(vector<SkyBlockGoods>::type& goods);
		void setPrivilegeData(vector<SkyBlockGoods>::type& goods);
		bool isHavePrivilege(int priId);
		bool isGoodLock(SkyBlockGoods& good);
		bool isGoodUnLock(SkyBlockGoods& good);
		bool setBlockPri(SkyBlockGoods& good);
		bool setNpcPri(SkyBlockGoods& good);
		bool getItemById(SkyBlockGoods& good, int id);
		bool getPrivilegeShopItemByTabId(int tabId, SkyBlockGoodGroup& goods);
		bool getPrivilegeRewardDataByPriId(SkyBlockPrivilegeReward& priInfo, int priId);

		int m_sortType = 0;
		int m_level = 0;
		int old_sortType = 0;
		int old_level = 0;

		vector<SkyBlockGoodGroup>::type m_goodGrops;
		map<int, SkyBlockPriviLege>::type m_priData;
	};

	struct SortShopItemByType
	{
		bool operator()(const SkyBlockGoods&first, const SkyBlockGoods& second)
		{
			auto index = SkyBlockAppShopSetting::Instance()->m_sortType;

			return StringUtil::ParseInt(first.weights[index - 1].c_str()) > StringUtil::ParseInt(second.weights[index - 1].c_str());
		}
	};

}