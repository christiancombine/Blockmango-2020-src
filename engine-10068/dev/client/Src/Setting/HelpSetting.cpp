#include "HelpSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	vector<HelpItem*>::type  HelpSetting::m_Helps = {};

	bool HelpSetting::loadSetting()
	{
		unloadSetting();
		HelpItem* pHelpItem = nullptr;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/Help.csv").c_str();

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}
		pReader->Read();
		while (pReader->Read())
		{
			pHelpItem = new HelpItem();
			pReader->GetInt("Id", pHelpItem->id);
			pReader->GetString("Icon", pHelpItem->icon);
			pReader->GetString("Name", pHelpItem->name);
			pReader->GetString("Content", pHelpItem->content);
			m_Helps.push_back(pHelpItem);
		}

		
		LordSafeDelete(pReader);
		return true;
	}

	void  HelpSetting::unloadSetting()
	{
		for (auto& item : m_Helps)
		{
			delete item;
			item = nullptr;
		}
		m_Helps.clear();
	}

	const vector<HelpItem*>::type& HelpSetting::getHelps()
	{
		return m_Helps;
	}

}


