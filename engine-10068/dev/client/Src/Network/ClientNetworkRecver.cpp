#include "ClientNetworkRecver.h"
#include "Network/protocol/AutoRegisterPacketS2C.h"
#include "Network/S2CPacketDispatch.h"
#include "game.h"

ClientNetworkRecver::ClientNetworkRecver(std::shared_ptr<ClientNetworkCore> & netCore)
{
	m_networkCore = netCore;
	AutoRegisterS2C s2c;
	this->registerPacket100(&s2c);
	this->registerPacket200(&s2c);
	this->registerPacketEnd(&s2c);
    m_packetDispatch = LORD::make_shared<S2CPacketDispatch>();
	this->initInner();

}

void ClientNetworkRecver::recvPacket(std::shared_ptr<DataPacket> &packet)
{
	this->m_waitHandlerPacket.push(packet);
}

void ClientNetworkRecver::handlePacket()
{
	std::shared_ptr<DataPacket> ptr;
	while(m_waitHandlerPacket.try_pop(ptr))
	{
		if(ptr)
		{
			if (ptr->isConnEvent())
			{
				this->m_networkCore->handleConnectEvent(std::dynamic_pointer_cast<RaknetInstance::ConnEventPacket>(ptr)->connEvent);
				if (m_networkCore->getStatus() >= ClientNetworkCore::emConnectFailed)
					break;
			}
			else
			{
				m_packetDispatch->handlePacket(ptr);
			}
		}
	}
}


void ClientNetworkRecver::recvTick()
{
	if (!GameClient::CGame::Instance()->isEnterBack())
	{
		this->handlePacket();
	}
}