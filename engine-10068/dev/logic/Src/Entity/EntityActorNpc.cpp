#include "EntityActorNpc.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "Common.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Setting/ActorSizeSetting.h"
#include "Setting/MultiLanTipSetting.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	EntityActorNpc::~EntityActorNpc()
	{

	}

	void EntityActorNpc::onUpdate()
	{
		Entity::onUpdate();
		if (isFlyEntity())
		{
			return;
		}
		if (canFall() && !m_suspended)
		{
			moveEntity(motion);
		}

		float slipperiness = 0.91F;

		if (onGround)
		{
			slipperiness = 0.546F;
			int ix = int(Math::Floor(position.x));
			int iy = int(Math::Floor(boundingBox.vMin.y)) - 1;
			int iz = int(Math::Floor(position.z));
			int blockID = world->getBlockId(BlockPos(ix, iy, iz));
			if (blockID > 0)
			{
				slipperiness = BlockManager::sBlocks[blockID]->slipperiness() * 0.91F;
			}
		}

		if (position.y > -128.f)
		{
			motion.y -= 0.08f;
		}
		else
		{
			motion.y = 0.0f;
		}

		motion.y *= 0.98f;
		motion.x *= slipperiness;
		motion.z *= slipperiness;
	}

	void EntityActorNpc::setActorName(const String& actorName)
	{
		m_actorName = actorName;
		ActorSize* pSize;
		if (ActorSizeSetting::isHaveActor(m_actorName))
		{
			pSize = ActorSizeSetting::getActorSize(m_actorName);
		}
		else
		{
			pSize = ActorSizeSetting::getActorSize("ActorNpc");
		}
		setSize(pSize->width, pSize->length, pSize->height);
		setScale(pSize->scale);
		setPosition(position);
	}

	void EntityActorNpc::setScale(float s)
	{
		m_scale = s;
	}
	void EntityActorNpc::setHeight(float h)
	{
		height = h;
	}
}