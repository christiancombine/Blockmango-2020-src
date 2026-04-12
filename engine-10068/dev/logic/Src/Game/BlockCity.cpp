#include "BlockCity.h"
#include "Common.h"
#include "Entity/EntityPlayer.h"
#include "Network/CommonPacketSender.h"
#include "World/World.h"

namespace BLOCKMAN
{
	BlockCity::BlockCity()
	{

	}

	BlockCity::~BlockCity()
	{
		dealloc();
	}

	void BlockCity::onUpdate(EntityPlayer* player)
	{

		if (m_isManorManagerChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_MANOR);
			m_isManorManagerChange = false;
		}

		if (m_isPlayerInfoChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_PLAYER_INFO);
			m_isPlayerInfoChange = false;
		}

		if (m_isStoreChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_STORE);
			m_isStoreChange = false;
		}

		if (m_isManorInfoChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_MANORINFO);
			m_isManorInfoChange = false;
		}

		if (m_isBuyLackItemsChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_BUYLACKITEMS);
			m_isBuyLackItemsChange = false;
		}

		if (m_isHouseShopChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_HOUSE_SHOP);
			m_isHouseShopChange = false;
		}

		if (m_isManorAreaChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_MANOR_AREA);
			m_isManorAreaChange = false;
		}

		if (m_isElevatorAreaChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_ELEVATOR_AREA);
			m_isElevatorAreaChange = false;
		}

		if (m_isWeekRewardChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_WEEK_REWARD);
			m_isWeekRewardChange = false;
		}

		if (m_isArchiveChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockCity(player, SyncBlockCityType::CITY_ARCHIVE);
			m_isArchiveChange = false;
		}
	}

	void BlockCity::setManorManager(BlockCityManorManager* manorInfo)
	{
		if (!m_manorManager)
		{
			m_manorManager = new BlockCityManorManager();
		}
		m_manorManager->blockScore = manorInfo->blockScore;
		m_manorManager->decorateScore = manorInfo->decorateScore;
		m_manorManager->allScore = manorInfo->allScore;
		m_manorManager->openStatus = manorInfo->openStatus;
		m_manorManager->upgradeMoney = manorInfo->upgradeMoney;
		m_manorManager->upgradeMoneyType = manorInfo->upgradeMoneyType;
		m_manorManager->deleteMoney = manorInfo->deleteMoney;
		m_manorManager->deleteMoneyType = manorInfo->deleteMoneyType;
		m_manorManager->curManor = manorInfo->curManor;
		m_manorManager->nextManor = manorInfo->nextManor;
		m_manorManager->maxLevel = manorInfo->maxLevel;
		m_isManorManagerChange = true;
	}

	void BlockCity::setPlayerInfo(BlockCityPlayerInfo* playerInfo)
	{
		if (!m_playerInfo)
		{
			m_playerInfo = new BlockCityPlayerInfo();
		}
		m_playerInfo->score = playerInfo->score;
		m_playerInfo->isEditMode = playerInfo->isEditMode;
		m_playerInfo->isCanFlying = playerInfo->isCanFlying;
		m_playerInfo->flyingRemainTime = playerInfo->flyingRemainTime;
		m_playerInfo->isGotFreeFly = playerInfo->isGotFreeFly;
		m_playerInfo->lastWeekRank = playerInfo->lastWeekRank;
		m_isPlayerInfoChange = true;
	}

	void BlockCity::setManorInfo(BlockCityManorInfo * manorInfo)
	{
		if (!m_manorInfo)
		{
			m_manorInfo = new BlockCityManorInfo();
		}
		m_manorInfo->startPos = manorInfo->startPos;
		m_manorInfo->endPos = manorInfo->endPos;
		m_manorInfo->canPlaceStartPos = manorInfo->canPlaceStartPos;
		m_manorInfo->canPlaceEndPos = manorInfo->canPlaceEndPos;
		m_manorInfo->flyingInfo = manorInfo->flyingInfo;
		//m_manorInfo->flyMoneyType = manorInfo->flyMoneyType;
		//m_manorInfo->flyPrice = manorInfo->flyPrice;
		//m_manorInfo->flyMsg = manorInfo->flyMsg;
		m_isManorInfoChange = true;
	}

	void BlockCity::setBuyLackItems(BlockCityBuyLackItems * lackItems)
	{
		if (!m_lackItems)
		{
			m_lackItems = new BlockCityBuyLackItems();
		}
		m_lackItems->type = lackItems->type;
		m_lackItems->items = lackItems->items;
		m_lackItems->price = lackItems->price;
		m_lackItems->priceCube = lackItems->priceCube;
		m_isBuyLackItemsChange = true;
	}

	void BlockCity::setArchive(BlockCityArchive *archive)
	{
		if (!m_archive)
		{
			m_archive = new BlockCityArchive();
		}
		m_archive->items = archive->items;
		m_archive->nextPrice = archive->nextPrice;
		m_archive->maxNum = archive->maxNum;
		m_isArchiveChange = true;
	}

	void BlockCity::setStoresFromLua(std::vector<BlockCityStoreTab> stores)
	{
		m_store.clear();
		for (auto store : stores)
		{
			m_store.push_back(store);
		}
		m_isStoreChange = true;
	}

	void BlockCity::setStoreTab(const BlockCityStoreTab& tab)
	{
		for (auto& item : m_store)
		{
			if (item.tabId == tab.tabId)
			{
				item = tab;
				return;
			}
		}
		m_store.push_back(tab);
	}

	void BlockCity::setStore(const vector<BlockCityStoreTab>::type& tabs)
	{
		m_store.clear();
		for (auto tab : tabs)
		{
			sort(tab.items.begin(), tab.items.end(), [](const BlockCityStoreItem &item1, const BlockCityStoreItem &item2) {
				return item1.weight < item2.weight;
			});

			m_store.push_back(tab);
		}
		updateItem();
	}

	void BlockCity::updateItems(const vector<BlockCityStoreItem>::type& items)
	{
		for (const auto& item : items)
		{
			changeItem(item);
		}
		updateItem();
	}

	i32 BlockCity::getItemNum(i32 itemId, i32 meta)
	{
		for (const auto& tab : m_store)
		{
			for (const auto& item : tab.items)
			{
				i32 itemMeta = item.itemMate > 0 ? item.itemMate : 0;
				if (itemId == item.itemId && itemMeta == meta)
				{
					return item.num;
				}
			}
		}
		return 0;
	}

	i32 BlockCity::getShopItemNum(i32 itemId)
	{
		for (const auto &mainShop : m_houseShops)
		{
			for (const auto &branch : mainShop.shops)
			{
				for (const auto &item : branch.items)
				{
					if (itemId == item.itemId)
					{
						return item.haveNum;
					}
				}
			}
		}
		return 0;
	}

	BlockCityWeekReward BlockCity::getRewardByRank(i32 rank)
	{
		i32 rewardRank = getRewardRank(rank);
		if (rewardRank == 0)
		{
			return BlockCityWeekReward();
		}

		for (auto reward : m_weekRewards)
		{
			if (reward.rank == rewardRank)
			{
				return reward;
			}
		}

		return BlockCityWeekReward();
	}

	i32 BlockCity::getRewardRank(i32 rank)
	{
		if (rank > 0 && rank <= 3)
		{
			return rank;
		}
		else if (rank >= 4 && rank <= 10)
		{
			return 4;
		}
		else if (rank >= 11 && rank <= 30)
		{
			return 5;
		}
		else
		{
			return 0;
		}
	}
			

	i32 BlockCity::getElevatorFloorByPos(Vector3 position)
	{
		for (const auto &area : m_elevatorArea)
		{
			if (position.x > area.startPos.x && position.y > area.startPos.y && position.z > area.startPos.z &&
				position.x < area.endPos.x && position.y < area.endPos.y && position.z < area.endPos.z)
			{
				return area.floor;
			}
		}

		return 0;
	}

	void BlockCity::changeItem(const BlockCityStoreItem & storeItem)
	{
		for (auto& tab : m_store)
		{
			for (auto& item : tab.items)
			{
				if (storeItem.itemId == item.itemId && storeItem.itemMate == item.itemMate)
				{
					item.num = storeItem.num;
					return;
				}
			}
		}
	}

	void BlockCity::updateItem()
	{
		for (auto& tab : m_store)
		{
			if (tab.tabId == 1)
			{
				for (auto& item : tab.items)
				{
					int i = 0;
					item.num = 0;
					for (auto & material : item.materials)
					{
						material.haveNum = getItemNum(material.itemId, material.itemMate);
						i32 number = material.haveNum / material.needNum;
						if (i == 0 || number < item.num)
						{
							item.num = number;
						}
						i++;
					}
				}
			}
		}
	}

	void BlockCity::setHouseShopFromLua(std::vector<BlockCityHouseShopClassify> shops)
	{
		m_houseShops.clear();
		for (const auto &shop : shops)
		{
			m_houseShops.push_back(shop);
		}
		m_isHouseShopChange = true;
	}

	void BlockCity::setHouseShop(const vector<BlockCityHouseShopClassify>::type& shops)
	{
		m_houseShops.clear();
		for (const auto &shop : shops)
		{
			m_houseShops.push_back(shop);
		}
	}

	BlockCityHouseShopClassify BlockCity::getHouseShopByAreaId(i32 areaPushId)
	{
		for (const auto &shopClassify : m_houseShops)
		{
			for (const auto &areaShop : shopClassify.shops)
			{
				for (const auto &areaId : areaShop.areaIds)
				{
					if (areaId == areaPushId)
					{
						return shopClassify;
					}
				}
			}
		}
		return BlockCityHouseShopClassify();
	}

	vector<BlockCityHouseShopItem>::type BlockCity::getShopItems(i32 shopClassifyId)
	{
		vector<BlockCityHouseShopItem>::type items;
		for (const auto &shopClassify : m_houseShops)
		{
			if (shopClassifyId == shopClassify.shopClassifyId)
			{
				for (const auto &shop : shopClassify.shops)
				{
					for (const auto &item : shop.items)
					{
						items.push_back(item);
					}
				}
			}
		}
		return items;
	}

	i32 BlockCity::getTabIdByItemId(i32 itemId)
	{
		for (const auto &shopClassify : m_houseShops)
		{
			for (const auto &areaShop : shopClassify.shops)
			{
				for (const auto &item : areaShop.items)
				{
					if (itemId == item.itemId)
					{
						return shopClassify.shopClassifyId;
					}
				}
			}
		}
 		return 0;
	}

	void BlockCity::setPackTab(const BlockCityHouseShopClassify& tab)
	{
		for (auto& item : m_houseShops)
		{
			if (item.shopClassifyId == tab.shopClassifyId)
			{
				item = tab;
				return;
			}
		}
		m_houseShops.push_back(tab);
	}

	void BlockCity::setManorAreaFromLua(std::vector<BlockCityManorArea> manorAreas)
	{
		m_manorArea.clear();
		for (const auto &area : manorAreas)
		{
			m_manorArea.push_back(area);
		}
		m_isManorAreaChange = true;
	}

	void BlockCity::setManorArea(const vector<BlockCityManorArea>::type &manorAreas)
	{
		m_manorArea.clear();
		for (const auto &area : manorAreas)
		{
			m_manorArea.push_back(area);
		}
	}

	void BlockCity::setElevatorAreaFromLua(std::vector<BlockCityElevatorArea> elevatorAreas)
	{
		m_elevatorArea.clear();
		for (const auto &area : elevatorAreas)
		{
			m_elevatorArea.push_back(area);
		}
		m_isElevatorAreaChange = true;
	}

	void BlockCity::setElevatorArea(const vector<BlockCityElevatorArea>::type & elevatorAreas)
	{
		m_elevatorArea.clear();
		for (const auto &area : elevatorAreas)
		{
			m_elevatorArea.push_back(area);
		}
	}

	void BlockCity::setWeekReward(const vector<BlockCityWeekReward>::type &rewards)
	{
		m_weekRewards.clear();
		for (const auto &reward : rewards)
		{
			m_weekRewards.push_back(reward);
		}
	}

	void BlockCity::setWeekRewardFromLua(std::vector<BlockCityWeekReward> rewards)
	{
		m_weekRewards.clear();
		for (const auto &reward : rewards)
		{
			m_weekRewards.push_back(reward);
		}
		m_isWeekRewardChange = true;
	}

	BlockCityHouseShopItemInfo BlockCity::getShopItemInfoByAreaId(i32 areaPushId)
	{
		BlockCityHouseShopItemInfo itemInfos;
		for (const auto &mainShop : m_houseShops)
		{
			for (const auto &branch : mainShop.shops)
			{
				for (const auto &areaId : branch.areaIds)
				{
					if (areaId == areaPushId)
					{
						itemInfos.items = branch.items;
						itemInfos.shopClassifyId = mainShop.shopClassifyId;
						itemInfos.shopClassifyIdName = mainShop.shopClassifyName;
						return itemInfos;
					}
				}
			}
		}
		return itemInfos;
	}

	i32 BlockCity::getShopItemCurrencyTypeByLackItemId(i32 itemId)
	{
		for (const auto &store : m_store)
		{
			for (const auto &item : store.items)
			{
				if (item.id == itemId)
				{
					return item.moneyType;
				}
			}
		}
		return 0;
	}

	map<i32, String>::type BlockCity::getElevatorFloorIcons()
	{
		map<i32, String>::type icons;
		for (const auto &elevator : m_elevatorArea)
		{
			icons.insert({ elevator.floor, elevator.floorIcon });
		}
		return icons;
	}

	void BlockCity::dealloc()
	{
		delete m_manorManager;
		delete m_playerInfo;
		delete m_manorInfo;
		delete m_lackItems;
		delete m_archive;
		m_manorManager = nullptr;
		m_playerInfo = nullptr;
		m_manorInfo = nullptr;
		m_lackItems = nullptr;
		m_archive = nullptr;
	}

};
