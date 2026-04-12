#include "ClientNetworkRecver.h"
#include "Network/protocol/AutoRegisterPacketS2C.h"
#include "Network/protocol/S2CPackets.h"

void ClientNetworkRecver::registerPacketEnd(AutoRegisterS2C *pRegister)
{
	pRegister->autoRegister<(S2CPacketType)200, S2CPacketType::ProtocolEnd>();
}