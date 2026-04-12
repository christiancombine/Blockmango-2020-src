#include "SkyBlockAppShopSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Util/UserManager.h"
#include "rapidjson/document.h"
#include "SkyBlockPrivilegeShopSetting.h"

namespace BLOCKMAN
{

	SkyBlockAppShopSetting::SkyBlockAppShopSetting()
	{

	}

	SkyBlockAppShopSetting::~SkyBlockAppShopSetting()
	{

	}


	bool SkyBlockAppShopSetting::loadSetting()
	{

		loadShopTabSetting();
		loadShopItemSetting();
		return true;
	}

	bool SkyBlockAppShopSetting::loadShopItemSetting()
	{
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "AppShopSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}

		pReader->Read(); // discard the second line
		pReader->Read();
		while (pReader->Read())
		{
			SkyBlockGoods pSetting;
			pReader->GetInt("Id", pSetting.id);
			pReader->GetInt("TabId", pSetting.tabId);
			pReader->GetInt("ItemId", pSetting.itemId);
			pReader->GetString("HaveItem", pSetting.haveItem);
			pReader->GetInt("Meta", pSetting.meta);
			pReader->GetInt("CoinId", pSetting.coinId);
			pReader->GetInt("GoodType", pSetting.goodType);
			pReader->GetInt("GroupId", pSetting.groupId);
			pReader->GetString("Icon", pSetting.icon);
			if (pSetting.icon == "#")
				pSetting.icon = "";
			pReader->GetString("Name", pSetting.name);
			pReader->GetString("Detail", pSetting.detail);
			pReader->GetString("Weight", pSetting.weight);
			pReader->GetString("CoinImg", pSetting.coinImg);
			pReader->GetString("GroupText", pSetting.groupText);
			pReader->GetString("LockTip", pSetting.lockTip);
			pReader->GetString("DisCountTips", pSetting.disCountTips);
			
			pReader->GetInt("Level", pSetting.level);
			pReader->GetInt("Time", pSetting.time);
			pReader->GetInt("Hot", pSetting.hot);
			pReader->GetInt("CoinNum1", pSetting.coinNum1);
			pReader->GetInt("CoinNum2", pSetting.coinNum2);
			pSetting.coinNum = pSetting.coinNum1;
			int foreverId;
			pReader->GetInt("Forever", foreverId);
			if (foreverId == 1)
				pSetting.forever = true;
			else
				pSetting.forever = false;

			int priBlock;
			pReader->GetInt("PriBlock", priBlock);
			if (priBlock == 1)
				pSetting.isPriBlock = true;
			else
				pSetting.isPriBlock = false;

			int PriNpc;
			pReader->GetInt("PriNpc", PriNpc);
			if (PriNpc == 1)
				pSetting.isPriNpc = true;
			else
				pSetting.isPriNpc = false;

			pSetting.weights = StringUtil::Split(pSetting.weight, "#");

			for (auto &group : m_goodGrops)
			{
				if (group.m_id == pSetting.tabId)
				{
					group.m_goods.push_back(pSetting);
					break;
				}
			}
		}

		LordSafeDelete(pReader);
		return true;
	}

	bool SkyBlockAppShopSetting::loadShopTabSetting()
	{
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "AppShopTabSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}
		m_goodGrops.clear();
		pReader->Read(); // discard the second line
		pReader->Read();
		while (pReader->Read())
		{
			SkyBlockGoodGroup pSetting;
			pReader->GetInt("Id", pSetting.m_id);
			pReader->GetString("Icon", pSetting.m_icon);
			pReader->GetString("Name", pSetting.m_name);

			m_goodGrops.push_back(pSetting);
		}
		LordSafeDelete(pReader);
		return true;
	}

	void SkyBlockAppShopSetting::unloadSetting()
	{
		for (auto &group : m_goodGrops)
		{
			group.m_goods.clear();
		}

		m_goodGrops.clear();
	}

	bool SkyBlockAppShopSetting::getShopItem(vector<SkyBlockGoodGroup>::type& goods)
	{

		for (auto group : m_goodGrops)
		{
			if (group.m_id == SKYBLOCK_TAB_OLD_PRI)
				continue;

			SkyBlockGoodGroup innerGroup;
			innerGroup.m_id = group.m_id;
			innerGroup.m_icon = group.m_icon;
			innerGroup.m_name = group.m_name;

			for (auto cur_good : group.m_goods)
			{
				innerGroup.m_goods.push_back(cur_good);
			}
			goods.push_back(innerGroup);
		}

		if (goods.empty())
			return false;
		return true;
	}

	bool SkyBlockAppShopSetting::getShopItemByGroupId(int tabId,int groupId, SkyBlockGoodGroup& goods)
	{
		for (auto group : m_goodGrops)
		{
			if (group.m_id == tabId)
			{
				goods.m_id = group.m_id;
				goods.m_icon = group.m_icon;
				goods.m_name = group.m_name;

				for (auto cur_good : group.m_goods)
				{
					if (cur_good.groupId == groupId)
					{
						goods.m_goods.push_back(cur_good);
					}
				}
				return true;
			}
		}
		return false;
	}


	bool SkyBlockAppShopSetting::getPrivilegeShopItemByTabId(int tabId, SkyBlockGoodGroup& goods)
	{
		for (auto group : m_goodGrops)
		{
			if (group.m_id == tabId)
			{
				goods.m_id = group.m_id;
				goods.m_icon = group.m_icon;
				goods.m_name = group.m_name;

				for (auto cur_good : group.m_goods)
				{
					goods.m_goods.push_back(cur_good);
				}
				return true;
			}
		}
		return false;
	}

	void SkyBlockAppShopSetting::parseUpdateShopData(int SortType, int level, vector<SkyBlockGoods>::type &goods, const String& priData)
	{
		old_sortType = m_sortType;
		old_level = m_level;
		m_sortType = SortType;
		m_level = level;
		parseUpdateLevelAndSort(goods);
		parseUpdateData(priData, goods);
	}

	bool SkyBlockAppShopSetting::isHavePrivilege(int priId)
	{
		for (auto pri : m_priData)
		{
			if (pri.second.m_type == priId && pri.second.m_have)
			{
				return true;
			}
		}
		return false;
	}


	bool SkyBlockAppShopSetting::setBlockPri(SkyBlockGoods& good)
	{
		auto isPush = false;
		if (isHavePrivilege(SKYBLOCK_BLOCK_USE_MONEY) && good.coinId == SKYBLOCK_DIAMOND)
		{
			good.haveUseMoney = true;
			good.coinId = SKYBLOCK_MONEY;
			good.coinNum = good.coinNum2;
			isPush = true;
		}
		else if (!isHavePrivilege(SKYBLOCK_BLOCK_USE_MONEY) && good.haveUseMoney)
		{
			good.haveUseMoney = false;
			good.coinId = SKYBLOCK_DIAMOND;
			good.coinNum = good.coinNum1;
			isPush = true;
		}

		if (isHavePrivilege(SKYBLOCK_PRI_OF_BLOCK))
		{
			if (good.isPriBlock && good.level <= m_level && good.status == SKYBLOCK_GOOD_LOCK)
			{
				good.status = SKYBLOCK_GOOD_UN_LOCK;
				isPush = true;
			}
		}
		else if (!isHavePrivilege(SKYBLOCK_PRI_OF_BLOCK) && good.isPriBlock)
		{
			good.status = SKYBLOCK_GOOD_LOCK;
			isPush = true;
		}

		if (isHavePrivilege(SKYBLOCK_BUY_BLOCK_HALF) && !good.haveHalfPrice)
		{
			good.haveHalfPrice = true;
			good.coinNum = good.coinNum / 2;
			isPush = true;
		}
		else if (!isHavePrivilege(SKYBLOCK_BUY_BLOCK_HALF) && good.haveHalfPrice)
		{
			good.haveHalfPrice = false;
			good.coinNum = good.coinNum * 2;
			isPush = true;
		}

		return isPush;
	}
	bool SkyBlockAppShopSetting::setNpcPri(SkyBlockGoods& good)
	{
		bool isPush = false;

		if (isHavePrivilege(SKYBLOCK_PRI_OF_NPC))
		{
			if (good.isPriNpc && good.level <= m_level && good.status == SKYBLOCK_GOOD_LOCK)
			{
				good.status = SKYBLOCK_GOOD_UN_LOCK;
				isPush = true;
			}
		}
		else if (!isHavePrivilege(SKYBLOCK_PRI_OF_NPC) && good.isPriNpc)
		{
			good.status = SKYBLOCK_GOOD_LOCK;
			isPush = true;
		}
		return isPush;
	}

	void SkyBlockAppShopSetting::setPrivilegeData(vector<SkyBlockGoods>::type& goods)
	{
		for (auto &group : m_goodGrops)
		{
			for (auto &good : group.m_goods)
			{
				bool isPush = false;
				if (good.goodType == SKYBLOCK_BLOCK)
				{
					isPush = setBlockPri(good);
				}

				if (good.goodType == SKYBLOCK_NPC_UNLOCK)
				{
					isPush = setNpcPri(good);
				}

				if (isPush)
				{
					goods.push_back(good);
				}
			}
		}
	}

	void SkyBlockAppShopSetting::parseUpdateLevelAndSort(vector<SkyBlockGoods>::type& goods)
	{
		for (auto &group : m_goodGrops)
		{
			if (old_level != m_level)
			{
				for (auto &good : group.m_goods)
				{
					if (isGoodLock(good) || isGoodUnLock(good))
					{
						goods.push_back(good);
					}

				}
			}

			if (old_sortType != m_sortType)
			{
				if (group.m_goods.size() >= 2) {
					sort(group.m_goods.begin(), group.m_goods.end(), SortShopItemByType());
				}
			}
		}
	}


	bool SkyBlockAppShopSetting::isGoodUnLock(SkyBlockGoods& good)
	{
		if (good.status != SKYBLOCK_GOOD_UN_LOCK && good.level <= m_level)
		{
			if (good.isPriBlock)
			{
				if (isHavePrivilege(SKYBLOCK_PRI_OF_BLOCK))
				{
					good.status = SKYBLOCK_GOOD_UN_LOCK;
					return true;
				}
			}
			else if (good.isPriNpc)
			{
				if (isHavePrivilege(SKYBLOCK_PRI_OF_NPC))
				{
					good.status = SKYBLOCK_GOOD_UN_LOCK;
					return true;
				}
			}
			else
			{
				good.status = SKYBLOCK_GOOD_UN_LOCK;
				return true;
			}
		}
		return false;
	}

	bool SkyBlockAppShopSetting::isGoodLock(SkyBlockGoods& good)
	{
		if (good.status == SKYBLOCK_GOOD_UN_LOCK)
		{
			if (good.level > m_level)
			{
				good.status = SKYBLOCK_GOOD_LOCK;
				return true;
			}

			if (good.isPriBlock)
			{
				if (!isHavePrivilege(SKYBLOCK_PRI_OF_NPC))
				{
					good.status = SKYBLOCK_GOOD_LOCK;
					return true;
				}
			}
			else if (good.isPriNpc)
			{
				if (!isHavePrivilege(SKYBLOCK_PRI_OF_NPC))
				{
					good.status = SKYBLOCK_GOOD_LOCK;
					return true;
				}
			}
		}
		return false;
	}

	void SkyBlockAppShopSetting::parseUpdateData(const String& priData, vector<SkyBlockGoods>::type& goods)
	{
		if (priData.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(priData.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return ;
		}
		if (!doc->HasMember("priData") || !doc->HasMember("npcData") || !doc->HasMember("priItemData"))
		{
			LordLogError("SkyBlockAppShopSetting::parseUpdateData getItemInfo The game result content player_info missed some field");
			LordDelete(doc);
			return;
		}

		rapidjson::Value priArr = doc->FindMember("priData")->value.GetArray();
		rapidjson::Value npcArr = doc->FindMember("npcData")->value.GetArray();
		rapidjson::Value priItemData = doc->FindMember("priItemData")->value.GetArray();

		for (size_t i = 0; i < priArr.Size(); ++i)
		{
			if (!priArr[i].HasMember("id") || !priArr[i].HasMember("time") || !priArr[i].HasMember("forever") || !priArr[i].HasMember("itemId"))
			{
				LordLogError("SkyBlockAppShopSetting::getItemInfo The game result content player_info missed some field. %d", i);
				break;
			}

			int id = priArr[i].FindMember("id")->value.GetInt();
			int time = priArr[i].FindMember("time")->value.GetInt();
			int itemId = priArr[i].FindMember("itemId")->value.GetInt();
			bool forever = priArr[i].FindMember("forever")->value.GetBool();
			bool isfind = false;

			for (auto &group : m_goodGrops)
			{
				for (auto &good : group.m_goods)
				{
					if (good.id == id)
					{
						good.curTime = time;
						good.isHave = false;
						good.forever = forever;
						if(good.curTime > 0 || good.forever) 
							good.isHave = true;
						goods.push_back(good);
						isfind = true;
						itemId = good.itemId;
					}

					if (good.itemId == itemId && forever)
					{
						good.haveForever = true;
					}
				}

				if (isfind)
					break;
			}
		}

		for (size_t i = 0; i < npcArr.Size(); ++i)
		{
			if (!npcArr[i].HasMember("id") || !npcArr[i].HasMember("time") || !npcArr[i].HasMember("forever") || !npcArr[i].HasMember("itemId"))
			{
				LordLogError("SkyBlockAppShopSetting::getItemInfo The game result content player_info missed some field. %d", i);
				break;
			}

			int id = npcArr[i].FindMember("id")->value.GetInt();
			int time = npcArr[i].FindMember("time")->value.GetInt();
			int itemId = npcArr[i].FindMember("itemId")->value.GetInt();
			bool forever = npcArr[i].FindMember("forever")->value.GetBool();
			bool isfind = false;
	
			for (auto &group : m_goodGrops)
			{
				for (auto &good : group.m_goods)
				{
					if (good.id == id)
					{
						good.curTime = time;
						good.isHave = false;
						good.forever = forever;
						if (good.curTime > 0 || good.forever)
							good.isHave = true;
						goods.push_back(good);
						isfind = true;
					}

					if (good.itemId == itemId && forever)
					{
						good.haveForever = true;
					}
				}

				if (isfind)
					break;
			}
		}

		if (priItemData.Size() > 0)
		{
			m_priData.clear();
			for (size_t i = 0; i < priItemData.Size(); ++i)
			{
				if (!priItemData[i].HasMember("id") || !priItemData[i].HasMember("have") || !priItemData[i].HasMember("detail") || !priItemData[i].HasMember("priType"))
				{
					LordLogError("SkyBlockAppShopSetting::getItemInfo The game result content player_info missed some field. %d", i);
					break;
				}

				int id = priItemData[i].FindMember("id")->value.GetInt();
				int priType = priItemData[i].FindMember("priType")->value.GetInt();
				String detail = priItemData[i].FindMember("detail")->value.GetString();
				bool have = priItemData[i].FindMember("have")->value.GetBool();

				SkyBlockPriviLege cur_pri;
				cur_pri.m_id = id;
				cur_pri.m_type = priType;
				cur_pri.m_have = have;
				cur_pri.detail = detail;
				m_priData[cur_pri.m_id] = cur_pri;
			}

			setPrivilegeData(goods);
		}


		LordDelete(doc);
		return;
	}

	bool SkyBlockAppShopSetting::getPrivilegeRewardDataByPriId(SkyBlockPrivilegeReward& priInfo, int priId)
	{
		for (auto group : m_goodGrops)
		{
			for (auto cur_good : group.m_goods)
			{
				if (cur_good.id == priId)
				{
					priInfo.id = cur_good.id;
					priInfo.icon = cur_good.icon;
					priInfo.name = cur_good.name;
					priInfo.detail = cur_good.detail;
					return true;
				}
			}
		}
		return false;
	}

	bool SkyBlockAppShopSetting::getItemById(SkyBlockGoods& goods, int id)
	{
		for (auto group : m_goodGrops)
		{
			for (auto cur_good : group.m_goods)
			{
				if (cur_good.id == id)
				{
					goods = cur_good;
					return true;
				}
			}
		}
		return false;

	}
}