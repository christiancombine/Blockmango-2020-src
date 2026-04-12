#pragma once

#ifndef __SELLITEM_SETTING_HEADER__
#define __SELLITEM_SETTING_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{
	struct SellItemInfo
	{
		int uniqueId = 0;
		int itemId = 0;
		int itemMeta = 0;
		int itemMoney = 0;
		String itemDesc = "";
	};

	class SellItemSetting : public Singleton<SellItemSetting>, public ObjectAlloc
	{
	private:
		map<int, SellItemInfo>::type m_sellItemInfoMap;

	public:
		SellItemSetting();
		~SellItemSetting();

		bool getSellInfoFromMap(int itemId, int itemMeta, SellItemInfo& sellItemInfo);
		bool loadSetting(bool isClient);
		void unloadSetting();
	};
}

#endif // __SELLITEM_SETTING_HEADER__


