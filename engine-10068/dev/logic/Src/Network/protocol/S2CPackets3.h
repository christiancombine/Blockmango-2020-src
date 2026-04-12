#pragma once
#ifndef __S2C_PACKETS3_H__
#define __S2C_PACKETS3_H__

#include "Network/protocol/PacketDefine.h"
#include "Network/NetworkDefine.h"
#include "Game/BirdSimulator.h"
#include "Game/BlockCity.h"
#include "Game/BlockFort.h"
#include "Setting/ActionAreaManager.h"
#include "Setting/SceneSetting.h"
#include "Setting/AppItemsSetting.h"
#include "Entity/Entity.h"

using namespace LORD;

//S2CPacketSyncBirdSimulatorStoreItem
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdSimulatorStoreItem, DataPacket::emNormal, S2CPacketSyncBirdSimulatorStoreItem)
_SERIALIZE_MEMBER(BLOCKMAN::BirdStore, store)
_END_PACKET

//S2CPacketSyncBirdSimulatorTaskItem
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdSimulatorTaskItem, DataPacket::emNormal, S2CPacketSyncBirdSimulatorTaskItem)
_SERIALIZE_MEMBER(bool, isRemove)
_SERIALIZE_MEMBER(BLOCKMAN::BirdTask, taskItem)
_END_PACKET

//S2CPacketSyncBirdSimulatorPersonStoreTab
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdSimulatorPersonStoreTab, DataPacket::emNormal, S2CPacketSyncBirdSimulatorPersonStoreTab)
_SERIALIZE_MEMBER(BLOCKMAN::BirdPersonalStoreTab, storeTab)
_END_PACKET

//S2CPacketBirdAddMessage
_BEGIN_PACKET_S2C(S2CPacketType::BirdAddMessage, DataPacket::emNormal, S2CPacketBirdAddMessage)
_SERIALIZE_MEMBER(String, message)
_END_PACKET

//S2CPacketChangeGuideArrowStatus
_BEGIN_PACKET_S2C(S2CPacketType::ChangeGuideArrowStatus, DataPacket::emNormal, S2CPacketChangeGuideArrowStatus)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, arrowPos)
_SERIALIZE_MEMBER(bool, flag)
_END_PACKET

//S2CPacketAddWallText
_BEGIN_PACKET_S2C(S2CPacketType::AddWallText, DataPacket::emNormal, S2CPacketAddWallText)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(i32, timeLeft)
_SERIALIZE_MEMBER(String, text)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, textPos)
_SERIALIZE_MEMBER(float, scale)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(float, pitch)
_SERIALIZE_MEMBER(float, r)
_SERIALIZE_MEMBER(float, g)
_SERIALIZE_MEMBER(float, b)
_SERIALIZE_MEMBER(float, a)
_END_PACKET

//S2CPacketDeleteWallText
_BEGIN_PACKET_S2C(S2CPacketType::DeleteWallText, DataPacket::emNormal, S2CPacketDeleteWallText)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, textPos)
_END_PACKET

//S2CPacketKeepItemAndReviveCountDown
_BEGIN_PACKET_S2C(S2CPacketType::KeepItemAndReviveCountDown, DataPacket::emNormal, S2CPacketKeepItemAndReviveCountDown)
_SERIALIZE_MEMBER(i32, m_second)
_SERIALIZE_MEMBER(int, type)
_END_PACKET


//S2CPacketActorNpcSuspended
_BEGIN_PACKET_S2C(S2CPacketType::ActorNpcSuspended, DataPacket::emNormal, S2CPacketActorNpcSuspended)
_SERIALIZE_MEMBER(int, entityId)
_SERIALIZE_MEMBER(bool, suspended)
_SERIALIZE_MEMBER(Vector3, pos)
_END_PACKET

//S2CPacketSessionNpcHeadInfoMultiLang
_BEGIN_PACKET_S2C(S2CPacketType::ActorNpcHeadInfoMultilang, DataPacket::emNormal, S2CPacketSessionNpcHeadInfoMultiLang)
_SERIALIZE_MEMBER(int, entityId)
_SERIALIZE_MEMBER(String, msg)
_SERIALIZE_MEMBER(int, lv)
_SERIALIZE_MEMBER(int, time)
_END_PACKET

//S2CPacketChangeApple
_BEGIN_PACKET_S2C(S2CPacketType::ChangeApple, DataPacket::emNormal, S2CPacketChangeApple)
_SERIALIZE_MEMBER(i32, cur)
_SERIALIZE_MEMBER(i32, max)
_END_PACKET

//S2CPacketChangeQuicklyBuildBlockStatus
_BEGIN_PACKET_S2C(S2CPacketType::ChangeQuicklyBuildBlockStatus, DataPacket::emNormal, S2CPacketChangeQuicklyBuildBlockStatus)
_SERIALIZE_MEMBER(bool, m_allowStatus)
_SERIALIZE_MEMBER(bool, m_currStatus)
_END_PACKET

//S2CPacketAppleFresh
_BEGIN_PACKET_S2C(S2CPacketType::AppleFresh, DataPacket::emNormal, S2CPacketAppleFresh)
_SERIALIZE_MEMBER(i32, time)
_END_PACKET

//S2CPacketSetPlayerGlide
_BEGIN_PACKET_S2C(S2CPacketType::SetPlayerGlide, DataPacket::emNormal, S2CPacketSetPlayerGlide)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isGlide)
_SERIALIZE_MEMBER(bool, isEquipWing)
_SERIALIZE_MEMBER(float, glideMoveAddition)
_SERIALIZE_MEMBER(float, glideDropResistance)
_END_PACKET

//S2CPacketShowAddCarryGuide
_BEGIN_PACKET_S2C(S2CPacketType::ShowAddCarryGuide, DataPacket::emNormal, S2CPacketShowAddCarryGuide)
_END_PACKET

//S2CPacketShowAddCarryGuide
_BEGIN_PACKET_S2C(S2CPacketType::BirdCheckinData, DataPacket::emNormal, S2CPacketBirdCheckinData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketShowBirdFeedGuide
_BEGIN_PACKET_S2C(S2CPacketType::ShowBirdFeedGuide, DataPacket::emNormal, S2CPacketShowBirdFeedGuide)
_END_PACKET

//S2CPacketBirdShowWingTip
_BEGIN_PACKET_S2C(S2CPacketType::BirdShowWingTip, DataPacket::emNormal, S2CPacketBirdShowWingTip)
_SERIALIZE_MEMBER(i32, page)
_END_PACKET

//S2CPacketDefaultNotification
_BEGIN_PACKET_S2C(S2CPacketType::DefaultNotification, DataPacket::emNormal, S2CPacketDefaultNotification)
_SERIALIZE_MEMBER(String, content)
_END_PACKET

//S2CPacketNotifyKills
_BEGIN_PACKET_S2C(S2CPacketType::NotifyKills, DataPacket::emNormal, S2CPacketNotifyKills)
_SERIALIZE_MEMBER(String, icon)
_SERIALIZE_MEMBER(uint, soundType)
_SERIALIZE_MEMBER(i32, stay)
_SERIALIZE_MEMBER(i32, animate)
_END_PACKET

//S2CPacketSupplyTip
_BEGIN_PACKET_S2C(S2CPacketType::SupplyTip, DataPacket::emNormal, S2CPacketSupplyTip)
_SERIALIZE_MEMBER(String, icon)
_SERIALIZE_MEMBER(i32, value)
_SERIALIZE_MEMBER(i32, type)
_END_PACKET

//S2CPacketPickUpEntityItem
_BEGIN_PACKET_S2C(S2CPacketType::PickUpEntityItem, DataPacket::emNormal, S2CPacketPickUpEntityItem)
_SERIALIZE_MEMBER(int, m_playerEntityId)
_SERIALIZE_MEMBER(int, m_itemEntityId)
_END_PACKET

// S2CPacketPlayingSelectableAction
_BEGIN_PACKET_S2C(S2CPacketType::PlayingSelectableAction, DataPacket::emNormal, S2CPacketPlayingSelectableAction)
_SERIALIZE_MEMBER(int, m_entityId)
_SERIALIZE_MEMBER(int, m_actionId)
_SERIALIZE_MEMBER(String, m_langKey)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::AllowUseItemStatus, DataPacket::emNormal, S2CPacketAllowUseItemStatus)
_SERIALIZE_MEMBER(i32, m_status)
_END_PACKET

//S2CPacketUpdateSelectableAction
_BEGIN_PACKET_S2C(S2CPacketType::UpdateSelectableAction, DataPacket::emNormal, S2CPacketUpdateSelectableAction)
_SERIALIZE_MEMBER(vector<i32>::type, m_selectableAction)
_END_PACKET

//S2CPacketAppItemsSettingInfo
_BEGIN_PACKET_S2C(S2CPacketType::AppItemsSettingInfo, DataPacket::emNormal, S2CPacketAppItemsSettingInfo)
_SERIALIZE_MEMBER(vector<BLOCKMAN::SelectableActionInfo>::type, m_actionInfoList)
_END_PACKET

//S2CPacketSceneSettingInfo
_BEGIN_PACKET_S2C(S2CPacketType::SceneSettingInfo, DataPacket::emNormal, S2CPacketSceneSettingInfo)
_SERIALIZE_MEMBER(vector<BLOCKMAN::SkyInfo>::type, skyInfoList)
_SERIALIZE_MEMBER(String, sunName)
_SERIALIZE_MEMBER(String, moonName)
_SERIALIZE_MEMBER(int, skyType)
_SERIALIZE_MEMBER(int, cloudType)
_SERIALIZE_MEMBER(bool, showSunAndMoon)
_SERIALIZE_MEMBER(bool, showMeteor)
_SERIALIZE_MEMBER(bool, showStarEffect)
_END_PACKET

//S2CPacketUpdateGuideData
_BEGIN_PACKET_S2C(S2CPacketType::UpdateGuideData, DataPacket::emNormal, S2CPacketUpdateGuideData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketAttackScore
_BEGIN_PACKET_S2C(S2CPacketType::AttackScore, DataPacket::emNormal, S2CPacketAttackScore)
_SERIALIZE_MEMBER(i32, score)
_SERIALIZE_MEMBER(i32, scoreType)
_END_PACKET

//S2CPacketAppItemsSettingInfo
_BEGIN_PACKET_S2C(S2CPacketType::PlayerActionArea, DataPacket::emNormal, S2CPacketPlayerActionArea)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ActionArea>::type, m_areas)
_END_PACKET

//S2CPacketSyncEntityDecoration
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityDecoration, DataPacket::emNormal, S2CPacketSyncEntityDecoration)
_SERIALIZE_MEMBER(i64, userId)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, nodeSize)
_SERIALIZE_MEMBER(i32, decorationId)
_SERIALIZE_MEMBER(bool, isPlayAction)
_SERIALIZE_MEMBER(vector<String>::type, nodeKeys)
_SERIALIZE_MEMBER(vector<ui64>::type, nodeValues)
_END_PACKET

//S2CPacketSyncTakeDecoration
_BEGIN_PACKET_S2C(S2CPacketType::SyncTakeDecoration, DataPacket::emNormal, S2CPacketSyncTakeDecoration)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(i32, decorationEntityId)
_END_PACKET

//S2CPacketSyncBlockCityManor
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityManorManager, DataPacket::emNormal, S2CPacketSyncBlockCityManor)
_SERIALIZE_MEMBER(BLOCKMAN::BlockCityManorManager, manorManager)
_END_PACKET

//S2CPacketSyncBlockCityPlayerInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityPlayerInfo, DataPacket::emNormal, S2CPacketSyncBlockCityPlayerInfo)
_SERIALIZE_MEMBER(BLOCKMAN::BlockCityPlayerInfo, playerInfo)
_END_PACKET

//S2CPacketShowBlockCityCommonTip
_BEGIN_PACKET_S2C(S2CPacketType::ShowBlockCityCommonTip, DataPacket::emNormal, S2CPacketShowBlockCityCommonTip)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockCityPayment>::type, pays)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(String, msg)
_END_PACKET

//S2CPacketSyncBlockCityStore
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityStore, DataPacket::emNormal, S2CPacketSyncBlockCityStore)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockCityStoreTab>::type, stores)
_END_PACKET

//S2CPacketSyncBlockCityManorInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityManorInfo, DataPacket::emNormal, S2CPacketSyncBlockCityManorInfo)
_SERIALIZE_MEMBER(BLOCKMAN::BlockCityManorInfo, manorInfo)
_END_PACKET

//S2CPacketSyncBlockCityBuyLackItems
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityBuyLackItems, DataPacket::emNormal, S2CPacketSyncBlockCityBuyLackItems)
_SERIALIZE_MEMBER(BLOCKMAN::BlockCityBuyLackItems, lackItems)
_END_PACKET

//S2CPacketShowRotateView
_BEGIN_PACKET_S2C(S2CPacketType::ShowRotateView, DataPacket::emNormal, S2CPacketShowRotateView)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(i32, type)
_END_PACKET

//S2CPacketuUpdateBlockCityItems
_BEGIN_PACKET_S2C(S2CPacketType::UpdateBlockCityItems, DataPacket::emNormal, S2CPacketuUpdateBlockCityItems)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockCityStoreItem>::type, items)
_END_PACKET

//S2CPacketCommonTip
_BEGIN_PACKET_S2C(S2CPacketType::CommonTip, DataPacket::emNormal, S2CPacketCommonTip)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(String, tips)
_END_PACKET

//S2CPacketCraftItemUpdate
_BEGIN_PACKET_S2C(S2CPacketType::CraftItemUpdate, DataPacket::emNormal, S2CPacketCraftItemUpdate)
_END_PACKET


//S2CPacketUpdateSkyBlockMainLineTaskData
_BEGIN_PACKET_S2C(S2CPacketType::UpdateSkyBlockMainLineTaskData, DataPacket::emNormal, S2CPacketUpdateSkyBlockMainLineTaskData)
_SERIALIZE_MEMBER(int, lv)
_SERIALIZE_MEMBER(int, progressesLevel)
_SERIALIZE_MEMBER(int, dareIsUnLock)
_SERIALIZE_MEMBER(String, data)
_SERIALIZE_MEMBER(int, taskType)
_END_PACKET

//S2CPackeSkyBlockDareTaskInfo
_BEGIN_PACKET_S2C(S2CPacketType::SkyBlockDareTaskInfo, DataPacket::emNormal, S2CPackeSkyBlockDareTaskInfo)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketSetMoveState
_BEGIN_PACKET_S2C(S2CPacketType::SetMoveState, DataPacket::emNormal, S2CPacketSetMoveState)
_SERIALIZE_MEMBER(i32, state)
_END_PACKET

//S2CPackeSkyBlockRankData
_BEGIN_PACKET_S2C(S2CPacketType::SkyBlockRankData, DataPacket::emNormal, S2CPackeSkyBlockRankData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketCanUseItemBucket
_BEGIN_PACKET_S2C(S2CPacketType::CanUseItemBucket, DataPacket::emNormal, S2CPacketCanUseItemBucket)
_SERIALIZE_MEMBER(bool, canUseItemBucket)
_END_PACKET

//S2CPacketSceneSettingSwitch
_BEGIN_PACKET_S2C(S2CPacketType::SceneSettingSwitch, DataPacket::emNormal, S2CPacketSceneSettingSwitch)
_SERIALIZE_MEMBER(int, skyType)
_SERIALIZE_MEMBER(int, cloudType)
_SERIALIZE_MEMBER(bool, showSunAndMoon)
_SERIALIZE_MEMBER(bool, showMeteor)
_SERIALIZE_MEMBER(bool, showStarEffect)
_END_PACKET

//S2CPackeSkyBlockShowAppShopPage
_BEGIN_PACKET_S2C(S2CPacketType::SkyBlockShowAppShopPage, DataPacket::emNormal, S2CPackeSkyBlockShowAppShopPage)
_SERIALIZE_MEMBER(i32, pageId)
_END_PACKET

//S2CPackeNotifyGetGoodsByItemId
_BEGIN_PACKET_S2C(S2CPacketType::NotifyGetGoodsByItemId, DataPacket::emNormal, S2CPackeNotifyGetGoodsByItemId)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, meta)
_SERIALIZE_MEMBER(i32, num)
_SERIALIZE_MEMBER(String, img)
_END_PACKET

//S2CPacketServerCommonData
_BEGIN_PACKET_S2C(S2CPacketType::ServerCommonData, DataPacket::emNormal, S2CPacketServerCommonData)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(String, key)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketSyncBlockFortManorInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockFortManorInfo, DataPacket::emNormal, S2CPacketSyncBlockFortManorInfo)
_SERIALIZE_MEMBER(BLOCKMAN::BlockFortManorInfo, manorInfo)
_END_PACKET

//S2CPacketSyncBlockFortManorManager
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockFortManorManager, DataPacket::emNormal, S2CPacketSyncBlockFortManorManager)
_SERIALIZE_MEMBER(BLOCKMAN::BlockFortManorManager, manorManager)
_END_PACKET

//S2CPacketSyncBlockFortPlayerInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockFortPlayerInfo, DataPacket::emNormal, S2CPacketSyncBlockFortPlayerInfo)
_SERIALIZE_MEMBER(BLOCKMAN::BlockFortPlayerInfo, playerInfo)
_END_PACKET

//S2CPacketSyncBlockFortStore
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockFortStore, DataPacket::emNormal, S2CPacketSyncBlockFortStore)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockFortStoreTab>::type, stores)
_END_PACKET

//S2CPacketuUpdateBlockFortItems
_BEGIN_PACKET_S2C(S2CPacketType::UpdateBlockFortItems, DataPacket::emNormal, S2CPacketuUpdateBlockFortItems)
_SERIALIZE_MEMBER(BLOCKMAN::BlockFortStoreItem, items)
_SERIALIZE_MEMBER(i32, tabId)
_END_PACKET

//S2CPacketuShowBlockFortBuildingInfo
_BEGIN_PACKET_S2C(S2CPacketType::ShowBlockFortBuildingInfo, DataPacket::emNormal, S2CPacketuShowBlockFortBuildingInfo)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(i32, index)
_END_PACKET

//S2CPacketShowFloatTip
_BEGIN_PACKET_S2C(S2CPacketType::ShowFlotTip, DataPacket::emNormal, S2CPacketShowFloatTip)
_SERIALIZE_MEMBER(String, msg)
_END_PACKET

//S2CPacketSyncBlockCityHouseShop
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityHouseShop, DataPacket::emNormal, S2CPacketSyncBlockCityHouseShop)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockCityHouseShopClassify>::type, shops)
_END_PACKET

//S2CPacketSyncBlockCityPackTab
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityPackTab, DataPacket::emNormal, S2CPacketSyncBlockCityPackTab)
_SERIALIZE_MEMBER(BLOCKMAN::BlockCityHouseShopClassify, packTab)
_END_PACKET

//S2CPacketSyncBlockCityManorArea
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityManorArea, DataPacket::emNormal, S2CPacketSyncBlockCityManorArea)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockCityManorArea>::type, manorAreas)
_END_PACKET

//S2CPacketSetPlayerElevator
_BEGIN_PACKET_S2C(S2CPacketType::SetPlayerElevator, DataPacket::emNormal, S2CPacketSetPlayerElevator)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, floor)
_SERIALIZE_MEMBER(bool, isUse)
_SERIALIZE_MEMBER(float, speed)
_SERIALIZE_MEMBER(i32, arriveFloor)
_SERIALIZE_MEMBER(i32, elevatorId)
_SERIALIZE_MEMBER(float, arriveHeight)
_END_PACKET

//S2CPacketSyncBlockCityElevatorArea
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityElevatorArea, DataPacket::emNormal, S2CPacketSyncBlockCityElevatorArea)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockCityElevatorArea>::type, elevatorAreas)
_END_PACKET

//S2CPacketSyncBlockCityWeekReward
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityWeekReward, DataPacket::emNormal, S2CPacketSyncBlockCityWeekReward)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockCityWeekReward>::type, weekRewards)
_END_PACKET

//S2CPacketSyncBlockCityArchive
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockCityArchive, DataPacket::emNormal, S2CPacketSyncBlockCityArchive)
_SERIALIZE_MEMBER(BLOCKMAN::BlockCityArchive, archive)
_END_PACKET


// S2CPacketUpdateBlockChunks
_BEGIN_PACKET_S2C(S2CPacketType::UpdateBlockChunks, DataPacket::emMapData, S2CPacketUpdateBlockChunks)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::TempChunk>::type, updates)
_END_PACKET

//S2CPacketPlayVehicleHornSound
_BEGIN_PACKET_S2C(S2CPacketType::SyncVehicleHornSound, DataPacket::emMapData, S2CPacketPlayVehicleHornSound)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, pos)
_END_PACKET

//S2CPacketDBDataReady
_BEGIN_PACKET_S2C(S2CPacketType::DBDataReady, DataPacket::emNormal, S2CPacketDBDataReady)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncCreatureTargetId, DataPacket::emNormal, S2CPacketSyncCreatureTargetId)
_SERIALIZE_MEMBER(int, EntityId)
_SERIALIZE_MEMBER(int, TargetId)
_END_PACKET

//S2CPacketUserRelation
_BEGIN_PACKET_S2C(S2CPacketType::SyncUserRelation, DataPacket::emNormal, S2CPacketUserRelation)
_SERIALIZE_MEMBER(vector<i64>::type, friends)
_END_PACKET

//S2CPacketSyncBindEntity
_BEGIN_PACKET_S2C(S2CPacketType::SyncBindEntity, DataPacket::emNormal, S2CPacketSyncBindEntity)
_SERIALIZE_MEMBER(int, parentEntityId)
_SERIALIZE_MEMBER(int, childEntityId)
_SERIALIZE_MEMBER(String, nodeName)
_SERIALIZE_MEMBER(int, idleActionId)
_END_PACKET

//S2CPacketSyncUnbindEntity
_BEGIN_PACKET_S2C(S2CPacketType::SyncUnbindEntity, DataPacket::emNormal, S2CPacketSyncUnbindEntity)
_SERIALIZE_MEMBER(int, childEntityId)
_END_PACKET

//S2CPacketSyncEntityConnection
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityConnection, DataPacket::emNormal, S2CPacketSyncEntityConnection)
_SERIALIZE_MEMBER(i32, myEntityId)
_SERIALIZE_MEMBER(i32, parentEntityId)
_SERIALIZE_MEMBER(String, nodeName)
_SERIALIZE_MEMBER(i32, idleActionId)
_SERIALIZE_MEMBER(vector<i32>::type, childrenEntityId)
_END_PACKET


//S2CFlyingMountMovement
_BEGIN_PACKET_S2C(S2CPacketType::FlyingMountMovement, DataPacket::emNormal, S2CPacketFlyingMountMovement)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(real32, bodyOrientW)
_SERIALIZE_MEMBER(real32, bodyOrientX)
_SERIALIZE_MEMBER(real32, bodyOrientY)
_SERIALIZE_MEMBER(real32, bodyOrientZ)
_SERIALIZE_MEMBER(real32, viewOrientW)
_SERIALIZE_MEMBER(real32, viewOrientX)
_SERIALIZE_MEMBER(real32, viewOrientY)
_SERIALIZE_MEMBER(real32, viewOrientZ)
_END_PACKET

//S2CPacketMountFlyingMount
_BEGIN_PACKET_S2C(S2CPacketType::MountFlyingMount, DataPacket::emNormal, S2CPacketMountFlyingMount)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, mountId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::FloatingMountMovement, DataPacket::emNormal, S2CPacketFloatingMountMovement)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(real32, bodyYaw)
_SERIALIZE_MEMBER(real32, viewYaw)
_SERIALIZE_MEMBER(real32, pitch)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::MountFloatingMount, DataPacket::emNormal, S2CPacketMountFloatingMount)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, mountId)
_END_PACKET

//S2CPacketSyncGameWeather
_BEGIN_PACKET_S2C(S2CPacketType::SyncGameWeather, DataPacket::emNormal, S2CPacketSyncGameWeather)
_SERIALIZE_MEMBER(i32, weatherType)
_END_PACKET

// S2CPacketPlaceBlocks
_BEGIN_PACKET_S2C(S2CPacketType::PlaceBlocks, DataPacket::emNormal, S2CPacketPlaceBlocks)
_END_PACKET

// S2CPacketUpdateBlocks
_BEGIN_PACKET_S2C(S2CPacketType::UpdateBlocks, DataPacket::emMapData, S2CPacketUpdateBlocks)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::S2CPacketUpdateBlockInfo>::type, updates)
_END_PACKET

//S2CPacketShowSkyShopInfo
_BEGIN_PACKET_S2C(S2CPacketType::ShowSkyShopInfo, DataPacket::emNormal, S2CPacketShowSkyShopInfo)
_SERIALIZE_MEMBER(int, sortType)
_SERIALIZE_MEMBER(int, lv)
_SERIALIZE_MEMBER(String, priData)
_END_PACKET

#endif