#pragma once
#include "Event.h"

namespace BLOCKMAN
{
	class ItemStack;
	class EntityPlayer;
	class TileEntitySign;
	class IInventory;
	class Entity;

	inline namespace PLAYER
	{
		struct AimingStateChangeEvent : Event<AimingStateChangeEvent, bool, int> {};
	}

	inline namespace LOGIC
	{
		struct SignTextsChangeEvent : Event<SignTextsChangeEvent, TileEntitySign*> {};
		struct FurnaceChangeEvent : Event<FurnaceChangeEvent, BlockPos, IInventory*> {};
		struct FurnaceTimeSynEvent : Event<FurnaceTimeSynEvent, BlockPos, i32, i32> {};
		struct OnUseItemSeedsEvent : Event<OnUseItemSeedsEvent, EntityPlayer*, BlockPos, int, ui64> {};
		struct OnNewCropsBlockDestroyEvent : Event<OnNewCropsBlockDestroyEvent, BlockPos> {};
		struct ShowBirdSimulatorMiniTipEvent : Event<ShowBirdSimulatorMiniTipEvent, i32, const String&> {};

		struct G1049UseGatherSkillEvent :Event<G1049UseGatherSkillEvent, const BlockPos&, int > {};
		struct G1049UseStopGatherEvent :Event<G1049UseGatherSkillEvent> {};

		struct ClientOnGroundCheatEvent : Event<ClientOnGroundCheatEvent> {};
	}
}
