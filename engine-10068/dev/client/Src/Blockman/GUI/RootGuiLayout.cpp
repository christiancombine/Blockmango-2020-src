#include "RootGuiLayout.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "GUI/gui_mainControl.h"
#include "GUI/gui_playerInventoryControl.h"
#include "GUI/gui_loadingPage.h"
#include "GUI/gui_playerInfo.h"
#include "GUI/gui_gameTipDialog.h"
#include "GUI/gui_chest.h" 
#include "GUI/gui_chat.h"
#include "GUI/gui_menu.h"
#include "GUI/gui_toolBar.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/IInventory.h"
#include "Inventory/Container.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "Network/ClientNetwork.h"
#include "Entity/Enchantment.h"
#include "GUI/gui_deadSummary.h"
#include "GUI/gui_finalSummary.h"
#include "GUI/gui_shop.h"
#include "GUI/gui_summaryPoint.h"
#include "GUI/gui_countdown.h"
#include "GUI/gui_appShop.h"
#include "GUI/gui_summaryTip.h"
#include "GUI/gui_jailBreakRoleChoice.h"
#include "GUI/gui_tipDialog.h"
#include "GUI/gui_buyGoodsTip.h"
#include "GUI/gui_rank.h"
#include "GUI/gui_blockLoading.h"
#include "GUI/gui_miniMap.h"
#include "GUI/gui_manor.h"
#include "GUI/gui_manorUpgrade.h"
#include "GUI/gui_personalShop.h"
#include "GUI/gui_realTimeRank.h"
#include "GUI/gui_multiTipDialog.h"
#include "GUI/gui_buildwarGrade.h"
#include "GUI/gui_buildwarGuess.h"
#include "GUI/gui_buildwarGuessResult.h"
#include "GUI/gui_outputConfig.h"
#include "GUI/gui_superShop.h"
#include "GUI/gui_killPanel.h"
#include "GUI/gui_carProgressTable.h"
#include "GUI/gui_multiTipInteractionDialog.h"
#include "GUI/gui_ranch.h"
#include "GUI/gui_ranchUpgrade.h"
#include "GUI/gui_ranchOrder.h"
#include "GUI/gui_ranchTip.h"
#include "GUI/gui_ranchTimeTip.h"
#include "GUI/gui_ranchGainTip.h"
#include "GUI/gui_ranchRank.h"
#include "GUI/gui_ranchProsperityRank.h"
#include "GUI/gui_ranchBuildFactory.h"
#include "GUI/gui_ranchBuildFarm.h"
#include "GUI/gui_occupationUnlock.h"
#include "GUI/gui_enchantmentPanel.h"
#include "GUI/gui_tigerLottery.h"
#include "GUI/gui_HideAndSeekHallResult.h"
#include "GUI/gui_pixelGunHallModelSelect.h"
#include "GUI/gui_gunStore.h"
#include "GUI/gui_pixelgun1v1.h"
#include "GUI/gui_chestLottery.h"
#include "GUI/gui_lotteryChestDetail.h"
#include "GUI/gui_pixelgunrevive.h"
#include "GUI/gui_pixelGunHallArmorUpgrade.h"
#include "GUI/gui_pixelgungameresult.h"
#include "GUI/gui_seasonRank.h"
#include "GUI/gui_seasonReward.h"
#include "GUI/gui_bedwarSeasonRank.h"
#include "GUI/gui_bedwarSeasonReward.h"
#include "GUI/gui_pixelgungamelvup.h"
#include "GUI/gui_birdPackAndFuse.h"
#include "GUI/gui_birdPersonalShop.h"
#include "GUI/gui_birdStore.h"
#include "GUI/gui_birdLottery.h"
#include "GUI/gui_birdTip.h"
#include "GUI/gui_birdTaskTip.h"
#include "GUI/gui_birdAtlas.h"
#include "GUI/gui_birdGainTip.h"
#include "GUI/gui_birdActivity.h"
#include "cEntity/PlayerControlller.h"
#include "GUI/gui_checkin.h"
#include "GUI/gui_birdHelp.h"
#include "GUI/gui_notification.h"
#include "GUI/gui_supplyTip.h"
#include "GUI/gui_taskSystem.h"
#include "GUI/gui_bedWarRune.h"
#include "GUI/gui_guideSystem.h"
#include "GUI/gui_pixelGunGiftBagStore.h"
#include "GUI/gui_briefIntroduction.h"
#include "GUI/gui_pixelgunSignIn.h"
#include "GUI/gui_chestRewardDetail.h"
#include "GUI/gui_rewardResult.h"
#include "GUI/gui_pixelGunIndieMain.h"
#include "GUI/gui_blockCityTigerLottery.h"
#include "GUI/gui_blockCityManor.h"
#include "GUI/gui_blockCityTip.h"
#include "GUI/gui_blockCityStore.h"
#include "GUI/gui_blockCityFriend.h"
#include "GUI/gui_blockCityRank.h"
#include "GUI/gui_blockCityChoosePaper.h"
#include "GUI/gui_skyBlockTaskSystem.h"
#include "GUI/gui_tipMessage.h"
#include "GUI/gui_commonTip.h"
#include "GUI/gui_baseMessage.h"
#include "GUI/gui_levelUpPanel.h"
#include "GUI/gui_furnacePanel.h"
#include "GUI/gui_skyBlockAppShop.h"
#include "GUI/gui_skyBlockRank.h"
#include "GUI/gui_skyblock_productUpdate.h"
#include "GUI/gui_skyblockUnlockTip.h"
#include "GUI/gui_walkingDeadStore.h"
#include "GUI/GUIWalkingDeadStoreSupplyPopup.h"
#include "GUI/gui_designationPanelControl.h"

#include "GUI/gui_commonTip.h"
#include "GUI/gui_baseMessage.h"
#include "GUI/gui_levelUpPanel.h"
#include "GUI/gui_deathSettlementControl.h"
#include "GUI/gui_tycoonVedioAd.h"
#include "GUI/gui_videoAdTip.h"
#include "GUI/gui_blockFortTip.h"
#include "GUI/gui_blockFortBuildingInfo.h"
#include "GUI/gui_blockFortStore.h"
#include "GUI/gui_blockFortManorUpgrade.h"
#include "GUI/gui_blockFortPack.h"
#include "GUI/gui_blockCityPack.h"
#include "GUI/gui_blockCityHouseShop.h"
#include "GUI/gui_blockCityElevator.h"
#include "GUI/gui_blockCityReceiveFlower.h"
#include "GUI/gui_blockCityCharmRank.h"
#include "GUI/gui_blockCitySaveFile.h"
#include "GUI/gui_blockCityRankAward.h"
#include "GUI/gui_pixelChickenMain.h"
#include "GUI/gui_pixelChickenAward.h"
#include "GUI/gui_skyBlockPrivilegeShop.h"
#include "GUI/gui_skyBlockSignIn.h"
#include "GUI/gui_skyblockRewardDetail.h"
#include "GUI/gui_skyblockRewardResult.h"
#include "GUI/gui_blockCityBuyFlyTip.h"
#include "GUI/gui_blockCityPortal.h"
#include "GUI/gui_skyBlockReciveNewPri.h"
#include "Object/Root.h"
#include "ShellInterface.h"
#include "game.h"
#include "Setting/RewardSetting.h"
#include "Util/PlayerPrefs.h"
#include "Plugins/GameClientEvents.h"

namespace BLOCKMAN
{
	const RootGuiLayout::LayoutId RootGuiLayout::ALL_LAYOUT_ID[] = {
		LayoutId::MAIN,
		LayoutId::PLAYER_INVENTORY,
		LayoutId::LOADING_PAGE,
		LayoutId::PLAYER_INFO,
		LayoutId::GAME_TIP_DIALOG,
		LayoutId::CHEST,
		LayoutId::CHAT,
		LayoutId::MENU,
		LayoutId::TOOL_BAR,
		LayoutId::DEAD_SUMMARY,
		LayoutId::FINAL_SUMMARY,
		LayoutId::SHOP,
		LayoutId::SUMMARY_POINT,
		LayoutId::COUNTDOWN,
		LayoutId::APP_SHOP,
		LayoutId::SUMMARY_TIP,
		LayoutId::JAIL_BREAK_ROLE_CHOICE,
		LayoutId::TIP_DIALOG,
		LayoutId::BUY_GOODS_TIP,
		LayoutId::RANK,
		LayoutId::BLOCK_LOADING,
		LayoutId::MINI_MAP,
		LayoutId::MANOR,
		LayoutId::UPGRADE_MANOR,
		LayoutId::PERSONAL_SHOP,
		LayoutId::BUILD_WAR_GRADE,
		LayoutId::BUILD_WAR_GUESS,
		LayoutId::BUILD_WAR_GUESS_RESULT,
		LayoutId::MULTI_TIP_DIALOG,
		LayoutId::REAL_TIME_RANK,
		LayoutId::OUTPUT_CONFIG,
		LayoutId::SUPER_SHOP,
		LayoutId::KILL_PANEL,
		LayoutId::CAR_PROGRESS_TABLE,
		LayoutId::MULTI_TIP_INTERACTION_DIALOG,
		LayoutId::RANCH,
		LayoutId::RANCH_UPGRAGE,
		LayoutId::RANCH_ORDER,
		LayoutId::RANCH_TIP,
		LayoutId::RANCH_FACTORY_BUILD,
		LayoutId::RANCH_FARM_BUILD,
		LayoutId::RANCH_TIME_TIP,
		LayoutId::RANCH_GAIN_TIP,
		LayoutId::RANCH_RANK,
		LayoutId::RANCH_PROSPERITY_RANK,
		LayoutId::OCCUPATION_UNLOCK,
		LayoutId::ENCHANTMENT_PANEL,
		LayoutId::TIGER_LOTTERY,
		LayoutId::HIDEANDSEEKHALL_RESULT,
		LayoutId::PIXEL_GUNHALL_MODE_SELECT,
		LayoutId::GUN_STORE,
		LayoutId::PIXEL_GUN_1V1,
		LayoutId::CHEST_LOTTERY,
		LayoutId::PIXEL_GUN_REVIVE,
		LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE,
		LayoutId::PIXEL_GUN_RESULT,
		LayoutId::SEASON_RANK,
		LayoutId::SEASON_REWARD,
		LayoutId::LOTTERY_CHEST_DETAIL,
		LayoutId::PIXEL_GUN_GAME_LVUP,
		LayoutId::BIRD_PACKANDFUSE,
		LayoutId::BIRD_PERSONAL_SHOP,
		LayoutId::BIRD_STORE,
		LayoutId::BIRD_LOTTERY,
		LayoutId::BIRD_TIP,
		LayoutId::BIRD_TASK_TIP,
		LayoutId::BIRD_ATLAS,
		LayoutId::BIRD_GAIN_TIP,
		LayoutId::BIRD_ACTIVITY,
		LayoutId::CHECKIN,
		LayoutId::BIRD_HELP,
		LayoutId::NOTIFICATION,
		LayoutId::SUPPLY_TIP,
		LayoutId::TASK_SYSTEM,
		LayoutId::BEDWAR_SEASON_RANK,
		LayoutId::BEDWAR_SEASON_REWARD,
		LayoutId::BEDWAR_RUNE,
		LayoutId::GUIDE_SYSTEM,
		LayoutId::BRIEF_INTRODUCTION,
		LayoutId::PIXEL_GUN_SIGN_IN,
		LayoutId::CHEST_REWARD_DETAIL,
		LayoutId::CHEST_REWARD_RESULT,
		LayoutId::PIXEL_GUN_GIFT_BAG_STORE,
		LayoutId::PIXEL_GUN_INDIE_MAIN,
		LayoutId::BLOCK_CITY_TIGER_LOTTERY,
		LayoutId::BLOCK_CITY_MANOR,
		LayoutId::BLOCK_CITY_TIP,
		LayoutId::BLOCK_CITY_STORE,
		LayoutId::BLOCK_CITY_FRIEND,
		LayoutId::BLOCK_CITY_RANK,
		LayoutId::BLOCK_CITY_CHOOSE_PAPER,
		LayoutId::SKYBLOCK_TASK_SYSTEM,
		LayoutId::SKYBLOCK_TIP,
		LayoutId::COMMON_TIP,
		LayoutId::BASE_MESSAGE,
		LayoutId::LEVEL_UP_PANEL,
		LayoutId::FURNACE_PANEL,
		LayoutId::SKYBLOCK_APP_SHOP,
		LayoutId::SKYBLOCK_RANK,
		LayoutId::UPDATE_PRODUCT_INFO,
		LayoutId::UNLOCK_PRODUCT_INFO,
		LayoutId::TYCOON_VEDIO_AD,
		LayoutId::WALKINGDEAD_STORE,
		LayoutId::WALKINGDEAD_DEATHSETTLEMENT,
		LayoutId::WALKINGDEAD_SupplyPopup,
		LayoutId::BLOCK_FORT_TIP,
		LayoutId::BLOCK_FORT_BUILDING_INFO,
		LayoutId::BLOCK_FORT_STORE,
		LayoutId::BLOCK_FORT_MANOR_UPGRADE,
		LayoutId::BLOCK_FORT_PACK,
		LayoutId::BLOCK_CITY_PACK,
		LayoutId::BLOCK_CITY_HOUSE_SHOP,
		LayoutId::BLOCK_CITY_ELEVATOR,
		LayoutId::BLOCK_CITY_RECEIVE_FLOWER,
		LayoutId::BLOCK_CITY_CHARM_RANK,
		LayoutId::BLOCK_CITY_SAVE_FILE,
		LayoutId::BLOCK_CITY_RANK_AWARD,
		LayoutId::VIDEO_AD_TIP,
		LayoutId::PIXEL_CHICKEN_MAIN,
		LayoutId::PIXEL_CHICKEN_AWARD,
		LayoutId::SKYBLOCK_PRIVILEGE_SHOP,
		LayoutId::SKYBLOCK_SIGN_IN,
		LayoutId::SKYBLOCK_REWARD_DETAIL,
		LayoutId::SKYBLOCK_REWARD_RESULT,
		LayoutId::BLOCK_CITY_BUY_FLY,
		LayoutId::BLOCK_CITY_PORTAL,
		LayoutId::SKYBLOCK_REVIVE_NEW_PRI,
	};

	RootGuiLayout::~RootGuiLayout()
	{
		LordSafeDelete(m_stItemName);
        
		m_subscriptionGuard.unsubscribeAll();
		for (auto it : m_layouts) {
		    LordSafeDelete(it.second);
		}
		m_layouts.clear();
		RewardSetting::unloadSetting();
	}

	void RootGuiLayout::hideAllChildren()
	{
		for (auto id : ALL_LAYOUT_ID)
		{
			m_layoutShown[id] = false;
		}
		SCRIPT_EVENT::HideAllLayoutEvent::invoke(true);
	}

	void RootGuiLayout::recoverAllChildren()
	{
		SCRIPT_EVENT::RecoverAllLayoutEvent::invoke(true);
	}

	bool RootGuiLayout::openCraftingTable()
	{
		bool isShowCrafting = PlayerPrefs::Instance()->getBoolPrefs("IsShowCrafting");
		if (isShowCrafting)
		{
			dynamic_cast<gui_playerInventoryControl*>(m_layouts[LayoutId::PLAYER_INVENTORY])->setInCraftingTable(true);
			showPlayerInventory();
		}

		return true;
	}

	void RootGuiLayout::checkGunUI()
	{
		if (m_layoutShown[LayoutId::MAIN] && dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]))
			dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN])->CheckGunUI();
	}

	void RootGuiLayout::armorPanelSellItem()
	{
		if (dynamic_cast<gui_playerInventoryControl*>(m_layouts[LayoutId::PLAYER_INVENTORY]))
			return dynamic_cast<gui_playerInventoryControl*>(m_layouts[LayoutId::PLAYER_INVENTORY])->armorPanelSellItem();
	}

	void RootGuiLayout::againDropItem(int index)
	{
		if (dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]))
			return dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN])->onAgainDropItem(index);
	}

	bool RootGuiLayout::openFurnaceTable(IInventory* inventory, const Vector3i & blockPos, int face, const Vector3 & hisPos)
	{
		hideAllChildren();
		m_layoutShown[LayoutId::FURNACE_PANEL] = true;

		dynamic_cast<gui_furnacePanel*>(m_layouts[LayoutId::FURNACE_PANEL])->setInventory(inventory, blockPos, face, hisPos);

		return true;
	}

	void RootGuiLayout::setFurnaceTime(int time, int maxTime)
	{
		dynamic_cast<gui_furnacePanel*>(m_layouts[LayoutId::FURNACE_PANEL])->setBurnTime(time, maxTime);
	}

	bool RootGuiLayout::setFurnaceInventory(IInventory * inventory)
	{
		dynamic_cast<gui_furnacePanel*>(m_layouts[LayoutId::FURNACE_PANEL])->setInventory(inventory);
		return true;
	}

	void RootGuiLayout::onLoad()
	{
		hideAllChildren();
		m_layouts[LayoutId::MAIN] = LordNew gui_mainControl;
		m_layouts[LayoutId::PLAYER_INVENTORY] = LordNew gui_playerInventoryControl;
		m_layouts[LayoutId::LOADING_PAGE] = LordNew gui_loadingPage;
		m_layouts[LayoutId::PLAYER_INFO] = LordNew gui_playerInfo;
		m_layouts[LayoutId::GAME_TIP_DIALOG] = LordNew gui_gameTipDialog;
		m_layouts[LayoutId::CHEST] = LordNew gui_chest;
		m_layouts[LayoutId::CHAT] = LordNew gui_chat;
		m_layouts[LayoutId::MENU] = LordNew gui_menu;
		m_layouts[LayoutId::TOOL_BAR] = LordNew gui_toolBar;
		m_layouts[LayoutId::DEAD_SUMMARY] = LordNew gui_deadSummary;
		m_layouts[LayoutId::FINAL_SUMMARY] = LordNew gui_finalSummary;
		m_layouts[LayoutId::SHOP] = LordNew gui_shop;
		m_layouts[LayoutId::SUMMARY_POINT] = LordNew gui_summaryPoint;
		m_layouts[LayoutId::COUNTDOWN] = LordNew gui_countdown;
		m_layouts[LayoutId::APP_SHOP] = LordNew gui_appShop;
		m_layouts[LayoutId::WALKINGDEAD_STORE] = LordNew gui_walkingDeadStore;
		m_layouts[LayoutId::WALKINGDEAD_SupplyPopup] = LordNew GUIWalkingDeadStoreSupplyPopup;

		m_layouts[LayoutId::SUMMARY_TIP] = LordNew gui_summaryTip;
		m_layouts[LayoutId::JAIL_BREAK_ROLE_CHOICE] = LordNew gui_jailBreakRoleChoice;
		m_layouts[LayoutId::RANK] = LordNew gui_rank;
		m_layouts[LayoutId::MINI_MAP] = LordNew gui_miniMap;
		m_layouts[LayoutId::TIP_DIALOG] = LordNew gui_tipDialog;
		m_layouts[LayoutId::BUY_GOODS_TIP] = LordNew gui_buyGoodsTip;
		m_layouts[LayoutId::BLOCK_LOADING] = LordNew gui_blockLoading;
		m_layouts[LayoutId::MANOR] = LordNew gui_manor;
		m_layouts[LayoutId::UPGRADE_MANOR] = LordNew gui_manorUpgrade;
		m_layouts[LayoutId::PERSONAL_SHOP] = LordNew gui_personalShop;
		m_layouts[LayoutId::REAL_TIME_RANK] = LordNew gui_realTimeRank;
		m_layouts[LayoutId::BUILD_WAR_GRADE] = LordNew gui_buildwarGrade;
		m_layouts[LayoutId::BUILD_WAR_GUESS] = LordNew gui_buildwarGuess;
		m_layouts[LayoutId::BUILD_WAR_GUESS_RESULT] = LordNew gui_buildwarGuessResult;
		m_layouts[LayoutId::MULTI_TIP_DIALOG] = LordNew gui_multiTipDialog;
		m_layouts[LayoutId::MULTI_TIP_INTERACTION_DIALOG] = LordNew gui_multiTipInteractionDialog;
		m_layouts[LayoutId::OUTPUT_CONFIG] = LordNew gui_outputConfig;
		m_layouts[LayoutId::SUPER_SHOP] = LordNew gui_superShop;
		m_layouts[LayoutId::KILL_PANEL] = LordNew gui_killPanel;
		m_layouts[LayoutId::CAR_PROGRESS_TABLE] = LordNew gui_carProgressTable;
		m_layouts[LayoutId::RANCH] = LordNew gui_ranch;
		m_layouts[LayoutId::RANCH_ORDER] = LordNew gui_ranchOrder;
		m_layouts[LayoutId::RANCH_UPGRAGE] = LordNew gui_ranchUpgrade;
		m_layouts[LayoutId::RANCH_TIP] = LordNew gui_ranchTip;
		m_layouts[LayoutId::RANCH_FACTORY_BUILD] = LordNew gui_ranchBuildFactory;
		m_layouts[LayoutId::RANCH_FARM_BUILD] = LordNew gui_ranchBuildFarm;
		m_layouts[LayoutId::RANCH_TIME_TIP] = LordNew gui_ranchTimeTip;
		m_layouts[LayoutId::RANCH_GAIN_TIP] = LordNew gui_ranchGainTip;
		m_layouts[LayoutId::RANCH_RANK] = LordNew gui_ranchRank;
		m_layouts[LayoutId::RANCH_PROSPERITY_RANK] = LordNew gui_ranchProsperityRank;
		m_layouts[LayoutId::OCCUPATION_UNLOCK] = LordNew gui_occupationUnlock;
		m_layouts[LayoutId::ENCHANTMENT_PANEL] = LordNew gui_enchantmentPanel;
		m_layouts[LayoutId::TIGER_LOTTERY] = LordNew gui_tigerLottery;
		m_layouts[LayoutId::HIDEANDSEEKHALL_RESULT] = LordNew gui_hideAndSeekHallResult;
		m_layouts[LayoutId::PIXEL_GUNHALL_MODE_SELECT] = LordNew gui_pixelGunHallModelSelect;
		m_layouts[LayoutId::GUN_STORE] = LordNew gui_gunStore;
		m_layouts[LayoutId::PIXEL_GUN_1V1] = LordNew gui_pixelgun1v1;
		m_layouts[LayoutId::CHEST_LOTTERY] = LordNew gui_chestLottery;
		m_layouts[LayoutId::PIXEL_GUN_REVIVE] = LordNew gui_pixelgunrevive;
		m_layouts[LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE] = LordNew gui_pixelGunHallArmorUpgrade;
		m_layouts[LayoutId::PIXEL_GUN_RESULT] = LordNew gui_pixelgungameresult;
		m_layouts[LayoutId::SEASON_RANK] = LordNew gui_seasonRank;
		m_layouts[LayoutId::SEASON_REWARD] = LordNew gui_seasonReward;
		m_layouts[LayoutId::BEDWAR_SEASON_RANK] = LordNew gui_bedwarSeasonRank;
		m_layouts[LayoutId::BEDWAR_SEASON_REWARD] = LordNew gui_bedwarSeasonReward;
		m_layouts[LayoutId::LOTTERY_CHEST_DETAIL] = LordNew gui_lotteryChestDetail;
		m_layouts[LayoutId::PIXEL_GUN_GAME_LVUP] = LordNew gui_pixelgungamelvup;
		m_layouts[LayoutId::BIRD_PACKANDFUSE] = LordNew gui_birdPackAndFuse;
		m_layouts[LayoutId::BIRD_PERSONAL_SHOP] = LordNew gui_birdPersonalShop;
		m_layouts[LayoutId::BIRD_STORE] = LordNew gui_birdStore;
		m_layouts[LayoutId::BIRD_LOTTERY] = LordNew gui_birdLottery;
		m_layouts[LayoutId::BIRD_TIP] = LordNew gui_birdTip;
		m_layouts[LayoutId::BIRD_TASK_TIP] = LordNew gui_birdTaskTip;
		m_layouts[LayoutId::BIRD_ATLAS] = LordNew gui_birdAtlas;
		m_layouts[LayoutId::BIRD_GAIN_TIP] = LordNew gui_birdGainTip;
		m_layouts[LayoutId::BIRD_ACTIVITY] = LordNew gui_birdActivity;
		m_layouts[LayoutId::CHECKIN] = LordNew gui_checkin;
		m_layouts[LayoutId::BIRD_HELP] = LordNew gui_birdHelp;
		m_layouts[LayoutId::NOTIFICATION] = LordNew gui_notification;
		m_layouts[LayoutId::SUPPLY_TIP] = LordNew gui_supplyTip;
		m_layouts[LayoutId::TASK_SYSTEM] = LordNew gui_taskSystem;
		m_layouts[LayoutId::BEDWAR_RUNE] = LordNew gui_bedwar_rune;
		m_layouts[LayoutId::GUIDE_SYSTEM] = LordNew gui_guideSystem;
		m_layouts[LayoutId::BRIEF_INTRODUCTION] = LordNew gui_briefIntroduction;
		m_layouts[LayoutId::PIXEL_GUN_SIGN_IN] = LordNew gui_pixelgunSignIn;
		m_layouts[LayoutId::PIXEL_GUN_GIFT_BAG_STORE] = LordNew gui_pixelGunGiftBagStore;
		m_layouts[LayoutId::CHEST_REWARD_DETAIL] = LordNew gui_chestRewardDetail;
		m_layouts[LayoutId::CHEST_REWARD_RESULT] = LordNew gui_rewardResult;
		m_layouts[LayoutId::PIXEL_GUN_INDIE_MAIN] = LordNew gui_pixelGunIndieMain;
		m_layouts[LayoutId::BLOCK_CITY_TIGER_LOTTERY] = LordNew gui_blockCityTigerLottery;
		m_layouts[LayoutId::BLOCK_CITY_MANOR] = LordNew gui_blockCityManor;
		m_layouts[LayoutId::BLOCK_CITY_TIP] = LordNew gui_blockCityTip;
		m_layouts[LayoutId::BLOCK_CITY_STORE] = LordNew gui_blockCityStore;
		m_layouts[LayoutId::BLOCK_CITY_FRIEND] = LordNew gui_blockCityFriend;
		m_layouts[LayoutId::BLOCK_CITY_RANK] = LordNew gui_blockCityRank;
		m_layouts[LayoutId::BLOCK_CITY_CHOOSE_PAPER] = LordNew gui_blockCityChoosePaper;
		m_layouts[LayoutId::SKYBLOCK_TASK_SYSTEM] = LordNew gui_skyBlockTaskSystem;
		m_layouts[LayoutId::SKYBLOCK_TIP] = LordNew gui_tipMessage;
		m_layouts[LayoutId::COMMON_TIP] = LordNew gui_commonTip;
		m_layouts[LayoutId::BASE_MESSAGE] = LordNew gui_baseMessage;
		m_layouts[LayoutId::LEVEL_UP_PANEL] = LordNew gui_levelUpPanel;
		m_layouts[LayoutId::FURNACE_PANEL] = LordNew gui_furnacePanel;
		m_layouts[LayoutId::SKYBLOCK_APP_SHOP] = LordNew gui_skyBlockAppShop;
		m_layouts[LayoutId::SKYBLOCK_RANK] = LordNew gui_skyBlockRank;
		m_layouts[LayoutId::UPDATE_PRODUCT_INFO] = LordNew gui_skyblock_productUpdate;
		m_layouts[LayoutId::UNLOCK_PRODUCT_INFO] = LordNew gui_skyblockUnlockTip;

		m_layouts[LayoutId::TYCOON_VEDIO_AD] = LordNew gui_tycoonVedioAd;
		m_layouts[LayoutId::WALKINGDEAD_DEATHSETTLEMENT] = LordNew gui_deathSettlementControl;
		m_layouts[LayoutId::VIDEO_AD_TIP] = LordNew gui_videoAdTip;
		m_layouts[LayoutId::PIXEL_CHICKEN_MAIN] = LordNew gui_pixelChickenMain;
		m_layouts[LayoutId::PIXEL_CHICKEN_AWARD] = LordNew gui_pixelChickenAward;

		m_layouts[LayoutId::BLOCK_FORT_TIP] = LordNew gui_blockFortTip;
		m_layouts[LayoutId::BLOCK_FORT_BUILDING_INFO] = LordNew gui_blockFortBuildingInfo;
		m_layouts[LayoutId::BLOCK_FORT_STORE] = LordNew gui_blockFortStore;
		m_layouts[LayoutId::BLOCK_FORT_MANOR_UPGRADE] = LordNew gui_blockFortManorUpgrade;
		m_layouts[LayoutId::BLOCK_FORT_PACK] = LordNew gui_blockFortPack;
		m_layouts[LayoutId::BLOCK_CITY_PACK] = LordNew gui_blockCityPack;
		m_layouts[LayoutId::BLOCK_CITY_HOUSE_SHOP] = LordNew gui_blockCityHouseShop;
		m_layouts[LayoutId::BLOCK_CITY_ELEVATOR] = LordNew gui_blockCityElevator;
		m_layouts[LayoutId::BLOCK_CITY_RECEIVE_FLOWER] = LordNew gui_blockCityReceiveFlower;
		m_layouts[LayoutId::BLOCK_CITY_CHARM_RANK] = LordNew gui_blockCityCharmRank;
		m_layouts[LayoutId::BLOCK_CITY_SAVE_FILE] = LordNew gui_blockCitySaveFile;
		m_layouts[LayoutId::BLOCK_CITY_RANK_AWARD] = LordNew gui_blockCityRankAward;
		m_layouts[LayoutId::SKYBLOCK_PRIVILEGE_SHOP] = LordNew gui_skyBlockPrivilegeShop;
		m_layouts[LayoutId::SKYBLOCK_SIGN_IN] = LordNew gui_skyblockSignIn;
		m_layouts[LayoutId::SKYBLOCK_REWARD_DETAIL] = LordNew gui_skyblockRewardDetail;
		m_layouts[LayoutId::SKYBLOCK_REWARD_RESULT] = LordNew gui_skyblockRewardResult;
		m_layouts[LayoutId::BLOCK_CITY_BUY_FLY] = LordNew gui_blockCityBuyFlyTip;
		m_layouts[LayoutId::BLOCK_CITY_PORTAL] = LordNew gui_blockCityPortal;
		m_layouts[LayoutId::SKYBLOCK_REVIVE_NEW_PRI] = LordNew gui_skyBlockReciveNewPri;


		m_layouts[LayoutId::WALKINGDEAD_DESIGNATIONPANEL] = LordNew gui_designationPanelControl; 


		addLayout(m_layouts[LayoutId::LOADING_PAGE]);
		addLayout(m_layouts[LayoutId::TOOL_BAR]);
		m_layoutShown[LayoutId::TOOL_BAR] = false;
		m_layoutShown[LayoutId::LOADING_PAGE] = true;

		m_subscriptionGuard.add(PlayerDeathEvent::subscribe(std::bind(&RootGuiLayout::handlePlayerDeath, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BackKeyDownEvent::subscribe(std::bind(&RootGuiLayout::onBackKeyDown, this)));
		m_subscriptionGuard.add(ShowItemNameEvent::subscribe(std::bind(&RootGuiLayout::showItemNameUi, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowGuiJailBreakRoleChoiceEvent::subscribe(std::bind(&RootGuiLayout::showJailBreakRoleChoice, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RankActivateEvent::subscribe(std::bind(&RootGuiLayout::showRank, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(OpenPersonalShopEvent::subscribe(std::bind(&RootGuiLayout::showPersonalShop, this)));
		m_subscriptionGuard.add(OpenSuperShopEvent::subscribe(std::bind(&RootGuiLayout::showSuperShop, this)));
		m_subscriptionGuard.add(OpenGunStoreEvent::subscribe(std::bind(&RootGuiLayout::showGunStore, this)));
		m_subscriptionGuard.add(OpenChestLotteryEvent::subscribe(std::bind(&RootGuiLayout::showChestLottery, this)));
		m_subscriptionGuard.add(OpenSeasonRankEvent::subscribe(std::bind(&RootGuiLayout::showSeasonRank, this)));
		m_subscriptionGuard.add(OpenBedWarSeasonRankEvent::subscribe(std::bind(&RootGuiLayout::showBedWarSeasonRank, this)));
		m_subscriptionGuard.add(ShowLotteryChestDetailEvent::subscribe(std::bind(&RootGuiLayout::showLotteryChestDetail, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BirdPackOpenEvent::subscribe(std::bind(&RootGuiLayout::showBirdPackAndFuse, this)));
		m_subscriptionGuard.add(OpenBedWarRuneEvent::subscribe(std::bind(&RootGuiLayout::showBedWarRune, this)));
		m_subscriptionGuard.add(ShowArmorUpgradeEvent::subscribe(std::bind(&RootGuiLayout::showPixelGunHallArmorUpgrade, this)));
		m_subscriptionGuard.add(ShowPixelGunModeSelectEvent::subscribe(std::bind(&RootGuiLayout::showPixelGunHallModeSelect, this)));
		m_subscriptionGuard.add(ShowWindowByNameEvent::subscribe(std::bind(&RootGuiLayout::showWindowByName, this, std::placeholders::_1)));
		m_subscriptionGuard.add(SetGuidePanelShowEvent::subscribe(std::bind(&RootGuiLayout::setGuidePanelShow, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowChestRewardDetailEvent::subscribe(std::bind(&RootGuiLayout::showChestRewardDetail, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowGiftBagDetailEvent::subscribe(std::bind(&RootGuiLayout::showGiftBagDetail, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowLevelUpPanelEvent::subscribe(std::bind(&RootGuiLayout::showLevelUpPanel, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowSkyBlockSignInRewardDetailEvent::subscribe(std::bind(&RootGuiLayout::ShowSkyBlockSignInRewardDetail, this, std::placeholders::_1)));

		m_openChestArgs.delayTickToOpenChest = -1;
		m_openChestArgs.inventory = nullptr;
		initNeedLoadView();
	}

	void RootGuiLayout::initNeedLoadView()
	{
		m_needLoadView.push_back(LayoutId::OUTPUT_CONFIG);
		m_needLoadView.push_back(LayoutId::COUNTDOWN);
		m_needLoadView.push_back(LayoutId::PLAYER_INFO);
		m_needLoadView.push_back(LayoutId::MAIN);
		m_needLoadView.push_back(LayoutId::PIXEL_GUN_INDIE_MAIN);
		m_needLoadView.push_back(LayoutId::PLAYER_INVENTORY);
		m_needLoadView.push_back(LayoutId::GAME_TIP_DIALOG);
		m_needLoadView.push_back(LayoutId::CHEST);
		m_needLoadView.push_back(LayoutId::TIGER_LOTTERY);
		m_needLoadView.push_back(LayoutId::CHAT);
		m_needLoadView.push_back(LayoutId::MENU);
		m_needLoadView.push_back(LayoutId::DEAD_SUMMARY);
		m_needLoadView.push_back(LayoutId::FINAL_SUMMARY);
		m_needLoadView.push_back(LayoutId::SHOP);
		m_needLoadView.push_back(LayoutId::APP_SHOP);

		m_needLoadView.push_back(LayoutId::WALKINGDEAD_STORE);
		m_needLoadView.push_back(LayoutId::WALKINGDEAD_SupplyPopup);

		m_needLoadView.push_back(LayoutId::PERSONAL_SHOP);
		m_needLoadView.push_back(LayoutId::SUPER_SHOP);
		m_needLoadView.push_back(LayoutId::SUMMARY_POINT);
		m_needLoadView.push_back(LayoutId::SUMMARY_TIP);
		m_needLoadView.push_back(LayoutId::JAIL_BREAK_ROLE_CHOICE);
		m_needLoadView.push_back(LayoutId::RANK);
		m_needLoadView.push_back(LayoutId::BLOCK_LOADING);
		m_needLoadView.push_back(LayoutId::MINI_MAP);
		m_needLoadView.push_back(LayoutId::MANOR);
		m_needLoadView.push_back(LayoutId::UPGRADE_MANOR);
		m_needLoadView.push_back(LayoutId::REAL_TIME_RANK);
		m_needLoadView.push_back(LayoutId::MULTI_TIP_DIALOG);
		m_needLoadView.push_back(LayoutId::BUILD_WAR_GRADE);
		m_needLoadView.push_back(LayoutId::BUILD_WAR_GUESS);
		m_needLoadView.push_back(LayoutId::BUILD_WAR_GUESS_RESULT);
		m_needLoadView.push_back(LayoutId::RANCH);
		m_needLoadView.push_back(LayoutId::RANCH_ORDER);
		m_needLoadView.push_back(LayoutId::MULTI_TIP_INTERACTION_DIALOG);
		m_needLoadView.push_back(LayoutId::RANCH_FACTORY_BUILD);
		m_needLoadView.push_back(LayoutId::RANCH_FARM_BUILD);
		m_needLoadView.push_back(LayoutId::RANCH_UPGRAGE);
		m_needLoadView.push_back(LayoutId::RANCH_TIME_TIP);
		m_needLoadView.push_back(LayoutId::RANCH_RANK);
		m_needLoadView.push_back(LayoutId::RANCH_PROSPERITY_RANK);
		m_needLoadView.push_back(LayoutId::OCCUPATION_UNLOCK);
		m_needLoadView.push_back(LayoutId::ENCHANTMENT_PANEL);
		m_needLoadView.push_back(LayoutId::HIDEANDSEEKHALL_RESULT);
		m_needLoadView.push_back(LayoutId::PIXEL_GUNHALL_MODE_SELECT);
		m_needLoadView.push_back(LayoutId::GUN_STORE);
		m_needLoadView.push_back(LayoutId::PIXEL_GUN_1V1);
		m_needLoadView.push_back(LayoutId::CHEST_LOTTERY);
		m_needLoadView.push_back(LayoutId::PIXEL_GUN_REVIVE);
		m_needLoadView.push_back(LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE);
		m_needLoadView.push_back(LayoutId::PIXEL_GUN_RESULT);
		m_needLoadView.push_back(LayoutId::SEASON_RANK);
		m_needLoadView.push_back(LayoutId::BIRD_PACKANDFUSE);
		m_needLoadView.push_back(LayoutId::BIRD_PERSONAL_SHOP);
		m_needLoadView.push_back(LayoutId::BIRD_STORE);
		m_needLoadView.push_back(LayoutId::BIRD_LOTTERY);
		m_needLoadView.push_back(LayoutId::BIRD_TASK_TIP);
		m_needLoadView.push_back(LayoutId::BIRD_ATLAS);
		m_needLoadView.push_back(LayoutId::BIRD_ACTIVITY);
		m_needLoadView.push_back(LayoutId::CHECKIN);
		m_needLoadView.push_back(LayoutId::BIRD_HELP);
		m_needLoadView.push_back(LayoutId::TASK_SYSTEM);
		m_needLoadView.push_back(LayoutId::BEDWAR_RUNE);
		m_needLoadView.push_back(LayoutId::BEDWAR_SEASON_RANK);
		m_needLoadView.push_back(LayoutId::TYCOON_VEDIO_AD);
		m_needLoadView.push_back(LayoutId::VIDEO_AD_TIP);

		// -- Support Guide System
		m_needLoadView.push_back(LayoutId::GUIDE_SYSTEM);
		// -- Don't Support Guide System

		m_needLoadView.push_back(LayoutId::PIXEL_GUN_GIFT_BAG_STORE);
		m_needLoadView.push_back(LayoutId::PIXEL_CHICKEN_MAIN);
		m_needLoadView.push_back(LayoutId::PIXEL_CHICKEN_AWARD);
		m_needLoadView.push_back(LayoutId::PIXEL_GUN_GAME_LVUP);
		m_needLoadView.push_back(LayoutId::LEVEL_UP_PANEL);
		m_needLoadView.push_back(LayoutId::PIXEL_GUN_SIGN_IN);
		m_needLoadView.push_back(LayoutId::BRIEF_INTRODUCTION);
		m_needLoadView.push_back(LayoutId::LOTTERY_CHEST_DETAIL);
		m_needLoadView.push_back(LayoutId::CHEST_REWARD_DETAIL);
		m_needLoadView.push_back(LayoutId::CHEST_REWARD_RESULT);
		m_needLoadView.push_back(LayoutId::SEASON_REWARD);
		m_needLoadView.push_back(LayoutId::BEDWAR_SEASON_REWARD);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_TIGER_LOTTERY);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_MANOR);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_STORE);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_FRIEND);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_RANK);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_CHOOSE_PAPER);
		m_needLoadView.push_back(LayoutId::SKYBLOCK_TASK_SYSTEM);
		m_needLoadView.push_back(LayoutId::SKYBLOCK_TIP);
		m_needLoadView.push_back(LayoutId::COMMON_TIP);
		m_needLoadView.push_back(LayoutId::BASE_MESSAGE);
		m_needLoadView.push_back(LayoutId::FURNACE_PANEL);
		m_needLoadView.push_back(LayoutId::SKYBLOCK_APP_SHOP);
		m_needLoadView.push_back(LayoutId::SKYBLOCK_RANK);
		m_needLoadView.push_back(LayoutId::UPDATE_PRODUCT_INFO);
		m_needLoadView.push_back(LayoutId::UNLOCK_PRODUCT_INFO);
		m_needLoadView.push_back(LayoutId::WALKINGDEAD_DEATHSETTLEMENT);
		m_needLoadView.push_back(LayoutId::BLOCK_FORT_STORE);
		m_needLoadView.push_back(LayoutId::BLOCK_FORT_MANOR_UPGRADE);
		m_needLoadView.push_back(LayoutId::BLOCK_FORT_BUILDING_INFO);
		m_needLoadView.push_back(LayoutId::BLOCK_FORT_PACK);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_PACK);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_HOUSE_SHOP);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_ELEVATOR);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_RECEIVE_FLOWER);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_CHARM_RANK);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_SAVE_FILE);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_RANK_AWARD);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_PORTAL);
		m_needLoadView.push_back(LayoutId::SKYBLOCK_PRIVILEGE_SHOP);
		m_needLoadView.push_back(LayoutId::SKYBLOCK_SIGN_IN);
		m_needLoadView.push_back(LayoutId::SKYBLOCK_REWARD_DETAIL);
		m_needLoadView.push_back(LayoutId::SKYBLOCK_REWARD_RESULT);
		m_needLoadView.push_back(LayoutId::SKYBLOCK_REVIVE_NEW_PRI);

		//--------------------------Top view divider start----------------------------
		m_needLoadView.push_back(LayoutId::KILL_PANEL);
		m_needLoadView.push_back(LayoutId::CAR_PROGRESS_TABLE);
		m_needLoadView.push_back(LayoutId::RANCH_TIP);
		m_needLoadView.push_back(LayoutId::RANCH_GAIN_TIP);
		m_needLoadView.push_back(LayoutId::BIRD_TIP);
		m_needLoadView.push_back(LayoutId::BIRD_GAIN_TIP);
		m_needLoadView.push_back(LayoutId::TIP_DIALOG);
		m_needLoadView.push_back(LayoutId::BUY_GOODS_TIP);
		m_needLoadView.push_back(LayoutId::NOTIFICATION);
		m_needLoadView.push_back(LayoutId::SUPPLY_TIP);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_TIP);
		m_needLoadView.push_back(LayoutId::BLOCK_FORT_TIP);
		m_needLoadView.push_back(LayoutId::BLOCK_CITY_BUY_FLY);

		//--------------------------Top view divider end----------------------------
	}

	void RootGuiLayout::loadGameGui()
	{
		if (m_isLoad)
			return;

		RewardSetting::loadSetting();
		while (m_needLoadView.size() > 0)
		{
			if (m_needLoadView.begin() != m_needLoadView.end())
			{
				if (m_layouts[*m_needLoadView.begin()]->isNeedLoad())
				{
					addLayout(m_layouts[*m_needLoadView.begin()]);
					m_layouts[*m_needLoadView.begin()]->hide();
				}
				m_needLoadView.erase(m_needLoadView.begin());
			}
		}

		if (m_isChoiceRole)
		{
			auto roleChoice = dynamic_cast<gui_jailBreakRoleChoice*>(m_layouts[LayoutId::JAIL_BREAK_ROLE_CHOICE]);
			if (roleChoice)
			{
				roleChoice->setChoiceRole(m_choiceRole);
			}
			m_layoutShown[LayoutId::JAIL_BREAK_ROLE_CHOICE] = true;
		}
		else
		{
			m_layoutShown[LayoutId::LOADING_PAGE] = false;
			m_layoutShown[LayoutId::NOTIFICATION] = true;
			if (checkGameType(ClientGameType::PixelGunHall) &&
				GameClient::CGame::Instance()->isEnableIndie())
			{
				m_layoutShown[LayoutId::PIXEL_GUN_INDIE_MAIN] = true;
			}
			else
			{
				m_layoutShown[LayoutId::MAIN] = true;
				m_layoutShown[LayoutId::PLAYER_INFO] = true;
				m_layoutShown[LayoutId::TOOL_BAR] = true;
				m_layoutShown[LayoutId::MINI_MAP] = m_isShowMiniMap;
				m_layoutShown[LayoutId::REAL_TIME_RANK] = false;
				m_layoutShown[LayoutId::KILL_PANEL] = true;
				m_layoutShown[LayoutId::CAR_PROGRESS_TABLE] = false;
				m_layoutShown[LayoutId::SUPPLY_TIP] = true;
				m_layoutShown[LayoutId::BASE_MESSAGE] = true;
			}
		}

		m_subscriptionGuard.add(CraftingTableActivateEvent::subscribe(std::bind(&RootGuiLayout::openCraftingTable, this)));
		m_subscriptionGuard.add(FurnaceTableActivateEvent::subscribe(std::bind(&RootGuiLayout::openFurnaceTable, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ChestActivateEvent::subscribe(std::bind(&RootGuiLayout::openChest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(MerchantActivateEvent::subscribe(std::bind(&RootGuiLayout::showShop, this, std::placeholders::_1)));
		m_subscriptionGuard.add(MerchantUpdateEvent::subscribe(std::bind(&RootGuiLayout::shopUpdate, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateUnlockedCommodityEvent::subscribe(std::bind(&RootGuiLayout::updateShopUnlockedState, this)));
		createItemNameUi();
		m_isLoad = true;
	}

	void RootGuiLayout::onUpdate(ui32 nTimeElapse)
	{
		m_layoutShown[LayoutId::OUTPUT_CONFIG] = false;
		auto player = Blockman::Instance()->m_pPlayer;
		if (player && m_layoutShown[LayoutId::MAIN] )
		{
			m_layoutShown[LayoutId::PLAYER_INFO] = !player->capabilities.isWatchMode;
		}
		for (auto id : ALL_LAYOUT_ID)
		{
			if (m_layoutShown[id])
			{
				m_layouts[id]->show();
			}
			else
			{
				m_layouts[id]->hide();
			}

			if (!m_orderLayoutList.empty())
			{
				if ((m_lastOrderLayout == id && !m_layoutShown[id]) || m_lastOrderLayout == LayoutId::MAIN)
				{
					showOrderLayout();
				}
			}
		}

		if (m_openChestArgs.delayTickToOpenChest > 0) {
			--m_openChestArgs.delayTickToOpenChest;
			if (m_openChestArgs.delayTickToOpenChest == 0) {
				showChestManage(true);
				dynamic_cast<gui_chest*>(m_layouts[LayoutId::CHEST])->setChestInventory(
					m_openChestArgs.inventory,
					m_openChestArgs.blockPos, m_openChestArgs.face, m_openChestArgs.hisPos
				);

				m_openChestArgs.delayTickToOpenChest = -1;
				m_openChestArgs.inventory = nullptr;
			}
		}

		if (m_stItemName)
		{
			m_stItemName->SetVisible(m_isShowItemName);
			m_stItemName->SetTouchable(false);
			m_showItemNameTime = m_isShowItemName ? m_showItemNameTime + nTimeElapse : 0;
			m_isShowItemName = m_isShowItemName && m_showItemNameTime > 3000 ? false : m_isShowItemName;
		}
		if (!m_layoutShown[LayoutId::BLOCK_CITY_TIGER_LOTTERY])
		{
			gui_blockCityTigerLottery* pLayout = dynamic_cast<gui_blockCityTigerLottery*>(m_layouts[LayoutId::BLOCK_CITY_TIGER_LOTTERY]);
			if (pLayout)
			{
				pLayout->onSelectFirst(EventArgs());
			}
		}
	}

	void RootGuiLayout::showPlayerInventory()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::PLAYER_INVENTORY] = true;
	}

	void RootGuiLayout::showMainControl()
	{
		bool showCountdown = m_layoutShown[LayoutId::COUNTDOWN];
		bool showTigerLottery = m_layoutShown[LayoutId::TIGER_LOTTERY];
		bool showPixelGun1v1 = m_layoutShown[LayoutId::PIXEL_GUN_1V1];
		bool showGuideSystem = m_layoutShown[LayoutId::GUIDE_SYSTEM];
		bool showSignSystem = m_layoutShown[LayoutId::PIXEL_GUN_SIGN_IN];
		bool showSeasonReward = m_layoutShown[LayoutId::SEASON_REWARD];
		bool showGiftBagStore = m_layoutShown[LayoutId::PIXEL_GUN_GIFT_BAG_STORE];
		bool showLevelUpPanel = m_layoutShown[LayoutId::LEVEL_UP_PANEL];
        bool showCarProgress = m_layoutShown[LayoutId::CAR_PROGRESS_TABLE];
		bool showPixelChickenMain = m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN];
		bool showVideoAdTip = m_layoutShown[LayoutId::VIDEO_AD_TIP];

		hideAllChildren();

		m_layoutShown[LayoutId::GUIDE_SYSTEM] = showGuideSystem;

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunHall &&
			GameClient::CGame::Instance()->isEnableIndie())
		{
			m_layoutShown[LayoutId::PIXEL_GUN_INDIE_MAIN] = true;
			m_layoutShown[LayoutId::NOTIFICATION] = true;
			m_layoutShown[LayoutId::BASE_MESSAGE] = true;
			m_layoutShown[LayoutId::PIXEL_GUN_SIGN_IN] = showSignSystem;
			m_layoutShown[LayoutId::SEASON_REWARD] = showSeasonReward;
			m_layoutShown[LayoutId::PIXEL_GUN_GIFT_BAG_STORE] = showGiftBagStore;
			m_layoutShown[LayoutId::LEVEL_UP_PANEL] = showLevelUpPanel;
			m_layoutShown[LayoutId::VIDEO_AD_TIP] = showVideoAdTip;
			return;
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BED_WAR_HALL &&
			GameClient::CGame::Instance()->isEnableIndie())
		{
			SCRIPT_EVENT::ShowPluginLayoutEvent::invoke("BedWarNewIndieMain.json", "");
			m_layoutShown[LayoutId::NOTIFICATION] = true;
			m_layoutShown[LayoutId::BASE_MESSAGE] = true;
			m_layoutShown[LayoutId::VIDEO_AD_TIP] = showVideoAdTip;
			return;
		}

		if (showCountdown)
		{
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::COUNTDOWN] = true;
		}
		else
		{
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::PLAYER_INFO] = true;
			m_layoutShown[LayoutId::TOOL_BAR] = true;
			m_layoutShown[LayoutId::MINI_MAP] = m_isShowMiniMap;
			m_layoutShown[LayoutId::KILL_PANEL] = true;
			m_layoutShown[LayoutId::CAR_PROGRESS_TABLE] = showCarProgress;
			m_layoutShown[LayoutId::NOTIFICATION] = true;
			m_layoutShown[LayoutId::SUPPLY_TIP] = true;
			m_layoutShown[LayoutId::BASE_MESSAGE] = true;
			dynamic_cast<gui_toolBar*>(m_layouts[LayoutId::TOOL_BAR])->resetCheckBox();
		}
		m_layoutShown[LayoutId::TIGER_LOTTERY] = showTigerLottery;
		m_layoutShown[LayoutId::PIXEL_GUN_1V1] = showPixelGun1v1;
		m_layoutShown[LayoutId::GUIDE_SYSTEM] = showGuideSystem;
		m_layoutShown[LayoutId::PIXEL_GUN_SIGN_IN] = showSignSystem;
		m_layoutShown[LayoutId::SEASON_REWARD] = showSeasonReward;
		m_layoutShown[LayoutId::PIXEL_GUN_GIFT_BAG_STORE] = showGiftBagStore;
		m_layoutShown[LayoutId::LEVEL_UP_PANEL] = showLevelUpPanel;
		m_layoutShown[LayoutId::VIDEO_AD_TIP] = showVideoAdTip;
		m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN] = showPixelChickenMain;
	}

	void RootGuiLayout::showGameTipDialogManage(bool isShow)
	{

		if (m_layoutShown[LayoutId::LOADING_PAGE])
		{
			return;
		}

		m_layoutShown[LayoutId::GAME_TIP_DIALOG] = isShow;
		if (isShow)
		{
			hideAllChildren();
			m_layoutShown[LayoutId::GAME_TIP_DIALOG] = true;
			m_layoutShown[LayoutId::TOOL_BAR] = false;
		}
		else
		{
			showMainControl();
		}
	}

	void RootGuiLayout::showGameCloseTipDialog()
	{
		dynamic_cast<gui_gameTipDialog*>(m_layouts[LayoutId::GAME_TIP_DIALOG])->setShowType(gui_gameTipDialog::ShowType::CLOSE_GAME);
		showGameTipDialogManage(true);
	}

	void RootGuiLayout::showAppShop(int page)
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		default:
			showMainControl();
			m_layoutShown[LayoutId::MINI_MAP] = false;
			m_layoutShown[LayoutId::APP_SHOP] = true;
			if (page != -1)
			{
				gui_appShop* shop = dynamic_cast<gui_appShop*>(m_layouts[LayoutId::APP_SHOP]);
				if (shop)
				{
					shop->showPage(page);
				}
			}
			break;
		}
	}

	bool RootGuiLayout::showPersonalShop()
	{
		bool showCountdown = m_layoutShown[LayoutId::COUNTDOWN];
		hideAllChildren();
		m_layoutShown[LayoutId::COUNTDOWN] = showCountdown;
		m_layoutShown[LayoutId::PERSONAL_SHOP] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BASE_MESSAGE] = true;
		return true;
	}

	bool RootGuiLayout::showSuperShop()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::SUPER_SHOP] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BASE_MESSAGE] = true;
		return true;
	}

	bool RootGuiLayout::handlePlayerDeath(ui64 playerRuntimeId)
	{
		return true; //in case rebirth
		if (m_layoutShown[LayoutId::DEAD_SUMMARY] || m_layoutShown[LayoutId::FINAL_SUMMARY])
		{
			return true;
		}

		if (m_layoutShown[LayoutId::COUNTDOWN])
		{
			return true;
		}

		if (m_layoutShown[LayoutId::TIP_DIALOG])
		{
			gui_tipDialog* pLayout = dynamic_cast<gui_tipDialog*>(m_layouts[LayoutId::TIP_DIALOG]);
			if (pLayout && pLayout->getShowTipType() == TipType::REVIVE)
			{
				return true;
			}
		}

		if (m_layoutShown[LayoutId::LOADING_PAGE])
		{
			return true;
		}

		EntityPlayerSPMP* me = Blockman::Instance()->m_pPlayer;
		if (!m_layouts[LayoutId::GAME_TIP_DIALOG]->isShown() && me->entityId == playerRuntimeId)
		{
			LordLogInfo("RootGuiLayout::handlePlayerDeath user die");
			dynamic_cast<gui_gameTipDialog*>(m_layouts[LayoutId::GAME_TIP_DIALOG])->setShowType(gui_gameTipDialog::ShowType::DEATH);
			showGameTipDialogManage(true);
		}
		return true;
	}

	bool RootGuiLayout::onBackKeyDown()
	{

		if (m_layoutShown[LayoutId::LOADING_PAGE] || m_layoutShown[LayoutId::JAIL_BREAK_ROLE_CHOICE] || m_layoutShown[LayoutId::GUIDE_SYSTEM] || m_layoutShown[LayoutId::COMMON_TIP])
		{
			return true;
		}

		for (auto id : ALL_LAYOUT_ID)
		{
			if (m_layoutShown[id] && 
				id != LayoutId::GAME_TIP_DIALOG && 
				id != LayoutId::MAIN && 
				id != LayoutId::PLAYER_INFO && 
				id != LayoutId::TOOL_BAR && 
				id != LayoutId::MINI_MAP && 
				id != LayoutId::BUILD_WAR_GUESS_RESULT &&
				id != LayoutId::OUTPUT_CONFIG && 
				id != LayoutId::KILL_PANEL &&
				id != LayoutId::CAR_PROGRESS_TABLE &&
				id != LayoutId::PIXEL_GUN_1V1 &&
				id != LayoutId::NOTIFICATION &&
				id != LayoutId::SUPPLY_TIP &&
				id != LayoutId::PIXEL_GUN_INDIE_MAIN &&
				id != LayoutId::BASE_MESSAGE &&
				id != LayoutId::PIXEL_CHICKEN_MAIN)
			{
				if (m_layoutShown[id] && id == LayoutId::CHEST)
				{
					showChestManage(false);
				}
				showMainControl();
				return true;
			}
		}

		if (!m_layouts[LayoutId::GAME_TIP_DIALOG]->isShown())
		{
			showMenu();
		}
		else
		{
			if (dynamic_cast<gui_gameTipDialog*>(m_layouts[LayoutId::GAME_TIP_DIALOG])->getShowType() != gui_gameTipDialog::ShowType::DEATH  &&
				dynamic_cast<gui_gameTipDialog*>(m_layouts[LayoutId::GAME_TIP_DIALOG])->getShowType() != gui_gameTipDialog::ShowType::NETWORK_DISCONNECT)
			{
				showGameTipDialogManage(false);
			}
		}
		return true;
	}

	bool RootGuiLayout::openChest(IInventory* inventory, const Vector3i & blockPos, int face, const Vector3 & hisPos)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::RanchersExplore) return true;

		if (m_openChestArgs.delayTickToOpenChest < 0) {
			m_openChestArgs.delayTickToOpenChest = 20;
			m_openChestArgs.inventory = inventory;
			m_openChestArgs.blockPos = blockPos;
			m_openChestArgs.face = face;
			m_openChestArgs.hisPos = hisPos;
		}
		return true;
	}

	void RootGuiLayout::showChestManage(bool isShow)
	{
		if (!isShow)
		{
			showMainControl();
			Container* pContainer = Blockman::Instance()->m_pPlayer->openContainer;
			ClientNetwork::Instance()->getSender()->sendCloseContainer(BlockPos::ONE);
		}
		else
		{
			hideAllChildren();
			m_layoutShown[LayoutId::CHEST] = isShow;
		}
	}

	void RootGuiLayout::showChatGui()
	{
		bool showTigerLottery = m_layoutShown[LayoutId::TIGER_LOTTERY];
		bool showPixelGun1v1 = m_layoutShown[LayoutId::PIXEL_GUN_1V1];
		bool showPixelChicken = m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN];
		hideAllChildren();
		m_layoutShown[LayoutId::CHAT] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::TIGER_LOTTERY] = showTigerLottery;
		m_layoutShown[LayoutId::PIXEL_GUN_1V1] = showPixelGun1v1;
		m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN] = showPixelChicken;
	}

	void RootGuiLayout::showDeadSummaryGui()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::DEAD_SUMMARY] = true;
	}


	void RootGuiLayout::showFinalSummaryGui()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::FINAL_SUMMARY] = true;
	}

	bool RootGuiLayout::showShop(EntityMerchant * merchant)
	{
		hideAllChildren();
		m_layoutShown[LayoutId::SHOP] = true;
		auto shop = dynamic_cast<gui_shop*>(m_layouts[LayoutId::SHOP]);
		if (shop)
		{
			shop->onShopOpen(merchant);
		}
		return true;
	}

	bool RootGuiLayout::shopUpdate(EntityMerchant * merchant) {
		if (m_layoutShown[LayoutId::SHOP]) {
			auto shop = dynamic_cast<gui_shop*>(m_layouts[LayoutId::SHOP]);
			if (shop)
			{
				shop->onShopOpen(merchant);
			}
		}
		return true;
	}

	void RootGuiLayout::createItemNameUi()
	{
		if (!m_stItemName)
		{
			m_stItemName = LordNew GUIStaticText(GWT_STATIC_TEXT, "RootGuiLayout-Item-Name");
			m_stItemName->SetArea(UDim(0, 0), UDim(1, -160), UDim(1, 0), UDim(0, 30.0f));
			m_stItemName->SetTextHorzAlign(HA_CENTRE);
			m_stItemName->SetTextVertAlign(VA_CENTRE);
			m_stItemName->SetText("");
			m_stItemName->SetWordWrap(true);
			m_rootWindow->AddChildWindow(m_stItemName);
		}
	}

	bool RootGuiLayout::showItemNameUi(ItemStackPtr itemStack)
	{
		LordAssert(itemStack && itemStack->stackSize > 0);
		StringStream ss;
		ss << LanguageManager::Instance()->getItemName(itemStack->getItemName());
		auto enchantmentList = itemStack->getEnchantmentTagList();
		if (enchantmentList && enchantmentList->tagCount() > 0)
		{
			for (int i = 0; i < enchantmentList->tagCount(); ++i)
			{
				auto id = static_cast<NBTTagCompound*>(enchantmentList->tagAt(i))->getShort("id");
				auto level = static_cast<NBTTagCompound*>(enchantmentList->tagAt(i))->getShort("lvl");
				auto enchantment = Enchantment::enchantmentsList[id];
				ss << "#n"
					<< LanguageManager::Instance()->getString(enchantment->getName())
					<< ' '
					<< LanguageManager::Instance()->getString("enchantment.level." + StringUtil::ToString(level));
			}
		}
		if (m_stItemName)
		{
			m_isShowItemName = true;
			auto displayString = ss.str();
			StringUtil::Trim(displayString);
			m_stItemName->SetText(displayString.c_str());
			m_showItemNameTime = 0;
		}
		return true;
	}

	void RootGuiLayout::showMenu()
	{
		if (SCRIPT_EVENT::ShowSettingMenuEvent::invoke(true))
		{
			if (checkGameType({ ClientGameType::PixelGunHall , ClientGameType::BED_WAR_HALL , ClientGameType::SkyWarLuckyBlockHall }) &&
				GameClient::CGame::Instance()->isEnableIndie())
			{
				GameClient::CGame::Instance()->getShellInterface()->
					onAppActionTrigger((int)AppActionType::APP_ACTION_SETTING);
			}
			else
			{
				bool showTigerLottery = m_layoutShown[LayoutId::TIGER_LOTTERY];
				bool showPixelGun1v1 = m_layoutShown[LayoutId::PIXEL_GUN_1V1];
				bool showPixelChicken = m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN];
				hideAllChildren();
				m_layoutShown[LayoutId::MENU] = true;
				m_layoutShown[LayoutId::TOOL_BAR] = true;
				m_layoutShown[LayoutId::BASE_MESSAGE] = true;
				m_layoutShown[LayoutId::TIGER_LOTTERY] = showTigerLottery;
				m_layoutShown[LayoutId::PIXEL_GUN_1V1] = showPixelGun1v1;
				m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN] = showPixelChicken;
				dynamic_cast<gui_toolBar*>(m_layouts[LayoutId::TOOL_BAR])->openMenu();
			}
		}
	}

	void RootGuiLayout::showLoadPage()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::LOADING_PAGE] = true;
		auto loadingPage = dynamic_cast<gui_loadingPage*>(m_layouts[LayoutId::LOADING_PAGE]);
		if (loadingPage)
			loadingPage->show();
	}

	void RootGuiLayout::showSummarPoint(const LORD::UVector2& pos, const std::map<int, int>& pointMap)
	{
		gui_summaryPoint* pLayout = dynamic_cast<gui_summaryPoint*>(m_layouts[LayoutId::SUMMARY_POINT]);
		if (!pLayout)
			return;

		m_layoutShown[LayoutId::SUMMARY_POINT] = !m_layoutShown[LayoutId::SUMMARY_POINT];
		if (m_layoutShown[LayoutId::SUMMARY_POINT])
		{
			pLayout->showContent(pos, pointMap);
		}
	}

	void RootGuiLayout::showSummaryRewardTip(const LORD::UVector2& pos, int total)
	{
		gui_summaryTip* pLayout = dynamic_cast<gui_summaryTip*>(m_layouts[LayoutId::SUMMARY_TIP]);
		if (!pLayout)
			return;

		m_layoutShown[LayoutId::SUMMARY_TIP] = !m_layoutShown[LayoutId::SUMMARY_TIP];
		if (m_layoutShown[LayoutId::SUMMARY_TIP])
		{
			pLayout->showContent(pos, total);
		}
	}

	void RootGuiLayout::showCountdown(i8 second)
	{
		bool showPixelChickenMain = m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN];
		hideAllChildren();
		gui_countdown* pLayout = dynamic_cast<gui_countdown*>(m_layouts[LayoutId::COUNTDOWN]);
		if (!pLayout)
		{
			return;
		}
		pLayout->setCountdown(second);
		m_layoutShown[LayoutId::MAIN] = true;
		m_layoutShown[LayoutId::COUNTDOWN] = true;
		m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN] = showPixelChickenMain;
	}

	void RootGuiLayout::showCountdown(i8 second, int type)
	{
		bool showPixelChickenMain = m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN];
		hideAllChildren();
		gui_countdown* pLayout = dynamic_cast<gui_countdown*>(m_layouts[LayoutId::COUNTDOWN]);
		if (!pLayout)
		{
			return;
		}
		pLayout->setCountdown(second, type);

		if (type != gui_countdown::CANCEL_COUNTDOWN)
		{
			Blockman::Instance()->m_playerControl->setDisableMovement(true);
			Blockman::Instance()->m_playerControl->setPerspectiveMove(true);
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::COUNTDOWN] = true;
			m_layoutShown[LayoutId::PIXEL_CHICKEN_MAIN] = showPixelChickenMain;
		}
	}

	void RootGuiLayout::hideCountdown()
	{
		Blockman::Instance()->m_playerControl->setDisableMovement(false);
		m_layoutShown[LayoutId::COUNTDOWN] = false;
		showMainControl();
	}

	void RootGuiLayout::changePlayerMaxHealth(float health)
	{
		gui_playerInfo * playerInfo = dynamic_cast<gui_playerInfo*>(m_layouts[LayoutId::PLAYER_INFO]);
		if (playerInfo)
		{
			playerInfo->resetMaxHealth(health);
		}
	}

	bool RootGuiLayout::showJailBreakRoleChoice(ChoiceRole role)
	{
		m_isChoiceRole = true;
		m_choiceRole = role;
		if (m_isLoad)
		{
			hideAllChildren();
			auto roleChoice = dynamic_cast<gui_jailBreakRoleChoice*>(m_layouts[LayoutId::JAIL_BREAK_ROLE_CHOICE]);
			if (roleChoice)
			{
				roleChoice->setChoiceRole(role);
			}
			m_layoutShown[LayoutId::JAIL_BREAK_ROLE_CHOICE] = true;
		}
		return true;
	}

	bool RootGuiLayout::showTipDialog()
	{
		m_layoutShown[LayoutId::TIP_DIALOG] = true;
		return true;
	}

	bool RootGuiLayout::hideTipDialog()
	{
		m_layoutShown[LayoutId::TIP_DIALOG] = false;
		return true;
	}

	int RootGuiLayout::getSelfTeamId()
	{
		gui_toolBar* pLayout = dynamic_cast<gui_toolBar*>(m_layouts[LayoutId::TOOL_BAR]);
		if (!pLayout)
		{
			return 0;
		}

		return pLayout->getSelfTeamId();
	}

	bool RootGuiLayout::showBuyGoodsTip(bool bShow)
	{
		m_layoutShown[LayoutId::BUY_GOODS_TIP] = bShow;
		return true;
	}

	bool RootGuiLayout::showRank(const String& rankInfo,int EntityId)
	{
		hideAllChildren();
		m_layoutShown[LayoutId::RANK] = true;
		return true;
	}

	bool RootGuiLayout::hideRank() {
		hideAllChildren();
		showMainControl();
		return true;
	}

	bool RootGuiLayout::showBlockLoading(bool bShow)
	{
		m_layoutShown[LayoutId::BLOCK_LOADING] = bShow;
		return true;
	}

	void RootGuiLayout::switchSerpece() {
		Blockman::Instance()->switchPerspece();
		updateSwitchSerpece();
	}

	void RootGuiLayout::updateSwitchSerpece() {
		gui_toolBar* pLayout = dynamic_cast<gui_toolBar*>(m_layouts[LayoutId::TOOL_BAR]);
		pLayout->updatePerspeceIcon(Blockman::Instance()->getCurrPersonView());
	}

	int RootGuiLayout::getViewNum() {
		return Blockman::Instance()->getCurrPersonView();
	}

	void RootGuiLayout::showParachuteBtn(bool isNeedOpen) {
		gui_mainControl* pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		pLayout->showParachuteBtn(isNeedOpen);
	}

	void RootGuiLayout::showBuildWarBlockBtn(bool isNeedOpen)
	{
		gui_mainControl* pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		pLayout->showBuildWarBlockBtn(isNeedOpen);
	}

	void RootGuiLayout::showHideAndSeekBtn(bool isShowChangeActoBtn, bool isShowLockCameraBtn, bool isShowOthenBtn)
	{
		gui_mainControl* pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		pLayout->showHideAndSeekBtn(isShowChangeActoBtn, isShowLockCameraBtn,isShowOthenBtn);
	}

	bool RootGuiLayout::showManor()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::MANOR] = true;
		return true;
	}

	void RootGuiLayout::showUpgradeManor()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::MANOR] = true;
		m_layoutShown[LayoutId::UPGRADE_MANOR] = true;
	}

	void RootGuiLayout::hidePlayerOperation()
	{
		auto pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		if (pLayout)
		{
			pLayout->hidePlayerOperationView(EventArgs());
		}
	}

	void RootGuiLayout::showDeadSummaryGui(const String& gameResult, bool isNextServer) {
		gui_deadSummary* pLayout = dynamic_cast<gui_deadSummary*>(m_layouts[LayoutId::DEAD_SUMMARY]);
		pLayout->showDeadSummary(gameResult, isNextServer);
	}

	bool RootGuiLayout::updateRealTimeRankData(String result) 
	{
		gui_realTimeRank* pLayout = dynamic_cast<gui_realTimeRank*>(m_layouts[LayoutId::REAL_TIME_RANK]);
		if (pLayout)
			pLayout->updateRealTimeRankData(result);
		return true;
	}

	bool RootGuiLayout::canPersonalShop()
	{
		gui_personalShop* pShop = dynamic_cast<gui_personalShop*>(m_layouts[LayoutId::PERSONAL_SHOP]);
		if (pShop)
			return pShop->canShow();
		return false;
	}

	bool RootGuiLayout::isShowRealTimeRankStatus()
	{
		gui_realTimeRank* pRank = dynamic_cast<gui_realTimeRank*>(m_layouts[LayoutId::REAL_TIME_RANK]);
		if (pRank)
			return pRank->isShow();

		return false;
	}

	void RootGuiLayout::isColseRealTimeRank(bool status)
	{
		gui_realTimeRank* pLayout = dynamic_cast<gui_realTimeRank*>(m_layouts[LayoutId::REAL_TIME_RANK]);
		if (pLayout)
			pLayout->isColseRealTimeRankOp(status);
	}

	void RootGuiLayout::showRealTimeRank()
	{
		m_layoutShown[LayoutId::REAL_TIME_RANK] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
	}

	void RootGuiLayout::hideRealTimeRank()
	{
		m_layoutShown[LayoutId::REAL_TIME_RANK] = false;
	}

	void RootGuiLayout::showBuildWarGrade(bool show)
	{
		if (show)
		{
			if (!isMainControlShown())
			{
				return;
			}

			m_layoutShown[LayoutId::BUILD_WAR_GRADE] = true;
		}
		else
		{
			m_layoutShown[LayoutId::BUILD_WAR_GRADE] = false;
		}

	}

	void RootGuiLayout::showBuildWarGuessResult(bool show)
	{

		m_layoutShown[LayoutId::BUILD_WAR_GUESS_RESULT] = show;
	}

	void RootGuiLayout::showBuildWarGuessUi(bool show)
	{
		m_layoutShown[LayoutId::BUILD_WAR_GUESS] = show;
	}

	bool RootGuiLayout::updateShopUnlockedState()
	{
		if (m_layoutShown[LayoutId::SHOP]) {
			auto shop = dynamic_cast<gui_shop*>(m_layouts[LayoutId::SHOP]);
			if (shop)
			{
				shop->onUpdateUnlockedState();
			}
		}
		return true;
	}

	void RootGuiLayout::showMultiTipDialog()
	{
		m_layoutShown[LayoutId::MULTI_TIP_DIALOG] = true;
	}

	void RootGuiLayout::hideMultiTipDialog()
	{
		m_layoutShown[LayoutId::MULTI_TIP_DIALOG] = false;
	}

	void RootGuiLayout::showMultiTipInteractionDialog()
	{
		m_layoutShown[LayoutId::MULTI_TIP_INTERACTION_DIALOG] = true;
	}

	void RootGuiLayout::hideMultiTipInteractionDialog()
	{
		m_layoutShown[LayoutId::MULTI_TIP_INTERACTION_DIALOG] = false;
	}

	void RootGuiLayout::showRanch()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::RANCH] = true;
		dynamic_cast<gui_ranch*>(m_layouts[LayoutId::RANCH])->openRanch();
	}

	void RootGuiLayout::showRanchTip()
	{
		/*hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;*/
		m_layoutShown[LayoutId::RANCH_TIP] = true;
	}

	void RootGuiLayout::hideRanchTip()
	{
		m_layoutShown[LayoutId::RANCH_TIP] = false;
	}

	void RootGuiLayout::showRanchOrder()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::RANCH_ORDER] = true;
	}

	void RootGuiLayout::showRanchUpgrade()
	{
		m_layoutShown[LayoutId::RANCH_UPGRAGE] = true;
	}

	void RootGuiLayout::hideRanchUpgrade()
	{
		m_layoutShown[LayoutId::RANCH_UPGRAGE] = false;
	}

	void RootGuiLayout::showRanchBuildFactory()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::RANCH_FACTORY_BUILD] = true;
	}

	void RootGuiLayout::showRanchBuildFarm()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::RANCH_FARM_BUILD] = true;
	}

	void RootGuiLayout::setRanchTimeTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::RANCH_TIME_TIP] = isShow;
	}

	void RootGuiLayout::setRanchGainTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::RANCH_GAIN_TIP] = isShow;
	}

	void RootGuiLayout::setRanchRankVisible(bool isShow)
	{
		m_layoutShown[LayoutId::RANCH_RANK] = isShow;
	}

	void RootGuiLayout::setRanchProsperityRankVisible(bool isShow)
	{
		m_layoutShown[LayoutId::RANCH_PROSPERITY_RANK] = isShow;
	}

	void RootGuiLayout::setOccupationUnlockVisible(bool isShow)
	{
		m_layoutShown[LayoutId::OCCUPATION_UNLOCK] = isShow;
	}

	void RootGuiLayout::checkBackgroundMusic()
	{
		m_layouts[LayoutId::MENU]->checkBackgroundMusic();
	}

	bool RootGuiLayout::setEnchantmentShow(bool isShow)
	{
		if (isShow)
		{
			hideAllChildren();
			m_layoutShown[LayoutId::MAIN] = true;
		}

		m_layoutShown[LayoutId::ENCHANTMENT_PANEL] = isShow;
		return true;
	}

	void RootGuiLayout::showTigerLottery()
	{
		showMainControl();
		m_layoutShown[LayoutId::TIGER_LOTTERY] = true;
	}

	void RootGuiLayout::hideTigerLottery()
	{
		m_layoutShown[LayoutId::TIGER_LOTTERY] = false;
		showMainControl();
	}

	void RootGuiLayout::showHideAndSeekHallResult()
	{
		showMainControl();
		m_layoutShown[LayoutId::HIDEANDSEEKHALL_RESULT] = true;
	}

	void RootGuiLayout::showLackOfMoney(i32 difference, CurrencyType type)
	{
		hideAllChildren();
		showMainControl();
		m_layoutShown[LayoutId::TIP_DIALOG] = true;
		auto tip = dynamic_cast<gui_tipDialog*>(m_layouts[LayoutId::TIP_DIALOG]);
		if (tip)
		{
			tip->showLackOfMoney(difference, type);
		}
	}
	
	bool RootGuiLayout::showBirdPackAndFuse()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BIRD_PACKANDFUSE] = true;
		auto view = dynamic_cast<gui_birdPackAndFuse*>(m_layouts[LayoutId::BIRD_PACKANDFUSE]);
		if (view)
		{
			view->setShowPack();
		}
		return true;
	}

	void RootGuiLayout::showBirdPersonShop()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::MAIN] = true;
		m_layoutShown[LayoutId::BIRD_PERSONAL_SHOP] = true;
	}

	void RootGuiLayout::showBirdStore()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BIRD_STORE] = true;
	}
	
	void RootGuiLayout::showBirdLottery()
	{
		m_layoutShown[LayoutId::BIRD_LOTTERY] = true;
	}

	bool RootGuiLayout::isShowBirdLottery()
	{
		return m_layoutShown[LayoutId::BIRD_LOTTERY];
	}
	
	void RootGuiLayout::setBirdTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::BIRD_TIP] = isShow;
	}
	
	bool RootGuiLayout::isShowBirdStore()
	{
		return m_layoutShown[LayoutId::BIRD_STORE];
	}

	bool RootGuiLayout::isBirdViewShow()
	{
		return isShowBirdStore()  || isShowBirdTaskTip()  || isShowBirdLottery() 
			|| m_layoutShown[LayoutId::BIRD_PERSONAL_SHOP] || m_layoutShown[LayoutId::BIRD_ACTIVITY]  || m_layoutShown[LayoutId::BIRD_ATLAS] 
			|| m_layoutShown[LayoutId::MENU] || m_layoutShown[LayoutId::CHAT] || m_layoutShown[LayoutId::BIRD_PACKANDFUSE]
			|| m_layoutShown[LayoutId::CHECKIN] || m_layoutShown[LayoutId::BIRD_HELP];
	}

	void RootGuiLayout::setBirdTaskTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::BIRD_TASK_TIP] = isShow;
	}

	bool RootGuiLayout::isShowBirdTaskTip()
	{
		return m_layoutShown[LayoutId::BIRD_TASK_TIP];
	}

	void RootGuiLayout::showBirdAtlas()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BIRD_ATLAS] = true;
	}

	void RootGuiLayout::setBirdGainTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::BIRD_GAIN_TIP] = isShow;
	}

	void RootGuiLayout::showBirdActivity()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BIRD_ACTIVITY] = true;
	}

	bool RootGuiLayout::showPixelGunHallModeSelect()
	{
		showMainControl();
		m_layoutShown[LayoutId::PIXEL_GUNHALL_MODE_SELECT] = true;
		return true;
	}

	void RootGuiLayout::setPixelGun1V1Show(bool isShow)
	{
		if (isShow)
		{
			showMainControl();
		}
		m_layoutShown[LayoutId::PIXEL_GUN_1V1] = isShow;
	}

	void RootGuiLayout::setPixelGunReviveShow(bool isShow)
	{
		showMainControl();
		m_layoutShown[LayoutId::PIXEL_GUN_REVIVE] = isShow;
		if (isShow)
		{
			m_layoutShown[LayoutId::MAIN] = false;
			m_layoutShown[LayoutId::PLAYER_INFO] = false;
		}
		else
		{
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::PLAYER_INFO] = true;
		}
	}

	void RootGuiLayout::setPixelGunResultShow(bool isShow)
	{
		showMainControl();
		m_layoutShown[LayoutId::PIXEL_GUN_RESULT] = isShow;
		if (isShow)
		{
			m_layoutShown[LayoutId::MAIN] = false;
			m_layoutShown[LayoutId::PLAYER_INFO] = false;
		}
		else 
		{
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::PLAYER_INFO] = true;
		}
	}


	bool RootGuiLayout::showGunStore()
	{
		showMainControl();
		m_layoutShown[LayoutId::GUN_STORE] = true;
		return true;
	}

	bool RootGuiLayout::showChestLottery()
	{
		showMainControl();
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BED_WAR_HALL)
		{
			//m_layoutShown[LayoutId::BOX_LOTTERY] = true;
		}
		else
			m_layoutShown[LayoutId::CHEST_LOTTERY] = true;
		return true;
	}

	bool RootGuiLayout::showPixelGunHallArmorUpgrade()
	{
		showMainControl();
		m_layoutShown[LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE] = true;
		return true;
	}

	bool RootGuiLayout::showSeasonRank()
	{
		showMainControl();
		m_layoutShown[LayoutId::SEASON_RANK] = true;
		return true;
	}

	bool RootGuiLayout::showBedWarSeasonRank()
	{
		showMainControl();
		m_layoutShown[LayoutId::BEDWAR_SEASON_RANK] = true;
		return true;
	}

	bool RootGuiLayout::showLotteryChestDetail(int type)
	{

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BED_WAR_HALL)
		{

		}

		m_layoutShown[LayoutId::LOTTERY_CHEST_DETAIL] = true;
		auto lottery_chest_detail = dynamic_cast<gui_lotteryChestDetail*>(m_layouts[LayoutId::LOTTERY_CHEST_DETAIL]);
		if (lottery_chest_detail)
		{
			lottery_chest_detail->ShowChestDetail((LotteryChestType)type);
		}
		return true;
	}

	bool RootGuiLayout::showGiftBagDetail(int type, int giftId)
	{
		m_layoutShown[LayoutId::LOTTERY_CHEST_DETAIL] = true;
		auto lottery_chest_detail = dynamic_cast<gui_lotteryChestDetail*>(m_layouts[LayoutId::LOTTERY_CHEST_DETAIL]);
		if (lottery_chest_detail)
		{
			lottery_chest_detail->ShowGiftBagDetail((LotteryChestType)type, giftId);
		}
		return true;
	}

	bool RootGuiLayout::isShowTaskSystem()
	{
		return m_layoutShown[LayoutId::TASK_SYSTEM];
	}

	bool RootGuiLayout::showLevelUpPanel(bool isShow)
	{
		m_layoutShown[LayoutId::LEVEL_UP_PANEL] = isShow;
		if (!isShow)
		{
			HideTaskActiveRewardEvent::emit();
			m_layoutShown[LayoutId::TASK_SYSTEM] = false;
		}
		return false;
	}

	void RootGuiLayout::onDispatchCommonData(LayoutId id, String key, String data)
	{
		auto window = m_layouts[id];
		if (window)
		{
			window->onGetCommonData(key, data);
		}
	}

	void RootGuiLayout::hideLotteryChestDetail()
	{
		m_layoutShown[LayoutId::LOTTERY_CHEST_DETAIL] = false;
	}

	bool RootGuiLayout::showChestRewardDetail(int id)
	{
		m_layoutShown[LayoutId::CHEST_REWARD_DETAIL] = true;
		auto chest_reward_detail = dynamic_cast<gui_chestRewardDetail*>(m_layouts[LayoutId::CHEST_REWARD_DETAIL]);
		auto reward = RewardSetting::findRewardById(id);
		if (reward == nullptr)
			return false;
		playSoundByType(ST_Click);
		if (chest_reward_detail)
		{
			chest_reward_detail->ShowLotteryChestReward(reward);
		}
		return true;
	}

	void RootGuiLayout::hideChestRewardDetail()
	{
		m_layoutShown[LayoutId::CHEST_REWARD_DETAIL] = false;
	}

	bool RootGuiLayout::showChestRewardResult()
	{
		m_layoutShown[LayoutId::CHEST_REWARD_RESULT] = true;
		return true;
	}

	void RootGuiLayout::hideChestRewardResult()
	{
		m_layoutShown[LayoutId::PIXEL_GUN_SIGN_IN] = false;
		m_layoutShown[LayoutId::CHEST_REWARD_RESULT] = false;
	}

	void RootGuiLayout::showPixelGunGameLvUp(bool show)
	{
		m_layoutShown[LayoutId::PIXEL_GUN_GAME_LVUP] = show;
	}

	bool RootGuiLayout::isPixelGunResultOpen()
	{
		return m_layoutShown[LayoutId::PIXEL_GUN_RESULT];
	}

	void RootGuiLayout::showQuicklyBuildBlockTip()
	{
		m_layoutShown[LayoutId::TIP_DIALOG] = true;
		gui_tipDialog* pLayout = dynamic_cast<gui_tipDialog*>(m_layouts[LayoutId::TIP_DIALOG]);
		if (pLayout)
		{
			pLayout->showQuicklyBuildBlock();
		}
	}
    
	void RootGuiLayout::hideSelectableAction()
	{
		gui_toolBar* pLayout = dynamic_cast<gui_toolBar*>(m_layouts[LayoutId::TOOL_BAR]);
		if (pLayout)
		{
			pLayout->hideSelectableActionView(EventArgs());
		}
	}

	void RootGuiLayout::showUnlockSelectableActionTip(int actionId)
	{
		m_layoutShown[LayoutId::TIP_DIALOG] = true;
		gui_tipDialog* pLayout = dynamic_cast<gui_tipDialog*>(m_layouts[LayoutId::TIP_DIALOG]);
		if (pLayout)
		{
			pLayout->showUnlockSelectableActionTip(actionId);
		}
	}

	void RootGuiLayout::setQuicklyBuildBlockBtnStatus(bool status)
	{
		gui_mainControl* pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		if (pLayout)
		{
			pLayout->setQuicklyBuildBlockBtn(status);
		}
	}

	void RootGuiLayout::showCheckin()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::CHECKIN] = true;
	}

	void RootGuiLayout::showBirdHelp()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BIRD_HELP] = true;
	}

	bool RootGuiLayout::showTaskSystem()
	{
		showMainControl();
		m_layoutShown[LayoutId::TASK_SYSTEM] = true;
		return true;
	}

	bool RootGuiLayout::showBedWarRune()
	{
		showMainControl();
		m_layoutShown[LayoutId::BEDWAR_RUNE] = true;
		return true;
	}

	bool RootGuiLayout::showBriefIntroduction()
	{
		addOrderShowLayout(LayoutId::BRIEF_INTRODUCTION);
		return true;
	}

	void RootGuiLayout::showPixelGunSignIn()
	{
		addOrderShowLayout(LayoutId::PIXEL_GUN_SIGN_IN);
	}

	bool RootGuiLayout::showWindowByName(const String & name)
	{
		for (auto id : ALL_LAYOUT_ID)
		{
			auto layout = m_layouts[id];
			if (layout && layout->getName() == name)
			{
				showWindowById((int)id);
				return true;
			}
		}
		SCRIPT_EVENT::ShowPluginLayoutEvent::invoke(name, "");
		return true;
	}

	bool RootGuiLayout::showWindowById(int id)
	{
		showMainControl();
		showWindowByLayoutId((LayoutId)id);
		return true;
	}

	void RootGuiLayout::showWindowByLayoutId(LayoutId id)
	{
		m_layoutShown[id] = true;
	}

	void RootGuiLayout::hideWindowByName(const String & name)
	{
		for (auto id : ALL_LAYOUT_ID)
		{
			auto layout = m_layouts[id];
			if (layout && layout->getName() == name)
			{
				hideWindowById((int)id);
				return;
			}
		}
		SCRIPT_EVENT::HidePluginLayoutEvent::invoke(name);
	}

	void RootGuiLayout::hideWindowById(int id)
	{
		hideWindowByLayoutId((LayoutId)id);
	}

	void RootGuiLayout::hideWindowByLayoutId(LayoutId id)
	{
		m_layoutShown[id] = false;
	}

	bool RootGuiLayout::setGuidePanelShow(bool isShow)
	{
		m_layoutShown[LayoutId::GUIDE_SYSTEM] = isShow;
		return true;
	}

	bool RootGuiLayout::showPixelGunGiftBagStore(bool isShow)
	{
		if (isShow)
		{
			addOrderShowLayout(LayoutId::PIXEL_GUN_GIFT_BAG_STORE);
			return true;
		}
		m_layoutShown[LayoutId::PIXEL_GUN_GIFT_BAG_STORE] = isShow;
		return true;
	}
	bool RootGuiLayout::isShowBlockCityView()
	{
		return m_layoutShown[LayoutId::BLOCK_CITY_TIGER_LOTTERY] || m_layoutShown[LayoutId::BLOCK_CITY_MANOR] || m_layoutShown[LayoutId::BLOCK_CITY_STORE]
			   || m_layoutShown[LayoutId::BLOCK_CITY_FRIEND] || m_layoutShown[LayoutId::BLOCK_CITY_RANK] || m_layoutShown[LayoutId::MENU]
			   || m_layoutShown[LayoutId::CHAT] || m_layoutShown[LayoutId::CHECKIN] || m_layoutShown[LayoutId::BLOCK_CITY_TIP] || m_layoutShown[LayoutId::BLOCK_CITY_PACK]
			   || m_layoutShown[LayoutId::BLOCK_CITY_HOUSE_SHOP];
	}

	void RootGuiLayout::showBlockCityTigerLottery()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::BLOCK_CITY_TIGER_LOTTERY] = true;
	}

	void RootGuiLayout::hideBlockCityTigerLottery()
	{
		m_layoutShown[LayoutId::BLOCK_CITY_TIGER_LOTTERY] = false;
		showMainControl();
	}

	void RootGuiLayout::showBlockCityManor()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_CITY_MANOR] = true;
	}

	void RootGuiLayout::setBlockCityTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::BLOCK_CITY_TIP] = isShow;
	}

	void RootGuiLayout::showBlockCityStore()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_CITY_STORE] = true;
	}

	void RootGuiLayout::showBlockCityFriend()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_CITY_FRIEND] = true;
	}

	void RootGuiLayout::showBlockCityRank()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_CITY_RANK] = true;
	}

	void RootGuiLayout::showBlockCityChooseView()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_CITY_CHOOSE_PAPER] = true;
	}

	void RootGuiLayout::setCommonTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::COMMON_TIP] = isShow;
	}

	bool RootGuiLayout::isShowSkyBlockTaskSystem()
	{
		return m_layoutShown[LayoutId::SKYBLOCK_TASK_SYSTEM];
	}


	void RootGuiLayout::notifyCraft()
	{
		if (m_layoutShown[LayoutId::PLAYER_INVENTORY])
		{
			if (dynamic_cast<gui_playerInventoryControl*>(m_layouts[LayoutId::PLAYER_INVENTORY]))
			{
				dynamic_cast<gui_playerInventoryControl*>(m_layouts[LayoutId::PLAYER_INVENTORY])->notifyUpdateCraft();
			}
		}
	}

	bool RootGuiLayout::showSkyBlockTaskSystem()
	{
		showMainControl();
		m_layoutShown[LayoutId::SKYBLOCK_TASK_SYSTEM] = true;
		gui_skyBlockTaskSystem* pLayout = dynamic_cast<gui_skyBlockTaskSystem*>(m_layouts[LayoutId::SKYBLOCK_TASK_SYSTEM]);
		if (pLayout)
		{
			pLayout->showMainTab();
		}
		return true;
	}

	bool RootGuiLayout::showSkyBlockTip()
	{
		showMainControl();
		m_layoutShown[LayoutId::SKYBLOCK_TIP] = true;
		return true;
	}

	bool RootGuiLayout::showUpdateProductTip()
	{
		showMainControl();
		m_layoutShown[LayoutId::UPDATE_PRODUCT_INFO] = true;
		return true;
	}

	bool RootGuiLayout::showUnlockProductTip()
	{
		showMainControl();
		m_layoutShown[LayoutId::UNLOCK_PRODUCT_INFO] = true;
		return true;
	}

	void RootGuiLayout::showSkyBlockAppShop()
	{
		showMainControl();
		m_layoutShown[LayoutId::SKYBLOCK_APP_SHOP] = true;
		gui_skyBlockAppShop* pLayout = dynamic_cast<gui_skyBlockAppShop*>(m_layouts[LayoutId::SKYBLOCK_APP_SHOP]);
		if (pLayout)
		{
			GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSkyBlockBuyGoods(0, 0, pLayout->OperateType::UPDATE_DATA);
		}
	}


	void RootGuiLayout::showSkyBlockAppShopPage(i32 pageId)
	{
		showMainControl();
		m_layoutShown[LayoutId::SKYBLOCK_APP_SHOP] = true;
		gui_skyBlockAppShop* pLayout = dynamic_cast<gui_skyBlockAppShop*>(m_layouts[LayoutId::SKYBLOCK_APP_SHOP]);
		if (pLayout)
		{
			GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSkyBlockBuyGoods(0, 0, pLayout->OperateType::UPDATE_DATA);
			pLayout->changePageToIndex(pageId);
		}
	}

	void RootGuiLayout::showPixelChickenResult()
	{
		showMainControl();
		m_layoutShown[LayoutId::PIXEL_CHICKEN_AWARD] = true;
		m_layoutShown[LayoutId::MAIN] = false;
		m_layoutShown[LayoutId::PLAYER_INFO] = false;
	}

	void RootGuiLayout::addOrderShowLayout(LayoutId layoutId)
	{
		if (!ifLayoutIsInList(layoutId))
		{
			m_orderLayoutList.push_back(layoutId);
			sortOrderLayoutList();
		}
	}

	void RootGuiLayout::showOrderLayout()
	{
		showMainControl();
		m_lastOrderLayout = m_orderLayoutList.back();
		m_layoutShown[m_orderLayoutList.back()] = true;
		removeLastOrderLayout();
	}

	bool RootGuiLayout::ifLayoutIsInList(LayoutId layoutId)
	{
		if (m_orderLayoutList.empty())
		{
			return false;
		}

		for (auto iter : m_orderLayoutList)
		{
			if (iter == layoutId)
				return true;
		}

		return false;
	}

	void RootGuiLayout::removeLastOrderLayout()
	{
		m_orderLayoutList.pop_back();
	}

	void RootGuiLayout::sortOrderLayoutList()
	{
		m_orderLayoutList.sort();
	}

	bool RootGuiLayout::sortOrderLayoutFunc(LayoutId id1, LayoutId id2)
	{
		return id1 < id2;
	}

	bool RootGuiLayout::showSkyBlockRank()
	{
		showMainControl();
		m_layoutShown[LayoutId::SKYBLOCK_RANK] = true;
		return true;
	}

	void RootGuiLayout::showWalkingDeadStore()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::WALKINGDEAD_STORE] = true;
	}

	void RootGuiLayout::showWalkingDeadDeathSettlement()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::WALKINGDEAD_DEATHSETTLEMENT] = true;
	}
	void RootGuiLayout::showSupplyPopup()
	{
		//hideAllChildren();
		m_layoutShown[LayoutId::WALKINGDEAD_SupplyPopup] = true;
	}

	void RootGuiLayout::showBlockFortStore()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_FORT_STORE] = true;
	}

	void RootGuiLayout::setBlockFortTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::BLOCK_FORT_TIP] = isShow;
	}

	void RootGuiLayout::showBlockFortBuildingInfo(bool isShow)
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_FORT_BUILDING_INFO] = isShow;
	}

	void RootGuiLayout::showBlockFortManorUpgrade()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_FORT_MANOR_UPGRADE] = true;
	}
	void RootGuiLayout::showBlockFortPack()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_FORT_PACK] = true;
	}

	bool RootGuiLayout::isGuiding()
	{
		return m_layoutShown[LayoutId::GUIDE_SYSTEM] || m_layoutShown[LayoutId::COMMON_TIP];
	}

	void RootGuiLayout::showBlockCityPack()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_CITY_PACK] = true;
	}

	void RootGuiLayout::showBlockCityHouseShop()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_CITY_HOUSE_SHOP] = true;
	}

	void RootGuiLayout::showBlockCityElevator()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_CITY_ELEVATOR] = true;
	}

	void RootGuiLayout::hideBlockCityElevator()
	{
		if (m_layoutShown[LayoutId::BLOCK_CITY_ELEVATOR])
		{
			m_layoutShown[LayoutId::BLOCK_CITY_ELEVATOR] = false;
			RootGuiLayout::Instance()->showMainControl();
			playSoundByType(ST_Click);
		}
	}

	void RootGuiLayout::setShowBlockCityReceiveFlower(bool isShow)
	{
		m_layoutShown[LayoutId::BLOCK_CITY_RECEIVE_FLOWER] = isShow;
	}

	void RootGuiLayout::setShowBlockCityCharmRank(bool isShow)
	{
		m_layoutShown[LayoutId::BLOCK_CITY_CHARM_RANK] = isShow;
	}

	void RootGuiLayout::setShowBlockCitySaveFile(bool isShow)
	{
		m_layoutShown[LayoutId::BLOCK_CITY_SAVE_FILE] = isShow;
	}

	void RootGuiLayout::setShowBlockCityRankAward(bool isShow)
	{
		m_layoutShown[LayoutId::BLOCK_CITY_RANK_AWARD] = isShow;
	}

	void RootGuiLayout::showBlockCityPortal()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BLOCK_CITY_PORTAL] = true;
	}

	void RootGuiLayout::hideBlockCityPortal()
	{
		if (m_layoutShown[LayoutId::BLOCK_CITY_PORTAL])
		{
			m_layoutShown[LayoutId::BLOCK_CITY_PORTAL] = false;
			RootGuiLayout::Instance()->showMainControl();
			playSoundByType(ST_Click);
		}
	}

	void RootGuiLayout::showSkyBlockPrivilegeShop()
	{
		showMainControl();
		m_layoutShown[LayoutId::SKYBLOCK_PRIVILEGE_SHOP] = true;
		gui_skyBlockAppShop* pLayout = dynamic_cast<gui_skyBlockAppShop*>(m_layouts[LayoutId::SKYBLOCK_PRIVILEGE_SHOP]);
		if (pLayout)
		{
			GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendSkyBlockBuyGoods(0, 0, pLayout->OperateType::UPDATE_DATA);
		}
	}

	void RootGuiLayout::showSkyBlockSignIn()
	{
		showMainControl();
		m_layoutShown[LayoutId::SKYBLOCK_SIGN_IN] = true;
	}

	bool RootGuiLayout::ShowSkyBlockSignInRewardDetail(int id)
	{
		m_layoutShown[LayoutId::SKYBLOCK_REWARD_DETAIL] = true;
		auto chest_reward_detail = dynamic_cast<gui_skyblockRewardDetail*>(m_layouts[LayoutId::SKYBLOCK_REWARD_DETAIL]);
		playSoundByType(ST_Click);
		if (chest_reward_detail)
		{
			chest_reward_detail->ShowRewardDetail(id);
		}
		return true;
	}

	void RootGuiLayout::hideSkyBlockRewardDetail()
	{
		m_layoutShown[LayoutId::SKYBLOCK_REWARD_DETAIL] = false;
	}

	bool RootGuiLayout::showSkyBlockRewardResult()
	{
		m_layoutShown[LayoutId::SKYBLOCK_REWARD_RESULT] = true;
		return true;
	}

	void RootGuiLayout::hideSkyBlockRewardResult()
	{
		m_layoutShown[LayoutId::SKYBLOCK_SIGN_IN] = false;
		m_layoutShown[LayoutId::SKYBLOCK_REWARD_RESULT] = false;
	}

	void RootGuiLayout::showSkyBlocReciveNewPri()
	{
		showMainControl();
		m_layoutShown[LayoutId::SKYBLOCK_REVIVE_NEW_PRI] = true;
	}

	void RootGuiLayout::hideSkyBlocReciveNewPri()
	{
		m_layoutShown[LayoutId::SKYBLOCK_REVIVE_NEW_PRI] = false;
	}



	void RootGuiLayout::showCameraDistanceSlider(bool isShow)
	{
		gui_mainControl* pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		pLayout->showCameraDistanceSlider(isShow);
	}

}
