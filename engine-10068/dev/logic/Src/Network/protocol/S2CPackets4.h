#pragma once
#ifndef __S2C_PACKETS4_H__
#define __S2C_PACKETS4_H__

#include "Network/protocol/PacketDefine.h"
#include "Network/NetworkDefine.h"
#include "Setting/LogicSetting.h"
#include "Game/Ranch.h"
#include "Game/BirdSimulator.h"
#include "Entity/Entity.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityBulletin.h"

using namespace LORD;

// S2CPacketSuperShopTip
_BEGIN_PACKET_S2C(S2CPacketType::SuperShopTip, DataPacket::emNormal, S2CPacketSuperShopTip)
_SERIALIZE_MEMBER(String, tip)
_END_PACKET

//S2CPacketEnterOtherGame
_BEGIN_PACKET_S2C(S2CPacketType::EnterOtherGame, DataPacket::emNormal, S2CPacketEnterOtherGame)
_SERIALIZE_MEMBER(String, gameType)
_SERIALIZE_MEMBER(ui64, targetId)
_SERIALIZE_MEMBER(String, mapId)
_END_PACKET

//S2CPacketKillMsg
_BEGIN_PACKET_S2C(S2CPacketType::KillMsg, DataPacket::emNormal, S2CPacketKillMsg)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketShowPersonalShop
_BEGIN_PACKET_S2C(S2CPacketType::ShowPersonalShop, DataPacket::emNormal, S2CPacketShowPersonalShop)
_END_PACKET

//S2CPacketPersonalShopArea
_BEGIN_PACKET_S2C(S2CPacketType::PersonalShopArea, DataPacket::emNormal, S2CPacketPersonalShopArea)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, startPos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, endPos)
_END_PACKET

//S2CPacketAddGunBulletNum
_BEGIN_PACKET_S2C(S2CPacketType::AddGunBulletNum, DataPacket::emNormal, S2CPacketAddGunBulletNum)
_SERIALIZE_MEMBER(int, gunId)
_SERIALIZE_MEMBER(int, bulletNum)
_END_PACKET

//S2CPacketSubGunRecoil
_BEGIN_PACKET_S2C(S2CPacketType::SubGunRecoil, DataPacket::emNormal, S2CPacketSubGunRecoil)
_SERIALIZE_MEMBER(int, gunId)
_SERIALIZE_MEMBER(float, recoil)
_END_PACKET

// S2CPacketCustomPropsData
_BEGIN_PACKET_S2C(S2CPacketType::CustomPropsData, DataPacket::emNormal, S2CPacketCustomPropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// S2CPacketUpdateCustomProps
_BEGIN_PACKET_S2C(S2CPacketType::UpdateCustomProps, DataPacket::emNormal, S2CPacketUpdateCustomProps)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketEntityTeleportWithMotion
_BEGIN_PACKET_S2C(S2CPacketType::EntityPlayerTeleportWithMotion, DataPacket::emNormal, S2CPacketEntityPlayerTeleportWithMotion)
public:
	S2CPacket(BLOCKMAN::Entity* pEntity)
	{
		m_entityId = pEntity->entityId;
		m_x = pEntity->position.x;
		m_y = pEntity->position.y;
		m_z = pEntity->position.z;
		m_mx = pEntity->motion.x;
		m_my = pEntity->motion.y;
		m_mz = pEntity->motion.z;
		m_isOnGround = pEntity->onGround;
	}
_SERIALIZE_MEMBER(i32, m_entityId)
_SERIALIZE_MEMBER(real32, m_x)
_SERIALIZE_MEMBER(real32, m_y)
_SERIALIZE_MEMBER(real32, m_z)
_SERIALIZE_MEMBER(real32, m_mx)
_SERIALIZE_MEMBER(real32, m_my)
_SERIALIZE_MEMBER(real32, m_mz)
_SERIALIZE_MEMBER(bool, m_isOnGround)
_END_PACKET

// S2CPacketSyncCannonFire
_BEGIN_PACKET_S2C(S2CPacketType::SyncCannonFire, DataPacket::emNormal, S2CPacketSyncCannonFire)
_SERIALIZE_MEMBER(i32, cannonEntityId)
_END_PACKET

//S2CPacketSyncRanchInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchInfo, DataPacket::emNormal, S2CPacketSyncRanchInfo)
_SERIALIZE_MEMBER(BLOCKMAN::RanchInfo, m_info)
_END_PACKET

//S2CPacketSyncRanchHouse
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchHouse, DataPacket::emNormal, S2CPacketSyncRanchHouse)
_SERIALIZE_MEMBER(BLOCKMAN::RanchHouse, m_house)
_END_PACKET

//S2CPacketSyncRanchStorage
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchStorage, DataPacket::emNormal, S2CPacketSyncRanchStorage)
_SERIALIZE_MEMBER(BLOCKMAN::RanchStorage, m_storage)
_END_PACKET

//S2CPacketSyncRanchRank
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchRank, DataPacket::emNormal, S2CPacketSyncRanchRank)
_SERIALIZE_MEMBER(i32, m_rankType)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchRank>::type, m_ranks)
_END_PACKET

//S2CPacketSyncRanchBuild
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchBuild, DataPacket::emNormal, S2CPacketSyncRanchBuild)
_SERIALIZE_MEMBER(i32, m_buildType)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchBuild>::type, m_builds)
_END_PACKET

//S2CPacketSyncEntityBuildNpc
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityBuildNpc, DataPacket::emNormal, S2CPacketSyncEntityBuildNpc)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, m_actorId)
_SERIALIZE_MEMBER(ui64, m_userId)

_SERIALIZE_MEMBER(i32, m_productCapacity)
_SERIALIZE_MEMBER(i32, m_maxQueueNum)
_SERIALIZE_MEMBER(i32, m_queueUnlockPrice)
_SERIALIZE_MEMBER(i32, m_queueUnlockCurrencyType)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ProductRecipe>::type, m_recipes)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchCommon>::type, m_products)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ProductQueue>::type, m_unlockQueues)
_END_PACKET

//S2CPacketSyncEntityLandNpc
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityLandNpc, DataPacket::emNormal, S2CPacketSyncEntityLandNpc)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(ui64, userId)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, actorBody)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, actorBodyId)
_SERIALIZE_MEMBER(i32, status)
_SERIALIZE_MEMBER(i32, timeLeft)
_SERIALIZE_MEMBER(i32, landCode)
_SERIALIZE_MEMBER(i32, price)
_SERIALIZE_MEMBER(i32, totalTime)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchCommon>::type, recipe)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchCommon>::type, reward)
_END_PACKET


//S2CPacketRanchGain
_BEGIN_PACKET_S2C(S2CPacketType::RanchGain, DataPacket::emNormal, S2CPacketRanchGain)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchCommon>::type, items)
_END_PACKET

//S2CPacketSyncRanchOrder
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchOrder, DataPacket::emNormal, S2CPacketSyncRanchOrder)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchOrder>::type, m_orders)
_END_PACKET

//S2CPacketSyncRanchAchievement
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchAchievement, DataPacket::emNormal, S2CPacketSyncRanchAchievement)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchAchievement>::type, m_achievements)
_END_PACKET

//S2CPacketSyncRanchTimePrice
_BEGIN_PACKET_S2C(S2CPacketType::RanchTimePrice, DataPacket::emNormal, S2CPacketSyncRanchTimePrice)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchTimePrice>::type, m_timePrices)
_END_PACKET

//S2CPacketSyncRanchShortcutFreeTimes
_BEGIN_PACKET_S2C(S2CPacketType::RanchShortcutFreeTimes, DataPacket::emNormal, S2CPacketSyncRanchShortcutFreeTimes)
_SERIALIZE_MEMBER(vector<vector<i32>::type>::type , m_shortcutFreeTimes)
_END_PACKET


//S2CPacketBroadcastMessage
_BEGIN_PACKET_S2C(S2CPacketType::BroadcastMessage, DataPacket::emNormal, S2CPacketBroadcastMessage)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(String, content)
_END_PACKET


//S2CPacketRanchUnlockItem
_BEGIN_PACKET_S2C(S2CPacketType::RanchUnlockItem, DataPacket::emNormal, S2CPacketRanchUnlockItem)
_SERIALIZE_MEMBER(vector<i32>::type, items)
_END_PACKET

//S2CPacketCropInfo
_BEGIN_PACKET_S2C(S2CPacketType::CropInfo, DataPacket::emNormal, S2CPacketCropInfo)
_SERIALIZE_MEMBER(Vector3i, blockPos)
_SERIALIZE_MEMBER(i32, blockId)
_SERIALIZE_MEMBER(i32, stage)
_SERIALIZE_MEMBER(i32, curStageTime)
_SERIALIZE_MEMBER(i32, residueHarvestNum)
_END_PACKET

//S2CPacketCropInfo
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuyKeepItemTip, DataPacket::emNormal, S2CPacketShowBuyKeepItemTip)
_SERIALIZE_MEMBER(int, coinType)
_SERIALIZE_MEMBER(int, coin)
_SERIALIZE_MEMBER(int, tipTime)
_END_PACKET

//S2CPacketSendOpenEnchantment
_BEGIN_PACKET_S2C(S2CPacketType::SendOpenEnchantment, DataPacket::emNormal, S2CPacketSendOpenEnchantment)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketAppExpResult
_BEGIN_PACKET_S2C(S2CPacketType::AppExpResult, DataPacket::emNormal, S2CPacketAppExpResult)
_SERIALIZE_MEMBER(i32, curLv)
_SERIALIZE_MEMBER(i32, toLv)
_SERIALIZE_MEMBER(i32, addExp)
_SERIALIZE_MEMBER(i32, curExp)
_SERIALIZE_MEMBER(i32, toExp)
_SERIALIZE_MEMBER(i32, upExp)
_SERIALIZE_MEMBER(i32, status)
_END_PACKET

//S2CPacketOccupationUnlock
_BEGIN_PACKET_S2C(S2CPacketType::OccupationUnlock, DataPacket::emNormal, S2CPacketOccupationUnlock)
_SERIALIZE_MEMBER(String, occupationInfo)
_END_PACKET

//S2CPacketBuyFlying
_BEGIN_PACKET_S2C(S2CPacketType::BuyFlying, DataPacket::emNormal, S2CPacketBuyFlying)
_END_PACKET

//S2CPacketPlayCloseup
_BEGIN_PACKET_S2C(S2CPacketType::PlayCloseup, DataPacket::emNormal, S2CPacketPlayCloseup)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, playerPos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, closeupPos)
_SERIALIZE_MEMBER(float, farDistance)
_SERIALIZE_MEMBER(float, nearDistance)
_SERIALIZE_MEMBER(float, velocity)
_SERIALIZE_MEMBER(float, duration)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(float, pitch)
_END_PACKET

//S2CPacketSubGunFireCd
_BEGIN_PACKET_S2C(S2CPacketType::SubGunFireCd, DataPacket::emNormal, S2CPacketSubGunFireCd)
_SERIALIZE_MEMBER(int, gunId)
_SERIALIZE_MEMBER(float, fireCd)
_END_PACKET

//S2CPacketEntityHurtTime
_BEGIN_PACKET_S2C(S2CPacketType::EntityHurtTime, DataPacket::emNormal, S2CPacketEntityHurtTime)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, time)
_END_PACKET

//S2CPacketLotteryData
_BEGIN_PACKET_S2C(S2CPacketType::LotteryData, DataPacket::emNormal, S2CPacketLotteryData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketLotteryResult
_BEGIN_PACKET_S2C(S2CPacketType::LotteryResult, DataPacket::emNormal, S2CPacketLotteryResult)
_SERIALIZE_MEMBER(String, firstId)
_SERIALIZE_MEMBER(String, secondId)
_SERIALIZE_MEMBER(String, thirdId)
_END_PACKET

//S2CPacketHideAndSeekHallResult
_BEGIN_PACKET_S2C(S2CPacketType::HideAndSeekHallResult, DataPacket::emNormal, S2CPacketHideAndSeekHallResult)
_SERIALIZE_MEMBER(String, itemInfo)
_END_PACKET

//S2CPacketEntityColorfulTime
_BEGIN_PACKET_S2C(S2CPacketType::EntityColorfulTime, DataPacket::emNormal, S2CPacketEntityColorfulTime)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, time)
_END_PACKET

//S2CPacketEntitySelected
_BEGIN_PACKET_S2C(S2CPacketType::EntitySelected, DataPacket::emNormal, S2CPacketEntitySelected)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(int, type)
_END_PACKET


//S2CPacketEntityDisableMoveTime
_BEGIN_PACKET_S2C(S2CPacketType::EntityDisableMoveTime, DataPacket::emNormal, S2CPacketEntityDisableMoveTime)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, time)
_END_PACKET

//S2CPacketLogicSettingInfo
_BEGIN_PACKET_S2C(S2CPacketType::LogicSettingInfo, DataPacket::emNormal, S2CPacketLogicSettingInfo)
_SERIALIZE_MEMBER(i32, m_mainInventorySize)
_SERIALIZE_MEMBER(i32, m_enderInventorySize)
_SERIALIZE_MEMBER(vector<BLOCKMAN::GunPluginSetting>::type, m_gunSetting)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BulletClipPluginSetting>::type, m_bulletClipSetting)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockDynamicAttr>::type, m_blockDynamicAttr)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ToolItemDynamicAttr>::type, m_toolItemDynamicAttr)
_SERIALIZE_MEMBER(float, m_thirdPersonDistance)
_SERIALIZE_MEMBER(int, m_quicklyBuildBlockCost)
_SERIALIZE_MEMBER(int, m_quicklyBuildBlockNum)
_SERIALIZE_MEMBER(float, m_renderYawThreshold)
_SERIALIZE_MEMBER(float, m_renderYawScale)
_SERIALIZE_MEMBER(String, m_deathAnimEffect)
_SERIALIZE_MEMBER(int, m_max_changes_in_on_chunk)
_SERIALIZE_MEMBER(vector<BLOCKMAN::SkillItem>::type, m_skillItemSetting)
_SERIALIZE_MEMBER(float, m_parachuteDownSpeed)
_SERIALIZE_MEMBER(float, m_parachuteMoveSpeed)
_SERIALIZE_MEMBER(int, m_gameWeatherType)
_SERIALIZE_MEMBER(bool, m_sneakShowName)
_SERIALIZE_MEMBER(bool, m_breakBlockSoon)
_SERIALIZE_MEMBER(bool, m_isCanDamageItem)
_SERIALIZE_MEMBER(bool, m_disableSelectEntity)
_SERIALIZE_MEMBER(bool, m_canEatSetting)
_SERIALIZE_MEMBER(bool, m_gunIsNeedBullet)
_SERIALIZE_MEMBER(bool, m_hideClouds)
_SERIALIZE_MEMBER(bool, m_canCloseChest)
_SERIALIZE_MEMBER(bool, m_showGunEffectWithSingle)
_SERIALIZE_MEMBER(bool, m_allowHeadshot)
_SERIALIZE_MEMBER(bool, m_swordCanBreakBlock)
_SERIALIZE_MEMBER(bool, m_need_durable)
_SERIALIZE_MEMBER(bool, m_isUseMovingEnderChest)
_SERIALIZE_MEMBER(bool, m_hideArmor)
_SERIALIZE_MEMBER(bool, m_isSeparateCameraControl)
_SERIALIZE_MEMBER(bool, m_useLocalScaleInHand)
_END_PACKET

//S2CPacketSyncEffectGunFire
_BEGIN_PACKET_S2C(S2CPacketType::SyncEffectGunFire, DataPacket::emNormal, S2CPacketSyncEffectGunFire)
_SERIALIZE_MEMBER(ui32, shootingEntityId)
_SERIALIZE_MEMBER(ui32, gunId)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, endPos)
_SERIALIZE_MEMBER(bool, hasBlockTarget)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3i, blockTargetPos)
_END_PACKET

//S2CPacketSyncEffectGunFireAtEntityResult
_BEGIN_PACKET_S2C(S2CPacketType::SyncEffectGunFireAtEntityResult, DataPacket::emNormal, S2CPacketSyncEffectGunFireAtEntityResult)
_SERIALIZE_MEMBER(ui32, shootingEntityId)
_SERIALIZE_MEMBER(ui32, hitEntityId)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, dir)
_SERIALIZE_MEMBER(ui32, traceType)
_SERIALIZE_MEMBER(bool, isHeadshot)
_SERIALIZE_MEMBER(int, gunId)
_END_PACKET

//S2CPacketSyncEffectGunFireAtBlockResult
_BEGIN_PACKET_S2C(S2CPacketType::SyncEffectGunFireAtBlockResult, DataPacket::emNormal, S2CPacketSyncEffectGunFireAtBlockResult)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3i, blockPos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, dir)
_SERIALIZE_MEMBER(bool, isRed)
_SERIALIZE_MEMBER(int, gunId)
_END_PACKET

//S2CPacketSyncEffectGunFireExplosionResult
_BEGIN_PACKET_S2C(S2CPacketType::SyncEffectGunFireExplosionResult, DataPacket::emNormal, S2CPacketSyncEffectGunFireExplosionResult)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(ui32, gunId)
_END_PACKET

//S2CPacketStopLaserGun
_BEGIN_PACKET_S2C(S2CPacketType::StopLaserGun, DataPacket::emNormal, S2CPacketStopLaserGun)
_SERIALIZE_MEMBER(i32, shootingEntityId)
_END_PACKET

//S2CPacketShowSkyBlockInfo
_BEGIN_PACKET_S2C(S2CPacketType::ShowSkyBlockInfo, DataPacket::emNormal, S2CPacketShowSkyBlockInfo)
_SERIALIZE_MEMBER(int, lv)
_SERIALIZE_MEMBER(i64, cur_exp)
_SERIALIZE_MEMBER(i64, max_exp)
_SERIALIZE_MEMBER(int, island_lv)
_SERIALIZE_MEMBER(String, area)
_SERIALIZE_MEMBER(bool, is_max)
_SERIALIZE_MEMBER(bool, isMaster)
_END_PACKET

//S2CPacketEnableAutoShoot
_BEGIN_PACKET_S2C(S2CPacketType::EnableAutoShoot, DataPacket::emNormal, S2CPacketEnableAutoShoot)
_SERIALIZE_MEMBER(bool, enable)
_END_PACKET

//S2CPacketDisarmament
_BEGIN_PACKET_S2C(S2CPacketType::Disarmament, DataPacket::emNormal, S2CPacketDisarmament)
_SERIALIZE_MEMBER(bool, isDisarmament)
_END_PACKET

//S2CPacketSetPlayerOpacity
_BEGIN_PACKET_S2C(S2CPacketType::SetPlayerOpacity, DataPacket::emNormal, S2CPacketSetPlayerOpacity)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, opacityValue)
_END_PACKET

//S2CPacketNotifyGetGoods
_BEGIN_PACKET_S2C(S2CPacketType::NotifyGetGoods, DataPacket::emNormal, S2CPacketNotifyGetGoods)
_SERIALIZE_MEMBER(String, icon)
_SERIALIZE_MEMBER(i32, count)
_END_PACKET

//S2CPacketPlayerNamePerspective
_BEGIN_PACKET_S2C(S2CPacketType::PlayerNamePerspective, DataPacket::emNormal, S2CPacketPlayerNamePerspective)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isPerspective)
_END_PACKET

//S2CPacketAddEntityFeature
_BEGIN_PACKET_S2C(S2CPacketType::AddEntityFeature, DataPacket::emNormal, S2CPacketAddEntityFeature)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(String, description)
_SERIALIZE_MEMBER(float, duration)
_SERIALIZE_MEMBER(float, scale)
_SERIALIZE_MEMBER(int, type)
_END_PACKET

//S2CPacketSyncEntityFeature
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityFeature, DataPacket::emNormal, S2CPacketSyncEntityFeature)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(vector<BLOCKMAN::EntityFeature>::type, entityFeatures)
_END_PACKET

//S2CPacketSyncEntityBulletin
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityBulletin, DataPacket::emNormal, S2CPacketSyncEntityBulletin)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, bulletinId)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(BLOCKMAN::BulletinRank, userRank)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BulletinRank>::type, ranks)
_END_PACKET

//S2CPacketSyncEntityBird
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityBird, DataPacket::emNormal, S2CPacketSyncEntityBird)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(ui64, userId)
_SERIALIZE_MEMBER(i64, birdId)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, actorBody)
_SERIALIZE_MEMBER(String, actorBodyId)
_SERIALIZE_MEMBER(String, dressGlasses)
_SERIALIZE_MEMBER(String, dressHat)
_SERIALIZE_MEMBER(String, dressBeak)
_SERIALIZE_MEMBER(String, dressWing)
_SERIALIZE_MEMBER(String, dressTail)
_SERIALIZE_MEMBER(String, dressEffect)
_END_PACKET

//S2CPacketSyncPlayerBagInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerBagInfo, DataPacket::emNormal, S2CPacketSyncPlayerBagInfo)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isConvert)
_SERIALIZE_MEMBER(i32, curCapacity)
_SERIALIZE_MEMBER(i32, maxCapacity)
_END_PACKET

//S2CPacketSyncBirdStore
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdStore, DataPacket::emNormal, S2CPacketSyncBirdStore)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdStore>::type, stores)
_END_PACKET

//S2CPacketSyncBirdStore
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdPersonalStore, DataPacket::emNormal, S2CPacketSyncBirdPersonalStore)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdPersonalStoreTab>::type, personalStore)
_END_PACKET

//S2CPacketSyncBag
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdBag, DataPacket::emNormal, S2CPacketSyncBirdBag)
_SERIALIZE_MEMBER(BLOCKMAN::BirdBag, birdBag)
_END_PACKET

//S2CPacketSyncBirdPlayerInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdPlayerInfo, DataPacket::emNormal, S2CPacketSyncBirdPlayerInfo)
_SERIALIZE_MEMBER(BLOCKMAN::BirdPlayerInfo, playerInfo)
_END_PACKET

//S2CPacketSyncBirdDress
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdDress, DataPacket::emNormal, S2CPacketSyncBirdDress)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdDress>::type, dress)
_END_PACKET

//S2CPacketSyncBirdFood
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdFood, DataPacket::emNormal, S2CPacketSyncBirdFood)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdFood>::type, foods)
_END_PACKET

//S2CPacketSyncBirdTask
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdTask, DataPacket::emNormal, S2CPacketSyncBirdTask)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdTask>::type, tasks)
_END_PACKET

//S2CPacketSyncBirdAtlas
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdAtlas, DataPacket::emNormal, S2CPacketSyncBirdAtlas)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdAtlas>::type, atlas)
_END_PACKET

//S2CPacketBirdGain
_BEGIN_PACKET_S2C(S2CPacketType::BirdGain, DataPacket::emNormal, S2CPacketBirdGain)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdGain>::type, items)
_END_PACKET

//S2CPacketBirdNestOperation
_BEGIN_PACKET_S2C(S2CPacketType::BirdNestOperation, DataPacket::emNormal, S2CPacketBirdNestOperation)
_SERIALIZE_MEMBER(i64, birdId)
_SERIALIZE_MEMBER(bool, isUnlock)
_END_PACKET

//S2CPacketSyncBirdScope
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdScope, DataPacket::emNormal, S2CPacketSyncBirdScope)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdScope>::type, scopes)
_END_PACKET

//S2CPacketSyncBirdActivity
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdActivity, DataPacket::emNormal, S2CPacketSyncBirdActivity)
_SERIALIZE_MEMBER(BLOCKMAN::BirdGiftBag, activity)
_END_PACKET


//S2CPacketBirdLotteryResult
_BEGIN_PACKET_S2C(S2CPacketType::BirdLotteryResult, DataPacket::emNormal, S2CPacketBirdLotteryResult)
_SERIALIZE_MEMBER(String, eggBodyId)
_SERIALIZE_MEMBER(BLOCKMAN::BirdInfo, birdInfo)
_END_PACKET


//S2CPacketSyncEntityAction
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityAction, DataPacket::emNormal, S2CPacketSyncEntityAction)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(int, entityClass)
_SERIALIZE_MEMBER(int, actionStatus)
_SERIALIZE_MEMBER(int, baseAction)
_SERIALIZE_MEMBER(int, preBaseAction)
_END_PACKET

//S2CPacketSyncBirdTimePrice
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdTimePrice, DataPacket::emNormal, S2CPacketSyncBirdTimePrice)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdTimePrice>::type, timePrices)
_END_PACKET

//S2CPacketSyncBirdAddScore
_BEGIN_PACKET_S2C(S2CPacketType::BirdAddScore, DataPacket::emNormal, S2CPacketBirdAddScore)
_SERIALIZE_MEMBER(i32, score)
_SERIALIZE_MEMBER(i32, scoreType)
_END_PACKET

//S2CPacketSyncBirdBuff
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdBuff, DataPacket::emNormal, S2CPacketSyncBirdBuff)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdBuff>::type, buffs)
_END_PACKET

//S2CPacketSyncEntityHealth
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityHealth, DataPacket::emNormal, S2CPacketSyncEntityHealth)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, health)
_SERIALIZE_MEMBER(float, maxHealth)
_END_PACKET



#endif