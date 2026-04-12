#include "S2CBlockFortPacketHandles.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Util/ClientEvents.h"
#include "World/World.h"
#include "Util/LanguageManager.h"

using namespace BLOCKMAN;

void S2CBlockFortPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockFortManorInfo>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockFort)
	{
		player->m_blockFort->setManorInfo(&packet->manorInfo);
	}
}


void S2CBlockFortPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockFortManorManager>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockFort)
	{
		player->m_blockFort->setManorManager(&packet->manorManager);
		BlockFortManorManagerUpdateEvent::emit();
	}
}

void S2CBlockFortPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockFortPlayerInfo>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockFort)
	{
		player->m_blockFort->setPlayerInfo(&packet->playerInfo);
		UpdateBlockFortPlayerInfoEvent::emit();
	}
}

void S2CBlockFortPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockFortStore>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockFort)
	{
		player->m_blockFort->setStores(packet->stores);
		BlockFortStoreUpdateEvent::emit(0);
	}
}

void S2CBlockFortPacketHandles::handlePacket(std::shared_ptr<S2CPacketuUpdateBlockFortItems>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockFort)
	{
		player->m_blockFort->updateItems(packet->items);
		BlockFortStoreUpdateEvent::emit(packet->tabId);
	}
}

void S2CBlockFortPacketHandles::handlePacket(std::shared_ptr<S2CPacketuShowBlockFortBuildingInfo>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockFort)
	{
		ShowBlockFortBuildingInfoEvent::emit(packet->id, packet->index);
	}
}

void S2CBlockFortPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowFloatTip>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockFort)
	{
		ShowFloatTipEvent::emit(LanguageManager::Instance()->getString(packet->msg).c_str());
	}
}