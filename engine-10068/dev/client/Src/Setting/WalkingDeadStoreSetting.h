#pragma once
#ifndef _WALKINGDEAD_STORE_SETTING_HEADER_
#define _WALKINGDEAD_STORE_SETTING_HEADER_

#include "Core.h"
#include "Inventory/Wallet.h"
#include "Util/ClientEvents.h"
using namespace LORD;

namespace BLOCKMAN
{
	enum WalkingDeadStoreType
	{
		GeneralShop = 1,
		TravellerShop = 2,
		MerchantShop = 3
	};

	enum WalkingDeadStoreTabType
	{
		GeneralTabType = 1,
		SupplyChestType = 2,
	};

	class WalkingDeadStore
	{
	public:
		int StoreId = 0;
		String TabIds = "";
	};

	class WalkingDeadStoreTab
	{
	public:
		int TabId = 0;
		WalkingDeadStoreTabType Type = GeneralTabType;
		String Icon = "";
		String Name = "";
	};

	class WalkingDeadStoreGoods
	{
	public:
		int Index = 0;
		int GoodsId = 0;
		int TabId = 0;
		int RewardId = 0;
		String Image = "";
		String Desc = "";
		String Name = "";
		int Num = 0;
		int Price = 0;
		int PriceA = 0;
		int PriceB = 0;
		int PriceC = 0;
		int MoneyType = 0;

		int Limit = -1;
		bool Change = false;
	};

	enum WalkingDeadSupplyType
	{
		TiroBox = 1,
		DailyBox = 2,
		RandomBox = 3,
		VipBox = 4,
	};

	enum WalkingDeadSupplyStatus
	{
		CanReceive = 1,
		UnReceive = 2,
		CanBuy = 3,
		UnBuy = 4
	};

	class WalkingDeadSupply
	{
	public:
		int Id = 0;
		WalkingDeadSupplyType Type = DailyBox;
		String Describe = "";
		String Name = "";
		String Image = "";
		int Price = 0;
		int PriceA = 0;
		int PriceB = 0;
		int PriceC = 0;
		WalkingDeadSupplyStatus Status = CanBuy;
		int MoneyType = 0;
		String RewardIds = "";

		int64_t RefreshTime = 0;
		bool Change = false;
	};

	class WalkingDeadStoreSetting
	{
	private:
		static int m_vip;
		static int64_t m_vip_time;

		static map<int, WalkingDeadStore*>::type m_store_map;
		static map<int, WalkingDeadStoreGoods*>::type  m_goods_map;
		static list<WalkingDeadStoreTab*>::type m_store_tab_list;
		static map<int, WalkingDeadSupply*>::type m_supply_map;
		static WalkingDeadStoreType m_cur_store_type;
	public:
		static void loadSetting();	
		static void unloadSetting();

		static void onUpdate(ui32 nTimeElapse);

		static list<WalkingDeadStoreGoods*>::type m_traveller_goods_list;
		static void changeStoreType(WalkingDeadStoreType storeType);

		static WalkingDeadStoreTab* findTabByTabId(int tabId);
		static list<WalkingDeadStoreTab*>::type findTabsByStoreId(int storeId);
		static list<WalkingDeadStoreTab*>::type findTabsFromTraveller();

		static WalkingDeadStoreGoods* findGoodsById(int id);
		static list<WalkingDeadStoreGoods*>::type findGoodsListByTabId(int tabId);
		static WalkingDeadStoreGoods* findFirstGoodsByTabId(int tabId);

		static map<int, WalkingDeadSupply*>::type findAllSupplys() {return m_supply_map; };
		static WalkingDeadSupply* findSupplyById(int id);
		static WalkingDeadSupply* findSupplyByType(WalkingDeadSupplyType type);

		static void parseTravellerGoodsData(const String & data);
		static void parseWalkingDeadStoreVipData(int vip, int64_t vipTime);
		static void parseUpdateWalkingDeadSupplyData(const String & data);
		static bool sortGoodsFunc(WalkingDeadStoreGoods* goods1, WalkingDeadStoreGoods* goods2);
		static list<WalkingDeadStoreGoods*>::type sortGoods(list<WalkingDeadStoreGoods*>::type goods);

		static void updateGoods(int goodsId, int limit);
		static int getVipLevel() { return m_vip; }
		static int64_t getVipTime() { return m_vip_time; }
		static WalkingDeadStoreType getWalkingDeadStoreType() { return m_cur_store_type; }

	private:
		static void loadStoreSetting();
		static void loadStoreTabSetting();
		static void loadStoreGoodsSetting();
		static void loadSupplySetting();
	};
}

#endif