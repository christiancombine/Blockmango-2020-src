#pragma once

#ifndef __ENTITY_ACTOR_NPC_HEADER__
#define __ENTITY_ACTOR_NPC_HEADER__
#include "Entity.h"
#include "World/World.h"
namespace BLOCKMAN
{
	class EntityActorNpc : public Entity
	{
		RTTI_DECLARE(EntityActorNpc);
	private:
		String m_actorName = "";
		String m_headName = "";
		String m_skillName = "";
		String m_content = "";
		String m_HaloEffectName = "";
		bool canObstruct = true;
		bool m_canCollided = true;
		bool m_suspended = false;
		bool isFlying = false;
		String m_weight = ""; //重量(使用大数处理)

	public:
		EntityActorNpc(World* pWorld) : Entity(pWorld)
		{
			preventEntitySpawning = true;
		}
		EntityActorNpc(World* pWorld, const Vector3& pos) : EntityActorNpc(pWorld)
		{
			setPosition(pos);
		}
		~EntityActorNpc();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;

		bool canBeCollidedWith() { return m_canCollided; }
		bool canBePushed() { return true; }
		void setWeight(const String& weight) { m_weight = weight; }
		void setActorName(const String& actorName);
		virtual void setScale(float s) override;
		virtual bool isFlyEntity() { return isFlying; }
		void setFlying(bool allow) { isFlying = allow; }

		void setHeight(float h);
		float getHeight() { return height; }
		String getActorName() const
		{
			return m_actorName;
		}

		void setHeadName(const String& headName)
		{
			m_headName = headName;
		}

		String getHeadName() const
		{
			return m_headName;
		}

		void setSkillName(const String& skillName)
		{
			m_skillName = skillName;
		}

		String getSkillName()
		{
			return m_skillName;
		}

		void setHaloEffectName(const String& haloEffectName)
		{
			m_HaloEffectName = haloEffectName;
		}

		String getHaloEffectName()
		{
			return m_HaloEffectName;
		}

		bool isShowContent()
		{
			return m_content.length() > size_t(0);
		}

		void setContent(const String& content)
		{
			m_content = content;
		}

		String getContent()
		{
			return m_content;
		}

		void setSuspended(bool sign, Vector3 pos) { m_suspended = sign; setPosition(pos); }
		bool getSuspended() { return m_suspended; }
		String getWeight() {
			return m_weight;
		};
		bool isImmuneToExplosions() { return true; }

		bool isCanObstruct() { return canObstruct; }

		void setCanObstruct(bool canObstruct) { this->canObstruct = canObstruct; }

		void setCanCollided(bool canCollided) { m_canCollided = canCollided; }

		void setNoClip(bool canNoClip) { noClip = canNoClip; }
	};
}
#endif
