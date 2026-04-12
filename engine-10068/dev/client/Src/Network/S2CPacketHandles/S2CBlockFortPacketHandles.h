#ifndef __S2C_BLOCK_FORT_PACKET_HANDLES_H__
#define __S2C_BLOCK_FORT_PACKET_HANDLES_H__
#include "Network/protocol/S2CPackets.h"

class S2CBlockFortPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockFortManorInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockFortManorManager>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockFortPlayerInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockFortStore>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketuUpdateBlockFortItems>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketuShowBlockFortBuildingInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowFloatTip>& packet);
};

#endif
