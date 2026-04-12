#include "EntityRenderFlyingMount.h"
#include "Entity/EntityFlyingMount.h"
#include "Blockman/EntityRenders/EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "cWorld/Blockman.h"
#include "Blockman/Render/RenderEntity.h"
#include "Setting/MountSetting.h"
#include "Anim/Skeleton.h"
#include "Object/Root.h"
#include "Entity/EntityPlayer.h"

using namespace LORD;

namespace BLOCKMAN
{
	void EntityRenderFlyingMount::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
	{
		auto entity = dynamic_cast<EntityFlyingMount*>(pEntity);
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
		actor->SetPosition(pos);
		Quaternion orientation;
		Quaternion::Slerp(orientation, entity->getPrevOrientation(), entity->getOrientation(), rdt, true);
		actor->SetOrientation(orientation);
		int brightUV = entity->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		actor->PlaySkill("fly");

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
		auto playerOrientation = boneOrientation * setting.riderRenderOrientation;
		playerActor->SetOrientation(playerOrientation);
		auto offset = playerOrientation * setting.riderRenderOffset;
		playerActor->SetPosition(offset + bonePos);
		playerActor->PlaySkill(setting.rideAnimation);
	}
}