#include "StoreTabSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	StoreTabSetting::StoreTabSetting()
	{
		loadSetting();
	}

	StoreTabSetting::~StoreTabSetting()
	{
		unloadSetting();
	}

	bool StoreTabSetting::loadSetting()
	{
		StoreTab* pStoreTab = NULL;
		m_itemMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/storeTab.csv").c_str();
		
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}

		pReader->Read(); // discard the second line
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pStoreTab = new StoreTab();
			pReader->GetInt("Id", pStoreTab->id);
			pReader->GetString("Icon", pStoreTab->icon);
			pReader->GetString("Name", pStoreTab->name);
			m_itemMap[pStoreTab->id] = pStoreTab;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void  StoreTabSetting::unloadSetting()
	{
		for (auto iter = m_itemMap.begin(); iter != m_itemMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}
		m_itemMap.clear();
	}

	StoreTab * StoreTabSetting::findItem(i32 id)
	{
		auto item = m_itemMap.find(id);
		if (id != 0 && item != m_itemMap.end())
		{
			return item->second;
		}
		return nullptr;
	}
}
