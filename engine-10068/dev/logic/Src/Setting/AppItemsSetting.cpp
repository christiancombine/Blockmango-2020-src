#include "AppItemsSetting.h"

namespace BLOCKMAN
{
	AppItemsSetting::AppItemsSetting()
	{

	}

	AppItemsSetting::~AppItemsSetting()
	{

	}

	void AppItemsSetting::addActionInfoToMap(int actionId, int price, int currency, int buyId)
	{
		SelectableActionInfo actionInfo;
		actionInfo.actionId = actionId;
		actionInfo.price = price;
		actionInfo.currency = currency;
		actionInfo.buyId = buyId;
		m_actionInfoMap.insert(std::pair<int, SelectableActionInfo>(actionId, actionInfo));
	}

	bool AppItemsSetting::getActionInfoFromMap(int actionId, SelectableActionInfo& actionInfo)
	{
		auto iter = m_actionInfoMap.find(actionId);
		if (iter != m_actionInfoMap.end())
		{
			actionInfo = m_actionInfoMap[actionId];
			return true;
		}
		return false;
	}

	vector<SelectableActionInfo>::type AppItemsSetting::getActionInfoList(int vip)
	{
		float scale = 1.0f;
		if (vip == 2)// VIP+
		{
			scale = 0.9f;
		}
		else if (vip == 3)// MVP
		{
			scale = 0.8f;
		}

		vector<SelectableActionInfo>::type actionInfoList;
		for (auto iter : m_actionInfoMap)
		{
			SelectableActionInfo actionInfo;
			actionInfo = iter.second;
			actionInfo.price = actionInfo.price * scale;
			actionInfoList.push_back(actionInfo);
		}
		return actionInfoList;
	}

	vector<int>::type AppItemsSetting::getPurchasableActionIdList()
	{
		vector<int>::type actionIdList;
		for (auto iter : m_actionInfoMap)
		{
			actionIdList.push_back(iter.first);
		}
		return actionIdList;
	}

	void AppItemsSetting::setActionInfoList(vector<SelectableActionInfo>::type actionInfoList)
	{
		m_actionInfoMap.clear();
		SelectableActionInfo actionInfo;
		for (auto iter : actionInfoList)
		{
			actionInfo.actionId = iter.actionId;
			actionInfo.price = iter.price;
			actionInfo.currency = iter.currency;
			actionInfo.buyId = iter.buyId;
			m_actionInfoMap.insert(std::pair<int, SelectableActionInfo>(iter.actionId, actionInfo));
		}
	}

	int AppItemsSetting::getActionIdFromBuyId(int buyId)
	{
		int actionId = 0;
		for (auto iter : m_actionInfoMap)
		{
			if (iter.second.buyId == buyId)
			{
				actionId = iter.second.actionId;
				break;
			}
		}
		return actionId;
	}
}