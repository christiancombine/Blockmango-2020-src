#include "EntityVehicle.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "Common.h"
#include "Script/Event/LogicScriptEvents.h"
#include "DamageSource.h"
#include "Enchantment.h"
#include "Entity/EntityPlayer.h"
#include <algorithm>
#include "UI/Touch.h"

#undef max
#undef min

namespace BLOCKMAN
{

static CarSetting createDefaultCarSetting()
{
	CarSetting setting;
	return setting;
}

const CarSetting EntityVehicle::defaultSetting = createDefaultCarSetting();

EntityVehicle::EntityVehicle(World* pWorld, int id, const Vector3& pos, float yaw)
	: Entity(pWorld)
	, m_vehicleID(id)
	, m_passengerEye(0, 2.f, 0)
{
	stepHeight = 1.1;
	prevRotationYaw = rotationYaw = yaw;
	setSize(2.f, 2.f, 1.3f);
	setPosition(pos);
	serverPos.x = int(Math::Floor(pos.x * 32.f));
	serverPos.y = int(Math::Floor(pos.y * 32.f));
	serverPos.z = int(Math::Floor(pos.z * 32.f));
	if (auto setting = CarSetting::getCarSetting(m_vehicleID))
	{
		m_setting = setting;
	}
}

void EntityVehicle::resetInputs()
{
	m_input = m_networkInput = m_playerInput = m_directInput = {};
}

void EntityVehicle::mergeInputs()
{
	m_input.braking = m_playerInput.braking || m_directInput.braking || m_networkInput.braking;

	if (m_networkInput.throttle != 0)
	{
		m_input.throttle = m_networkInput.throttle;
	}
	else if (m_directInput.throttle != 0)
	{
		m_input.throttle = m_directInput.throttle;
	}
	else if (m_playerInput.throttle != 0)
	{
		m_input.throttle = m_playerInput.throttle;
	}
	else
	{
		m_input.throttle = 0;
	}

	if (m_networkInput.turningRadius != 0)
	{
		m_input.turningRadius = m_networkInput.turningRadius;
	}
	else if (m_directInput.turningRadius != 0)
	{
		m_input.turningRadius = m_directInput.turningRadius;
	}
	else if (m_playerInput.turningRadius != 0)
	{
		m_input.turningRadius = m_playerInput.turningRadius;
	}
	else
	{
		m_input.turningRadius = 0;
	}
}

void EntityVehicle::setInput(const MovementInput& input)
{
	if (input.moveStrafe == 0)
	{
		m_playerInput.turningRadius = 0;
	}
	else
	{
		m_playerInput.turningRadius = -m_setting->minTurningRadius / input.moveStrafe;
	}

	Vector2 headDir = ToDirection(rotationYaw * Math::DEG2RAD);
	float dotResult = headDir.dot(m_velocity);

	if (input.moveForward == 0)
	{
		m_playerInput.throttle = 0;
		m_playerInput.braking = false;
		return;
	}

	if (dotResult == 0)
	{
		// 当前速度为0，开始起步
		m_playerInput.throttle = input.moveForward;
		m_playerInput.braking = false;
		return;
	}

	if (dotResult > 0)
	{
		if (input.moveForward > 0)
		{
			m_playerInput.throttle = input.moveForward;
			m_playerInput.braking = false;
		}
		else
		{
			m_playerInput.throttle = 0;
			m_playerInput.braking = true;
		}
	}
	else
	{
		if (input.moveForward > 0)
		{
			m_playerInput.throttle = 0;
			m_playerInput.braking = true;
		}
		else
		{
			m_playerInput.throttle = input.moveForward;
			m_playerInput.braking = false;
		}
	}
}

void EntityVehicle::setTurning(float turning)
{
	if (turning == 0)
	{
		m_directInput.turningRadius = 0;
	}
	else
	{
		m_directInput.turningRadius = -m_setting->minTurningRadius / turning;
	}
}

// 将角度调整到(-PI, PI]的范围
float EntityVehicle::NormalizeAngle(float angle)
{
	static constexpr float PI = 3.14159265358979323846;
	static constexpr float PIx2 = PI * 2;
	angle = std::fmod(angle, PIx2);
	if (angle > PI)
		angle -= PIx2;
	if (angle < -PI)
		angle += PIx2;
	return angle;
}

float EntityVehicle::ToAngle(const Vector2& dir)
{
	return std::atan2(-dir.x, dir.y);
}

Vector2 EntityVehicle::ToDirection(float yaw)
{
	return { -Math::Sin(yaw), Math::Cos(yaw) };
}

Vector2 EntityVehicle::Rotate90(const Vector2& dir)
{
	return { -dir.y, dir.x };
}

void EntityVehicle::updateFallState(float y, bool onGroundIn)
{
	handleWaterMovement();

	if (onGroundIn && fallDistance > 0.0f)
	{
		BlockPos pos = (position - Vector3(0.f, 0.2f + yOffset, 0.f)).getFloor();
		int blockID = world->getBlockId(pos);

		if (blockID == 0)
		{
			int blockID2 = world->getBlockId(pos.getNegY());

			if (blockID2 == BLOCK_ID_FENCE ||
				blockID2 == BLOCK_ID_FENCE_GATE ||
				blockID2 == BLOCK_ID_COBBLE_STONE_WALL)
			{
				blockID = blockID2;
			}
		}

		if (blockID > 0)
		{
			BlockManager::sBlocks[blockID]->onFallenUpon(world, pos, this, fallDistance);
		}
	}

	Entity::updateFallState(y, onGroundIn);
}

void EntityVehicle::stopVehicle()
{
	m_velocity = Vector2::ZERO;
	resetInputs();
	setActionState(VehicleActionState::IDLE);
}

void EntityVehicle::updateActionState()
{
	if (m_input.throttle > 0)
	{
		m_curState = VehicleActionState::RUN;
		return;
	}
	if (m_input.throttle < 0)
	{
		m_curState = VehicleActionState::BACK;
		return;
	}

	Vector2 headDir = ToDirection(rotationYaw * Math::DEG2RAD);
	float dotResult = headDir.dot(m_velocity);
	if (dotResult > 0)
	{
		m_curState = VehicleActionState::RUN;
	}
	else if (dotResult < 0)
	{
		m_curState = VehicleActionState::BACK;
	}
	else
	{
		m_curState = VehicleActionState::IDLE;
	}
}

void EntityVehicle::fall(float distance)
{
	Entity::fall(distance);
	int dis = int(Math::Ceil(distance - 3.0F));

	if (dis >= -1)
	{
		if (dis > 3)
			playSoundByType(ST_FallBig);
		else
			playSoundByType(ST_FallSmall);
	}

}

void EntityVehicle::kill()
{
	// attackEntityFrom(DamageSource::outOfWorld, 4.0F);
}

void EntityVehicle::readEntityFromNBT(NBTTagCompound* pNBT)
{
}

void EntityVehicle::writeEntityToNBT(NBTTagCompound* pNBT)
{
}


bool EntityVehicle::attackEntityFrom(DamageSource* pSource, float amount)
{
	return false;
}

void EntityVehicle::onUpdate()
{
	if (position.y < -64.f)
	{
		setDead();
	}

	prevPos = position;
	prevRotationYaw = rotationYaw;
	prevRotationPitch = rotationPitch;

	if (Math::Abs(motion.x) < 0.005f)
		motion.x = 0.0f;
	if (Math::Abs(motion.y) < 0.005f)
		motion.y = 0.0f;
	if (Math::Abs(motion.z) < 0.005f)
		motion.z = 0.0f;

	mergeInputs();
	syncInputs();
	if (m_syncMotion)
	{
		setPosition(m_syncedPos);
		rotationYaw = m_syncedYaw;
	}
	else
	{
		updateMotion();
	}
	m_syncMotion = false;
	motion.x = 0.0f;
	motion.z = 0.0f;

	collideWithNearbyEntities();
	syncPositionAndRotation();
	syncPositionToPassenger();
	updateActionState();
}

void EntityVehicle::setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment)
{
	prevPos = position;
	setPositionAndRotation(pos, yaw, pitch);
}

void EntityVehicle::collideWithNearbyEntities()
{
	EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.expland(0.2f, 0.0f, 0.2f));

	for (int i = 0; i < int(entities.size()); ++i)
	{
		Entity* entity = entities[i];

		if (entity->canBeCollidedWith())
		{
			collideWithEntity(entity);
		}
	}
}

int EntityVehicle::getFreeSeatNum()
{
	return m_setting->maxPassengers - m_passengerIds.size();
}

bool EntityVehicle::carryPassenger(int entityPlayerId, bool bIsDriver)
{
	if (bIsDriver)
	{
		m_driverId = entityPlayerId;
		m_hasDriver = true;
		return true;
	}

	if (m_passengerIds.size() < m_setting->maxPassengers)
	{
		m_passengerIds.push_back(entityPlayerId);
		return true;
	}

	return false;
}

bool EntityVehicle::getOffPassenger(int entityPlayerId)
{
	if (entityPlayerId == m_driverId)
	{
		m_driverId = 0;
		m_hasDriver = false;
		m_locked = false; 
		m_pullCameraCountdown = 0;
		stopVehicle();
		return true;
	}

	int oldSize = m_passengerIds.size();
	m_passengerIds.erase(std::remove(m_passengerIds.begin(), m_passengerIds.end(), entityPlayerId), m_passengerIds.end());
	return oldSize != m_passengerIds.size();
}

void EntityVehicle::updateMotion()
{
	float dt = world->getFrameDeltaTime();
	Vector2 headDir = ToDirection(rotationYaw * Math::DEG2RAD);
	float speedAlongHeading = headDir.dot(m_velocity);

	// 计算漂移导致的轮胎侧向力
	Vector2 lateralFriction = Vector2::ZERO;
	if (m_velocity != Vector2::ZERO)
	{
		float velocityAngle = ToAngle(m_velocity);
		float yaw = rotationYaw * Math::DEG2RAD;
		float slipAngle = NormalizeAngle(yaw - velocityAngle);
		// slipAngle应该是锐角
		if (slipAngle > Math::PI_DIV2)
			slipAngle = Math::PI - slipAngle;
		if (slipAngle < -Math::PI_DIV2)
			slipAngle = -Math::PI - slipAngle;
		float lateralForce = m_setting->mass * 9.8f * Math::Clamp(
			m_setting->corneringStiffness * slipAngle * Math::RAD2DEG,
			-m_setting->maxCoefLateralFriction,
			m_setting->maxCoefLateralFriction);
		lateralFriction = Rotate90(headDir) * lateralForce;
	}

	// 计算总阻力
	float speed = m_velocity.len();
	Vector2 resistance = Vector2::ZERO;
	if (speed != 0)
	{
		resistance = -(m_setting->coefAirDrag * speed * speed
			+ m_setting->coefRollingFriction * m_setting->mass * 9.8f) / speed * m_velocity;
	}
	Vector2 traction = m_setting->traction * m_input.throttle * headDir;

	// 计算制动力
	Vector2 braking = Vector2::ZERO;
	if (m_input.braking)
	{
		if (speedAlongHeading > 0)
			braking = -headDir * m_setting->brakingForce;
		else if (speedAlongHeading < 0)
			braking = headDir * m_setting->brakingForce;
	}

	// 计算车辆受力及速度
	Vector2 sumForce = traction + resistance + braking + lateralFriction;
	m_velocity += sumForce * (dt / m_setting->mass);

	// 各种阻力摩擦力的方向都和当前速度有关系，所以速度分量接近零的时候会很不稳定，可能会出现车身抖动现象。
	// 这个现象在车辆速度接近0时特别明显，所以当车速接近0时，直接将车速设为0
	static float kMinSpeed = 1;
	float newSpeed = m_velocity.len();
	if (newSpeed < kMinSpeed && m_input.throttle == 0)
	{
		m_velocity = Vector2::ZERO;
		newSpeed = 0;
	}

	// 计算位移及转角
	if (m_input.turningRadius != 0 && newSpeed != 0)
	{
		float centripetalAcc = newSpeed * newSpeed / m_input.turningRadius;
		float maxCentripetalAcceleration = m_setting->maxCentripetalForce / m_setting->mass;
		centripetalAcc = Math::Clamp(centripetalAcc, -maxCentripetalAcceleration, maxCentripetalAcceleration);
		m_velocity += centripetalAcc * dt * Rotate90(headDir);
		float angularVelocity = std::sqrt(centripetalAcc / m_input.turningRadius);
		if ((m_input.turningRadius < 0) ^ (headDir.dot(m_velocity) < 0))
			angularVelocity = -angularVelocity;
		rotationYaw += angularVelocity * dt * Math::RAD2DEG;
	}

	if (m_hasDriver || !onGround)
	{
		motion.x = m_velocity.x * dt;
		motion.z = m_velocity.y * dt;
		motion.y -= 0.08f;
		moveEntity(motion);

		// 处理碰撞后的速度变化
		static constexpr float kMinSpeedAfterCollision = 1;
		if (prevPos.x == position.x)
		{
			float speedX = std::abs(m_velocity.x);
			if (speedX > m_setting->minBounceSpeed)
			{
				m_velocity.x = -m_velocity.x * m_setting->coefRestitution;
			}
			else if (speedX > kMinSpeedAfterCollision)
			{
				m_velocity.x = Math::Sgn(m_velocity.x) * kMinSpeedAfterCollision;
			}
		}
		if (prevPos.z == position.z)
		{
			float speedZ = std::abs(m_velocity.y);
			if (speedZ > m_setting->minBounceSpeed)
			{
				m_velocity.y = -m_velocity.y * m_setting->coefRestitution;
			}
			else if (speedZ > kMinSpeedAfterCollision)
			{
				m_velocity.y = Math::Sgn(m_velocity.y) * kMinSpeedAfterCollision;
			}
		}
	}
}

bool EntityVehicle::lockVehicle(EntityPlayer* pPlayer, bool bState)
{
	if (!pPlayer)
		return false;

	if (!isDriver(pPlayer->entityId))
		return false;

	if (bState == m_locked)
		return false;

	m_locked = bState;
	return true;
}

bool EntityVehicle::canDrivedBy(EntityPlayer * pPlayer) const
{
	if (CarSetting::m_sbIsCarFree)
		return true;

	if (m_setting->price <= 0)
		return true;

	if (pPlayer->hasOwnVehicle(m_vehicleID))
		return true;

	return false;
}

bool EntityVehicle::shootTargetByGun(EntityPlayer* pTarget, const PACKET_DETAIL::Vector3& hitpos, const GunSetting* pGunSetting)
{
	if (m_driverId != 0 && m_hasDriver)
	{
		EntityPlayer* pDriver = world->getPlayerEntity(m_driverId);
		if(pDriver != NULL && !pDriver->capabilities.disableDamage)
			damageCalc(pTarget, pDriver, pGunSetting, hitpos);
	}

	auto tempList = m_passengerIds;
	for (int passengerId : tempList)
	{
		EntityPlayer* passenger = world->getPlayerEntity(passengerId);
		if (passenger != NULL && !passenger->capabilities.disableDamage)
			damageCalc(pTarget, passenger, pGunSetting, hitpos);
	}

	return true;
}

bool EntityVehicle::damageCalc(EntityPlayer * pTarget, EntityPlayer * hitPlayer, const GunSetting* pGunSetting, Vector3 hitpos) {
	const CarSetting* pSetting = CarSetting::getCarSetting(this->m_vehicleID);
	if (pSetting) {
		SCRIPT_EVENT::EntityHitEvent::invoke(pGunSetting->adaptedBulletId, hitpos, hitPlayer->entityId);
		float amount = pGunSetting->damage;
		amount *= pSetting->passengersDamage;

		DamageSource* pSource = NULL;
		pSource = DamageSource::causePlayerDamage(pTarget);
		pSource->knockBackDis = pGunSetting->knockbackDistance;

		if (hitPlayer->attackEntityFrom(pSource, amount))
		{
			EnchantmentThorns::damageArmor(pTarget, hitPlayer, rand);
		}
		LordSafeDelete(pSource);
	}
	return true;
}

void EntityVehicle::whenRemoved()
{
	if (m_driverId != 0 && m_hasDriver)
	{
		EntityPlayer* pDriver = world->getPlayerEntity(m_driverId);
		if (pDriver != NULL)
		{
			pDriver->getOffVehicle();
			m_driverId = 0;
			m_hasDriver = false;
		}
	}
		
	auto tempList = m_passengerIds;
	for (int id : tempList)
	{
		if(auto player = world->getPlayerEntity(id))
			player->getOffVehicle();
	}

	m_passengerIds.clear();
}

void EntityVehicle::syncPositionToPassenger()
{
	// copy all the pos to passenger.
	if (m_hasDriver && m_driverId != 0)
	{
		EntityPlayer* pDriver = world->getPlayerEntity(m_driverId);
		if (pDriver != NULL)
		{
			pDriver->prevPos = prevPos + m_passengerEye;
			pDriver->setPosition(position + m_passengerEye);
			pDriver->lastTickPos = lastTickPos + m_passengerEye;
			pDriver->rotationYaw += rotationYaw - prevRotationYaw;
			pullBackDriverYaw(pDriver);
		}
	}

	for (int passengerId : m_passengerIds)
	{
		EntityPlayer* passenger = world->getPlayerEntity(passengerId);
		if (passenger != NULL)
		{
			passenger->prevPos = prevPos + m_passengerEye;
			passenger->setPosition(position + m_passengerEye);
			passenger->lastTickPos = lastTickPos + m_passengerEye;
			passenger->rotationYaw += rotationYaw - prevRotationYaw;
		}
	}
}

void EntityVehicle::pullBackDriverYaw(EntityPlayer* pDriver)
{
	if (TouchManager::Instance()->hasSceneTouch())
	{
		static constexpr float kCountdownTime = 1; // in seconds
		m_pullCameraCountdown = kCountdownTime;
		return;
	}

	float dt = world->getFrameDeltaTime();
	m_pullCameraCountdown -= dt;
	if (m_pullCameraCountdown > 0)
	{
		return;
	}

	m_pullCameraCountdown = 0;
	static constexpr float kPullBackSpeed = 180; // deg/s
	float pullBackAngle = kPullBackSpeed * dt;

	// 计算车辆和玩家的视角差值，规范化到[-180, 180]区间
	float angleDiff = pDriver->rotationYaw - rotationYaw;
	angleDiff = std::fmod(angleDiff, 360);
	if (angleDiff < -180)
		angleDiff += 360;
	else if (angleDiff > 180)
		angleDiff -= 360;

	// 移动玩家视角
	if (angleDiff > pullBackAngle)
	{
		pDriver->rotationYaw -= pullBackAngle;
	}
	else if (angleDiff < -pullBackAngle)
	{
		pDriver->rotationYaw += pullBackAngle;
	}
	else
	{
		pDriver->rotationYaw -= angleDiff;
	}
}

void EntityVehicle::setSyncedPosAndYaw(const Vector3& pos, float yaw)
{
	m_syncMotion = true;
	m_syncedPos = pos;
	m_syncedYaw = yaw;
}

void EntityVehicle::playHornSound()
{
	playSoundByType(ST_take_car);
	syncVehicleHorn();
}

}