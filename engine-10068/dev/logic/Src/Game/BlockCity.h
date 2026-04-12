#pragma once
#ifndef __BLOCK_CITY_HEADER__
#define __BLOCK_CITY_HEADER__

#include "BM_TypeDef.h"
#include "Network/protocol/PacketDefine.h"

namespace BLOCKMAN
{
	class EntityPlayer;

	enum SyncBlockCityType {
		CITY_NONE = -1,
		CITY_DEFAULT,
		CITY_MANOR,
		CITY_PLAYER_INFO,
		CITY_STORE,
		CITY_MANORINFO,
		CITY_BUYLACKITEMS,
		CITY_HOUSE_SHOP,
		CITY_MANOR_AREA,
		CITY_ELEVATOR_AREA,
		CITY_WEEK_REWARD,
		CITY_ARCHIVE,
	};

	enum ShopClassifyId {
		Wings = 4,
		Gun = 5,
		Car = 6,
		Flower = 7,
	};

	class BlockCityPlayerInfo 
	{
	public:
		i64 score = 0;
		bool isEditMode = false;
		bool isCanFlying = false;
		i32 flyingRemainTime = 0;
		bool isGotFreeFly = true;
		i32 lastWeekRank = 0;
		DEFINE_NETWORK_SERIALIZER(score, isEditMode, isCanFlying, flyingRemainTime, isGotFreeFly, lastWeekRank);
		BlockCityPlayerInfo() {};
	};

	class BlockCityManor
	{
	public:
		i32 level = 0;
		String area = "";
		i32 score = 0;
		DEFINE_NETWORK_SERIALIZER(level, area, score);
		BlockCityManor() {};
	};

	class BlockCityFlyingInfo 
	{
	public:
		i32 id = 0;
		i32 day = 0;
		i32 moneyType = 0;
		i32 price = 0;
		String flyMsg = "";
		DEFINE_NETWORK_SERIALIZER(id, day, moneyType, price, flyMsg);
		BlockCityFlyingInfo() {};
	};

	class BlockCityManorInfo
	{
	public:
		Vector3 startPos = Vector3::ZERO;
		Vector3 endPos = Vector3::ZERO;
		Vector3 canPlaceStartPos = Vector3::ZERO;
		Vector3 canPlaceEndPos = Vector3::ZERO;
		std::vector<BlockCityFlyingInfo> flyingInfo;
		//i32 flyMoneyType = 0;
		//i32 flyPrice = 0;
		//String flyMsg = "";
		DEFINE_NETWORK_SERIALIZER(startPos, endPos, canPlaceStartPos, canPlaceEndPos, flyingInfo);// , flyMoneyType, flyPrice, flyMsg);
		BlockCityManorInfo() {};
	};

	class BlockCityManorManager
	{
	public:
		i32 blockScore = 0;
		i32 decorateScore = 0;
		i32 allScore = 0;
		i32 openStatus = 1;
		i32 upgradeMoney = 0;
		i32 upgradeMoneyType = 0;
		i32 deleteMoney = 0;
		i32 deleteMoneyType = 0;
		i32 maxLevel = 0;
		BlockCityManor curManor;
		BlockCityManor nextManor;
		DEFINE_NETWORK_SERIALIZER(blockScore, decorateScore, allScore, openStatus, upgradeMoney, upgradeMoneyType, deleteMoney, deleteMoneyType, maxLevel, curManor, nextManor);
		BlockCityManorManager() {};
	};

	class BlockCityPayment
	{
	public:
		i32 num = 0;
		i32 moneyType = 0;
		i32 price = 0;
		i32 tabId = 0;
		i32 itemId = 0;
		i32 index = 0;
		DEFINE_NETWORK_SERIALIZER(num, moneyType, price, tabId, itemId, index);
		BlockCityPayment() {};
	};

	class BlockCityMaterial
	{
	public:
		i32 id = 0;
		i32 itemId = 0;
		i32 itemMate = -1;
		i32 needNum = 0;
		i32 haveNum = 0;
		DEFINE_NETWORK_SERIALIZER(id, itemId, itemMate, needNum, haveNum);
		BlockCityMaterial() {};
	};

	class BlockCityStoreItem
	{
	public:
		i32 id = 0;
		i32 groupId = 0;
		i32 itemId = 0;
		i32 itemMate = -1;
		i32 num = 0;
		i32 limit = 0;
		String volume = "";
		i32 score = 0;
		i32 onePriceCube = 0;
		i32 onePrice = 0;
		i32 groupPriceCube = 0;
		i32 groupPrice = 0;
		i32 weight = 0;
		i32 isNew = 0;
		i32 isHot = 0;
		i32 moneyType = 0;
		std::vector<BlockCityMaterial> materials;

		DEFINE_NETWORK_SERIALIZER(id, groupId, itemId, itemMate, num, limit, volume, score, onePriceCube, onePrice, groupPriceCube, groupPrice, materials, weight, isNew, isHot, moneyType);
		BlockCityStoreItem() {};
	};

	class BlockCityStoreGroup
	{
	public:
		i32 groupId = 0;
		String groupName = "";
		String groupIcon = "";
		i32 isNew = 0;

		DEFINE_NETWORK_SERIALIZER(groupId, groupName, groupIcon, isNew);
		BlockCityStoreGroup() {};
	};

	class BlockCityStoreTab
	{
	public:
		i32 tabId = 0;
		std::vector<BlockCityStoreItem> items;
		std::vector<BlockCityStoreGroup> groups;

		DEFINE_NETWORK_SERIALIZER(tabId, items, groups);
		BlockCityStoreTab() {};
	};

	class BlockCityLackItem
	{
	public:
		i32 id = 0;
		i32 itemId = 0;
		i32 itemMeta = -1;
		i32 haveNum = 0;
		i32 needNum = 0;

		DEFINE_NETWORK_SERIALIZER(id, itemId, itemMeta, haveNum, needNum);
		BlockCityLackItem() {};
	};

	class BlockCityBuyLackItems 
	{
	public:
		std::vector<BlockCityLackItem> items;
		i32 price = 0;
		i32 priceCube = 0;
		i32 type = 0;
		DEFINE_NETWORK_SERIALIZER(items, price, priceCube, type);
		BlockCityBuyLackItems() {};
	};

	class BlockCityHouseShopItem
	{
	public:
		i32 itemId = 0;
		i32 itemStatus = 0;
		String itemActorName = "";
		float itemScale = 1.0f;
		i32 isCanBuy = -1;
		i32 moneyType = 0;
		i32 limit = -1;
		i32 haveNum = -1;
		i32 limitTime = -1;
		DEFINE_NETWORK_SERIALIZER(itemId, itemStatus, itemActorName, itemScale, isCanBuy, moneyType, limit, haveNum, limitTime);
		BlockCityHouseShopItem() {};
	};

	class BlockCityHouseShop
	{
	public:
		std::vector<BlockCityHouseShopItem> items;
		std::vector<i32> areaIds;
		DEFINE_NETWORK_SERIALIZER(items, areaIds);
		BlockCityHouseShop() {};
	};

	class BlockCityHouseShopClassify
	{
	public:
		i32 shopClassifyId = 0;
		String shopClassifyName = "";
		String shopClassifyImage = "";
		std::vector<BlockCityHouseShop> shops;
		DEFINE_NETWORK_SERIALIZER(shopClassifyId, shopClassifyName, shopClassifyImage, shops);
		BlockCityHouseShopClassify() {};
	};

	class BlockCityHouseShopItemInfo
	{
	public:
		i32 shopClassifyId = 0;
		String shopClassifyIdName = "";
		std::vector<BlockCityHouseShopItem> items;
		BlockCityHouseShopItemInfo() {};
	};

	class BlockCityManorArea
	{
	public:
		ui64 userId = 0;
		Vector3 startPos = Vector3::ZERO;
		Vector3 endPos = Vector3::ZERO;
		DEFINE_NETWORK_SERIALIZER(userId, startPos, endPos);
		BlockCityManorArea() {};
	};

	class BlockCityElevatorArea
	{
	public:
		i32 id = 0;
		i32 floor = 0;
		Vector3 startPos = Vector3::ZERO;
		Vector3 endPos = Vector3::ZERO;
		String floorIcon = "";
		DEFINE_NETWORK_SERIALIZER(id, floor, startPos, endPos, floorIcon);
		BlockCityElevatorArea() {};
	};

	class BlockCityWeekRewardItem
	{
	public:
		i32 id = -1;
		i32 meta = -1;
		DEFINE_NETWORK_SERIALIZER(id, meta);
		BlockCityWeekRewardItem() {};
	};

	class BlockCityWeekReward
	{
	public:
		i32 rank = 0;
		std::vector<BlockCityWeekRewardItem> items;
		DEFINE_NETWORK_SERIALIZER(rank, items);
		BlockCityWeekReward() {};
	};

	class BlockCityArchiveItem
	{
	public:
		String name = "";
		String updateAt = "";
		DEFINE_NETWORK_SERIALIZER(name, updateAt);
		BlockCityArchiveItem() {};
	};

	class BlockCityArchive
	{
	public:
		std::vector<BlockCityArchiveItem> items;
		i32 nextPrice = 0;
		i32 maxNum = 0;
		DEFINE_NETWORK_SERIALIZER(items, nextPrice, maxNum);
		BlockCityArchive() {};
	};

	class BlockCity : public ObjectAlloc
	{
	public:
		BlockCity();
		~BlockCity();

	private:
		BlockCityManorManager	*m_manorManager = nullptr;
		BlockCityPlayerInfo		*m_playerInfo = nullptr;
		BlockCityManorInfo		*m_manorInfo = nullptr;
		BlockCityBuyLackItems	*m_lackItems = nullptr;
		BlockCityArchive        *m_archive = nullptr;
		vector<BlockCityStoreTab>::type m_store;
		vector<BlockCityHouseShopClassify>::type m_houseShops;
		vector<BlockCityManorArea>::type m_manorArea;
		vector<BlockCityElevatorArea>::type m_elevatorArea;
		vector<BlockCityWeekReward>::type m_weekRewards;

	private:
		bool m_isManorManagerChange = false;
		bool m_isPlayerInfoChange = false;
		bool m_isStoreChange = false;
		bool m_isManorInfoChange = false;
		bool m_isBuyLackItemsChange = false;
		bool m_isHouseShopChange = false;
		bool m_isManorAreaChange = false;
		bool m_isElevatorAreaChange = false;
		bool m_isWeekRewardChange = false;
		bool m_isArchiveChange = false;

	public:
		void onUpdate(EntityPlayer* player);

		void setManorManager(BlockCityManorManager* manorInfo);
		void setPlayerInfo(BlockCityPlayerInfo* playerInfo);
		void setManorInfo(BlockCityManorInfo *manorInfo);
		void setBuyLackItems(BlockCityBuyLackItems *lackItems);
		void setArchive(BlockCityArchive *archive);

		void setStoresFromLua(std::vector<BlockCityStoreTab> stores);
		void setStoreTab(const BlockCityStoreTab& tab);
		void setStore(const vector<BlockCityStoreTab>::type& tabs);
		void updateItems(const vector<BlockCityStoreItem>::type& items);
		
		void setHouseShop(const vector<BlockCityHouseShopClassify>::type& shops);
		void setHouseShopFromLua(std::vector<BlockCityHouseShopClassify> shops);
		void setPackTab(const BlockCityHouseShopClassify& tab);
		void setManorArea(const vector<BlockCityManorArea>::type &manorAreas);
		void setManorAreaFromLua(std::vector<BlockCityManorArea> manorAreas);
		void setElevatorArea(const vector<BlockCityElevatorArea>::type &elevatorAreas);
		void setElevatorAreaFromLua(std::vector<BlockCityElevatorArea> elevatorAreas);
		void setWeekReward(const vector<BlockCityWeekReward>::type &rewards);
		void setWeekRewardFromLua(std::vector<BlockCityWeekReward> rewards);

		BlockCityManorManager* getManorManager() { return m_manorManager; }
		BlockCityPlayerInfo* getPlayerInfo() { return m_playerInfo; }
		BlockCityManorInfo *getManorInfo() { return m_manorInfo; }
		BlockCityBuyLackItems *getBuyLackItems() { return m_lackItems; }
		BlockCityArchive *getArchive() { return m_archive; }

		const vector<BlockCityStoreTab>::type& getStore() { return m_store; }
		i32 getItemNum(i32 itemId, i32 meta);

		i64 getScore() { return m_playerInfo ? m_playerInfo->score : 0; }

		const vector<BlockCityHouseShopClassify>::type& getHouseShop() { return m_houseShops; }

		i32 getTabIdByItemId(i32 itemId);
		BlockCityHouseShopClassify getHouseShopByAreaId(i32 areaPushId);
		vector<BlockCityHouseShopItem>::type getShopItems(i32 shopClassifyId);

		const vector<BlockCityManorArea>::type &getManorArea() { return m_manorArea; }
		const vector<BlockCityElevatorArea>::type &getElevatorArea() { return m_elevatorArea; }
		const vector<BlockCityWeekReward>::type &getWeekReward() { return m_weekRewards; }
		BlockCityWeekReward getRewardByRank(i32 rank);
		i32 getRewardRank(i32 rank);
		
		i32 getElevatorFloorByPos(Vector3 pos);

		BlockCityHouseShopItemInfo getShopItemInfoByAreaId(i32 areaPushId);
		i32 getShopItemCurrencyTypeByLackItemId(i32 itemId);
		map<i32, String>::type getElevatorFloorIcons();
		i32 getShopItemNum(i32 itemId);

	private:
		bool getIsEditMode() { return m_playerInfo && m_playerInfo->isEditMode; }
		void changeItem(const BlockCityStoreItem& item);
		void updateItem();
		void dealloc();
	};
};

#endif