#include "SkyblockSignInSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Util/UserManager.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	SkyBlockSignInSetting::SkyBlockSignInSetting()
	{

	}

	SkyBlockSignInSetting::~SkyBlockSignInSetting()
	{

	}

	bool SkyBlockSignInSetting::loadSetting()
	{
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "SkyBlockSignInSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}
		m_goods.clear();
		pReader->Read(); // discard the second line
		pReader->Read();
		while (pReader->Read())
		{
			SkyBlockSignInInfo pSetting;
			pReader->GetInt("Id", pSetting.id);
			pReader->GetInt("Day", pSetting.day);
			pReader->GetString("Title", pSetting.title);
			pReader->GetString("Background", pSetting.background);
			pReader->GetString("Effect", pSetting.effect);
			pReader->GetString("Image", pSetting.image);
			pReader->GetString("RewardName", pSetting.rewardName);
			pReader->GetString("Desc", pSetting.desc);
			pReader->GetInt("Side", pSetting.side);
			pReader->GetInt("Num", pSetting.num);
			pReader->GetInt("RewardId", pSetting.rewardId);
			pReader->GetInt("Meta", pSetting.meta);
			pReader->GetInt("RewardType", pSetting.rewardType);

			m_goods.push_back(pSetting);
		}

		LordSafeDelete(pReader);
		return true;
	}

	void SkyBlockSignInSetting::unloadSetting()
	{
		m_goods.clear();
	}



	bool SkyBlockSignInSetting::getShowItems(vector<SkyBlockSignInInfo>::type& goods, int nowOrder, int status)
	{

		int UseRoom = 0;
		for (auto &cur_good : m_goods)
		{
			if (UseRoom + cur_good.side > 8)
			{
				break;
			}
			if (cur_good.day < nowOrder)
			{
				cur_good.status = SkyBlockSignInStatus::SKYBLOCK_HAVE_RECEIVED_SIGN;
			}
			else
			{
				if (cur_good.day == nowOrder)
				{
					cur_good.status = status;
				}
				else
				{
					cur_good.status = SkyBlockSignInStatus::SKYBLOCK_UN_RECEIVED_SIGN;
				}
			}
			if (UseRoom == 3 && cur_good.side == 2)
			{
				UseRoom++;
			}
			cur_good.useRoom = UseRoom;
			UseRoom = UseRoom + cur_good.side;

		}

		for (auto cur_good : m_goods)
		{
			goods.push_back(cur_good);
		}
		if (goods.empty())
			return false;
		return true;
	}

	bool SkyBlockSignInSetting::getSettingByOrder(SkyBlockSignInInfo& info, int goodId)
	{
		for (auto cur_good : m_goods)
		{
			if (goodId == cur_good.id)
			{
				info = cur_good;
				return true;
			}
		}
		return false;
	}
}

