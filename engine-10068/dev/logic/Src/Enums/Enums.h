#pragma once
namespace BLOCKMAN {

	enum class SkillType
	{
		NONE,
		DEFENSE,
		SPRINT,
		RELEASE_TOXIC
	};

	enum class ResultCode
	{
		UNKNOWN = -1,
		COMMON = 0,
		PAY_SUCCESS =1,
		ITEM_NOT_EXIST = 2,
		LACK_MONEY = 3,
		INVENTORY_FULL = 4,
		PAY_FAILURE = 5,
		CALL_MANOR = 6,
		SELL_MANOR_SUCCEED = 7,
		MANOR_BAN_OPERATION = 8,
		RANCH_ORDER_HELP_RESULT = 9,
		MAIL_RECEIVE_RESULT = 10,
		GIVE_GIFT = 11,
		RANCH_STORAGE_FULL = 12,
		BIRD_RECEIVE_TASK = 13,
		BIRD_BUY_PERMISSION = 14,
		PIXEL_GUN_STORE_LACK_TICKET = 15,
	};

	enum class PutFurnitureStatus
	{
		START,
		SURE,
		CANCEL,
		EDIT_START,
		EDIT_SURE,
		EDIT_CANCEL,
	};

	enum class BroadcastType {
		COMMON = 0,
		RANCH_ORDER_HELP,
		RANCH_ORDER_FINISH,
		RANCH_RANK_ENTER_GAME,
		NOTIFICATION,
		FRIEND_INVITE,
		PRAISE,
		RACE_INVITE = 9,
		RACE_RESULE,
		SKYBLOCK_PARTY_ALMOST_OVER = 104801,
		SKYBLOCK_PARTY_OVER = 104802,
		SKYBLOCK_FRIEND_UPDATE = 104803,
		LIFTING_CHAMPION_INFO = 105501,
	};

	enum class SupplyType {
		HEALTH = 1,
		DEFENSE,
		BULLET
	};

	enum class AdType
	{
		COMMON = 0,
		GOLD = 1,
		SHOP = 2,
		GAME = 3,
		TASK = 4,
		GIFT = 5,
		DAILY_KEY = 6,
		DAILY_GOLD = 7,
		DAILY_CUBE = 8,

		COMMON_DAILY_BCUBE = 100001, // common DAILY_CUBE

		TYCOON_MONEY = 102501, // tycoon money
		TYCOON_BUILDING = 102502, // tycoon building
		TYCOON_APP_SHOP = 102503, // tycoon app shop

		JAIL_BREAK_RESPAWN = 101401, // jail break respawn
		JAIL_BREAK_SHOP = 101402, // jail break shop
		JAIL_BREAK_UNLOCK_VEHICLE = 101403, // jail break unlock vehicle

		BIRD_SIMULATOR_MONEY = 104101,			// bird simulator money
		BIRD_SIMULATOR_EGG_TICKET = 104102,		// bird simulator egg ticket
		BIRD_SIMULATOR_CHEST_CD = 104103,		// bird simulator chest cd
		BIRD_SIMULATOR_VIP_CHEST = 104104,		// bird simulator vip chest
		BIRD_SIMULATOR_FEED = 104105,			// bird simulator feed

		BLOCK_CITY_TIGER = 104701,				// block city tiger lottery
		BLOCK_CITY_FLYING = 104702,				// block city flying

		PIXEL_GUN_TASK = 104201,
		PIXEL_FREE_CHEST = 104202,
		PIXEL_FREE_VIP = 104203,

		SKY_ROYALE_RESPAWN = 102701, //skyRoyale respawn

		HUNGRY_GAME_RESPAWN = 100101, //hungryGame respawn
	};

	// game invoke app interface action type
	enum AppActionType
	{
		APP_ACTION_SETTING = 1, // open setting page
		APP_ACTION_EMAIL = 2, // open email page
		APP_ACTION_FRIEND = 3, // open friend page
		APP_ACTION_ADD_CUBE = 4, // open add cube page
		APP_ACTION_FREE_CUBE = 5, // watch video ad got free cube
		APP_ACTION_DRESS = 6, // open dress shop page
		APP_ACTION_MORE_GAME = 7, // open more game page
		APP_ACTION_SCREEN_CAPTURE = 8, // screen capture share
		APP_ACTION_NOT_REMIND_FALSE = 9, //set NotShowDiamondBlueLackTips false
		APP_ACTION_NOT_REMIND_TRUE = 10, //set NotShowDiamondBlueLackTips true
		APP_ACTION_FREE_PROPS = 11, // open free props page
	};

	// app invoke game interface action type
	enum class GameActionType
	{
		GAME_ACTION_REFRESH_DRESS = 1,  // refresh dress
		GAME_ACTION_ENABLE_INDIE = 2, // enable indie
		GAME_ACTION_DISABLE_RECHARGE = 3, //disable recharge
		GAME_ACTION_SHOW_VIDEO_AD = 4, // show video ad
		GAME_ACTION_HIDE_VIDEO_AD = 5, // hide video ad
		GAME_ACTION_SHOW_EMAIL_RED = 6, // show email red point
		GAME_ACTION_HIDE_EMAIL_RED = 7, // hide email red point
		GAME_ACTION_SHOW_FRIEND_RED = 8, // show friend red point
		GAME_ACTION_HIDE_FRIEND_RED = 9, // hide friend red point
		GAME_ACTION_NOT_REMIND_FALSE = 10, //get NotShowDiamondBlueLackTips false
		GAME_ACTION_NOT_REMIND_TRUE = 11, //get NotShowDiamondBlueLackTips true
		GAME_ACTION_REFRESH_PROPS = 12, // refresh props
		GAME_ACTION_EXTRA_PARAMS = 13, // extra params
	};

	enum class RechargeType
	{
		RECHARGE_TYPE_CUBE = 1, // cube
		RECHARGE_TYPE_FLYING = 2, // flying
		RECHARGE_TYPE_BED_WAR_SP = 3, // bed war super player
	};
}