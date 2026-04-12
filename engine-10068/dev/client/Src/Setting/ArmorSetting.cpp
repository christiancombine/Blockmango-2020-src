#include "ArmorSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<int, ArmorItem*>::type ArmorSetting::m_item_setting_map = {};

	void ArmorSetting::loadSetting()
	{
		ArmorItem* item = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "ArmorConfig.csv").c_str();
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
			item = new ArmorItem();
			pReader->GetInt("itemId", item->itemId);
			pReader->GetInt("extraInvCount", item->extraInvCount);
			m_item_setting_map[item->itemId] = item;
		}
		LordSafeDelete(pReader);
	}

	void ArmorSetting::unloadSetting()
	{
		for (auto item : m_item_setting_map)
		{
			delete item.second;
		}
		m_item_setting_map.clear();
	}
}