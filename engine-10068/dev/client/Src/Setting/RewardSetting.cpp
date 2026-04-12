#include "RewardSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<int, Reward*>::type RewardSetting::m_reward_map = {};

	void RewardSetting::loadSetting()
	{
		unloadSetting();
		loadRewardSetting();
	}

	void RewardSetting::unloadSetting()
	{
		for (auto iter : m_reward_map)
		{
			delete iter.second;
		}
		m_reward_map.clear();
	}

	void RewardSetting::loadRewardSetting()
	{
		Reward* pPixelGunReward = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "RewardSetting.csv").c_str();
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
			pPixelGunReward = new Reward();
			pReader->GetInt("Id", pPixelGunReward->Id);
			pReader->GetString("Title", pPixelGunReward->Title);
			pReader->GetString("Name", pPixelGunReward->Name);
			pReader->GetString("Image", pPixelGunReward->Image);
			pReader->GetString("Desc", pPixelGunReward->Desc);
			pReader->GetInt("Count", pPixelGunReward->Count);
			int Type = 0;
			pReader->GetInt("RewardType", Type);
			pPixelGunReward->Type = (RewardType)Type;
			pReader->GetString("CountText", pPixelGunReward->CountText);
			m_reward_map[pPixelGunReward->Id] = pPixelGunReward;
		}
		
		LordSafeDelete(pReader);
	}

	Reward* RewardSetting::findRewardById(int id)
	{
		for (auto iter : m_reward_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	list<Reward*>::type RewardSetting::findRewardByIds(String rewardIds)
	{
		list<Reward*>::type result;
		StringArray ids = StringUtil::Split(rewardIds, "#");
		for (String id : ids)
		{
			if (id.empty())
			{
				continue;
			}
			Reward* reward = findRewardById(StringUtil::ParseI32(id));
			if (reward)
			{
				result.push_back(reward);
			}
		}
		return result;
	}
};