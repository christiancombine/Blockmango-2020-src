#pragma once

#include "Script/Event/ScriptEvent.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Network/ClientPeer.h"
#include "Entity/DamageSource.h"
#include "Inventory/IInventory.h"
#include "Blockman/World/ServerWorld.h"
#include "TileEntity/TileEntityInventory.h"
#include "RoomManager.h"
#include "Network/RoomClient.h"
#include "Script/VarProxy/BaseValueProxy.h"
#include "Game/Manor.h"
#include "Enums/Enums.h"

using namespace BLOCKMAN;

namespace SCRIPT_EVENT
{
	// respath  webVer, RankAddr,  serverworld roomgameconfig
	struct GameInitEvent :public ScriptEvent<GameInitEvent, const char *, const char *, ServerWorld*, RoomGameConfig&> {};

	//ClientPeer
	struct PlayerLoginEvent :public ScriptEvent<PlayerLoginEvent, ClientPeer*, String, bool, int> {};

	// platformUid
	struct PlayerLogoutEvent :public ScriptEvent<PlayerLogoutEvent, ui64> {};

	// diePlayer iskillbyplayer entityPlayer
	struct PlayerDieEvent :public ScriptEvent<PlayerDieEvent, EntityPlayerMP*, bool, EntityPlayerMP*> {};

	// diePlayer entityPlayer
	struct PlayerDieTipEvent :public ScriptEvent<PlayerDieTipEvent, EntityPlayerMP*, EntityPlayerMP*> {};

	// hurt player hurt from hurtvalue
	struct PlayerAttackedEvent :public ScriptEvent<PlayerAttackedEvent,
		EntityPlayerMP*, EntityPlayerMP *, const char * , FloatProxy*> {};

	struct PlayerHurtEvent :public ScriptEvent<PlayerHurtEvent,
		EntityPlayerMP*, EntityPlayerMP*, const char*, float> {};

	//move player x , y , z
	struct PlayerMoveEvent :public ScriptEvent<PlayerMoveEvent, EntityPlayerMP*, float, float, float > {};

	//player dynamic attr
	struct PlayerDynamicAttrEvent :public ScriptEvent<PlayerDynamicAttrEvent, const UserAttrInfo&, ui64> {};

	//Inventory
	struct ChestOpenEvent :public ScriptEvent<ChestOpenEvent, Vector3i, TileEntityChest*, ui64> {};

	struct FurnaceOpenEvent :public ScriptEvent<FurnaceOpenEvent, Vector3i, TileEntityFurnace*, ui64> {};

	// block break blokid, meta, x, y, z
	struct BlockBreakWithMetaEvent:public ScriptEvent<BlockBreakWithMetaEvent, ui64, int, int, Vector3i, int, int> {};

	// block break blockid, x, y, z, gunId
	struct BlockBreakWithGunEvent :public ScriptEvent<BlockBreakWithGunEvent, ui64, int, Vector3i, int> {};

	// use prop 
	struct PlayerUsePropEvent :public ScriptEvent<PlayerUsePropEvent, ui64, const char *> {};

	// player place block box judge
	struct PlayerPlaceBlockBoxJudgeEvent : public ScriptEvent<PlayerPlaceBlockBoxJudgeEvent, ui64, int> {};

	// player spawn event
	struct PlayerFirstSpawnEvent :public ScriptEvent<PlayerFirstSpawnEvent, ClientPeer*> {};
	// player Rebirth event
	struct PlayerRespawnEvent : public ScriptEvent<PlayerRespawnEvent, ClientPeer*> {};

	struct PlayerAfterRespawnEvent : public ScriptEvent<PlayerAfterRespawnEvent, ClientPeer*> {};

	// exchange item event
	// rakssid
	// exchange type
	//				1, playerinv to chestinv
	//				2, chestinv to playerinv
	// itemid
	// itemMeta
	// itemnum
	// bullets
	// chest position
	struct PlayerExchangeItemEvent :public ScriptEvent<PlayerExchangeItemEvent, ui64, ui16, int, int, int, int, Vector3i> {};

	// change item in hand

	// item id
	// item meta
	struct PlayerChangeItemInHandEvent :public ScriptEvent<PlayerChangeItemInHandEvent, ui64, int , int > {};
	
	// use throwable item event
	struct PlayerUseThrowableItemEvent :public ScriptEvent<PlayerUseThrowableItemEvent, int> {};

	// player shopping 
	// rakssId type goodsId
	struct PlayerBuyCommodityEvent : public ScriptEvent<PlayerBuyCommodityEvent, ui64, ui8, CommodityInfo*, ShoppingAttr*, IntProxy*> {};

	// pickup item
	struct PlayerPickupItemEvent : public ScriptEvent<PlayerPickupItemEvent, ui64, int, int, i32> {};

	// pickup item end
	struct PlayerPickupItemEndEvent : public ScriptEvent<PlayerPickupItemEndEvent, ui64, int, int, int, int> {};

	// player buy goods 
	// rakssId type goodsId
	struct PlayerBuyGoodsEvent : public ScriptEvent<PlayerBuyGoodsEvent, ui64, i32, i32, IntProxy*, BoolProxy*> {};

	// bug goods add item handle
	struct PlayerBuyGoodsAddItemEvent : public ScriptEvent<PlayerBuyGoodsAddItemEvent, ui64, i32, i32, i32> {};

	// player buy goods 
	// rakssId type goodsId
	struct PlayerBuyGoodsSuccessEvent : public ScriptEvent<PlayerBuyGoodsSuccessEvent, ui64, i32, i32> {};

	// currency change
	struct PlayerCurrencyChangeEvent : public ScriptEvent<PlayerCurrencyChangeEvent, ui64, ui64> {};
	
	// player unlock commodity
	struct PlayerUnlockCommodityEvent : public ScriptEvent<PlayerUnlockCommodityEvent, ui64, int, int> {};

	// select role
	struct PlayerSelectRoleEvent : public ScriptEvent<PlayerSelectRoleEvent, ui64, ui32> {};

	// bug vehicle
	struct PlayerBuyVehicleSuccessEvent : public ScriptEvent<PlayerBuyVehicleSuccessEvent, ui64, ui32> {};

	// player ready
	struct PlayerReadyEvent : public ScriptEvent<PlayerReadyEvent, ui64> {};

	// player use item
	struct PlayerUseCommonItemEvent : public ScriptEvent<PlayerUseCommonItemEvent, ui64, ui32> {};

	// player buy Respawn result
	struct PlayerBuyRespawnResultEvent : public ScriptEvent<PlayerBuyRespawnResultEvent, ui64, ui32> {};

	// zscore from redis back
	struct ZScoreFromRedisDBEvent : public ScriptEvent<ZScoreFromRedisDBEvent, String, String, int, int> {};

	// zrange from redis back
	struct ZRangeFromRedisDBEvent : public ScriptEvent<ZRangeFromRedisDBEvent, String, String> {};

	// player drop item
	struct PlayerDropItemEvent : public ScriptEvent<PlayerDropItemEvent, ui64, ui32, ui32, int, int> {};

	// player remove item
	struct PlayerRemoveItemEvent : public ScriptEvent<PlayerRemoveItemEvent, ui64, ui32, IntProxy*, i32> {};

	// player click teleport
	struct PlayerClickTeleportEvent : public ScriptEvent<PlayerClickTeleportEvent, ui64> {};

	// player close container
	struct PlayerCloseContainerEvent : public ScriptEvent<PlayerCloseContainerEvent, ui64, Vector3i> {};

	// player skill event
	struct PlayerSkillTypeEvent : public ScriptEvent<PlayerSkillTypeEvent, ui64, int, bool> {};
	
	//change player actor
	struct PlayerClickChangeActorEvent : public  ScriptEvent<PlayerClickChangeActorEvent, ui64> {};

	//change player actor
	struct PlayerChangeActorEvent : public  ScriptEvent<PlayerChangeActorEvent, ui64> {};

	//player buy pick up item
	struct PlayerBuyPickupItemEvent : public  ScriptEvent <PlayerBuyPickupItemEvent, ui64, i32, i32> {};

	//player buy pick up item result
	struct PlayerBuyPickupItemResultEvent : public  ScriptEvent <PlayerBuyPickupItemResultEvent, ui64, bool, i32, i32, i32, i32, i32> {};

	// attack actor npc
	struct PlayerAttackActorNpcEvent : public  ScriptEvent<PlayerAttackActorNpcEvent, ui64, ui32> {};

	// review player
	struct PlayerReviewPlayerEvent : public  ScriptEvent<PlayerReviewPlayerEvent, ui64, ui64> {};

	// buy actor after
	struct PlayerBuyActorResultEvent : public ScriptEvent<PlayerBuyActorResultEvent, ui64, bool> {};
	
	// get manor 
	struct PlayerGetManorEvent : public ScriptEvent<PlayerGetManorEvent, ui64, String> {};

	// sell manor
	struct PlayerSellManorEvent : public ScriptEvent <PlayerSellManorEvent, ui64, bool, String > {};

	// update manor result
	struct PlayerUpdateManorResultEvent : public ScriptEvent <PlayerUpdateManorResultEvent, ui64, bool, String> {};

	// buy house result 
	struct PlayerBuyHouseResultEvent : public ScriptEvent <PlayerBuyHouseResultEvent, ui64, bool, i32, i32, i32> {};

	// buy furinture result
	struct PlayerBuyFurnitureResultEvent : public ScriptEvent <PlayerBuyFurnitureResultEvent, ui64, bool, i32, i32> {};

	// build house
	struct PlayerBuildHouseEvent : public ScriptEvent <PlayerBuildHouseEvent, ui64, i32> {};

	// put furniture
	struct PlayerPutFurnitureEvent : public ScriptEvent <PlayerPutFurnitureEvent, ui64, i32, PutFurnitureStatus> {};

	// recycle furniture
	struct PlayerRecycleFurnitureEvent : public ScriptEvent <PlayerRecycleFurnitureEvent, ui64, i32> {};

	// player click manor transform button
	struct PlayerManorTeleportEvent : public ScriptEvent <PlayerManorTeleportEvent, ui64> {};

	// update real time rank
	struct PlayerUpdateRealTimeRankEvent : public ScriptEvent<PlayerUpdateRealTimeRankEvent, ui64> {};

	// buy switchable prop
	struct PlayerBuySwitchablePropEvent : public ScriptEvent<PlayerBuySwitchablePropEvent, ui64, String, IntProxy*> {};

	// buy upgrade prop
	struct PlayerBuyUpgradePropEvent : public ScriptEvent<PlayerBuyUpgradePropEvent, ui64, String, IntProxy*> {};

	// attack creature
	struct PlayerAttackCreatureEvent : public  ScriptEvent<PlayerAttackCreatureEvent, ui64, ui32, ui32, Vector3, float, int> {};
	
	// upgrade resource 
	struct PlayerUpgradeResourceEvent : public ScriptEvent<PlayerUpgradeResourceEvent, ui64, int> {};

	//crops update 
	struct BlockCropsUpdateEvent : public ScriptEvent<BlockCropsUpdateEvent, ui64, int, int, int, int, Vector3i, int> {};

	// crops pick
	struct BlockCropsPickEvent : public ScriptEvent<BlockCropsPickEvent, ui64, int, int, int, Vector3i, int, int> {};

	// crops steal
	struct BlockCropsStealEvent : public ScriptEvent<BlockCropsStealEvent, ui64, int, int, int, Vector3i, i32> {};

	// crops can steal delete
	struct BlockCropsStealDeleteEvent : public ScriptEvent<BlockCropsStealDeleteEvent, ui64, int, int, int, Vector3i, i32> {};

	// get exchange rate from platform money to game money
	struct GetMoneyExchangeRateEvent : public ScriptEvent<GetMoneyExchangeRateEvent, IntProxy*> {};

	//load user's manor infomation 
	struct PlayerLoadManorInfoEvent : public ScriptEvent<PlayerLoadManorInfoEvent, ui64, String> {};

	// load manor master infomation 
	struct PlayerLoadManorMasterInfoEvent : public ScriptEvent<PlayerLoadManorMasterInfoEvent, ui64, String> {};

	// load  manor config 
	struct LoadManorConfigEvent : public ScriptEvent<LoadManorConfigEvent, String> {};

	// load  manor inventory 
	struct LoadManorInventoryEvent : public ScriptEvent<LoadManorInventoryEvent, ui64, String, String> {};

	// load manor block
	struct LoadManorBlockEvent : public ScriptEvent<LoadManorBlockEvent, ui64, String, String> {};

	// block crops place
	struct BlockCropsPlaceEvent : public ScriptEvent<BlockCropsPlaceEvent, ui64, int, Vector3i , int> {};

	//player transfer to target monor, return false if target not in the same server 
	struct PlayerCallOnTargetManorEvent : public ScriptEvent<PlayerCallOnTargetManorEvent, ui64, ui64, i32, bool, i64> {};

	// watch respawn
	struct PlayerWatchRespawnEvent : public ScriptEvent<PlayerWatchRespawnEvent, ui64> {};
	
	// player praise result
	struct PlayerPraiseResultEvent : public ScriptEvent<PlayerPraiseResultEvent, ui64, int> {};

	//custom tip result
	struct CustomTipMsgEvent : public ScriptEvent<CustomTipMsgEvent, ui64, String, bool> {};

	// manor charm rank
	struct LoadManorCharmRankEvent : public ScriptEvent<LoadManorCharmRankEvent, bool, String> {};

	// manor potential rank
	struct LoadManorPotentialRankEvent : public ScriptEvent<LoadManorPotentialRankEvent, bool, String> {};

	// my manor rank
	struct LoadMyManorRankEvent : public ScriptEvent<LoadMyManorRankEvent, bool, ui64, String> {};

	//user manor release
	struct UserManorReleaseEvent : public ScriptEvent<UserManorReleaseEvent, ui64> {};

	// open shop
	struct BuildWarOpenShopEvent : public ScriptEvent<BuildWarOpenShopEvent, ui64> {};

	// build war grade
	struct BuildWarGradeEvent : public ScriptEvent<BuildWarGradeEvent, ui64, ui8> {};

	// build war open guess result
	struct BuildWarOpenGuessResultEvent : public ScriptEvent<BuildWarOpenGuessResultEvent, ui64> {};

	// build war guess visit
	struct BuildWarGuessVisitEvent : public ScriptEvent<BuildWarGuessVisitEvent, ui64, ui64> {};

	// build war guess suc
	struct BuildWarGuessSucEvent : public ScriptEvent<BuildWarGuessSucEvent, ui64, int> {};

	// build war guess suc
	struct BuildWarShowResultEvent : public ScriptEvent<BuildWarShowResultEvent, ui64> {};
	
	// attack sessionNpc
	struct PlayerAttackSessionNpcEvent : public ScriptEvent<PlayerAttackSessionNpcEvent, ui64, ui32> {};

	// consume tip result
	struct ConsumeTipMsgEvent : public ScriptEvent<ConsumeTipMsgEvent, ui64, String, bool> {};

	// consume diamonds result
	struct ConsumeDiamondsEvent : public ScriptEvent<ConsumeDiamondsEvent, ui64, bool, String, String, String> {};

	// consume golds result
	struct ConsumeGoldsEvent : public ScriptEvent<ConsumeGoldsEvent, ui64, bool, String, String, String> {};

	// buy enchantment prop
	struct PlayerBuyEnchantmentPropEvent : public ScriptEvent<PlayerBuyEnchantmentPropEvent, ui64, String, IntProxy*> {};

	// buy super prop
	struct PlayerBuySuperPropEvent : public ScriptEvent<PlayerBuySuperPropEvent, ui64, String, IntProxy*> {};

	// buy custom prop
	struct PlayerBuyCustomPropEvent : public ScriptEvent<PlayerBuyCustomPropEvent, ui64, String, IntProxy*> {};

	// back hall
	struct PlayerBackHallEvent : public ScriptEvent<PlayerBackHallEvent, ui64> {};

	// long hit entity
	struct PlayerLongHitEntityEvent : public ScriptEvent<PlayerLongHitEntityEvent, ui64, ui32> {};

	// buy bullet
	struct PlayerBuyBulletEvent : public ScriptEvent<PlayerBuyBulletEvent, ui64> {};

	// player buy ranch item result
	struct PlayerBuyRanchItemResultEvent : public  ScriptEvent <PlayerBuyRanchItemResultEvent, ui64, bool, std::vector<RanchCommon> > {};

	// player ranch receive event
	struct PlayerRanchReceiveEvent : public  ScriptEvent <PlayerRanchReceiveEvent, ui64, ui64> {};

	// player ranch expand event
	struct PlayerRanchExpandEvent : public  ScriptEvent <PlayerRanchExpandEvent, ui64, i32> {};

	// player ranch upgrade house event
	struct PlayerRanchUpgradeHouseEvent : public  ScriptEvent <PlayerRanchUpgradeHouseEvent, ui64> {};

	// player ranch upgrade storage event
	struct PlayerRanchUpgradeStorageEvent : public  ScriptEvent <PlayerRanchUpgradeStorageEvent, ui64> {};

	// player ranch use cube upgrade storage event
	struct PlayerRanchUseCubeUpgradeStorageEvent : public  ScriptEvent <PlayerRanchUseCubeUpgradeStorageEvent, ui64> {};

	//player ranch sell item
	struct PlayerRanchSellItemEvent : public ScriptEvent <PlayerRanchSellItemEvent, ui64, i32, i32> {};

	// player ranch shortcut event
	struct PlayerRanchShortcutEvent : public ScriptEvent <PlayerRanchShortcutEvent, ui64, bool, i32> {};

	// player ranch destroy house
	struct PlayerRanchDestroyHouseEvent : public ScriptEvent <PlayerRanchDestroyHouseEvent, ui64> {};

	// get ranch shortcut cost
	struct GetRanchShortcutCostEvent : public ScriptEvent <GetRanchShortcutCostEvent, ui64, i32, BoolProxy*, IntProxy*> {};

	// can ranch storage save items. return true if enough capacity to save items
	struct CanRanchStorageSaveItemsEvent : public ScriptEvent <CanRanchStorageSaveItemsEvent, ui64, std::vector<RanchCommon>, i32, IntProxy*, BoolProxy*> {};

	// get ranch building item cost
	struct GetRanchBuildItemCostEvent : public ScriptEvent <GetRanchBuildItemCostEvent, ui64, i32, i32, i32, BoolProxy*, IntProxy*, IntProxy*> {};

	// player buy ranch building item result
	struct PlayerBuyRanchBuildItemResultEvent : public ScriptEvent <PlayerBuyRanchBuildItemResultEvent, ui64, i32, bool, i32, i32> {};

	// player take away items from ranch building cargo
	struct PlayerTakeAwayRanchBuildCargoEvent : public ScriptEvent <PlayerTakeAwayRanchBuildCargoEvent, ui64, i32, i32, i32> {};

	// player ranch building queue product
	struct PlayerRanchBuildingQueueProductEvent : public ScriptEvent <PlayerRanchBuildingQueueProductEvent, ui64, i32, i32, i32> {};

	// player ranch building queue speed up
	struct PlayerRanchBuildingQueueSpeedUpEvent : public ScriptEvent <PlayerRanchBuildingQueueSpeedUpEvent, ui64, i32, i32, i32, bool> {};

	//player ranch building queue unlock
	struct PlayerRanchBuildingQueueUnlockEvent : public ScriptEvent <PlayerRanchBuildingQueueUnlockEvent, ui64, i32, i32, i32, bool> {};

	// get ranch building speedup cost
	struct GetRanchBuildingSpeedUpCostEvent : public ScriptEvent <GetRanchBuildingSpeedUpCostEvent, ui64, i32, i32, i32, BoolProxy*, IntProxy*> {};

	// get ranch building queue unlock cost
	struct GetRanchBuildQueueUnlockCostEvent : public ScriptEvent <GetRanchBuildQueueUnlockCostEvent, ui64, i32, i32, BoolProxy*, IntProxy*, IntProxy*> {};

	// create explore map
	struct CreateExploreMapEvent : public ScriptEvent<CreateExploreMapEvent, ui64> {};

	// create add exp
	struct AddExpEvent: public ScriptEvent<AddExpEvent, ui64, int> {};

	// create add rune
	struct AddRuneEvent : public ScriptEvent<AddRuneEvent, ui64, int , int> {};

	// rancher explore begin
	struct RancherExBeginEvent : public ScriptEvent <RancherExBeginEvent, ui64> {};

	// rancher explore task finish
	struct RancherExTaskFinishEvent : public ScriptEvent <RancherExTaskFinishEvent, ui64, int, int> {};

	// rancher player help finish event
	struct PlayerRanchHelpFinishEvent : public ScriptEvent <PlayerRanchHelpFinishEvent, ui64, ui64, i32> {};

	// ranch player receive mail reward event
	struct PlayerRanchReceiveMailRewardEvent : public ScriptEvent <PlayerRanchReceiveMailRewardEvent, ui64, i64> {};

	// ranch player send every day gift event
	struct PlayerRanchSendEveryDayGiftEvent : public ScriptEvent <PlayerRanchSendEveryDayGiftEvent, ui64, i64> {};

	// ranch player broadcast refresh order
	struct RefreshPlayerRanchOrderEvent : public ScriptEvent <RefreshPlayerRanchOrderEvent, ui64> {};

	// ranch player receive achievement reward
	struct PlayerRanchReceiveAchievementRewardEvent : public ScriptEvent <PlayerRanchReceiveAchievementRewardEvent, ui64, i64> {};

	// ranch player go explore event
	struct PlayerRanchGoExploreEvent : public ScriptEvent <PlayerRanchGoExploreEvent, ui64> {};

	// ranch player invite other player join game
	struct PlayerRanchSendInviteJoinGameEvent : public ScriptEvent <PlayerRanchSendInviteJoinGameEvent, ui64, ui64> {};

	// change current item info event
	struct ChangeCurrentItemInfoEvent : public ScriptEvent <ChangeCurrentItemInfoEvent, ui64, int> {};

	// change current item info event
	struct BlockCropsInfoEvent : public ScriptEvent <BlockCropsInfoEvent, ui64, int, int, int, int> {};

	// get ranch task items cost
	struct GetRanchTaskItemsCostEvent : public ScriptEvent <GetRanchTaskItemsCostEvent, ui64, i32, i32, BoolProxy*, IntProxy*> {};

	// buy ranch task items result
	struct PlayerBuyRanchTaskItemsResultEvent : public ScriptEvent <PlayerBuyRanchTaskItemsResultEvent, ui64, i32, i32, String> {};

	// building production speed up
	struct PlayerRanchBuildingProductionSpeedUpEvent : public ScriptEvent <PlayerRanchBuildingProductionSpeedUpEvent, ui64, i32, i32, i32> {};

	// shot actor npc
	struct PlayerShotActorNpcEvent : public  ScriptEvent<PlayerShotActorNpcEvent, ui64, ui32> {};

	// select lottery
	struct PlayerSelectLotteryEvent : public  ScriptEvent<PlayerSelectLotteryEvent, ui64, String> {};

	// lottery again
	struct PlayerLotteryAgainEvent : public  ScriptEvent<PlayerLotteryAgainEvent, ui64> {};

	// hall lottery again
	struct PlayerHallLotteryAgainEvent : public  ScriptEvent<PlayerHallLotteryAgainEvent, ui64, int, int> {};

	// review actor npc
	struct PlayerReviewActorNpcEvent : public  ScriptEvent<PlayerReviewActorNpcEvent, ui64, int> {};

	struct BuyFlyingPermissionResultEvent : public ScriptEvent <BuyFlyingPermissionResultEvent, ui64, bool> {};

	struct CheckFlyingPermissionEvent : public ScriptEvent <CheckFlyingPermissionEvent, ui64> {};

	struct PlayerPotionEffectFinishedEvent : public ScriptEvent <PlayerPotionEffectFinishedEvent, ui64, i32> {};

	struct PlayerKeepItemEvent : public ScriptEvent<PlayerKeepItemEvent, ui64, bool, int > {};

	struct ItemSkillConsumptionItemEvent : public ScriptEvent<ItemSkillConsumptionItemEvent, ui64, int> {};

	struct ItemSkillSpHandleEvent : public ScriptEvent<ItemSkillSpHandleEvent, int, int, Vector3> {};

	struct OpenEnchantmentEvent : public ScriptEvent<OpenEnchantmentEvent, ui64> {};

	struct EnchantmentEffectClickEvent : public ScriptEvent<EnchantmentEffectClickEvent, ui64, int, int, int> {};

	struct PlayerEnchantmentQuickEvent : public ScriptEvent<PlayerEnchantmentQuickEvent, ui64, int, int> {};

	struct BirdSimulatorBagOperationEvent : public ScriptEvent<BirdSimulatorBagOperationEvent, ui64, i64, int> {};

	struct BirdSimulatorStoreOperationEvent : public ScriptEvent<BirdSimulatorStoreOperationEvent, ui64, int, int, int> {};

	struct BirdSimulatorPersonalStoreBuyGoodsEvent : public ScriptEvent<BirdSimulatorPersonalStoreBuyGoodsEvent, ui64, int, int> {};

	struct BirdSimulatorFuseEvent : public ScriptEvent<BirdSimulatorFuseEvent, ui64, std::vector<i64>> {};

	struct BirdSimulatorOpenTreasureChestEvent : public ScriptEvent<BirdSimulatorOpenTreasureChestEvent, ui64, i32> {};

	struct BirdSimulatorLotteryEvent : public ScriptEvent<BirdSimulatorLotteryEvent, ui64, i32> {};

	struct BirdSimulatorTaskEvent : public ScriptEvent<BirdSimulatorTaskEvent, ui64, i32, i32> {};

	struct BirdSimulatorFeedEvent : public ScriptEvent<BirdSimulatorFeedEvent, ui64, i64, i32, i32> {};

	struct BirdSimulatorSetDressEvent : public ScriptEvent<BirdSimulatorSetDressEvent, ui64, i64, i32, i32> {};

	struct BirdSimulator_MonsterDieEvent: public ScriptEvent<BirdSimulator_MonsterDieEvent, i32> {};

	struct BirdSimulator_MonsterFindPlayerEvent: public ScriptEvent<BirdSimulator_MonsterFindPlayerEvent, i32, ui64> {};

	struct SkyBlockPartyAlmostOverEvent : public ScriptEvent <SkyBlockPartyAlmostOverEvent, ui64> {};
	struct SkyBlockPartyOverEvent : public ScriptEvent <SkyBlockPartyOverEvent, ui64> {};
	struct SkyBlockFriendUpdateEvent : public ScriptEvent <SkyBlockFriendUpdateEvent, ui64, String> {};
	struct LiftingChampionInfoEvent : public ScriptEvent <LiftingChampionInfoEvent, String> {};

	struct CheckStateChangeCondition1Event : public ScriptEvent<CheckStateChangeCondition1Event, int, int> {};
	struct CheckStateChangeCondition2Event : public ScriptEvent<CheckStateChangeCondition2Event, int, int> {};
	struct CheckStateChangeCondition3Event : public ScriptEvent<CheckStateChangeCondition3Event, int, int> {};
	struct CheckStateChangeCondition4Event : public ScriptEvent<CheckStateChangeCondition4Event, int, int> {};
	struct FindTargetEvent : public ScriptEvent<FindTargetEvent, int> {};
	struct PatrolStateEvent : public ScriptEvent<PatrolStateEvent, int> {};
	struct ChaseStateEvent : public ScriptEvent<ChaseStateEvent, int> {};

	struct ServerSetBlockEvent : public ScriptEvent<ServerSetBlockEvent, Vector3i, i32, i32, i32> {};

	struct ItemSkillNeedCustomSpeedEvent : public ScriptEvent<ItemSkillNeedCustomSpeedEvent, ui64, int> {};

	struct PlayerUseCannonEvent : public ScriptEvent<PlayerUseCannonEvent, ui64, ui32> {};

	struct BirdSimulatorOpenEggChestEvent : public ScriptEvent<BirdSimulatorOpenEggChestEvent, ui64, i32> {};

	struct SendGuideInfoEvent : public ScriptEvent<SendGuideInfoEvent, ui64, i32, i32> {};

	struct WatchAdvertFinishedEvent : public ScriptEvent<WatchAdvertFinishedEvent, ui64, i32, String, i32> {};

	struct KnockBackExtraStrengthEvent :public ScriptEvent<KnockBackExtraStrengthEvent, int, int, FloatProxy* > {};

	struct PlayerAttackDecorationEvent : public ScriptEvent<PlayerAttackDecorationEvent, ui64, ui32> {};

	struct PlayerChangeEditModeEvent : public ScriptEvent<PlayerChangeEditModeEvent, ui64, bool> {};

	struct PlayerCheckinEvent : public ScriptEvent<PlayerCheckinEvent, ui64, i32> {};

	struct BlockCityStorageOperationEvent : public ScriptEvent<BlockCityStorageOperationEvent, ui64, bool, i32, i32, i32, bool> {};

	struct PlayerOperationEvent : public ScriptEvent<PlayerOperationEvent, ui64, ui64, i32 , String, bool, i32> {};

	struct PlayerTakeOnOrOffVehicleEvent : public ScriptEvent<PlayerTakeOnOrOffVehicleEvent, ui64, i32, bool> {};

	struct PlayerTakeOnDecorationEvent : public ScriptEvent<PlayerTakeOnDecorationEvent, ui64, i32> {};

	struct BlockCropsNotDropEvent : public ScriptEvent<BlockCropsNotDropEvent, ui64, i32, i32, Vector3i, ui64> {};

	struct RechargeResultEvent : public ScriptEvent<RechargeResultEvent, ui64, i32, i32> {};

	struct ClientCommonDataEvent : public ScriptEvent<ClientCommonDataEvent, ui64, String, String> {};

	struct FormulationSubmitEvent : public ScriptEvent<FormulationSubmitEvent, ui64, i32, i32> {};

	struct PlayerEquipArmorEvent : public ScriptEvent<PlayerEquipArmorEvent,ui64, i32, i32 > {};

	struct PlayerArmorChangeEvent : public ScriptEvent<PlayerArmorChangeEvent, ui64> {};

	struct PlayerFoodLevelChangeEvent :public ScriptEvent<PlayerFoodLevelChangeEvent, ui64, i32,  i32> {};

	struct OpenMovingEnderChestEvent :public ScriptEvent<OpenMovingEnderChestEvent, ui64> {};

	struct PlayerUseInitSpawnPositionEvent : public ScriptEvent<PlayerUseInitSpawnPositionEvent, ui64, Vector3iProxy*> {};

	struct PlayerHealthChangedEvent : public ScriptEvent<PlayerHealthChangedEvent, EntityPlayerMP*, float, float> {};

	struct PlayerSetCameraLockedEvent : public ScriptEvent<PlayerSetCameraLockedEvent, ui64, bool> {};

	struct BlockCityManorPraiseEvent : public ScriptEvent<BlockCityManorPraiseEvent, ui64, ui64> {};

	struct ElevatorArriveEvent : public ScriptEvent<ElevatorArriveEvent, ui64, i32, i32> {};

	struct SyncEntityPlayerInfoEvent : public ScriptEvent<SyncEntityPlayerInfoEvent, ui64, int, String, String> {};

	struct LuaDoPayRequestEvent : public ScriptEvent<LuaDoPayRequestEvent, ui64, i32> {};

	struct CropsNotChangeNeighborEvent : public ScriptEvent<CropsNotChangeNeighborEvent, ui64, i32, Vector3i> {};

	struct PacketBindEntityEvent : public ScriptEvent<PacketBindEntityEvent, Entity*, Entity*> {};

	struct NeedLogicSpeedUpCheatEvent : public ScriptEvent<NeedLogicSpeedUpCheatEvent, bool> {};

	struct PacketUserAttrEvent : public ScriptEvent<PacketUserAttrEvent, EntityPlayer*, ui64> {};

	struct PacketUnbindEntityEvent : public ScriptEvent<PacketUnbindEntityEvent, int> {};

	struct OtherSessionNpcInfoEvent : public  ScriptEvent<OtherSessionNpcInfoEvent, ui64, IntProxy*> {};

	struct PlayerAIArriveEvent :public ScriptEvent<PlayerAIArriveEvent, ui64> {};

	struct PlayerAIMoveFailEvent :public ScriptEvent<PlayerAIMoveFailEvent, ui64> {};

	struct SavePlayerDBDataEvent :public ScriptEvent<SavePlayerDBDataEvent, ui64> {};

	struct PlayerReconnectSuccessEvent :public ScriptEvent<PlayerReconnectSuccessEvent, ClientPeer*> {};

	struct GameStatusChangeEvent :public ScriptEvent<GameStatusChangeEvent, i32> {};
}


