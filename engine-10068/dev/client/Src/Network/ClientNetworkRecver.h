#ifndef __CLIENTNETWORK_RECVER_H__
#define __CLIENTNETWORK_RECVER_H__
#include "Util/Concurrent/ThreadSafeQueue.h"
#include "Core.h"

using namespace  LORD;

class S2CPacketDispatch;
class AutoRegisterS2C;
class DataPacket;
class ClientNetworkCore;
class ClientNetworkRecver:public ObjectAlloc
{
public:

public:
	ClientNetworkRecver(std::shared_ptr<ClientNetworkCore> & netCore);
	virtual ~ClientNetworkRecver(void) {};
	
	void initInner();
	void registerPacket100(AutoRegisterS2C *pRegister);
	void registerPacket200(AutoRegisterS2C *pRegister);
	void registerPacketEnd(AutoRegisterS2C *pRegister);
	void recvTick();
	void recvPacket(std::shared_ptr<DataPacket> &packet);

private:
	
	void handlePacket();
private:
	std::shared_ptr<S2CPacketDispatch> m_packetDispatch;
	std::shared_ptr<ClientNetworkCore> m_networkCore;
	ThreadSafeQueue<std::shared_ptr<DataPacket>> m_waitHandlerPacket;

};


#endif
