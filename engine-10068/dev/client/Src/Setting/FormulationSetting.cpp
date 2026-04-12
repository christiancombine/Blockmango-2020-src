#include "FormulationSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<int, FormulationItem*>::type FormulationSetting::m_formulation_map = {};
	map<int, MaterialItem*>::type FormulationSetting::m_material_map = {};

	void FormulationSetting::loadSetting()
	{
		loadFormulationSetting();
		loadMaterialSetting();

	}

	void FormulationSetting::unloadSetting()
	{
		for (auto item : m_formulation_map)
		{
			delete item.second;
		}
		m_formulation_map.clear();
		for (auto item : m_material_map)
		{
			delete item.second;
		}
		m_material_map.clear();
	}

	FormulationItem* FormulationSetting::findFormulationById(int id)
	{
		return m_formulation_map.find(id)->second;
	}

	MaterialItem* FormulationSetting::findMaterialById(int id)
	{
		return m_material_map.find(id)->second;
	}

	void FormulationSetting::loadFormulationSetting()
	{
		FormulationItem* item = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "FormulationSetting.csv").c_str();
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
			item = new FormulationItem();
			pReader->GetInt("Id", item->id);
			pReader->GetInt("ItemId", item->ItemId);
			pReader->GetInt("Count", item->Count);
			pReader->GetString("Icon", item->Icon);
			pReader->GetString("Name", item->Name);
			pReader->GetString("Materials", item->Materials);
			m_formulation_map[item->id] = item;
		}
		LordSafeDelete(pReader);
	}

	void FormulationSetting::loadMaterialSetting()
	{
		MaterialItem* item = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "MaterialSetting.csv").c_str();
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
			item = new MaterialItem();
			pReader->GetInt("Id", item->id);
			pReader->GetInt("ItemId", item->ItemId);
			pReader->GetString("Icon", item->Icon);
			pReader->GetInt("Count", item->Count);
			m_material_map[item->id] = item;
		}
		LordSafeDelete(pReader);
	}
}