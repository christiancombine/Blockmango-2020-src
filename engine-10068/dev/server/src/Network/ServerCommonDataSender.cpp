#include "ServerCommonDataSender.h"
#include "Server.h"
#include "Network/protocol/S2CPackets3.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Network/ServerNetwork.h"
#include "Blockman/Entity/EntityTracker.h"
#include "Blockman/Entity/EntityPlayerMP.h"

using namespace LORD;

void ServerCommonDataSender::sendServerCommonData(ui64 rakssId, String name, String key, String data)
{
	auto packet = LORD::make_shared<S2CPacketServerCommonData>();
	packet->name = name;
	packet->key = key;
	packet->data = data;
	ServerNetwork::Instance()->sendPacket(packet, rakssId);
}

void ServerCommonDataSender::broadCastServerCommonData(String name, String key, String data)
{
	auto packet = LORD::make_shared<S2CPacketServerCommonData>();
	packet->name = name;
	packet->key = key;
	packet->data = data;
	ServerNetwork::Instance()->broadCastEntityPacket(packet);
}

void ServerCommonDataSender::sendServerCommonDataToTrackingPlayers(int entityId, String name, String key, String data)
{
	auto network = Server::Instance()->getNetwork();
	const auto& targetPlayers = EntityTracker::Instance()->getTrackingPlayersOf(entityId);
	for (const auto& pair : targetPlayers)
	{
		sendServerCommonData(dynamic_cast<EntityPlayerMP*>(pair.second)->getRaknetID(), name, key, data);
	}
}

void ServerCommonDataSender::sendBlockCityStorageOperation(ui64 rakssId, i32 hotbarIndex, i32 itemId, i32 meta, bool isEditMode)
{
	String data = CommonDataBuilder().addIntParam("hotbarIndex", hotbarIndex)
		.addIntParam("itemId", itemId)
		.addIntParam("meta", meta)
		.addBoolParam("isEditMode", isEditMode).getData();
	sendServerCommonData(rakssId, "BlockCityStorageOperation", "", data);
}

void ServerCommonDataSender::sendPlayingPraiseAction(ui64 rakssId, i32 entityId, int duration)
{
	String data = CommonDataBuilder().addIntParam("entityId", entityId)
		.addIntParam("duration", duration).getData();
	sendServerCommonData(rakssId, "PlayingPraiseAction", "", data);
}

void ServerCommonDataSender::sendOpenMovingEnderChest(ui64 rakssId)
{
	sendServerCommonData(rakssId, "OpenMovingEnderChest");
}
