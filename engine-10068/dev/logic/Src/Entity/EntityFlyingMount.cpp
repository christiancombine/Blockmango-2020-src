#include "EntityFlyingMount.h"
#include "World/World.h"
#include "Entity/EntityPlayer.h"
#include "Setting/MountSetting.h"
#include "Audio/SoundSystem.h"

using namespace LORD;

namespace BLOCKMAN
{
	EntityFlyingMount::EntityFlyingMount(World* pWorld, Vector3 pos, int settingId)
		: Entity(pWorld)
	{
		this->setPosition(pos);	
		setting = MountSetting::getSetting(settingId);
		setSize(setting->size.x, setting->size.z, setting->size.y);
		ignoreFrustumCheck = true;
		stepHeight = 0;
		if (world->m_isClient && setting->spawnSoundId != (int)SoundType::ST_Invalid)
			SoundSystem::Instance()->playEffectByType((SoundType)setting->spawnSoundId, this->getPosition());
		waitPlaySoundTickCount = 100;//5s
	}

	void EntityFlyingMount::playTakingSound()
	{
		if (world->m_isClient && setting->takingSoundId != (int)SoundType::ST_Invalid)
			takingSoundId = SoundSystem::Instance()->playEffectByType((SoundType)setting->takingSoundId, this->getPosition());
		else
			takingSoundId = -1;
	}

	bool EntityFlyingMount::attackEntityFrom(DamageSource* pSource, float amount)
	{
		auto rider = world->getEntity(riderId);
		if (rider)
		{
			rider->attackEntityFrom(pSource, amount);
		}
		return false;
	}

	EntityPlayer* EntityFlyingMount::getRider() const
	{
		return dynamic_cast<EntityPlayer*>(world->getEntity(riderId));
	}

	void EntityFlyingMount::handleMouseInput(float dx, float dy)
	{
		Vector3 rotateAxis(-dy, -dx, 0);
		rotateAxis.normalize();
		Quaternion rotation;
		float len = sqrt(dx * dx + dy * dy);
		rotation.rotateAxis(rotateAxis, len * setting->sensitivity);
		viewOrientation = viewOrientation * rotation;

		auto bodyLookAxis = orientation.zAxis();
		auto cameraLookAxis = viewOrientation.zAxis();
		if (bodyLookAxis.dot(cameraLookAxis) < std::cos(setting->maxCameraBodyAngle))
		{
			auto rotateAxis = bodyLookAxis.cross(cameraLookAxis).normalizedCopy();
			viewOrientation = Quaternion(rotateAxis, setting->maxCameraBodyAngle) * orientation;
		}
	}

	void EntityFlyingMount::setDead()
	{
		if (takingSoundId != -1)
			SoundSystem::Instance()->stopEffect(takingSoundId);

		if (world->m_isClient && setting->deadSoundId != (int)SoundType::ST_Invalid)
			SoundSystem::Instance()->playEffectByType((SoundType)setting->deadSoundId, this->getPosition());
		Entity::setDead();
	}

	LORD::Quaternion EntityFlyingMount::getCurrentViewOrientation(float rdt) const
	{
		Quaternion interpolatedOrientation;
		Quaternion::Slerp(interpolatedOrientation, prevViewOrientation, viewOrientation, rdt, true);
		return interpolatedOrientation;
	}

	void EntityFlyingMount::onUpdate()
	{
		Entity::onUpdate();
		if (takingSoundId == 0)
		{
			waitPlaySoundTickCount--;
			if (waitPlaySoundTickCount == 0)
			{
				playTakingSound();
			}
		}
		else if (takingSoundId > 0)
		{
			SoundSystem::Instance()->updateSoundPos(takingSoundId, getPosition());
		}
		prevOrientation = orientation;
		prevViewOrientation = viewOrientation;
		Quaternion::SlerpWithAngle(orientation, orientation, viewOrientation, setting->turnSpeed);
		moveEntity(orientation * Vector3(0, 0, setting->moveSpeed));

		if (viewOrientation != orientation)
		{
			waitReturnTickCount = 0;
		}
		if (++waitReturnTickCount > setting->waitStraightenTime * 20)
		{
			straightenOrientation();
			orientation = viewOrientation;
		}
		syncMovement();
		if (EntityPlayer * rider = world->getPlayerEntity(riderId))
		{
			Vector3 offset(0, yOffset, 0);
			rider->prevPos = prevPos + offset;
			rider->setPosition(position + offset);
			rider->lastTickPos = lastTickPos;
			auto direction = orientation * Vector3(0.0f, 0.0f, 1.0f);
			float yaw = Math::ATan2(-direction.x, direction.z) * Math::RAD2DEG;
			float pitch = Math::ATan2(-direction.y, Math::Sqrt(direction.x * direction.x + direction.z * direction.z)) * Math::RAD2DEG;
			rider->rotationYaw = yaw;
			rider->prevRotationYaw = yaw;
			rider->rotationPitch = pitch;
			rider->prevRotationPitch = pitch;
		}
	}

	void EntityFlyingMount::whenRemoved()
	{
		if (auto player = dynamic_cast<EntityPlayer*>(world->getEntity(riderId)))
		{
			player->getOffFlyingMount();
		}
	}

	void EntityFlyingMount::straightenOrientation()
	{
		auto cameraLookAxis = viewOrientation.zAxis();
		auto cameraSideAxis = viewOrientation.xAxis();
		Vector3 targetSideAxis = Vector3(cameraLookAxis.z, 0, -cameraLookAxis.x).normalizedCopy();
		auto dotProduct = targetSideAxis.dot(cameraSideAxis);
		auto threshold = std::cos(setting->straightenSpeed) - Math::LOWEPSILON;
		if (dotProduct > threshold)
		{
			viewOrientation.fromAxes(targetSideAxis, cameraLookAxis.cross(targetSideAxis), cameraLookAxis);
			return;
		}
		auto rotationAxis = cameraLookAxis;
		constexpr float kSafeCrossThreshold = -0.999;
		if (dotProduct > kSafeCrossThreshold)
		{
			rotationAxis = cameraSideAxis.cross(targetSideAxis).normalizedCopy();
		}
		viewOrientation = Quaternion(rotationAxis, setting->straightenSpeed) * viewOrientation;
	}
}