#pragma once

#include "Core.h"
#include "Inventory/Wallet.h"
#include "Game/Ranch.h"
using namespace LORD;

namespace BLOCKMAN {

	class BirdScope;
	class EntityDecoration;

	class UICommon
	{

	public: 
		static void checkMoney(i32 price, i32 type, std::function<void(bool)> func);
		static void checkMoney(i32 price, CurrencyType type, std::function<void(bool)> func);
		static bool checkRanchItems(const std::vector<RanchCommon>& items);
		static bool checkRanchItems(const vector<RanchCommon>::type& items);
		static bool checkRanchStorageCapacity(const vector<RanchCommon>::type& items);
		static bool checkRanchLevel(i32 needLevel);
		static bool isRanchIconVisible();
		static bool checkOpenFlying();

		static vector<RanchCommon>::type getLackRanchItems(const std::vector<RanchCommon>& items);
		static vector<RanchCommon>::type getLackRanchItems(const vector<RanchCommon>::type& items);

		static bool checkBirdMiniTip(const BirdScope& scope);
		static bool checkBirdConvert();
		static String getBirdMiniTipText(const BirdScope& scope);

		static bool checkBlockCityMiniTip();

	public:

		static void onMiniTipClick();
		static void onBlockCityMiniTipClick();
		static void interactionWithDecoration(EntityDecoration* entity);
		static bool checkBlockCityFlyBtnShow();
		static bool checkBlockCityEditModeBtnShow();
		static bool checkBlockCityIsInManor();
		static bool checkBlockFortIsInManor();
		static ui64 checkBlockCityManorUserId();

	public:
		static bool checkPlayerParam();
		static bool checkWalletParam();
		static bool checkRanchParam();
		static bool checkBirdParam();
		static bool checkBirdBagParam();
		static bool checkBlockCityParam();
		static bool checkBlockFortParam();

		static bool checkItemPlaceInManor(Vector3 beginPos, Vector3 endPos);
		static bool checkRangeLimit(Vector3 beginPos, Vector3 endPos, Vector3 rangeBeginPos, Vector3 rangeEndPos);

	public:
		static i64 getOtherCurrency();
		static String getCurrencyIcon();
		static String getKeyIcon();
		static String getOtherCurrencyIcon();
		static String getPlatformGold();
		static String getCurrencyIconByType(i32 currencyType);
		static String getCurrencyIconByType(CurrencyType currencyType);
		static String timeFormat(i32 time);
		static String luckyBlockTimeFormat(i32 time);
		static String numberToString(i64 num);
	};

}
