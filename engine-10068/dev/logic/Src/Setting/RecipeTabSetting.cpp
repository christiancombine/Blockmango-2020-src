#include "RecipeTabSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Inventory/CraftingManager.h"

namespace BLOCKMAN
{
	RecipeTabSetting::RecipeTabSetting()
	{

	}

	RecipeTabSetting::~RecipeTabSetting()
	{

	}

	bool RecipeTabSetting::loadSetting(bool isClient)
	{
		m_recipeTabInfoMap.clear();

		CsvReader* pReader = new CsvReader();

		std::string path;
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "recipeTab.csv").c_str();
		}
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "recipeTab.csv").c_str();
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
			RecipeTabSettingInfo recipeTabSettingInfo;
			pReader->GetString("RecipeName", recipeTabSettingInfo.recipeName);
			pReader->GetInt("RecipeTab", recipeTabSettingInfo.recipeTab);
			pReader->GetInt("RecipeAllWeight", recipeTabSettingInfo.recipeAllWeight);
			pReader->GetInt("RecipeTypeWeight", recipeTabSettingInfo.recipeTypeWeight);
			m_recipeTabInfoMap[recipeTabSettingInfo.recipeName] = recipeTabSettingInfo;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void RecipeTabSetting::unloadSetting()
	{
		m_recipeTabInfoMap.clear();
	}

	void RecipeTabSetting::loadRecipe()
	{
		auto manager = CraftingManager::Instance();
		for (auto iter = m_recipeTabInfoMap.begin(); iter != m_recipeTabInfoMap.end(); ++iter)
		{
			if (iter->first.length() > 0)
			{
				manager->loadRecipe(iter->first.c_str());
			}
		}
	}

	bool RecipeTabSetting::getRecipeTabInfoFromMap(const String &recipeName, RecipeTabSettingInfo& recipeTabSettingInfo)
	{
		auto iter = m_recipeTabInfoMap.find(recipeName);
		if (iter != m_recipeTabInfoMap.end())
		{
			recipeTabSettingInfo = m_recipeTabInfoMap[recipeName];
			return true;
		}
		return false;
	}
}