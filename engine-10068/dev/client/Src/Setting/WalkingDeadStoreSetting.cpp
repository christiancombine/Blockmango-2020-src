
#include "WalkingDeadStoreSetting.h"
#include "RewardSetting.h"
#include "Object/Root.h"
#include "Util/CSVReader.h"
#include "rapidjson/document.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GUIWalkingDeadMain.h"
namespace BLOCKMAN
{
	int WalkingDeadStoreSetting::m_vip = 0;
	int64_t WalkingDeadStoreSetting::m_vip_time = 0;

	WalkingDeadStoreType WalkingDeadStoreSetting::m_cur_store_type = GeneralShop;
	map<int, WalkingDeadStore*>::type WalkingDeadStoreSetting::m_store_map = {};
	map<int, WalkingDeadStoreGoods*>::type WalkingDeadStoreSetting::m_goods_map = {};
	list<WalkingDeadStoreTab*>::type		WalkingDeadStoreSetting::m_store_tab_list = {};
	list<WalkingDeadStoreGoods*>::type WalkingDeadStoreSetting::m_traveller_goods_list = {};
	map<int, WalkingDeadSupply*>::type WalkingDeadStoreSetting::m_supply_map = {};
	void WalkingDeadStoreSetting::loadSetting()
	{
		loadStoreSetting();
		loadStoreGoodsSetting();
		loadStoreTabSetting();
		loadSupplySetting();
	}

	void WalkingDeadStoreSetting::unloadSetting()
	{
		for (auto iter : m_store_map)
		{
			delete iter.second;
		}
		m_store_map.clear();
		for (auto iter : m_goods_map)
		{
			delete iter.second;
		}
		m_goods_map.clear();
		for (auto tab : m_store_tab_list)
		{
			delete tab;
		}
		m_store_tab_list.clear();
		m_traveller_goods_list.clear();
	}

	void WalkingDeadStoreSetting::onUpdate(ui32 nTimeElapse)
	{
		if (m_vip_time > 0)
		{
			m_vip_time -= nTimeElapse;
			if (m_vip_time < 0)
			{
				m_vip_time = 0;
				m_vip = 0;
				for (auto iter : m_goods_map)
				{
					iter.second->Change = true;
				}
			}
		}

		for (auto iter : m_supply_map)
		{
			auto supply = iter.second;
			if (supply->RefreshTime > 0)
			{
				supply->RefreshTime -= nTimeElapse;
				if (supply->RefreshTime <= 0)
				{
					supply->RefreshTime = 0;
					supply->Status = CanReceive;
					supply->Change = true;
				}
			}
		}
	}

	void WalkingDeadStoreSetting::changeStoreType(WalkingDeadStoreType storeType)
	{
		m_cur_store_type = storeType;
		if (m_cur_store_type == GeneralShop)
		{
			for (auto goods : m_traveller_goods_list)
			{
				goods->Limit = -1;
			}
			m_traveller_goods_list.clear();
		}
	}

	void WalkingDeadStoreSetting::loadStoreSetting()
	{
		WalkingDeadStore* pStore = NULL;
		auto pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(),
			Root::Instance()->getWorldName(),"Setting","Store.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pStore = new WalkingDeadStore();
			pReader->GetInt("StoreId", pStore->StoreId);
			pReader->GetString("TabIds", pStore->TabIds);
			m_store_map.insert({ pStore->StoreId , pStore });
		}
		LordSafeDelete(pReader);
	}

	void WalkingDeadStoreSetting::loadStoreTabSetting()
	{
		WalkingDeadStoreTab* pStoreTab = NULL;
		auto pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), 
			Root::Instance()->getWorldName(), "Setting","StoreTab.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pStoreTab = new WalkingDeadStoreTab();
			pReader->GetInt("TabId", pStoreTab->TabId);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pStoreTab->Type = (WalkingDeadStoreTabType)Type;
			pReader->GetString("Icon", pStoreTab->Icon);
			pReader->GetString("Name", pStoreTab->Name);
			m_store_tab_list.push_back(pStoreTab);
		}
		LordSafeDelete(pReader);
	
	}

	void WalkingDeadStoreSetting::loadStoreGoodsSetting()
	{
		WalkingDeadStoreGoods* pStoreGoods = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), 
			Root::Instance()->getWorldName(), "Setting", "StoreGoods.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pStoreGoods = new WalkingDeadStoreGoods();
			pReader->GetInt("Index", pStoreGoods->Index);
			pReader->GetInt("GoodsId", pStoreGoods->GoodsId);
			pReader->GetInt("TabId", pStoreGoods->TabId);
			pReader->GetInt("RewardId", pStoreGoods->RewardId);
			pReader->GetString("Image", pStoreGoods->Image);
			pReader->GetString("Desc", pStoreGoods->Desc);
			pReader->GetString("Name", pStoreGoods->Name);
			pReader->GetInt("Num", pStoreGoods->Num);
			pReader->GetInt("Price", pStoreGoods->Price);
			pReader->GetInt("PriceA", pStoreGoods->PriceA);
			pReader->GetInt("PriceB", pStoreGoods->PriceB);
			pReader->GetInt("PriceC", pStoreGoods->PriceC);
			pReader->GetInt("MoneyType", pStoreGoods->MoneyType);
			m_goods_map.insert({ pStoreGoods->GoodsId , pStoreGoods });
		}
		LordSafeDelete(pReader);
	}

	void WalkingDeadStoreSetting::loadSupplySetting()
	{
		WalkingDeadSupply* pSupply = NULL;
		auto pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(),
			Root::Instance()->getWorldName(), "setting", "SupplyChest.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pSupply = new WalkingDeadSupply();
			pReader->GetInt("Id", pSupply->Id);
			pReader->GetInt("Price", pSupply->Price);
			pReader->GetInt("PriceA", pSupply->PriceA);
			pReader->GetInt("PriceB", pSupply->PriceB);
			pReader->GetInt("PriceC", pSupply->PriceC);

			int Type = 0;
			pReader->GetInt("Type", Type);
			pSupply->Type = (WalkingDeadSupplyType)Type;
			pReader->GetString("Describe", pSupply->Describe);
			pReader->GetString("Name", pSupply->Name);
			pReader->GetString("Image", pSupply->Image);
			pReader->GetInt("MoneyType", pSupply->MoneyType);
			pReader->GetString("RewardIds", pSupply->RewardIds);
			m_supply_map.insert({ pSupply->Id, pSupply });
		}
		LordSafeDelete(pReader);
	}

	WalkingDeadStoreTab * WalkingDeadStoreSetting::findTabByTabId(int tabId)
	{
		for (auto tab : m_store_tab_list)
		{
			if (tab->TabId == tabId)
			{
				return tab;
			}
		}
		return nullptr;
	}

	list<WalkingDeadStoreTab*>::type WalkingDeadStoreSetting::findTabsByStoreId(int storeId)
	{
		list<WalkingDeadStoreTab*>::type result;
		auto iStore = m_store_map.find(storeId);
		if (iStore != m_store_map.end())
		{
			WalkingDeadStore* store = iStore->second;
			StringArray tabIds = StringUtil::Split(store->TabIds, "#");
			for (auto tabId : tabIds)
			{
				auto tab = findTabByTabId(StringUtil::ParseI32(tabId));
				if (tab != nullptr)
				{
					result.push_back(tab);
				}
			}
		}
		return result;
	}

	list<WalkingDeadStoreTab*>::type WalkingDeadStoreSetting::findTabsFromTraveller()
	{
		list<WalkingDeadStoreTab*>::type result;
		map<int, int>::type tabIds;
		for (auto goods : m_traveller_goods_list)
		{
			if (tabIds.find(goods->TabId) != tabIds.end())
			{
				continue;
			}
			tabIds.insert({ goods->TabId, goods->TabId });
		}
		for (auto tabId : tabIds)
		{
			auto tab = findTabByTabId(tabId.second);
			if (tab != nullptr)
			{
				result.push_back(tab);
			}
		}
		return result;
	}

	WalkingDeadStoreGoods* WalkingDeadStoreSetting::findGoodsById(int id)
	{
		auto iter = m_goods_map.find(id);
		if (iter != m_goods_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	list<WalkingDeadStoreGoods*>::type WalkingDeadStoreSetting::findGoodsListByTabId(int tabId)
	{
		list<WalkingDeadStoreGoods*>::type result;
		if (m_cur_store_type == GeneralShop|| m_cur_store_type == MerchantShop)
		{
			for (auto iter : m_goods_map)
			{
				if (iter.second->TabId == tabId)
				{
					result.push_back(iter.second);
				}
			}
		} 
		else 
		{
			for (auto goods : m_traveller_goods_list)
			{
				if (goods->TabId == tabId)
				{
					result.push_back(goods);
				}
			}
			result = sortGoods(result);
		}
		return result;
	}

	WalkingDeadStoreGoods* WalkingDeadStoreSetting::findFirstGoodsByTabId(int tabId)
	{
		if (m_cur_store_type == GeneralShop || m_cur_store_type == MerchantShop)
		{
			for (auto iter : m_goods_map)
			{
				if (iter.second->TabId == tabId)
				{
					return iter.second;
				}
			}
		}
		else
		{
			for (auto goods : m_traveller_goods_list)
			{
				if (goods->TabId == tabId)
				{
					return goods;
				}
			}
		}
		return nullptr;
	}

	WalkingDeadSupply * WalkingDeadStoreSetting::findSupplyById(int id)
	{
		auto iter = m_supply_map.find(id);
		if (iter != m_supply_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	WalkingDeadSupply * WalkingDeadStoreSetting::findSupplyByType(WalkingDeadSupplyType type)
	{
		for (auto supply : m_supply_map)
		{
			if (supply.second->Type == type)
			{
				return supply.second;
			}
		}
		return nullptr;
	}

	void WalkingDeadStoreSetting::parseTravellerGoodsData(const String & data)
	{
		m_traveller_goods_list.clear();
		if (data.length() == 0)
		{
			return ;
		}
		auto doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordSafeDelete(doc);
			return ;
		}
		if (!doc->IsArray())
		{
			LordSafeDelete(doc);
			return ;
		}
		auto items = doc->GetArray();
		for (size_t i= 0; i<items.Size();++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("goodsId"))
			{
				continue;
			}
			int goodsId = obj.FindMember("goodsId")->value.GetInt();
			int limit = obj.FindMember("limit")->value.GetInt();
		// find goods from m_goods_map into m_traveller_goods_list
			for (auto iter : m_goods_map)
			{
				if (iter.second->GoodsId == goodsId)
				{
					m_traveller_goods_list.push_back(iter.second);
				}
			}
			// set Limit goods.Limit
			for (auto goods : m_traveller_goods_list)
			{
				goods->Limit = limit;
			}
		}
	}


	void WalkingDeadStoreSetting::parseWalkingDeadStoreVipData(int vip, int64_t vipTime)
	{
		m_vip = vip;
		m_vip_time = vipTime;
		m_vip_time = m_vip_time * 1000;
		for (auto iter : m_goods_map)
		{
			iter.second->Change = true;
		}
	}

	void WalkingDeadStoreSetting::parseUpdateWalkingDeadSupplyData(const String & data)
	{
		if (data.length() == 0)
		{
			return;
		}
		auto doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordSafeDelete(doc);
			return;
		}
		if (!doc->IsArray())
		{
			LordSafeDelete(doc);
			return;
		}
		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("Id"))
			{
				continue;
			}
			int Id = obj.FindMember("Id")->value.GetInt();
			auto supply = findSupplyById(Id);
			if (!supply)
			{
				continue;
			}
			supply->RefreshTime = obj.FindMember("Time")->value.GetInt64() * 1000;
			supply->Status = (WalkingDeadSupplyStatus)obj.FindMember("Status")->value.GetInt();
			supply->Change = true;
		}
	}

	bool WalkingDeadStoreSetting::sortGoodsFunc(WalkingDeadStoreGoods * goods1, WalkingDeadStoreGoods * goods2)
	{
		if (goods1->Index != goods2->Index)
		{
			return (int)goods1->Index < (int)goods2->Index;
		}
		return goods1->Index > goods1->Index;
		return false;
	}

	list<WalkingDeadStoreGoods*>::type WalkingDeadStoreSetting::sortGoods(list<WalkingDeadStoreGoods*>::type goods)
	{
		goods.sort(sortGoodsFunc);
		return goods;
	}

	void WalkingDeadStoreSetting::updateGoods(int goodsId, int limit)
	{
		// TODO find goods	
		for (auto goods : m_traveller_goods_list)
		{
			if (goods->GoodsId == goodsId)
			{
				goods->Limit = limit;
				goods->Change = true;
				break;
			}
		}
	}

}