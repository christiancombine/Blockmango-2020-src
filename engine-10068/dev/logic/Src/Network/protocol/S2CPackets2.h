#pragma once
#ifndef __S2C_PACKETS2_H__
#define __S2C_PACKETS2_H__

#include "Network/protocol/PacketDefine.h"
#include "Network/NetworkDefine.h"
#include "FixedLengthInteger.h"
#include "Chat/ChatComponent.h"
#include "Game/Manor.h"
#include "Game/BirdSimulator.h"
#include "Entity/Entity.h"

using namespace LORD;

//S2CPacketDeathUnequipArmor
_BEGIN_PACKET_S2C(S2CPacketType::DeathUnequipArmor, DataPacket::emNormal, S2CPacketDeathUnequipArmor)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//S2CPacketSyncChangeEntityPerspece
_BEGIN_PACKET_S2C(S2CPacketType::SyncChangeEntityPerspece, DataPacket::emNormal, S2CPacketSyncChangeEntityPerspece)
_SERIALIZE_MEMBER(int, View)
_END_PACKET

//S2CPacketPlayerInvisible
_BEGIN_PACKET_S2C(S2CPacketType::PlayerInvisible, DataPacket::emNormal, S2CPacketPlayerInvisible)
_SERIALIZE_MEMBER(bool, isInvisible)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//S2CPacketChangeHeart
_BEGIN_PACKET_S2C(S2CPacketType::ChangeHeart, DataPacket::emNormal, S2CPacketChangeHeart)
_SERIALIZE_MEMBER(i32, hp)
_SERIALIZE_MEMBER(i32, maxHp)
_END_PACKET

//S2CPacketSyncSessionNpc
_BEGIN_PACKET_S2C(S2CPacketType::SyncSessionNpc, DataPacket::emNormal, S2CPacketSyncSessionNpc)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, sessionType)
_SERIALIZE_MEMBER(i32, timeLeft)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, actorBody)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, actorAction)
_SERIALIZE_MEMBER(String, actorBodyId)
_SERIALIZE_MEMBER(String, sessionContent)
_SERIALIZE_MEMBER(String, effectName)
_SERIALIZE_MEMBER(bool, isPerson)
_SERIALIZE_MEMBER(bool, isCanCollided)
_SERIALIZE_MEMBER(bool, isSuspended)
_END_PACKET

//S2CPacketUpdateManor
_BEGIN_PACKET_S2C(S2CPacketType::UpdateManor, DataPacket::emNormal, S2CPacketUpdateManor)
_SERIALIZE_MEMBER(BLOCKMAN::ManorInfo, m_manorInfo)
_SERIALIZE_MEMBER(BLOCKMAN::ManorInfo, m_nextManorInfo)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ManorHouse>::type, m_manorHouses)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ManorFurniture>::type, m_manorFurnitures)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ManorMessage>::type, m_manorMessages)
_END_PACKET

//S2CPacketUpdateRealTimeRankInfo
_BEGIN_PACKET_S2C(S2CPacketType::UpdateRealTimeRankInfo, DataPacket::emNormal, S2CPacketUpdateRealTimeRankInfo)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

//S2CPacketUpdateGameMonsterInfo
_BEGIN_PACKET_S2C(S2CPacketType::UpdateGameMonsterInfo, DataPacket::emNormal, S2CPacketUpdateGameMonsterInfo)
_SERIALIZE_MEMBER(i32, currCheckPointNum)
_SERIALIZE_MEMBER(i32, surplusMonsterNum)
_SERIALIZE_MEMBER(i32, surplusCheckPointNum)
_END_PACKET

//S2CPacketUpdateBossBloodStrip
_BEGIN_PACKET_S2C(S2CPacketType::UpdateBossBloodStrip, DataPacket::emNormal, S2CPacketUpdateBossBloodStrip)
_SERIALIZE_MEMBER(i32, currBloodStrip)
_SERIALIZE_MEMBER(i32, maxBloodStrip)
_END_PACKET

// S2CPacketSwitchablePropsData
_BEGIN_PACKET_S2C(S2CPacketType::SwitchablePropsData, DataPacket::emNormal, S2CPacketSwitchablePropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// S2CPacketUpgradePropsData
_BEGIN_PACKET_S2C(S2CPacketType::UpgradePropsData, DataPacket::emNormal, S2CPacketUpgradePropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// S2CPacketPersonalShopTip
_BEGIN_PACKET_S2C(S2CPacketType::PersonalShopTip, DataPacket::emNormal, S2CPacketPersonalShopTip)
_SERIALIZE_MEMBER(String, tip)
_END_PACKET

// S2CPacketCreateHouseFromSchematic
_BEGIN_PACKET_S2C(S2CPacketType::CreateHouseFromSchematic, DataPacket::emMapData, S2CPacketCreateHouseFromSchematic)
_SERIALIZE_MEMBER(String, fileName)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3i, start_pos)
_SERIALIZE_MEMBER(ui64, rakssid)
_SERIALIZE_MEMBER(int, blockId)
_SERIALIZE_MEMBER(int, offset)
_SERIALIZE_MEMBER(LORD::vector<PACKET_DETAIL::Vector3i>::type, placeCoverResultVector3i)
_SERIALIZE_MEMBER(LORD::vector<bool>::type, placeCoverResultBool)
_SERIALIZE_MEMBER(bool, xImage)
_SERIALIZE_MEMBER(bool, zImage)
_SERIALIZE_MEMBER(bool, createOrDestroy)
_SERIALIZE_MEMBER(bool, coverBlock)
_END_PACKET


// S2CPacketFillAreaByBlockIdAndMate
_BEGIN_PACKET_S2C(S2CPacketType::FillAreaByBlockIdAndMate, DataPacket::emMapData, S2CPacketFillAreaByBlockIdAndMate)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3i, start_pos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3i, end_pos)
_SERIALIZE_MEMBER(i32, block_id)
_SERIALIZE_MEMBER(i32, mate)
_END_PACKET


// S2CPacketShowPlayerOperation
_BEGIN_PACKET_S2C(S2CPacketType::ShowPlayerOperation, DataPacket::emNormal, S2CPacketShowPlayerOperation)
_SERIALIZE_MEMBER(ui64, m_targetUserId)
_SERIALIZE_MEMBER(String, m_targetUserName)
_END_PACKET


//S2CPacketSyncEntityCreature
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityCreature, DataPacket::emNormal, S2CPacketSyncEntityCreature)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(int, creatureEntityType)
_SERIALIZE_MEMBER(bool, canFly)
_SERIALIZE_MEMBER(bool, keepJumping)
_SERIALIZE_MEMBER(bool, onGround)
_END_PACKET

//S2CPacketSyncCreatureEntityAction
_BEGIN_PACKET_S2C(S2CPacketType::SyncCreatureEntityAction, DataPacket::emNormal, S2CPacketSyncCreatureEntityAction)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(int, actionStatus)
_SERIALIZE_MEMBER(int, baseAction)
_SERIALIZE_MEMBER(int, preBaseAction)
_END_PACKET

//S2CPacketPlayerSpeedLevel
_BEGIN_PACKET_S2C(S2CPacketType::EntitySpeedLevel, DataPacket::emNormal, S2CPacketEntitySpeedLevel)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, speedLevel)
_END_PACKET

//S2CPacketPlayerExpInfo
_BEGIN_PACKET_S2C(S2CPacketType::PlayerExpInfo, DataPacket::emNormal, S2CPacketPlayerExpInfo)
_SERIALIZE_MEMBER(i32, level)
_SERIALIZE_MEMBER(float, exp)
_SERIALIZE_MEMBER(i32, maxExp)
_END_PACKET

//S2CPacketShowUpgradeResourceUI
_BEGIN_PACKET_S2C(S2CPacketType::ShowUpgradeResourceUI, DataPacket::emNormal, S2CPacketShowUpgradeResourceUI)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, itemNum)
_SERIALIZE_MEMBER(i32, resourceId)
_SERIALIZE_MEMBER(String, tipString)
_END_PACKET

// S2CPacketManorBtnVisible
_BEGIN_PACKET_S2C(S2CPacketType::ManorBtnVisible, DataPacket::emNormal, S2CPacketManorBtnVisible)
_SERIALIZE_MEMBER(bool, visible)
_END_PACKET

// S2CPacketCallOnManorResetClient
_BEGIN_PACKET_S2C(S2CPacketType::CallOnManorResetClient, DataPacket::emNormal, S2CPacketCallOnManorResetClient)
_SERIALIZE_MEMBER(ui64, m_targetUserId)
_END_PACKET

// S2CPacketCallOnManorResetClient
_BEGIN_PACKET_S2C(S2CPacketType::UpdateManorOwner, DataPacket::emNormal, S2CPacketUpdateManorOwner)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ManorOwner>::type, m_owners)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerMovementInput, DataPacket::emNormal, S2CPacketSyncPlayerMovementInput)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(float, forward)
_SERIALIZE_MEMBER(float, strafe)
_END_PACKET

//S2CPacketUpdateBossBloodStrip
_BEGIN_PACKET_S2C(S2CPacketType::UpdateBasementLife, DataPacket::emNormal, S2CPacketUpdateBasementLife)
_SERIALIZE_MEMBER(float, curLife)
_SERIALIZE_MEMBER(float, maxLife)
_END_PACKET

//S2CPacketShowBuildWarBlockBtn
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarBlockBtn, DataPacket::emNormal, S2CPacketShowBuildWarBlockBtn)
_SERIALIZE_MEMBER(bool, isNeedOpen)
_END_PACKET

//S2CPacketShowBuildWarPlayNum
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarPlayNum, DataPacket::emNormal, S2CPacketShowBuildWarPlayNum)
_SERIALIZE_MEMBER(ui8, cur_num)
_SERIALIZE_MEMBER(ui8, max_num)
_END_PACKET

//S2CPacketShowBuildWarLeftTime
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarLeftNum, DataPacket::emNormal, S2CPacketShowBuildWarLeftTime)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(ui16, left_time)
_SERIALIZE_MEMBER(String, theme)
_END_PACKET

//S2CPacketOpenShopByEntityId
_BEGIN_PACKET_S2C(S2CPacketType::OpenShopByEntityId, DataPacket::emNormal, S2CPacketOpenShopByEntityId)
_SERIALIZE_MEMBER(int, entityId)
_END_PACKET

//S2CPacketShowGrade
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarGrade, DataPacket::emNormal, S2CPacketShowGrade)
_SERIALIZE_MEMBER(bool, openOrClose)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(String, msg)
_SERIALIZE_MEMBER(ui8, left_time)
_END_PACKET

//S2CPacketShowGuessResult
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarGuessResult, DataPacket::emNormal, S2CPacketShowGuessResult)
_SERIALIZE_MEMBER(bool, right)
_SERIALIZE_MEMBER(int, rank)
_END_PACKET

//S2CPacketShowGuessUi
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarGuessUi, DataPacket::emNormal, S2CPacketShowGuessUi)
_SERIALIZE_MEMBER(String, info)
_SERIALIZE_MEMBER(int, guess_room_id)
_END_PACKET

// S2CPacketGameSettlementExtra
_BEGIN_PACKET_S2C(S2CPacketType::GameSettlementExtra, DataPacket::emNormal, S2CPacketGameSettlementExtra)
_SERIALIZE_MEMBER(bool, guess_right)
_SERIALIZE_MEMBER(String, guess_name)
_SERIALIZE_MEMBER(int, guess_reward)
_END_PACKET

//S2CPacketUnlockedCommodity
_BEGIN_PACKET_S2C(S2CPacketType::UnlockedCommodity, DataPacket::emNormal, S2CPacketUnlockedCommodity)
_SERIALIZE_MEMBER(i32, merchantId)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, meta)
_END_PACKET

//S2CPacketInitUnlockedCommodity
_BEGIN_PACKET_S2C(S2CPacketType::InitUnlockedCommodity, DataPacket::emNormal, S2CPacketInitUnlockedCommodity)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, meta)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerRotation, DataPacket::emNormal, S2CPacketSyncPlayerRotation)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(FixedLengthInteger<uint8_t>, rotationYaw)
_SERIALIZE_MEMBER(FixedLengthInteger<uint8_t>, rotationPitch)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerJumpChanged, DataPacket::emNormal, S2CPacketSyncPlayerJumpChanged)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(bool, isJump)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerDownChanged, DataPacket::emNormal, S2CPacketSyncPlayerDownChanged)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(bool, isDown)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerSetFlyingState, DataPacket::emNormal, S2CPacketSyncPlayerSetFlyingState)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(bool, isFlying)
_END_PACKET

//S2CPacketEntityTeleport
_BEGIN_PACKET_S2C(S2CPacketType::EntityPlayerTeleport, DataPacket::emNormal, S2CPacketEntityPlayerTeleport)
_SERIALIZE_MEMBER(i32, m_entityId)
_SERIALIZE_MEMBER(i32, m_x)
_SERIALIZE_MEMBER(i32, m_y)
_SERIALIZE_MEMBER(i32, m_z)
_END_PACKET

//S2CPacketCustomTipMsg
_BEGIN_PACKET_S2C(S2CPacketType::CustomTipMsg, DataPacket::emNormal, S2CPacketCustomTipMsg)
_SERIALIZE_MEMBER(String, messageLang)
_SERIALIZE_MEMBER(String, extra)
_END_PACKET

//S2CPacketContentTipMsg
_BEGIN_PACKET_S2C(S2CPacketType::ContentTipMsg, DataPacket::emNormal, S2CPacketContentTipMsg)
_SERIALIZE_MEMBER(String, messageLang)
_SERIALIZE_MEMBER(String, titleLang)
_END_PACKET


//S2CPacketEntityPlayerOccupation
_BEGIN_PACKET_S2C(S2CPacketType::EntityPlayerOccupation, DataPacket::emNormal, S2CPacketEntityPlayerOccupation)
_SERIALIZE_MEMBER(int, entityId)
_SERIALIZE_MEMBER(int , occupation)
_END_PACKET


//S2CPacketTeamResourcesUpdate
_BEGIN_PACKET_S2C(S2CPacketType::TeamResourcesUpdate, DataPacket::emNormal, S2CPacketTeamResourcesUpdate)
_SERIALIZE_MEMBER(String, m_data)
_END_PACKET

//S2CPacketCloseBGM
_BEGIN_PACKET_S2C(S2CPacketType::CloseBGM, DataPacket::emNormal, S2CPacketCloseBGM)
_END_PACKET

//S2CPacketSetArmStack
_BEGIN_PACKET_S2C(S2CPacketType::SetArmItem, DataPacket::emNormal, S2CPacketSetArmItem)
_SERIALIZE_MEMBER(int, itemId)
_END_PACKET

//S2CPacketActorNpcContent
_BEGIN_PACKET_S2C(S2CPacketType::ActorNpcContent, DataPacket::emNormal, S2CPacketActorNpcContent)
_SERIALIZE_MEMBER(int, entityId)
_SERIALIZE_MEMBER(String, content)
_END_PACKET

//S2CPacketBuildProgress
_BEGIN_PACKET_S2C(S2CPacketType::BuildProgress, DataPacket::emNormal, S2CPacketBuildProgress)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(int, curProgress)
_SERIALIZE_MEMBER(int, maxProgress)
_SERIALIZE_MEMBER(String, iconPath)
_END_PACKET

// S2CPacketEnchantmentPropsData
_BEGIN_PACKET_S2C(S2CPacketType::EnchantmentPropsData, DataPacket::emNormal, S2CPacketEnchantmentPropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// S2CPacketPersonalSuperPropsData
_BEGIN_PACKET_S2C(S2CPacketType::PersonalSuperPropsData, DataPacket::emNormal, S2CPacketPersonalSuperPropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketEnableMovement
_BEGIN_PACKET_S2C(S2CPacketType::EnableMovement, DataPacket::emNormal, S2CPacketEnableMovement)
_SERIALIZE_MEMBER(int, entityId)
_END_PACKET

//S2CPacketConsumeCoinTip
_BEGIN_PACKET_S2C(S2CPacketType::ConsumeCoinTip, DataPacket::emNormal, S2CPacketConsumeCoinTip)
_SERIALIZE_MEMBER(String, message)
_SERIALIZE_MEMBER(int, coinId)
_SERIALIZE_MEMBER(int, price)
_SERIALIZE_MEMBER(String, extra)
_END_PACKET

//S2CPacketSyncEntityBlockmanInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityBlockman, DataPacket::emNormal, S2CPacketSyncEntityBlockmanInfo)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, blockmanType)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, actorBody)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, actorBodyId)
_SERIALIZE_MEMBER(float, maxSpeed)
_SERIALIZE_MEMBER(float, curSpeed)
_SERIALIZE_MEMBER(float, addSpeed)
_SERIALIZE_MEMBER(bool, isRecordRoute)
_SERIALIZE_MEMBER(int, longHitTimes)
_END_PACKET

//S2CPacketBlockmanMoveAttr
_BEGIN_PACKET_S2C(S2CPacketType::BlockmanMoveAttr, DataPacket::emNormal, S2CPacketBlockmanMoveAttr)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, pushX)
_SERIALIZE_MEMBER(float, pushZ)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, motion)
_END_PACKET

//S2CSyncCannonActorInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncCannonActorInfo, DataPacket::emNormal, S2CSyncCannonActorInfo)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isCanObstruct)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, headName)
_SERIALIZE_MEMBER(String, skillName)
_SERIALIZE_MEMBER(String, haloEffectName)
_SERIALIZE_MEMBER(String, content)
_SERIALIZE_MEMBER(String, bodyId)
_SERIALIZE_MEMBER(String, body)
_END_PACKET

// S2CPacketSuperPropsData
_BEGIN_PACKET_S2C(S2CPacketType::SuperPropsData, DataPacket::emNormal, S2CPacketSuperPropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// GameTipNotification
_BEGIN_PACKET_S2C(S2CPacketType::GameTipNotification, DataPacket::emNormal, S2CPacketGameTipNotification)
_SERIALIZE_MEMBER(i32, m_tipType)
_SERIALIZE_MEMBER(std::shared_ptr<ChatComponentNotification>, m_chatComponent)
_END_PACKET

// S2CPacketChatString
_BEGIN_PACKET_S2C(S2CPacketType::ChatMessageString, DataPacket::emNormal, S2CPacketChatString)
_SERIALIZE_MEMBER(i32, m_chatType)
_SERIALIZE_MEMBER(std::shared_ptr<ChatComponentString>, m_chatComponent)
_END_PACKET

// S2CPacketChatTranslation
_BEGIN_PACKET_S2C(S2CPacketType::ChatMessageTranslation, DataPacket::emNormal, S2CPacketChatTranslation)
_SERIALIZE_MEMBER(i32, m_chatType)
_SERIALIZE_MEMBER(std::shared_ptr<ChatComponentTranslation>, m_chatComponent)
_END_PACKET

// ChatComponentNotification
_BEGIN_PACKET_S2C(S2CPacketType::ChatMessageNotification, DataPacket::emNormal, S2CPacketChatNotification)
_SERIALIZE_MEMBER(i32, m_chatType)
_SERIALIZE_MEMBER(std::shared_ptr<ChatComponentNotification>, m_chatComponent)
_END_PACKET

//S2CPacketChangeDefense
_BEGIN_PACKET_S2C(S2CPacketType::ChangeDefense, DataPacket::emNormal, S2CPacketChangeDefense)
_SERIALIZE_MEMBER(i32, defense)
_SERIALIZE_MEMBER(i32, maxDefense)
_END_PACKET

//S2CPacketChangeAttack
_BEGIN_PACKET_S2C(S2CPacketType::ChangeAttack, DataPacket::emNormal, S2CPacketChangeAttack)
_SERIALIZE_MEMBER(i32, attack)
_SERIALIZE_MEMBER(i32, maxAttack)
_END_PACKET

//S2CPacketSyncPlayerSpecialJump
_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerSpecialJump, DataPacket::emNormal, S2CPacketSyncPlayerSpecialJump)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(float, yFactor)
_SERIALIZE_MEMBER(float, xFactor)
_SERIALIZE_MEMBER(float, zFactor)
_END_PACKET

//S2CPacketAnimate
_BEGIN_PACKET_S2C(S2CPacketType::Animate, DataPacket::emNormal, S2CPacketAnimate)
_SERIALIZE_MEMBER(i32, m_entityRuntimeId)
_SERIALIZE_MEMBER(NETWORK_DEFINE::PacketAnimateType, m_animateType)
_SERIALIZE_MEMBER(i32, m_value)
_SERIALIZE_MEMBER(i32, m_holdItemId)
_END_PACKET

//S2CPacketSyncPlayerSetSpYaw
_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerSetSpYaw, DataPacket::emNormal, S2CPacketSyncPlayerSetSpYaw)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(bool, isSpYaw)
_SERIALIZE_MEMBER(float, radian)
_END_PACKET

//S2CPacketShowRanchExTip
_BEGIN_PACKET_S2C(S2CPacketType::ShowRanchExTip, DataPacket::emNormal, S2CPacketShowRanchExTip)
_SERIALIZE_MEMBER(ui16, type)
_SERIALIZE_MEMBER(ui16, num)
_END_PACKET

//S2CPacketShowRanchExTask
_BEGIN_PACKET_S2C(S2CPacketType::ShowRanchExTask, DataPacket::emNormal, S2CPacketShowRanchExTask)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(String, task)
_END_PACKET

//S2CPacketShowRanchExCurrentItemInfo
_BEGIN_PACKET_S2C(S2CPacketType::ShowRanchExCurrentItemInfo, DataPacket::emNormal, S2CPacketShowRanchExCurrentItemInfo)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(String, itemInfo)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(i32, num)
_END_PACKET

//S2CPacketShowRanchExItem
_BEGIN_PACKET_S2C(S2CPacketType::ShowRanchExItem, DataPacket::emNormal, S2CPacketShowRanchExItem)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(String, item)
_END_PACKET

//S2CPacketAddSimpleEffect
_BEGIN_PACKET_S2C(S2CPacketType::AddSimpleEffect, DataPacket::emNormal, S2CPacketAddSimpleEffect)
_SERIALIZE_MEMBER(String, effectName)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(int, duration)
_SERIALIZE_MEMBER(float, scale)
_END_PACKET

//S2CPacketRemoveSimpleEffect
_BEGIN_PACKET_S2C(S2CPacketType::RemoveSimpleEffect, DataPacket::emNormal, S2CPacketRemoveSimpleEffect)
_SERIALIZE_MEMBER(String, effectName)
_END_PACKET

//S2CPacketCarDirection
_BEGIN_PACKET_S2C(S2CPacketType::CarDirection, DataPacket::emNormal, S2CPacketCarDirection)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(i32, direction)
_END_PACKET

//S2CPacketCarProgress
_BEGIN_PACKET_S2C(S2CPacketType::CarProgress, DataPacket::emNormal, S2CPacketCarProgress)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(float, progress)
_END_PACKET

//S2CPacketEntityFrozenTime
_BEGIN_PACKET_S2C(S2CPacketType::EntityFrozenTime, DataPacket::emNormal, S2CPacketEntityFrozenTime)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, time)
_END_PACKET

//S2CPacketSyncPlayerSprintChanged
_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerSprintChanged, DataPacket::emNormal, S2CPacketSyncPlayerSprintChanged)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(bool, isSprintInput)
_END_PACKET

#endif