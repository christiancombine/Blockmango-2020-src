/********************************************************************
filename: 	ServerNetwork
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\Network\ServerNetwork.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef _SERVERNETWORK_H__
#define _SERVERNETWORK_H__
#include "Global.h"
#include "Network/ServerNetworkCore.h"
#include "Network/ServerNetworkRecver.h"
#include "Util/Rwq/readerwriterqueue.h"
#include "Util/SubscriptionGuard.h"
#include "ServerPacketSender.h"
#include "ServerCommonDataSender.h"
#include <queue>

class C2SPacketDispatcher;
class AutoRegisterS2C;
class ClientPeer;
class ServerNetwork:public Singleton<ServerNetwork>, public ObjectAlloc
{
public:
	enum class SpeedCheatType {
		CHEAT_SPEED_NONE = 0,
		CHEAT_SPEED_UP = 1,
		CHEAT_SLOW_DOWN = 2,
	};

	struct PingInfo {
		ui32 clientSendPingTime = 0;
		ui32 serverSendPingTime = 0;
		ui32 clientRecvPingAckTime = 0;
	};

	struct ClientPing {
		PingInfo firstPing;
		PingInfo secondPing;
	};

	struct PacketRecord
	{
		ui32 sendTime = 0;
		std::shared_ptr<DataPacket> packet = nullptr;
	};

	struct PacketRecords
	{
		std::list<PacketRecord> records;
	};

	struct ReconnectRecord {
		ui64 reconnectUserId = 0;
		ui32 startReconnectTime = 0;
		bool isReconnectSuccess = false;
		std::list<std::shared_ptr<DataPacket>> historyDataPackets;
	};

public:
	ServerNetwork(const char *ip, short port);
	virtual ~ServerNetwork(void);

public:
	ServerPacketSender* getLuaSender() { return m_packetSender.get(); }
	ServerCommonDataSender* getLuaCommonSender() { return m_commonDataSender.get(); }
	void setAskToQuit() { m_networkRecver->setAskToQuit(); }

	void tick();
	bool checkReconnect(ui64 platformUserId, ui64 rakssid);
	bool kickOldLogin(ui64 platformUserId, ui64 rakssid);
	void sendPacket(std::shared_ptr<DataPacket> packet, ui64 rakssid);
	void broadCastEntityPacket(std::shared_ptr<DataPacket> packet, ui64 runtimeId);
	void broadCastEntityPacket(std::shared_ptr<DataPacket> packet);
	template<typename RaknetIdList>
	void multicastEntityPacket(std::shared_ptr<DataPacket> packet, const RaknetIdList& targets);
	void flush();
	void host(ui32 maxSession);
	void closeSession(std::shared_ptr<ClientPeer>& clientPeer);
	void closeAllSession();
	void foreachPlayer(std::function<void(std::shared_ptr<ClientPeer>)> callBack);
	void broadCastMsg(ui64 targetId, ui8 type, i32 time, const char* args);
	void broadCastMsgByType(ui64 targetId, ui8 type, i32 time, int msgType, const char* args);
	void sendGameOver(ui64 rakssid, int msgType, int code);
	void sendGameOverByPlatformUserId(ui64 platformUserId, const String& overMessage, int code);
	void broadCastGameTipByType(ui64 targetId, ui8 type, i32 time, int msgType, const char* args);
	void broadCastServerQuitting(const char* msg);

	std::shared_ptr<ClientPeer> findPlayerByRakssid(ui64 ssid);
	std::shared_ptr<ClientPeer> findPlayerByPlatformUserId(ui64 userId);
	std::shared_ptr<ServerPacketSender> getSender() { return m_packetSender; }
	std::shared_ptr<ServerCommonDataSender> getCommonSender() { return m_commonDataSender; }

	std::map<ui64, std::shared_ptr<ClientPeer>> getPlayers() { return m_playerMap; }
	std::set<ui64> getPlatformUserIds();
	int getPlayerNumber();
	void printDataPacket() { m_networkCore->printDataPacket(); }
	void deferLoginProcess(ui64 platformUserId, std::shared_ptr<DataPacket> packet);
	bool hasBeenDeferredLogin(ui64 platformUserId);

	bool onServerNetworkConnSucc(const ui64 rakssid, const LORD::String& clientIp);
	bool onServerNetworkDisconnect(const ui64 rakssid);
	bool onServerNetworkTimeout(const ui64 rakssid);
	String getPlayerIp(const ui64 rakssId);

	SpeedCheatType speedCheatCheck(const ui64 raknetId, ui32 clientRecvLastPingAckTime, ui32 clientSendPingTime, ui32 currServerTime);
	bool isKickingOut() { return m_kickingOut; }

	String getSendTotalPacketInfo();

private:
	void registerPacket100(AutoRegisterS2C& pRegister);
	void registerPacket200(AutoRegisterS2C& pRegister);
	void registerPacket300(AutoRegisterS2C& pRegister);
	void registerPacket400(AutoRegisterS2C& pRegister);

private:
	void tickReconnect();
	void tickCheckPacketRecord();
	std::shared_ptr<ChatComponentNotification> getBroadCastMsgNotify(ui8 type, i32 time, String& lanType, int msgType, const char* args);
	void onDisconnect(std::shared_ptr<ClientPeer>& clientPeer);
	void onConnectTimeout(std::shared_ptr<ClientPeer>& clientPeer);
	void clearPlayerCache(ui64 rakssid);
	void recordPacket(std::shared_ptr<DataPacket> packet, ui64 rakssid);

private:
	BLOCKMAN::SubscriptionGuard m_subscriptionGuard;

	std::shared_ptr<ServerNetworkCore> m_networkCore;
	std::shared_ptr<ServerNetworkRecver> m_networkRecver;
	std::shared_ptr<ServerPacketSender> m_packetSender;
	std::shared_ptr<ServerCommonDataSender> m_commonDataSender;

	std::map<ui64, std::shared_ptr<ClientPeer>> m_playerMap;

	std::map<ui64, ClientPing> m_clientPingMap;

	std::map<ui64, std::shared_ptr<ReconnectRecord>> m_reconnectRecordMap;

	std::map<ui64, std::shared_ptr<PacketRecords>> m_packetRecordMap;

	bool m_kickingOut = false;
};


#endif
