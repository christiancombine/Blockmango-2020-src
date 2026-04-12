#pragma once

#ifndef __CONFIGURABLE_BLOCK_SETTING_HEADER__
#define __CONFIGURABLE_BLOCK_SETTING_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{
	enum BLOCK_TYPE
	{
		BT_CUBE = 0,
		BT_STAIRS = 1,
		BT_STEP_SINGLE = 2,
		BT_STEP_DOUBLE = 3,
		BT_GRASS_SIMILAR = 4,
		BT_DIFFERENT = 5,
	};

	struct ConfigurableBlockInfo
	{
		int uniqueId = 0;
		int blockId = 0;
		int blockMeta = 0;
		int blockType = 0;
		int extraParam1 = 0;
		int extraParam2 = 0;
		String iconName = "";
		String blockDesc = "";
	};

	class ConfigurableBlockSetting : public Singleton<ConfigurableBlockSetting>, public ObjectAlloc
	{
	private:
		map<int, ConfigurableBlockInfo>::type mConfigurableBlockMap;

	public:
		ConfigurableBlockSetting(bool isClient);
		~ConfigurableBlockSetting();

		bool loadSetting(bool isClient);
		void unloadSetting();
		bool getConfigurableBlockInfo(int blockId, int blockMeta, ConfigurableBlockInfo& configurableBlockInfo);
		int getBlockMapSize() { return (int)mConfigurableBlockMap.size(); }
		bool getConfigurableBlockInfoByUniqueId(int uniqueId, ConfigurableBlockInfo& configurableBlockInfo);
		bool isStairsBlock(int blockID);
		bool isSingleSlab(int blockID);
		bool isDoubleSlab(int blockID);
		bool isSlab(int blockID);
		int getSingleSlabByDoubleSlab(int blockID);
		int getDoubleSlabBySingleSlab(int blockID);
	};
}

#endif // __CUSTOM_BLOCK_SETTING_HEADER__

