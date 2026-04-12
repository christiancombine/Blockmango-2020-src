#pragma once
#ifndef __CLASSIFICATION_SETTING_HEADER__
#define __CLASSIFICATION_SETTING_HEADER__

#include "Core.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct ClassificationTab
	{
		int tabId = 0;
		String tabName = "";
		vector<int>::type itemIds;
	};

	struct ClassificationItem
	{
		int itemId = 0;
		String detail= "";
		String backGround = "";
	};

	class ClassificationSetting
	{
	private:
		static map<int, ClassificationTab*>::type m_tab_map;
		static map<int, ClassificationItem*>::type m_item_map;

	public:
		static void	loadSetting();
		static void unloadSetting();
		static ClassificationItem* getItem(int itemId) { return m_item_map[itemId]; }
		static void setDetails(String data);
		static ClassificationTab* findTabById(int tabId) { return m_tab_map[tabId]; }
		static map<int, ClassificationTab*>::type getTabMap() { return m_tab_map; }
		static bool haveCustomDetail() { return m_item_map.begin() != m_item_map.end(); }
	};
}

#endif 
