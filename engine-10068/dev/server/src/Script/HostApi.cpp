#include "HostApi.h"

void HostApi::addMonsterSetting(MonsterSetting setting)
{
    LogicSetting::Instance()->addMonsterSetting(setting);
}

void HostApi::doThrow(int entityId, const Vector3 & launchPoint, float rotationYaw, float rotationPitch, float speed, int remainingFlyTicks)
{
	auto entity = Server::Instance()->getWorld()->getEntity(entityId);
	NULL_RETURN_VOID(entity);

	entity->setPositionAndRotation(launchPoint, rotationYaw, rotationPitch);
	entity->onGround = false;
	entity->m_isPulled = true;
	entity->motion.x = -float(Math::Sin(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD)) * speed;
	entity->motion.z = float(Math::Cos(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD)) * speed;
	entity->motion.y = -float(Math::Sin(double(rotationPitch) * Math::DEG2RAD)) * speed;
	entity->m_remainingFlyTicksByPulled = remainingFlyTicks < 10 ? 10 : remainingFlyTicks;
	entity->m_flyingByPulled = true;
}
