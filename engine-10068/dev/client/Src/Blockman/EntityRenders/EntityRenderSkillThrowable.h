#ifndef __ENTITY_RENDER_SKILL_THROWABLE_HEADER__
#define __ENTITY_RENDER_SKILL_THROWABLE_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{
	class AtlasSprite;
	class EntitySkillThrowable;

	class EntityRenderSkillThrowable : public EntityRender
	{
	private:
		AtlasSprite* sprite = nullptr;
	public:
		EntityRenderSkillThrowable();
		~EntityRenderSkillThrowable();
		/** implement override functions for EntityRender */
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);

	private:
		void renderActor(EntitySkillThrowable* entity, const Vector3& pos, float yaw, float rdt);

	protected:
		void render_impl(const Vector3& pos, int brightness, int color, AtlasSprite* sprite);

	};

}

#endif