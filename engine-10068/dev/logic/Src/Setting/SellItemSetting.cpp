#include "SellItemSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	SellItemSetting::SellItemSetting()
	{

	}

	SellItemSetting::~SellItemSetting()
	{

	}

	bool SellItemSetting::getSellInfoFromMap(int itemId, int itemMeta, SellItemInfo & sellItemInfo)
	{
		for (auto iter : m_sellItemInfoMap)
		{
			if (iter.second.itemMeta == -1)
			{
				if (iter.second.itemId == itemId)
				{
					sellItemInfo = iter.second;
					return true;
				}
			}
			else
			{
				if (iter.second.itemId == itemId && iter.second.itemMeta == itemMeta)
				{
					sellItemInfo = iter.second;
					return true;
				}
			}
		}

		return false;
	}

	bool SellItemSetting::loadSetting(bool isClient)
	{
		m_sellItemInfoMap.clear();

		CsvReader* pReader = new CsvReader();

		std::string path;
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "SellItem.csv").c_str();
		}
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "SellItem.csv").c_str();
		}
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
			SellItemInfo sellItemInfo;
			pReader->GetInt("UniqueId", sellItemInfo.uniqueId);
			pReader->GetInt("ItemId", sellItemInfo.itemId);
			pReader->GetInt("ItemMeta", sellItemInfo.itemMeta);
			pReader->GetInt("Money", sellItemInfo.itemMoney);
			pReader->GetString("Desc", sellItemInfo.itemDesc);
			m_sellItemInfoMap[sellItemInfo.uniqueId] = sellItemInfo;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void SellItemSetting::unloadSetting()
	{
		m_sellItemInfoMap.clear();
	}
}