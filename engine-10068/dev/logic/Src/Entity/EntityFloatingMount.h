#pragma once
#include "Core.h"
#include "Entity/Entity.h"

namespace BLOCKMAN
{
	class MountSetting;
	class EntityPlayer;

	class EntityFloatingMount : public Entity
	{
		RTTI_DECLARE(EntityFloatingMount);
	private:
		const MountSetting* setting = nullptr;
		int riderId = 0;
		int takingSoundId = 0;
		float speed = 0;
		long waitPlaySoundTickCount = -1;

	public:
		bool isHurting = false;

	public:
		EntityFloatingMount(World* pWorld, Vector3 pos, int settingId);

		virtual bool attackEntityFrom(DamageSource* pSource, float amount);

		const MountSetting& getSetting() const { return *setting; }
		void setRider(const Entity* rider)
		{
			riderId = rider ? rider->entityId : 0;
		}

		int getRiderId() const { return riderId; }
		EntityPlayer* getRider() const;

		void setHurting(bool isHurting) { this->isHurting = isHurting; }
		/**
		 *  /\
		 * y|
		 *  |
		 *   -----> x
		 */
		virtual void handleMouseInput(float dx, float dy) override;
		virtual void setDead() override;
	private:
		void playTakingSound();
		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;
		bool canTriggerWalking() override { return false; }
		void whenRemoved() override;
		virtual void syncMovement() {}
	};
}