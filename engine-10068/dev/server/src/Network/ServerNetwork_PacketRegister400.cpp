#include "ServerNetwork.h"
#include "Network/protocol/AutoRegisterPacketS2C.h"
#include "Network/protocol/S2CPackets.h"

void ServerNetwork::registerPacket400(AutoRegisterS2C& pRegister) 
{
	pRegister.autoRegister<S2CPacketType(300), S2CPacketType::ProtocolEnd>();
}