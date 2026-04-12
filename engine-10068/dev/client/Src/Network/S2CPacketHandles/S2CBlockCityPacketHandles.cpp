#include "S2CBlockCityPacketHandles.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Util/ClientEvents.h"
#include "World/World.h"

using namespace BLOCKMAN;

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityManor>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setManorManager(&packet->manorManager);
		UpdateBlockCityManorEvent::emit();
	}
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityPlayerInfo>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setPlayerInfo(&packet->playerInfo);
		UpdateBlockCityPlayerInfoEvent::emit();
	}
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityStore>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setStore(packet->stores);
		UpdateBlockCityStoreEvent::emit();
	}
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityManorInfo>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setManorInfo(&packet->manorInfo);
	}
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityBuyLackItems>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setBuyLackItems(&packet->lackItems);
		ShowBlockCityLockItemsEvent::emit();
	}
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowBlockCityCommonTip>& packet)
{
	ShowBlockCityShowCommonTipEvent::emit(packet->pays, packet->type, packet->msg);
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketuUpdateBlockCityItems>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->updateItems(packet->items);
		UpdateBlockCityStoreEvent::emit();
	}
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityHouseShop>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setHouseShop(packet->shops);
		UpdateBlockCityHouseShopEvent::emit();
		UpdateBlockCityPlayerPackEvent::emit();
	}
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityPackTab>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setPackTab(packet->packTab);
		UpdateBlockCityHouseShopEvent::emit();
		UpdateBlockCityPlayerPackEvent::emit();
	}
}
void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityManorArea>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setManorArea(packet->manorAreas);
	}
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityElevatorArea>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setElevatorArea(packet->elevatorAreas);
	}
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityWeekReward>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setWeekReward(packet->weekRewards);
	}
	UpdateBlockCityWeekRewardEvent::emit();
}

void S2CBlockCityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockCityArchive>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_blockCity)
	{
		player->m_blockCity->setArchive(&packet->archive);
		UpdateBlockCityArchiveEvent::emit();
	}
}