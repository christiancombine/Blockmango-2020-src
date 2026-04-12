#pragma once

#ifndef _SCRIPT_EVENT_REGISTER_H__
#define _SCRIPT_EVENT_REGISTER_H__

#include "Plugins/GameClientEvents.h"
#include "Script/Event/LogicScriptEvents.h"
#include "LuaRegister/Template/LuaRegister.h"
//#include "Script/Event/CoreScriptEvents.h"

using namespace SCRIPT_EVENT;

#define  _REGISTER_SCRIPT_EVENT(cls)                                                 \
         _BEGIN_REGISTER_CLASS(cls)                                                  \
         _CLASSREGISTER_AddStaticMember(registerCallBack, cls::registerCallBack)     \
		 _CLASSREGISTER_AddStaticMember(unregisterAll, cls::unregisterAll)			 \
         _END_REGISTER_CLASS() 

_REGISTER_SCRIPT_EVENT(GameInitEvent)
_REGISTER_SCRIPT_EVENT(GameLoadMapSuccessEvent)
_REGISTER_SCRIPT_EVENT(GameReadyEvent)
_REGISTER_SCRIPT_EVENT(ClickSceneEvent)
_REGISTER_SCRIPT_EVENT(LongTouchSceneEvent)
_REGISTER_SCRIPT_EVENT(PlayerRebirthEvent)
_REGISTER_SCRIPT_EVENT(ClickScenePlayActionEvent)
_REGISTER_SCRIPT_EVENT(OtherPlayerSpawnEvent)
_REGISTER_SCRIPT_EVENT(GetArmSwingAnimationEndEvent)
_REGISTER_SCRIPT_EVENT(BindChildEntityEvent)
_REGISTER_SCRIPT_EVENT(UnbindChildEntityEvent)
_REGISTER_SCRIPT_EVENT(GameDestroyEvent)
_REGISTER_SCRIPT_EVENT(ServerCommonDataEvent)
_REGISTER_SCRIPT_EVENT(GameActionTriggerEvent)
_REGISTER_SCRIPT_EVENT(GameMainTickEvent)
_REGISTER_SCRIPT_EVENT(HideAllLayoutEvent)
_REGISTER_SCRIPT_EVENT(RecoverAllLayoutEvent)
_REGISTER_SCRIPT_EVENT(SkyBlockOpenRankEvent)
_REGISTER_SCRIPT_EVENT(ShowPluginLayoutEvent)
_REGISTER_SCRIPT_EVENT(ShowEngineLayoutEvent)
_REGISTER_SCRIPT_EVENT(HideEngineLayoutEvent)
_REGISTER_SCRIPT_EVENT(PlayUpperActionEvent)
_REGISTER_SCRIPT_EVENT(SpawnEntityEvent)
_REGISTER_SCRIPT_EVENT(RemoveEntityEvent)
_REGISTER_SCRIPT_EVENT(UpdateUserListEvent)
_REGISTER_SCRIPT_EVENT(SkyBlockShowPriDetailEvent)
_REGISTER_SCRIPT_EVENT(SellItemEvent)
_REGISTER_SCRIPT_EVENT(DropItemEvent)
_REGISTER_SCRIPT_EVENT(ShowChatMessageReceiveEvent)
_REGISTER_SCRIPT_EVENT(CatchEntityEvent)
_REGISTER_SCRIPT_EVENT(SyncBindEntityEvent)
_REGISTER_SCRIPT_EVENT(SyncUnbindEntityEvent)
_REGISTER_SCRIPT_EVENT(ClientCmdEvent)
_REGISTER_SCRIPT_EVENT(ClickSceneToControllerEvent)
_REGISTER_SCRIPT_EVENT(cSyncEntityPlayerInfoEvent)
_REGISTER_SCRIPT_EVENT(RenderPlayerShowNameEvent)
_REGISTER_SCRIPT_EVENT(RenderPlayerShowChatBubbleEvent)
_REGISTER_SCRIPT_EVENT(PlayerLoginEvent)
_REGISTER_SCRIPT_EVENT(PlayerLogoutEvent)
_REGISTER_SCRIPT_EVENT(LuaPlayerDeathEvent)
_REGISTER_SCRIPT_EVENT(ClickToBlockEvent)
_REGISTER_SCRIPT_EVENT(AttackActorNpcEvent)
_REGISTER_SCRIPT_EVENT(CreatureAttackPlayerEvent)
_REGISTER_SCRIPT_EVENT(CreatureAttackCreatureEvent)
_REGISTER_SCRIPT_EVENT(HurtCemaraEffectEvent)
_REGISTER_SCRIPT_EVENT(GamePauseEvent)
_REGISTER_SCRIPT_EVENT(GameResumeEvent)
_REGISTER_SCRIPT_EVENT(SkyBlockShowLackMoney)
_REGISTER_SCRIPT_EVENT(AddDiamondEvent)
_REGISTER_SCRIPT_EVENT(LuaLoadWorldEndEvent)
_REGISTER_SCRIPT_EVENT(DBDataReadyEvent)
_REGISTER_SCRIPT_EVENT(EntitySkillConflictEvent)
_REGISTER_SCRIPT_EVENT(PlayUpperActionConflictEvent)
_REGISTER_SCRIPT_EVENT(PlayUpperActionSpecialEvent)
_REGISTER_SCRIPT_EVENT(GameAnalyticsInitEvent)
//_REGISTER_SCRIPT_EVENT(CastLuaStringEvent)
//_REGISTER_SCRIPT_EVENT(ParseUI64Event)
_REGISTER_SCRIPT_EVENT(LuaHotUpdateEvent)
_REGISTER_SCRIPT_EVENT(GameAnalyticsDesignEvent)
_REGISTER_SCRIPT_EVENT(BlockStartBreakEvent)
_REGISTER_SCRIPT_EVENT(BlockAbortBreakEvent)
_REGISTER_SCRIPT_EVENT(BlockBreakEvent)
_REGISTER_SCRIPT_EVENT(AddChatMessageEvent)
_REGISTER_SCRIPT_EVENT(PlayerPlaceBlockEvent)
_REGISTER_SCRIPT_EVENT(ShowSettingMenuEvent)
_REGISTER_SCRIPT_EVENT(PlayerPlayBaseActionEvent)
_REGISTER_SCRIPT_EVENT(PlayerPlayUpperActionEvent)
_REGISTER_SCRIPT_EVENT(AttackEntityEvent)
_REGISTER_SCRIPT_EVENT(CallPluginFunctionEvent)
_REGISTER_SCRIPT_EVENT(GameOverEvent)
_REGISTER_SCRIPT_EVENT(ClickSelectableActionItemEvent)
_REGISTER_SCRIPT_EVENT(BlockCanHarvestEvent)
_REGISTER_SCRIPT_EVENT(ShowMainControlEvent)
_REGISTER_SCRIPT_EVENT(ClickMainControlEvent)

#endif