#ifndef __ENTITY_CREATURE_BULLET_HEADER__
#define __ENTITY_CREATURE_BULLET_HEADER__

#include "Entity.h"
#include "Entity/EntityLivingBase.h"
#include "World/World.h"
#include "World/WorldEffectManager.h"

namespace BLOCKMAN
{
	enum CreatureBulletType
	{
		Default = 0, 
		SpriteBomb = 1,
		EffectBomb = 2,
		SpriteAndEffectBomb = 3,
	};

	class EntityCreatureBullet : public Entity
	{
		RTTI_DECLARE(EntityCreatureBullet);
	private:
		int m_throwerId = 0;
		int m_targetId = 0;

		int currUpdateTime = 0;
		int lastUpdateTime = 0;
		float m_yawY = 0;
        float m_speedFactor = 1.0F;
		float m_effectScale = 1.0F;
		float m_spriteScale = 0.5F;
		Vector3 m_box = Vector3::ONE;
		CreatureBulletType m_type;
		String m_jsonName = "";
		String m_spriteName = "";
        String m_strBombEffect{""}; //Զ�̹���Ч��
		SimpleEffect* bombEffect = NULL;

	public:
		EntityCreatureBullet(World* pWorld)
			: Entity(pWorld)
		{
			m_type = CreatureBulletType::Default;
		}
		EntityCreatureBullet(World* pWorld, int throwerId, int targetId);
		~EntityCreatureBullet();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}

		void onUpdate() override;
		void onEffectUpdate();
		void setYawY(float yawY);
		void setSpeedFactor(float speed) { m_speedFactor = speed; }
		void setSprite(String jsonName, String spriteName) {
			m_jsonName = jsonName;
			m_spriteName = spriteName;
		}
		void setBombEffect(String effect) { 
			if (effect.empty())
				return;
			if (m_jsonName.empty() || m_spriteName.empty())
				m_type = CreatureBulletType::EffectBomb;
			else
				m_type = CreatureBulletType::SpriteAndEffectBomb;
			m_strBombEffect = effect; 
			initBombEffect();
		}
		void setBox(Vector3 box) { m_box = box; }
		void setEffectScale(float effectScale) { 
			m_effectScale = effectScale; 
			initBombEffect();
		}
		void setSpriteScale(float spriteScale) { m_spriteScale = spriteScale; }
		float getSpeedFactor() const { return m_speedFactor; }
		float getYawY() const { return m_yawY; }
		float getEffectScale()const { return m_effectScale; }
		float getSpriteScale()const { return m_spriteScale; }
		int getThrowerId() { return m_throwerId; }
		int getTargetId() { return m_targetId; }
		CreatureBulletType getType() { return m_type; }
		String getJsonName(){ return m_jsonName; }
		String getSpriteName() { return m_spriteName; }
		String getBombEffect() { return m_strBombEffect; }
		//void setBombEffect(const String& bombEffect) { m_strBombEffect = bombEffect; }
		virtual bool isTrackerSyncMove() { return false; }
	private:
		String getBombEffectName();
		void initBombEffect();

	};
}
#endif