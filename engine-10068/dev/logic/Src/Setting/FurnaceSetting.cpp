#include "FurnaceSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Inventory/CraftingManager.h"
#include "Item/Recipes.h"

namespace BLOCKMAN
{
	FurnaceSetting::FurnaceSetting()
	{

	}

	FurnaceSetting::~FurnaceSetting()
	{

	}

	bool FurnaceSetting::getMaterialInfoFromMap(int materialId, int materialMeta, FurnaceMaterialSettingInfo & furnaceMaterialSettingInfo)
	{
		for (auto iter : m_furnaceMaterialInfoMap)
		{
			if (iter.second.furnaceMaterialId == materialId && iter.second.furnaceMaterialMeta == materialMeta)
			{
				furnaceMaterialSettingInfo = iter.second;
				return true;
			}
		}

		return false;
	}

	bool FurnaceSetting::getFuelInfoFromMap(int fuelId, FurnaceFuelSettingInfo & furnaceFuelSettingInfo)
	{
		auto iter = m_furnaceFuelInfoMap.find(fuelId);
		if (iter != m_furnaceFuelInfoMap.end())
		{
			furnaceFuelSettingInfo = m_furnaceFuelInfoMap[fuelId];
			return true;
		}
		return false;
	}

	bool FurnaceSetting::loadSetting(bool isClient)
	{
		unloadSetting();
		loadMaterialSetting(isClient);
		loadFuelSetting(isClient);
		return true;
	}

	bool FurnaceSetting::unloadSetting()
	{
		unloadMaterialSetting();
		unloadFuelSetting();
		return true;
	}

	bool FurnaceSetting::loadMaterialSetting(bool isClient)
	{
		CsvReader* pReader = new CsvReader();

		std::string path;
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "FurnaceMaterial.csv").c_str();
		}
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "FurnaceMaterial.csv").c_str();
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
			FurnaceMaterialSettingInfo furnaceMaterialSettingInfo;
			pReader->GetInt("UniqueId", furnaceMaterialSettingInfo.uniqueId);
			pReader->GetInt("FurnaceMaterialId", furnaceMaterialSettingInfo.furnaceMaterialId);
			pReader->GetInt("FurnaceMaterialMeta", furnaceMaterialSettingInfo.furnaceMaterialMeta);
			pReader->GetInt("FurnaceMaterialOutId", furnaceMaterialSettingInfo.furnaceMaterialOutId);
			pReader->GetInt("FurnaceMaterialOutMeta", furnaceMaterialSettingInfo.furnaceMaterialOutMeta);
			m_furnaceMaterialInfoMap[furnaceMaterialSettingInfo.uniqueId] = furnaceMaterialSettingInfo;
		}

		LordSafeDelete(pReader);
		return true;
	}

	bool FurnaceSetting::loadFuelSetting(bool isClient)
	{
		CsvReader* pReader = new CsvReader();

		std::string path;
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "FurnaceFuel.csv").c_str();
		}
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "FurnaceFuel.csv").c_str();
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
			FurnaceFuelSettingInfo furnaceFuelSettingInfo;
			pReader->GetInt("FurnaceFuelId", furnaceFuelSettingInfo.furnaceFuelId);
			m_furnaceFuelInfoMap[furnaceFuelSettingInfo.furnaceFuelId] = furnaceFuelSettingInfo;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void FurnaceSetting::unloadMaterialSetting()
	{
		m_furnaceMaterialInfoMap.clear();
	}

	void FurnaceSetting::unloadFuelSetting()
	{
		m_furnaceFuelInfoMap.clear();
	}

	void FurnaceSetting::loadRecipe()
	{
		auto furnaceRecipes = FurnaceRecipes::smelting();

		for (auto iter = m_furnaceMaterialInfoMap.begin(); iter != m_furnaceMaterialInfoMap.end(); ++iter)
		{
			if (iter->second.furnaceMaterialId > 0 && iter->second.furnaceMaterialOutId > 0)
			{
				furnaceRecipes->addSmelting(iter->second.furnaceMaterialId, LORD::make_shared<ItemStack>(iter->second.furnaceMaterialOutId, 1, iter->second.furnaceMaterialOutMeta));
			}
		}
	}

	bool FurnaceSetting::isItemFuel(int itemId)
	{
		for (auto iter = m_furnaceFuelInfoMap.begin(); iter != m_furnaceFuelInfoMap.end(); ++iter)
		{
			if (iter->second.furnaceFuelId == itemId)
			{
				return true;
			}
		}
		return false;
	}
}