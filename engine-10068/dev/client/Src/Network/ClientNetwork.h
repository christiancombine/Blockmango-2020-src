#ifndef __CLIENTNETWORK_H__
#define __CLIENTNETWORK_H__
#include "ClientNetworkCore.h"
#include "Network/protocol/DataPacket.h"
#include "Util/Concurrent/ThreadSafeQueue.h"
#include "Util/SubscriptionGuard.h"
#include "Core.h"
#include "Object/Singleton.h"
#include "ClientPacketSender.h"
#include "Network/DataCache.h"
#include "ClientCommonDataSender.h"

using namespace  LORD;

class S2CPacketDispatch;
class ClientNetworkRecver;
class ClientNetwork:public Singleton<ClientNetwork>, public ObjectAlloc
{
public:

public:
	ClientNetwork();
	virtual ~ClientNetwork(void);
	
	ClientPacketSender* getLuaSender() { return m_packetSender.get(); }
	ClientCommonDataSender* getLuaCommonSender() { return m_commonDataSender.get(); }
	DataCache* getLuaDataCache() { return DataCache::Instance(); }

	void handleIncomeData();
	void sendPacket(std::shared_ptr<DataPacket> packet);
	void connect(const char *ip, ui16 port);
	bool onNetworkConnectionSuccessful();
	void logout();

	void tearDown() { m_networkCore->tearDown(); };
	void flush() { m_networkCore->flush(); };
	void closeSession(ui64 ssid) { m_networkCore->closeSession(ssid); }
	bool isConnected() { return m_networkCore->isConnected(); }
	ClientNetworkCore::NetStatus getStatus(){ return m_networkCore->getStatus(); }
	void setStatus(ClientNetworkCore::NetStatus status) { m_networkCore->setStatus(status); }
	bool isAlive() { return m_networkCore->isAlive(); }
	void printColletingPacket();

	std::shared_ptr<ClientPacketSender>  getSender() { return m_packetSender; }
	std::shared_ptr<ClientCommonDataSender>  getCommonSender() { return m_commonDataSender; }
	DataCache* getDataCache() { return DataCache::Instance(); }

	int getRaknetPing();

private:
	SubscriptionGuard m_subscriptionGuard;

	std::shared_ptr<ClientNetworkCore> m_networkCore;

	std::shared_ptr<ClientNetworkRecver> m_networkRecver;

	std::shared_ptr<ClientPacketSender> m_packetSender;

	std::shared_ptr<ClientCommonDataSender> m_commonDataSender;

};


#endif
