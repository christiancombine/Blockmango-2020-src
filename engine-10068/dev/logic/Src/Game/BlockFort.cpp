#include "BlockFort.h"
#include "Common.h"
#include "Entity/EntityPlayer.h"
#include "Network/CommonPacketSender.h"
#include "World/World.h"

namespace BLOCKMAN
{
	BlockFort::BlockFort()
	{

	}

	BlockFort::~BlockFort()
	{
		dealloc();
	}

	void BlockFort::onUpdate(EntityPlayer* player)
	{
		if (m_isManorInfoChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockFort(player, SyncBlockFortType::FORT_MANORINFO);
			m_isManorInfoChange = false;
		}

		if (m_isManorManagerChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockFort(player, SyncBlockFortType::FORT_MANORMANAGER);
			m_isManorManagerChange = false;
		}

		if (m_isPlayerInfoChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockFort(player, SyncBlockFortType::FORT_PLAYERINFO);
			m_isPlayerInfoChange = false;
		}

		if (m_isStoreChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBlockFort(player, SyncBlockFortType::FORT_STORE);
			m_isStoreChange = false;
		}

	}

	void BlockFort::setManorInfo(BlockFortManorInfo * manorInfo)
	{
		if (!m_manorInfo)
		{
			m_manorInfo = new BlockFortManorInfo();
		}

		m_manorInfo->startPos = manorInfo->startPos;
		m_manorInfo->endPos = manorInfo->endPos;
		m_manorInfo->canPlaceStartPos = manorInfo->canPlaceStartPos;
		m_manorInfo->canPlaceEndPos = manorInfo->canPlaceEndPos;
		m_isManorInfoChange = true;
	}


	void BlockFort::setManorManager(BlockFortManorManager *manorManager)
	{
		if (!m_manorManger)
		{
			m_manorManger = new BlockFortManorManager();
		}
		
		m_manorManger->curManor = manorManager->curManor;
		m_manorManger->nextManor = manorManager->nextManor;
		m_manorManger->maxLevel = manorManager->maxLevel;
		m_manorManger->upgradeMoneyType = manorManager->upgradeMoneyType;
		m_manorManger->upgradeMoney = manorManager->upgradeMoney;
		m_manorManger->unlockIds = manorManager->unlockIds;
		m_isManorManagerChange = true;
	}

	void BlockFort::setPlayerInfo(BlockFortPlayerInfo *playerInfo)
	{
		if (!m_playerInfo)
		{
			m_playerInfo = new BlockFortPlayerInfo();
		}

		m_playerInfo->exp = playerInfo->exp;
		m_playerInfo->level = playerInfo->level;
		m_playerInfo->totalExp = playerInfo->totalExp;
		m_isPlayerInfoChange = true;
	}

	void BlockFort::setStoresFromLua(std::vector<BlockFortStoreTab> stores)
	{
		m_store.clear();
		for (auto store : stores)
		{
			m_store.push_back(store);
		}
		m_isStoreChange = true;
	}

	void BlockFort::setStores(const vector<BlockFortStoreTab>::type& stores)
	{
		m_store.clear();
		for (auto store : stores)
		{
			m_store.push_back(store);
		}
	}

	BlockFortStoreTab BlockFort::getStoreTab(i32 tabId)
	{
		for (const auto& tab : m_store)
		{
			if (tab.tabId == tabId )
			{
				return tab;
			}
		}
		return BlockFortStoreTab();
	}
	
	void BlockFort::updateItems(const BlockFortStoreItem& items)
	{
		changeItem(items);
	}

	void BlockFort::changeItem(const BlockFortStoreItem & storeItem)
	{
		for (auto& tab : m_store)
		{
			for (auto& item : tab.items)
			{
				if (storeItem.itemId == item.itemId && storeItem.itemMate == item.itemMate)
				{
					item.itemNum = storeItem.itemNum;
					return;
				}
			}
		}
	}

	void BlockFort::dealloc()
	{
		delete m_manorInfo;
		delete m_manorManger;
		delete m_playerInfo;

		m_manorInfo = nullptr;
		m_manorManger = nullptr;
		m_playerInfo = nullptr;
	}

};