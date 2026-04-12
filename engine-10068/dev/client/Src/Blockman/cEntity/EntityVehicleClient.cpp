#include "EntityVehicleClient.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{
	void EntityVehicleClient::syncInputs()
	{
		auto player = Blockman::Instance()->m_pPlayer;
		if (!player || !isDriver(player->entityId))
		{
			return;
		}
		if (getInput() == m_lastInput)
		{
			return;
		}
		m_lastInput = getInput();
		ClientNetwork::Instance()->getSender()->syncVehicleInput(this);
	}

	void EntityVehicleClient::syncPositionAndRotation()
	{
		auto player = Blockman::Instance()->m_pPlayer;
		if (!player || !isDriver(player->entityId))
		{
			return;
		}
		m_moveTick = ++m_moveTick % 100;
		if (m_moveTick != 0
			&& (position - m_lastSyncPos).lenSqr() < 0.25
			&& std::abs(rotationYaw - m_lastSyncYaw) < 10)
		{
			return;
		}

		ClientNetwork::Instance()->getSender()->sendVehicleMovement(this);
		m_moveTick = 0;
		m_lastSyncPos = position;
		m_lastSyncYaw = rotationYaw;
	}

	void EntityVehicleClient::syncVehicleHorn()
	{
		auto player = Blockman::Instance()->m_pPlayer;
		if (!player)
		{
			return;
		}
		ClientNetwork::Instance()->getSender()->sendVehicleHornSoundSource(position);
	}
}