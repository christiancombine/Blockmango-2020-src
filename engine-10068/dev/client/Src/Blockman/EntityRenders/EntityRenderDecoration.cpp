#include "EntityRenderDecoration.h"
#include "EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Render/HeadTextRenderer.h"
#include "Render/RenderEntity.h"
#include "World/WorldEffectManager.h"
#include "cWorld/Blockman.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityDecoration.h"
#include "Object/Root.h"

namespace BLOCKMAN
{

	EntityRenderDecoration::EntityRenderDecoration()
	{
	}

	EntityRenderDecoration::~EntityRenderDecoration()
	{
	}

	void EntityRenderDecoration::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entity = dynamic_cast<EntityDecoration*>(pEntity);
		if (!entity)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entity);
		if (!actor || !actor->getHasInited())
			return;

		if (actor && actor->isUpdateManually())
			actor->Update(Root::Instance()->getFrameTime());

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entity->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);

		int brightUV = entity->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		if (entity->m_isActorChange && !entity->getActorBody().empty() && !entity->getActorBodyId().empty())
		{
			actor->AttachSlave(entity->getActorBody(), entity->getActorBodyId());
			entity->m_isActorChange = false;
			if (!entity->getActorAction().empty())
			{
				actor->PlaySkill(entity->getActorAction());
			}
		}

		if (!entity->getEffectName().empty())
		{
			actor->AttachSelectEffect(entity->getEffectName());
		}
		auto nodes = entity->getNodes();
		for (auto item : nodes)
		{
			if (item.second > 0)
			{
				SceneNode* nodeDecoration = actor->getSocketSceneNode(item.first);
				auto player = entity->world->getPlayerByPlatformUserId(item.second);
				if (nodeDecoration && player)
				{
					if (ActorObject* playerActor = EntityRenderManager::Instance()->getEntityActor(player))
					{
						playerActor->SetPosition(nodeDecoration->getWorldPosition());
						playerActor->SetOrientation(nodeDecoration->getWorldOrientation());
					}
				}
			}
		}
	}

}


