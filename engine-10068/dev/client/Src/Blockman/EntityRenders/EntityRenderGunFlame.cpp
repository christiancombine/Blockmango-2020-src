#include "EntityRenderGunFlame.h"
#include "cEntity/EntityGunFlame.h"
#include "World/WorldEffectManager.h"
#include "EntityRenderManager.h"
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/GunSetting.h"

#include "Tessolator/BatchRenderable.h"
#include "Tessolator/LineRenderable.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN
{
	EntityRenderGunFlame::EntityRenderGunFlame()
	{
		if (Blockman::Instance()->m_isShowGunFlameCoordinate)
		{
			m_line = TessManager::Instance()->createLineRenderable(LRP_SELECTED);
		}
	}

	EntityRenderGunFlame::~EntityRenderGunFlame()
	{
		if (Blockman::Instance()->m_isShowGunFlameCoordinate)
		{
			TessManager::Instance()->destroyLineRenderable(m_line);
		}
	}

	void EntityRenderGunFlame::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
	{
		if (!pEntity || pEntity->isDead)
		{
			return;
		}
		EntityGunFlame* gunFlame = dynamic_cast<EntityGunFlame*>(pEntity);
		if (!gunFlame)
		{
			return;
		}

		if (Blockman::Instance()->m_isShowGunFlameCoordinate && gunFlame->m_pShooting && !gunFlame->m_pShooting->isDead)
		{
			drawCoordinateLines(gunFlame->m_pShooting, rdt);
		}

		Vector3 beginPos = gunFlame->getLastTickPosition(rdt);
		if (gunFlame->m_isFirstPerspective)
		{
			Camera* mainCamera = SceneManager::Instance()->getMainCamera();
			if (mainCamera)
			{
				beginPos = mainCamera->getPosition();
				beginPos += mainCamera->getDirection() * gunFlame->m_frontOff_1;
				beginPos += mainCamera->getRight() * gunFlame->m_rightOff_1;
				beginPos += -mainCamera->getUp() * gunFlame->m_downOff_1;
			}
		}

		if (!gunFlame->m_isGunFlameEffectInit)
		{
			if (gunFlame->m_flameEffectName.size())
			{
				gunFlame->m_flameEffect = WorldEffectManager::Instance()->addSimpleEffect(gunFlame->m_flameEffectName, beginPos, 0.0f, -1, gunFlame->m_scale, gunFlame->m_scale, gunFlame->m_scale);
				if (gunFlame->m_flameEffect && gunFlame->m_isFirstPerspective)
				{
					gunFlame->m_flameEffect->setViewSpace(true);
				}
			}
			gunFlame->m_isGunFlameEffectInit = true;
		}

		if (gunFlame->m_flameEffect)
		{
			gunFlame->m_flameEffect->mPosition = beginPos;
			gunFlame->m_flameEffect->mNode->setLocalScaling(Vector3(gunFlame->m_scale, gunFlame->m_scale, gunFlame->m_scale));
		}
	}

	void EntityRenderGunFlame::drawCoordinateLines(EntityLivingBase* shooting, float rdt)
	{
		bool isFirstView = shooting == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST;
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		Vector3 dir, right, down;
		if (isFirstView)
		{
			dir = mainCamera->getDirection();
			right = mainCamera->getRight();
			down = -mainCamera->getUp();
		}
		else
		{
			float yaw = shooting->prevRenderYawOffset + (shooting->renderYawOffset - shooting->prevRenderYawOffset) * rdt;
			float pitch = 0.f;

			dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));
			right = Vector3::NEG_UNIT_Y.cross(dir);
			down = Vector3::NEG_UNIT_Y;
		}

		Vector3 pointO, pointXP, pointYP, pointZP;
		pointO = dir + shooting->getPosition(rdt) + shooting->getEyeHeight();
		pointXP = pointO + dir;
		pointYP = pointO + right;
		pointZP = pointO + down;
		m_line->beginPolygon();
		LineVertexFmt* points = NULL;
		m_line->getLockPtr_line(2, points);
		points[0].set(pointO.x, pointO.y, pointO.z, 0x000000FF);
		points[1].set(pointXP.x, pointXP.y, pointXP.z, 0x000000FF);
		LineVertexFmt* points2 = NULL;
		m_line->getLockPtr_line(2, points2);
		points2[0].set(pointO.x, pointO.y, pointO.z, 0x00FF0000);
		points2[1].set(pointYP.x, pointYP.y, pointYP.z, 0x00FF0000);
		LineVertexFmt* points3 = NULL;
		m_line->getLockPtr_line(2, points3);
		points3[0].set(pointO.x, pointO.y, pointO.z, 0x0000FF00);
		points3[1].set(pointZP.x, pointZP.y, pointZP.z, 0x0000FF00);
		m_line->endPolygon();
		if (m_line->getPointNum() > 0)
			TessManager::Instance()->addLineRenderable(m_line);
	}
}