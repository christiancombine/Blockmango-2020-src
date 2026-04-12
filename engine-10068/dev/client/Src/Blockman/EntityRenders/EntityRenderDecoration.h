#ifndef __ENTITY_RENDER_DECORATION_HEADER__
#define __ENTITY_RENDER_DECORATION_HEADER__
#include "EntityRender.h"
namespace BLOCKMAN
{
	class EntityDecoration;

	class EntityRenderDecoration : public EntityRender
	{

	public:
		EntityRenderDecoration();
		virtual ~EntityRenderDecoration();
		/** implement override functions from RenderLiving */
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float ticks) override;
	};
}
#endif
