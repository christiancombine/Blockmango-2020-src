#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "ConfigurableBlockSetting.h"

namespace BLOCKMAN
{
	ConfigurableBlockSetting::ConfigurableBlockSetting(bool isClient)
	{
		loadSetting(isClient);
	}

	ConfigurableBlockSetting::~ConfigurableBlockSetting()
	{
		unloadSetting();
	}

	bool ConfigurableBlockSetting::loadSetting(bool isClient)
	{
		mConfigurableBlockMap.clear();

		CsvReader* pReader = new CsvReader();

		std::string path;
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "configurableBlock.csv").c_str();
		}
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "configurableBlock.csv").c_str();
		}
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}
		pReader->Read(); // discard the second line
		pReader->Read();
		while (pReader->Read())
		{
			ConfigurableBlockInfo configurableBlockInfo;
			pReader->GetInt("UniqueId", configurableBlockInfo.uniqueId);
			pReader->GetInt("BlockId", configurableBlockInfo.blockId);
			pReader->GetInt("BlockMeta", configurableBlockInfo.blockMeta);
			pReader->GetInt("BlockType", configurableBlockInfo.blockType);
			pReader->GetInt("ExtraParam1", configurableBlockInfo.extraParam1);
			pReader->GetInt("ExtraParam2", configurableBlockInfo.extraParam2);
			pReader->GetString("IconName", configurableBlockInfo.iconName);
			pReader->GetString("BlockDes", configurableBlockInfo.blockDesc);
			mConfigurableBlockMap[configurableBlockInfo.uniqueId] = configurableBlockInfo;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void ConfigurableBlockSetting::unloadSetting()
	{
		mConfigurableBlockMap.clear();
	}

	bool ConfigurableBlockSetting::getConfigurableBlockInfo(int blockId, int blockMeta, ConfigurableBlockInfo & configurableBlockInfo)
	{
		for (auto iter : mConfigurableBlockMap)
		{
			if (iter.second.blockId == blockId && iter.second.blockMeta == blockMeta)
			{
				configurableBlockInfo = iter.second;
				return true;
			}
		}
		return false;
	}

	bool ConfigurableBlockSetting::getConfigurableBlockInfoByUniqueId(int uniqueId, ConfigurableBlockInfo& configurableBlockInfo)
	{
		for (auto iter : mConfigurableBlockMap)
		{
			if (iter.second.uniqueId == uniqueId)
			{
				configurableBlockInfo = iter.second;
				return true;
			}
		}
		return false;
	}

	bool ConfigurableBlockSetting::isStairsBlock(int blockID)
	{
		for (auto iter : mConfigurableBlockMap)
		{
			if (iter.second.blockId == blockID && iter.second.blockType == BT_STAIRS)
			{
				return true;
			}
		}
		return false;
	}
	bool ConfigurableBlockSetting::isSingleSlab(int blockID)
	{
		for (auto iter : mConfigurableBlockMap)
		{
			if (iter.second.blockId == blockID && iter.second.blockType == BT_STEP_SINGLE)
			{
				return true;
			}
		}
		return false;
	}
	bool ConfigurableBlockSetting::isDoubleSlab(int blockID)
	{
		for (auto iter : mConfigurableBlockMap)
		{
			if (iter.second.blockId == blockID && iter.second.blockType == BT_STEP_DOUBLE)
			{
				return true;
			}
		}
		return false;
	}

	bool ConfigurableBlockSetting::isSlab(int blockID)
	{
		for (auto iter : mConfigurableBlockMap)
		{
			if (iter.second.blockId == blockID && (iter.second.blockType == BT_STEP_DOUBLE || iter.second.blockType == BT_STEP_SINGLE))
			{
				return true;
			}
		}
		return false;
	}

	int ConfigurableBlockSetting::getSingleSlabByDoubleSlab(int blockID)
	{
		for (auto iter : mConfigurableBlockMap)
		{
			if (iter.second.blockId == blockID && iter.second.blockType == BT_STEP_DOUBLE)
			{
				return iter.second.extraParam1;
			}
		}
		return 0;
	}

	int ConfigurableBlockSetting::getDoubleSlabBySingleSlab(int blockID)
	{
		for (auto iter : mConfigurableBlockMap)
		{
			if (iter.second.blockId == blockID && iter.second.blockType == BT_STEP_SINGLE)
			{
				return iter.second.extraParam1;
			}
		}
		return 0;
	}
}
