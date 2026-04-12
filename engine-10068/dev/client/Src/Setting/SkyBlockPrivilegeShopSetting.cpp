#include "SkyBlockPrivilegeShopSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Util/UserManager.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	SkyBlockPrivilegeShopSetting::SkyBlockPrivilegeShopSetting()
	{

	}

	SkyBlockPrivilegeShopSetting::~SkyBlockPrivilegeShopSetting()
	{

	}

	bool SkyBlockPrivilegeShopSetting::loadSetting()
	{

		loadShopTabSetting();
		loadShopItemSetting();
		loadRewardItemSetting();
		return true;
	}

	bool SkyBlockPrivilegeShopSetting::loadShopItemSetting()
	{
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "PrivilegeShopSetting.csv").c_str();
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
			SkyBlockPrivilegeGoods pSetting;
			pReader->GetInt("Id", pSetting.id);
			pReader->GetInt("TabId", pSetting.tabId);
			pReader->GetInt("ItemId", pSetting.itemId);
			pReader->GetString("HaveItem", pSetting.haveItem);
			pReader->GetString("Icon", pSetting.icon);
			if (pSetting.icon == "#")
				pSetting.icon = "";
			pReader->GetString("Name", pSetting.name);
			pReader->GetString("Detail", pSetting.detail);
			pReader->GetString("Rewards", pSetting.rewards);
			pReader->GetInt("CoinNum", pSetting.coinNum);
			pReader->GetInt("CoinId", pSetting.coinId);

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

	bool SkyBlockPrivilegeShopSetting::loadShopTabSetting()
	{
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "PrivilegeShopTabSetting.csv").c_str();
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
			SkyBlockPrivilegeGoodGroup pSetting;
			pReader->GetInt("Id", pSetting.m_id);
			pReader->GetString("Name", pSetting.m_name);

			m_goodGrops.push_back(pSetting);
		}
		LordSafeDelete(pReader);
		return true;
	}

	bool SkyBlockPrivilegeShopSetting::loadRewardItemSetting()
	{
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "PrivilegeRewardSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}
		m_reward.clear();
		pReader->Read(); // discard the second line
		pReader->Read();
		while (pReader->Read())
		{
			SkyBlockPrivilegeReward pSetting;
			pReader->GetInt("Id", pSetting.id);
			pReader->GetString("Icon", pSetting.icon);
			pReader->GetString("Name", pSetting.name);
			pReader->GetString("Detail", pSetting.detail);

			m_reward.push_back(pSetting);
		}
		LordSafeDelete(pReader);
		return true;
	}

	void SkyBlockPrivilegeShopSetting::unloadSetting()
	{
		for (auto &group : m_goodGrops)
		{
			group.m_goods.clear();
		}

		m_goodGrops.clear();
		m_reward.clear();
		m_priImg.clear();
	}

	bool SkyBlockPrivilegeShopSetting::getShopItem(vector<SkyBlockPrivilegeGoodGroup>::type& goods)
	{

		for (auto group : m_goodGrops)
		{
			SkyBlockPrivilegeGoodGroup innerGroup;
			innerGroup.m_id = group.m_id;
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

	bool SkyBlockPrivilegeShopSetting::getShopItemByTabId(vector<SkyBlockPrivilegeGoods>::type& goods, int tabId)
	{
		for (auto group : m_goodGrops)
		{
			if (group.m_id == tabId)
			{
				for (auto cur_good : group.m_goods)
				{
					goods.push_back(cur_good);
				}
			}
		}

		if (goods.empty())
			return false;
		return true;
	}

	bool SkyBlockPrivilegeShopSetting::getPrivilegeRewardItem(vector<SkyBlockPrivilegeReward>::type& goods, String rewards)
	{
		StringArray rewardList = StringUtil::Split(rewards, "#");
		for (size_t i = 0; i < rewardList.size(); i++)
		{
			int rewardId = StringUtil::ParseInt(rewardList[i].c_str());
			for (auto reward : m_reward)
			{
				if (rewardId == reward.id)
					goods.push_back(reward);
			}
		}

		if (goods.empty())
			return false;
		return true;
	}

	bool SkyBlockPrivilegeShopSetting::getPrivilegeRewardDataByPriId(SkyBlockPrivilegeReward& priInfo, int priId)
	{
		for (auto group : m_goodGrops)
		{
			for (auto good : group.m_goods)
			{
				if (good.itemId == priId)
				{
					priInfo.id = good.itemId;
					priInfo.icon = good.icon;
					priInfo.name = good.name;
					priInfo.detail = good.detail;
					return true;
				}
			}
		}
		return false;
	}

	void SkyBlockPrivilegeShopSetting::parseUpdateShopData(vector<SkyBlockPrivilegeGoods>::type &goods, const String& priData)
	{
		parseUpdateData(priData, goods);
	}

	void SkyBlockPrivilegeShopSetting::parseUpdateData(const String& priData, vector<SkyBlockPrivilegeGoods>::type& goods)
	{
		if (priData.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(priData.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}
		if (!doc->HasMember("priData"))
		{
			LordLogError("SkyBlockPrivilegeShopSetting::parseUpdateData getItemInfo The game result content player_info missed some field");
			LordDelete(doc);
			return;
		}

		rapidjson::Value priArr = doc->FindMember("priData")->value.GetArray();

		for (size_t i = 0; i < priArr.Size(); ++i)
		{
			if (!priArr[i].HasMember("id") || !priArr[i].HasMember("time") || !priArr[i].HasMember("forever") || !priArr[i].HasMember("itemId"))
			{
				LordLogError("SkyBlockPrivilegeShopSetting::getItemInfo The game result content player_info missed some field. %d", i);
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
					if (good.itemId == id)
					{
						//good.curTime = time;
						good.isHave = false;
						good.forever = forever;
						if (good.forever)
							good.isHave = true;
						goods.push_back(good);
						isfind = true;
					}
				}

				if (isfind)
					break;
			}
		}
		LordDelete(doc);
		return;
	}

	void SkyBlockPrivilegeShopSetting::parseUpdatePriImgData(const String& priData)
	{
		if (priData.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(priData.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}
		if (!doc->HasMember("priList"))
		{
			LordLogError("SkyBlockPrivilegeShopSetting::parseUpdatePriImgData getItemInfo The game result content player_info missed some field");
			LordDelete(doc);
			return;
		}

		rapidjson::Value priImg = doc->FindMember("priList")->value.GetArray();
		m_priImg.clear();

		for (size_t i = 0; i < priImg.Size(); ++i)
		{
			int id = priImg[i].GetInt();
			m_priImg.push_back(id);
		}

		LordDelete(doc);
		return;
	}

	bool SkyBlockPrivilegeShopSetting::isHavePriImg(int id)
	{
		for (auto curId : m_priImg)
		{
			if (curId == id)
				return true;
		}
		return false;
	}
}