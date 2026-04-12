#pragma once
#include "Entity/EntityFlyingMount.h"
#include "Network/ClientPacketSender.h"
#include "Network/ClientNetwork.h"

namespace BLOCKMAN
{
	class EntityFlyingMountClient : public EntityFlyingMount
	{
		using EntityFlyingMount::EntityFlyingMount;
	private:
		virtual void syncMovement()
		{
			if (Blockman::Instance()->m_pPlayer->entityId == getRiderId())
			{
				ClientNetwork::Instance()->getSender()->sendFlyingMountMovement(
					entityId, getPosition(), getOrientation(), getViewOrientation());
			}
		}
	};
}