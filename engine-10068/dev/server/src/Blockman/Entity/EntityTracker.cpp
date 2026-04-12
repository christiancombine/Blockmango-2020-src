#include "EntityTracker.h"
#include "EntityPlayerMP.h"

#include "World/World.h"
#include "Chunk/Chunk.h"
#include "Util/PlayerPrefs.h"


namespace BLOCKMAN
{

EntityTracker::EntityTracker(World* pWorld)
{
	m_world = pWorld;
}

EntityTracker::~EntityTracker()
{
	for (auto i : m_entitySet) {
		EntityTrackerEntry* pEntry = i;
		LordDelete(pEntry);
	}
	m_entitySet.clear();
	m_idEntityMap.clear();
}

void EntityTracker::addEntityToTracker(Entity* toAddEntity)
{
	if (toAddEntity->isClass(ENTITY_CLASS_PLAYERMP))
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)toAddEntity;
		if (pPlayer->isLogout())
			return;
        addEntityToTracker_inner(toAddEntity, 512, 2);
		for (auto it : m_entitySet)
		{
			EntityTrackerEntry* pEntry = it;
            if (pEntry->myEntity != pPlayer)
            {
                pEntry->spawnOrRemoveSelfEntityAtClientOf(pPlayer);
            }
		}

        if (toAddEntity)
        {
            forceTeleport(toAddEntity->entityId);
        }
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FISHHOOK))
	{
        addEntityToTracker_inner(toAddEntity, 64, 5, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ARROW))
	{
        addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BULLET))
	{
        addEntityToTracker_inner(toAddEntity, 128, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_SMALL_FIRE_BALL))
	{
        addEntityToTracker_inner(toAddEntity, 64, 10, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FIRE_BALL))
	{
		addEntityToTracker_inner(toAddEntity, 64, 10, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ENDER_PEARL))
	{
		addEntityToTracker_inner(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ENDER_EYE))
	{
		addEntityToTracker_inner(toAddEntity, 64, 4, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_EGG))
	{
		addEntityToTracker_inner(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_POTION))
	{
		addEntityToTracker_inner(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_EXP_BOTTLE))
	{
		addEntityToTracker_inner(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FIREWORK_ROCKET))
	{
		addEntityToTracker_inner(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ITEM))
	{
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BLOCKMAN))
	{
		addEntityToTracker_inner(toAddEntity, 64, 20, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BOAT))
	{
		addEntityToTracker_inner(toAddEntity, 80, 3, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_SQUID))
	{
		addEntityToTracker_inner(toAddEntity, 64, 3, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_WITHER))
	{
		addEntityToTracker_inner(toAddEntity, 80, 3, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BAT))
	{
		addEntityToTracker_inner(toAddEntity, 80, 3, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ANIMALS))
	{
		addEntityToTracker_inner(toAddEntity, 80, 3, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_DRAGON))
	{
		addEntityToTracker_inner(toAddEntity, 160, 3, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_TNT_PRIMED))
	{
		addEntityToTracker_inner(toAddEntity, 160, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FALLING_SAND))
	{
		addEntityToTracker_inner(toAddEntity, 160, 20, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_HANGING))
	{
		addEntityToTracker_inner(toAddEntity, 160, Math::MAX_I32, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_XPORB))
	{
		addEntityToTracker_inner(toAddEntity, 160, 20, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ENDER_CRYSTAL))
	{
		addEntityToTracker_inner(toAddEntity, 256, Math::MAX_I32, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_SNOWBALL))
	{
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_TNT_THROWABLE))
	{
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_MERCHANT))
	{
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_GRENADE))
	{
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_VEHICLE))
	{
		addEntityToTracker_inner(toAddEntity, 128, 10, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_RANK_NPC))
	{
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_AIRCRAFT))
	{
		addEntityToTracker_inner(toAddEntity, 128, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ACTOR_NPC)) {
		int viewDis = 64;
		if (PlayerPrefs::Instance()->HasProperty("ActorNpcViewDis"))
		{
			viewDis = PlayerPrefs::Instance()->GetProperty<int>("ActorNpcViewDis");
		}
		addEntityToTracker_inner(toAddEntity, viewDis, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_SESSION_NPC)) {
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_CREATUREAI)) {
		int viewDis = 256;
		int syncMoveTick = 12;
		if (PlayerPrefs::Instance()->HasProperty("CreatureAIViewDis"))
			viewDis = PlayerPrefs::Instance()->getIntPrefs("CreatureAIViewDis");
		if (PlayerPrefs::Instance()->HasProperty("CreatureAISyncMoveTicks"))
			syncMoveTick = PlayerPrefs::Instance()->getIntPrefs("CreatureAISyncMoveTicks");
		addEntityToTracker_inner(toAddEntity, viewDis, syncMoveTick, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_CREATURE_BULLET)) {
		addEntityToTracker_inner(toAddEntity, 128, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ITEM_SKILL))
	{
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BLOCKMAN_EMPTY)) {
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BUILD_NPC)) {
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_LAND_NPC)) {
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ACTOR_CANNON)) {
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BULLETIN)) {
		addEntityToTracker_inner(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BIRDAI)) {
		addEntityToTracker_inner(toAddEntity, 64, 12, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_DECORATION)) {
		addEntityToTracker_inner(toAddEntity, 64, 6, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FLYING_MOUNT)) {
		addEntityToTracker_inner(toAddEntity, 512, 1, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FLOATING_MOUNT)) {
		addEntityToTracker_inner(toAddEntity, 512, 1, false);
	}
	else {
		LordLogError("EntityTracker::addEntityToTracker, entity is belong to unknown class");
	}
}

void EntityTracker::addEntityToTracker_inner(Entity* toAddEntity, int viewDis, int frequency, bool sendVelocity)
{
	EntityTrackerEntry* pEntry = nullptr;
	bool rebirth = false;
	if (m_idEntityMap.find(toAddEntity->entityId) == m_idEntityMap.end()) {
		pEntry = LordNew EntityTrackerEntry(toAddEntity, viewDis, frequency, sendVelocity);
		m_entitySet.emplace(pEntry);
		m_idEntityMap.emplace(std::make_pair(toAddEntity->entityId, pEntry));
	}
	else {
		pEntry = m_idEntityMap.find(toAddEntity->entityId)->second;
		pEntry->reinit(toAddEntity, viewDis, frequency, sendVelocity);
		rebirth = true;
		LordLogInfo("Entity is already tracked,id=%d, name=%s", toAddEntity->entityId, toAddEntity->getEntityName().c_str());
	}

	//LordLogInfo("Add entity [%s] to EntityTracker list", toAddEntity->getEntityName().c_str());
	pEntry->spawnOrRemoveSelfEntityFor(m_world->getPlayers(), rebirth);
    /*if (toAddEntity && toAddEntity->isClass(ENTITY_CLASS_PLAYERMP))
    {
        forceTeleport(toAddEntity->entityId);
    }*/
}

void EntityTracker::removeEntityFromAllTrackingPlayers(Entity* pEntity)
{
	if (pEntity->isClass(ENTITY_CLASS_PLAYERMP))
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)pEntity;

		for (auto it : m_entitySet)
		{
			EntityTrackerEntry* pEntry = it;
			//LordLogInfo("Del player [%s] to EntityTracker list", pEntity->getEntityName().c_str());
			it->removeFromWatchingList(pPlayer);
		}
	}

	auto it = m_idEntityMap.find(pEntity->entityId);
	if (it != m_idEntityMap.end())
	{
		EntityTrackerEntry* pEntry = it->second;
		if (pEntry)
		{
			m_entitySet.erase(pEntry);
			pEntry->informAllAssociatedPlayersOfItemDestruction();
			m_idEntityMap.erase(it);

			LordDelete(pEntry);
		}
	}
}

void EntityTracker::updateTrackedEntities()
{
	vector<EntityPlayerMP*>::type players;

	for(auto it : m_entitySet)
	{
		EntityTrackerEntry* pEntry = it;
		pEntry->informMovementChangeToTrackingPlayers(m_world->getPlayers());

		if (pEntry->playerEntitiesUpdated && pEntry->myEntity->isClass(ENTITY_CLASS_PLAYERMP))
		{
			players.emplace_back((EntityPlayerMP*)pEntry->myEntity);
		}
	}

	for (auto it : players)
	{
		EntityPlayerMP* pPlayer = it;
		
		for (auto jt : m_entitySet)
		{
			EntityTrackerEntry* pEntry = jt;

			if (pEntry->myEntity != pPlayer)
			{
				pEntry->spawnOrRemoveSelfEntityAtClientOf(pPlayer);
			}
		}
	}
}

void EntityTracker::removePlayerFromTrackers(EntityPlayerMP* pPlayer)
{
	for (auto it : m_entitySet)
	{
		EntityTrackerEntry* pEntry = it;
		it->removePlayerFromTracker(pPlayer);
	}
}

void EntityTracker::sendLeashedEntitiesInChunk(EntityPlayerMP* pPlayer, const ChunkPtr& pChunk)
{
	for (auto it : m_entitySet)
	{
		EntityTrackerEntry* pEntry = it;

		if (pEntry->myEntity != pPlayer && 
			pEntry->myEntity->chunkCoord.x == pChunk->m_posX &&
			pEntry->myEntity->chunkCoord.z == pChunk->m_posZ)
		{
			pEntry->spawnOrRemoveSelfEntityAtClientOf(pPlayer);
		}
	}
}

const EntityPlayers EntityTracker::getTrackingPlayersOf(int entityId) const
{
	auto iter = m_idEntityMap.find(entityId);
	if (iter == m_idEntityMap.end())
	{
		return {};
	}
	return iter->second->trackingPlayers;
}

bool EntityTracker::forceTeleport(int entityId)
{
	auto iter = m_idEntityMap.find(entityId);
	if (iter == m_idEntityMap.end()) {
		return false;
	}

	iter->second->forceTelport();

	return true;
}

}
