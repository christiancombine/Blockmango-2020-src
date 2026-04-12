#ifndef __ENTITY_BULLET_CLIENT_HEADER__
#define __ENTITY_BULLET_CLIENT_HEADER__

#include "Entity/EntityBullet.h"
#include "Scene/ModelEntity.h"

namespace BLOCKMAN
{
	class SimpleEffect;
	class RayTraceResult;

	enum BULLET_TYPE
	{
		BT_NORMAL = 0,
		BT_MORTAR,
		BT_LASER,
		BT_MOMENTARYLASER,
	};

	// bullet of rifle, sniper, and shotgun
	class EntityBulletClient : public EntityBullet
	{
		RTTI_DECLARE(EntityBulletClient);

	public:
		SimpleEffect* m_bulletEffect = nullptr;
		String m_bulletEffectName = "";
		BULLET_TYPE m_bulletType = BT_NORMAL;
		bool m_isEffectInit = false;
		EntityLivingBase* m_shootingEntity = nullptr;
		GunSetting* m_gunSetting = nullptr;
		bool m_isFirstPerspective = false;

	protected:
		bool m_needDeleteEffect = true;
		int m_bounceTimes = 0;
		bool m_isShootingByMe = false;

		bool m_hasBlockTarget = false;
		Vector3i m_blockTargetPos = Vector3i::ZERO;
		Vector3 m_aimPoint = Vector3::ZERO;
		bool m_aimFailed = false;
		float m_aimDis = 0.0f;

		//getRayTraceAtBlock param
		Vector3 m_begin = Vector3::ZERO;
		Vector3 m_entityEnd = Vector3::ZERO;
		Vector3 m_blockEnd = Vector3::ZERO;
		float m_minDis = 0.0f;
		float m_ratio = 1.0f;
		bool m_hasBeenDead = false;

	public:
		EntityBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos, float yawOffset = 0.0f, float pitchOffset = 0.0f);
		~EntityBulletClient();

		virtual void onUpdate() override;
		Vector3 calcPosition(EntityLivingBase* shooting, GunSetting* gunSetting, float rdt = 1.0f);
		void trySendGunFireResult(RayTraceResult trace);
		void trySendGunFireExplosionResult(const Vector3& hitPos);

	protected:
		RayTraceResult getRayTraceAtBlock();
	};

	// bullet of mortar
	class EntityMortarBulletClient : public EntityBulletClient
	{
	public:
		ModelEntity* m_modelEntity = nullptr;

	public:
		EntityMortarBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos);
		~EntityMortarBulletClient();

		virtual void onUpdate() override;
	};

	// bullet of laserGun, and electricityGun
	class EntityLaserBulletClient : public EntityBulletClient
	{
	public:
		SimpleEffect* m_startEffect = nullptr;
		String m_startEffectName = "";
		Vector3 m_laserBeginPos = Vector3::ZERO;
		Vector3 m_laserEndPos = Vector3::ZERO;
		Vector3 m_lastLaserEndPos = Vector3::ZERO;

	protected:
		float m_contractiveValue = 0.0f;
		int m_currTime = 0;
		int m_lastConsumeBulletTime = 0;
		bool m_canHurt = true;
		int m_record_time = 0;
		int m_sound_type = 0;

	public:
		EntityLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos, float contractiveValue = 0.0f);
		~EntityLaserBulletClient();

		virtual void onUpdate() override;
	};

	// bullet of armor piercing
	class EntityArmorPiercingBulletClient : public EntityBulletClient
	{
	private:
		vector<int>::type hitEntities;

	public:
		EntityArmorPiercingBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos, float yawOffset = 0.0f, float pitchOffset = 0.0f)
			: EntityBulletClient(pWorld, pShooting, pSetting, endPos, hasBlockTarget, blockTargetPos, yawOffset, pitchOffset)
		{}

		virtual void onUpdate() override;
	};

	// bullet of momentary laserGun, and momentary electricityGun
	class EntityMomentaryLaserBulletClient : public EntityLaserBulletClient
	{
	public:
		bool m_hasUsed = false;

	protected:
		int m_existenceCnt = 0;

	public:
		EntityMomentaryLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos);

		virtual void onUpdate() override;
	};

	// bullet of momentary piercing laserGun, and momentary piercing electricityGun
	class EntityMomentaryPiercingLaserBulletClient : public EntityMomentaryLaserBulletClient
	{
	public:
		EntityMomentaryPiercingLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos)
			: EntityMomentaryLaserBulletClient(pWorld, pShooting, pSetting, endPos, hasBlockTarget, blockTargetPos)
		{}

		virtual void onUpdate() override;
	};
}

#endif