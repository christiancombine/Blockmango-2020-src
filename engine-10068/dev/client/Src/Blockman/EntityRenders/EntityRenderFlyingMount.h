#pragma once
#include "EntityRenders/EntityRender.h"

namespace BLOCKMAN
{
	class EntityRenderFlyingMount : public EntityRender
	{
	private:
		void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;
	};
}