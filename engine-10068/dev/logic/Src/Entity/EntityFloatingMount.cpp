#include "EntityFloatingMount.h"
#include "Setting/MountSetting.h"
#include "World/World.h"
#include "Entity/EntityPlayer.h"
#include "Audio/SoundSystem.h"

namespace BLOCKMAN
{
	EntityFloatingMount::EntityFloatingMount(World* pWorld, Vector3 pos, int settingId)
		: Entity(pWorld)
	{
		this->setPosition(pos);
		setting = MountSetting::getSetting(settingId);
		setSize(setting->size.x, setting->size.z, setting->size.y);
		ignoreFrustumCheck = true;
		stepHeight = 0;
		yOffset = -setting->yOffset;
		if (world->m_isClient && setting->spawnSoundId != (int)SoundType::ST_Invalid)
			SoundSystem::Instance()->playEffectByType((SoundType)setting->spawnSoundId, this->getPosition());
		waitPlaySoundTickCount = 100;//5s
	}

	void EntityFloatingMount::playTakingSound()
	{
		if (world->m_isClient && setting->takingSoundId != (int)SoundType::ST_Invalid)
			takingSoundId = SoundSystem::Instance()->playEffectByType((SoundType)setting->takingSoundId, this->getPosition());
		else
			takingSoundId = -1;
	}

	EntityPlayer* EntityFloatingMount::getRider() const
	{
		return dynamic_cast<EntityPlayer*>(world->getEntity(riderId));
	}
	
	bool EntityFloatingMount::attackEntityFrom(DamageSource * pSource, float amount)
	{
		auto rider = world->getEntity(riderId);
		if (rider)
		{
			rider->attackEntityFrom(pSource, amount);
		}
		return false;
	}

	void EntityFloatingMount::handleMouseInput(float dx, float dy)
	{
		auto player = dynamic_cast<EntityPlayer*>(world->getEntity(riderId));
		if (!player)
			return;
		player->handleMouseInput(dx, dy);
		rotationPitch = player->rotationPitch;
		auto oldPlayerYaw = player->rotationYaw;
		auto angleDiff = Math::WrapDegree(rotationYaw - player->rotationYaw);
		auto maxAngleDiff = setting->maxCameraBodyAngle * Math::RAD2DEG;
		if (angleDiff < -maxAngleDiff)
			player->rotationYaw = rotationYaw + maxAngleDiff;
		else if (angleDiff > maxAngleDiff)
			player->rotationYaw = rotationYaw - maxAngleDiff;
		player->prevRotationYaw += player->rotationYaw - oldPlayerYaw;
	}

	void EntityFloatingMount::setDead()
	{
		if (takingSoundId != -1)
			SoundSystem::Instance()->stopEffect(takingSoundId);

		if (world->m_isClient && setting->deadSoundId != (int)SoundType::ST_Invalid)
			SoundSystem::Instance()->playEffectByType((SoundType)setting->deadSoundId, this->getPosition());
		Entity::setDead();
	}

	void EntityFloatingMount::onUpdate()
	{
		if (takingSoundId == 0)
		{
			waitPlaySoundTickCount--;
			if (waitPlaySoundTickCount == 0)
			{
				playTakingSound();
			}
		}
		else if(takingSoundId > 0)
		{
			SoundSystem::Instance()->updateSoundPos(takingSoundId, getPosition());
		}
		prevRotationYaw = rotationYaw;
		prevRotationPitch = rotationPitch;
		prevPos = position;
		auto player = dynamic_cast<EntityPlayer*>(world->getEntity(riderId));
		if (!player)
			return;
		auto angleDiff = Math::WrapDegree(rotationYaw - player->rotationYaw);
		auto turnSpeed = setting->turnSpeed * Math::RAD2DEG;
		if (angleDiff < -turnSpeed)
			rotationYaw += turnSpeed;
		else if (angleDiff > turnSpeed)
			rotationYaw -= turnSpeed;
		else
			rotationYaw = player->rotationYaw;

		auto pitch = player->rotationPitch * Math::DEG2RAD;
		auto yaw = rotationYaw * Math::DEG2RAD;
		Vector3 direction;
		direction.y = -Math::Sin(pitch);
		auto projection = Math::Cos(pitch);
		direction.z = Math::Cos(yaw) * projection;
		direction.x = -Math::Sin(yaw) * projection;
		moveEntity(direction * setting->moveSpeed);

		syncMovement();
		if (EntityPlayer* rider = world->getPlayerEntity(riderId))
		{
			Vector3 offset(0, yOffset, 0);
			rider->prevPos = prevPos + offset;
			rider->setPosition(position + offset);
			rider->lastTickPos = lastTickPos;
			rider->prevRenderYawOffset = prevRotationYaw;
			rider->renderYawOffset = rotationYaw;
		}
	}

	void EntityFloatingMount::whenRemoved()
	{
		if (auto player = dynamic_cast<EntityPlayer*>(world->getEntity(riderId)))
		{
			player->getOffFloatingMount();
		}
	}
}