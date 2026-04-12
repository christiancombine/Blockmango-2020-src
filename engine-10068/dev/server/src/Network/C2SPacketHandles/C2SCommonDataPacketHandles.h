#pragma once

#ifndef __C2S_COMMON_DATA_PACKET_HANDLES_H__
#define __C2S_COMMON_DATA_PACKET_HANDLES_H__

#include <memory>
#include "Network/protocol/C2SPackets.h"

class ServerCommonDataHelper
{
public:
	void init();
};

class ClientPeer;

class C2SCommonDataPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketClientCommonData>& packet);
	
	static void onLuaCommonEvent(ui64 userId, String& name, String& key, String& data);
	static void onInteractionWithDecoration(ui64 userId, String& name, String& key, String& data);
	static void onBlockCityStorageOperation(ui64 userId, String& name, String& key, String& data);
	static void onPlayerOperationEvent(ui64 userId, String& name, String& key, String& data);
	static void onPlayerOpenMovingEnderChest(ui64 userId, String& name, String& key, String& data);
	//static void onNolongerRemindMsg(ui64 userId, String& name, String& key, String& data);
	//static void onConsumeDiamonds(ui64 userId, String& name, String& key, String& data);
};



#endif
