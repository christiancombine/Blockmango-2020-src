#pragma once
#ifndef __S2C_PACKETS5_H__
#define __S2C_PACKETS5_H__

#include "Network/protocol/PacketDefine.h"
#include "Network/NetworkDefine.h"
#include "Item/ItemStack.h"
#include "Util/AboutShopping.h"
#include "Entity/Entity.h"
#include "Entity/EntityActorCannon.h"

// S2CPacketAddEntityEffect
_BEGIN_PACKET_S2C(S2CPacketType::AddEntityEffect, DataPacket::emNormal, S2CPacketAddEntityEffect)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, potionId)
_SERIALIZE_MEMBER(i32, amplifier)
_SERIALIZE_MEMBER(i32, duration)
_SERIALIZE_MEMBER(bool, isAmbient)
_SERIALIZE_MEMBER(bool, isSplashPotion)
_SERIALIZE_MEMBER(bool, isPotionDurationMax)
_END_PACKET

// S2CPacketRemoveEntityEffect
_BEGIN_PACKET_S2C(S2CPacketType::RemoveEntityEffect, DataPacket::emNormal, S2CPacketRemoveEntityEffect)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, potionId)
_END_PACKET

// S2CPacketUsePropResults
_BEGIN_PACKET_S2C(S2CPacketType::UsePropResults, DataPacket::emNormal, S2CPacketUsePropResults)
_SERIALIZE_MEMBER(String, m_propId)
_SERIALIZE_MEMBER(i32, m_results)
_END_PACKET


// S2CPacketUserIn
_BEGIN_PACKET_S2C(S2CPacketType::UserIn, DataPacket::emNormal, S2CPacketUserIn)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_SERIALIZE_MEMBER(String, m_nickName)
_SERIALIZE_MEMBER(i32, m_teamId)
_SERIALIZE_MEMBER(String, m_teamName)
_SERIALIZE_MEMBER(i32, m_curPlayer)
_SERIALIZE_MEMBER(i32, m_maxPlayer)
_SERIALIZE_MEMBER(i32, m_vip)
_SERIALIZE_MEMBER(i32, m_sex)
_SERIALIZE_MEMBER(String, m_picUrl)
_SERIALIZE_MEMBER(String, m_country)
_SERIALIZE_MEMBER(String, m_lang)
_END_PACKET

// S2CPacketUserOut
_BEGIN_PACKET_S2C(S2CPacketType::UserOut, DataPacket::emNormal, S2CPacketUserOut)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_END_PACKET

// S2CPacketLoginResult
_BEGIN_PACKET_S2C(S2CPacketType::LoginFailure, DataPacket::emNormal, S2CPacketLoginResult)
_SERIALIZE_MEMBER(i32, m_resultCode)
_END_PACKET

// S2CPacketPing
_BEGIN_PACKET_S2C(S2CPacketType::Ping, DataPacket::emNormal, S2CPacketPing)
_SERIALIZE_MEMBER(ui32, m_clientSendPingTime)
_END_PACKET

// S2CPacketSyncWorldTime
_BEGIN_PACKET_S2C(S2CPacketType::SyncWorldTime, DataPacket::emNormal, S2CPacketSyncWorldTime)
_SERIALIZE_MEMBER(i64, time)
_END_PACKET

// S2CPacketSettlement
_BEGIN_PACKET_S2C(S2CPacketType::Settlement, DataPacket::emNormal, S2CPacketSettlement)
_SERIALIZE_MEMBER(String, m_gameType)
_SERIALIZE_MEMBER(String, m_gameResult)
_END_PACKET

// S2CPacketGameover
_BEGIN_PACKET_S2C(S2CPacketType::Gameover, DataPacket::emNormal, S2CPacketGameover)
_SERIALIZE_MEMBER(String, m_overMessage)
_SERIALIZE_MEMBER(i32, m_code)
_END_PACKET

// S2CPacketCloseContainer
_BEGIN_PACKET_S2C(S2CPacketType::CloseContainer, DataPacket::emNormal, S2CPacketCloseContainer)
_SERIALIZE_MEMBER(bool8, m_close)
_SERIALIZE_MEMBER(i32, m_playerID)
_SERIALIZE_MEMBER(i16, m_containerPosX)
_SERIALIZE_MEMBER(i16, m_containerPosY)
_SERIALIZE_MEMBER(i16, m_containerPosZ)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::PlaySound, DataPacket::emNormal, S2CPacketPlaySound)
_SERIALIZE_MEMBER(uint, m_soundType)
_SERIALIZE_MEMBER(i32, m_entityId)
_SERIALIZE_MEMBER(uint, m_channelId)
_END_PACKET


_BEGIN_PACKET_S2C(S2CPacketType::Explosion, DataPacket::emNormal, S2CPacketExplosion)
_SERIALIZE_MEMBER(real32, m_size)
_SERIALIZE_MEMBER(vector<PACKET_DETAIL::Vector3i>::type, m_poss)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, m_pos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, m_motion)
_SERIALIZE_MEMBER(real32, m_customsize)
_SERIALIZE_MEMBER(bool8, m_smoke)
_END_PACKET

//S2CPacketSetAllowFlying
_BEGIN_PACKET_S2C(S2CPacketType::SetAllowFlying, DataPacket::emNormal, S2CPacketSetAllowFlying)
_SERIALIZE_MEMBER(bool8, allowFlying)
_END_PACKET


_BEGIN_PACKET_S2C(S2CPacketType::SetSignTexts, DataPacket::emMapData, S2CPacketSetSignTexts)
public:
	struct SignData
	{
		PACKET_DETAIL::Vector3i position;
		std::array<String, 4> texts;
		bool isWordWrap;
		DEFINE_NETWORK_SERIALIZER(position, texts, isWordWrap);
	};
_SERIALIZE_MEMBER(vector<SignData>::type, signData)
_END_PACKET


_BEGIN_PACKET_S2C(S2CPacketType::ChangePlayerInfo, DataPacket::emNormal, S2CPacketChangePlayerInfo)
_SERIALIZE_MEMBER(i32, m_playerId)
_SERIALIZE_MEMBER(String, m_playerShowName)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::GameStatus, DataPacket::emNormal, S2CPacketGameStatus)
_SERIALIZE_MEMBER(i32, m_status)
_END_PACKET

//S2CPacketSetWatchMode
_BEGIN_PACKET_S2C(S2CPacketType::SetWatchMode, DataPacket::emNormal, S2CPacketSetWatchMode)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(bool8, isWatchMode)
_END_PACKET


//S2CPacketUpdateMerchant
_BEGIN_PACKET_S2C(S2CPacketType::UpdateMerchant, DataPacket::emNormal, S2CPacketUpdateMerchant)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(vector<BLOCKMAN::CommodityGroup>::type, commodities)
_END_PACKET


//S2CPacketSetFlying
_BEGIN_PACKET_S2C(S2CPacketType::SetFlying, DataPacket::emNormal, S2CPacketSetFlying)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(bool8, isFlying)
_END_PACKET

// S2CPacketPlayerSettlement
_BEGIN_PACKET_S2C(S2CPacketType::PlayerSettlement, DataPacket::emNormal, S2CPacketPlayerSettlement)
_SERIALIZE_MEMBER(String, m_result)
_SERIALIZE_MEMBER(bool8, m_isNextServer)
_END_PACKET

// S2CPacketGameSettlement
_BEGIN_PACKET_S2C(S2CPacketType::GameSettlement, DataPacket::emNormal, S2CPacketGameSettlement)
_SERIALIZE_MEMBER(String, m_result)
_SERIALIZE_MEMBER(bool8, m_isNextServer)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::PlayerLifeStatus, DataPacket::emNormal, S2CPacketPlayerLifeStatus)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_SERIALIZE_MEMBER(bool8, m_isLife)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncWorldWeather, DataPacket::emNormal, S2CPacketSyncWorldWeather)
_SERIALIZE_MEMBER(String, weather)
_SERIALIZE_MEMBER(i32, time)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::BedDestroy, DataPacket::emNormal, S2CPacketBedDestroy)
_SERIALIZE_MEMBER(i32, m_teamId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::CheckCSVersionResult, DataPacket::emNormal, S2CPacketCheckCSVersionResult)
_SERIALIZE_MEMBER(bool8, m_success)
_SERIALIZE_MEMBER(int, m_serverVersion)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::RespawnCountdown, DataPacket::emNormal, S2CPacketRespawnCountdown)
_SERIALIZE_MEMBER(i32, m_second)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::AllPlayerTeamInfo, DataPacket::emNormal, S2CPacketAllPlayerTeamInfo)
_SERIALIZE_MEMBER(String, m_result)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ChangePlayerTeam, DataPacket::emNormal, S2CPacketChangePlayerTeam)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_SERIALIZE_MEMBER(i32, m_teamId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::PlayerChangeActor, DataPacket::emNormal, S2CPacketPlayerChangeActor)
_SERIALIZE_MEMBER(int, m_entityId)
_SERIALIZE_MEMBER(String, m_actorName)
_SERIALIZE_MEMBER(String, m_bodyId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::PlayerRestoreActor, DataPacket::emNormal, S2CPacketPlayerRestoreActor)
_SERIALIZE_MEMBER(int, m_entityId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ReloadBullet, DataPacket::emNormal, S2CPacketReloadBullet)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::UpdateShop, DataPacket::emNormal, S2CPacketUpdateShop)
_SERIALIZE_MEMBER(vector<BLOCKMAN::GoodsGroup>::type, m_goods)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RespawnGoods>::type, m_respawnGoods)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::UpdateShopGoods, DataPacket::emNormal, S2CPacketUpdateShopGoods)
_SERIALIZE_MEMBER(i32, groupIndex)
_SERIALIZE_MEMBER(i32, goodsIndex)
_SERIALIZE_MEMBER(i32, limit)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ShoppingResult, DataPacket::emNormal, S2CPacketShoppingResult)
_SERIALIZE_MEMBER(i32, code)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::InitMoney, DataPacket::emNormal, S2CPacketInitMoney)
_SERIALIZE_MEMBER(i64, diamondBlues)
_SERIALIZE_MEMBER(i64, diamondGolds)
_SERIALIZE_MEMBER(i64, golds)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncGunFireResult, DataPacket::emNormal, S2CPacketSyncGunFireResult)
_SERIALIZE_MEMBER(ui32, sourceEntityId)					// where the gun fires comes from
_SERIALIZE_MEMBER(ui32, gunId)							// which type gun it is
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, pos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, dir)
_SERIALIZE_MEMBER(ui32, tracyType)						// 0: none  1: block  2: entity(Player)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3i, blockPos)	// if tracyType is 1, the blockPos is the collision Block!
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, hitpos)	// if tracyType is 1 or 2, the hitpos is the collision position!
_SERIALIZE_MEMBER(ui32, hitEntityID)					// if tracyType is 2, the hitEntityID is serverID
_SERIALIZE_MEMBER(bool, isHeadshot)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncTakeVehicle, DataPacket::emNormal, S2CPacketSyncTakeVehicle)
_SERIALIZE_MEMBER(ui32, playerId)
_SERIALIZE_MEMBER(ui32, vehicleId)
_SERIALIZE_MEMBER(bool, onoff)
_SERIALIZE_MEMBER(bool, isDriver)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ChangeMaxHealth, DataPacket::emNormal, S2CPacketChangeMaxHealth)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, health)
_SERIALIZE_MEMBER(bool, singleMaxHealth)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::UpdateCurrency, DataPacket::emNormal, S2CPacketUpdateCurrency)
_SERIALIZE_MEMBER(i64, currency)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SelectRoleData, DataPacket::emNormal, S2CPacketSelectRoleData)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerOwnVehicle, DataPacket::emNormal, S2CPacketSyncPlayerOwnVehicle)
_SERIALIZE_MEMBER(vector<int>::type, m_vehicles)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncLockVehicle, DataPacket::emNormal, S2CPacketSyncLockVehicle)
_SERIALIZE_MEMBER(ui32, playerId)
_SERIALIZE_MEMBER(bool8, onoff)
_SERIALIZE_MEMBER(ui32, vehicleId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncVehicleState, DataPacket::emNormal, S2CPacketSyncVehicleState)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, driverId)
_SERIALIZE_MEMBER(i32, state)
_SERIALIZE_MEMBER(real32, velocityX)
_SERIALIZE_MEMBER(real32, velocityY)
_SERIALIZE_MEMBER(real32, turningRadius)
_SERIALIZE_MEMBER(real32, throttle)
_SERIALIZE_MEMBER(vector<i32>::type, m_passengers)
_SERIALIZE_MEMBER(bool, isLocked)
_SERIALIZE_MEMBER(bool, hasDriver)
_SERIALIZE_MEMBER(bool, braking)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncVehicleAction, DataPacket::emNormal, S2CPacketSyncVehicleActionState)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, state)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncVehicleMovement, DataPacket::emNormal, S2CPacketSyncVehicleMovement)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(real32, velocityX)
_SERIALIZE_MEMBER(real32, velocityY)
_SERIALIZE_MEMBER(real32, yaw)
_SERIALIZE_MEMBER(real32, posX)
_SERIALIZE_MEMBER(real32, posY)
_SERIALIZE_MEMBER(real32, posZ)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncVehicleInput, DataPacket::emNormal, S2CPacketSyncVehicleInput)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(real32, turningRadius)
_SERIALIZE_MEMBER(real32, throttle)
_SERIALIZE_MEMBER(bool8, braking)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::NotifyGetMoney, DataPacket::emNormal, S2CPacketNotifyGetMoney)
_SERIALIZE_MEMBER(i32, count)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::NotifyGetItem, DataPacket::emNormal, S2CPacketNotifyGetItem)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, meta)
_SERIALIZE_MEMBER(i32, count)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::BuyCommodityResult, DataPacket::emNormal, S2CPacketBuyCommodityResult)
_SERIALIZE_MEMBER(i32, code)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ShowBuyRespawn, DataPacket::emNormal, S2CPacketShowBuyRespawn)
_SERIALIZE_MEMBER(i32, times)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ShowGoNpcMerchant, DataPacket::emNormal, S2CPacketShowGoNpcMerchant)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, x)
_SERIALIZE_MEMBER(float, y)
_SERIALIZE_MEMBER(float, z)
_SERIALIZE_MEMBER(float, yaw)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::UpdateBackpack, DataPacket::emNormal, S2CPacketUpdateBackpack)
_SERIALIZE_MEMBER(i32, capacity)
_SERIALIZE_MEMBER(i32, maxCapacity)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::RankData, DataPacket::emNormal, S2CPacketRankData)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

//S2CPacketActivateTrigger
_BEGIN_PACKET_S2C(S2CPacketType::ActivateTrigger, DataPacket::emNormal, S2CPacketActivateTrigger)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_SERIALIZE_MEMBER(i32, blockId)
_SERIALIZE_MEMBER(i32, triggerType)
_SERIALIZE_MEMBER(ui32, triggerIndex)
_END_PACKET

//S2CPacketUpdateRankData
_BEGIN_PACKET_S2C(S2CPacketType::UpdateRankNpc, DataPacket::emNormal, S2CPacketUpdateRankNpc)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(String, rankInfo)
_END_PACKET

//S2CPacketTakeAircraft
_BEGIN_PACKET_S2C(S2CPacketType::TakeAircraft, DataPacket::emNormal, S2CPacketTakeAircraft)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, passengerId)
_SERIALIZE_MEMBER(bool8, isOn)
_END_PACKET

//S2CPacketAircraftStartFly
_BEGIN_PACKET_S2C(S2CPacketType::AircraftStartFly, DataPacket::emNormal, S2CPacketAircraftStartFly)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, speed)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, startPos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, endPos)
_END_PACKET

//S2CPacketSyncAircraftState
_BEGIN_PACKET_S2C(S2CPacketType::SyncAircraftState, DataPacket::emNormal, S2CPacketSyncAircraftState)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, flyingTick)
_SERIALIZE_MEMBER(bool8, isFlying)
_SERIALIZE_MEMBER(float, speed)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, startPos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, endPos)
_SERIALIZE_MEMBER(vector<i32>::type, m_passengers)
_END_PACKET

//S2CPacketFriendOperation
_BEGIN_PACKET_S2C(S2CPacketType::FriendOperation, DataPacket::emNormal, S2CPacketFriendOperation)
_SERIALIZE_MEMBER(i32, operationType)
_SERIALIZE_MEMBER(ui64, userId)
_END_PACKET

//S2CPacketChangeAircraftUI
_BEGIN_PACKET_S2C(S2CPacketType::ChangeAircraftUI, DataPacket::emNormal, S2CPacketChangeAircraftUI)
_SERIALIZE_MEMBER(bool8, isShowUI)
_END_PACKET

//S2CPacketAirdrop
_BEGIN_PACKET_S2C(S2CPacketType::AirDorp, DataPacket::emNormal, S2CPacketAirdrop)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, m_airDropPos)
_END_PACKET

//S2CPacketPoisonCircleRange
_BEGIN_PACKET_S2C(S2CPacketType::PoisonCircleRange, DataPacket::emNormal, S2CPacketPoisonCircleRange)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, minSafeRange)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, maxSafeRange)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, minPoisonRange)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, maxPoisonRange)
_SERIALIZE_MEMBER(float, speed)
_END_PACKET

//S2CPacketMemberLeftAndKill
_BEGIN_PACKET_S2C(S2CPacketType::MemberLeftAndKill, DataPacket::emNormal, S2CPacketMemberLeftAndKill)
_SERIALIZE_MEMBER(i32, m_left)
_SERIALIZE_MEMBER(i32, m_kill)
_END_PACKET

//S2CPacketSyncBlockDestroyeFailure
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockDestroyeFailure, DataPacket::emNormal, S2CPacketSyncBlockDestroyeFailure)
_SERIALIZE_MEMBER(Bool, destroyeStatus)
_SERIALIZE_MEMBER(i32, destroyeFailureType)
_END_PACKET


//S2CPacketSkillType
_BEGIN_PACKET_S2C(S2CPacketType::SkillType, DataPacket::emNormal, S2CPacketSkillType)
_SERIALIZE_MEMBER(i32, skillType)
_SERIALIZE_MEMBER(float, duration)
_SERIALIZE_MEMBER(float, coldDownTime)
_END_PACKET


//S2CSyncChangePlayerActorInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncChangePlayerActorInfo, DataPacket::emNormal, S2CSyncChangePlayerActorInfo)
_SERIALIZE_MEMBER(int, changeActorCount)
_SERIALIZE_MEMBER(int, needMoneyCount)
_SERIALIZE_MEMBER(String, actorName)
_END_PACKET

//S2CSyncNpcActorInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncNpcActorInfo, DataPacket::emNormal, S2CSyncNpcActorInfo)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, headName)
_SERIALIZE_MEMBER(String, skillName)
_SERIALIZE_MEMBER(String, haloEffectName)
_SERIALIZE_MEMBER(String, content)
_SERIALIZE_MEMBER(bool, isCanObstruct)
_SERIALIZE_MEMBER(bool, isCanCollided)
_SERIALIZE_MEMBER(bool, isSuspended)
_SERIALIZE_MEMBER(float, scale)
_SERIALIZE_MEMBER(float, height)
_END_PACKET

//S2CPacketHideAndSeekBtnStatus
_BEGIN_PACKET_S2C(S2CPacketType::SyncHideAndSeekBtnStatus, DataPacket::emNormal, S2CPacketHideAndSeekBtnStatus)
_SERIALIZE_MEMBER(bool, isShowChangeActorBtn)
_SERIALIZE_MEMBER(bool, isShowChangeCameraBtn)
_SERIALIZE_MEMBER(bool, isShowOtherBtn)
_END_PACKET

//S2CPacketEntitySkillEffect
_BEGIN_PACKET_S2C(S2CPacketType::EntitySkillEffect, DataPacket::emNormal, S2CPacketEntitySkillEffect)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(float, duration)
_SERIALIZE_MEMBER(i32, width)
_SERIALIZE_MEMBER(i32, height)
_SERIALIZE_MEMBER(int, density)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, color)
_END_PACKET

//S2CPacketPickUpItemOrder
_BEGIN_PACKET_S2C(S2CPacketType::PickUpItemOrder, DataPacket::emNormal, S2CPacketPickUpItemOrder)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, pickUpPrice)
_SERIALIZE_MEMBER(i32, moneyType)
_SERIALIZE_MEMBER(i32, itemEntityId)
_END_PACKET

//S2CPacketRequestResult
_BEGIN_PACKET_S2C(S2CPacketType::RequestResult, DataPacket::emNormal, S2CPacketRequestResult)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, resultCode)
_SERIALIZE_MEMBER(String, resultMessage)
_END_PACKET

//S2CPacketSyncGameTimeShowUi
_BEGIN_PACKET_S2C(S2CPacketType::SyncGameTimeShowUi, DataPacket::emNormal, S2CPacketSyncGameTimeShowUi)
_SERIALIZE_MEMBER(int, time)
_SERIALIZE_MEMBER(bool, isShowUI)
_END_PACKET

//S2CPacketSyncCameraLock
_BEGIN_PACKET_S2C(S2CPacketType::SyncCameraLock, DataPacket::emNormal, S2CPacketSyncCameraLock)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isCameraLock)
_END_PACKET

//S2CPacketUpdateMerchantInfo
_BEGIN_PACKET_S2C(S2CPacketType::UpdateMerchantInfo, DataPacket::emNormal, S2CPacketUpdateMerchantInfo)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, name)
_END_PACKET

//S2CPacketUpdateMerchantCommodities
_BEGIN_PACKET_S2C(S2CPacketType::UpdateMerchantCommodities, DataPacket::emNormal, S2CPacketUpdateMerchantCommodities)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(vector<BLOCKMAN::CommodityGroup>::type, commodities)
_END_PACKET

// S2CPacketOpenCraftTable
_BEGIN_PACKET_S2C(S2CPacketType::OpenCraftTable, DataPacket::emNormal, S2CPacketOpenCraftTable)
_END_PACKET


// S2CPacketUpdateFurnaceTime
_BEGIN_PACKET_S2C(S2CPacketType::UpdateFurnaceTime, DataPacket::emNormal, S2CPacketUpdateFurnaceTime)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_SERIALIZE_MEMBER(i32, time)
_SERIALIZE_MEMBER(i32, maxTime)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SynSellItemSuc, DataPacket::emNormal, S2CPacketSynSellItemSuc)
_END_PACKET

#endif