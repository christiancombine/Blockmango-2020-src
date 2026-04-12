/********************************************************************
filename: 	RaknetInstance
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\network\RaknetInstance.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __RAKNETINSTANCE_H__
#define __RAKNETINSTANCE_H__

#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <mutex> 
#include <chrono>

#include "raknet/include/RakPeerInterface.h"
#include "raknet/include/BitStream.h"
#include "raknet/include/MessageIdentifiers.h"

#include "Network/protocol/DataPacket.h"
#include "Util/UThread.h"
#include "Util/UAsyncCall.h"
#include "TypeDef.h"
using namespace RakNet;

static constexpr ui64 INVALID_RAKSOCKET = -1;

class RaknetSession;

class RaknetInstance
{
private:
	enum class CommandType
	{
		SEND_PACKET,
		CLOSE_SESSION,
		PRINT_STATISTICS
	};

	struct Command
	{
		CommandType type;
		ui64 sessionId;
		std::shared_ptr<DataPacket> packet;
	};

public:
	enum ConnectEventType
	{
		emConnectSuc,
		emConnectFailed,
		emConnectTimeout,
		emConnectKickOut,
		emDisconnect,
		emNewConnect
	};

	struct ConnectEvent
	{
	public:
		ConnectEvent() {}
		ConnectEvent(ui64 id, ConnectEventType _type, LORD::String ip = "") :rakssid(id), type(_type), clientIp(ip)
		{}
		ui64 rakssid;
		ConnectEventType type;
		LORD::String clientIp;
	};

	// 特殊Packet 为了ConnEvent 能跟 packet处于同一列表中。
	class ConnEventPacket :public DataPacket
	{
	public:
		ConnEventPacket(ConnectEvent evn)
			:connEvent(evn)
		{}

		bool isConnEvent() override { return true; }
	public:
		ConnectEvent  connEvent;
	};

public:
	RaknetInstance(const char *ip, ui16 port, std::chrono::milliseconds sendFrameTime = std::chrono::milliseconds(50));
	RaknetInstance();
	virtual ~RaknetInstance(void);
	void tearDown();

	void connect(const char * ip, ui16 port);
	void listen(i32 maxPlayers);
	void setHeartbeatCallback(std::function<void()> heartBeatHandler);

	void closeSession(ui64 rakSock);
	void connEventHandler(ConnectEvent evn);

	void send(std::shared_ptr<DataPacket> packet, ui64 rakssid);
	void flush() { m_forceFlush = true; }

	void recv(std::function<void(std::shared_ptr<DataPacket>)> handler);

	void printCollectingPacket();

	void enableRaknetPing();
	int getRaknetPing();

	bool isAlive();
	bool isSessionAlive(ui64 sessionId);

	String getSendTotalPacketInfo();

private:
	void setup(const char * ip, ui16 port);

	void networkThreadMainLoop(StopFlag shouldStop);
	bool receivePackets(int num);
	bool sendPackets(int num);

	void onConnectResult(SystemAddress &addr,bool ret);
	void onConnectTimeout(SystemAddress &addr);
	void onConnectKickOut(SystemAddress &addr);
	void onDisconnect(SystemAddress &addr);
	void onNewIncomingConn(SystemAddress &addr);

	void handleNormalData(SystemAddress &addr, Packet *rakPacket);

	void count_recv_packet_num(int packetId, int size, ui64 rakssid);
	void count_send_packet_num(int packetId, int size, ui64 rakssid);
	void printPacketStatistics();

private:

	RakNet::SystemAddress m_local;

	std::shared_ptr<RakPeerInterface> m_pRakpeer;

	std::function<void(ConnectEvent&)> m_connectEventcallback = nullptr;

	std::function<void(std::shared_ptr<DataPacket>)> m_handleDataCallBack = nullptr;
	std::function<void()> m_sendHeartbeatCallback = nullptr;

	bool m_isServer = false;

	std::map<ui64, std::unique_ptr<RaknetSession>> m_sessionList;

	std::shared_ptr<UThread> m_networkThread;

	moodycamel::ReaderWriterQueue<Command> m_commandList;

	char m_localIp[20] = { 0 };
	ui16 m_localPort = 0;
	bool m_destroyed = false;

	std::atomic<int> m_raknetPing{0};

	// for collecting data packets
	bool m_bCollectingPacketData = false;
	int send_total_packet_cnt = 0;  // total amount of sending packet
	int send_total_packet_byte = 0; // total size of sending packet
	std::map<int, std::pair<int, int>> send_type_cnt_byte_map; // key: sending packetId value: pair, first: total amount of this kind of packet second: total size(byte) of this kind of packet
	std::map<ui64, int> send_user_cnt_map; // key: user raknetId value: total amount of packet sending to this user  

	int recv_total_packet_cnt = 0;  // total amount of receiving packet
	int recv_total_packet_byte = 0; // total size of receiving packet
	std::map<int, std::pair<int, int>> recv_type_cnt_byte_map; // key: receiving packetId value: pair, first: total amount of this kind of packet second: total size(byte) of this kind of packet
	std::map<ui64, int> recv_user_cnt_map; // key: user raknetId value: total amount of packet receiving from this user 

	std::atomic<bool> m_isAlive{true};
	std::chrono::steady_clock::time_point m_lastRecvedPacketTime;
	std::chrono::milliseconds m_recvedTimeoutThreshold{ 20000 };
	std::chrono::milliseconds m_sendFrameTime{ 50 };
	std::atomic_bool m_forceFlush{false};
};
#endif
