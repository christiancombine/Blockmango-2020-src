#ifndef __CLIENTNETWORKCORE_H__
#define __CLIENTNETWORKCORE_H__
#include "Network/RaknetInstance.h"
#include "Util/Concurrent/ThreadSafeQueue.h"
#include "Core.h"

class ClientNetworkCore: public ObjectAlloc
{
public:
	enum NetStatus
	{
		emConncting,
		emConnected,
		emLoginSuc,
		emConnectFailed,
		emDisconnect,
		emLogout,
		emRetryConnect,
	};

public:
	ClientNetworkCore();
	virtual ~ClientNetworkCore(void);
	void connect(const char *ip, ui16 port);
	void tearDown();
	void sendPacket(std::shared_ptr<DataPacket> packet) { m_rakinstance->send(packet, m_rakssid);}

	void logout() { m_status = emLogout; };
	void flush() { m_rakinstance->flush(); };
	void closeSession(ui64 ssid) { m_rakinstance->closeSession(ssid); };
	bool isConnected(){return m_status == emConnected || m_status == emLoginSuc;}
	NetStatus getStatus(){return m_status;}
	void setStatus(NetStatus status) { m_status = status; }
	bool isAlive() { return isConnected() && m_rakinstance->isAlive(); }
	std::shared_ptr<RaknetInstance> getRaknetInstance() { return m_rakinstance; }
	int getRaknetPing();
	void handleConnectEvent(const RaknetInstance::ConnectEvent &connEvent);

private:
	char  m_hostIP[20] = { 0 };
	short m_hostPort = 0;

	NetStatus  m_status = emDisconnect;

	ui64 m_rakssid = 0;

	std::shared_ptr<RaknetInstance> m_rakinstance;

	ui32 startRetryConnectTime = 0;
	ui32 maxRetryConnectTime = 10000;
	bool isRetryConnectChangeIp = false;

	ThreadSafeQueue<RaknetInstance::ConnectEvent> m_waitHandleConnEvent;
	bool m_destroyed = false;
};

#endif
