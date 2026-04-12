#ifndef __GUI_DEF_HEADER__
#define __GUI_DEF_HEADER__

#include "UI/GUICommonDef.h"

using namespace LORD;

namespace BLOCKMAN
{
	constexpr GUIWindowType GWT_ITEM_STACK = "ItemStack"_gwt;
	constexpr GUIWindowType GWT_ITEM_SLOT = "ItemSlot"_gwt;
	constexpr GUIWindowType GWT_SLOT_TABLE = "SlotTable"_gwt;
	constexpr GUIWindowType GWT_SCROLLABLE_INVENTORY = "ScrollableInventory"_gwt;
	constexpr GUIWindowType GWT_ENTITY_WINDOW = "EntityWindow"_gwt;
	constexpr GUIWindowType GWT_ACTOR_WINDOW = "ActorWindow"_gwt;
	constexpr GUIWindowType GWT_SHOP_GRID_VIEW = "ShopGridView"_gwt;
	constexpr GUIWindowType GWT_SHOP_GOODS = "ShopGoods"_gwt;
	constexpr GUIWindowType GWT_SUMMARY_ITEM = "SummaryItem"_gwt;
	constexpr GUIWindowType GWT_PLYAER_ITEM = "PlyaerItem"_gwt;
	constexpr GUIWindowType GWT_COMMON_TEAM_INFO = "CommonTeamInfo"_gwt;
	constexpr GUIWindowType GWT_APP_SHOP_ITEM = "AppShopItem"_gwt;
	constexpr GUIWindowType GWT_APP_SHOP_AD_ITEM = "AppShopAdItem"_gwt;
	constexpr GUIWindowType GWT_MANOR_HOUSE_TEMPLATE_ITEM = "ManorHouseTemplateItem"_gwt;
	constexpr GUIWindowType GWT_MANOR_MESSAGE_ITEM = "ManorMessageItem"_gwt;
	constexpr GUIWindowType GWT_MANOR_FURNITURE_ITEM = "ManorFurnitureItem"_gwt;
	constexpr GUIWindowType GWT_SWITCHABLE_PROP_ITEM = "SwitchablePropItem"_gwt;
	constexpr GUIWindowType GWT_UPGRADE_PROP_ITEM = "UpgradePropItem"_gwt;
	constexpr GUIWindowType GWT_BUILDWAR_GUESS_ITEM = "BuildWarGuessItem"_gwt;
	constexpr GUIWindowType GWT_TEAM_RESOURCES_ITEM = "TeamResourcesItem"_gwt;
	constexpr GUIWindowType GWT_ENCHANTMENT_PROP_ITEM = "EnchantmentPropItem"_gwt;
	constexpr GUIWindowType GWT_SUPER_PROP_ITEM = "SuperPropItem"_gwt;
	constexpr GUIWindowType GWT_KILL_MSG_ITEM = "KillMsgItem"_gwt;
	constexpr GUIWindowType GWT_CUSTOM_PROP_ITEM = "CustomPropItem"_gwt;
	constexpr GUIWindowType GWT_CAR_PROGRESS = "CarProgress"_gwt;
	constexpr GUIWindowType GWT_RANCH_FUNCTION_ITEM = "RanchFunctionItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_ITEM = "RanchItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_RANK_ITEM = "RanchRankItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_RANK_TAB = "RanchRankTab"_gwt;
	constexpr GUIWindowType GWT_RANCH_COMMON_ITEM = "RanchCommonItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_STORAGE_ITEM = "RanchStorageItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_BUILD_ITEM = "RanchBuildItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_BUILD_SHORTCUT = "RanchBuildShortcut"_gwt;
	constexpr GUIWindowType GWT_RANCH_SELL_ITEM = "RanchSellItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_COMMUNITY_ITEM = "RanchCommunityItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_ORDER_ITEM = "RanchOrderItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_ORDER_OPERATE = "RanchOrderOperate"_gwt;
	constexpr GUIWindowType GWT_RANCH_FRIEND_OPERATE = "RanchFriendOperate"_gwt;
	constexpr GUIWindowType GWT_RANCH_FRIEND_HELP_ITEM = "RanchFriendHelpItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_MAIL_ITEM = "RanchMailItem"_gwt;
	constexpr GUIWindowType GWT_RANCH_ACHIEVEMENT_ITEM = "RanchAchievementItem"_gwt;
	constexpr GUIWindowType GWT_URL_IMAGE = "UrlImage"_gwt;
	constexpr GUIWindowType GWT_EFFECT_VIEW_OF_OBTAINMENT = "EffectViewOfObtainment"_gwt;
	constexpr GUIWindowType GWT_RANCH_BUILD_RECIPES = "RanchBuildRecipes"_gwt;
	constexpr GUIWindowType GWT_RANCH_BUILD_PRODUCT = "RanchBuildProduct"_gwt;
	constexpr GUIWindowType GWT_RANCH_BUILD_OPERATION = "RanchBuildOperation"_gwt;
	constexpr GUIWindowType GWT_RANCH_BUILD_FACTORY_QUEUE = "RanchBuildFactoryQueue"_gwt;
	constexpr GUIWindowType GWT_RANCH_BUILD_FARM_QUEUE = "RanchBuildFarmQueue"_gwt;
	constexpr GUIWindowType GWT_GUN_STORE_TAB = "GunStoreTab"_gwt;
	constexpr GUIWindowType GWT_STORE_GUN_ITEM = "StoreGunItem"_gwt;
	constexpr GUIWindowType GWT_STORE_PROP_ITEM = "StorePropItem"_gwt;
	constexpr GUIWindowType GWT_STORE_PROPERTY_ITEM = "StorePropertyItem"_gwt;
	constexpr GUIWindowType GWT_PIXEL_GUNHALL_MODE_SELECT_MAP_ITEM = "PixelGunHallModeSelectMapItem"_gwt;
	constexpr GUIWindowType GWT_STORE_ATTRIBUTE_ITEM = "StoreAttributeItem"_gwt;
	constexpr GUIWindowType GWT_GUN_STORE_LEVEL = "GunStoreLevel"_gwt;
	constexpr GUIWindowType GWT_CHEST_LOTTERY_ITEM = "ChestLotteryItem"_gwt;
	constexpr GUIWindowType GWT_CHEST_REWARD_ITEM = "ChestRewardItem"_gwt;
	constexpr GUIWindowType GWT_SEASON_HONOR_ITEM = "SeasonHonorItem"_gwt;
	constexpr GUIWindowType GWT_SEASON_RANK_ITEM = "SeasonRankItem"_gwt;
	constexpr GUIWindowType GWT_SEASON_REWARD_ITEM = "SeasonRewardItem"_gwt;
	constexpr GUIWindowType GWT_SEASON_RANK_PANEL = "SeasonRankPanel"_gwt;
	constexpr GUIWindowType GWT_BEDWAR_SEASON_HONOR_ITEM = "BedWarSeasonHonorItem"_gwt;
	constexpr GUIWindowType GWT_BEDWAR_SEASON_RANK_ITEM = "BedWarSeasonRankItem"_gwt;
	constexpr GUIWindowType GWT_BEDWAR_SEASON_REWARD_ITEM = "BedWarSeasonRewardItem"_gwt;
	constexpr GUIWindowType GWT_BEDWAR_SEASON_RANK_PANEL = "BedWarSeasonRankPanel"_gwt;
	constexpr GUIWindowType GWT_BIRD_PACK = "BirdPack"_gwt;
	constexpr GUIWindowType GWT_BIRD_FUSE = "BirdFuse"_gwt;
	constexpr GUIWindowType GWT_BIRD_TOOL = "BirdTool"_gwt;
	constexpr GUIWindowType GWT_BIRD_PACK_AND_FUSE_ITEM = "BirdPackAndFuseItem"_gwt;
	constexpr GUIWindowType GWT_BIRD_DRESS_ITEM = "BirdDressItem"_gwt;
	constexpr GUIWindowType GWT_BIRD_FEED_ITEM = "BirdFeedItem"_gwt;
	constexpr GUIWindowType GWT_BIRD_PERSONAL_SHOP_ITEM = "BirdPersonalShopItem"_gwt;
	constexpr GUIWindowType GWT_BIRD_LOTTERY_ITEM = "BirdLotteryItem"_gwt;
	constexpr GUIWindowType GWT_BIRD_ATLAS_ITEM = "BirdAtlasItem"_gwt;
	constexpr GUIWindowType GWT_BIRD_TASK_ITEM = "BirdTaskItem"_gwt;
	constexpr GUIWindowType GWT_BIRD_ACTIVITY_ITEM = "BirdActivityItem"_gwt;
	constexpr GUIWindowType GWT_CHECKIN_ITEM = "BirdCheckinItem"_gwt;
	constexpr GUIWindowType GWT_PIXEL_GUN_ACTIVE_CHEST = "PixelGunActiveChest"_gwt;
	constexpr GUIWindowType GWT_TASK_MAINLINE_TAB = "TaskMainLineTab"_gwt;
	constexpr GUIWindowType GWT_TASK_PANEL = "TaskPanel"_gwt;
	constexpr GUIWindowType GWT_TASK_INFO_ITEM = "TaskInfoItem"_gwt;
	constexpr GUIWindowType GWT_TASK_REWARD_ITEM = "TaskRewardItem"_gwt;
	constexpr GUIWindowType GWT_SELECTABLE_ACTION_ITEM = "SelectableActionItem"_gwt;
	constexpr GUIWindowType GWT_BED_WAR_RUNE_TYPE_ITEM = "BedWarRuneTypeItem"_gwt;
	constexpr GUIWindowType GWT_BED_WAR_RUNE_ITEM = "BedWarRuneItem"_gwt;
	constexpr GUIWindowType GWT_BED_WAR_RUNE_PRO_ITEM = "BedWarRuneProItem"_gwt;
	constexpr GUIWindowType GWT_BED_WAR_RECLAIM_RUNE_ITEM = "BedWarReclaimRuneItem"_gwt;
	constexpr GUIWindowType GWT_PIXEL_GUN_SIGN_IN_ITEM = "GUIPixelGunSignInItem"_gwt;
	constexpr GUIWindowType GWT_PIXEL_GUN_GIFT_BAG_ITEM = "PixelGunGiftBagItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_TIGER_LOTTERY_ITEM = "BlockCityTigerLotteryItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_FRIEND_ITEM = "BlockCityFriendItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_RANK_ITEM = "BlockCityRankItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_CHOOSE_PAPER_ITEM = "BlockCityChoosePaperItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_BUY_FLY_ITEM = "BlockCityBuyFlyItem"_gwt;
	constexpr GUIWindowType GWT_SKYBLOCK_TASK_REWARD_ITEM = "SkyBlockTaskRewardItem"_gwt;
	constexpr GUIWindowType GWT_SKYBLOCK_TASK_INFO_ITEM = "SkyBlockTaskInfoItem"_gwt;
	constexpr GUIWindowType GWT_SKYBLOCK_TASK_MAINLINE_TAB = "SkyBlockTaskMainLineTab"_gwt;
	constexpr GUIWindowType GWT_SKYBLOCK_TASK_PANEL = "SkyBlockTaskPanel"_gwt;
	constexpr GUIWindowType GWT_GUN_STORE_MODULE_PANEL = "GunStoreModulePanel"_gwt;
	constexpr GUIWindowType GWT_GUN_STORE_MODULE_ITEM = "GunStoreModuleItem"_gwt;
	constexpr GUIWindowType GWT_SKYBLOCK_APP_SHOP_ITEM = "SkyBlockAppShopItem"_gwt;
	constexpr GUIWindowType GWT_SKYBLOCK_RANK_ITEM = "SkyBlockRankItem"_gwt;
	constexpr GUIWindowType GWT_DESIGNATION_ITEM = "DesignationItem"_gwt;
	constexpr GUIWindowType GWT_WALKING_DEAD_STORE_TAB = "WalkingDeadStoreTab"_gwt;
	constexpr GUIWindowType GWT_WALKING_DEAD_STORE_ITEM = "WalkingDeadStoreItem"_gwt;
	constexpr GUIWindowType GWT_WALKING_DEAD_SUPPLY_ITEM = "WalkingDeadSupplyItem"_gwt;
	constexpr GUIWindowType GWT_WALKING_DEAD_STORE_SUPPLY_ITEM = "WalkingDeadStoreSupplyItem"_gwt;
	constexpr GUIWindowType GWT_GUI_COMMON_TAB_GRID_VIEW = "CommonTabGridView"_gwt;
	constexpr GUIWindowType GWT_BLOCKFORT_ITEM_INFO = "BlockFortItemInfo"_gwt;
	constexpr GUIWindowType GWT_GUI_BLOCK_FORT_STORE_ITEM_VIEW = "BlockFortStoreItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKFORT_MANOR_UPGRADE_ITEM = "BlockFortManorUpgradeItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_ELEVATOR_ITEM = "BlockCityElevatorItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_CHARM_RANK_ITEM = "BlockCityCharmRankItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_RECEIVE_FLOWER_ITEM = "BlockCityReceiveFlowerItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_SAVE_FILE_ITEM = "BlockCitySaveFileItem"_gwt;
	constexpr GUIWindowType GWT_BLOCKCITY_PORTAL_ITEM = "BlockCityPortalItem"_gwt;
	constexpr GUIWindowType GWT_SKYBLOCK_PRIVILEGE_SHOP_ITEM = "SkyBlockPrivilegeShopItem"_gwt;
	constexpr GUIWindowType GWT_SKYBLOCK_PRIVILEGE_REWARD_ITEM = "SkyBlockPrivilegeRewardItem"_gwt;
	constexpr GUIWindowType GWT_SKYBLOCK_SIGN_IN_ITEM = "SkyBlockSignInItem"_gwt;


	constexpr GUIWindowProperty GWP_TABLE_ROW_COUNT = "RowCount"_gwp;
	constexpr GUIWindowProperty GWP_TABLE_COLUMN_COUNT = "ColumnCount"_gwp;
	constexpr GUIWindowProperty GWP_TABLE_HORIZONTAL_INTERVAL = "HorizontalInterval"_gwp;
	constexpr GUIWindowProperty GWP_TABLE_VERTICAL_INTERVAL = "VerticalInterval"_gwp;
	constexpr GUIWindowProperty GWP_TABLE_AUTO_COLUMN_COUNT = "AutoColumnCount"_gwp;
	constexpr GUIWindowProperty GWP_TABLE_AUTO_SIZE = "TableAutoSize"_gwp;
	constexpr GUIWindowProperty GWP_SLOT_WIDTH = "SlotWidth"_gwp;
	constexpr GUIWindowProperty GWP_SLOT_HEIGHT = "SlotHeight"_gwp;
	constexpr GUIWindowProperty GWP_SLOT_STYLE = "SlotStyle"_gwp;
	constexpr GUIWindowProperty GWP_ITEM_STACK_BACKGROUND_COLOR = "ItemStackBackgroundColor"_gwp;
	constexpr GUIWindowProperty GWP_ITEM_STACK_NUMBER_VISIBILITY = "NumberVisibility"_gwp;
	constexpr GUIWindowProperty GWP_ENTITY_WINDOW_PITCH = "EntityWindowPitch"_gwp;
	constexpr GUIWindowProperty GWP_ENTITY_WINDOW_YAW = "EntityWindowYaw"_gwp;
	constexpr GUIWindowProperty GWP_ENTITY_WINDOW_ROLL = "EntityWindowRoll"_gwp;
	constexpr GUIWindowProperty GWP_ENTITY_WINDOW_SCALE = "EntityWindowScale"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_WINDOW_ACTORNAME = "ActorWindowActorName"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_WINDOW_SKILLNAME = "ActorWindowSkillName"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_WINDOW_ROTATEX = "ActorWindowRotateX"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_WINDOW_ROTATEY = "ActorWindowRotateY"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_WINDOW_ROTATEZ = "ActorWindowRotateZ"_gwp;
	constexpr GUIWindowProperty GWP_ACTOR_WINDOW_SCALE = "ActorWindowScale"_gwp;

	constexpr GUIWindowProperty GWP_GRID_VIEW_CONFIG = "GridViewConfig"_gwp;
	constexpr GUIWindowProperty GWP_GRID_VIEW_AREA = "GridViewArea"_gwp;
	constexpr GUIWindowProperty GWP_TABS_AREA = "TabsArea"_gwp;
	

	bool registerCustomUiWidget();
	// bool unRegisterCustomUiWidget();
}

#endif // !__GUI_DEF_HEADER__

