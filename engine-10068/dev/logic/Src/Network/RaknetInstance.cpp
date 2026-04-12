#include "RaknetInstance.h"
#include "raknet/include/MessageIdentifiers.h"
#include "Network/protocol/DataPacket.h"
#include "Network/protocol/DataPacketFactory.h"
#include "Object/Time.h"
#include <thread>
#include "RaknetSession.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

using namespace RakNet;

RaknetInstance::RaknetInstance(const char* ip, ui16 port, std::chrono::milliseconds sendFrameTime)
	: m_sendFrameTime(sendFrameTime)
{
	assert(ip != NULL);
	this->setup(ip, port);
}

RaknetInstance::RaknetInstance()
{
	this->setup("", 0);
}

RaknetInstance::~RaknetInstance(void)
{
	tearDown();
}

void RaknetInstance::setup(const char* ip, ui16 port)
{
	memset(m_localIp, 0, sizeof(m_localIp));
	strcpy(m_localIp, ip);
	m_localPort = port;

	std::shared_ptr<RakPeerInterface> pPeer(RakPeerInterface::GetInstance());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	pPeer->SetTimeoutTime(1800000, UNASSIGNED_SYSTEM_ADDRESS);
#else
	pPeer->SetTimeoutTime(10000, UNASSIGNED_SYSTEM_ADDRESS);
#endif

	this->m_pRakpeer = pPeer;
	m_recvedTimeoutThreshold = std::chrono::milliseconds(m_pRakpeer->GetTimeoutTime(UNASSIGNED_SYSTEM_ADDRESS) * 7);
	LordLogInfo("RaknetInstance::m_recvedTimeoutThreshold=%u", m_recvedTimeoutThreshold);
	m_networkThread = std::shared_ptr<UThread>(new UThread("Raknetinstance", &RaknetInstance::networkThreadMainLoop, this));
	m_bCollectingPacketData = true;   //switch collecting data packet or not
}

void RaknetInstance::tearDown()
{
	if (m_destroyed)
	{
		return;
	}

	m_networkThread->stopSync();
	m_pRakpeer->Shutdown(0);
	m_destroyed = true;
}

void RaknetInstance::connEventHandler(ConnectEvent env)
{
	auto pk = std::make_shared<ConnEventPacket>(env);
	if (m_handleDataCallBack)
		m_handleDataCallBack(pk);

}

void RaknetInstance::connect(const char * ip, ui16 port)
{
	assert(ip != NULL);

	SocketDescriptor socketDescriptor;
	socketDescriptor.port = m_localPort;
	strcpy(socketDescriptor.hostAddress, m_localIp);

	StartupResult b = m_pRakpeer->Startup(1, &socketDescriptor, 1);
	if (StartupResult::RAKNET_STARTED != b)
	{
		ConnectEvent connEvent(INVALID_RAKSOCKET, emConnectFailed);
		connEventHandler(connEvent);
		return;
	}

	bool ok = m_pRakpeer->Connect(ip, port, NULL, 0) == RakNet::CONNECTION_ATTEMPT_STARTED;
	if (ok)
	{
		m_isServer = false;
		m_networkThread->start();
	}
	else
	{
		ConnectEvent connEvent(INVALID_RAKSOCKET, emConnectFailed);
		connEventHandler(connEvent);
	}
}

void RaknetInstance::listen(i32 maxPlayers)
{

	SocketDescriptor socketDescriptor;
	socketDescriptor.port = m_localPort;
	strcpy(socketDescriptor.hostAddress, m_localIp);

	StartupResult b = m_pRakpeer->Startup(maxPlayers, &socketDescriptor, 1);
	if (StartupResult::RAKNET_STARTED != b)
	{
		return;
	}
	m_pRakpeer->SetMaximumIncomingConnections(maxPlayers);// must call this for server
	m_isServer = true;
	m_networkThread->start();
}

void RaknetInstance::setHeartbeatCallback(std::function<void()> heartBeatHandler)
{
	m_sendHeartbeatCallback = heartBeatHandler;
}

void RaknetInstance::closeSession(ui64 ssid)
{
	m_commandList.enqueue({ CommandType::CLOSE_SESSION, ssid, nullptr });
}

void RaknetInstance::count_send_packet_num(int packetId, int size, ui64 rakssid)
{
	if (!m_bCollectingPacketData)
		return;
	send_total_packet_cnt++;
	send_total_packet_byte += size;
	send_type_cnt_byte_map[packetId].first++;
	send_type_cnt_byte_map[packetId].second += size;
	send_user_cnt_map[rakssid]++;
}

void RaknetInstance::printPacketStatistics()
{
	LordLogInfo("###########################################################");
	LordLogInfo("#################send packet count begin###################");
	LordLogInfo("## Total number of sending packet: %d", send_total_packet_cnt);
	LordLogInfo("## Total size(byte) of sending packet: %d", send_total_packet_byte);
	for (auto it : send_type_cnt_byte_map) {
		float p = (it.second.second / (float)send_total_packet_byte) * 100;
		char focusFlag = ' ';
		if (p > 10.0f) {
			focusFlag = '*';
		}
		LordLogInfo("##Sending Packet Id %3d --> Total Number: %8d, Total Size: %10d, Percentage: %5.2f%%, %3c", it.first, it.second.first, it.second.second, p, focusFlag);
	}
	for (auto it : send_user_cnt_map) {
		LordLogInfo("---send user %lld --> %d", it.first, it.second);
	}
	LordLogInfo("################send packet count end######################");
	LordLogInfo("###########################################################\n");

	LordLogInfo("----------I---am---a---dividing---line--------------------\n");

	LordLogInfo("###########################################################");
	LordLogInfo("#################recv packet count begin###################");
	LordLogInfo("## Total number of receiving packet: %d", recv_total_packet_cnt);
	LordLogInfo("## Total size(byte) of receiving packet: %d", recv_total_packet_byte);
	for (auto it : recv_type_cnt_byte_map) {
		float p = (it.second.second / (float)recv_total_packet_byte) * 100;
		char focusFlag = ' ';
		if (p > 10.0f) {
			focusFlag = '*';
		}
		LordLogInfo("##Receiving Packet Id %3d --> Total Number: %8d Total Size: %10d, Percentage: %5.2f%%, %3c", it.first, it.second.first, it.second.second, p, focusFlag);
	}
	for (auto it : recv_user_cnt_map) {
		LordLogInfo("---send user %lld --> %d", it.first, it.second);
	}
	LordLogInfo("################recv packet count end######################");
	LordLogInfo("###########################################################\n");
}

void RaknetInstance::count_recv_packet_num(int packetId, int size, ui64 rakssid)
{
	if (!m_bCollectingPacketData)
		return;
	recv_total_packet_cnt++;
	recv_total_packet_byte += size;
	recv_type_cnt_byte_map[packetId].first++;
	recv_type_cnt_byte_map[packetId].second += size;
	recv_user_cnt_map[rakssid]++;
}

void RaknetInstance::send(std::shared_ptr<DataPacket> packet, ui64 rakssid)
{
	m_commandList.enqueue({ CommandType::SEND_PACKET, rakssid, std::move(packet) });
}


void RaknetInstance::networkThreadMainLoop(StopFlag shouldStop)
{
	using namespace std::chrono;
	auto lastFlushTime = steady_clock::now();
	while(!shouldStop)
	{
		const int limit = m_isServer ? 500 : 100;
		bool receiveFinished = receivePackets(limit);
		bool sendFinished = sendPackets(limit);

		auto currentTime = steady_clock::now();
		if (currentTime - lastFlushTime >= m_sendFrameTime || m_forceFlush)
		{
			lastFlushTime = currentTime;
			m_forceFlush = false;
			for (auto&& pair : m_sessionList)
			{
				pair.second->flush();
			}
		}
	
		if (receiveFinished && sendFinished)
		{
			std::this_thread::sleep_for(milliseconds(5));
		}

		if (m_isServer && m_sendHeartbeatCallback)
		{
			m_sendHeartbeatCallback();
		}

		if (!m_sessionList.empty())
		{
			const auto& firstSession = m_sessionList.begin()->second;
			m_raknetPing.store(m_pRakpeer->GetLastPing(firstSession->getRemoteAddress()));
		}
		else
		{
			m_raknetPing.store(-1);
		}

		if (m_lastRecvedPacketTime.time_since_epoch().count() == 0 || 
			steady_clock::now() - m_lastRecvedPacketTime < m_recvedTimeoutThreshold)
		{
			m_isAlive.store(true);
		}
		else
		{
			m_isAlive.store(false);
		}
	}
}

bool RaknetInstance::receivePackets(int num)
{
	for (int i = 0; i < num; ++i)
	{
		Packet* packet = m_pRakpeer->Receive();
		if (!packet)
			return true;
		ui64 sessionId = SystemAddress::ToInteger(packet->systemAddress);
		switch (packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			onConnectResult(packet->systemAddress, true);
			LordLogInfo("recv raknet message ID_CONNECTION_REQUEST_ACCEPTED, sessionId=" U64FMT, sessionId);
			break;

		case ID_CONNECTION_LOST://Time Out
			onConnectTimeout(packet->systemAddress);
			LordLogInfo("recv raknet message ID_CONNECTION_LOST, sessionId=" U64FMT, sessionId);
			break;

		case ID_DISCONNECTION_NOTIFICATION: // server kickOut
			onDisconnect(packet->systemAddress);
			LordLogInfo("recv raknet message ID_DISCONNECTION_NOTIFICATION, sessionId=" U64FMT, sessionId);
			break;

		case ID_NEW_INCOMING_CONNECTION:
			onNewIncomingConn(packet->systemAddress);
			LordLogInfo("recv raknet message ID_NEW_INCOMING_CONNECTION, sessionId=" U64FMT, sessionId);
			break;

		case ID_CONNECTION_ATTEMPT_FAILED:
			onConnectResult(packet->systemAddress, false);
			LordLogInfo("recv raknet message ID_CONNECTION_ATTEMPT_FAILED, sessionId=" U64FMT, sessionId);
			break;

		case RaknetSession::ID_NORMAL_DATA:
			handleNormalData(packet->systemAddress, packet);
			break;

		default:
			LordLogWarning("recv raknet message unknown, type=%d, sessionId=" U64FMT, int(packet->data[0]), sessionId);
			onDisconnect(packet->systemAddress);
			break;
		}

		m_pRakpeer->DeallocatePacket(packet);
	}
	return false;
}

bool RaknetInstance::sendPackets(int num)
{
	int count = 0;
	Command command;
	while (count < num && m_commandList.try_dequeue(command))
	{
		switch (command.type)
		{
		case CommandType::SEND_PACKET:
		{
			auto it = this->m_sessionList.find(command.sessionId);
			if (it == this->m_sessionList.end())
				continue;
			auto&& session = it->second;
			const auto& packet = *command.packet;
			auto sentSize = session->sendPacket(packet);
			count_send_packet_num(packet.pid(), sentSize, command.sessionId);
			++count;
			break;
		}
		case CommandType::CLOSE_SESSION:
		{
			auto it = this->m_sessionList.find(command.sessionId);
			if (it == this->m_sessionList.end())
				continue;
			auto&& session = it->second;
			session->flush();
			session->close();
			this->m_sessionList.erase(it);
			break;
		}
		case CommandType::PRINT_STATISTICS:
		{
			printPacketStatistics();
			break;
		}
		default:
			break;
		}
	}
	return count < num;
}

void  RaknetInstance::recv(std::function<void(std::shared_ptr<DataPacket>)> handler)
{
	this->m_handleDataCallBack = handler;
}

void RaknetInstance::onConnectResult(SystemAddress& addr, bool ret)
{
	ui64 sock = INVALID_RAKSOCKET;
	if (ret)
	{
		sock = SystemAddress::ToInteger(addr);
		m_sessionList[sock] = std::make_unique<RaknetSession>(m_pRakpeer.get(), addr);

		m_raknetPing.store(m_pRakpeer->GetLastPing(addr));

		ConnectEvent connEvent(sock, emConnectSuc);
		connEventHandler(connEvent);

	}
	else
	{
		ConnectEvent connEvent(sock, emConnectFailed);
		connEventHandler(connEvent);
	}


}

void RaknetInstance::onConnectTimeout(SystemAddress& addr)
{
	ui64 sock = SystemAddress::ToInteger(addr);

	auto session = m_sessionList.find(sock);
	if (session != m_sessionList.end())
	{
		m_sessionList.erase(session);
	}

	ConnectEvent connEvent(sock, emConnectTimeout);
	connEventHandler(connEvent);
}

void RaknetInstance::onConnectKickOut(SystemAddress& addr)
{
	ui64 sock = SystemAddress::ToInteger(addr);

	auto session = m_sessionList.find(sock);
	if (session != m_sessionList.end())
	{
		m_sessionList.erase(session);
	}

	ConnectEvent connEvent(sock, emConnectKickOut);
	connEventHandler(connEvent);
}

void RaknetInstance::onDisconnect(SystemAddress& addr)
{
	ui64 sock = SystemAddress::ToInteger(addr);

	auto session = m_sessionList.find(sock);
	if (session != m_sessionList.end())
	{
		m_sessionList.erase(session);
	}

	ConnectEvent connEvent(sock, emDisconnect);
	connEventHandler(connEvent);
}

void RaknetInstance::onNewIncomingConn(SystemAddress& addr)
{
	LORD::String clientIp = LORD::String(addr.ToString(false, '.'));
	ui64 sock = SystemAddress::ToInteger(addr);
	m_sessionList[sock] = std::make_unique<RaknetSession>(m_pRakpeer.get(), addr);

	ConnectEvent connEvent(sock, emNewConnect, clientIp);
	connEventHandler(connEvent);
}


static std::string netDataToHex(const void* data, int len)
{
	std::string rt;
	for (int i = 0; i < len; i++)
	{
		char buf[10];
		memset(buf, 0, 10);
		sprintf(buf, " %02x", reinterpret_cast<const unsigned char*>(data)[i]);

		rt = rt + buf;
	}

	return rt;
}

void RaknetInstance::handleNormalData(SystemAddress& addr, Packet* rakPacket)
{
	m_lastRecvedPacketTime = std::chrono::steady_clock::now();
	ui64 rakssid = SystemAddress::ToInteger(addr);
	if (rakPacket->length <= 1)
	{
		LordLogError("received empty packet, rakid: %llu", rakssid);
		return;
	}
	BitStream stream(rakPacket->data + 1, rakPacket->length - 1, false);
	auto paddingSize = rakPacket->length * 8 - rakPacket->bitSize;
	try
	{
		while (stream.GetNumberOfUnreadBits() > paddingSize)
		{
			auto offsetBefore = stream.GetReadOffset();
			ui16 packetId;
			Serializer::read(packetId, &stream);
			std::shared_ptr<DataPacket> pDataPacket(
				DataPacketFactory::getInstance(m_isServer)->create(packetId));

			if (!pDataPacket)
			{
				LordLogError("unknown packet id: %d", packetId);
				return;
			}
			pDataPacket->decode(&stream);
			if (stream.GetNumberOfUnreadBits() < paddingSize)
			{
				LordLogError("reached end of stream");
				return;
			}
			auto readBits = stream.GetReadOffset() - offsetBefore;
			auto readBytes = (readBits + 7) / 8;
			count_recv_packet_num(packetId, readBytes, rakssid);
			pDataPacket->setSender(SystemAddress::ToInteger(rakPacket->systemAddress));
			if (m_handleDataCallBack)
			{
				m_handleDataCallBack(pDataPacket);
			}
		}
	}
	catch (const std::exception& e)
	{
		LordLogError("packet decode error, rakid: %llu, error: %s ", rakssid, e.what());
		LordLogError("packet data=[%s]", netDataToHex(stream.GetData(), stream.GetNumberOfBytesUsed()).c_str());
	}
}

void RaknetInstance::printCollectingPacket()
{
	m_commandList.enqueue({ CommandType::PRINT_STATISTICS, 0, nullptr });
}

void RaknetInstance::enableRaknetPing()
{
	m_pRakpeer->SetOccasionalPing(true);
}

int RaknetInstance::getRaknetPing()
{
	return m_raknetPing;
}

bool RaknetInstance::isAlive() {
	if (!m_isAlive)
	{
		LordLogInfo("m_isAlive.store false");
		return false;
	}
	return true;
}

bool RaknetInstance::isSessionAlive(ui64 sessionId)
{
	auto session = m_sessionList.find(sessionId);
	if (session != m_sessionList.end())
	{
		return true;
	}
	return false;
}

String RaknetInstance::getSendTotalPacketInfo()
{
	return StringUtil::Format("TotalPacketCount=%d&TotalPacketByte=%d", send_total_packet_cnt, send_total_packet_byte);
}
