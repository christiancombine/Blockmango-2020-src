#pragma once
#include <initializer_list>
#include "Util/Event.h"
#include "Inventory/IInventory.h"
#include "Inventory/Wallet.h"
#include "Structs/ChoiceRole.h"
#include "Enums/Enums.h"
#include "UI/GUIUDim.h"
#include "Setting/SkyBlockAppShopSetting.h"
#include "Setting/SkyBlockPrivilegeShopSetting.h"

namespace BLOCKMAN
{
	class EntityMerchant;
	class RanchCommon;
	class ProductQueue;
	class ProductRecipe;
	class RanchOrderInfo;
	class BroadcastMessage;
	class BirdGiftBag;
	class BirdGain;
	class BirdInfo;
	class Goods;
	class BlockCityPayment;
	class BlockCityStoreItem;
	class BlockCityHouseShopItem;
	class UserRequest;
	class Friend;
	class UserRank;
	class ManorHouse;
	class ThumbUpRecord;
	class BlockCityPortalItem;

	inline namespace NETWORK
	{
		struct NetworkConnectionFailureEvent : Event<NetworkConnectionFailureEvent> {};
		struct NetworkConnectionDisconnectEvent : Event<NetworkConnectionDisconnectEvent> {};
		struct NetworkConnectionTimeoutEvent : Event<NetworkConnectionTimeoutEvent> {};
		struct NetworkConnectionKickOutEvent : Event<NetworkConnectionKickOutEvent> {};
		struct NetworkConnectionSuccessfulEvent : Event<NetworkConnectionSuccessfulEvent> {};
		struct NetworkRetryConnectionEvent : Event<NetworkRetryConnectionEvent, bool> {};
	}

	inline namespace APPLICATION
	{
		struct LoadWorldEndEvent : Event<LoadWorldEndEvent> {};
		struct LoadWorldStartEvent : Event<LoadWorldStartEvent> {};
		struct LoginFailureEvent :Event<LoginFailureEvent> {};
		struct LoginTokenErrorEvent :Event<LoginTokenErrorEvent> {};
		struct LoginGetUserAttrFailEvent :Event<LoginGetUserAttrFailEvent> {};
		struct LoginWaitRoleAttrFailEvent :Event<LoginWaitRoleAttrFailEvent> {};
		struct LoginServerQuittingEvent :Event<LoginServerQuittingEvent> {};
		struct LoginResponseTimeoutEvent :Event<LoginResponseTimeoutEvent> {};
		struct LoginSuccEvent :Event<LoginSuccEvent> {};
		struct UserInEvent:Event<UserInEvent, const ui64, const String&, const String&, const int , const int, const int> {};
		struct UserOutEvent:Event<UserOutEvent, const ui64> {};
		struct GameStatusEvent :Event<GameStatusEvent, const int> {};
		struct CheckCSVersionFailEvent :Event<CheckCSVersionFailEvent> {};
		struct CheckCSVersionSuccEvent :Event<CheckCSVersionSuccEvent> {};
		struct BeKickedEvent :Event<BeKickedEvent> {};
		struct FriendOperationForAppHttpResultEvent:Event<FriendOperationForAppHttpResultEvent, int, ui64> {};
		struct FriendOperationForServerEvent :Event<FriendOperationForServerEvent, int, ui64> {};
		struct AllowUseItemStatusEvent :Event<AllowUseItemStatusEvent, const int> {};
		struct UserEnterEvent :Event<UserEnterEvent, vector<ui64>::type> {};
		struct UserInfoUpdataEvent : Event<UserInfoUpdataEvent, ui64, String, String, String, i32, i32, bool> {};
		struct AllocationGameFailure : Event<AllocationGameFailure> {};
		struct UpdateUserRelationEvent : Event<UpdateUserRelationEvent, const vector<i64>::type> {};
	}

	inline namespace GUI
	{
		struct ShowItemNameEvent : Event<ShowItemNameEvent, ItemStackPtr> {};
		struct DownloadMapFailureEvent :Event<DownloadMapFailureEvent> {};
		struct DownloadMapSuccessEvent :Event<DownloadMapSuccessEvent, bool> {};
		struct DownloadMapProgressEvent :Event<DownloadMapProgressEvent, const float, const float> {};
		struct GameoverEvent :Event<GameoverEvent, const String&> {};
		struct BreakBlockProgressEvent :Event<BreakBlockProgressEvent, float, RAYTRACE_TYPE> {};
		struct BreakBlockUiManageEvent :Event <BreakBlockUiManageEvent, bool, float, float > {};
		struct PlayerLifeStatusEvent : Event<PlayerLifeStatusEvent, ui64, bool> {};
		struct PlayerChangeItemEvent : Event<PlayerChangeItemEvent, int, bool> {};
		struct ReceiveDeadSummaryEvent : Event<ReceiveDeadSummaryEvent, const String&, bool> {};
		struct ReceiveFinalSummaryEvent : Event<ReceiveFinalSummaryEvent, const String&, bool> {};
		struct TeamBedDestroyedEvent : Event<TeamBedDestroyedEvent, int> {};
		struct GetAllPlayerTeamInfoEvent : Event<GetAllPlayerTeamInfoEvent, const String&> {};
		struct PlayerChangeTeamEvent : Event<PlayerChangeTeamEvent, ui64, int> {};
		struct AppShopUpdateEvent : Event<AppShopUpdateEvent> {};
		struct BuyGoodsResultEvent : Event<BuyGoodsResultEvent, const String&> {};
		struct ShowGuiJailBreakRoleChoiceEvent : Event<ShowGuiJailBreakRoleChoiceEvent, ChoiceRole> {};
		struct ShowVehicleTipDialogEvent : Event<ShowVehicleTipDialogEvent, int, int> {};
		struct showUnlockCommodityTipDialogEvent : Event<showUnlockCommodityTipDialogEvent, int, int, int, int, int, const String&> {};
		struct ShowGoAppShopTipDialogEvent : Event<ShowGoAppShopTipDialogEvent> {};
		struct ShowGoNpcMerchantTipDialogEvent : Event<ShowGoNpcMerchantTipDialogEvent, int, float, float, float, float> {};
		struct VehicleLockEvent : Event<VehicleLockEvent, bool> {};
		struct VehicleOnOffEvent : Event<VehicleOnOffEvent, bool, bool> {};
		struct GetMoneyEvent : Event<GetMoneyEvent, int> {};
		struct GetItemEvent : Event<GetItemEvent, int, int, int> {};
		struct GetGoodsEvent : Event<GetGoodsEvent, const String&, int> {};
		struct BuyCommodityResultEvent : Event<BuyCommodityResultEvent, const String&> {};
		struct ShowReviveEvent : Event<ShowReviveEvent, int, int, int, int, int> {};
		struct UpdateBlockProgressEvent : Event<UpdateBlockProgressEvent, float, bool> {};
		struct MemberLeftAndKillUpdateEvent : Event<MemberLeftAndKillUpdateEvent, int, int> {};
		struct ChangeSkillTypeEvent : Event<ChangeSkillTypeEvent, SkillType, float, float> {};
		struct ShowPickUpItemTipDialogEvent : Event<ShowPickUpItemTipDialogEvent, i32, i32, i32, CurrencyType> {};
		struct ShowResultTipDialogEvent : Event<ShowResultTipDialogEvent, ResultCode, const String&> {};
		struct ReceiveRankResultEvent : Event<ReceiveRankResultEvent, const String&> {};
		struct SyncChangePlayerActortEvent : Event<SyncChangePlayerActortEvent, int, int, const String&> {};
		struct SyncGameTimeShowUiEvent : Event<SyncGameTimeShowUiEvent, bool, int> {};
		struct SchemeticPlaceBlockFinishEvent : Event<SchemeticPlaceBlockFinishEvent> {};
		struct PlaceBlocksEvent : Event<PlaceBlocksEvent> {};
		struct SyncClientPeerPropEvent : Event<SyncClientPeerPropEvent, const String&> {};
		struct ChangeHeartEvent : Event<ChangeHeartEvent, int, int> {};
		struct ChangeIsCanJumpEvent : Event<ChangeIsCanJumpEvent, bool> {};
		struct ShowGetSellManorTipDialogEvent : Event<ShowGetSellManorTipDialogEvent> {};
		struct ManorOnUpdateEvent : Event<ManorOnUpdateEvent> {};
		struct ManorHouseSelectEvent : Event<ManorHouseSelectEvent, ManorHouse*> {};
		struct ShowLackOfMoneyTipDialogEvent : Event<ShowLackOfMoneyTipDialogEvent, i32, CurrencyType> {};
		struct SetPutFurnitureIdEvent : Event<SetPutFurnitureIdEvent, i32> {};
		struct SetManorBtnVisibleEvent : Event<SetManorBtnVisibleEvent, bool> {};
		struct ShowPlayerOperationEvent : Event<ShowPlayerOperationEvent, ui64, const String&> {};
		struct ShowManorBuildTipDialogEvent : Event<ShowManorBuildTipDialogEvent> {};
		struct SureBuildManorHouseEvent : Event<SureBuildManorHouseEvent> {};
		struct RefreshManorUiEvent : Event<RefreshManorUiEvent> {};
		struct ManorMessageRemoveEvent : Event<ManorMessageRemoveEvent, const String&, const String&> {};
		struct UpdateGameMonsterInfoEvent : Event<UpdateGameMonsterInfoEvent, int, int, int> {};
		struct UpdateBossBloodStripEvent : Event<UpdateBossBloodStripEvent, int, int> {};
		struct ChangeSwitchablePropsEvent : Event<ChangeSwitchablePropsEvent, const String&> {};
		struct ChangeUpgradePropsEvent : Event<ChangeUpgradePropsEvent, const String&> {};
		struct PersonalShopTipEvent : Event<PersonalShopTipEvent, const String&> {};
		struct ShowSwitchablePropEvent : Event<ShowSwitchablePropEvent, const String&, const String&, const String&, int> {};
		struct ShowUpgradeResourceEvent : Event<ShowUpgradeResourceEvent, int, int, int, const String&> {};
		struct UpdateBasementLifeEvent : Event<UpdateBasementLifeEvent, float, float> {};
		struct ShowCustomTipMsgEvent : Event<ShowCustomTipMsgEvent, const String&, const String&> {};
		struct ShowContentTipMsgEvent : Event<ShowContentTipMsgEvent, const String&, const String&> {};
		struct ServerSyncGameTypeEvent : Event<ServerSyncGameTypeEvent> {};
		struct ShowGetManorTipDialogEvent : Event<ShowGetManorTipDialogEvent> {};
		struct ShowSellManorTipDialogEvent : Event<ShowSellManorTipDialogEvent> {};
		struct ShowShopItemTipEvent : Event<ShowShopItemTipEvent, int, int, const String&, int, const String&, const String&, int > {};
		struct ShowBuildWarPlayerNum : Event<ShowBuildWarPlayerNum, ui8, ui8> {};
		struct ShowBuildWarLeftTime : Event<ShowBuildWarLeftTime, bool, ui16, const String&> {};
		struct ShowBuildWarGrade : Event<ShowBuildWarGrade, const String&, ui8> {};
		struct ShowBuildWarGuessResult : Event<ShowBuildWarGuessResult, bool, int> {};
		struct ShowBuildWarGuessUi : Event<ShowBuildWarGuessUi, const String&, int> {};
		struct SetBuildWarGuessShow : Event<SetBuildWarGuessShow, bool> {};
		struct ShowBuildWarGuessTipDialogEvent : Event<ShowBuildWarGuessTipDialogEvent, int, const String&> {};
		struct ReceiveFinalSummaryExtraEvent : Event<ReceiveFinalSummaryExtraEvent, bool, const String&, int> {};
		struct ShowMulitTipDialogEvent : Event<ShowMulitTipDialogEvent, const String&> {};
		struct TeamResourcesUpdateEvent : Event<TeamResourcesUpdateEvent, const String&> {};
		struct OpenPersonalShopEvent : Event<OpenPersonalShopEvent> {};
		struct UpdateBuildProgressEvent : Event<UpdateBuildProgressEvent, const String&, int, int, const String&> {};
		struct ChangeEnchantmentPropsEvent : Event<ChangeEnchantmentPropsEvent, const String&> {};
		struct ChangePersonalSuperPropsEvent :Event<ChangePersonalSuperPropsEvent, const String&> {};
		struct ShowConsumeCoinTipEvent : Event<ShowConsumeCoinTipEvent, const String&, int, int, const String&> {};
		struct OpenSuperShopEvent : Event<OpenSuperShopEvent> {};
		struct ChangeSuperPropsEvent : Event<ChangeSuperPropsEvent, const String&> {};
		struct SuperShopTipEvent : Event<SuperShopTipEvent, const String&> {};
		struct ShowSuperPropEvent : Event<ShowSuperPropEvent, const String&, const String&, const String&> {};
		struct ChangeDefenseEvent : Event<ChangeDefenseEvent, int, int> {};
		struct ChangeAttackEvent : Event<ChangeAttackEvent, int, int> {};
		struct ChangeAppleEvent : Event<ChangeAppleEvent, int, int> {};
		struct AutoCastSkillEvent :Event<AutoCastSkillEvent> {};
		struct CastSkillEvent :Event<CastSkillEvent> {};
		struct ShootWarningEvent : Event<ShootWarningEvent, float, bool> {};
		struct PlayEndingAnimationEvent : Event<PlayEndingAnimationEvent, const String&, bool, int> {};
		struct ReceiveKillMsgEvent : Event<ReceiveKillMsgEvent, const String&> {};
		struct PersonalShopAreaEvent : Event<PersonalShopAreaEvent, const Vector3 &, const Vector3 &> {};
		struct ChangeCustomPropsEvent : Event<ChangeCustomPropsEvent, const String&> {};
		struct UpdateCustomPropsEvent : Event<UpdateCustomPropsEvent, const String&> {};
		struct UpdateCarDirectionEvent : Event<UpdateCarDirectionEvent, int, int> {};
		struct UpdateCarProgressEvent : Event<UpdateCarProgressEvent, int, float> {};
		struct ShowCannonBtnEvent : Event<ShowCannonBtnEvent, bool, int> {};
		struct DisableRechargeEvent : Event<DisableRechargeEvent> {};
		struct ShowKillsEvent : Event<ShowKillsEvent, const String&, uint, i32, i32> {};

		struct ShowMulitTipInteractionDialogEvent : Event<ShowMulitTipInteractionDialogEvent, const String&> {};
		struct RanchDataChangeEvent : Event<RanchDataChangeEvent> {};
		struct RanchHouseChangeEvent : Event<RanchHouseChangeEvent> {};
		struct RanchStorageChangeEvent : Event<RanchStorageChangeEvent> {};
		struct RanchRankChangeEvent : Event<RanchRankChangeEvent> {};
		struct RanchBuildChangeEvent : Event<RanchBuildChangeEvent> {};
		struct RanchFactoryBuildChangeEvent : Event<RanchFactoryBuildChangeEvent> {};
		struct RanchFarmingBuildChangeEvent : Event<RanchFarmingBuildChangeEvent,i32> {};
		struct ShowRanchExpandTipEvent : Event<ShowRanchExpandTipEvent, i32, i32, const vector<RanchCommon>::type&, const vector<RanchCommon>::type& > {};
		struct ShowRanchStorageUpgradeTipEvent : Event<ShowRanchStorageUpgradeTipEvent> {};
		struct ShowRanchTimeTipEvent : Event<ShowRanchTimeTipEvent, i32, i32, i32, const String &> {};
		struct ShowRanchUnlockItemsTipEvent : Event<ShowRanchUnlockItemsTipEvent, const vector<i32>::type&> {};
		struct ShowRanchCommonTipEvent : Event<ShowRanchCommonTipEvent, const String&> {};
		struct ShowRanchBuildFarmEvent : Event<ShowRanchBuildFarmEvent, i32> {};
		struct ShowRanchBuildFactoryEvent : Event<ShowRanchBuildFactoryEvent, i32> {};
		
		struct ShowRanchQueueUnlockEvent : Event<ShowRanchQueueUnlockEvent, i32, i32, i32, i32, const String & > {};
		struct ShowRanchLockOfMoneyEvent : Event<ShowRanchLockOfMoneyEvent, i32, i32> {};
		struct ShowRanchLockItemsEvent : Event<ShowRanchLockItemsEvent, const vector<RanchCommon>::type&> {};
		struct ShowRanchSellItemEvent : Event<ShowRanchSellItemEvent, const UVector2&, const RanchCommon&> {};
		struct ShowRanchGainTipEvent : Event<ShowRanchGainTipEvent, const vector<RanchCommon>::type&> {};
		struct RanchBuildFarmUpdateEvent : Event<RanchBuildFarmUpdateEvent, i32> {};
		struct RanchBuildFactoryUpdateEvent : Event<RanchBuildFactoryUpdateEvent, i32> {};
		struct RanchItemClickEvent : Event<RanchItemClickEvent, i32, const String &> {};
		struct RanchStorageOpenEvent : Event<RanchStorageOpenEvent> {};
		struct RanchOrderChangeEvent : Event<RanchOrderChangeEvent> {};
		struct RanchOrderItemClickEvent : Event<RanchOrderItemClickEvent, const UVector2&, const RanchOrderInfo&> {};
		struct RanchOrderItemHideEvent : Event<RanchOrderItemHideEvent> {};
		struct ShowRanchRankEvent : Event<ShowRanchRankEvent, i32> {};
		struct ShowRanchTakeEvent : Event<ShowRanchTakeEvent, i32, const UVector2&> {};
		struct FrendDataChangeEvent : Event<FrendDataChangeEvent> {};
		struct ClanMemberDataChangeEvent : Event<ClanMemberDataChangeEvent> {};
		
		struct ShowRanchExBeginTipDialogEvent : Event<ShowRanchExBeginTipDialogEvent> {};
		struct ShowRanchExTaskFinishTipDialogEvent : Event<ShowRanchExTaskFinishTipDialogEvent> {};
		struct ShowRanchExTip : Event<ShowRanchExTip, ui16, ui16> {};
		struct ShowRanchExCurrentItemInfo : Event<ShowRanchExCurrentItemInfo, bool, const String &, int, int> {};
		struct ShowRanchExTask : Event<ShowRanchExTask, bool, const String &> {};
		struct ShowRanchExItem : Event<ShowRanchExItem, bool, const String &> {};
		struct RanchFriendOperateHideEvent : Event<RanchFriendOperateHideEvent> {};
		struct RanchFriendClickEvent : Event<RanchFriendClickEvent, i64, const String &> {};
		struct ShowRanchFriendHelpListEvent : Event<ShowRanchFriendHelpListEvent, i64> {};
		struct MailUpdateEvent : Event<MailUpdateEvent> {};
		struct FriendUpdateEvent : Event<FriendUpdateEvent> {};
		struct RanchAchievementChangeEvent : Event<RanchAchievementChangeEvent> {};
		struct BroadcastReceiveEvent : Event<BroadcastReceiveEvent, const BroadcastMessage&> {};
		struct UserActorChangeEvent : Event<UserActorChangeEvent, i64> {};
		struct ShowCropInfoEvent : Event<ShowCropInfoEvent, Vector3i, i32 , i32, i32, i32> {};
		struct ShowSpeedUpQueueEvent : Event<ShowSpeedUpQueueEvent, i32, i32, i32, i32, i32> {};
		struct ShowRanchOrderOperateLockItemsEvent : Event<ShowRanchOrderOperateLockItemsEvent, const vector<RanchCommon>::type&, i32, i32> {};
		struct RanchRecipeItemClickEvent : Event<RanchRecipeItemClickEvent,  const ProductRecipe&> {};
		struct ShowRanchLockItemsByBuildOperationEvent : Event<ShowRanchLockItemsByBuildOperationEvent, const vector<RanchCommon>::type&, i32, i32> {};
		struct ShowRanchStorageFullTipEvent : Event<ShowRanchStorageFullTipEvent, const String&> {};
		struct ShowRanchLockItemsByUpgradeStorageEvent : Event<ShowRanchLockItemsByUpgradeStorageEvent, const vector<RanchCommon>::type&> {};
		struct ShowRanchLockItemsByOrderHelpEvent : Event<ShowRanchLockItemsByOrderHelpEvent, const vector<RanchCommon>::type&, i64> {};
		struct GameVideoChangeEvent : Event<GameVideoChangeEvent, i32> {};
		struct ShowOccupationUnlockTip : Event<ShowOccupationUnlockTip, const String&> {};
		struct ShowKeepItemTipEvent : Event<ShowKeepItemTipEvent, int, int, int> {};
		struct ShowEnchantMentEvent : Event<ShowEnchantMentEvent, const String &> {};
		struct ShowEnchantMentQuickEvent : Event<ShowEnchantMentQuickEvent, int, int, int> {};

		struct ShowPixelGunHallNotOpenEvent : Event<ShowPixelGunHallNotOpenEvent, int> {};
		struct ShowPixelGunHallLvNotEnoughEvent : Event<ShowPixelGunHallLvNotEnoughEvent, int, int> {};
		struct ShowPixelGunHallLvMapLockDiamondEvent : Event<ShowPixelGunHallLvMapLockDiamondEvent, int, int> {};

		struct ShowPixelGunModeSelectEvent : Event<ShowPixelGunModeSelectEvent> {};
		struct ShowPixelGunHallModeSelectReWardInfoEvent : Event<ShowPixelGunHallModeSelectReWardInfoEvent, int> {};
		struct ShowPixelGunHallMapSelectChangeEvent : Event<ShowPixelGunHallMapSelectChangeEvent, int> {};
		struct ShowWindowByNameEvent : Event<ShowWindowByNameEvent, const String &> {};
		struct SetGuidePanelShowEvent : Event<SetGuidePanelShowEvent, bool> {};
		struct ShowGuideInfoEvent : Event<ShowGuideInfoEvent, bool, int, const UVector2&, int, int> {};
		struct ShowGuideEffectEvent : Event<ShowGuideEffectEvent, String, StringArray> {};

		struct OpenGunStoreEvent : Event<OpenGunStoreEvent> {};
		struct GunStoreTabChangeEvent : Event<GunStoreTabChangeEvent, int> {};
		struct GunStoreGunItemClickEvent : Event<GunStoreGunItemClickEvent, int> {};
		struct GunStorePropItemClickEvent : Event<GunStorePropItemClickEvent, int> {};
		struct GunStoreModuleItemClickEvent : Event<GunStoreModuleItemClickEvent, int> {};
		struct ShowStoreGunDetailEvent : Event<ShowStoreGunDetailEvent, int> {};
		struct ShowStorePropDetailEvent : Event<ShowStorePropDetailEvent, int> {};
		struct ShowStoreModuleDetailEvent : Event<ShowStoreModuleDetailEvent, int> {};

		struct ShowPixelGunHallInfo : Event<ShowPixelGunHallInfo, int, int, int, int, bool> {};
		struct ShowSkyBlockInfo : Event<ShowSkyBlockInfo, int, i64, i64,int, String, bool, bool> {};

		struct OpenChestLotteryEvent : Event<OpenChestLotteryEvent> {};
		struct ShowArmorUpgradeEvent : Event<ShowArmorUpgradeEvent> {};
		struct ShowLotteryChestDetailEvent : Event<ShowLotteryChestDetailEvent, int> {};
		struct ShowChestRewardDetailEvent : Event<ShowChestRewardDetailEvent, int> {};
		struct ShowChestRewardResultEvent : Event<ShowChestRewardResultEvent, int> {};
		struct OpenLotteryChestEvent : Event<OpenLotteryChestEvent, int> {};
		struct UpdateLotteryChestKeyEvent : Event<UpdateLotteryChestKeyEvent, int> {};
		struct ShowGiftBagDetailEvent : Event<ShowGiftBagDetailEvent, int, int> {};

		struct OpenSeasonRankEvent : Event<OpenSeasonRankEvent> {};
		struct UpdateSeasonRuleEvent : Event<UpdateSeasonRuleEvent, const String&> {};
		struct UpdateSeasonRankDataEvent : Event<UpdateSeasonRankDataEvent, int, const String&> {};
		struct UpdateCurrentSeasonInfoEvent : Event<UpdateCurrentSeasonInfoEvent, int, int, int, int> {};

		struct OpenBedWarSeasonRankEvent : Event<OpenBedWarSeasonRankEvent> {};
		struct UpdateBedWarSeasonRuleEvent : Event<UpdateBedWarSeasonRuleEvent, const String&> {};
		struct UpdateBedWarSeasonRankDataEvent : Event<UpdateBedWarSeasonRankDataEvent, int, const String&> {};
		struct UpdateBedWarCurrentSeasonInfoEvent : Event<UpdateBedWarCurrentSeasonInfoEvent, int, int, int, int> {};

		struct OpenBedWarRuneEvent : Event<OpenBedWarRuneEvent> {};

		struct UpdateDisarmamentStatusEvent : Event<UpdateDisarmamentStatusEvent, bool> {};
		struct TigerLotteryDataEvent : Event<TigerLotteryDataEvent, const String&> {};
		struct TigerLotteryResultEvent : Event<TigerLotteryResultEvent, const String&, const String&, const String&> {};
		struct HideAndSeekHallResult : Event<HideAndSeekHallResult, const String&> {};
		struct UpdateBirdPackEvent : Event<UpdateBirdPackEvent> {};
		struct ShowBirdInfoClickEvent : Event<ShowBirdInfoClickEvent, i64> {};
		struct BirdFoodItemClickEvent : Event<BirdFoodItemClickEvent, i32, i32> {};
		struct BirdFuseRemoveItemEvent : Event<BirdFuseRemoveItemEvent, i64> {};
		struct BirdRemoveFuseItemEvent : Event<BirdRemoveFuseItemEvent, i64> {};
		struct BirdFusePutItemEvent : Event<BirdFusePutItemEvent, i64, const String&, const String&> {};
		struct UpdateBirdStoreEvent : Event<UpdateBirdStoreEvent> {};
		struct UpdateBirdPersonalShopInfoEvent : Event<UpdateBirdPersonalShopInfoEvent, const BirdGiftBag&, i32> {};
		struct UpdateBirdPersonalShopEvent : Event<UpdateBirdPersonalShopEvent> {};
		struct UpdateBirdAtlasEvent : Event<UpdateBirdAtlasEvent> {};	
		struct UpdateBirdActivityEvent : Event<UpdateBirdActivityEvent> {};		
		struct ShowBirdStoreEvent : Event<ShowBirdStoreEvent, i32, i32> {};
		struct ShowBirdStoreByNpcEvent : Event<ShowBirdStoreByNpcEvent, const String&> {};
		struct ShowBirdCommonTipEvent : Event<ShowBirdCommonTipEvent, const String&> {};
		struct ShowBirdLockOfMoneyEvent : Event<ShowBirdLockOfMoneyEvent, i32, i32> {};
		struct ShowBirdUnlockNestTipEvent : Event<ShowBirdUnlockNestTipEvent, i32, i32> {};
		struct ShowBirdExpandCarryTipEvent : Event<ShowBirdExpandCarryTipEvent, i32, i32> {};
		struct ShowBirdCarryFullTipEvent : Event<ShowBirdCarryFullTipEvent, i32, i32> {};
		struct ShowBirdCapacityFullTipEvent : Event<ShowBirdCapacityFullTipEvent, i32, i32> {};
		struct ShowBirdExpandCapacityTipEvent : Event<ShowBirdExpandCapacityTipEvent, i32, i32> {};
		struct ShowBirdLotteryTipEvent : Event<ShowBirdLotteryTipEvent, i32, i32> {};
		struct ShowBirdOpenChestTipEvent : Event<ShowBirdOpenChestTipEvent, i32, i32, i32, const String&> {};
		struct ShowBirdLotteryByNpcEvent : Event<ShowBirdLotteryByNpcEvent, i32> {};
		struct UpdateBirdLotteryEvent : Event<UpdateBirdLotteryEvent> {};
		struct ShowBirdTaskTipEvent : Event<ShowBirdTaskTipEvent, const String&> {};
		struct BirdTaskChangeEvent : Event<BirdTaskChangeEvent> {};
		struct ShowBirdGainTipEvent : Event<ShowBirdGainTipEvent, const vector<BirdGain>::type&> {};
		struct BirdPackOpenEvent : Event<BirdPackOpenEvent> {};
		struct BirdNestExpandEvent : Event<BirdNestExpandEvent> {};
		struct HideBirdActivityEvent : Event<HideBirdActivityEvent> {};
		struct ShowBirdLotteryResultTipEvent : Event<ShowBirdLotteryResultTipEvent, const String &, const String &, const BirdInfo &> {};
		struct BirdAddScoreEvent : Event<BirdAddScoreEvent, i32, i32> {};
		struct BirdBuffChangeEvent : Event<BirdBuffChangeEvent> {};
		struct BirdAcceptTaskEvent : Event<BirdAcceptTaskEvent> {};
		struct BirdBuyPermissionEvent : Event<BirdBuyPermissionEvent, const String &> {};
		struct ShowBirdPersonalShopEvent : Event<ShowBirdPersonalShopEvent, i32> {};

		struct ShowAppleFreshTimeEvent : Event<ShowAppleFreshTimeEvent, int> {};

		struct BirdAddMessageEvent : Event<BirdAddMessageEvent, const String&> {};
		struct BirdShowPackGuideEvent : Event<BirdShowPackGuideEvent> {};
		struct BirdShowCarryGuideEvent : Event<BirdShowCarryGuideEvent> {};
		struct ShowBirdNpcCommonTipEvent : Event<ShowBirdNpcCommonTipEvent, const String&, const vector<String>::type &> {};
		struct ShowBirdOpenEggChestTipEvent : Event<ShowBirdOpenEggChestTipEvent, i32, i32, i32> {};
		struct BirdCheckinDataUpdateEvent : Event<BirdCheckinDataUpdateEvent, const String&> {};
		struct ShowBirdToolInfoClickEvent : Event<ShowBirdToolInfoClickEvent, i32> {};
		struct BirdShowBirdFeedGuideEvent : Event<BirdShowBirdFeedGuideEvent> {};
		struct ShowBirdFeedCookieGuideEvent : Event<ShowBirdFeedCookieGuideEvent> {};
		struct ClickBirdFeedCookieGuideEvent : Event<ClickBirdFeedCookieGuideEvent> {};
		struct ShowBirdWingTipEvent : Event<ShowBirdWingTipEvent, i32> {};
		struct ShowBirdDeleteTipEvent : Event<ShowBirdDeleteTipEvent, i64, const String &> {};

		struct ReceiveNotificationEvent : Event<ReceiveNotificationEvent, int, const String &> {};
		struct AddHealthSupplyTipEvent : Event<AddHealthSupplyTipEvent, const String &, int> {};
		struct AddDefenseSupplyTipEvent : Event<AddDefenseSupplyTipEvent, const String &, int> {};
		struct AddBulletSupplyTipEvent : Event<AddBulletSupplyTipEvent, const String &, int> {};

		struct ShowPixelGunTaskIconEvent : Event<ShowPixelGunTaskIconEvent> {};
		struct ChangeDayActiveValueEvent : Event<ChangeDayActiveValueEvent, int> {};
		struct ChangeWeekActiveValueEvent : Event<ChangeWeekActiveValueEvent, int> {};
		struct ChangeDayActiveTimeEvent : Event<ChangeDayActiveTimeEvent, int> {};
		struct ChangeWeekActiveTimeEvent : Event<ChangeWeekActiveTimeEvent, int> {};
		struct ReceiveTaskActiveRewardEvent : Event<ReceiveTaskActiveRewardEvent, int> {};
		struct ShowRewardsEvent : Event<ShowRewardsEvent, const String &> {};
		struct HideTaskActiveRewardEvent : Event<HideTaskActiveRewardEvent> {};
		struct ShowRewardsAdBtnEvent : Event<ShowRewardsAdBtnEvent, String> {};

		struct TaskMainLineTabChangeEvent : Event<TaskMainLineTabChangeEvent, int> {};
		struct UpdateMainLineRedPoiontEvent : Event<UpdateMainLineRedPoiontEvent, bool> {};
		struct UpdateDailyRedPoiontEvent : Event<UpdateDailyRedPoiontEvent, bool> {};
		struct CheckDailyRedPointEvent : Event<CheckDailyRedPointEvent> {};
		struct CheckMainLineRedPointEvent : Event<CheckMainLineRedPointEvent> {};
		struct ShowMainLineTaskLvNotEnoughEvent : Event<ShowMainLineTaskLvNotEnoughEvent, int> {};
		struct BuyActionSuccessEvent : Event<BuyActionSuccessEvent, i32> {};
		struct UpdateSelectableActionEvent : Event<UpdateSelectableActionEvent> {};
		struct UpdatePixelGunGiftRefreshTimeEvent : Event<UpdatePixelGunGiftRefreshTimeEvent, int> {};

		struct ShowQuickBuildBlockEvent : Event<ShowQuickBuildBlockEvent, bool> {};

		struct BedWarRuneTroughSelectChangeEvent : Event<BedWarRuneTroughSelectChangeEvent, int, int> {};
		struct BedWarRuneClickEvent : Event<BedWarRuneClickEvent, int> {};
		struct BedWarRuneReclaimClickEvent : Event<BedWarRuneReclaimClickEvent, int> {};
		struct BedWarLeftEvent : Event<BedWarLeftEvent> {};
		struct BedWarRightEvent : Event<BedWarRightEvent> {};
		struct BedWarOneKeyUnloadEvent : Event<BedWarOneKeyUnloadEvent> {};
		struct BedWarOneKeyAssembleEvent : Event<BedWarOneKeyAssembleEvent> {};
		struct BedWarProSumChangeEvent : Event<BedWarProSumChangeEvent> {};
		struct BedWarRuneBackEvent : Event<BedWarRuneBackEvent> {};

		struct UpdateGoldAdInfoEvent : Event<UpdateGoldAdInfoEvent, int, int> {};
		struct ShowGoldAdRewardEvent : Event<ShowGoldAdRewardEvent, int, int, int> {};
		struct ClickPlayerInventoryOutside : Event<ClickPlayerInventoryOutside> {};

		struct ShowBedWarReclaimTipEvent : Event<ShowBedWarReclaimTipEvent, int, const String&, int> {};
		struct ShowBedWarReclaimAllTipEvent : Event<ShowBedWarReclaimAllTipEvent, int, int, int> {};
		struct ShowBedWarRuneUnlockTipEvent : Event<ShowBedWarRuneUnlockTipEvent, int, int> {};

		struct AttackScoreEvent : Event<AttackScoreEvent, i32, i32> {};

		struct UpdatePixelGunVipTimeEvent : Event<UpdatePixelGunVipTimeEvent, int> {};
		struct UpdatePlayerDressEvent : Event<UpdatePlayerDressEvent> {};

		struct BoxRewardShowEvent : Event<BoxRewardShowEvent, bool> {};
		struct BoxLotterVisibleEvent : Event<BoxLotterVisibleEvent, bool> {};

		struct ShowMessageEvent : Event<ShowMessageEvent, int, const String&, i32, bool> {};
		struct ShowLevelUpPanelEvent : Event<ShowLevelUpPanelEvent, bool> {};
		struct VideoAdVisibleEvent : Event<VideoAdVisibleEvent, bool> {};
		struct SetBlockCityLotteryByNpcEvent : Event<SetBlockCityLotteryByNpcEvent, i32> {};
		struct UpdateBlockCityManorEvent : Event<UpdateBlockCityManorEvent> {};
		struct ShowBlockCityTigerLotteryTipEvent : Event<ShowBlockCityTigerLotteryTipEvent, i32> {};
		struct ShowBlockCityShowCommonTipEvent : Event<ShowBlockCityShowCommonTipEvent, const vector<BlockCityPayment>::type&, i32, const String&> {};
		struct UpdateBlockCityStoreEvent : Event<UpdateBlockCityStoreEvent> {};
		struct BlockCityStoreItemClickEvent : Event<BlockCityStoreItemClickEvent, const BlockCityStoreItem&> {};
		struct UpdateUserRequestEvent : Event<UpdateUserRequestEvent, const vector<UserRequest>::type&> {};
		struct UpdateUserFriendEvent : Event<UpdateUserFriendEvent, const vector<Friend>::type&> {};
		struct ShowBlockCityLockItemsEvent : Event<ShowBlockCityLockItemsEvent> {};
		struct BlockCityStorageOperationEvent : Event<BlockCityStorageOperationEvent, i32 , i32 , i32, bool> {};
		struct BlockCityInviteEvent : Event<BlockCityInviteEvent, ui64, String, String, i32, i32> {};
		struct ShowRotateViewEvent : Event<ShowRotateViewEvent, i32, i32> {};
		struct UpdateBlockCityGolbalRankEvent : Event<UpdateBlockCityGolbalRankEvent, const vector<UserRank>::type&> {};
		struct UpdateBlockCityFriendRankEvent : Event<UpdateBlockCityFriendRankEvent, const vector<UserRank>::type&> {};
		struct UpdateBlockCityMyRankEvent : Event<UpdateBlockCityMyRankEvent, UserRank &> {};
		struct UpdateIsHaveViewShowEvent : Event<UpdateIsHaveViewShowEvent, bool> {};
		struct UpdateBlockCityUserRequestRankEvent : Event<UpdateBlockCityUserRequestRankEvent, const vector<UserRank>::type&> {};
		struct BlockCityDeleteFriendEvent : Event<BlockCityDeleteFriendEvent, i64> {};
		struct BlockCitySendDeleteFriendIdEvent : Event<BlockCitySendDeleteFriendIdEvent, i64> {};
		struct BlockCityFriendInviteEvent : Event<BlockCityFriendInviteEvent, i64> {};
		struct BlockCityShowManorEvent : Event<BlockCityShowManorEvent> {};
		struct ShowBlockCityTipEvent : Event<ShowBlockCityTipEvent, const String&> {};
		struct ShowCommonTipEvent : Event<ShowCommonTipEvent, vector<String>::type, i32> {};
		struct ShowActorCommonTipEvent : Event<ShowActorCommonTipEvent, const String&> {};
		struct BlockCityShowChooseItemEvent : Event<BlockCityShowChooseItemEvent, i32, vector<i32>::type> {};
		struct BlockCityChooseItemClickEvent : Event<BlockCityChooseItemClickEvent, i32> {};
		struct ShowBlockCityBuyFlyTipEvent : Event<ShowBlockCityBuyFlyTipEvent> {};
		struct UpdateBlockCityPlayerInfoEvent : Event<UpdateBlockCityPlayerInfoEvent> {};


		struct SkyBlockTaskMainLineTabChangeEvent : Event<SkyBlockTaskMainLineTabChangeEvent, int> {};
		struct SkyBlockUpdateMainLineTaskDataEvent : Event<SkyBlockUpdateMainLineTaskDataEvent, int, int, const String&> {};
		struct SkyBlockCheckMainLineRedPointEvent : Event<SkyBlockCheckMainLineRedPointEvent> {};
		struct ShowSkyBlockMainLineTaskLvNotEnoughEvent : Event<ShowSkyBlockMainLineTaskLvNotEnoughEvent, const String> {};
		struct SkyBlockUpdateMainLineRedPoiontEvent : Event<SkyBlockUpdateMainLineRedPoiontEvent, bool> {};
		struct ShowSkyBlockTaskIconEvent : Event<ShowSkyBlockTaskIconEvent> {};
		struct CraftAllSwitchEvent : Event<CraftAllSwitchEvent, bool> {};
		struct SkyBlockUpdateDareTaskDataEvent : Event<SkyBlockUpdateDareTaskDataEvent, int, int, const String&> {};
		struct SkyBlockCheckDareRedPointEvent : Event<SkyBlockCheckDareRedPointEvent, bool> {};
		struct UpdateSkyBlockDareTaskInfoEvent : Event<UpdateSkyBlockDareTaskInfoEvent, const String&> {};
		struct SkyBlockAchieveMaxDareTaskEvent : Event<SkyBlockAchieveMaxDareTaskEvent, bool> {};
		struct SkyBlockUpdataShopInfoEvent : Event<SkyBlockUpdataShopInfoEvent, int, int, const String&> {};
		struct SkyBlockShopClickItemEvent : Event<SkyBlockShopClickItemEvent,  const SkyBlockGoods, int> {};
		struct RefreshSellItemEvent : Event<RefreshSellItemEvent> {};

		struct SetMoveStateEvent : Event<SetMoveStateEvent, int> {};
		struct StoreUpgradeCompleteEvent : Event<StoreUpgradeCompleteEvent, int> {};
		struct UpgradeCompleteItemSelectEvent : Event<UpgradeCompleteItemSelectEvent, int> {};
		struct HideStoreUpgradeRedPointEvent : Event<HideStoreUpgradeRedPointEvent> {};
		struct DefenseUpgradeCompleteEvent : Event<DefenseUpgradeCompleteEvent> {};
		struct updateSkyBlockRankDataEvent : Event<updateSkyBlockRankDataEvent, const String&> {};
		struct notifyGetGoodsByItemIdEvent : Event<notifyGetGoodsByItemIdEvent, int, int, int, const String&> {};
		struct unLockDareTaskEvent : Event<unLockDareTaskEvent, bool> {};
		struct WalkingDeadStoreTabChangeEvent : Event<WalkingDeadStoreTabChangeEvent, int> {};
		struct WalkingDeadShowStoreEvent : Event<WalkingDeadShowStoreEvent, int> {};
		struct WalkingDeadStoreByTravellerEvent : Event<WalkingDeadStoreByTravellerEvent, const String&> {};
		struct WalkingDeadStoreUpdateGoodsEvent : Event<WalkingDeadStoreUpdateGoodsEvent, int, int> {};
		struct UpdateWalkingDeadStoreAppShopInfoEvent : Event<UpdateWalkingDeadStoreAppShopInfoEvent, int> {};
		struct UpdateDesignationDetailEvent :Event<UpdateDesignationDetailEvent, const String&, const String&> {};
		struct WalkingDeadDeathSettlementEvent :Event<WalkingDeadDeathSettlementEvent, const String &> {};
		struct UpdateWalkingDeadSupplyEvent :Event<UpdateWalkingDeadSupplyEvent, const String &> {};
		struct WalkingDeadShowVipInfoEvent :Event<WalkingDeadShowVipInfoEvent, int, int64_t> {};
		struct EmailRedPointVisibleEvent : Event<EmailRedPointVisibleEvent, bool> {};
		struct WalkingDeadshowSupplyPopupEvent : Event<WalkingDeadshowSupplyPopupEvent, int> {};
		struct OnGunUpgradeEvent : Event<OnGunUpgradeEvent, bool> {};
		struct SyncPixelGunTicketEvent : Event<SyncPixelGunTicketEvent, const String&>{};
		struct SetWarningRangeEvent : Event<SetWarningRangeEvent, float, float, float> {};

		struct ShowBlockFortCommonTipEvent : Event<ShowBlockFortCommonTipEvent, const String&> {};
		struct ShowBlockFortFunctionTipEvent : Event<ShowBlockFortFunctionTipEvent, const String&, i32> {};
		struct ShowBlockFortLackOfMoneyTipEvent : Event<ShowBlockFortLackOfMoneyTipEvent, i32, i32> {};
		struct ShowBlockFortBuildingInfoEvent : Event<ShowBlockFortBuildingInfoEvent, i32, i32> {};
		struct UpdateBlockFortIsShowTransferTipEvent : Event<UpdateBlockFortIsShowTransferTipEvent, bool> {};
		struct BlockFortSureTransferEvent : Event<BlockFortSureTransferEvent> {};
		struct ShowFloatTipEvent : Event<ShowFloatTipEvent, const String&> {};
		struct BlockFortStoreUpdateEvent : Event<BlockFortStoreUpdateEvent, i32> {};
		struct UpdateBlockFortStoreEvent : Event<UpdateBlockFortStoreEvent> {};
		struct BlockFortManorManagerUpdateEvent : Event<BlockFortManorManagerUpdateEvent> {};
		struct UpdateBlockFortPlayerInfoEvent : Event<UpdateBlockFortPlayerInfoEvent> {};
		struct BlockFortSendDeleteFriendIdEvent : Event<BlockFortSendDeleteFriendIdEvent, i64> {};

		struct SendChickenWarningEvent : Event<SendChickenWarningEvent, const Vector3 &, int> {};
		struct ShowChickenWatchEvent : Event<ShowChickenWatchEvent> {};
		struct UpdateBlockCityHouseShopEvent : Event<UpdateBlockCityHouseShopEvent> {};
		struct ShowBlockCityHouseShopEvent : Event<ShowBlockCityHouseShopEvent, i32, i32> {};
		struct ShowBlockCityHouseShopByNpcEvent : Event<ShowBlockCityHouseShopByNpcEvent, const String&> {};
		struct UpdateBlockCityPlayerPackEvent : Event<UpdateBlockCityPlayerPackEvent> {};
		struct BlockCityPackItemClickEvent : Event<BlockCityPackItemClickEvent, const BlockCityHouseShopItem&> {};
		struct ShowBlockCityElevatorEvent : Event<ShowBlockCityElevatorEvent, i32> {};
		struct UpdateBlockCityFriendCharmRankEvent : Event<UpdateBlockCityFriendCharmRankEvent, const vector<UserRank>::type&, const String&> {};
		struct IsShowBlockCityCharmRankEvent : Event<IsShowBlockCityCharmRankEvent, bool> {};
		struct UpdateBlockCityGolbalCharmRankEvent : Event<UpdateBlockCityGolbalCharmRankEvent, const vector<UserRank>::type&, const String&> {};
		struct UpdateBlockCityMyCharmRankEvent : Event<UpdateBlockCityMyCharmRankEvent, UserRank &, const String&> {};
		struct UpdateBlockCityWeekRewardEvent : Event<UpdateBlockCityWeekRewardEvent> {};
		struct UpdateBlockCityArchiveEvent : Event<UpdateBlockCityArchiveEvent> {};
		struct BlockCityArchiveItemClickEvent : Event<BlockCityArchiveItemClickEvent, i32> {};
		struct BlockCityArchiveSureEvent : Event<BlockCityArchiveSureEvent, i32> {};
		struct UpdateBlockCityThumbUpRecordEvent : Event<UpdateBlockCityThumbUpRecordEvent, const vector<ThumbUpRecord>::type&> {};
		struct DeleteBlockCityThumbUpRecordEvent : Event<DeleteBlockCityThumbUpRecordEvent> {};
		struct UpdateBlockCityMyLastCharmRankEvent : Event<UpdateBlockCityMyLastCharmRankEvent, i32> {};
		struct BlockCityGetPraiseEvent : Event<BlockCityGetPraiseEvent> {};
		struct FriendRequestEvent : Event<FriendRequestEvent, ui64> {};
		struct BlockCityCaptureStatusEvent : Event<BlockCityCaptureStatusEvent, bool> {};
		struct IsShowBlockCityEditEffectEvent : Event<IsShowBlockCityEditEffectEvent, GUIString, bool> {};
		struct SkyBlockPrivilegeShopClickItemEvent : Event<SkyBlockPrivilegeShopClickItemEvent, const SkyBlockPrivilegeGoods, int> {};
		struct SkyBlockPrivilegeRewardClickItemEvent : Event<SkyBlockPrivilegeRewardClickItemEvent, const SkyBlockPrivilegeReward, int> {};
		struct ShowSkyBlockSignInRewardDetailEvent : Event<ShowSkyBlockSignInRewardDetailEvent, int> {};
		struct ShowSkyBlockCostMuchMoneyEvent : Event<ShowSkyBlockCostMuchMoneyEvent, const String&, int, int, int, int, const String&> {};
		struct ShowSkyBlockSignInIconEvent : Event<ShowSkyBlockSignInIconEvent, bool> {};

		struct BlockCitySyncCDRewardEvent : Event<BlockCitySyncCDRewardEvent, const i32&, const String&> {};
		struct SendBirdMoneyAdInfoEvent : Event<SendBirdMoneyAdInfoEvent, i32, i32, i32> {};
		struct SendBirdShowFeedAdEvent : Event<SendBirdShowFeedAdEvent, bool, i32> {};
		struct ClickBirdFeedCdEvent : Event<ClickBirdFeedCdEvent> {};
		struct BlockCityPortalItemClickEvent : Event<BlockCityPortalItemClickEvent, const BlockCityPortalItem&> {};
		struct setRaceVehileCountdownEvent : Event<setRaceVehileCountdownEvent, bool> {};
		struct setRaceVehileResultEvent : Event<setRaceVehileResultEvent, bool> {};
		struct BroadcastRaceVehicleResultEvent : Event<BroadcastRaceVehicleResultEvent, const String&, const String&> {};
		struct UpdateBulletReload : Event<UpdateBulletReload, float> {};
		struct UpdateShootingFeedback : Event<UpdateShootingFeedback, bool> {};
		struct SkyBlockReciveNewPriClickItemEvent : Event<SkyBlockReciveNewPriClickItemEvent, const SkyBlockPrivilegeReward, int> {};
		struct WalkingDeadAttackPlayerEvent : Event<WalkingDeadAttackPlayerEvent, ui64> {};
        struct WalkingDeadAttackedPlayerEvent : Event<WalkingDeadAttackedPlayerEvent, ui64> {};
		struct WalkingDeadDiePlayerEvent :Event<WalkingDeadDiePlayerEvent, ui64, float, float, float> {};
		struct setRaceVehileStatusEvent : Event<setRaceVehileStatusEvent, bool> {};
		struct BlockCityRaceBtnEvent :Event<BlockCityRaceBtnEvent, i64> {};
		struct SkyBlockSupremeClickItemEvent : Event<SkyBlockSupremeClickItemEvent, int> {};

	}

	inline namespace KEY
	{
		struct BackKeyDownEvent : Event<BackKeyDownEvent> {};
	}

	inline namespace WORLD
	{
		struct PlayerDeathEvent : Event<PlayerDeathEvent, ui64> {};
		struct CraftingTableActivateEvent : Event<CraftingTableActivateEvent> {};
		struct ChestActivateEvent : Event<ChestActivateEvent, IInventory*, const Vector3i &, int, const Vector3 &> {};
		struct MerchantActivateEvent : Event<MerchantActivateEvent, EntityMerchant*> {};
		struct MerchantUpdateEvent : Event<MerchantUpdateEvent, EntityMerchant*> {};
		struct RankActivateEvent : Event<RankActivateEvent, const String&,int> {};
		struct ChickenAirDropPosition : Event<ChickenAirDropPosition, const Vector3 &> {};
		struct UpdateUnlockedCommodityEvent : Event<UpdateUnlockedCommodityEvent> {};
		struct FurnaceTableActivateEvent : Event<FurnaceTableActivateEvent, IInventory*, const Vector3i &, int, const Vector3 &> {};
	}

	inline namespace CHAT
	{
		struct ChatMessageReceiveEvent :Event<ChatMessageReceiveEvent, const String&, bool> {};
		struct TopSystemNotificationEvent :Event<TopSystemNotificationEvent, const String&, i32> {};
		struct BottomSystemNotificationEvent :Event<BottomSystemNotificationEvent, const String&, i32> {};
		struct CenterSystemNotificationEvent :Event<CenterSystemNotificationEvent, const String&, i32> {};
		struct TeamResourcesShowNotificationEvent : Event<TeamResourcesShowNotificationEvent, const String&, i32> {};
	}

	inline namespace MISC
	{
		struct ClientBlockChangeRecordClearEvent : Event<ClientBlockChangeRecordClearEvent, int, int> {};
	}

	inline namespace MAP_MANAGER
	{
		struct MapManagerDownloadProgress : Event<MapManagerDownloadProgress, int, uint32_t> {};
		struct MapManagerDownloadFail : Event<MapManagerDownloadFail, int, const std::string&, bool> {};
		struct MapManagerDownloadSucc : Event<MapManagerDownloadSucc> {};
	}
}
