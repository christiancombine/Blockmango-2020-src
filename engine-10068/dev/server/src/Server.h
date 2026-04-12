/********************************************************************
filename: 	Server
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\Server.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/23
*********************************************************************/
#pragma once

#ifndef __SERVER_H__
#define __SERVER_H__

#include "Global.h"
#include "Blockman/World/ServerWorld.h"
#include "BM_TypeDef.h"
#include "Util/UThread.h"
#include "Object/Singleton.h"
#include "Util/UEvent.h"
#include "Util/SubscriptionGuard.h"
#include "Network/RoomClient.h"
#include "Util/UAsyncCall.h"
#include "Blockman/World/BlockDoorManager.h"
#include "Setting/GameTypeSetting.h"
#include "Blockman/World/BlockCropsManager.h"
#include "Blockman/World/BlockFruitsManager.h"

using namespace BLOCKMAN;
class ClientPeer;
class ServerNetwork;
class RoomManager;
class RoomGameConfig;
class RedisHttpRequest;

class Server:public Singleton<Server>, public ObjectAlloc
{
public:
	const static String WEB_HTTP_SECRET;

public:
	static std::string getGameName();

#if LORD_PLATFORM == LORD_PLATFORM_LINUX
	static std::string getCurrDir();
#endif

public:
	Server();
	~Server();

	void init(const RoomGameConfig& rgConfig);
	void start();
	void waitForStopEvent();
	void stop();
	void unInit();
	void stopThread();

	float getServerFramTime() { return m_serverFrameTime / 1000.0f; }
	std::shared_ptr<ServerNetwork> getNetwork() { return m_serverNetwork; }
	ServerWorld * getWorld() { return m_serverWorld; }
	void setStopEvent() { m_stopEvent.set(); }

	RoomGameConfig getConfig() { return m_config; }

	bool getEnableRoom() {	return m_enableRoom; }
	std::shared_ptr<RoomManager> getRoomManager() { return m_roomManager; }
	RoomManager* getLuaRoomManager() { return m_roomManager.get(); }
	String& getGameType() { return m_gameType; }
	void setGameType(String& gameType);
	ClientGameType getClientGameType();
	std::shared_ptr<BlockDoorManager> getBlockDoorManager() { return m_pBlockDoorManager; }
	std::shared_ptr<BlockCropsManager> getBlockCropsManager() { return m_pBlockCropsManager; }
	std::shared_ptr<BlockFruitsManager> getBlockFruitsManager() { return m_pBlockFruitsManager; }

	void startRedis();
	void setRaknetState() { m_isRaknetAlive = true; }

	void setInitPos(const Vector3i& pos);
	Vector3i& getInitPos() { return m_initPos; }
	
	bool isScriptLoaded() { return m_scriptLoaded; }

	void addAsyncCall(std::function<void(void)> fuc) { m_asyncCall.noblockCall(fuc); };
	//TODO
	std::string  getServerInfo() { return m_serverInfo; }
	void setServerInfo(std::string serverInfo);

	String getDataServerUrl();

	void setAskedToQuit() {
		m_askedToQuit.store(true);
	}
	bool askedToQuit() {
		return m_askedToQuit.load();
	}
private:
	void initSetting();
	void uninitSetting();
	void initRecipes();
	void initializeWorldModule();
	void unInitializeWorlModule();
	void initTestAttr();
	void checkAskedToQuit();
	void tick(StopFlag shouldStop);
	void sendHeartBeat();
	void initBehaviac();
	void uninitBehaviac();
	void uninitMainLogicThread();

private:
	std::atomic_bool m_isRaknetAlive { false };
	ui32      m_serverFrameTime = 0;
	int	      tickPerHeartBeat{};
	String    m_gameType = "";
	std::string    m_serverInfo = "";
	RoomGameConfig m_config;
	SubscriptionGuard m_subscriptionGuard;

	std::shared_ptr<UThread>  m_tickThread;

	ServerWorld * m_serverWorld = nullptr;

	std::shared_ptr<ServerNetwork> m_serverNetwork;

	UEvent     m_stopEvent;

	std::shared_ptr<RoomManager> m_roomManager;
	bool m_enableRoom = true;

	Vector3i   m_initPos;
	bool m_scriptLoaded = false;
	ui32      m_lastSendHeartBeatTime = 0;

	SafeAsyncCall m_asyncCall;

	std::shared_ptr<BlockDoorManager> m_pBlockDoorManager;
	std::shared_ptr<BlockCropsManager> m_pBlockCropsManager;
	std::shared_ptr<BlockFruitsManager> m_pBlockFruitsManager;
	std::shared_ptr<RedisHttpRequest> m_redisHttpRequest;

	std::atomic_bool m_askedToQuit{ false };
	ui32 m_askedToQuitTime = 0;
	i32 m_askedToQuitTipCnt = 0;

};
#endif
