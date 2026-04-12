#pragma once

#ifndef __ROOT_GUI_LAYOUT_HEADER__
#define __ROOT_GUI_LAYOUT_HEADER__

#include <functional>
#include "Core.h"
#include "GUI/gui_layout.h"
#include "Util/ClientEvents.h"
#include "UI/GUIStaticText.h"
#include "Inventory/IInventory.h"
#include "Util/SubscriptionGuard.h"
#include "BattleSummary.h"
#include "Structs/ChoiceRole.h"

namespace GameClient
{
	struct ResultEntry;
}

namespace BLOCKMAN
{
	class RootGuiLayout : public gui_layout, public Singleton<RootGuiLayout>
	{
	private:
		struct OpenChestArgs {
			int delayTickToOpenChest;
			IInventory* inventory;
			Vector3i blockPos;
			int face;
			Vector3 hisPos;
		};
	public:
		enum class LayoutId
		{
			MAIN,
			PLAYER_INVENTORY,
			LOADING_PAGE,
			PLAYER_INFO,
			GAME_TIP_DIALOG,
			CHEST,
			CHAT,
			MENU,
			TOOL_BAR,
			DEAD_SUMMARY,
			FINAL_SUMMARY,
			SHOP,
			SUMMARY_POINT,
			COUNTDOWN,
			APP_SHOP,
			SUMMARY_TIP,
			JAIL_BREAK_ROLE_CHOICE,
			TIP_DIALOG,
			BUY_GOODS_TIP,
			RANK,
			BLOCK_LOADING,
			MINI_MAP,
			MANOR,
			UPGRADE_MANOR,
			PERSONAL_SHOP,
			REAL_TIME_RANK,
			BUILD_WAR_GRADE,
			BUILD_WAR_GUESS,
			BUILD_WAR_GUESS_RESULT,
			MULTI_TIP_DIALOG,
			OUTPUT_CONFIG,
			SUPER_SHOP,
			KILL_PANEL,
			CAR_PROGRESS_TABLE,
			SUPERPRO_PSPANEL,
			MULTI_TIP_INTERACTION_DIALOG,
			RANCH,
			RANCH_TIP,
			RANCH_TIME_TIP,
			RANCH_ORDER,
			RANCH_UPGRAGE,
			RANCH_FACTORY_BUILD,
			RANCH_FARM_BUILD,
			RANCH_GAIN_TIP,
			RANCH_RANK,
			RANCH_PROSPERITY_RANK,
			OCCUPATION_UNLOCK,
			ENCHANTMENT_PANEL,
			TIGER_LOTTERY,
			HIDEANDSEEKHALL_RESULT,
			PIXEL_GUNHALL_MODE_SELECT,
			GUN_STORE,
			PIXEL_GUN_1V1,
			CHEST_LOTTERY,
			PIXEL_GUN_REVIVE,
			PIXEL_GUNHALL_ARMOR_UPGRADE,
			PIXEL_GUN_RESULT,
			SEASON_RANK,
			LOTTERY_CHEST_DETAIL,
			PIXEL_GUN_GAME_LVUP,
			BIRD_PACKANDFUSE,
			BIRD_PERSONAL_SHOP,
			BIRD_STORE,
			BIRD_LOTTERY,
			BIRD_TIP,
			BIRD_TASK_TIP,
			BIRD_ATLAS,
			BIRD_GAIN_TIP,
			BIRD_ACTIVITY,
			CHECKIN,
			BIRD_HELP,
			NOTIFICATION,
			SUPPLY_TIP,
			TASK_SYSTEM,
			BOX_LOTTERY,
			LOTTERY_BOX_DETAIL,
			BEDWAR_SEASON_RANK,
			BEDWAR_SEASON_REWARD,
			DRESS_SHOP,
			BEDWAR_RUNE,
			GUIDE_SYSTEM,
			BRIEF_INTRODUCTION,
			CHEST_REWARD_DETAIL,
			CHEST_REWARD_RESULT,
			BED_WAR_RESULT,
			PIXEL_GUN_INDIE_MAIN,
			PIXEL_GUN_GIFT_BAG_STORE,
			PIXEL_GUN_SIGN_IN,
			SEASON_REWARD,
			BLOCK_CITY_TIGER_LOTTERY,
			BLOCK_CITY_MANOR,
			BLOCK_CITY_TIP,
			BLOCK_CITY_STORE,
			BLOCK_CITY_FRIEND,
			BLOCK_CITY_RANK,
			BLOCK_CITY_CHOOSE_PAPER,
			SKYBLOCK_TASK_SYSTEM,
			SKYBLOCK_TIP,
			COMMON_TIP,
			BASE_MESSAGE,
			LEVEL_UP_PANEL,
			FURNACE_PANEL,
			SKYBLOCK_APP_SHOP,
			SKYBLOCK_RANK,
			SKYBLOCK_GAME,
			UPDATE_PRODUCT_INFO,
		    UNLOCK_PRODUCT_INFO,
			BLOCK_FORT_TIP,
			BLOCK_FORT_BUILDING_INFO,
			BLOCK_FORT_STORE,
			BLOCK_FORT_MANOR_UPGRADE,
			BLOCK_FORT_PACK,
			BOX_REWARD_RESULT,
			TYCOON_VEDIO_AD,
			WALKINGDEAD_STORE,
			WALKINGDEAD_DEATHSETTLEMENT,
			WALKINGDEAD_DESIGNATIONPANEL,
			BLOCK_CITY_PACK,
			BLOCK_CITY_HOUSE_SHOP,
			BLOCK_CITY_ELEVATOR,
			BLOCK_CITY_RECEIVE_FLOWER,
			BLOCK_CITY_CHARM_RANK,
			BLOCK_CITY_SAVE_FILE,
			BLOCK_CITY_RANK_AWARD,
			WALKINGDEAD_SupplyPopup,
			VIDEO_AD_TIP,
			PIXEL_CHICKEN_MAIN,
			PIXEL_CHICKEN_AWARD,
			SKYBLOCK_PRIVILEGE_SHOP,
			SKYBLOCK_SIGN_IN,
			SKYBLOCK_REWARD_DETAIL,
			SKYBLOCK_REWARD_RESULT,
			BLOCK_CITY_BUY_FLY,
			BLOCK_CITY_PORTAL,
			SKYBLOCK_REVIVE_NEW_PRI,
			LAYOUTID_MAX,
		};
	public:
		vector<LayoutId>::type m_needLoadView;

	private:
		SubscriptionGuard m_subscriptionGuard;

		static const LayoutId ALL_LAYOUT_ID[];
		map<LayoutId, gui_layout*>::type m_layouts;
		map<LayoutId, bool>::type m_layoutShown;
		list<LayoutId>::type m_orderLayoutList;

		bool openFurnaceTable(IInventory* inventory, const Vector3i & blockPos, int face, const Vector3 & hisPos);


		bool handlePlayerDeath(ui64 playerRuntimeId);
		bool onBackKeyDown();
		bool openChest(IInventory* inventory, const Vector3i & blockPos, int face, const Vector3 & hisPos);
		void initNeedLoadView();

	public:
		~RootGuiLayout();
		
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		
		void addLayout(gui_layout* layout)
		{
			gui_layout::addLayout(m_rootWindow, layout);
		}

		gui_layout* getLayout(int id)
		{
			if (id < 0 || id >= (int)LayoutId::LAYOUTID_MAX)
			{
				return nullptr;
			}
			return m_layouts[(LayoutId)id];
		}

		bool isLayoutShow(int id)
		{
			if (id < 0 || id >= (int)LayoutId::LAYOUTID_MAX)
			{
				return false;
			}
			return m_layoutShown[(LayoutId)id];
		}

		void loadGameGui();
		void showPlayerInventory();
		void showMainControl();
		void hideAllChildren();
		void recoverAllChildren();
		void showGameTipDialogManage(bool isShow);
		void showChestManage(bool idShow);
		void showChatGui();
		void createItemNameUi();
		bool showItemNameUi(ItemStackPtr itemStack);
		void showMenu();
		void showLoadPage();
		void showDeadSummaryGui();
		//void showBedWarDeadSummaryGui();
		void showFinalSummaryGui();
		bool showShop(EntityMerchant* merchant);
		bool shopUpdate(EntityMerchant* merchant);
		void showSummarPoint(const LORD::UVector2& pos, const std::map<int, int>& pointMap);
		void showSummaryRewardTip(const LORD::UVector2& pos, int total);
		void showCountdown(i8 second);
		void showCountdown(i8 second, int type);
		void hideCountdown();
		void showGameCloseTipDialog();
		void showAppShop(int page = -1);
		bool showPersonalShop();
		bool showSuperShop();
		int  getSelfTeamId();
		void changePlayerMaxHealth(float health);
		bool showJailBreakRoleChoice(ChoiceRole  role);
		bool showTipDialog();
		bool hideTipDialog();
		bool showBuyGoodsTip(bool bShow);
		bool showRank(const String& rankInfo, int EntityId);
		bool hideRank();
		bool showBlockLoading(bool bShow);
		void switchSerpece();
		void updateSwitchSerpece();
		int getViewNum();
		void showMiniMap(bool isShow) { m_isShowMiniMap = isShow; }
		void showParachuteBtn(bool isNeedOpen);
		void showBuildWarBlockBtn(bool isNeedOpen);
		void showDeadSummaryGui(const String& gameResult, bool isNextServer);
		bool isPlayerInventoryShown() { return m_layoutShown[LayoutId::PLAYER_INVENTORY]; }
		void showHideAndSeekBtn(bool isShowChangeActoBtn, bool isShowLockCameraBtn, bool isShowOthenBtn);
		bool isChestInventoryShown() { return m_layoutShown[LayoutId::CHEST]; }
		bool isLoading() { return m_layoutShown[LayoutId::LOADING_PAGE]; }
		bool showManor();
		void showUpgradeManor();
		void hidePlayerOperation();
		bool updateRealTimeRankData(String result);
		bool canPersonalShop();
		bool isShowRealTimeRankStatus();
		void isColseRealTimeRank(bool status);
		void showRealTimeRank();
		void hideRealTimeRank();
		void showBuildWarGrade(bool show);
		void showBuildWarGuessResult(bool show);
		void showBuildWarGuessUi(bool show);
		bool isBuildWarGradeShown() { return m_layoutShown[LayoutId::BUILD_WAR_GRADE]; }
		bool isMainControlShown() { return m_layoutShown[LayoutId::MAIN]; }
		bool isAppShopShown() { return m_layoutShown[LayoutId::APP_SHOP]; }
		
		bool isMenuShown() { return m_layoutShown[LayoutId::MENU]; }
		bool isChatShown() { return m_layoutShown[LayoutId::CHAT]; }
		bool isFinalSummaryShown() { return m_layoutShown[LayoutId::FINAL_SUMMARY]; }
		bool updateShopUnlockedState();
		void showMultiTipDialog();
		void hideMultiTipDialog();
		void showMultiTipInteractionDialog();
		void hideMultiTipInteractionDialog();
		void showRanch();
		void showRanchTip();
		void hideRanchTip();
		void showRanchOrder();
		void showRanchUpgrade();
		void hideRanchUpgrade();
		void showRanchBuildFactory();
		void showRanchBuildFarm();
		void setRanchTimeTipVisible(bool isShow);
		void setRanchGainTipVisible(bool isShow);
		void setRanchRankVisible(bool isShow);
		void setRanchProsperityRankVisible(bool isShow);
		void setOccupationUnlockVisible(bool isShow);
		void checkBackgroundMusic();
		bool setEnchantmentShow(bool isShow);
		void showTigerLottery();
		void hideTigerLottery();
		void showHideAndSeekHallResult();
		void showLackOfMoney(i32 difference, CurrencyType type);
		bool showBirdPackAndFuse();
		void showBirdPersonShop();
		void showBirdLottery();
		bool isShowBirdLottery();
		void showBirdStore();
		void setBirdTipVisible(bool isShow);
		bool isShowBirdStore();
		bool isBirdViewShow();
		
		void setBirdTaskTipVisible(bool isShow);
		bool isShowBirdTaskTip();
		void showBirdAtlas();
		void setBirdGainTipVisible(bool isShow);
		void showBirdActivity();
		bool showPixelGunHallModeSelect();
		void setPixelGun1V1Show(bool isShow);
		void setPixelGunReviveShow(bool isShow);
		void setPixelGunResultShow(bool isShow);
		//void setBedWarResultShow(bool isShow);
		bool showGunStore();
		bool showChestLottery();
		bool showPixelGunHallArmorUpgrade();
		bool showSeasonRank();
		bool showBedWarSeasonRank();
		bool showLotteryChestDetail(int type);
		void hideLotteryChestDetail();
		bool showChestRewardDetail(int id);
		void hideChestRewardDetail();
		bool showChestRewardResult();
		void hideChestRewardResult();
		void showPixelGunGameLvUp(bool show);
		bool isPixelGunResultOpen();
		void showQuicklyBuildBlockTip();
		void setQuicklyBuildBlockBtnStatus(bool status);
		bool showTaskSystem();
		bool showBriefIntroduction();
		void showPixelGunSignIn();
		void showCheckin();
		void showBirdHelp();
		void hideSelectableAction();
		void showUnlockSelectableActionTip(int actionId);
		bool showBedWarRune();

		bool showSkyBlockTaskSystem();
		bool showSkyBlockTip();

		bool showWindowByName(const String& name);
		bool showWindowById(int id);
		void showWindowByLayoutId(LayoutId id);
		void hideWindowByName(const String& name);
		void hideWindowById(int id);
		void hideWindowByLayoutId(LayoutId id);
		bool setGuidePanelShow(bool isShow);
		bool showPixelGunGiftBagStore(bool isShow);
		bool showGiftBagDetail(int type, int giftId);
		bool isShowTaskSystem();
		bool showLevelUpPanel(bool isShow);
		void onDispatchCommonData(LayoutId id, String key, String data);

		bool isShowBlockCityView();
		void showBlockCityTigerLottery();
		void hideBlockCityTigerLottery();
		void showBlockCityManor();
		void setBlockCityTipVisible(bool isShow);
		void showBlockCityStore();
		void showBlockCityFriend();
		void showBlockCityRank();
		void showBlockCityChooseView();
		void setCommonTipVisible(bool isShow);

		bool isShowSkyBlockTaskSystem();
		void notifyCraft();
		bool setFurnaceInventory(IInventory* inventory);
		void setFurnaceTime(int time, int maxTime);

		void showSkyBlockAppShop();

		void showWalkingDeadStore();
		void showWalkingDeadDeathSettlement();
		void showBlockCityPack();
		void showBlockCityHouseShop();
		void showBlockCityElevator();
		void hideBlockCityElevator();
		void setShowBlockCityReceiveFlower(bool isShow);
		void setShowBlockCityCharmRank(bool isShow);
		void setShowBlockCitySaveFile(bool isShow);
		void setShowBlockCityRankAward(bool isShow);
		void addOrderShowLayout(LayoutId layoutId);
		void showOrderLayout();
		bool ifLayoutIsInList(LayoutId layoutId);
		void removeLastOrderLayout();
		void sortOrderLayoutList();
		bool sortOrderLayoutFunc(LayoutId id1, LayoutId id2);
		void showSupplyPopup();
		bool showSkyBlockRank();
		bool showUpdateProductTip();
		bool showUnlockProductTip();
		void showSkyBlockAppShopPage(i32 pageId);

		void showBlockFortStore();
		void setBlockFortTipVisible(bool isShow);
		void showBlockFortBuildingInfo(bool isShow);
		void showBlockFortManorUpgrade();
		void showBlockFortPack();
		bool isGuiding();

		void showPixelChickenResult();
		void showSkyBlockPrivilegeShop();

		void showSkyBlockSignIn();
		bool ShowSkyBlockSignInRewardDetail(int id);
		bool showSkyBlockRewardResult();
		void hideSkyBlockRewardResult();
		void hideSkyBlockRewardDetail();
		void hideSkyBlocReciveNewPri();
		void showSkyBlocReciveNewPri();


		void showCameraDistanceSlider(bool isShow);
		void showBlockCityPortal();
		void hideBlockCityPortal();
		bool openCraftingTable();
		void checkGunUI();
		void armorPanelSellItem();
		void againDropItem(int index);

	private:
		bool m_isLoad = false;
		bool m_isChoiceRole = false;
		bool m_isShowItemName = false;
		bool m_isShowMiniMap = false;
		ui32 m_showItemNameTime = 0;
		OpenChestArgs m_openChestArgs;
		GUIStaticText* m_stItemName = nullptr;
		ChoiceRole m_choiceRole;
		LayoutId m_lastOrderLayout = LayoutId::MAIN;
	};
}

#endif // !__ROOT_GUI_LAYOUT_HEADER__

