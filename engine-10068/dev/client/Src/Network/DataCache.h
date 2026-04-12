/********************************************************************
filename: 	DataCache
file path:	H:\sandboxol\client\blockmango-client\dev\client\Src\Network\DataCache.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/25
*********************************************************************/
#ifndef __DATACACHE_H__
#define __DATACACHE_H__

#include "Core.h"
#include "Memory/MemAllocDef.h"
#include "Object/Singleton.h"

namespace BLOCKMAN
{
	class Entity;
	class WorldClient;
	class Blockman;
	class EntityPlayer;
}

class DataCache : public LORD::Singleton<DataCache>, public LORD::ObjectAlloc
{
public:
	typedef LORD::map<int, BLOCKMAN::Entity*>::type SyncEntities;
	typedef LORD::map<int, int>::type ClientToServerIdMap;
	typedef LORD::map<int, int>::type ServerToClientIdMap;

public:
	DataCache() {}
	~DataCache() {}

	BLOCKMAN::Entity* getEntityByServerId(int serverId);
	BLOCKMAN::EntityPlayer* getPlayerByServerId(int serverId);

	void setBlockmanWorld(BLOCKMAN::Blockman* bm, BLOCKMAN::WorldClient* world);

	void setSelfServerId(int serverId) {
		m_selfServerId = serverId;
	}
	int getSelfServerId() { return m_selfServerId; }

	int getServerId(int clientId);
	int getClientId(int serverId);
	void addClientServerIdPair(int clientId, int serverId);
	void removeClientServerIdPair(int clientId);

	BLOCKMAN::Blockman* getBlockman() {
		return m_bm;
	}

	BLOCKMAN::WorldClient* getWorldClient() {
		return m_world;
	}

	void removeEntityByServerId(int serverId, bool force);
	void removeAllEntity();

private:
	int   m_selfServerId = 0;

	BLOCKMAN::Blockman* m_bm = nullptr;
	BLOCKMAN::WorldClient* m_world = nullptr;

	SyncEntities m_syncEntities;
	ClientToServerIdMap m_clientToServerIdMap;
	ServerToClientIdMap m_serverToClientIdMap;
};
#endif
