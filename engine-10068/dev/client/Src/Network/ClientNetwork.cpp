#include <functional>
#include "ClientNetwork.h"
#include "ClientNetworkCore.h"
#include "Util/ClientEvents.h"
#include "Network/RaknetInstance.h"
#include "Common.h"
#include "Network/protocol/AutoRegisterPacketC2S.h"
#include "Network/protocol/C2SPackets.h"
#include "Network/CommonDataDispatch.h"
#include "Network/ClientNetworkRecver.h"
#include "Util/CPUTimer.h"

ClientNetwork::ClientNetwork()
{
	m_networkCore = LORD::make_shared<ClientNetworkCore>();
	m_subscriptionGuard.add(BLOCKMAN::NetworkConnectionSuccessfulEvent::subscribe(std::bind(&ClientNetwork::onNetworkConnectionSuccessful, this)));

	LordNew(CommonDataDispatch);

	m_packetSender = LORD::make_shared<ClientPacketSender>();
	m_commonDataSender = LORD::make_shared<ClientCommonDataSender>();

	if (DataCache::Instance() == NULL)
		LordNew DataCache;

	std::shared_ptr<CommonPacketSender> pCommon = std::dynamic_pointer_cast<CommonPacketSender>(this->m_packetSender);
	BLOCKMAN::GameCommon::Instance()->setCommonPacketSender(pCommon);

	AutoRegisterC2S c2s;
	c2s.autoRegister<C2SPacketType::ProtocolEnd>();
	m_networkRecver = LORD::make_shared<ClientNetworkRecver>(m_networkCore);
}

ClientNetwork::~ClientNetwork(void)
{
	tearDown();
	m_subscriptionGuard.unsubscribeAll();
	CommonDataDispatch* pDispatch = CommonDataDispatch::Instance();
	LordSafeDelete(pDispatch);
}


void ClientNetwork::connect(const char *ip, ui16 port)
{
	m_networkCore->getRaknetInstance()->recv([this](std::shared_ptr<DataPacket> packet) {m_networkRecver->recvPacket(packet); });
	m_networkCore->connect(ip, port);
}

bool ClientNetwork::onNetworkConnectionSuccessful()
{
	m_packetSender->sendLogin();
	return true;
}

void ClientNetwork::logout()
{
	auto packet = LORD::make_shared<C2SPacketLogout>();
	sendPacket(packet);
	m_networkCore->logout();
}

void ClientNetwork::printColletingPacket()
{
	if (m_networkCore && m_networkCore->getRaknetInstance())
	{
		m_networkCore->getRaknetInstance()->printCollectingPacket();
	}
}

void ClientNetwork::sendPacket(std::shared_ptr<DataPacket> packet)
{
	m_networkCore->sendPacket(packet);
}

void ClientNetwork::handleIncomeData()
{
	PROFILE_CPU("ClientNetwork::handleIncomeData");
	if (m_networkCore->getStatus() >= ClientNetworkCore::emConnectFailed)
		return;

	m_networkRecver->recvTick();
}

int ClientNetwork::getRaknetPing()
{
	return m_networkCore->getRaknetPing();
}
