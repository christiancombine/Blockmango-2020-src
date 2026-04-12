#include "BlockFortItemSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN
{
	BlockFortItemSetting::BlockFortItemSetting(bool isClient)
	{
		m_itemInfos.insert({ "Size", BlockFortItemInfo(1, LanguageManager::Instance()->getString("gui_blockfort_size"), "set:blockfort_store.json image:size") });
		m_itemInfos.insert({ "HP", BlockFortItemInfo(2, LanguageManager::Instance()->getString("gui_blockfort_hp"), "set:blockfort_store.json image:hp") });
		m_itemInfos.insert({ "Damage", BlockFortItemInfo(3, LanguageManager::Instance()->getString("gui_blockfort_damage"), "set:blockfort_store.json image:damage") });
		m_itemInfos.insert({ "Exp", BlockFortItemInfo(4, LanguageManager::Instance()->getString("gui_blockfort_exp"), "set:blockfort_store.json image:exp") });
		loadSetting(isClient); 
		
		m_buildingItemInfos.insert({ "HP", BlockFortItemInfo(2,  LanguageManager::Instance()->getString("gui_blockfort_hp"), "set:blockfort_store.json image:hp") });
		m_buildingItemInfos.insert({ "Damage", BlockFortItemInfo(3, LanguageManager::Instance()->getString("gui_blockfort_damage"), "set:blockfort_store.json image:damage") });
		m_buildingItemInfos.insert({ "Exp", BlockFortItemInfo(4, LanguageManager::Instance()->getString("gui_blockfort_exp"), "set:blockfort_store.json image:exp") });
		loadBuildingSetting(isClient);
	}

	BlockFortItemSetting::~BlockFortItemSetting()
	{
		unloadSetting();
		unloadBuildingSetting();
	}

	bool BlockFortItemSetting::loadSetting(bool isClient)
	{
		BlockFortItem* pBlockFortItem = NULL;
		m_itemMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = "";
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/fortItems.csv").c_str();
		}
		else
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/fortItems.csv").c_str();
#else
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting/fortItems.csv").c_str();
#endif
		}
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
			pBlockFortItem = new BlockFortItem();
			pReader->GetInt("Id", pBlockFortItem->id);
			pReader->GetInt("ItemId", pBlockFortItem->itemId);
			pReader->GetInt("Type", pBlockFortItem->type);
			pReader->GetInt("Price", pBlockFortItem->price);
			pReader->GetInt("Cube", pBlockFortItem->cube);
			pReader->GetInt("Level", pBlockFortItem->level);
			pReader->GetInt("HP", pBlockFortItem->hp);
			pReader->GetInt("Exp", pBlockFortItem->exp);
			pReader->GetInt("Damage", pBlockFortItem->damage);
			pReader->GetString("Icon", pBlockFortItem->icon);
			pReader->GetString("Name", pBlockFortItem->name);
			pReader->GetString("Desc", pBlockFortItem->desc);
			pReader->GetString("Size", pBlockFortItem->size);
			String infoSort = "";
			pReader->GetString("InfoSort", infoSort);
			if (!infoSort.empty())
			{
				StringArray keys = StringUtil::Split(infoSort, "#");
				for (const auto& key : keys)
				{
					auto item = getItemInfo(key, pReader);
					if (item.id > 0)
					{
						pBlockFortItem->infos.push_back(item);
					}
				}
			}
			m_itemMap[pBlockFortItem->id] = pBlockFortItem;
		}

		LordSafeDelete(pReader);
		return true;
	}

	bool BlockFortItemSetting::loadBuildingSetting(bool isClient)
	{
		BlockFortBuilding* pBlockFort = NULL;
		m_buildingMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = "";
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/buildingUpgrade.csv").c_str();
		}
		else
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/buildingUpgrade.csv").c_str();
#else
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting/buildingUpgrade.csv").c_str();
#endif
		}
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
			pBlockFort = new BlockFortBuilding();
			pReader->GetInt("Id", pBlockFort->id);
			pReader->GetInt("moneyType", pBlockFort->moneyType);
			pReader->GetString("Price", pBlockFort->price);
			pReader->GetString("Cube", pBlockFort->cube);

			String infoSort = "";
			pReader->GetString("InfoSort", infoSort);
			if (!infoSort.empty())
			{
				StringArray keys = StringUtil::Split(infoSort, "#");
				for (const auto& key : keys)
				{
					auto item = getBuildingItemInfo(key, pReader);
					if (item.id > 0)
					{
						pBlockFort->infos.push_back(item);
					}
				}
			}

			m_buildingMap[pBlockFort->id] = pBlockFort;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void  BlockFortItemSetting::unloadSetting()
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
	
	void  BlockFortItemSetting::unloadBuildingSetting()
	{
		for (auto iter = m_buildingMap.begin(); iter != m_buildingMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}
		m_buildingMap.clear();
	}

	BlockFortItem * BlockFortItemSetting::findItem(i32 id)
	{
		auto item = m_itemMap.find(id);
		if (id != 0 && item != m_itemMap.end())
		{
			return item->second;
		}
		return nullptr;
	}

	BlockFortItem * BlockFortItemSetting::findItemByItemId(i32 itemId)
	{
		for (auto& item : m_itemMap)
		{
			if (item.second->itemId == itemId)
			{
				return item.second;
			}
		}

		return nullptr;
	}

	String BlockFortItemSetting::getItemIcon(i32 id)
	{
		auto item = findItem(id);
		return item ? item->icon : "";
	}

	String BlockFortItemSetting::getItemName(i32 id)
	{
		auto item = findItem(id);
		return item ? item->name : "";
	}
	
	String BlockFortItemSetting::getItemDesc(i32 id)
	{
		auto item = findItem(id);
		return item ? item->desc : "";
	}

	i32 BlockFortItemSetting::getItemPrice(i32 id, i32 moneyType)
	{
		if (auto item = findItem(id))
		{
			return moneyType == 1 ? item->cube : item->price;
		}
		return 0;
	}

	i32 BlockFortItemSetting::getItemType(i32 id)
	{
		auto item = findItem(id);
		return item ? item->type : 0;
	}

	const vector<BlockFortItemInfo>::type BlockFortItemSetting::getItemInfos(i32 id)
	{
		if (auto item = findItem(id))
		{
			return item->infos;
		}
		return vector<BlockFortItemInfo>::type();
	}

	BlockFortItemInfo BlockFortItemSetting::getItemInfo(String key, CsvReader * reader)
	{
		auto item = m_itemInfos.find(key);
		if (item != m_itemInfos.end())
		{
			BlockFortItemInfo fortItem = item->second;
			reader->GetString(key.c_str(), fortItem.value);
			return fortItem;
		}

		return BlockFortItemInfo();
	}

	BlockFortItemInfo BlockFortItemSetting::getBuildingItemInfo(String key, CsvReader* reader)
	{
		auto item = m_buildingItemInfos.find(key);
		if (item != m_buildingItemInfos.end())
		{
			BlockFortItemInfo fortItem = item->second;
			reader->GetString(key.c_str(), fortItem.value);
			return fortItem;
		}

		return BlockFortItemInfo();
	}

	BlockFortBuilding* BlockFortItemSetting::findBuilding(i32 id)
	{
		auto item = m_buildingMap.find(id);
		return (item != m_buildingMap.end() && id != 0) ? item->second : nullptr;
	}

}
