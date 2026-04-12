#include "ModuleBlockSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include <memory>
#include "Util/StringUtil.h"

using namespace LORD;

namespace BLOCKMAN
{
	ModuleBlockDataMap ModuleBlockSetting::m_ModuleBlockDataMap;

	bool ModuleBlockSetting::loadSetting(bool isClient)
	{
		auto pReader = std::make_shared<CsvReader>();

		std::string path;
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "ModuleSetting.csv").c_str();
		}
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "ModuleSetting.csv").c_str();
		}

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogInfo("Can not open action state setting file %s", path.c_str());
			return false;
		}

		m_ModuleBlockDataMap.clear();

		pReader->Read(); // discard the second line
		pReader->Read();
		while (pReader->Read())
		{
			ModuleBlockData data;
			pReader->GetInt("Id", data._id);
			pReader->GetString("ModuleFile", data._moduleName);
			pReader->GetString("Image", data._moduleIcon);
			pReader->GetInt("CostBlockCount", data._costBlockNum);

			parseOffset(pReader, "offset45", data);
			parseOffset(pReader, "offset135", data);
			parseOffset(pReader, "offset225", data);
			parseOffset(pReader, "offset315", data);

			m_ModuleBlockDataMap.emplace(std::make_pair(data._id, data));
		}

		return true;
	}

	void ModuleBlockSetting::unloadSetting()
	{
		m_ModuleBlockDataMap.clear();
	}

	const ModuleBlockData * ModuleBlockSetting::getModuleBlockData(int id)
	{
		auto it = m_ModuleBlockDataMap.find(id);
		if (it == m_ModuleBlockDataMap.end())
		{
			return nullptr;
		}
		return &it->second;
	}

	const ModuleBlockData * ModuleBlockSetting::getModuleBlockData(std::string fileName)
	{
		for (auto it = m_ModuleBlockDataMap.begin();it != m_ModuleBlockDataMap.end();++it)
		{
			if (it->second._moduleName == fileName)
			{
				return &it->second;
			}
		}
		return nullptr;
	}

	void ModuleBlockSetting::parseOffset(const std::shared_ptr<LORD::CsvReader>& pReader, const std::string& name, ModuleBlockData& data)
	{
		String temp;
		pReader->GetString(name, temp);

		std::vector<std::string> result;
		StringUtil::SplitString(result, temp.c_str(), "#");
		if (result.size() < 5)
		{
			assert(0);
			return;
		}

		RotationType type{};
		if (name == "offset45")
		{
			type = RotationType::RotationType_0;
		}
		if (name == "offset135")
		{
			type = RotationType::RotationType_90;
		}
		if (name == "offset225")
		{
			type = RotationType::RotationType_180;
		}
		if (name == "offset315")
		{
			type = RotationType::RotationType_270;
		}

		data.vecOffsetPos[type].emplace_back(StringUtil::ParseI32(result[0].c_str()));
		data.vecOffsetPos[type].emplace_back(StringUtil::ParseI32(result[1].c_str()));
		data.vecOffsetPos[type].emplace_back(StringUtil::ParseI32(result[2].c_str()));

		data.vecRotation[type].emplace_back(StringUtil::ParseI32(result[3].c_str()));
		data.vecRotation[type].emplace_back(StringUtil::ParseI32(result[4].c_str()));
	}
}
