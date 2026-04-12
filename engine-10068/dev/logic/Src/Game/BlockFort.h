#pragma once
#ifndef __BLOCK_FORT_HEADER__
#define __BLOCK_FORT_HEADER__

#include "BM_TypeDef.h"
#include "Network/protocol/PacketDefine.h"

namespace BLOCKMAN
{
	class EntityPlayer;

	enum SyncBlockFortType {
		FORT_NONE = -1,
		FORT_DEFAULT = 0,
		FORT_MANORINFO,
		FORT_MANORMANAGER,
		FORT_PLAYERINFO,
		FORT_STORE,
	};
	
	class BlockFortStoreItem
	{
	public:
		i32 id = 0;
		i32 itemId = 0;
		i32 itemMate = 0;
		i32 limit = 0;
		i32 groupId = 0;
		i32 itemNum = 0;
		i32 weight = 0;
		i32 moneyType = 1;
		DEFINE_NETWORK_SERIALIZER(id, itemId, itemMate, limit, groupId, itemNum, weight, moneyType);
		BlockFortStoreItem() {};
	};

	class BlockFortStoreGroup
	{
	public:
		i32 groupId = 0;
		String groupName = "";
		String groupIcon = "";

		DEFINE_NETWORK_SERIALIZER(groupId, groupName, groupIcon);
		BlockFortStoreGroup() {};
	};

	class BlockFortStoreTab
	{
	public:
		i32 tabId = 0;
		std::vector<BlockFortStoreItem> items;
		std::vector<BlockFortStoreGroup> groups;

		DEFINE_NETWORK_SERIALIZER(tabId, items, groups);
		BlockFortStoreTab() {};
	};

	class BlockFortManorInfo
	{
	public:
		Vector3 startPos = Vector3::ZERO;
		Vector3 endPos = Vector3::ZERO;
		Vector3 canPlaceStartPos = Vector3::ZERO;
		Vector3 canPlaceEndPos = Vector3::ZERO;
		DEFINE_NETWORK_SERIALIZER(startPos, endPos, canPlaceStartPos, canPlaceEndPos);
		BlockFortManorInfo() {};
	};

	class BlockFortManor
	{
	public:
		i32 level = 0;
		String area = "";
		DEFINE_NETWORK_SERIALIZER(level, area);
		BlockFortManor() {};
	};

	class BlockFortManorManager
	{
	public:
		i32 upgradeMoney = 0;
		i32 upgradeMoneyType = 0;
		i32 maxLevel = 0;
		BlockFortManor curManor;
		BlockFortManor nextManor;
		std::vector<i32> unlockIds;
		DEFINE_NETWORK_SERIALIZER(upgradeMoney, upgradeMoneyType, maxLevel, curManor, nextManor, unlockIds);
		BlockFortManorManager() {};
	};

	class BlockFortPlayerInfo
	{
	public:
		i32 exp = 0;
		i32 level = 0;
		i32 totalExp = 0;
		DEFINE_NETWORK_SERIALIZER(exp, level, totalExp);
		BlockFortPlayerInfo() {};
	};

	class BlockFort : public ObjectAlloc
	{
	public:
		BlockFort();
		~BlockFort();

	public:
		void onUpdate(EntityPlayer* player);

		void setManorInfo(BlockFortManorInfo *manorInfo);
		void setManorManager(BlockFortManorManager *manorManager);
		void setPlayerInfo(BlockFortPlayerInfo *playerInfo);
		void setStoresFromLua(std::vector<BlockFortStoreTab> stores);

		BlockFortManorInfo *getManorInfo() { return m_manorInfo; }
		BlockFortManorManager *getManorManager() { return m_manorManger; }
		BlockFortPlayerInfo *getPlayerInfo() { return m_playerInfo; }

		void setStores(const vector<BlockFortStoreTab>::type& stores);
		void updateItems(const BlockFortStoreItem& items);
		const vector<BlockFortStoreTab>::type& getStore() { return m_store; }
		BlockFortStoreTab getStoreTab(i32 tabId);

	private:
		void dealloc();

	private:
		BlockFortManorInfo *m_manorInfo = nullptr;
		BlockFortManorManager *m_manorManger = nullptr;
		BlockFortPlayerInfo *m_playerInfo = nullptr;
		vector<BlockFortStoreTab>::type m_store;

	private:
		bool m_isManorInfoChange = false;
		bool m_isManorManagerChange = false;
		bool m_isPlayerInfoChange = false;
		bool m_isStoreChange = false;

	private:
		void changeItem(const BlockFortStoreItem& item);
	};
};

#endif