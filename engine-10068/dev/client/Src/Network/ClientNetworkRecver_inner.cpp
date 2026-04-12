#include "ClientNetworkRecver.h"
#include "Network/S2CPacketDispatch.h"

#include "Network/S2CPacketHandles/S2CChatPacketHandles.h"
#include "Network/S2CPacketHandles/S2CGamePacketHandles.h"
#include "Network/S2CPacketHandles/S2CRanchPacketHandles.h"
#include "Network/S2CPacketHandles/S2CBirdSimulatorPacketHandles.h"
#include "Network/S2CPacketHandles/S2CBlockCityPacketHandles.h"
#include "Network/S2CPacketHandles/S2CEntityPacketHandles.h"
#include "Network/S2CPacketHandles/S2CPlayerActionPacketHandles.h"
#include "Network/S2CPacketHandles/S2CCommonDataPacketHandles.h"
#include "Network/S2CPacketHandles/S2CBlockFortPacketHandles.h"
#include "Network/S2CPacketHandles/S2CInitPacketHandles.h"
#include "Network/S2CPacketHandles/S2CStatusPacketHandles.h"

void ClientNetworkRecver::initInner()
{
	m_packetDispatch->registerClass<S2CChatPacketHandles>();
	m_packetDispatch->registerClass<S2CGamePacketHandles>();
	m_packetDispatch->registerClass<S2CRanchPacketHandles>();
	m_packetDispatch->registerClass<S2CBirdSimulatorPacketHandles>();
	m_packetDispatch->registerClass<S2CBlockCityPacketHandles>();
	m_packetDispatch->registerClass<S2CEntityPacketHandles>();
	m_packetDispatch->registerClass<S2CPlayerActionPacketHandles>();
	m_packetDispatch->registerClass<S2CCommonDataPacketHandles>();
	m_packetDispatch->registerClass<S2CBlockFortPacketHandles>();
	m_packetDispatch->registerClass<S2CInitPacketHandles>();
	m_packetDispatch->registerClass<S2CStatusPacketHandles>();
	ClientCommonDataHelper().init();
}


