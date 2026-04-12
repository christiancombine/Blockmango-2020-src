#include "EntityDecoration.h"
#include "Entity/EntityPlayer.h"
#include "Setting/DecorationSetting.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Network/CommonPacketSender.h"
#include "Common.h"

namespace BLOCKMAN
{

	EntityDecoration::EntityDecoration(World* pWorld) : Entity(pWorld) 
	{
		renderPriority = 20;
		preventEntitySpawning = true;
		m_nodes.clear();
	}

	EntityDecoration::EntityDecoration(World* pWorld, const Vector3& pos) : EntityDecoration(pWorld)
	{
		setPosition(pos);
		prePosition = pos;
	}

	EntityDecoration::~EntityDecoration()
	{

	}

	void EntityDecoration::onUpdate()
	{
		Entity::onUpdate();

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

		int yaw = int(rotationYaw);
		auto decoration = DecorationSetting::findItem(m_decorationId);
		if (decoration && m_oldYaw != yaw)
		{
			m_oldYaw = yaw;
			firstUpdate = true;
			setSize(yaw % 180 == 0 ? decoration->length : decoration->width, yaw % 180 == 0 ? decoration->width : decoration->length, decoration->height);
			setPosition(position);
		}

		if (onGround && prePosition != Vector3::INVALID && position != prePosition)
		{
			prePosition = position;
			SCRIPT_EVENT::EntityPositionChangeEvent::invoke(entityId, position);
		}
	}

	bool EntityDecoration::canBeCollidedWithUser(ui64 userId)
	{
		for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
		{
			if (it->second == userId)
			{
				return false;
			}
		}
		return true;
	}

	void EntityDecoration::setDecorationId(i32 id)
	{
		auto decoration = DecorationSetting::findItem(id);
		if (m_decorationId != id && decoration)
		{
			m_decorationId = id;
			m_playerActionId = decoration->actionId;
			setActorName(decoration->actorName);
			setActorBody(decoration->actorBody);
			setActorBodyId(decoration->actorBodyId);
			setCanCollided(decoration->isCanCollided);
			int yaw = int(rotationYaw);
			setSize(yaw % 180 == 0 ? decoration->length : decoration->width, yaw % 180 == 0 ? decoration->width : decoration->length, decoration->height);
			setPosition(position);
			m_nodes.clear();
			for (const auto & str : decoration->m_nodes)
			{
				m_nodes.insert({ str, 0 });
			}
		}
	}

	void EntityDecoration::setActorName(const String& actorName)
	{
		if (m_actorName != actorName)
		{
			m_actorName = actorName;
			m_isActorChange = true;
		}

	}

	void EntityDecoration::setActorBody(String body)
	{
		if (m_actorBody != body)
		{
			m_actorBody = body;
			m_isActorChange = true;
		}
	}

	void EntityDecoration::setActorBodyId(String bodyId)
	{
		if (m_actorBodyId != bodyId)
		{
			m_actorBodyId = bodyId;
			m_isActorChange = true;
		}
	}

	void EntityDecoration::setActorAction(String action)
	{
		if (m_actorAction != action)
		{
			m_actorAction = action;
			m_isActorChange = true;
		}
	}

	String EntityDecoration::getActorAction()
	{
		if (auto decoration = DecorationSetting::findItem(m_decorationId))
		{
			return m_isPlayAction ? DecorationSetting::getAction(m_decorationId, m_nodes) : decoration->awaitAnimate;
		}
		return "";
	}

	bool EntityDecoration::playerJoin(const String& bone, EntityPlayer * player)
	{
		auto iter = m_nodes.find(bone);
		if (iter != m_nodes.end() && iter->second == 0)
		{
			iter->second = player->getPlatformUserId();
			player->position.x = position.x;
			player->position.z = position.z;
			player->setPosition(Vector3(position.x, player->position.y, position.z));
			m_isPlayAction = isCanPlayAction();
			return true;
		}
		return false;
	}

	bool EntityDecoration::playerLeave(ui64 userId)
	{
		for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
		{
			if (it->second == userId)
			{
				it->second = 0;
				m_isPlayAction = hasUser();
				return true;
			}
		}
		return false;
	}

	void EntityDecoration::changeNode(const String & node, ui64 userId)
	{
		auto iter = m_nodes.find(node);
		if (iter != m_nodes.end())
		{
			iter->second = userId;
		}
	}

	void EntityDecoration::setDead()
	{
		for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
		{
			if (it->second > 0)
			{
				if (auto player = world->getPlayerByPlatformUserId(it->second))
				{
					player->setDecorationEntityId(0);
				}
			}
		}
		Entity::setDead();
	}

	bool EntityDecoration::changeAction()
	{
		auto decoration = DecorationSetting::findItem(m_decorationId);
		if (decoration && decoration->interactionType == 2)
		{
			m_isPlayAction = !m_isPlayAction;
			GameCommon::Instance()->getCommonPacketSender()->syncEntityDecoration(0, this);
			return true;
		}
		return false;
	}

	void EntityDecoration::setPlayAction(bool isPlayAction)
	{
		if (m_isPlayAction != isPlayAction || isPlayAction)
		{
			m_isPlayAction = isPlayAction;
			m_isActorChange = true;
		}
	}

	bool EntityDecoration::isCanPlayAction()
	{
		return DecorationSetting::canPlayAction(m_decorationId);
	}

	bool EntityDecoration::hasUser()
	{
		for (auto& item : m_nodes)
		{
			if (item.second > 0)
			{
				return true;
			}
		}
		return false;
	}

}