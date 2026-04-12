#include "C2SCommonDataPacketHandles.h"
#include "Network/CommonDataDispatch.h"
#include "Network/ClientPeer.h"
#include "Network/ServerNetwork.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Script/GameServerEvents.h"
#include "Entity/EntityDecoration.h"

void ServerCommonDataHelper::init()
{
	CommonDataDispatch::Instance()->unregisterAll();
	CommonDataDispatch::Instance()->registerHandle("Lua", &C2SCommonDataPacketHandles::onLuaCommonEvent);
	CommonDataDispatch::Instance()->registerHandle("PlayerOperation", &C2SCommonDataPacketHandles::onPlayerOperationEvent);
	CommonDataDispatch::Instance()->registerHandle("BlockCityStorageOperation", &C2SCommonDataPacketHandles::onBlockCityStorageOperation);
	CommonDataDispatch::Instance()->registerHandle("InteractionWithDecoration", &C2SCommonDataPacketHandles::onInteractionWithDecoration);
	CommonDataDispatch::Instance()->registerHandle("OpenMovingEnderChest", &C2SCommonDataPacketHandles::onPlayerOpenMovingEnderChest);
}

void C2SCommonDataPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketClientCommonData>& packet)
{
	CommonDataDispatch::Instance()->handleCommonData(clientPeer->getPlatformUserId(), packet->name, packet->key, packet->data);
}

void C2SCommonDataPacketHandles::onLuaCommonEvent(ui64 userId, String & name, String & key, String & data)
{
	SCRIPT_EVENT::ClientCommonDataEvent::invoke(userId, key, data);
}

void C2SCommonDataPacketHandles::onInteractionWithDecoration(ui64 userId, String& name, String& key, String& data)
{
	auto build = CommonDataBuilder().fromData(data);
	i32 entityId = build.getIntParam("entityId");
	String node = build.getStringParam("node");

	auto player = Server::Instance()->getWorld()->getPlayerByPlatformUserId(userId);
	if (player)
	{
		auto entity = player->world->getEntity(entityId);
		auto entityDecoration = dynamic_cast<EntityDecoration*>(entity);
		if (!entityDecoration)
			return;
		if (player->isOnDecoration())
		{
			player->takeOffDecoration();
		}
		else
		{
			player->takeOnDecoration(node, entityDecoration);
			SCRIPT_EVENT::PlayerTakeOnDecorationEvent::invoke(userId, entityId);
		}
		ServerNetwork::Instance()->getSender()->syncEntityDecoration(0, entityDecoration);
		ServerNetwork::Instance()->getSender()->syncTakeDecoration(0, player->hashCode(), player->getDecorationEntity() ? player->getDecorationEntity()->hashCode() : 0);
	}
}

void C2SCommonDataPacketHandles::onBlockCityStorageOperation(ui64 userId, String& name, String& key, String& data)
{
	auto build = CommonDataBuilder().fromData(data);
	bool isAdd = build.getBoolParam("isAdd");
	i32 itemId = build.getIntParam("itemId");
	i32 meta = build.getIntParam("meta");
	i32 hotbarIndex = build.getIntParam("hotbarIndex");
	bool isEditMode = build.getBoolParam("isEditMode");

	if (SCRIPT_EVENT::BlockCityStorageOperationEvent::invoke(userId, isAdd, itemId, meta, hotbarIndex, isEditMode))
	{
		auto clientPeer = Server::Instance()->getNetwork()->findPlayerByPlatformUserId(userId);
		if (clientPeer)
		{
			ServerNetwork::Instance()->getSender()->sendPlayerInventory(clientPeer->getRakssid(), clientPeer->getEntityPlayer()->inventory);
			ServerNetwork::Instance()->getCommonSender()->sendBlockCityStorageOperation(clientPeer->getRakssid(), hotbarIndex, itemId, meta, isEditMode);
			clientPeer->getEntityPlayer()->m_inventoryChanged = false;
		}
	}
}

void C2SCommonDataPacketHandles::onPlayerOperationEvent(ui64 userId, String& name, String& key, String& data)
{
	auto build = CommonDataBuilder().fromData(data);
	int type = build.getIntParam("type");
	i64 targetId = build.getI64Param("userId");
	String picUrl = build.getStringParam("url");
	bool isFriend = build.getBoolParam("isFriend");
	int from = build.getIntParam("from");

	//type 5:invite - 6:ratify -  7:unratify - 8:remove out - 9:race
	switch (type)
	{
	case 8:
		auto player = Server::Instance()->getWorld()->getPlayerByPlatformUserId(targetId);
		if (player && player->isOnDecoration() && player->getDecorationEntity())
		{
			auto entityDecoration = player->getDecorationEntity();
			player->takeOffDecoration();
			ServerNetwork::Instance()->getSender()->syncEntityDecoration(0, entityDecoration);
			ServerNetwork::Instance()->getSender()->syncTakeDecoration(0, player->hashCode(), 0);
		}
		break;
	}
	SCRIPT_EVENT::PlayerOperationEvent::invoke(userId, targetId, type, picUrl, isFriend, from);
}

void C2SCommonDataPacketHandles::onPlayerOpenMovingEnderChest(ui64 userId, String& name, String& key, String& data)
{
	auto clientPeer = Server::Instance()->getNetwork()->findPlayerByPlatformUserId(userId);
	if (SCRIPT_EVENT::OpenMovingEnderChestEvent::invoke(clientPeer->getRakssid()))
	{
		EntityPlayer* m_pPlayer = clientPeer->getEntityPlayer();
		IInventory* pInventory = (IInventory*)m_pPlayer->getInventoryEnderChest();
		if (pInventory != NULL)
		{
			ServerNetwork::Instance()->getSender()->sendChestInventory(m_pPlayer, m_pPlayer->getPosition().getFloor(), 1, Vector3::ZERO, pInventory);
		}
		ServerNetwork::Instance()->getCommonSender()->sendOpenMovingEnderChest(clientPeer->getRakssid());
	}
}
