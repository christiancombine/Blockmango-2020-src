#include "DataCache.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/EntityOtherPlayerMP.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "game.h"

#include "World/World.h"
#include "Entity/Entity.h"
#include "Entity/EntityPlayer.h"
#include "Entity/DamageSource.h"
#include "Util/ClientEvents.h"
#include "cEntity/EntityFishHookClient.h"
#include "Entity/EntityItem.h"
#include "Plugins/GameClientEvents.h"

using namespace  BLOCKMAN;

void DataCache::setBlockmanWorld(BLOCKMAN::Blockman* bm, BLOCKMAN::WorldClient* world)
{
	m_bm = bm;
	m_world = world;
}

void DataCache::removeEntityByServerId(int serverId, bool force)
{
	Entity* entity = getEntityByServerId(serverId);
	if (entity != nullptr) {
		if (entity->isClass(ENTITY_CLASS_THROWABLE)) {
			// let it fly and will be destroyed when collided
			return;
		}
		if (entity->isClass(ENTITY_CLASS_FISHHOOK_CLIENT)) {
			EntityFishHookClient* fishHook = dynamic_cast<EntityFishHookClient*>(entity);
			if (fishHook)
			{
				EntityPlayer* angler = dynamic_cast<EntityPlayer*>(fishHook->getAngler());
				if (angler)
				{
					angler->m_fishEntity = nullptr;
				}
			}
		}
		if (entity->isClass(ENTITY_CLASS_ITEM)) {
			EntityItem* item = dynamic_cast<EntityItem*>(entity);
			if (item)
			{
				item->m_readyToDead = true;
				if (!force)
					return;
			}
		}
		entity->setDead();

		EntityPlayer* player = dynamic_cast<EntityPlayer*>(entity);
		if (player) {
			SCRIPT_EVENT::LuaPlayerDeathEvent::invoke(player->getPlatformUserId());
			BLOCKMAN::Blockman::Instance()->getWorld()->removePlayerEntityDangerously(player);
			player->setLogout(true);
			player->setDelayTicksToFree(force ? 0 : 20);
		}
	}
}

void DataCache::removeAllEntity()
{
	for (auto it : m_clientToServerIdMap)
	{
		removeEntityByServerId(it.second, true);
	}
	m_serverToClientIdMap.clear();
	m_clientToServerIdMap.clear();
}

BLOCKMAN::Entity* DataCache::getEntityByServerId(int serverId)
{
	if (serverId == m_selfServerId) {
		return (Entity*)m_bm->m_pPlayer;
	}

	int clientId = getClientId(serverId);
	if (clientId == 0) {
		return nullptr;
	}

	return BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(clientId);
}

BLOCKMAN::EntityPlayer* DataCache::getPlayerByServerId(int serverId)
{
	if (serverId == m_selfServerId) {
		return (EntityPlayer*)m_bm->m_pPlayer;
	}

	int clientId = getClientId(serverId);
	if (clientId == 0) {
		return nullptr;
	}
	return BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(clientId);
}

int DataCache::getClientId(int serverId)
{
	auto it = m_serverToClientIdMap.find(serverId);
	if (it == m_serverToClientIdMap.end()) {
		return 0;
	}

	return it->second;
}


int DataCache::getServerId(int clientId)
{
	auto it = m_clientToServerIdMap.find(clientId);
	if (it == m_clientToServerIdMap.end()) {
		return 0;
	}

	return it->second;
}

void DataCache::addClientServerIdPair(int clientId, int serverId)
{
	auto oldClientId = getClientId(serverId);
	if (oldClientId != 0)
	{
		removeEntityByServerId(serverId, true);
		removeClientServerIdPair(oldClientId);
	}
	m_clientToServerIdMap.insert(std::make_pair(clientId, serverId));
	m_serverToClientIdMap.insert(std::make_pair(serverId, clientId));
}

void DataCache::removeClientServerIdPair(int clientId)
{
	auto it = m_clientToServerIdMap.find(clientId);
	if (m_clientToServerIdMap.end() == it) {
		LordLogError("try to remove client server id pair, but can not find, clientId=%d", clientId);
		return;
	}
	int serverId = it->second;
	
	m_clientToServerIdMap.erase(clientId);
	m_serverToClientIdMap.erase(serverId);
}
