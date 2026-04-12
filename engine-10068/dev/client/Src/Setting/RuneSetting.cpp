#include "RuneSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{

	std::map<int, RuneSettingItem*>	RuneSetting::m_rune_map = {};
	std::map<int, RuneTroughSettingItem*> RuneSetting::m_rune_trough_map = {};
	std::map<int, RuneTypeSettingItem*> RuneSetting::m_rune_type_map = {};

	void RuneSetting::loadSetting()
	{
		loadRuneSetting();
		loadRuneTroughSetting();
		loadRuneTypeSetting();
	}

	void RuneSetting::unloadSetting()
	{
		for (auto iter : m_rune_map)
		{
			delete iter.second;
		}
		m_rune_map.clear();
		for (auto iter : m_rune_trough_map)
		{
			delete iter.second;
		}
		m_rune_trough_map.clear();
	}

	void RuneSetting::loadRuneSetting()
	{
		RuneSettingItem* pRuneSettingItem = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "RuneSetting.csv").c_str();
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
			pRuneSettingItem = new RuneSettingItem();
			pReader->GetInt("Id", pRuneSettingItem->Id);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pRuneSettingItem->Type = (RuneType)Type;
			pReader->GetInt("Level", pRuneSettingItem->Level);
			pReader->GetString("Name", pRuneSettingItem->Name);
			pReader->GetString("Des", pRuneSettingItem->Des);
			pReader->GetString("DesPro", pRuneSettingItem->DesPro);
			pReader->GetString("Image", pRuneSettingItem->Image);
			pReader->GetInt("Weight", pRuneSettingItem->Weight);
			pReader->GetInt("SellKey", pRuneSettingItem->SellKey);
			pReader->GetBool("CanSell", pRuneSettingItem->CanSell);
			m_rune_map[pRuneSettingItem->Id] = pRuneSettingItem;
		}
		
		LordSafeDelete(pReader);
	}

	void RuneSetting::loadRuneTroughSetting()
	{
		RuneTroughSettingItem* pRuneTroughSettingItem = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "RuneTroughSetting.csv").c_str();
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
			pRuneTroughSettingItem = new RuneTroughSettingItem();
			pReader->GetInt("Id", pRuneTroughSettingItem->Id);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pRuneTroughSettingItem->Type = (RuneType)Type;
			pReader->GetInt("Level", pRuneTroughSettingItem->Level);
			m_rune_trough_map[pRuneTroughSettingItem->Id] = pRuneTroughSettingItem;
		}
		
		LordSafeDelete(pReader);
	}

	void RuneSetting::loadRuneTypeSetting()
	{
		RuneTypeSettingItem* pRuneTypeSettingItem = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "RuneType.csv").c_str();
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
			pRuneTypeSettingItem = new RuneTypeSettingItem();
			pReader->GetInt("Type", pRuneTypeSettingItem->Type);
			pReader->GetString("FlagImg", pRuneTypeSettingItem->FlagImg);
			pReader->GetString("BgImg", pRuneTypeSettingItem->BgImg);
			pReader->GetString("Title", pRuneTypeSettingItem->Title);
			m_rune_type_map[pRuneTypeSettingItem->Type] = pRuneTypeSettingItem;
		}
		
		LordSafeDelete(pReader);
	}

	RuneSettingItem * RuneSetting::findRuneItemById(int id)
	{
		for (auto iter : m_rune_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	RuneTroughSettingItem * RuneSetting::findRuneTroughItemById(int id)
	{
		for (auto iter : m_rune_trough_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	RuneTypeSettingItem * RuneSetting::findRuneTypeItemByType(int type)
	{
		for (auto iter : m_rune_type_map)
		{
			if (iter.second->Type == type)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

};