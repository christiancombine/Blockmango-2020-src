#pragma once

#include "Script/Event/ScriptEvent.h"
#include "cEntity/EntityOtherPlayerMP.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Entity/EntityActorNpc.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Script/VarProxy/BaseValueProxy.h"

using namespace LORD;
using namespace BLOCKMAN;

namespace SCRIPT_EVENT
{
	struct GameInitEvent : public ScriptEvent<GameInitEvent, String> {};

	struct GameLoadMapSuccessEvent : public ScriptEvent<GameLoadMapSuccessEvent, String, String> {};

	struct GameReadyEvent : public ScriptEvent<GameReadyEvent, bool> {};

	struct ClickSceneEvent : public ScriptEvent<ClickSceneEvent, bool> {};

	struct LongTouchSceneEvent : public ScriptEvent<LongTouchSceneEvent, bool> {};

	struct PlayerRebirthEvent : public ScriptEvent<PlayerRebirthEvent, EntityPlayerSPMP*> {};

	struct ClickScenePlayActionEvent : public ScriptEvent<ClickScenePlayActionEvent, bool> {};

	struct OtherPlayerSpawnEvent : public ScriptEvent<OtherPlayerSpawnEvent, EntityOtherPlayerMP*> {};

	struct GameDestroyEvent : public ScriptEvent<GameDestroyEvent, bool> {};

	struct ServerCommonDataEvent : public ScriptEvent<ServerCommonDataEvent, String, String> {};

	struct GameActionTriggerEvent : public ScriptEvent<GameActionTriggerEvent, int, String> {};

	struct GameMainTickEvent : public ScriptEvent<GameMainTickEvent, ui32> {};

	struct HideAllLayoutEvent : public ScriptEvent<HideAllLayoutEvent, bool> {};

	struct RecoverAllLayoutEvent : public ScriptEvent<RecoverAllLayoutEvent, bool> {};

	struct SkyBlockOpenRankEvent : public ScriptEvent<SkyBlockOpenRankEvent, bool> {};

	struct ShowPluginLayoutEvent : public ScriptEvent<ShowPluginLayoutEvent, String, String> {};

	struct ShowEngineLayoutEvent : public ScriptEvent<ShowEngineLayoutEvent, String, String> {};

	struct HideEngineLayoutEvent : public ScriptEvent<HideEngineLayoutEvent, String, String> {};

	struct HidePluginLayoutEvent : public ScriptEvent<HidePluginLayoutEvent, String> {};

	struct PlayUpperActionEvent : public ScriptEvent<PlayUpperActionEvent, String, int> {};

	struct UpdateUserListEvent : public ScriptEvent<UpdateUserListEvent, int> {};

	struct SkyBlockShowPriDetailEvent : public ScriptEvent<SkyBlockShowPriDetailEvent, String, String, String> {};

	struct SellItemEvent : public ScriptEvent<SellItemEvent, int, int> {};

	struct DropItemEvent : public ScriptEvent<DropItemEvent, int, int, int> {};

	struct ShowChatMessageReceiveEvent : public ScriptEvent<ShowChatMessageReceiveEvent, bool> {};

	struct CatchEntityEvent : public ScriptEvent<CatchEntityEvent, EntityPlayerSPMP*, Entity*, int> {};

	struct SyncBindEntityEvent: public ScriptEvent<SyncBindEntityEvent, Entity*, Entity*>{};

	struct SyncUnbindEntityEvent : public ScriptEvent<SyncUnbindEntityEvent, Entity*> {};

	struct ClientCmdEvent : public ScriptEvent<ClientCmdEvent, String> {};

	struct ClickSceneToControllerEvent : public ScriptEvent<ClickSceneToControllerEvent, bool> {};

	struct cSyncEntityPlayerInfoEvent : public ScriptEvent<cSyncEntityPlayerInfoEvent, CommonDataBuilder*, EntityPlayer*> {};

	struct RenderPlayerShowNameEvent : public ScriptEvent<RenderPlayerShowNameEvent, EntityPlayer*, ActorObject*, Vector3> {};

	struct RenderPlayerShowChatBubbleEvent : public ScriptEvent<RenderPlayerShowChatBubbleEvent, EntityPlayer*, ActorObject*, Vector3> {};

	struct PlayerLoginEvent : public ScriptEvent<PlayerLoginEvent, ui64, int> {};

	struct PlayerLogoutEvent : public ScriptEvent<PlayerLogoutEvent, ui64> {};

	struct LuaPlayerDeathEvent : public ScriptEvent<LuaPlayerDeathEvent, ui64> {};

	struct ClickToBlockEvent : public ScriptEvent<ClickToBlockEvent, Vector3i, int, bool> {};

	struct AttackEntityEvent : public ScriptEvent<AttackEntityEvent, int> {};

	struct AttackActorNpcEvent : public ScriptEvent<AttackActorNpcEvent, int> {};

	struct HurtCemaraEffectEvent : public ScriptEvent<HurtCemaraEffectEvent, EntityPlayer*> {};

	struct GamePauseEvent : public ScriptEvent<GamePauseEvent, bool> {};

	struct GameResumeEvent : public ScriptEvent<GameResumeEvent, bool> {};

	struct SkyBlockShowLackMoney : public ScriptEvent<SkyBlockShowLackMoney, bool> {};

	struct AddDiamondEvent: public ScriptEvent<AddDiamondEvent, bool>{};

	struct LuaLoadWorldEndEvent : public ScriptEvent<LuaLoadWorldEndEvent, bool> {};

	struct DBDataReadyEvent : public ScriptEvent<DBDataReadyEvent, bool> {};

	struct PlayUpperActionConflictEvent : public ScriptEvent<PlayUpperActionConflictEvent, String, int, int, int> {};

	struct PlayUpperActionSpecialEvent : public ScriptEvent<PlayUpperActionSpecialEvent, String> {};

	struct AddChatMessageEvent : public ScriptEvent<AddChatMessageEvent, String> {};

	struct ShowSettingMenuEvent : public ScriptEvent<ShowSettingMenuEvent, bool> {};

	struct PlayerPlayBaseActionEvent : public ScriptEvent<PlayerPlayBaseActionEvent, EntityPlayer*, int, StringProxy*> {};

	struct PlayerPlayUpperActionEvent : public ScriptEvent<PlayerPlayUpperActionEvent, EntityPlayer*, int, StringProxy*> {};

	struct CallPluginFunctionEvent : public ScriptEvent<CallPluginFunctionEvent, String, String> {};

	struct GameOverEvent : public ScriptEvent<GameOverEvent, String> {};

	struct ClickSelectableActionItemEvent : public ScriptEvent<ClickSelectableActionItemEvent, bool> {};

	struct ShowMainControlEvent : public ScriptEvent<ShowMainControlEvent, bool> {};

	struct ClickMainControlEvent : public ScriptEvent<ClickMainControlEvent, bool> {};

}