#pragma once
#ifndef __BRIEF_INTRODUCTION_SETTING_HEADER__
#define __BRIEF_INTRODUCTION_SETTING_HEADER__

#include "Core.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct BI_picture
	{
		String picTitle = "";
		String picText = "";
		String picDetil = "";
	};

	struct BI_group
	{
		String GroupKey = "";
		vector<BI_picture*>::type pictures = {};
	};

	class BriefIntroductionSetting
	{
	private:
		static map<String, BI_group*>::type m_BI_group_map;

	public:
		static void	loadSetting();
		static void unloadSetting();
		static BI_group* getFirstGroup();
		static BI_group* findGroupByKey(String key);
	};
}

#endif 
