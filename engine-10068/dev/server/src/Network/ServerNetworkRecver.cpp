#include "ServerNetworkRecver.h"
#include "ClientPeer.h"
#include "Network/protocol/DataPacketFactory.h"

#include "C2SPacketHandles/C2SInitPacketHandles.h"
#include "C2SPacketHandles/C2SPlayerActionPacketHandles.h"
#include "C2SPacketHandles/C2SBreakBlockPacketHandles.h"
#include "C2SPacketHandles/C2SCommonDataPacketHandles.h"

#include "Network/protocol/AutoRegisterPacketC2S.h"

#include "Network/C2SPacketDispatcher.h"
#include "Core.h"
#include "Object/Root.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

#define MAX_DEFER_LOGIN_TIME 5000 // milliseconds
#define MAX_SERVER_NO_PACKET_RECVED_TIME 3600000 // milliseconds = 1 hour
#define MAX_SERVER_ASK_TO_QUIT_NO_PACKET_RECVED_TIME 10000 // milliseconds = 10 seconds

using namespace std::placeholders;
ServerNetworkRecver::ServerNetworkRecver()
{

	AutoRegisterC2S c2s;
	c2s.autoRegister<C2SPacketType::ProtocolEnd>();
	m_packetDispatcher = LORD::make_shared<C2SPacketDispatcher>();

	m_packetDispatcher->setPreHandle(std::bind(&ServerNetworkRecver::onPreHandlePacket, this, _1, _2));

	m_packetDispatcher->registerClass<C2SInitPacketHandles>();
	m_packetDispatcher->registerClass<C2SPlayerActionPacketHandles>();
	m_packetDispatcher->registerClass<C2SBreakBlockPacketHandles>();
	m_packetDispatcher->registerClass<C2SCommonDataPacketHandles>();

	ServerCommonDataHelper().init();
}

ServerNetworkRecver::~ServerNetworkRecver()
{

}

void ServerNetworkRecver::recvPacket(std::shared_ptr<DataPacket>& packet)
{
	this->m_waitHandlerPacket.enqueue(packet);
}


bool ServerNetworkRecver::onPreHandlePacket(std::shared_ptr<ClientPeer> &peer, std::shared_ptr<DataPacket>&packet)
{
	
	if (!peer) {
		return false;
	}
	
	if (peer && peer->getEntityPlayer() == nullptr
		&& packet->pid() != static_cast<short>(C2SPacketType::Login)
		&& packet->pid() != static_cast<short>(C2SPacketType::Rebirth)) 
	{
		return false;
	}

	return true;
}

void ServerNetworkRecver::setAskToQuit()
{
	// for server quit
	if (m_lastHaveHandlePacketTime == 0) {
		m_lastHaveHandlePacketTime = LORD::Root::Instance()->getCurrentTime();
	}
	m_askToQuit = true;
}

void ServerNetworkRecver::handleConnectEvent(const RaknetInstance::ConnectEvent& connEvent)
{

	ui64 id = connEvent.rakssid;
	switch (connEvent.type)
	{
	case RaknetInstance::emNewConnect:
	{
		BLOCKMAN::ServerNetworkConnSuccEvent::emit(id, connEvent.clientIp);
		LordLogInfo("_createNewSession_sessionid_" U64FMT, id);
	}
	break;

	case RaknetInstance::emConnectKickOut:
	case RaknetInstance::emDisconnect:
	{
		BLOCKMAN::ServerNetworkDisconnectEvent::emit(id);
		LordLogInfo("_disconnectSession_sessionid_" U64FMT, id);
	}
	break;

	case RaknetInstance::emConnectTimeout:
	{
		BLOCKMAN::ServerNetworkTimeoutEvent::emit(id);
		LordLogInfo("_timeoutSession_sessionid_" U64FMT, id);
	}
	break;

	default:
		LordLogError("handleConnectEvent RaknetInstance::other error with session id=" U64FMT, id);
		break;
	}

}

void ServerNetworkRecver::handlePacket()
{
	std::shared_ptr<DataPacket> ptr;
	int times = 0;
	int startHandlePacketTime = LORD::Root::Instance()->getCurrentTime();
	while(m_waitHandlerPacket.try_dequeue(ptr))
	{
		if (ptr->isConnEvent())
			handleConnectEvent(std::dynamic_pointer_cast<RaknetInstance::ConnEventPacket>(ptr)->connEvent);
		else
			m_packetDispatcher->handlePacket(ptr);
		times++;

		// The processing time of a packet in a frame cannot exceed 500 milliseconds
		if (LORD::Root::Instance()->getCurrentTime() - startHandlePacketTime >= 500)
			break;
	}

	LORD::ui32 currentTime = LORD::Root::Instance()->getCurrentTime();
#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
	if (times > 0)
	{
		m_lastHaveHandlePacketTime = currentTime;
	}
	else
	{
		if (m_lastHaveHandlePacketTime != 0) {
			ui32 passedTime = currentTime - m_lastHaveHandlePacketTime;
			if (passedTime > MAX_SERVER_NO_PACKET_RECVED_TIME
				|| (m_askToQuit && passedTime > MAX_SERVER_ASK_TO_QUIT_NO_PACKET_RECVED_TIME))
			{
				Server::Instance()->stop();
				return;
			}
		}
	}
#endif

	if (!m_deferLoginQueue.empty()) {
		while (!m_deferLoginQueue.empty()) {
			auto dl = m_deferLoginQueue.front();
			if (currentTime - dl.recvTime < MAX_DEFER_LOGIN_TIME) {
				break;
			}

			if (m_deferLoginPlatformUserIds.find(dl.platformUserId) != m_deferLoginPlatformUserIds.end()) {
				m_packetDispatcher->handlePacket(dl.loginPacket);
			}
			
			m_deferLoginQueue.pop();
			m_deferLoginPlatformUserIds.erase(dl.platformUserId);
		}
	}
}

void ServerNetworkRecver::deferLoginProcess(ui64 platformUserId, std::shared_ptr<DataPacket> packet) {
	ServerNetworkRecver::DeferLogin dl;
	dl.loginPacket = packet;
	dl.recvTime = LORD::Root::Instance()->getCurrentTime();
	dl.platformUserId = platformUserId;

	m_deferLoginPlatformUserIds.insert(platformUserId);
	m_deferLoginQueue.push(dl);
}

bool ServerNetworkRecver::hasBeenDeferredLogin(ui64 platformUserId) {
	if (m_deferLoginPlatformUserIds.find(platformUserId) == m_deferLoginPlatformUserIds.end()) {
		return false;
	}
	else {
		return true;
	}
}

void ServerNetworkRecver::clearDeferLogin(ui64 platformUserId) {
	m_deferLoginPlatformUserIds.erase(platformUserId);
}

void ServerNetworkRecver::recvTick()
{
	this->handlePacket();
}
