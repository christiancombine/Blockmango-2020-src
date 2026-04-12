#pragma once
#ifndef __RUNE_SETTING_HEADER__
#define __RUNE_SETTING_HEADER__

#include "Core.h"
#include <vector>
#include <map>

using namespace LORD;

namespace BLOCKMAN
{
	enum RuneType
	{
		RT_INVALID = -1,
		RT_ATTACK = 0,
		RT_DEFENCE = 1,
		RT_SKILL = 2,
		RT_FUNC = 3,
	};

	class RuneSettingItem
	{
	public:
		int Id = 0;
		RuneType Type = RT_INVALID;
		int Level = 0;
		String Name = "";
		String Des = "";
		String DesPro = "";
		String Image = "";
		int Weight = 0;
		int SellKey = 0;
		bool CanSell = true;
	};

	class RuneTroughSettingItem
	{
	public:
		int Id = 0;
		RuneType Type = RT_INVALID;
		int Level = 0;
	};

	class RuneTypeSettingItem
	{
	public:
		int Type = 0;
		String FlagImg = "";
		String BgImg = "";
		String Title = "";
	};

	class RuneSetting
	{
	private:
		static std::map<int, RuneSettingItem*> m_rune_map;
		static std::map<int, RuneTroughSettingItem*> m_rune_trough_map;
		static std::map<int, RuneTypeSettingItem*> m_rune_type_map;

	public:
		static void	loadSetting();
		static void	unloadSetting();

		static std::map<int, RuneSettingItem*> getAllRune() { return m_rune_map; }
		static std::map<int, RuneTroughSettingItem*> getAllRuneTrough() { return m_rune_trough_map; }
		static std::map<int, RuneTypeSettingItem*> getAllRuneType() { return m_rune_type_map; }

		static RuneSettingItem* findRuneItemById(int id);
		static RuneTroughSettingItem* findRuneTroughItemById(int id);
		static RuneTypeSettingItem* findRuneTypeItemByType(int type);

	private:
		static void loadRuneSetting();
		static void loadRuneTroughSetting();
		static void loadRuneTypeSetting();
	};
}

#endif
