#include "EntityCreatureBullet.h"
#include "EntityPlayer.h"
#include "EntityCreature.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	EntityCreatureBullet::EntityCreatureBullet(World* pWorld, int throwerId, int targetId)
	: EntityCreatureBullet(pWorld)
	{
		m_throwerId = throwerId;
		m_targetId = targetId;
		setPosition(pWorld->getEntity(throwerId)->getPosition());
		setSize(0.25F, 0.25F);
		currUpdateTime = Root::Instance()->getCurrentTime();
		lastUpdateTime = currUpdateTime;
	}

	EntityCreatureBullet::~EntityCreatureBullet()
	{
		if (bombEffect != NULL)
		{
			bombEffect->mDuration = 0;
			bombEffect = NULL;
		}
	}

	String EntityCreatureBullet::getBombEffectName()
	{
		if (!world->m_isClient)
			return "";

		String effectName = m_strBombEffect;
		switch (getType())
		{
		case CreatureBulletType::EffectBomb:
            if (effectName == "")
            {
                effectName = "tower_bomb.effect";
            }
			break;
		default:
			break;
		}
		return effectName;
	}

	void EntityCreatureBullet::initBombEffect()
	{
		String effectName = getBombEffectName();
		if (effectName.length() <= 7)
			return;
		lastUpdateTime = Root::Instance()->getCurrentTime() - 1;
		onUpdate();
		if (bombEffect)
			WorldEffectManager::Instance()->removeSimpleEffect(bombEffect->mEffectName);
		bombEffect = WorldEffectManager::Instance()->addSimpleEffect(effectName, position, rotationYaw, -1, m_effectScale, m_effectScale, m_effectScale);
	}

	void EntityCreatureBullet::onUpdate()
	{
		currUpdateTime = Root::Instance()->getCurrentTime();
		lastTickPos = position;

		if (m_throwerId == 0 || m_targetId == 0)
		{
			setDead();
			return;
		}

		Entity* targeter = world->getEntity(m_targetId);
		if (!targeter || targeter->isDead)
		{
			setDead();
			return;
		}

		Vector3 begin = position;
		Vector3 end = targeter->getPosition();
		if (targeter->isClass(ENTITY_CLASS_PLAYER))
			end.y += 0.2f;
		else
			end.y += targeter->getHeight() * targeter->getScale() / 2;

		float distance = Math::Max(begin.distanceTo(end), 0.00001F);
		float temp = 0.5f * ((currUpdateTime - lastUpdateTime) / 20.0f);
        motion = m_speedFactor * temp * (end - begin) / distance;
		
		if (motion.len() > distance)
		{
			IntProxy next_targetId;
			next_targetId.value = 0;
			if (targeter->isClass(ENTITY_CLASS_PLAYER))
				SCRIPT_EVENT::CreatureAttackPlayerEvent::invoke(m_targetId, m_throwerId, this->hashCode(), &next_targetId);
			else
				SCRIPT_EVENT::CreatureAttackCreatureEvent::invoke(m_targetId, m_throwerId, this->hashCode(), &next_targetId);
			m_targetId = next_targetId.value;
			if (m_targetId == 0)
			{
				setDead();
				return;
			}
		}

		position += motion;

		Vector3 dir = end - begin;
		rotationYaw = (float)atan2(dir.x, dir.z) * (180.0f / Math::PI);

		setPosition(position);
		onEffectUpdate();

		lastUpdateTime = currUpdateTime;
	}

	void EntityCreatureBullet::onEffectUpdate()
	{
		if (bombEffect == NULL)
			return;
		bombEffect->mPosition = position;
		bombEffect->mYaw = rotationYaw;
	}

	void EntityCreatureBullet::setYawY(float yawY)
	{
		float needYaw = yawY - m_yawY;
		auto m_pos = getPosition();
		m_pos.y += needYaw;
		setPosition(m_pos);
		m_yawY = yawY;
	}

}