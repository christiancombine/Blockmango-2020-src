#include "EntityGunFlame.h"
#include "Object/Root.h"
#include "Setting/GunSetting.h"
#include "World/World.h"
#include "Entity/EntityLivingBase.h"
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{
	EntityGunFlame::EntityGunFlame(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, int existenceTime)
		: Entity(pWorld)
	{
		if (!pSetting || !pShooting || pShooting->isDead)
		{
			setDead();
			return;
		}
		m_pShooting = pShooting;
		m_gunSetting = pSetting;
		m_existenceTime = existenceTime;
		m_flameEffectName = pSetting->flameEffect;
		m_beginTime = Root::Instance()->getCurrentTime();

		position = calcPosition();
		setPosition(position);
		lastTickPos = position;
	}

	EntityGunFlame::~EntityGunFlame()
	{
		if (m_flameEffect)
		{
			m_flameEffect->mDuration = 0;
			m_flameEffect = nullptr;
		}
	}
	void EntityGunFlame::onUpdate()
	{
		if (!m_pShooting || m_pShooting->isDead)
		{
			m_pShooting = nullptr;
			setDead();
			return;
		}

		if (!Blockman::Instance()->m_isShowGunFlameCoordinate && Root::Instance()->getCurrentTime() - m_beginTime >= m_existenceTime)
		{
			setDead();
			return;
		}

		lastTickPos = position;
		position = calcPosition();
		setPosition(position);

		if (m_flameEffect)
		{
			m_flameEffect->mPosition = position;
		}
	}

	Vector3 EntityGunFlame::calcPosition()
	{
		m_isFirstPerspective = m_pShooting == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST;
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		Vector3 pos, dir, right, down;
		float frontOff, rightOff, downOff;
		if (m_isFirstPerspective)
		{
			pos = mainCamera->getPosition();

			dir = mainCamera->getDirection();
			right = mainCamera->getRight();
			down = -mainCamera->getUp();

			frontOff = m_gunSetting->flame_1_frontOff;
			rightOff = m_gunSetting->flame_1_rightOff;
			downOff = m_gunSetting->flame_1_downOff;
			m_scale = m_gunSetting->flame_1_scale;
		}
		else
		{
			pos = m_pShooting->getPosition();
			pos.y += m_pShooting->getEyeHeight();
			if (m_pShooting == (EntityPlayer*)(Blockman::Instance()->m_pPlayer))
			{
				pos.y += 0.08f;
			}

			float yaw = m_pShooting->renderYawOffset;
			float pitch = 0.f;
			dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));
			right = Vector3::NEG_UNIT_Y.cross(dir);
			down = Vector3::NEG_UNIT_Y;

			frontOff = m_gunSetting->flame_3_frontOff;
			rightOff = m_gunSetting->flame_3_rightOff;
			downOff = m_gunSetting->flame_3_downOff;
			m_scale = m_gunSetting->flame_3_scale;
		}

		if (Blockman::Instance()->m_isShowGunFlameCoordinate)
		{
			std::ifstream infile("scale.txt");
			if (infile)
			{
				float flame_1_frontOff, flame_1_rightOff, flame_1_downOff, flame_1_scale;
				float flame_3_frontOff, flame_3_rightOff, flame_3_downOff, flame_3_scale;
				infile >> flame_1_frontOff;
				infile >> flame_1_rightOff;
				infile >> flame_1_downOff;
				infile >> flame_1_scale;
				infile >> flame_3_frontOff;
				infile >> flame_3_rightOff;
				infile >> flame_3_downOff;
				infile >> flame_3_scale;

				if (m_isFirstPerspective)
				{
					frontOff = flame_1_frontOff;
					rightOff = flame_1_rightOff;
					downOff = flame_1_downOff;
					m_scale = flame_1_scale;
				}
				else
				{
					frontOff = flame_3_frontOff;
					rightOff = flame_3_rightOff;
					downOff = flame_3_downOff;
					m_scale = flame_3_scale;
				}
			}
			infile.close();
		}

		if (m_isFirstPerspective)
		{
			m_frontOff_1 = frontOff;
			m_rightOff_1 = rightOff;
			m_downOff_1 = downOff;
		}

		pos += dir * frontOff;
		pos += right * rightOff;
		pos += down * downOff;

		return pos;
	}
}
