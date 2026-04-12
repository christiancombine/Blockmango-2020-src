#pragma once

#ifndef __FURNACE_SETTING_HEADER__
#define __FURNACE_SETTING_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{
	struct FurnaceMaterialSettingInfo
	{
		int uniqueId = 0;
		int furnaceMaterialId = 0;
		int furnaceMaterialMeta = 0;
		int furnaceMaterialOutId = 0;
		int furnaceMaterialOutMeta = 0;
	};

	struct FurnaceFuelSettingInfo
	{
		int furnaceFuelId = 0;
	};

	class FurnaceSetting : public Singleton<FurnaceSetting>, public ObjectAlloc
	{
	private:
		map<int, FurnaceMaterialSettingInfo>::type m_furnaceMaterialInfoMap;
		map<int, FurnaceFuelSettingInfo>::type m_furnaceFuelInfoMap;

	public:
		FurnaceSetting();
		~FurnaceSetting();

		bool getMaterialInfoFromMap(int materialId, int materialMeta, FurnaceMaterialSettingInfo& furnaceMaterialSettingInfo);
		bool getFuelInfoFromMap(int fuelId, FurnaceFuelSettingInfo& furnaceFuelSettingInfo);
		
		bool loadSetting(bool isClient);
		bool unloadSetting();

		void loadRecipe();
		bool isItemFuel(int itemId);

	private:
		bool loadMaterialSetting(bool isClient);
		bool loadFuelSetting(bool isClient);

		void unloadMaterialSetting();
		void unloadFuelSetting();


	};
}

#endif // __FURNACE_SETTING_HEADER__

