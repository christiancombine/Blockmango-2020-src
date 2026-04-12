#include "ClassificationSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<int, ClassificationTab*>::type ClassificationSetting::m_tab_map = {};
	map<int, ClassificationItem*>::type ClassificationSetting::m_item_map = {};
	void ClassificationSetting::loadSetting()
	{
		ClassificationTab* tab = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "ClassificationTabs.csv").c_str();
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
			tab = new ClassificationTab();
			pReader->GetInt("tabId", tab->tabId);
			pReader->GetString("tabName", tab->tabName);
			if (tab->tabName == "#")
				tab->tabName == "";
			tab->itemIds = {};
			m_tab_map[tab->tabId] = tab;
		}

		path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "ClassificationSetting.csv").c_str();
		bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		ClassificationItem* item = NULL;
		while (pReader->Read())
		{
			item = new ClassificationItem();
			int tabId;
			pReader->GetInt("tabId", tabId);
			tab = m_tab_map[tabId];
			pReader->GetInt("itemId", item->itemId);
			pReader->GetString("detail", item->detail);
			if (item->detail == "#")
				item->detail == "";
			pReader->GetString("backGround", item->backGround);
			if (item->backGround == "#")
				item->backGround == "";
			m_item_map[item->itemId] = item;
			tab->itemIds.push_back(item->itemId);
		}
		LordSafeDelete(pReader);
	}

	void ClassificationSetting::unloadSetting()
	{
		for (auto item : m_tab_map)
		{
			delete item.second;
		}
		m_tab_map.clear();

		for (auto item : m_item_map)
		{
			delete item.second;
		}
		m_item_map.clear();
	}

	void ClassificationSetting::setDetails(String data)
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
			if (!obj.HasMember("itemId"))
				continue;
			int itemId = obj.FindMember("itemId")->value.GetInt();
			if (m_item_map.find(itemId) == m_item_map.end())
			{
				m_item_map.emplace(std::make_pair(itemId, new ClassificationItem()));
			}
			m_item_map[itemId]->detail = obj.FindMember("detail")->value.GetString();
		}
		LordDelete(doc);
	}
}