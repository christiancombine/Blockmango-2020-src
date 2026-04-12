#include "ServerNetwork.h"
#include "Network/protocol/AutoRegisterPacketS2C.h"
#include "Network/protocol/S2CPackets.h"

void ServerNetwork::registerPacket100(AutoRegisterS2C& pRegister) 
{
	pRegister.autoRegister<S2CPacketType::ProtocolBegin, S2CPacketType(100)>();
}