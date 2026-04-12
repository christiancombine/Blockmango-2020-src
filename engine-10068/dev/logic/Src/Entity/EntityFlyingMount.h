#pragma once
#include "Core.h"
#include "Entity/Entity.h"

namespace BLOCKMAN
{
	class MountSetting;

	class EntityFlyingMount : public Entity
	{
		RTTI_DECLARE(EntityFlyingMount);
	private:
		const MountSetting* setting = nullptr;
		LORD::Quaternion prevViewOrientation = { 1, 0, 0, 0 };
		LORD::Quaternion viewOrientation = { 1, 0, 0, 0 };
		LORD::Quaternion prevOrientation = { 1, 0, 0, 0 };
		LORD::Quaternion orientation = { 1, 0, 0, 0 };
		int riderId = 0;
		int takingSoundId = 0;
		float speed = 0;
		unsigned waitReturnTickCount = 0;
		long waitPlaySoundTickCount = -1;

	public:
		bool isHurting = false;

	public:
		EntityFlyingMount(World* pWorld, Vector3 pos, int settingId);

		virtual bool attackEntityFrom(DamageSource* pSource, float amount);
		const MountSetting& getSetting() const { return *setting; }
		void setRider(const Entity* rider)
		{
			riderId = rider ? rider->entityId : 0;
		}
		EntityPlayer* getRider() const;
		int getRiderId() const { return riderId; }

		void setHurting(bool isHurting) { this->isHurting = isHurting; }

		/**
		 *  /\
		 * y|
		 *  |
		 *   -----> x
		 */
		virtual void handleMouseInput(float dx, float dy) override;
		virtual void setDead() override;
		void setOrientation(const LORD::Quaternion& newOrientation) { orientation = newOrientation; }
		const LORD::Quaternion& getOrientation() const { return orientation; }
		const LORD::Quaternion& getPrevOrientation() const { return prevOrientation; }
		void setViewOrientation(const LORD::Quaternion& newViewOrientation) { viewOrientation = newViewOrientation; }
		const LORD::Quaternion& getViewOrientation() const { return viewOrientation; }
		LORD::Quaternion getCurrentViewOrientation(float rdt) const;

		bool isForceSyncSpawn() { return true; }

	private:
		void playTakingSound();
		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override; 
		bool canTriggerWalking() override { return false; }
		void whenRemoved() override;

		void straightenOrientation();
		virtual void syncMovement() {}
	};
}