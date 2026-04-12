#include "BlockSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{

	BlockSetting::BlockSetting()
	{

	}

	BlockSetting::~BlockSetting()
	{
		
	}

	bool BlockSetting::loadSetting(bool bIsClient)
	{
		CsvReader* pReader = new CsvReader();

		std::string path;
		if (bIsClient)
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "BlockSetting.csv").c_str();
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "BlockSetting.csv").c_str();
		}

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogInfo("Can not open Block setting file %s", path.c_str());
			LordSafeDelete(pReader);
			return false;
		}

		for (auto &iter : m_blockSettingMap)
		{
			iter.second.m_breakBlockValue.clear();
		}
		m_blockSettingMap.clear();

		pReader->Read(); // discard the second line
		pReader->Read();
		while (pReader->Read())
		{
			BlockSetInfo pSetting;
			pReader->GetInt("Id", pSetting.id);
			pReader->GetInt("BlockId", pSetting.blockId);
			pReader->GetInt("Meta", pSetting.meta);
			pReader->GetFloat("DefaultHurt", pSetting.defaultHurt);
			String holdItemIdList = "";
			String holdItemHurtList = "";
			pReader->GetString("HoldItemId", holdItemIdList);
			pReader->GetString("HoldItemHurt", holdItemHurtList);
			StringArray holdItemIdArray = StringUtil::Split(holdItemIdList, "#");
			StringArray holdItemHurtArray = StringUtil::Split(holdItemHurtList, "#");
			for (size_t i = 0; i < holdItemIdArray.size(); i++)
			{
				pSetting.m_breakBlockValue[StringUtil::ParseInt(holdItemIdArray[i].c_str())] = StringUtil::ParseFloat(holdItemHurtArray[i].c_str());
			}
			m_blockSettingMap[pSetting.id] = pSetting;
		}
		LordSafeDelete(pReader);
		return true;
	}

	void BlockSetting::unloadSetting()
	{
		for (auto &iter : m_blockSettingMap)
		{
			iter.second.m_breakBlockValue.clear();
		}
		m_blockSettingMap.clear();
	}

	float BlockSetting::getBreakBlockValue(int toolId ,int blockId)
	{
		for (auto iter : m_blockSettingMap)
		{
			if (iter.second.blockId == blockId)
			{
				auto valua_iter = iter.second.m_breakBlockValue.find(toolId);

				if (valua_iter != iter.second.m_breakBlockValue.end())
				{
					return valua_iter->second;
				}
				else
				{
					return iter.second.defaultHurt;
				}
			}
		}

		return 48.f;
	}
}


