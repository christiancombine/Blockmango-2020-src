#include "ServerPacketSender.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityBullet.h"
#include "Entity/EntityPotion.h"
#include "Entity/EntityXPOrb.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityMerchant.h"
#include "Entity/EntityActorCannon.h"
#include "Entity/EntitySnowball.h"
#include "Entity/EntityTNTPrimed.h"
#include "Entity/EntityTNTThrowable.h"
#include "Entity/EntityCreatureBullet.h"
#include "Entity/EntityGrenade.h"
#include "Entity/EntityFireball.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Inventory/IInventory.h"
#include "Item/Potion.h"
#include "Blockman/Entity/EntityTracker.h"
#include "TileEntity/TileEntityInventory.h"
#include "Script/GameServerEvents.h"
#include "Inventory/CoinManager.h"
#include "Entity/EntityAircraft.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "Entity/EntitySkillThrowable.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Network/protocol/S2CPackets1.h"
#include "Entity/EntityFlyingMount.h"
#include "Entity/EntityFloatingMount.h"
#include "Setting/MountSetting.h"

using namespace BLOCKMAN;

ServerPacketSender::ServerPacketSender()
{

}

ServerPacketSender::~ServerPacketSender()
{

}

void ServerPacketSender::sendPacketToTrackingPlayers(int trackedEntityId, std::shared_ptr<DataPacket> packet, bool includeSelf/* = false*/)
{
	auto network = Server::Instance()->getNetwork();
	if (includeSelf) {
		EntityPlayerMP* self = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(trackedEntityId));
		if (self) {
			network->sendPacket(packet, self->getRaknetID());
		}
	}

	const auto& targetPlayers = EntityTracker::Instance()->getTrackingPlayersOf(trackedEntityId);
	for (const auto& pair : targetPlayers)
	{
		network->sendPacket(packet, dynamic_cast<EntityPlayerMP*>(pair.second)->getRaknetID());
	}
}

void ServerPacketSender::sendAddEntityEffectToTrackingPlayers(int entityId, PotionEffect* effect)
{
	auto packet = constructAddPotionEffectPacket(entityId, effect);

	//LordLogInfo("sendAddEntityEffectToTrackingPlayers: entityId: %d, potionId: %d, amplifier: %d, duration: %d",
	//	packet->entityId, packet->potionId, packet->amplifier, packet->duration);

	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::sendAddEntityEffect(BLOCKMAN::EntityPlayerMP* targetEntity, int entityId, BLOCKMAN::PotionEffect * effect)
{
	auto packet = constructAddPotionEffectPacket(entityId, effect);

	//LordLogInfo("sendAddEntityEffect: entityId: %d, potionId: %d, amplifier: %d, duration: %d",
	//	packet->entityId, packet->potionId, packet->amplifier, packet->duration);

	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID());
}

void ServerPacketSender::sendGameInfo(ui64 rakssid, int runtimeID, const Vector3i& spawnPos, 
	const vector<String>::type& recipes, bool isTimeStopped, int32_t sex, String gameType, int32_t defaultIdle, vector<i32>::type selectableActions, i32 vip)
{
	auto packet = LORD::make_shared<S2CPacketGameInfo>();
	packet->m_playerRuntimeId = runtimeID;
	packet->spawnPos = spawnPos;
	packet->recipes = recipes;
	packet->isTimeStopped = isTimeStopped;
	packet->sex = sex;
	packet->vip = vip;
	packet->m_gameType = gameType;
	packet->m_defaultIdle = defaultIdle;
	packet->m_selectableActions = selectableActions;
	LORD::map<int, int>::type coinMapping = CoinManager::Instance()->m_coinMapping;
	for (LORD::map<int, int>::type::iterator it = coinMapping.begin(); it != coinMapping.end(); ++it)
	{
		packet->m_coinMapping.push_back({ it->first, it->second });
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendUserAttrToTrackingPlayers(int entityId, const UserAttrInfo& attr, BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketUserAttr>();
	packet->entityId = entityId;
	packet->faceId = attr.faceId;
	packet->hairId = attr.hairId;
	packet->topsId = attr.topsId;
	packet->pantsId = attr.pantsId;
	packet->shoesId = attr.shoesId;
	packet->glassesId = attr.glassesId;
	packet->scarfId = attr.scarfId;
	packet->wingId = attr.wingId;
	packet->hatId = attr.hatId;
	packet->decoratehatId = attr.decoratehatId;
	packet->armId = attr.armId;
	packet->tailId = attr.tailId;
	packet->footHaloId = attr.footHaloId;
	packet->backEffectId = attr.backEffectId;
	packet->headEffectId = attr.headEffectId;
	packet->extrawingId = attr.extrawingId;
	packet->crownId = attr.crownId;
	packet->bagId = attr.bagId;
	packet->skinColor.r = attr.skinColor.r; 
	packet->skinColor.g = attr.skinColor.g;
	packet->skinColor.b = attr.skinColor.b;
	packet->skinColor.a = attr.skinColor.a;
	packet->defaultIdle = attr.defaultIdle;
	packet->selectableActions = attr.unlockedSelectableActions;
	packet->suitsId = attr.suitsId;
	packet->scale = player->getScale();
	packet->holdItemId = player->m_holdItemId;
	packet->holdItemActor = player->m_holdItemActor;
	SCRIPT_EVENT::PacketUserAttrEvent::invoke(player, 0);
	//LordLogDebug("ServerPacketSender::sendUserAttrToTrackingPlayers entityId:%d scale:%f", packet->entityId, packet->scale);
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::sendUserAttr(ui64 rakssid, int runtimeID, const UserAttrInfo& attr, BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketUserAttr>();
	packet->entityId = runtimeID;
	packet->faceId = attr.faceId;
	packet->hairId = attr.hairId;
	packet->topsId = attr.topsId;
	packet->pantsId = attr.pantsId;
	packet->shoesId = attr.shoesId;
	packet->glassesId = attr.glassesId;
	packet->scarfId = attr.scarfId;
	packet->wingId = attr.wingId;
	packet->hatId = attr.hatId;
	packet->decoratehatId = attr.decoratehatId;
	packet->armId = attr.armId;
	packet->tailId = attr.tailId;
	packet->footHaloId = attr.footHaloId;
	packet->backEffectId = attr.backEffectId;
	packet->headEffectId = attr.headEffectId;
	packet->extrawingId = attr.extrawingId;
	packet->crownId = attr.crownId;
	packet->bagId = attr.bagId;
	packet->skinColor.r = attr.skinColor.r;
	packet->skinColor.g = attr.skinColor.g;
	packet->skinColor.b = attr.skinColor.b;
	packet->skinColor.a = attr.skinColor.a;
	packet->defaultIdle = attr.defaultIdle;
	packet->selectableActions = attr.unlockedSelectableActions;
	packet->suitsId = attr.suitsId;
	packet->scale = player->getScale();
	packet->holdItemId = player->m_holdItemId;
	packet->holdItemActor = player->m_holdItemActor;
	SCRIPT_EVENT::PacketUserAttrEvent::invoke(player, rakssid);
	//LordLogDebug("ServerPacketSender::sendUserAttr rakssid:%s entityId:%d scale:%f", StringUtil::ToString(rakssid).c_str(), packet->entityId, packet->scale);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendUserAttrFromEntityToTrackingPlayers(BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketUserAttr>();
	packet->entityId = player->entityId;
	packet->faceId = player->m_faceID;
	packet->hairId = player->m_hairID;
	packet->topsId = player->m_topsID;
	packet->pantsId = player->m_pantsID;
	packet->shoesId = player->m_shoesID;
	packet->glassesId = player->m_glassesId;
	packet->scarfId = player->m_scarfId;
	packet->wingId = player->m_wingId;
	packet->hatId = player->m_hatId;
	packet->decoratehatId = player->m_decorate_hatId;
	packet->armId = player->m_armId;
	packet->footHaloId = player->m_footHaloId;
	packet->backEffectId = player->m_backEffectId;
	packet->extrawingId = player->m_extra_wingId;
	packet->crownId = player->m_crownId;
	packet->bagId = player->m_bagId;
	packet->skinColor.r = player->m_skinColor.r;
	packet->skinColor.g = player->m_skinColor.g;
	packet->skinColor.b = player->m_skinColor.b;
	packet->skinColor.a = player->m_skinColor.a;
	packet->defaultIdle = player->m_defaultIdle;
	packet->headEffectId = player->m_headEffectId;
	packet->suitsId = player->m_suitsId;
	packet->scale = player->getScale();
	packet->holdItemId = player->m_holdItemId;
	packet->holdItemActor = player->m_holdItemActor;
	packet->privilegeId = player->m_privilegeId;

	vector<i32>::type selectableActions;
	for (auto iter : player->m_actionLockedInfoMap)
	{
		selectableActions.emplace_back(iter.first);
	}
	packet->selectableActions = selectableActions;

	SCRIPT_EVENT::PacketUserAttrEvent::invoke(player, 0);

	//LordLogDebug("ServerPacketSender::sendUserAttrFromEntityToTrackingPlayers entityId:%d scale:%f", packet->entityId, packet->scale);

	sendPacketToTrackingPlayers(player->entityId, packet, true);
}

void ServerPacketSender::sendUserAttrFromEntity(ui64 rakssId, BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketUserAttr>();
	packet->entityId = player->entityId;
	packet->faceId = player->m_faceID;
	packet->hairId = player->m_hairID;
	packet->topsId = player->m_topsID;
	packet->pantsId = player->m_pantsID;
	packet->shoesId = player->m_shoesID;
	packet->glassesId = player->m_glassesId;
	packet->scarfId = player->m_scarfId;
	packet->wingId = player->m_wingId;
	packet->hatId = player->m_hatId;
	packet->decoratehatId = player->m_decorate_hatId;
	packet->armId = player->m_armId;
	packet->extrawingId = player->m_extra_wingId;
	packet->footHaloId = player->m_footHaloId;
	packet->backEffectId = player->m_backEffectId;
	packet->crownId = player->m_crownId;
	packet->headEffectId = player->m_headEffectId;
	packet->bagId = player->m_bagId;
	packet->skinColor.r = player->m_skinColor.r;
	packet->skinColor.g = player->m_skinColor.g;
	packet->skinColor.b = player->m_skinColor.b;
	packet->skinColor.a = player->m_skinColor.a;
	packet->defaultIdle = player->m_defaultIdle;
	packet->suitsId = player->m_suitsId;
	packet->scale = player->getScale();
	packet->holdItemId = player->m_holdItemId;
	packet->holdItemActor = player->m_holdItemActor;
	packet->privilegeId = player->m_privilegeId;

	vector<i32>::type selectableActions;
	for (auto iter : player->m_actionLockedInfoMap)
	{
		selectableActions.emplace_back(iter.first);
	}
	packet->selectableActions = selectableActions;

	SCRIPT_EVENT::PacketUserAttrEvent::invoke(player, rakssId);

	//LordLogDebug("ServerPacketSender::sendUserAttrFromEntity entityId:%d scale:%f", packet->entityId, packet->scale);

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerInventory(ui64 rakssid, InventoryPlayer* inventory, bool bForce/* = false*/)
{
	auto packet = LORD::make_shared<S2CPacketPlayerInventory>();
	packet->extra_inventory_count = inventory->getExtraInventoryCount();

	int dirtyIndex = inventory->getInventoryDirtyIndex();
	if (dirtyIndex >= 0)
	{
		if (inventory->mainInventory[dirtyIndex] != nullptr && inventory->mainInventoryNeedPlayEffect[dirtyIndex])
		{
			inventory->mainInventory[dirtyIndex]->m_needPlayHotbarEffect = true;
			inventory->mainInventoryNeedPlayEffect[dirtyIndex] = false;
		}
		packet->playerInventory.push_back({ inventory->mainInventory[dirtyIndex], dirtyIndex });
		inventory->setInventoryDirtyIndex(-1);
	}
	else
	{
		for (int i = 0; i < InventoryPlayer::MAIN_INVENTORY_COUNT; ++i)
		{
			if (!ItemStack::areItemStacksEqual(inventory->mainInventory[i], inventory->prevMainInventory[i]) || bForce)
			{
				if (inventory->mainInventory[i] != nullptr && inventory->mainInventoryNeedPlayEffect[i])
				{
					inventory->mainInventory[i]->m_needPlayHotbarEffect = true;
					inventory->mainInventoryNeedPlayEffect[i] = false;
				}
				packet->playerInventory.push_back({ inventory->mainInventory[i], i });
				inventory->prevMainInventory[i] = inventory->mainInventory[i] == nullptr ? nullptr : inventory->mainInventory[i]->copy();
			}
		}

		for (int i = 0; i < InventoryPlayer::ARMOR_INVENTORY_COUNT; ++i)
		{
			if (!ItemStack::areItemStacksEqual(inventory->armorInventory[i], inventory->prevArmorInventory[i]) || bForce)
			{
				packet->playerInventory.push_back({ inventory->armorInventory[i], i + InventoryPlayer::MAIN_INVENTORY_COUNT });
				inventory->prevArmorInventory[i] = inventory->armorInventory[i] == nullptr ? nullptr : inventory->armorInventory[i]->copy();
			}
		}
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendRebirth(ui64 rakssid, int runtimeID, const Vector3i& spawnPos, int sex, const String& showName, int defaultIdle, i32 vip)
{
	auto packet = LORD::make_shared<S2CPacketRebirth>();
	packet->m_playerRuntimeId = runtimeID;
	packet->m_posX = (i16)spawnPos.x;
	packet->m_posY = (i16)spawnPos.y;
	packet->m_posZ = (i16)spawnPos.z;
	packet->m_sex = sex;
	packet->m_vip = vip;
	packet->m_showName = showName;
	packet->m_defaultIdle = defaultIdle;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendEntityAttribute(EntityPlayerMP* targetEntity, EntityPlayerMP* changedEntity)
{
	auto packet = std::make_shared<S2CPacketUpdateAttribute>(changedEntity);
	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID());
}

void ServerPacketSender::sendEntityAttributeToTrackingPlayers(EntityPlayerMP* changedEntity)
{
	auto packet = std::make_shared<S2CPacketUpdateAttribute>(changedEntity);
	sendPacketToTrackingPlayers(changedEntity->entityId, packet);
}

void ServerPacketSender::sendRemoveEntity(int removedEntityId, EntityPlayerMP* targetEntity)
{
	auto packet = LORD::make_shared<S2CPacketRemoveEntity>();
	packet->m_entityID = removedEntityId;
	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID());
}

void ServerPacketSender::sendRemoveEntityToTrackingPlayers(int entityID)
{
	auto packet = LORD::make_shared<S2CPacketRemoveEntity>();
	packet->m_entityID = entityID;
	sendPacketToTrackingPlayers(entityID, packet);
}

void ServerPacketSender::sendEntityHeadRotationToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, real32 headYaw)
{
	auto packet = LORD::make_shared<S2CPacketEntityHeadRotation>();
	packet->m_entityID = entityId;
	packet->m_headYaw = headYaw;

	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID());
	}
}

void ServerPacketSender::sendEntityVelocity(ui64 rakssid, BLOCKMAN::Entity* pEntity)
{
	auto packet = LORD::make_shared<S2CPacketEntityVelocity>(pEntity);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);

}

void ServerPacketSender::sendEntityVelocityToTrackingPlayers(const EntityPlayers& trackingPlayers,
	BLOCKMAN::Entity* pEntity)
{
	auto packet = LORD::make_shared<S2CPacketEntityVelocity>(pEntity);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID());
	}
}

void ServerPacketSender::sendEntityVelocityToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, float motionX, float motionY, float motionZ)
{
	auto packet = LORD::make_shared<S2CPacketEntityVelocity>(entityId, motionX, motionY, motionZ);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID());
	}
}

void ServerPacketSender::sendEntityTeleport(ui64 rakssid, int entityId, int x, int y, int z, real32 yaw, real32 pitch)
{
	auto packet = LORD::make_shared<S2CPacketEntityTeleport>(entityId, x, y, z, yaw, pitch);
	//LordLogInfo("sendEntityTeleport, entityId=%d, (%d,%d,%d)", entityId, int(x / 32), int(y / 32), int(z / 32));
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendEntityTeleportToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, int x, int y, int z, real32 yaw, real32 pitch)
{
	auto packet = LORD::make_shared<S2CPacketEntityTeleport>(entityId, x, y, z, yaw, pitch);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		//LordLogInfo("sendEntityTeleportToTrackingPlayers, entityId=%d, (%d,%d,%d)", entityId, int(x/32), int(y/32), int(z/32));
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID());
	}
}

void ServerPacketSender::sendEntityMovementToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, i16 x, i16 y, i16 z, real32 yaw, real32 pitch)
{
	auto packet = LORD::make_shared<S2CPacketEntityMovement>(entityId, x, y, z, yaw, pitch);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID());
	}
}

void ServerPacketSender::sendEntityMovementToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, real32 yaw, real32 pitch)
{
	auto packet = LORD::make_shared<S2CPacketEntityMovement>(entityId, yaw, pitch);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID());
	}
}

void ServerPacketSender::sendEntityMovementToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, i16 x, i16 y, i16 z)
{
	auto packet = LORD::make_shared<S2CPacketEntityMovement>(entityId, x, y, z);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID());
	}
}

void ServerPacketSender::sendPlayerChangeItem(ui64 rakssid, int itemIndex)
{
	auto packet = LORD::make_shared<S2CPacketChangeCurrentItem>();
	packet->itemIndex = itemIndex;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);

}

void ServerPacketSender::sendEntitySpawn(ui64 rakssid, Entity* myEntity)
{
	std::shared_ptr<DataPacket> packet = constructEntitySpawnPacket(myEntity);
	if (packet != nullptr)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
		sendSyncEntityConnection(rakssid, myEntity);
	}
}

void ServerPacketSender::sendEquipArmors(BLOCKMAN::EntityPlayerMP * targetEntity, int entityId, const vector<ItemStackPtr>::type & armors)
{
	auto packet = LORD::make_shared<S2CPacketEquipArmors>();
	packet->playerId = entityId;
	for (const auto& armor : armors)
	{
		packet->armors.push_back({ armor });
	}
	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID());
}

void ServerPacketSender::sendEquipArmorsToTrackingPlayers(int entityId, const vector<ItemStackPtr>::type & armors)
{
	auto packet = LORD::make_shared<S2CPacketEquipArmors>();
	packet->playerId = entityId;
	for (const auto& armor : armors)
	{
		packet->armors.push_back({ armor });
	}
	sendPacketToTrackingPlayers(entityId, packet);
}

void ServerPacketSender::sendUnequipArmorToTrackingPlayers(int entityId, int slotIndex)
{
	auto packet = LORD::make_shared<S2CPacketUnequipArmor>();
	packet->playerId = entityId;
	packet->slotIndex = slotIndex;
	sendPacketToTrackingPlayers(entityId, packet);
}

void ServerPacketSender::sendDestroyArmorToTrackingPlayers(int entityId, int slotIndex)
{
	auto packet = LORD::make_shared<S2CPacketDestroyArmor>();
	packet->playerId = entityId;
	packet->slotIndex = slotIndex;
	sendPacketToTrackingPlayers(entityId, packet);
}

void ServerPacketSender::sendChangeItemInHand(BLOCKMAN::EntityPlayerMP * targetEntity, int entityId, BLOCKMAN::ItemStackPtr itemStack)
{
	auto packet = LORD::make_shared<S2CPacketChangeItemInHand>();
	packet->playerId = entityId;
	if (!itemStack)
	{
		packet->itemId = -1;
		packet->enchanted = false;
		packet->meta = 0;
	}
	else
	{
		packet->itemId = itemStack->itemID;
		auto tagList = itemStack->getEnchantmentTagList();
		packet->enchanted = tagList && tagList->tagCount() > 0;
		packet->meta = itemStack->itemDamage;
	}
	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID());
}

void ServerPacketSender::sendChangeItemInHandToTrackingPlayers(int entityId, BLOCKMAN::ItemStackPtr itemStack)
{
	auto packet = LORD::make_shared<S2CPacketChangeItemInHand>();
	packet->playerId = entityId;
	if (!itemStack)
	{
		packet->itemId = -1;
		packet->enchanted = false;
		packet->meta = 0;
	}
	else
	{
		packet->itemId = itemStack->itemID;
		auto tagList = itemStack->getEnchantmentTagList();
		packet->enchanted = tagList && tagList->tagCount() > 0;
		packet->meta = itemStack->itemDamage;
	}
	sendPacketToTrackingPlayers(entityId, packet);
}

std::shared_ptr<DataPacket> ServerPacketSender::constructEntitySpawnPacket(Entity* myEntity)
{
	if (myEntity->isDead)
	{
		// myEntity.worldObj.getWorldLogAgent().logWarning("Fetching addPacket for removed entity");
	}

	if (myEntity->isClass(ENTITY_CLASS_ITEM))
	{
		EntityItem* itemEntity = dynamic_cast<EntityItem*>(myEntity);
		auto packet = LORD::make_shared<S2CPacketSpawnEntityItem>();
		packet->entityId = itemEntity->entityId;
		packet->itemId = itemEntity->getEntityItem()->itemID;
		packet->itemMeta = itemEntity->getEntityItem()->itemDamage;
		packet->itemCount = itemEntity->getEntityItem()->stackSize;
		auto tagList = itemEntity->getEntityItem()->getEnchantmentTagList();
		packet->enchanted = tagList && tagList->tagCount() > 0;
		packet->position = itemEntity->position;
		packet->motion = itemEntity->motion;
		//LordLogInfo("SpawnEntityItem, name=%s", itemEntity->getEntityName().c_str());
		return packet;
	}
	else if (myEntity->isClass(ENTITY_CLASS_PLAYERMP))
	{
		//EntityPlayer* pPlayer = (EntityPlayer*)myEntity;
		EntityPlayer *pPlayer = dynamic_cast<EntityPlayer*>(myEntity);
		std::shared_ptr<DataPacket> packet = LORD::make_shared<S2CPacketSpawnPlayer>(pPlayer);
		//LordLogInfo("SpawnPlayer \"%s\" id[%d] pos (%.1f, %.1f, %.1f)", pPlayer->getCommandSenderName().c_str(), pPlayer->entityId, pPlayer->position.x, pPlayer->position.y, pPlayer->position.z);
		return packet;
	}
	else if (myEntity->isClass(ENTITY_CLASS_BLOCKMAN))
	{
		auto var9 =  dynamic_cast<EntityBlockman*>(myEntity);
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BLOCKMAN, (int) var9->getBlockmanType());
	}
	else if (myEntity->isClass(ENTITY_CLASS_BOAT))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BOAT, 0);
	}
	else if (myEntity->isClass(ENTITY_CLASS_ANIMALS) || myEntity->isClass(ENTITY_CLASS_DRAGON))
	{
		// do nothing special
	}
	else if (myEntity->isClass(ENTITY_CLASS_FISHHOOK))
	{
		Entity* anglerEntity = ((EntityFishHook*)myEntity)->getAngler();
		if (anglerEntity)
		{
			int anglerEntityId = anglerEntity->entityId;
			return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_FISHHOOK, anglerEntityId);
		}
	}
	else if (myEntity->isClass(ENTITY_CLASS_ARROW))
	{
		int shootingEntityId = ((EntityArrow*)myEntity)->m_shootingEntityId;
		//LordLogInfo("SpawnArrow, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f] onFire[%s]",
		//	myEntity->entityId, myEntity->position.x, myEntity->position.y, myEntity->position.z,
		//	myEntity->motion.x, myEntity->motion.y, myEntity->motion.z, myEntity->rotationYaw, myEntity->rotationPitch, myEntity->isBurning() ? "true" : "false");

		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_ARROE, shootingEntityId, ((EntityArrow*)myEntity)->m_potionEffect, ((EntityArrow*)myEntity)->getArrowID());
	}
	else if (myEntity->isClass(ENTITY_CLASS_BULLET))
	{
		int shootingEntityId = ((EntityBullet*)myEntity)->m_shootingEntityId;
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BULLET, shootingEntityId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_VEHICLE))
	{
		EntityVehicle* pVehicle = (EntityVehicle*)myEntity;
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_VEHICLE, pVehicle->getVehicleId());
	}
	else if (myEntity->isClass(ENTITY_CLASS_POTION))
	{
		Entity* thrower = ((EntityPotion*)myEntity)->getThrower();
		//LordLogInfo("SpawnEntityPotion, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f] onFire[%s]",
		//	myEntity->entityId, myEntity->position.x, myEntity->position.y, myEntity->position.z,
		//	myEntity->motion.x, myEntity->motion.y, myEntity->motion.z, myEntity->rotationYaw, myEntity->rotationPitch, myEntity->isBurning() ? "true" : "false");

		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_POTION, thrower ? thrower->entityId : myEntity->entityId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_EXP_BOTTLE))
	{
		// return new Packet23VehicleSpawn(myEntity, 75);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_ENDER_PEARL))
	{
		// return new Packet23VehicleSpawn(myEntity, 65);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_ENDER_EYE))
	{
		// return new Packet23VehicleSpawn(myEntity, 72);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_FIREWORK_ROCKET))
	{
		// return new Packet23VehicleSpawn(myEntity, 76);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_EGG))
	{
		// return new Packet23VehicleSpawn(myEntity, 62);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_TNT_PRIMED))
	{
		Entity* shootingEntity = ((EntityTNTPrimed*)myEntity)->getTntPlacedBy();
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_TNT_PRIMED, shootingEntity ? shootingEntity->entityId : 0);
	}
	else if (myEntity->isClass(ENTITY_CLASS_ENDER_CRYSTAL))
	{
		// return new Packet23VehicleSpawn(myEntity, 51);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_FALLING_SAND))
	{
		// EntityFallingSand var5 = (EntityFallingSand)myEntity;
		// return new Packet23VehicleSpawn(myEntity, 70, var5.blockID | var5.metadata << 16);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_PAINTING))
	{
		// return new Packet25EntityPainting((EntityPainting)myEntity);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_ITEM_FRAME))
	{
		/*EntityItemFrame var4 = (EntityItemFrame)myEntity;
		var2 = new Packet23VehicleSpawn(myEntity, 71, var4.hangingDirection);
		var2.xPosition = MathHelper.floor_float((float)(var4.xPosition * 32));
		var2.yPosition = MathHelper.floor_float((float)(var4.yPosition * 32));
		var2.zPosition = MathHelper.floor_float((float)(var4.zPosition * 32));
		return var2;*/
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_LEASH_KNOT))
	{
		/*EntityLeashKnot var1 = (EntityLeashKnot)myEntity;
		var2 = new Packet23VehicleSpawn(myEntity, 77);
		var2.xPosition = MathHelper.floor_float((float)(var1.xPosition * 32));
		var2.yPosition = MathHelper.floor_float((float)(var1.yPosition * 32));
		var2.zPosition = MathHelper.floor_float((float)(var1.zPosition * 32));
		return var2;*/
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_XPORB))
	{
		return LORD::make_shared<S2CPacketSpawnExpOrb>((EntityXPOrb*)myEntity);
	}
	else if (myEntity->isClass(ENTITY_CLASS_SNOWBALL))
	{
		//return LORD::make_shared<S2CPacketSpawnExpOrb>((EntityXPOrb*)myEntity);

		Entity* shootingEntity = ((EntitySnowball*)myEntity)->getThrower();
		int meta = ((EntitySnowball*)myEntity)->getMeta();
		//LordLogInfo("SpawnSnowBall, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f] onFire[%s]",
        //			myEntity->entityId, myEntity->position.x, myEntity->position.y, myEntity->position.z,
        //			myEntity->motion.x, myEntity->motion.y, myEntity->motion.z, myEntity->rotationYaw, myEntity->rotationPitch, myEntity->isBurning() ? "true" : "false");

		return LORD::make_shared<S2CEntitySpawnCommonPacket>(
				myEntity,
				ENTITY_CREATE_TYPE_SNOWBALL,
				shootingEntity ? shootingEntity->entityId : myEntity->entityId,
				nullptr,
				meta);
	}
	else if (myEntity->isClass(ENTITY_CLASS_GRENADE))
	{
		Entity* shootingEntity = ((EntityGrenade*)myEntity)->getThrower();
		auto packet = LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_GRENADE, shootingEntity ? shootingEntity->entityId : myEntity->entityId);
		packet->m_itemID = ((EntityGrenade*)myEntity)->getItemId();
		return packet;
	}
	else if (myEntity->isClass(ENTITY_CLASS_TNT_THROWABLE))
	{
		//return LORD::make_shared<S2CPacketSpawnExpOrb>((EntityXPOrb*)myEntity);

		Entity* shootingEntity = ((EntityTNTThrowable*)myEntity)->getThrower();
		//LordLogInfo("SpawnTNTThrowable, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f] onFire[%s]",
        //			myEntity->entityId, myEntity->position.x, myEntity->position.y, myEntity->position.z,
        //			myEntity->motion.x, myEntity->motion.y, myEntity->motion.z, myEntity->rotationYaw, myEntity->rotationPitch, myEntity->isBurning() ? "true" : "false");

		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_TNT_THROWABLE, shootingEntity ? shootingEntity->entityId : myEntity->entityId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_FIRE_BALL))
	{
		Entity* shootingEntity = ((EntityFireball*)myEntity)->getThrower();
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_FIREBALL, shootingEntity ? shootingEntity->entityId : myEntity->entityId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_MERCHANT))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_MERCHANT);
	}
	else if (myEntity->isClass(ENTITY_CLASS_RANK_NPC))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_RANK_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_AIRCRAFT))
	{
		EntityAircraft* pAircraft = (EntityAircraft*)myEntity;
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_AIRCRAFT, pAircraft->getAircraftId());
	}
	else if (myEntity->isClass(ENTITY_CLASS_ACTOR_NPC)) 
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_ACTOR_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_SESSION_NPC))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_SESSION_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_CREATUREAI))
	{
		EntityCreatureAI* pAi = (EntityCreatureAI*)myEntity;
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_CREATURE, pAi->m_MonsterId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_CREATURE_BULLET))
	{
		EntityCreatureBullet* pBullet = (EntityCreatureBullet*)myEntity;
		auto packet = LORD::make_shared<S2CPacketSpawnCreatureBullet>();
		packet->entityId = pBullet->hashCode();
		packet->type = (int)pBullet->getType();
		packet->throwerId = pBullet->getThrowerId();
		packet->targetId = pBullet->getTargetId();
		packet->bombEffect = pBullet->getBombEffect();
        packet->speedFactor = pBullet->getSpeedFactor();
		packet->yawY = pBullet->getYawY();
		return packet;
	}
	else if (myEntity->isClass(ENTITY_CLASS_ITEM_SKILL))
	{
		EntitySkillThrowable* pSkillThrowable = (EntitySkillThrowable*)myEntity;
		if (pSkillThrowable)
		{
			auto packet = LORD::make_shared<S2CPacketSpwanSkillItemThrowable>();
			packet->pos = pSkillThrowable->position;
			packet->throwerId = pSkillThrowable->getThrowerId();
			packet->skillId = pSkillThrowable->getSkillId();
			packet->isActor = pSkillThrowable->isRenderActor();
			packet->notCustomSpeed = pSkillThrowable->getNotCustomSpeed();
			packet->motion = pSkillThrowable->motion;
			return packet;
		}
	}
	else if (myEntity->isClass(ENTITY_CLASS_BLOCKMAN_EMPTY))
	{
		auto var9 = dynamic_cast<EntityBlockmanEmpty*>(myEntity);
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BLOCKMAN_EMPTY, (int) var9->getBlockmanType());
	}
	else if (myEntity->isClass(ENTITY_CLASS_ACTOR_CANNON))
	{
		EntityActorCannon* pCannon = (EntityActorCannon*)myEntity;
		if (pCannon)
		{
			return LORD::make_shared<S2CPacketSpawnCannon>(pCannon);
		}
	}
	else if (myEntity->isClass(ENTITY_CLASS_BUILD_NPC))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BUILD_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_LAND_NPC))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_LAND_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_BULLETIN))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BULLETIN);
	}
	else if (myEntity->isClass(ENTITY_CLASS_BIRDAI))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BIRD);
	}
	else if (myEntity->isClass(ENTITY_CLASS_DECORATION))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_DECORATION);
	}
	else if (myEntity->isClass(ENTITY_CLASS_FLYING_MOUNT))
	{
		auto mount = static_cast<EntityFlyingMount*>(myEntity);
		const auto& bodyOrientation = mount->getOrientation();
		const auto& viewOrientation = mount->getViewOrientation();
		auto packet = LORD::make_shared<S2CPacketSpawnFlyingMount>();
		packet->entityId = mount->entityId;
		packet->riderId = mount->getRiderId();
		packet->mountSettingId = mount->getSetting().id;
		packet->position = mount->position;
		packet->bodyOrientW = bodyOrientation.w;
		packet->bodyOrientX = bodyOrientation.x;
		packet->bodyOrientY = bodyOrientation.y;
		packet->bodyOrientZ = bodyOrientation.z;
		packet->viewOrientW = viewOrientation.w;
		packet->viewOrientX = viewOrientation.x;
		packet->viewOrientY = viewOrientation.y;
		packet->viewOrientZ = viewOrientation.z;
		return std::move(packet);
	}
	else if (myEntity->isClass(ENTITY_CLASS_FLOATING_MOUNT))
	{
		auto mount = static_cast<EntityFloatingMount*>(myEntity);
		auto packet = LORD::make_shared<S2CPacketSpawnFloatingMount>();
		packet->entityId = mount->entityId;
		packet->riderId = mount->getRiderId();
		packet->mountSettingId = mount->getSetting().id;
		packet->position = mount->position;
		packet->bodyYaw = mount->rotationYaw;
		auto rider = mount->getRider();
		packet->viewYaw = rider ? rider->rotationYaw : mount->rotationYaw;
		packet->pitch = mount->rotationPitch;
		return std::move(packet);
	}
	else
	{
		// throw new IllegalArgumentException("Don\'t know how to add " + myEntity.getClass() + "!");
	}

	return LORD::make_shared<S2CPacketSpawnMob>((EntityLivingBase*)myEntity);
}

void ServerPacketSender::sendPlayerAction(ui64 rakssId, int playerId, NETWORK_DEFINE::PacketPlayerActionType action)
{
	auto packet = LORD::make_shared<S2CPacketPlayerAction>();
	packet->playerId = playerId;
	packet->action = action;

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlaySound(BLOCKMAN::EntityPlayer* player, uint soundType)
{
	ui64 rakssId = dynamic_cast<EntityPlayerMP*>(player)->getRaknetID();
	auto packet = constructPlaySoundPacket(soundType);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlaySoundByType(ui64 rakssId, uint soundType)
{
	auto packet = constructPlaySoundPacket(soundType);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPlaySoundByType(uint soundType)
{
	auto packet = constructPlaySoundPacket(soundType);
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastPlaySoundByTypeAtEntity(uint soundType, i32 entityId)
{
	auto packet = constructPlaySoundPacket(soundType, entityId);
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendPlayerActionToTrackingPlayers(NETWORK_DEFINE::PacketPlayerActionType action, int playerId, bool includeSelf/* = false*/)
{
	auto packet = LORD::make_shared<S2CPacketPlayerAction>();
	packet->playerId = playerId;
	packet->action = action;

	sendPacketToTrackingPlayers(playerId, packet, includeSelf);
}

void ServerPacketSender::sendDebugString(ui64 rakssId, const String& str)
{
	auto packet = LORD::make_shared<S2CPacketDebugString>();
	packet->debugString = str;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendChestInventory(BLOCKMAN::EntityPlayer* player, Vector3i blockPos, int face, Vector3 hitPos, BLOCKMAN::IInventory* inventory)
{
	auto mp = dynamic_cast<EntityPlayerMP*> (player);
	BLOCKMAN::TileEntityChest* tileChest = dynamic_cast<BLOCKMAN::TileEntityChest*>(inventory);
	if (tileChest != nullptr)
	{
		tileChest->openChestPlayers.push_back(player->entityId);
		SCRIPT_EVENT::ChestOpenEvent::invoke(blockPos, tileChest, mp->getRaknetID());
		mp->setOpenChest(tileChest);
	}

	BLOCKMAN::TileEntityFurnace* tileFurnace = dynamic_cast<BLOCKMAN::TileEntityFurnace*>(inventory);
	if (mp && tileFurnace != nullptr)
	{
		if (!SCRIPT_EVENT::FurnaceOpenEvent::invoke(blockPos, tileFurnace, mp->getRaknetID()))
		{
			return;
		}
	}

	auto packet = LORD::make_shared<S2CPacketChestInventory>();

	for (int i = 0; i < inventory->getSizeInventory(); ++i)
	{
		if(inventory->getStackInSlot(i))
		{
			packet->chestInventory.push_back({ inventory->getStackInSlot(i), i});
		}
	}
	packet->face = face;
	packet->hitPos = hitPos;
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->entityId = player->entityId;
	LordLogInfo("=========sendChestInventory=========");

	/*int logkk = 0;
	for (auto inv : packet->chestInventory)
	{
		LordLogInfo("-----Inv-slot %d ---Id %d  ---num %d", logkk, inv.id, inv.size);
		logkk++;
	}*/
	Server::Instance()->getNetwork()->sendPacket(packet, mp->getRaknetID());

	sendCloseContainer(player->entityId, false, blockPos);
}

void ServerPacketSender::sendUpdateChestInventory(Vector3i blockPos, BLOCKMAN::IInventory * inventory)
{
	auto packet = LORD::make_shared<S2CPacketUpdateChestInventory>();
	LordLogInfo("=========sendUpdateChestInventory=========");
	for (int i = 0; i < inventory->getSizeInventory(); ++i)
	{
		if (!ItemStack::areItemStacksEqual(inventory->getStackInSlot(i), inventory->getPrevStackInSlot(i)))
		{
			packet->chestInventory.push_back({ inventory->getStackInSlot(i), i });
		}
	}
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;

	TileEntityChest* chestInventory = dynamic_cast<BLOCKMAN::TileEntityChest*>(inventory);
	if (!chestInventory)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
		return;
	}
	for (auto iter = chestInventory->openChestPlayers.begin(); iter != chestInventory->openChestPlayers.end();)
	{
		EntityPlayerMP* player = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity((*iter)));
		if (!player)
		{
			iter = chestInventory->openChestPlayers.erase(iter);
			continue;
		}
		Server::Instance()->getNetwork()->sendPacket(packet, player->getRaknetID());
		iter++;
	}
}

void ServerPacketSender::sendUpdateEnderChestInventory(ui64 rakssid, Vector3i blockPos, BLOCKMAN::IInventory * inventory)
{
	auto packet = LORD::make_shared<S2CPacketUpdateChestInventory>();
	for (int i = 0; i < inventory->getSizeInventory(); ++i)
	{
		if (!ItemStack::areItemStacksEqual(inventory->getStackInSlot(i), inventory->getPrevStackInSlot(i)))
		{
			packet->chestInventory.push_back({ inventory->getStackInSlot(i), i });
		}
	}
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendBroadCastUpdateInventory(Vector3i blockPos, BLOCKMAN::IInventory * inventory)
{
	auto packet = LORD::make_shared<S2CPacketUpdateChestInventory>();
	for (int i = 0; i < inventory->getSizeInventory(); ++i)
	{
		if (!ItemStack::areItemStacksEqual(inventory->getStackInSlot(i), inventory->getPrevStackInSlot(i)))
		{
			packet->chestInventory.push_back({ inventory->getStackInSlot(i), i });
		}
	}
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}