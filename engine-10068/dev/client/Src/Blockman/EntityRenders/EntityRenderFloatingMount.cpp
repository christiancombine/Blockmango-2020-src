#include "EntityRenderFloatingMount.h"
#include "Entity/EntityFloatingMount.h"
#include "Blockman/EntityRenders/EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "cWorld/Blockman.h"
#include "Blockman/Render/RenderEntity.h"
#include "Setting/MountSetting.h"
#include "Anim/Skeleton.h"
#include "Object/Root.h"
#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{
	void EntityRenderFloatingMount::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
	{
		auto entity = dynamic_cast<EntityFloatingMount*>(pEntity);
		if (!entity)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entity);
		if (!actor || !actor->getHasInited())
			return;

		Color color(1.f, 1.f, 1.f, 1.f);
		if (entity->isHurting)
		{
			color.set(1.f, 0.4f, 0.f, 1.f);
		}
		actor->SetNaturalColor(color, true);

		actor->setBlockManVisible();
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		actor->SetPosition(pos);
		int brightUV = entity->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		actor->PlaySkill("idle");

		auto playerActor = EntityRenderManager::Instance()->getEntityActor(entity->getRider());
		if (!playerActor || !playerActor->getHasInited())
			return;
		const auto& setting = entity->getSetting();
		auto mountBone = actor->GetSkeletonMaster()->getBone(setting.mountBone);
		auto mountBoneNode = actor->getBoneNode(mountBone);
		if (!mountBoneNode)
			return;
		if (actor->isUpdateManually())
			actor->Update(Root::Instance()->getFrameTime());
		const auto& bonePos = mountBoneNode->getWorldPosition();
		const auto& boneOrientation = mountBoneNode->getWorldOrientation();
		auto orientation = boneOrientation * setting.riderRenderOrientation;
		playerActor->SetOrientation(orientation);
		auto offset = orientation * setting.riderRenderOffset;
		playerActor->SetPosition(offset + bonePos);
		playerActor->PlaySkill(setting.rideAnimation);
	}
}