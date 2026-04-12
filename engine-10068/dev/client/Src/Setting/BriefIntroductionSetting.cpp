#include "BriefIntroductionSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<String, BI_group*>::type BriefIntroductionSetting::m_BI_group_map = {};

	void BriefIntroductionSetting::loadSetting()
	{
		BI_group* group = NULL;
		BI_picture* Picture = NULL;
		String GroupKey = "";
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "BriefIntroductionSetting.csv").c_str();
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
			Picture = new BI_picture();
			pReader->GetString("GroupKey", GroupKey);
			pReader->GetString("PicTitle", Picture->picTitle);
			pReader->GetString("PicText", Picture->picText);
			pReader->GetString("PicDetil", Picture->picDetil);
			auto iter_group = m_BI_group_map.find(GroupKey);
			if (iter_group == m_BI_group_map.end())
			{
				group = new BI_group();
				group->GroupKey = GroupKey;
				m_BI_group_map[GroupKey] = group;
			}
			else
			{
				group = iter_group->second;
			}
			group->pictures.push_back(Picture);
		}
		
		LordSafeDelete(pReader);
	}

	void BriefIntroductionSetting::unloadSetting()
	{
		for (auto iter : m_BI_group_map)
		{
			auto group = iter.second;
			for (auto picture : group->pictures)
			{
				delete picture;
			}
			group->pictures.clear();
			delete group;
		}
		m_BI_group_map.clear();
	}

	BI_group * BriefIntroductionSetting::getFirstGroup()
	{
		if (m_BI_group_map.begin() != m_BI_group_map.end())
		{
			return m_BI_group_map.begin()->second;
		}
		return nullptr;
	}

	BI_group * BriefIntroductionSetting::findGroupByKey(String Key)
	{
		auto iter = m_BI_group_map.find(Key);
		if (iter != m_BI_group_map.end())
		{
			return iter->second;
		}
		if (m_BI_group_map.end() != m_BI_group_map.begin())
		{
			return BriefIntroductionSetting::m_BI_group_map.begin()->second;
		}
		return nullptr;
	}
}