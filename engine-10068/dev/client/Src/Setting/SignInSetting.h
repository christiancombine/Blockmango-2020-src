#pragma once
#ifndef __SIGN_IN_SETTING_HEADER__
#define __SIGN_IN_SETTING_HEADER__

#include "Core.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum ItemStatus
	{
		UN_RECEIVED,
		ABLE_RECEIVED,
		HAVE_RECEIVED
	};

	struct SignInItem
	{
		int id = 0;
		ItemStatus status = UN_RECEIVED;
		int side = 0;
		int order = 0;
		int rewardId = 0;
	};

	struct ItemSetting
	{
		int id = 0;
		int order = 0;
		String title = "";
		int side = 1;
		String background = "";
		String effect = "";
		String image = "";
		int num = 0;
		String numText = "";
		int rewardId = 0;
	};

	class SignInSetting
	{
	private:
		static map<int, ItemSetting*>::type m_item_setting_map;
		static map<int, SignInItem*>::type m_cur_sign_in_items;

	public:
		static void	loadSetting();
		static void unloadSetting();
		static map<int, SignInItem*>::type getShowItems(int startOrder,int nowOrder, ItemStatus type);
		static ItemSetting* getSettingByOrder(int order);
	};
}

#endif 
