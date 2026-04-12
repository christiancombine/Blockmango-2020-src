#pragma once

#ifndef __ABANDON_ITEM_SETTING_HEADER__
#define __ABANDON_ITEM_SETTING_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{
	struct AbandonItemInfo
	{
		int id = 0;
		int itemId = 0;
		int itemMeta = 0;
		bool canAbandon = true;
		String itemName = "";
		String itemDesc = "";
	};

	class AbandonItemSetting : public Singleton<AbandonItemSetting>, public ObjectAlloc
	{
	private:
		map<int, AbandonItemInfo>::type m_abandonItemInfoMap;

	public:
		AbandonItemSetting();
		~AbandonItemSetting();

		bool getAbandonInfoFromMap(int itemId, int itemMeta, AbandonItemInfo& sellItemInfo);
		bool loadSetting(bool isClient);
		void unloadSetting();
	};
}

#endif // __ABANDON_ITEM_SETTING_HEADER__


