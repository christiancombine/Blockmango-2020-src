#pragma once
#ifndef __S2C_PACKETS1_H__
#define __S2C_PACKETS1_H__

#include "Network/protocol/PacketDefine.h"
#include "Network/NetworkDefine.h"
#include "Item/Potion.h"
#include "Item/Item.h"
#include "Item/ItemStack.h"
#include "Entity/Entity.h"
#include "Entity/EntityXPOrb.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntityCreature.h"
#include "Entity/EntityActorCannon.h"

using namespace LORD;

// S2CPacketDebugString
_BEGIN_PACKET_S2C(S2CPacketType::DebugString, DataPacket::emNormal, S2CPacketDebugString)
_SERIALIZE_MEMBER(String, debugString)
_END_PACKET

//S2CPacketUserAttr
_BEGIN_PACKET_S2C(S2CPacketType::UserAttr, DataPacket::emNormal, S2CPacketUserAttr)

public:
	struct SkinColor
	{
		Real r=0, g=0, b=0, a=0;
		SkinColor() {
		}
		SkinColor(Real fr, Real fg, Real fb, Real fa = 0.f)
		{
			r = fr;
			g = fg;
			b = fb;
			a = fa;
		}
		DEFINE_NETWORK_SERIALIZER(r, g, b, a);
	};
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, faceId)
_SERIALIZE_MEMBER(i32, hairId)
_SERIALIZE_MEMBER(i32, topsId)
_SERIALIZE_MEMBER(i32, pantsId)
_SERIALIZE_MEMBER(i32, shoesId)
_SERIALIZE_MEMBER(i32, glassesId)
_SERIALIZE_MEMBER(i32, scarfId)
_SERIALIZE_MEMBER(i32, wingId)
_SERIALIZE_MEMBER(i32, hatId)
_SERIALIZE_MEMBER(i32, decoratehatId)
_SERIALIZE_MEMBER(i32, armId)
_SERIALIZE_MEMBER(i32, tailId)
_SERIALIZE_MEMBER(i32, extrawingId)
_SERIALIZE_MEMBER(i32, footHaloId)
_SERIALIZE_MEMBER(i32, backEffectId)
_SERIALIZE_MEMBER(i32, crownId)
_SERIALIZE_MEMBER(i32, bagId)
_SERIALIZE_MEMBER(i32, headEffectId)
_SERIALIZE_MEMBER(SkinColor, skinColor)
_SERIALIZE_MEMBER(i32, defaultIdle)
_SERIALIZE_MEMBER(vector<i32>::type, selectableActions)
_SERIALIZE_MEMBER(vector<i32>::type, suitsId)
_SERIALIZE_MEMBER(real32, scale)
_SERIALIZE_MEMBER(i32, holdItemId)
_SERIALIZE_MEMBER(String, holdItemActor)
_SERIALIZE_MEMBER(String, commonData)
_SERIALIZE_MEMBER(i32, privilegeId)
_END_PACKET

//S2CPacketEntityHeadRotation
_BEGIN_PACKET_S2C(S2CPacketType::EntityHeadRotation, DataPacket::emNormal, S2CPacketEntityHeadRotation)
_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(real32, m_headYaw)
_END_PACKET

//S2CPacketEntityMovement
_BEGIN_PACKET_S2C(S2CPacketType::EntityMovement, DataPacket::emNormal, S2CPacketEntityMovement)
public:
	S2CPacket(int etyid)
		: m_entityId(etyid)
		, m_x(0)
		, m_y(0)
		, m_z(0)
		, m_yaw(0.0)
		, m_pitch(0.0)
		, m_moving(false)
		, m_rotating(false)
	{}

	S2CPacket(int etyid, i16 x, i16 y, i16 z)
		: m_entityId(etyid)
		, m_x(x)
		, m_y(y)
		, m_z(z)
		, m_yaw(0.0)
		, m_pitch(0.0)
		, m_moving(true)
		, m_rotating(false)
	{}

	S2CPacket(int etyid, real32 yaw, real32 pitch)
		: m_entityId(etyid)
		, m_x(0)
		, m_y(0)
		, m_z(0)
		, m_yaw(yaw)
		, m_pitch(pitch)
		, m_moving(false)
		, m_rotating(true)
	{}

	S2CPacket(int etyid, i16 x, i16 y, i16 z, real32 yaw, real32 pitch)
		: m_entityId(etyid)
		, m_x(x)
		, m_y(y)
		, m_z(z)
		, m_yaw(yaw)
		, m_pitch(pitch)
		, m_moving(true)
		, m_rotating(true)
	{}
_SERIALIZE_MEMBER(i32, m_entityId)
_SERIALIZE_MEMBER(i16, m_x)
_SERIALIZE_MEMBER(i16, m_y)
_SERIALIZE_MEMBER(i16, m_z)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(bool, m_moving)
_SERIALIZE_MEMBER(bool, m_rotating)
_END_PACKET

//S2CPacketEntityTeleport
_BEGIN_PACKET_S2C(S2CPacketType::EntityTeleport, DataPacket::emNormal, S2CPacketEntityTeleport)
public:
	S2CPacket(BLOCKMAN::Entity* pEntity)
	{
		m_entityId = pEntity->entityId;
		m_x = int(Math::Floor(pEntity->position.x * 32.0f));
		m_y = int(Math::Floor(pEntity->position.y * 32.0f));
		m_z = int(Math::Floor(pEntity->position.z * 32.0f));
		// m_yaw = (i16)((int)(pEntity->rotationYaw * 256.0F / 360.0F));
		// m_pitch = (i16)((int)(pEntity->rotationPitch * 256.0F / 360.0F));
		m_yaw = pEntity->rotationYaw;
		m_pitch = pEntity->rotationPitch;
	}

	S2CPacket(int eid, int x, int y, int z, real32 yaw, real32 pitch)
		: m_entityId(eid)
		, m_x(x)
		, m_y(y)
		, m_z(z)
		, m_yaw(yaw)
		, m_pitch(pitch)
	{}
_SERIALIZE_MEMBER(i32, m_entityId)
_SERIALIZE_MEMBER(i32, m_x)
_SERIALIZE_MEMBER(i32, m_y)
_SERIALIZE_MEMBER(i32, m_z)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(real32, m_pitch)
_END_PACKET

//S2CPacketEntityVelocity
_BEGIN_PACKET_S2C(S2CPacketType::EntityVelocity, DataPacket::emNormal, S2CPacketEntityVelocity)
public:
	S2CPacket(BLOCKMAN::Entity* pEntity)
	{
		m_entityID = pEntity->entityId;
		setVolocity(pEntity->motion.x, pEntity->motion.y, pEntity->motion.z);
	}

	S2CPacket(int id, float x, float y, float z)
	{
		m_entityID = id;
		setVolocity(x, y, z);
	}

	void setVolocity(float x, float y, float z)
	{
		float bound = 3.9f;

		x = Math::Clamp(x, -bound, bound);
		y = Math::Clamp(y, -bound, bound);
		z = Math::Clamp(z, -bound, bound);

		m_motionX = (int)(x * 8000.0f);
		m_motionY = (int)(y * 8000.0f);
		m_motionZ = (int)(z * 8000.0f);
	}

_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(i32, m_motionX)
_SERIALIZE_MEMBER(i32, m_motionY)
_SERIALIZE_MEMBER(i32, m_motionZ)
_END_PACKET


//S2CPacketSpawnExpOrb
_BEGIN_PACKET_S2C(S2CPacketType::SpawnExpOrb, DataPacket::emNormal, S2CPacketSpawnExpOrb)
public:
	S2CPacket(BLOCKMAN::EntityXPOrb* pXpOrb)
	{
		m_entityID = pXpOrb->entityId;
		m_posX = int(Math::Floor(pXpOrb->position.x * 32.f));
		m_posY = int(Math::Floor(pXpOrb->position.y * 32.f));
		m_posZ = int(Math::Floor(pXpOrb->position.z * 32.f));
		m_xpValue = pXpOrb->getXpValue();
	}

	S2CPacket(int entityID, int x, int y, int z, int exp)
		: m_entityID(entityID)
		, m_posX(x)
		, m_posY(y)
		, m_posZ(z)
		, m_xpValue(exp)
	{}

_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(i32, m_posX)
_SERIALIZE_MEMBER(i32, m_posY)
_SERIALIZE_MEMBER(i32, m_posZ)
_SERIALIZE_MEMBER(i32, m_xpValue)
_END_PACKET

//S2CPacketSpawnMob
_BEGIN_PACKET_S2C(S2CPacketType::SpawnMob, DataPacket::emNormal, S2CPacketSpawnMob)
public:
	S2CPacket(BLOCKMAN::EntityLivingBase* pLiving)
	{
		m_entityID = pLiving->entityId;
		m_type = pLiving->getClassID();
		m_posX = int(Math::Floor(pLiving->position.x * 32.f));
		m_posY = int(Math::Floor(pLiving->position.y * 32.f));
		m_posZ = int(Math::Floor(pLiving->position.z * 32.f));
		m_yaw = (i16)((int)(pLiving->rotationYaw * 256.f / 360.f));
		m_pitch = (i16)((int)(pLiving->rotationPitch * 256.f / 360.f));
		m_headYaw = (i16)((int)(pLiving->rotationYawHead * 256.f / 360.f));

		float bound = 3.9f;
		Vector3 motion = pLiving->motion;
		motion.x = Math::Clamp(motion.x, -bound, bound);
		motion.y = Math::Clamp(motion.y, -bound, bound);
		motion.z = Math::Clamp(motion.z, -bound, bound);

		m_motionX = (int)(motion.x * 8000.f);
		m_motionY = (int)(motion.y * 8000.f);
		m_motionZ = (int)(motion.z * 8000.f);
	}
_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(i32, m_type)
_SERIALIZE_MEMBER(i32, m_posX)
_SERIALIZE_MEMBER(i32, m_posY)
_SERIALIZE_MEMBER(i32, m_posZ)
_SERIALIZE_MEMBER(i32, m_motionX)
_SERIALIZE_MEMBER(i32, m_motionY)
_SERIALIZE_MEMBER(i32, m_motionZ)
_SERIALIZE_MEMBER(i16, m_yaw)
_SERIALIZE_MEMBER(i16, m_pitch)
_SERIALIZE_MEMBER(i16, m_headYaw)
_END_PACKET

//S2CPacketSpawnPlayer
_BEGIN_PACKET_S2C(S2CPacketType::SpawnPlayer, DataPacket::emNormal, S2CPacketSpawnPlayer)
public:
	S2CPacket(BLOCKMAN::EntityPlayer* pPlayer)
	{
		m_entityID = pPlayer->entityId;
		m_name = pPlayer->getCommandSenderName();
		m_showName = pPlayer->getShowName();
		m_posX = int(Math::Floor(pPlayer->position.x * 32.f));
		m_posY = int(Math::Floor(pPlayer->position.y * 32.f));
		m_posZ = int(Math::Floor(pPlayer->position.z * 32.f));
		// m_yaw = (i16)((int)(pPlayer->rotationYaw * 256.0F / 360.0F));
		// m_pitch = (i16)((int)(pPlayer->rotationPitch * 256.0F / 360.0F));
		m_yaw = pPlayer->rotationYaw;
		m_pitch = pPlayer->rotationPitch;
		m_invisible = pPlayer->isInvisible();
		m_sex = pPlayer->m_sex;
		m_vip = pPlayer->m_vip;
		m_teamId = pPlayer->getTeamId();
		m_teamName = pPlayer->getTeamName();
		m_platformUserId = pPlayer->getPlatformUserId();
		m_defaultIdle = pPlayer->m_defaultIdle;
		m_isSpYaw = pPlayer->spYaw;
		m_SpYawRadian = pPlayer->spYawRadian;
		m_holdItemActor = pPlayer->m_holdItemActor;
		m_isConcealPower = pPlayer->m_isConcealPower;
	}

_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(String, m_name)
_SERIALIZE_MEMBER(String, m_showName)
_SERIALIZE_MEMBER(i32, m_posX)
_SERIALIZE_MEMBER(i32, m_posY)
_SERIALIZE_MEMBER(i32, m_posZ)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(i32, m_sex)
_SERIALIZE_MEMBER(i32, m_vip)
_SERIALIZE_MEMBER(i32, m_teamId)
_SERIALIZE_MEMBER(String, m_teamName)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_SERIALIZE_MEMBER(i32, m_defaultIdle)
_SERIALIZE_MEMBER(real32, m_SpYawRadian)
_SERIALIZE_MEMBER(String, m_holdItemActor)
_SERIALIZE_MEMBER(bool, m_invisible)
_SERIALIZE_MEMBER(bool, m_isSpYaw)
_SERIALIZE_MEMBER(bool, m_isNamePerspective)
_SERIALIZE_MEMBER(bool, m_isConcealPower)
_END_PACKET


//S2CEntitySpawnCommonPacket
_BEGIN_PACKET_S2C(S2CPacketType::SpawnVehicle, DataPacket::emNormal, S2CEntitySpawnCommonPacket)
public:
	S2CPacket(BLOCKMAN::Entity* pEntity, int type, int throwerID = 0, BLOCKMAN::PotionEffect* effect = nullptr, int meta = 0)
	{
		m_entityID = pEntity->entityId;
		m_custom_properties = pEntity->getCustomProperties();
		m_pos = pEntity->position;
		m_pitch = pEntity->rotationPitch;
		m_yaw = pEntity->rotationYaw;
		m_type = type;
		m_throwerID = throwerID;
		m_onFire = pEntity->isBurning();
		m_motion = pEntity->motion;
		m_itemDamage = meta;
		m_scale = 1;
		m_textScale = 1;
		height = pEntity->height;

		if (effect)
		{
			potionId = effect->getPotionID();
			amplifier = effect->getAmplifier();
			duration = effect->getDuration();
			isAmbient = effect->getIsAmbient();
			isSplashPotion = effect->getIsSplashPotion();
			isPotionDurationMax = effect->getIsPotionDurationMax();
		}

		auto pActorNpc = dynamic_cast<BLOCKMAN::EntityActorNpc*>(pEntity);
		if (pActorNpc)
		{
			weight = pActorNpc->getWeight();
			m_scale = pActorNpc->getScale();
			height = pActorNpc->getHeight();
			if (pActorNpc->HasProperty("textScale"))
			{
				m_textScale = pActorNpc->getFloatProperty("textScale");
			}
		}

		auto pCreature = dynamic_cast<BLOCKMAN::EntityCreature*>(pEntity);
		if (pCreature)
		{
			m_scale = pCreature->getScale();
		}
	}

_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(String, m_custom_properties)
_SERIALIZE_MEMBER(i32, m_itemID)
_SERIALIZE_MEMBER(i32, m_itemDamage)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, m_pos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, m_motion)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(i32, m_type)
_SERIALIZE_MEMBER(i32, m_throwerID)
_SERIALIZE_MEMBER(i32, potionId)
_SERIALIZE_MEMBER(i32, amplifier)
_SERIALIZE_MEMBER(i32, duration)
_SERIALIZE_MEMBER(String, weight)
_SERIALIZE_MEMBER(real32, m_scale)
_SERIALIZE_MEMBER(real32, m_textScale)
_SERIALIZE_MEMBER(real32, height)
_SERIALIZE_MEMBER(bool, m_onFire)
_SERIALIZE_MEMBER(bool, isAmbient)
_SERIALIZE_MEMBER(bool, isSplashPotion)
_SERIALIZE_MEMBER(bool, isPotionDurationMax)
_END_PACKET 

//S2CPacketRemoveEntity
_BEGIN_PACKET_S2C(S2CPacketType::RemoveEntity, DataPacket::emNormal, S2CPacketRemoveEntity)
_SERIALIZE_MEMBER(i32, m_entityID)
_END_PACKET

//S2CPacketUpdateAttribute
_BEGIN_PACKET_S2C(S2CPacketType::UpdatePlayerAttribute, DataPacket::emNormal, S2CPacketUpdateAttribute)
public:
	S2CPacket()
		:m_entityRuntimeId(0)
		,m_health(0.0f)
		,m_air(0)
		,m_foodLevel(0)
		,m_maxfoodLevel(0)
		,m_foodSaturationLevel(0.0f)
		,m_waterLevel(0)
		,m_maxwaterLevel(0)
	{

	}

	S2CPacket(BLOCKMAN::EntityPlayer * pEntity)
	{
		m_entityRuntimeId = pEntity->entityId;
		m_health = pEntity->getHealth();
		m_air = pEntity->getAir();
		m_foodLevel = pEntity->getFoodStats()->getFoodLevel();
		m_maxfoodLevel = pEntity->getFoodStats()->getmaxFoodLevel();
		m_waterLevel = pEntity->getFoodStats()->getWaterLevel();
		m_maxwaterLevel = pEntity->getFoodStats()->getmaxWaterLevel();
		m_foodSaturationLevel = pEntity->getFoodStats()->getSaturationLevel();
	}
_SERIALIZE_MEMBER(i32, m_entityRuntimeId)
_SERIALIZE_MEMBER(real32, m_health)
_SERIALIZE_MEMBER(i32, m_air)
_SERIALIZE_MEMBER(i32, m_foodLevel)
_SERIALIZE_MEMBER(i32, m_maxfoodLevel)
_SERIALIZE_MEMBER(i32, m_waterLevel)
_SERIALIZE_MEMBER(i32, m_maxwaterLevel)
_SERIALIZE_MEMBER(real32, m_foodSaturationLevel)
_END_PACKET

//S2CPacketRebirth
_BEGIN_PACKET_S2C(S2CPacketType::Rebirth, DataPacket::emNormal, S2CPacketRebirth)
_SERIALIZE_MEMBER(i32, m_playerRuntimeId)
_SERIALIZE_MEMBER(i16, m_posX)
_SERIALIZE_MEMBER(i16, m_posY)
_SERIALIZE_MEMBER(i16, m_posZ)
_SERIALIZE_MEMBER(i32, m_sex)
_SERIALIZE_MEMBER(i32, m_vip)
_SERIALIZE_MEMBER(String, m_showName)
_SERIALIZE_MEMBER(i32, m_defaultIdle)
_END_PACKET


// S2CPacketChangeItemInHand
_BEGIN_PACKET_S2C(S2CPacketType::ChangeItemInHand, DataPacket::emNormal, S2CPacketChangeItemInHand)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, meta)
_SERIALIZE_MEMBER(bool8, enchanted)
_END_PACKET

// S2CPacketChangeCurrentItem
_BEGIN_PACKET_S2C(S2CPacketType::ChangeCurrentItem, DataPacket::emNormal, S2CPacketChangeCurrentItem)
_SERIALIZE_MEMBER(i32, itemIndex)
_END_PACKET

// S2CPacketEquipArmors
_BEGIN_PACKET_S2C(S2CPacketType::EquipArmors, DataPacket::emNormal, S2CPacketEquipArmors)
public:
	struct ArmorInfo
	{
		i32 itemId;
		bool8 enchanted;
		ArmorInfo() = default;
		ArmorInfo(BLOCKMAN::ItemStackPtr armor)
		{
			itemId = armor->itemID;
			const auto tagList = armor->getEnchantmentTagList();
			enchanted = tagList && tagList->tagCount() > 0;
		}

		DEFINE_NETWORK_SERIALIZER(itemId, enchanted);
	};
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(vector<ArmorInfo>::type, armors)
_END_PACKET

// S2CPacketUnequipArmor
_BEGIN_PACKET_S2C(S2CPacketType::UnequipArmor, DataPacket::emNormal, S2CPacketUnequipArmor)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(i32, slotIndex)
_END_PACKET

// S2CPacketDestroyArmor
_BEGIN_PACKET_S2C(S2CPacketType::DestroyArmor, DataPacket::emNormal, S2CPacketDestroyArmor)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(i32, slotIndex)
_END_PACKET

// S2CPacketPlayerAction
_BEGIN_PACKET_S2C(S2CPacketType::PlayerAction, DataPacket::emNormal, S2CPacketPlayerAction)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(NETWORK_DEFINE::PacketPlayerActionType, action)
_END_PACKET

// S2CPacketSyncClientPeerProp
_BEGIN_PACKET_S2C(S2CPacketType::SyncClientPeerProp, DataPacket::emNormal, S2CPacketSyncClientPeerProp)
_SERIALIZE_MEMBER(String, propertyList)
_END_PACKET

namespace NETWORK_DEFINE
{
	struct EnchantmentInfo
	{
		i16 id;
		i16 level;
		DEFINE_NETWORK_SERIALIZER(id, level);
	};

	struct ItemStackInfo
	{
		i32 id;
		i32 size;
		i32 damage;
		i32 curClipBullet;
		i32 totalBullet;
		i32 index;
		i32 maxDamage;
		vector<EnchantmentInfo>::type enchantments;
		bool valid;
		bool needAdjustStackIndex;
		bool needPlayHotbarEffect;
		bool needResetStack;

		DEFINE_NETWORK_SERIALIZER(id, size, damage, curClipBullet, totalBullet, index, maxDamage,
			enchantments, valid, needAdjustStackIndex, needPlayHotbarEffect, needResetStack);

		ItemStackInfo() = default;
		ItemStackInfo(std::shared_ptr<BLOCKMAN::ItemStack> itemStack, i32 inventoryIndex)
		{
			index = inventoryIndex;
			if (!itemStack)
			{
				valid = false;
				id = 0;
				size = 0;
				damage = 0;
				curClipBullet = 0;
				totalBullet = 0;
				maxDamage = 0;
				needAdjustStackIndex = true;
				needPlayHotbarEffect = false;
				needResetStack = false;
				return;
			}
			valid = true;
			id = itemStack->itemID;
			size = itemStack->stackSize;
			damage = itemStack->itemDamage;
			curClipBullet = itemStack->getCurrentClipBulletNum();
			totalBullet = itemStack->getTotalBulletNum();
			needAdjustStackIndex = itemStack->m_needAdjustStackIndex;
			needPlayHotbarEffect = itemStack->m_needPlayHotbarEffect;
			needResetStack = itemStack->m_needResetStack;
			itemStack->m_needPlayHotbarEffect = false;
			itemStack->m_needAdjustStackIndex = true;
			itemStack->m_needResetStack = false;
			BLOCKMAN::Item* pItem = BLOCKMAN::Item::getItemById(id);
			if (pItem)
				maxDamage = pItem->getMaxDamage();

			auto tagList = itemStack->getEnchantmentTagList();
			if (!tagList)
				return;
			for (int i = 0; i < tagList->tagCount(); ++i)
			{
				EnchantmentInfo enchantmentInfo;
				enchantmentInfo.id = ((BLOCKMAN::NBTTagCompound*)tagList->tagAt(i))->getShort("id");
				enchantmentInfo.level = ((BLOCKMAN::NBTTagCompound*)tagList->tagAt(i))->getShort("lvl");
				enchantments.push_back(enchantmentInfo);
			}
		}
	};

	struct CoinMapping
	{
		i32 coinId;
		i32 itemId;
		DEFINE_NETWORK_SERIALIZER(coinId, itemId);

		CoinMapping() = default;
		CoinMapping(int coinId, int itemId)
		{
			this->coinId = coinId;
			this->itemId = itemId;
		}
	};
}

//S2CPacketGameInfo
_BEGIN_PACKET_S2C(S2CPacketType::GameInfo, DataPacket::emNormal, S2CPacketGameInfo)
_SERIALIZE_MEMBER(i32, m_playerRuntimeId)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3i, spawnPos)
_SERIALIZE_MEMBER(vector<String>::type, recipes)
_SERIALIZE_MEMBER(bool8, isTimeStopped)
_SERIALIZE_MEMBER(i32, sex)
_SERIALIZE_MEMBER(i32, vip)
_SERIALIZE_MEMBER(String, m_gameType)
_SERIALIZE_MEMBER(i32, m_defaultIdle)
_SERIALIZE_MEMBER(vector<i32>::type, m_selectableActions)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::CoinMapping>::type, m_coinMapping)
_END_PACKET

// S2CPacketPlayerInventory
_BEGIN_PACKET_S2C(S2CPacketType::PlayerInventory, DataPacket::emNormal, S2CPacketPlayerInventory)
_SERIALIZE_MEMBER(i32, extra_inventory_count)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::ItemStackInfo>::type, playerInventory)
_END_PACKET

// S2CPacketChestInventory
_BEGIN_PACKET_S2C(S2CPacketType::ChestInventory, DataPacket::emNormal, S2CPacketChestInventory)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_SERIALIZE_MEMBER(i32, face)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::ItemStackInfo>::type, chestInventory)
_END_PACKET

// S2CPacketUpdateChestInventory
_BEGIN_PACKET_S2C(S2CPacketType::UpdateChestInventory, DataPacket::emNormal, S2CPacketUpdateChestInventory)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::ItemStackInfo>::type, chestInventory)
_END_PACKET

//S2CPacketSpwanCreatureBullet
_BEGIN_PACKET_S2C(S2CPacketType::SpawnCreatureBullet, DataPacket::emNormal, S2CPacketSpawnCreatureBullet)
_SERIALIZE_MEMBER(int, entityId)
_SERIALIZE_MEMBER(int, type)
_SERIALIZE_MEMBER(int, throwerId)
_SERIALIZE_MEMBER(int, targetId)
_SERIALIZE_MEMBER(String, bombEffect)
_SERIALIZE_MEMBER(float, speedFactor)
_SERIALIZE_MEMBER(float, yawY)
_END_PACKET

//S2CPacketSpwanSkillItemThrowable
_BEGIN_PACKET_S2C(S2CPacketType::SpwanSkillItemThrowable, DataPacket::emNormal, S2CPacketSpwanSkillItemThrowable)
_SERIALIZE_MEMBER(int, throwerId)
_SERIALIZE_MEMBER(int, skillId)
_SERIALIZE_MEMBER(bool, notCustomSpeed)
_SERIALIZE_MEMBER(bool, isActor)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, pos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, motion)
_END_PACKET

//S2CPacketSpawnCannon
_BEGIN_PACKET_S2C(S2CPacketType::SpawnCannon, DataPacket::emNormal, S2CPacketSpawnCannon)
public:
	S2CPacket(BLOCKMAN::EntityActorCannon* pEntity)
	{
		m_entityID = pEntity->entityId;
		m_pos = pEntity->position;
		m_fallOnPos = pEntity->m_fallOnPos;
		m_yaw = pEntity->rotationYaw;
		m_pitch = pEntity->rotationPitch;
	}
_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, m_pos)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, m_fallOnPos)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(real32, m_yaw)
_END_PACKET

//S2CSpawnFlyingMount
_BEGIN_PACKET_S2C(S2CPacketType::SpawnFlyingMount, DataPacket::emNormal, S2CPacketSpawnFlyingMount)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, riderId)
_SERIALIZE_MEMBER(i32, mountSettingId)
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

_BEGIN_PACKET_S2C(S2CPacketType::SpawnFloatingMount, DataPacket::emNormal, S2CPacketSpawnFloatingMount)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, riderId)
_SERIALIZE_MEMBER(i32, mountSettingId)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(real32, bodyYaw)
_SERIALIZE_MEMBER(real32, viewYaw)
_SERIALIZE_MEMBER(real32, pitch)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SpawnEntityItem, DataPacket::emNormal, S2CPacketSpawnEntityItem)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, itemMeta)
_SERIALIZE_MEMBER(i32, itemCount)
_SERIALIZE_MEMBER(i32, enchanted)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(PACKET_DETAIL::Vector3, motion)
_END_PACKET

#endif