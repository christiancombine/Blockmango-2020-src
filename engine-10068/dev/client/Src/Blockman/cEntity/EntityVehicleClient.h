#pragma once
#include "Entity/EntityVehicle.h"

namespace BLOCKMAN
{
	class EntityVehicleClient : public EntityVehicle
	{
		RTTI_DECLARE(EntityVehicleClient);
		using EntityVehicle::EntityVehicle;
	private:
		unsigned m_moveTick = 0;
		VehicleInputData m_lastInput;
		Vector3 m_lastSyncPos = Vector3::ZERO;
		float m_lastSyncYaw = 0;

	private:
		void syncInputs() override;
		void syncPositionAndRotation() override;
		void syncVehicleHorn() override;
	};
}