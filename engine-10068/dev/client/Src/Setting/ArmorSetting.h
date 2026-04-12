#pragma once
#ifndef __ARMOR_SETTING_HEADER__
#define __ARMOR_SETTING_HEADER__

#include "Core.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct ArmorItem
	{
		int itemId = 0;
		int extraInvCount = 0;
	};

	class ArmorSetting
	{
	private:
		static map<int, ArmorItem*>::type m_item_setting_map;

	public:
		static void	loadSetting();
		static void unloadSetting();
		static ArmorItem* getSettingByItemId(int itemId) {
			return m_item_setting_map[itemId];
		}
	};
}

#endif 
