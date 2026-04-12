#include "DesignationSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<int, DesignationItem*>::type DesignationSetting::m_designation_map = {};
	map<int, DesignationTypeItem*>::type DesignationSetting::m_designation_type_map = {};
	map<int, DesignationTotal*>::type DesignationSetting::m_designation_total_map = {};

	void DesignationSetting::loadSetting()
	{
		loadDesignationSetting();
		loadDesignationTypeSetting();

	}

	void DesignationSetting::unloadSetting()
	{
		for (auto item : m_designation_map)
		{
			delete item.second;
		}
		m_designation_map.clear();
		for (auto item : m_designation_type_map)
		{
			delete item.second;
		}
		m_designation_type_map.clear();
		for (auto item : m_designation_total_map)
		{
			delete item.second;
		}
		m_designation_total_map.clear();
	}

	DesignationItem* DesignationSetting::findDesignationById(int id)
	{
		if (m_designation_map.find(id)!=m_designation_map.end())
		{
			return m_designation_map.find(id)->second;
		}
		return nullptr;
	}

	DesignationTypeItem* DesignationSetting::findDesignationTypeById(int id)
	{
		if (m_designation_type_map.find(id) != m_designation_type_map.end())
		{
			return m_designation_type_map.find(id)->second;
		}
		return nullptr;
		
	}
	DesignationTotal* DesignationSetting::findDesignationTotalById(int id)
	{
		if (m_designation_total_map.find(id) != m_designation_total_map.end())
		{
			return m_designation_total_map.find(id)->second;
		}
		return nullptr;
	}
	void DesignationSetting::loadDesignationSetting()
	{
		DesignationItem* item = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "TitleSetting.csv").c_str();
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
			item = new DesignationItem();
			pReader->GetInt("Id", item->Id);
			pReader->GetInt("Type", item->Type);
			pReader->GetInt("Condition", item->Condition);
			pReader->GetInt("Weight", item->Weight);
			pReader->GetString("Name", item->Name);
			pReader->GetString("ConditionName", item->ConditionName);
			m_designation_map[item->Id] = item;
		}
		LordSafeDelete(pReader);
	}

	void DesignationSetting::loadDesignationTypeSetting()
	{
		DesignationTypeItem* item = NULL;
		DesignationTotal* itemTotal = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "TitleTypeSetting.csv").c_str();
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
			item = new DesignationTypeItem();
			pReader->GetInt("Id", item->Id);
			pReader->GetInt("Type", item->Type);
			pReader->GetInt("Weight", item->Weight);
			pReader->GetString("Name", item->Name);
			m_designation_type_map[item->Id] = item;

			itemTotal = new DesignationTotal();
			m_designation_total_map[item->Type] = itemTotal;
		}
		LordSafeDelete(pReader);
	}
	void DesignationSetting::parseUpdateDesignationData(const String & data)
	{
		if (data.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return;
		}
		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("Id"))
				continue;
			int Id = obj.FindMember("Id")->value.GetInt();
			DesignationItem* title =findDesignationById(Id);
			if (!title)
				continue;
			int Status = obj.HasMember("Status") ? obj.FindMember("Status")->value.GetInt() : title->Status;
			title->Status = Status;
		}
		LordDelete(doc);
	}

	void DesignationSetting::parseUpdateDesignationTypeData(const String & data)
	{
		if (data.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return;
		}
		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("TypeId"))
				continue;
			int TypeId = obj.FindMember("TypeId")->value.GetInt();
			DesignationTotal* titleTotal = findDesignationTotalById(TypeId);
			if (!titleTotal)
				continue;
			int Num = obj.HasMember("Num") ? obj.FindMember("Num")->value.GetInt() : titleTotal->num;
			titleTotal->num = Num;
		}
		LordDelete(doc);
	}
}