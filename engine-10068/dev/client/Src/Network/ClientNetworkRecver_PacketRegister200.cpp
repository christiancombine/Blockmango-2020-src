#include "ClientNetworkRecver.h"
#include "Network/protocol/AutoRegisterPacketS2C.h"
#include "Network/protocol/S2CPackets.h"

void ClientNetworkRecver::registerPacket200(AutoRegisterS2C *pRegister)
{
	pRegister->autoRegister<(S2CPacketType)100, (S2CPacketType)200>();
}