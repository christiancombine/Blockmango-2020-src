#ifndef __S2C_BLOCK_CITY_PACKET_HANDLES_H__
#define __S2C_BLOCK_CITY_PACKET_HANDLES_H__
#include "Network/protocol/S2CPackets.h"

class S2CBlockCityPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityManor>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityPlayerInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityStore>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityManorInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityBuyLackItems>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowBlockCityCommonTip>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketuUpdateBlockCityItems>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityHouseShop>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityPackTab>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityManorArea>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityElevatorArea>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityWeekReward>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockCityArchive>& packet);
};

#endif
