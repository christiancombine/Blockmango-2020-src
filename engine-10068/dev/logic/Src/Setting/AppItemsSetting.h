#pragma once

#ifndef __APP_ITEMS_SETTINF_HEADER__
#define __APP_ITEMS_SETTINF_HEADER__

#include "BM_TypeDef.h"
#include "Network/protocol/PacketDefine.h"

namespace BLOCKMAN
{
	class SelectableActionInfo
	{
	public:
		int actionId = 0;
		int price = 0;
		int currency = 0;
		int buyId = 0;
		DEFINE_NETWORK_SERIALIZER(actionId, price, currency, buyId);
	};

	class AppItemsSetting : public Singleton<AppItemsSetting>, public ObjectAlloc
	{
	private:
		map<int, SelectableActionInfo>::type m_actionInfoMap;

	public:
		AppItemsSetting();
		~AppItemsSetting();

		void addActionInfoToMap(int actionId, int price, int currency, int buyId);
		bool getActionInfoFromMap(int actionId, SelectableActionInfo& actionInfo);
		vector<SelectableActionInfo>::type getActionInfoList(int vip);
		vector<int>::type getPurchasableActionIdList();
		void setActionInfoList(vector<SelectableActionInfo>::type actionInfoList);
		int getActionIdFromBuyId(int buyId);
	};
}

#endif // __APP_ITEMS_SETTINF_HEADER__