#pragma once
#ifndef __PIXEL_GUN_GIFT_BAG_SETTING_HEADER__
#define __PIXEL_GUN_GIFT_BAG_SETTING_HEADER__

#include "Core.h"
#include "RewardSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum GiftBagType 
	{
		GIFT_TYPE_NONE = 0,
		GIFT_TYPE_NEW,
		GIFT_TYPE_GENERAL,
	};

	enum GiftBagStatus
	{
		GIFT_STATUS_UNBUY = 1,
		GIFT_STATUS_BOUGHT,
	};

	class GiftBag
	{
	public:
		int Id = 0;
		String Name = "";
		String Describe = "";
		GiftBagType Type = GIFT_TYPE_NONE;
		String Image = "";
		String BgImage = "";
		String TagIcon = "";
		int Price =  0;
		int Discount = 0;
		int MoneyType = 1;
		GiftBagStatus Status = GIFT_STATUS_UNBUY;
		String Items = "";
		int Weight = 0;
		int Sort = 0;
		bool HasChange = false;
	};

	class PixelGunGiftBagSetting
	{
	private:
		static map<int, GiftBag*>::type m_gift_bag_map;
		static list<GiftBag*>::type m_shown_gift_bag_list;
	public:
		static void	loadSetting();
		static void	unloadSetting();

		static GiftBag* findGiftBagById(int id);
		static GiftBag* findShownGiftBagById(int id);
		static list<Reward*>::type findGiftGoodsByGiftId(int id);
		static void removeShownGiftBagById(int id);
		static list<GiftBag*>::type parseUpdateGiftBagData(const String & data, bool isRefresh);
		static bool sortGiftFunc(GiftBag* gift1, GiftBag* gift2);
		static list<GiftBag*>::type sortGifts(list<GiftBag*>::type gifts);
		static void onGiftBought(int giftBagId);
		static int checkEnoughMoney(GiftBag* Gift);
	private:
		static void loadGiftBagSetting();
	};
}

#endif 
