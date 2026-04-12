#pragma once

#ifndef __RECIPE_TAB_SETTINF_HEADER__
#define __RECIPE_TAB_SETTINF_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{
	struct RecipeTabSettingInfo
	{
		String recipeName = "";
		int recipeTab = 0;
		int recipeAllWeight = 0;
		int recipeTypeWeight = 0;
	};

	class RecipeTabSetting : public Singleton<RecipeTabSetting>, public ObjectAlloc
	{
	private:
		map<String, RecipeTabSettingInfo>::type m_recipeTabInfoMap;

	public:
		RecipeTabSetting();
		~RecipeTabSetting();

 		bool getRecipeTabInfoFromMap(const String &recipeName, RecipeTabSettingInfo& recipeTabSettingInfo);
		bool loadSetting(bool isClient);
		void unloadSetting();
		void loadRecipe();
	};
}

#endif // __RECIPE_TAB_SETTING_HEADER__
