#include "EntityCreatureAI.h"
#include "Setting/MonsterSetting.h"
#include "Setting/SkillSetting.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Entity/EntityCreatureBullet.h"
#include "Script/GameServerEvents.h"
#include "Script/Event/LogicScriptEvents.h"
#include "BtTreeController.h"

namespace BLOCKMAN
{
	EntityCreatureAI::EntityCreatureAI(World * pWorld, int monsterId, Vector3 & pos, float yaw, String actorName)
		: EntityCreature(pWorld, monsterId, pos, yaw, actorName)
	{
	}

	EntityCreatureAI::~EntityCreatureAI()
	{
		unloadAgent();
	}

	void EntityCreatureAI::onUpdate()
	{
		EntityCreature::onUpdate();
	}

	void EntityCreatureAI::onLivingUpdate()
	{
		EntityCreature::onLivingUpdate();
	}

	void EntityCreatureAI::setPosition(const Vector3 & pos)
	{
		EntityCreature::setPosition(pos);
	}

	bool EntityCreatureAI::attackEntityFrom(DamageSource * pSource, float amount)
	{

		if (pSource && pSource->getEntity())
		{
			EntityPlayerMP* player = dynamic_cast<EntityPlayerMP*>(pSource->getEntity());
			player->attackCreatureEntity(this, (int)PLAYER_ATTACK_CREATURE_DAMAGE);
		}
		return false;
	}

	void EntityCreatureAI::selectBtTreeByMonsterId(int monsterId)
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(monsterId);
		if (pSetting)
		{
			if (pSetting->monsterBtTree.size() > 0)
				setCurrentAI((pSetting->monsterBtTree).c_str());

			setSpeedAdditionLevel(pSetting->moveSpeed);

			m_monsterType = pSetting->monsterType;
		}
	}

	void EntityCreatureAI::setCurrentAI(const char* pBtTree)
	{
		m_sLoadBTTree = pBtTree;
		loadAgent();
	}

	void EntityCreatureAI::setAIActive(bool bActive)
	{
		if (m_pAgentPlayer)
		{
			m_pAgentPlayer->SetActive(bActive);
		}
	}

	void EntityCreatureAI::setEntityHealth(float amount)
	{
		EntityLivingBase::setEntityHealth(amount);
		ServerNetwork::Instance()->getSender()->syncEntityHealth(0, entityId, getHealth(), getMaxHealth());
	}

	void EntityCreatureAI::unloadAgent()
	{
		if (m_btTreeId > 0)
			BtTreeController::Instance()->removeBtTree(m_btTreeId);
		m_pAgentPlayer = NULL;
	}

	void EntityCreatureAI::notifyAttackFrom(EntityPlayer* pEntity)
	{
		MonsterG1049 *pAgent = dynamic_cast<MonsterG1049*>(m_pAgentPlayer);
		if (pAgent)
		{
			pAgent->notifyAttacked(pEntity);

			/*
			float strength = 1.0;
			float xRadio = 0.0;
			float zRadio = 0.0;
			float lensqr = 0.0;

			float f1 = Math::Sin(rotationYaw * Math::PI / 180.0F);
			float f2 = Math::Cos(rotationYaw * Math::PI / 180.0F);

			motion.x = f1 * 0.2;
			motion.y = 0;
			motion.z = f2 * 0.2;


			float xRatio = pEntity->position.x - position.x;
			float zRatio = pEntity->position.z - position.z;

			float lensqr = Math::Sqrt(xRatio * xRatio + zRatio * zRatio);

			motion /= 2.0f;
			motion.x -= xRatio / (lensqr * 4 * 2);
			motion.y = 0;
			motion.z -= zRatio / (lensqr * 4 * 2);

			if (this->onGround)
			{
				motion.y = 0.4f;
			}
			*/
		}
	}

	bool EntityCreatureAI::loadAgent()
	{
		auto setting = MonsterSetting::getMonsterSetting(this->m_MonsterId);
		if (setting)
		{
			if (setting->monsterType == CRETURE_AI_TYPE::CRETURE_AI_TYPE_G1049)
			{
				auto agent = behaviac::Agent::Create<MonsterG1049>();
				agent->init(setting, this);
				m_pAgentPlayer = dynamic_cast<BaseAgent*>(agent);
			}
			else
			{
				auto agent = behaviac::Agent::Create<BaseAgent>();
				agent->setEntity(this);
				m_pAgentPlayer = agent;
			}
			m_btTreeId = BtTreeController::Instance()->addBtTreeFromAgent(m_sLoadBTTree.c_str(), m_pAgentPlayer);
		}
		return true;
	}

	bool EntityCreatureAI::changeCreatureAction(CreatureActionState nActionStatus)
	{
		bool isChange = EntityCreature::changeCreatureAction(nActionStatus);

		if (isChange && !isBase())
		{
			ServerNetwork::Instance()->getSender()->sendCreatureEntityAction(0, entityId, nActionStatus, (int)getCreatureAction(), (int)getPreCreatureAction());
		}

		return isChange;
	}

	void EntityCreatureAI::onAttackPlayer(EntityPlayer * player)
	{
		IntProxy next_targetId;
		SCRIPT_EVENT::CreatureAttackPlayerEvent::invoke(player->hashCode(), hashCode(), 0, &next_targetId);
	}

	void EntityCreatureAI::attackPerformance(EntityLivingBase* targeter)
	{
		if (getCreatureAction() == CREATURE_AS_DEATH)
			return;
		CRETURE_AI_ATTACK_TYPE attackType = EntityCreature::getCreatureAttackType();
		EntityCreatureBullet* creatureBullet = nullptr;
		IntProxy next_targetId;
		next_targetId.value = 0;
		switch (attackType)
		{
		case CRETURE_AI_ATTACK_TYPE_MELEE:
			switch (targeter->getClassID())
			{
			case ENTITY_CLASS_PLAYERMP:
				SCRIPT_EVENT::CreatureAttackPlayerEvent::invoke(targeter->hashCode(), this->hashCode(), 0, &next_targetId);
				break;
			case ENTITY_CLASS_CREATUREAI:

				SCRIPT_EVENT::CreatureAttackCreatureEvent::invoke(targeter->hashCode(), this->hashCode(), 0, &next_targetId);
				break;
			default:
				break;
			}
			break;
		case CRETURE_AI_ATTACK_TYPE_REMOTE:
		{
			auto pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
			String bombEffect = pSetting ? pSetting->bombEffect : "";
			creatureBullet = LordNew EntityCreatureBullet(world, this->hashCode(), targeter->hashCode());
			if (bombEffect != "")
			{
				creatureBullet->setBombEffect(bombEffect);
			}
		}
		break;
		case CRETURE_AI_ATTACK_TYPE_TOWER:
		{
			auto pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
			String bombEffect = pSetting ? pSetting->bombEffect : "";
			float speedFactor = pSetting ? pSetting->remoteSpeedFactor : 1.0F;
			creatureBullet = LordNew EntityCreatureBullet(world, this->hashCode(), targeter->hashCode());
			creatureBullet->setBombEffect(bombEffect);
			creatureBullet->setSpeedFactor(speedFactor);
		}
		break;
		case CRETURE_AI_ATTACK_TYPE_GROUP:
			SCRIPT_EVENT::CreatureAttackPlayerEvent::invoke(targeter->hashCode(), this->hashCode(), 0, &next_targetId);
			break;
		default:
			break;
		}
		if (creatureBullet)
			world->spawnEntityInWorld(creatureBullet);
	}

	void EntityCreatureAI::groupAttackPerformance()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (!pSetting)
			return;
		sendSkillEffect(pSetting->skillId);
	}

	void EntityCreatureAI::sendSkillEffect(int effectId)
	{
		SkillEffect* pSkill = SkillSetting::getSkillEffect(effectId);
		if (!pSkill)
			return;
		if (pSkill->effectType == 0)
		{
			ServerNetwork::Instance()->getSender()->sendEntitySkillEffect(position, pSkill->name, pSkill->duration,
				pSkill->width, pSkill->height, pSkill->color, pSkill->density);
		}
		else
		{
			ServerWorld *serverWorld = dynamic_cast<ServerWorld*>(world);
			serverWorld->addSimpleEffect(pSkill->name, position, rotationYaw, pSkill->duration, 0, pSkill->density);
		}
	}

	void EntityCreatureAI::playDeathEffect()
	{
		const MonsterSetting* pSetting = MonsterSetting::getMonsterSetting(m_MonsterId);
		if (!pSetting)
			return;

		if (pSetting->deathEffectName.empty())
			return;

		ServerWorld *serverWorld = dynamic_cast<ServerWorld*>(this->world);
		serverWorld->addSimpleEffect(pSetting->deathEffectName, this->position, this->rotationYaw, 5000);
	}
}