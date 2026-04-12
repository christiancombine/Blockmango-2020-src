#pragma once
#include "Entity/EntityFloatingMount.h"
#include "cWorld/Blockman.h"
#include "Network/ClientPacketSender.h"
#include "Network/ClientNetwork.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{
	class EntityFloatingMountClient : public EntityFloatingMount
	{
		using EntityFloatingMount::EntityFloatingMount;

	private:
		virtual void syncMovement() override
		{
			auto player = Blockman::Instance()->m_pPlayer;
			if (player->entityId == getRiderId())
			{
				ClientNetwork::Instance()->getSender()->sendFloatingMountMovement(
					entityId, position, rotationYaw, player->rotationYaw, player->rotationPitch);
			}
		}
	};
}