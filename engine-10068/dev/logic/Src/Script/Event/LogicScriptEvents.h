#pragma once
#include "Std/type_traits.h"
#include "Script/Event/ScriptEvent.h"
#include "Script/VarProxy/LogicVarProxy.h"
#include "Script/VarProxy/BaseValueProxy.h"
#include "TileEntity/TileEntityInventory.h"
#include "Item/ItemStack.h"
#include "Entity/EntityPlayer.h"

namespace SCRIPT_EVENT
{
	//Entity hit event
	// EntityId
	// hitPos
	// PlayerId
	struct EntityHitEvent : public ScriptEvent<EntityHitEvent, int, Vector3, int> {};

	//tnt explode
	struct BlockTNTExplodeEvent : public ScriptEvent<BlockTNTExplodeEvent, int, Vector3, ExplosionAttr*, int> {};

	//tnt explode
	struct BlockTNTExplodeBreakBlockEvent : public ScriptEvent<BlockTNTExplodeBreakBlockEvent, Vector3i, int, int> {};

	//tnt explode entity
	struct BlockTNTExplodeHurtEntityEvent : public ScriptEvent<BlockTNTExplodeHurtEntityEvent, int, int, int> {};

	//open the door
	struct PlayerOpenDoorEvent : public ScriptEvent<PlayerOpenDoorEvent, int, Vector3i> {};

	// entity item spawn
	struct EntityItemSpawnEvent : public ScriptEvent<EntityItemSpawnEvent, ui32, ui32, String, bool> {};

	// if clear player drop item(itemID, itemDamage, clearType, itemCount, raknetID)
	enum ClearDropItemType
	{
		ClearDropItemType_Main = 1,
		ClearDropItemType_Armor = 2,
		ClearDropItemType_Hotbar = 3,
	};
	struct ClearDropItemEvent : public ScriptEvent<ClearDropItemEvent, ui32, ui32, int, int, int> {};

	struct ClearInventoryEvent : public ScriptEvent<ClearInventoryEvent, bool> {};

	// potion convert glass bottle
	struct PotionConvertGlassBottleEvent : public ScriptEvent<PotionConvertGlassBottleEvent, ui32, ui32> {};

	// aircraft move
	struct AircraftMoveEvent : public ScriptEvent<AircraftMoveEvent, int, Vector3>{};

	// creature attack player
	struct CreatureAttackPlayerEvent : public  ScriptEvent<CreatureAttackPlayerEvent, ui32, ui32, ui32, IntProxy*> {};

	// creature attack creature
	struct CreatureAttackCreatureEvent : public  ScriptEvent<CreatureAttackCreatureEvent, ui32, ui32, ui32, IntProxy*> {};
	
	// use itemseeds 
	struct TryToUseItemSeedsEvent : public ScriptEvent<TryToUseItemSeedsEvent, int, Vector3> {};

	// click blocksign event
	struct OnClickBlockSignEvent : public ScriptEvent<OnClickBlockSignEvent, int, Vector3i> {};

	//skill attack
	struct ItemSkillAttackEvent : public  ScriptEvent<ItemSkillAttackEvent, int, int, Vector3, int> {};

	// consume block
	struct PlacingConsumeBlockEvent: public  ScriptEvent<PlacingConsumeBlockEvent, ui32, ui32, int, Vector3i>{};

	// block switch event:entityId, press down or up, blockId, pos, return true means can handle default neighbor event
	struct BlockSwitchEvent :public ScriptEvent<BlockSwitchEvent, int, bool, int, Vector3i> {};

	// block switch plus return false means can long distance
	struct BlockSwitchPlusEvent :public ScriptEvent<BlockSwitchPlusEvent, Vector3> {};

	// block empty move
	struct BlockmanEmptyMoveEvent : public ScriptEvent<BlockmanEmptyMoveEvent, int, Vector3>{};

	// block start break blockid, x, y, z
	struct BlockStartBreakEvent :public ScriptEvent<BlockStartBreakEvent, ui64, int, Vector3i> {};

	// block abort break blockid, x, y, z
	struct BlockAbortBreakEvent :public ScriptEvent<BlockAbortBreakEvent, ui64, int, Vector3i> {};

	// block break blockid, x, y, z
	struct BlockBreakEvent :public ScriptEvent<BlockBreakEvent, ui64, int, Vector3i> {};

	// player place block event
	struct PlayerPlaceBlockEvent : public ScriptEvent<PlayerPlaceBlockEvent, ui64, int, int, Vector3i> {};
	
	// block pressure weight event: entityId power, blockId, pos, 
	struct BlockPressurePlateWeightedEvent :public ScriptEvent<BlockPressurePlateWeightedEvent, int, int, int, Vector3i> {}; 

	// place item building
	struct PlaceItemBuildingEvent : public ScriptEvent<PlaceItemBuildingEvent, ui64, int, int, Vector3, int, Vector3, Vector3> {};

	// place item house
	struct PlaceItemHouseEvent : public ScriptEvent<PlaceItemHouseEvent, ui64, String, Vector3, Vector3> {};
	
	// remove item building event
	struct RemoveItemBuildingEvent : public ScriptEvent<RemoveItemBuildingEvent, ui64, i32> {};

	// use item teleport scoll
	struct UseItemTeleportEvent : public ScriptEvent<UseItemTeleportEvent, ui32, int> {};

	// player's ranch order ask for help 
	struct PlayerAskForHelpRanchOrderEvent : public ScriptEvent<PlayerAskForHelpRanchOrderEvent, ui64, i32, i32> {};

	// player put into ranch order cargo event
	struct PlayerPutIntoRanchOrderCargoEvent : public ScriptEvent<PlayerPutIntoRanchOrderCargoEvent, ui64, i32, i32> {};

	// player receive ranch order cargo event
	struct PlayerReceiveRanchOrderCargoEvent : public ScriptEvent<PlayerReceiveRanchOrderCargoEvent, ui64, i32, i32> {};

	// player refresh ranch order event
	struct PlayerRefreshRanchOrderEvent : public ScriptEvent<PlayerRefreshRanchOrderEvent, ui64, i32, i32> {};

	// blockore dropped
	struct BlockOreDroppedEvent : public ScriptEvent<BlockOreDroppedEvent, int> {};

	// itemPickaxe Harvest
	struct ItemPickaxeCanHarvestEvent : public ScriptEvent<ItemPickaxeCanHarvestEvent, int, int> {};

	//ChestOpen Special Handle
	struct ChestOpenSpHandleEvent :public ScriptEvent<ChestOpenSpHandleEvent, Vector3i, int> {};

	//FurnaceOpen Special Handle
	struct FurnaceOpenSpHandleEvent :public ScriptEvent<FurnaceOpenSpHandleEvent, Vector3i, int> {};

	//CraftOpen Special Handle
	struct CraftOpenSpHandleEvent :public ScriptEvent<CraftOpenSpHandleEvent, Vector3i, int> {};

	// block leaves break
	struct BlockLeavesBreakEvent : public ScriptEvent<BlockLeavesBreakEvent, int> {};

	// block stationary set not stationary
	struct BlockStationaryNotStationaryEvent : public ScriptEvent<BlockStationaryNotStationaryEvent, int> {};

	//Entity hit with current item event
	struct HitEntityWithCurrentItem : public ScriptEvent<HitEntityWithCurrentItem, int, int, int, int> {};

	// entity death drop item
	struct EntityDeathDropItemEvent : public ScriptEvent<EntityDeathDropItemEvent, int, String> {};

	// entity collide with player range enlarge
	struct EntityCollideRangeEnlargeEvent : public ScriptEvent<EntityCollideRangeEnlargeEvent, int> {};

	// use item bucket
	struct UseItemBucketEvent : public ScriptEvent<UseItemBucketEvent, ui32, int, Vector3i> {};

	// use item bucket in block
	struct UseItemBucketInBlockEvent : public ScriptEvent<UseItemBucketInBlockEvent, ui32, int, Vector3i> {};

	// use item bucket in block
	struct UseItemBucketInBlockChangeEvent : public ScriptEvent<UseItemBucketInBlockChangeEvent, ui32, int, Vector3i, int, int> {};

	// food eaten
	struct FoodEatenEvent : public ScriptEvent<FoodEatenEvent, ui32, int> {};

	// get gather num
	struct BirdSimulatorGatherEvent : public ScriptEvent<BirdSimulatorGatherEvent, ui64, IntProxy*, Vector3i> {};

	// add gather score
	struct BirdSimulatorAddGatherScoreEvent : public ScriptEvent<BirdSimulatorAddGatherScoreEvent, ui64, i64, int, Vector3i, int, int, IntProxy*> {};

	struct BirdSimulatorNestOperationEvent : public ScriptEvent<BirdSimulatorNestOperationEvent, ui64, i32,  Vector3i> {};

	// potion custom event
	struct PotionCustomEvent : public ScriptEvent<PotionCustomEvent, int, int> {};

	// tool durable
	struct ToolDurableEvent : public  ScriptEvent<ToolDurableEvent, i32, i32> {};

	struct PlayerOpenGlideEvent : public ScriptEvent<PlayerOpenGlideEvent, ui64, bool> {};

	struct EntityLivingBaseFallNoNeedCalBlockIdEvent : public ScriptEvent<EntityLivingBaseFallNoNeedCalBlockIdEvent, int> {};

	struct PlaceItemTemplateEvent : public ScriptEvent<PlaceItemTemplateEvent, ui64, String, Vector3, Vector3> {};

	struct PlaceItemDecorationEvent : public ScriptEvent<PlaceItemDecorationEvent, ui64, int, int, Vector3, int, Vector3, Vector3> {};

	struct PlaceDoorBlockEvent : public ScriptEvent<PlaceDoorBlockEvent, ui64, int, int, Vector3i> {};

	struct BlockCanHarvestEvent : public ScriptEvent<BlockCanHarvestEvent, i32> {};

	struct EntityPositionChangeEvent : public ScriptEvent<EntityPositionChangeEvent, i32, Vector3> {};

	struct PlaceSlabBlockEvent : public ScriptEvent<PlaceSlabBlockEvent, ui64, int, int, Vector3i, int> {};

	struct BlockNeighborChangeEvent : public ScriptEvent<BlockNeighborChangeEvent, Vector3i, int, int, int> {};

	struct DamageOnFireByLavaTimeEvent : public ScriptEvent<DamageOnFireByLavaTimeEvent, int, IntProxy*> {};

	struct BlockFLowingChangeEvent : public ScriptEvent<BlockFLowingChangeEvent, int, int, Vector3i, int> {};

	struct BlockPlacedByEvent : public ScriptEvent<BlockPlacedByEvent, Vector3i, int, int> {};

	struct CraftItemSucEvent : public ScriptEvent<CraftItemSucEvent, int, int, int, int> {};

	struct FurnaceBurnTimeEvent : public ScriptEvent<FurnaceBurnTimeEvent, IntProxy*> {};

	struct FurnaceIsUsingEvent : public ScriptEvent<FurnaceIsUsingEvent, int> {};

	struct BlockFLowingHighControlEvent : public ScriptEvent<BlockFLowingHighControlEvent, int, Vector3i, Vector3i> {};

	struct EntityItemDeadInLavaEvent : public ScriptEvent<EntityItemDeadInLavaEvent, int> {};

	struct PlaceFarmlandEvent : public ScriptEvent<PlaceFarmlandEvent, ui64, int, int, Vector3i> {};

	struct UseHoeToolEvent : public ScriptEvent<UseHoeToolEvent, ui64, int, Vector3i> {};

	struct BlockRedStoneLightPoweredEvent : public ScriptEvent<BlockRedStoneLightPoweredEvent, int, Vector3i> {};

	struct UseCustomItemEvent : public ScriptEvent<UseCustomItemEvent, ui64, i32, Vector3i> {};

	struct FurnaceBurningChangeEvent : public ScriptEvent<FurnaceBurningChangeEvent, Vector3i, int, int> {};

	struct SkillThrowableMoveEvent : public ScriptEvent<SkillThrowableMoveEvent, int, int, Vector3> {};

	struct PlayerCatchFishHookEvent : public ScriptEvent<PlayerCatchFishHookEvent, int, int> {};

	struct UseSignPostItemEvent : public ScriptEvent<UseSignPostItemEvent, ui64, i32> {};

	struct PlaceSignPostEvent : public ScriptEvent<PlaceSignPostEvent, ui64, int, int, Vector3i> {};

	struct CustomCropsOwnerEvent : public ScriptEvent<CustomCropsOwnerEvent, int, LongProxy*> {};

	struct GetArmSwingAnimationEndEvent : public ScriptEvent<GetArmSwingAnimationEndEvent, int, int*> {};

	struct BindChildEntityEvent : public ScriptEvent<BindChildEntityEvent, int, int> {};

	struct UnbindChildEntityEvent : public ScriptEvent<UnbindChildEntityEvent, int, int> {};

	struct FlyingByPulledCollisionEvent : public ScriptEvent<FlyingByPulledCollisionEvent, int> {};

	struct WrongMoveEvent : public ScriptEvent<WrongMoveEvent, int, Vector3> {};

	struct ChangeMaxHealthSingleEvent : public ScriptEvent<ChangeMaxHealthSingleEvent, int, float> {};

	struct SpawnEntityEvent : public ScriptEvent<SpawnEntityEvent, BLOCKMAN::Entity*, int, String> {};

	struct RemoveEntityEvent : public ScriptEvent<RemoveEntityEvent, int> {};

	struct UseCustomItemNotNeedAdjustPosEvent : public ScriptEvent<UseCustomItemNotNeedAdjustPosEvent, ui64, i32> {};

	struct UseCustomItemNotNeedReduceEvent : public ScriptEvent<UseCustomItemNotNeedReduceEvent, ui64, i32> {};

	struct ItemFoodUseEvent : public ScriptEvent<ItemFoodUseEvent, BLOCKMAN::ItemStack*, BLOCKMAN::EntityPlayer*, BLOCKMAN::BlockPos> {};

	struct EntitySkillConflictEvent : public ScriptEvent<EntitySkillConflictEvent, int, int, int> {};

	struct LuaHotUpdateEvent : public ScriptEvent<LuaHotUpdateEvent, bool> {};

	struct GameAnalyticsInitEvent : public ScriptEvent<GameAnalyticsInitEvent, bool, int64_t, String, String> {};

	struct GameAnalyticsDesignEvent : public ScriptEvent<GameAnalyticsDesignEvent, int64_t, float, std::vector<String>> {};

	struct PlaceItemBlockEvent : public ScriptEvent<PlaceItemBlockEvent, ui64, int, int, Vector3i> {};

	struct SchemeticPlaceBlockBeginEvent : public ScriptEvent<SchemeticPlaceBlockBeginEvent, ui64, Vector3i, i32, i32> {};

	struct SchemeticPlaceBlockEvent : public ScriptEvent<SchemeticPlaceBlockEvent, ui64, Vector3i, i32, i32, i32> {};

	struct SchemeticPlaceBlockFinishEvent : public ScriptEvent<SchemeticPlaceBlockFinishEvent, ui64, i32, i32, i32> {};

	struct SchemeticPlaceCoverEvent : public ScriptEvent<SchemeticPlaceCoverEvent, ui64, Vector3i, i32, i32, i32> {};
}
