/********************************************************************
filename: 	EntityVehicle.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2018-01-27
*********************************************************************/
#ifndef __ENTITY_VEHICLE_HEADER__
#define __ENTITY_VEHICLE_HEADER__

#include "Entity.h"
#include "Setting/CarSetting.h"
#include "Network/NetworkDefine.h"

class S2CPlayerActionPacketHandles;
class C2SPlayerActionPacketHandles;

namespace BLOCKMAN
{

class EntityPlayer;
class MovementInput;

enum class VehicleActionState
{
	IDLE = 0,
	RUN,
	BACK,
};

class VehicleInputData
{
public:
	// 圆心在车右侧则正
	float turningRadius = 0;
	float throttle = 0;
	bool braking = false;

	friend bool operator==(const VehicleInputData& lhs, const VehicleInputData& rhs)
	{
		return lhs.turningRadius == rhs.turningRadius
			&& lhs.throttle == rhs.throttle
			&& lhs.braking == rhs.braking;
	}
	friend bool operator!=(const VehicleInputData& lhs, const VehicleInputData& rhs)
	{
		return !(lhs == rhs);
	}
};

class EntityVehicle: public Entity
{
	RTTI_DECLARE(EntityVehicle);
	friend class ::S2CPlayerActionPacketHandles;
	friend class ::C2SPlayerActionPacketHandles;

private:
	static const CarSetting defaultSetting;
	static constexpr float kMinSpeed = 0.01f;
	static constexpr float kMinAngularSpeed = 0.01f;

private:
	int m_vehicleID = 0;
	const CarSetting* m_setting = &defaultSetting;
	int m_driverId = 0;
	LORD::vector<int>::type m_passengerIds;
	Vector3 m_passengerEye = Vector3::ZERO;
	bool m_locked = false;
	bool m_hasDriver = false;
	VehicleActionState m_curState = VehicleActionState::IDLE;
	VehicleActionState m_prevState = VehicleActionState::IDLE;
	Vector2 m_velocity = Vector2::ZERO;  // x方向对应世界坐标x轴方向，y方向对应世界坐标z轴方向
	float m_pullCameraCountdown = 0;

	bool m_syncMotion = false;
	Vector3 m_syncedPos = Vector3::ZERO;
	float m_syncedYaw = 0;

	VehicleInputData m_playerInput;
	VehicleInputData m_directInput;
	VehicleInputData m_networkInput;
	VehicleInputData m_input;

private:
	// 将角度调整到(-PI, PI]的范围
	static float NormalizeAngle(float angle);
	// 单位为rad
	static float ToAngle(const Vector2& dir);
	// 返回单位向量
	static Vector2 ToDirection(float yaw);
	static Vector2 Rotate90(const Vector2& dir);

	/** implement override functions from Entity */

	/** implement override functions */
	void entityInit() override {}
	void updateFallState(float y, bool onGroundIn) override;
	void fall(float distance) override;
	void kill() override;
	/** (abstract) Protected helper method to read subclass entity data from NBT. */
	void readEntityFromNBT(NBTTagCompound* pNBT) override;
	/** (abstract) Protected helper method to write subclass entity data to NBT. */
	void writeEntityToNBT(NBTTagCompound* pNBT) override;
	/** Called when the entity is attacked. */
	bool attackEntityFrom(DamageSource* pSource, float amount) override;
	/** Returns true if other Entities should be prevented from moving through this Entity. */
	bool canBeCollidedWith() override { return true; }
	/** Returns true if this entity should push and be pushed by other entities when colliding. */
	bool canBePushed() override { return true; }
	/** Checks whether target entity is alive. */
	bool isEntityAlive() override { return true; }
	/** Called to update the entity's position/logic. */
	void onUpdate() override;
	/** Sets the position and rotation. Only difference from the other one is no bounding on the rotation. Args: posX, posY, posZ, yaw, pitch */
	void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment) override;
	void whenRemoved() override;
	float getEyeHeight() override { return 1.2F; }

	void updateMotion();
	void collideWithNearbyEntities();
	void collideWithEntity(Entity* pEntity) { pEntity->applyEntityCollision(this); }
	void stopVehicle();
	void updateActionState();
	bool damageCalc(EntityPlayer* pTarget, EntityPlayer* hitPlayer, const GunSetting* pGunSetting, Vector3 hitpos);
	void syncPositionToPassenger();
	// 将玩家视角yaw值拉回到车辆yaw值
	void pullBackDriverYaw(EntityPlayer* pDriver);
	void resetInputs();
	void mergeInputs();
	virtual void syncInputs() {}
	virtual void syncPositionAndRotation() {}
	virtual void syncVehicleHorn() {}

public:
	EntityVehicle(World* pWorld, int id, const Vector3& pos, float yaw = 0.f);

	const VehicleInputData& getInput() const { return m_input; }
	void setInput(const MovementInput& input);
	void setThrottle(float throttle) { m_directInput.throttle = throttle; }
	// 参数负数时向右转
	void setTurning(float turning);
	void setBraking(float isBraking) { m_directInput.braking = isBraking; }
	int getVehicleId() const { return m_vehicleID; }
	int getDriverId() const { return m_driverId; }
	const Vector2& getVelocity() const { return m_velocity; }
	bool isLocked() const { return m_locked; }
	bool hasDriver() const { return m_hasDriver; }
	const LORD::vector<int>::type& getPassengersIds() const { return m_passengerIds; }
	bool hasPlayer() const { return m_hasDriver || m_passengerIds.size() > 0; }
	const CarSetting* getSetting() const { return m_setting; }
	bool lockVehicle(EntityPlayer* pPlayer, bool bState);
	bool canDrivedBy(EntityPlayer* pPlayer) const;
	bool carryPassenger(int entityPlayerId, bool bIsDriver);
	bool getOffPassenger(int entityPlayerId);
	bool isDriver(int playerId) { return m_driverId > 0 && m_driverId == playerId; }
	int getFreeSeatNum();
	/** Move the entity to the coordinates informed, but keep yaw/pitch values. */
	void setPositionAndUpdate(const Vector3& pos) { setLocationAndAngles(pos, rotationYaw, rotationPitch); }
	/** set position and yaw*/
	void setPositionAndYaw(const Vector3& pos, float yaw) { setLocationAndAngles(pos, yaw, rotationPitch); }
	void setSyncedPosAndYaw(const Vector3& pos, float yaw);
	bool shootTargetByGun(EntityPlayer* pTarget, const PACKET_DETAIL::Vector3& hitpos, const GunSetting* pGunSetting);
	void setActionState(VehicleActionState state) { m_prevState = m_curState; m_curState = state; }
	VehicleActionState getActionState() const { return m_curState; }
	bool isActionStateChanged() const { return m_prevState != m_curState; }
	void playHornSound();
	virtual bool isForceSyncSpawn() { return true; }
};

}

#endif