#include "AbandonItemSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	AbandonItemSetting::AbandonItemSetting()
	{

	}

	AbandonItemSetting::~AbandonItemSetting()
	{

	}

	bool AbandonItemSetting::getAbandonInfoFromMap(int itemId, int meta, AbandonItemInfo & sellItemInfo)
	{
		for (auto iter : m_abandonItemInfoMap)
		{
			if (iter.second.itemId == itemId && iter.second.itemMeta == meta)
			{
				sellItemInfo = iter.second;
				return true;
			}
		}

		return false;
	}

	bool AbandonItemSetting::loadSetting(bool isClient)
	{
		CsvReader* pReader = new CsvReader();

		std::string path;
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "AbandonItem.csv").c_str();
		}
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "AbandonItem.csv").c_str();
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
			AbandonItemInfo abandonItemInfo;
			pReader->GetInt("Id", abandonItemInfo.id);
			pReader->GetInt("ItemId", abandonItemInfo.itemId);
			pReader->GetInt("ItemMeta", abandonItemInfo.itemMeta);
			int canAbandon = 0;
			pReader->GetInt("ItemMeta", canAbandon);
			abandonItemInfo.canAbandon = canAbandon == 0 ? false : true;
			pReader->GetString("ItemName", abandonItemInfo.itemName);
			pReader->GetString("Desc", abandonItemInfo.itemDesc);
			m_abandonItemInfoMap[abandonItemInfo.id] = abandonItemInfo;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void AbandonItemSetting::unloadSetting()
	{
		m_abandonItemInfoMap.clear();
	}
}