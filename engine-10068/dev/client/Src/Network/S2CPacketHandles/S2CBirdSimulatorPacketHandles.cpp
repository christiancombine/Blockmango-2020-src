#include "S2CBirdSimulatorPacketHandles.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Game/BirdSimulator.h"
#include "cWorld/Blockman.h"
#include "Util/ClientEvents.h"
#include "World/World.h"
#include "Audio/SoundSystem.h"

using namespace BLOCKMAN;

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdBag>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setBag(&packet->birdBag);
		UpdateBirdPackEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdStore>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setStores(packet->stores);
		UpdateBirdStoreEvent::emit();
		UpdateBirdPackEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdPersonalStore>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setPersonalStore(packet->personalStore);
		UpdateBirdPersonalShopEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdPlayerInfo>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setPlayerInfo(&packet->playerInfo);
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdDress>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setDress(packet->dress);
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdFood>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setFoods(packet->foods);
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdTask>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setTasks(packet->tasks);
		BirdTaskChangeEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdAtlas>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setAtlas(packet->atlas);
		UpdateBirdAtlasEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdScope>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setScopes(packet->scopes);
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdActivity>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setActivity(&packet->activity);
		UpdateBirdActivityEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketBirdGain>& packet)
{
	LORD::SoundSystem::Instance()->playEffectByType(ST_GetItem);
	ShowBirdGainTipEvent::emit(packet->items);
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketBirdNestOperation>& packet)
{
	if (!packet->isUnlock)
	{
		BirdNestExpandEvent::emit();
	}
	else
	{
		BirdPackOpenEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketBirdLotteryResult>& packet)
{
	ShowBirdLotteryResultTipEvent::emit("g1041_egg.actor", packet->eggBodyId, packet->birdInfo);
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdTimePrice>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setTimePrices(packet->timePrices);
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketBirdAddScore>& packet)
{
	BirdAddScoreEvent::emit(packet->score, packet->scoreType);
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdBuff>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setBuffs(packet->buffs);
		BirdBuffChangeEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdSimulatorStoreItem>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setStoreItem(packet->store);
		UpdateBirdStoreEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdSimulatorTaskItem>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		bool isChange =player->m_birdSimulator->setTaskItem(packet->isRemove, packet->taskItem);
		if (isChange)
		{
			BirdTaskChangeEvent::emit();
		}
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBirdSimulatorPersonStoreTab>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && player->m_birdSimulator)
	{
		player->m_birdSimulator->setPersonalStoreTab(packet->storeTab);
		UpdateBirdPersonalShopEvent::emit();
	}
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketBirdAddMessage>& packet)
{
	BirdAddMessageEvent::emit(packet->message);
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketBirdCheckinData>& packet)
{
	BirdCheckinDataUpdateEvent::emit(packet->data);
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowAddCarryGuide>& packet)
{
	BirdShowPackGuideEvent::emit();
	BirdShowCarryGuideEvent::emit();
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowBirdFeedGuide>& packet)
{
	BirdShowPackGuideEvent::emit();
	BirdShowBirdFeedGuideEvent::emit();
}

void S2CBirdSimulatorPacketHandles::handlePacket(std::shared_ptr<S2CPacketBirdShowWingTip>& packet)
{
	ShowBirdWingTipEvent::emit(packet->page);
}