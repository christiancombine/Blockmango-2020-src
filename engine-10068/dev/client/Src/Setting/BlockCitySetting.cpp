#include "BlockCitySetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<i32, BlockCityItem*>  BlockCitySetting::m_blockCityItemMap = {};


	void  BlockCitySetting::unloadSetting()
	{
		for (auto iter = m_blockCityItemMap.begin(); iter != m_blockCityItemMap.end(); iter++)
		{
			delete iter->second;
			iter->second = nullptr;
		}
		m_blockCityItemMap.clear();
	}

	void  BlockCitySetting::loadSetting()
	{
		BlockCityItem* pBlockCityItem = NULL;
		m_blockCityItemMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/items.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}

		pReader->Read();
		while (pReader->Read())
		{
			pBlockCityItem = new BlockCityItem();
			pReader->GetInt("Id", pBlockCityItem-> id);
			pReader->GetInt("Score", pBlockCityItem->score);
			pReader->GetInt("Price", pBlockCityItem->price);
			pReader->GetInt("Cube", pBlockCityItem->cube);
			pReader->GetString("Name", pBlockCityItem->name);
			pReader->GetString("Desc", pBlockCityItem->desc);
			pReader->GetString("Icon", pBlockCityItem->icon);
			m_blockCityItemMap[pBlockCityItem->id] = pBlockCityItem;
		}
		
		LordSafeDelete(pReader);
	}

	BlockCityItem *  BlockCitySetting::getItem(i32  id)
	{
		auto item = m_blockCityItemMap.find(id);
		if (id != 0 && item != m_blockCityItemMap.end())
		{
			return item->second;
		}
		return nullptr;
	}

	i32 BlockCitySetting::getItemPrice(i32 id)
	{
		auto item = getItem(id);
		return item ? item->price : 0;
	}

	i32 BlockCitySetting::getItemCube(i32 id)
	{
		auto item = getItem(id);
		return item ? item->cube : 0;
	}

	String BlockCitySetting::getItemIcon(i32 id)
	{
		auto item = getItem(id);
		return item ? item->icon : "";
	}

	String BlockCitySetting::getItemName(i32 id)
	{
		auto item = getItem(id);
		return item ? item->name : "";
	}

	String BlockCitySetting::getItemDesc(i32 id)
	{
		auto item = getItem(id);
		return item ? item->desc : "";
	}

}


