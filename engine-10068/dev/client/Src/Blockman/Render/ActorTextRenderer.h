#ifndef __HEAD_ACTOR_TEXT_RENDERER_HEADER__
#define __HEAD_ACTOR_TEXT_RENDERER_HEADER__

#include "Core.h"

namespace BLOCKMAN
{
	enum TextAlign
	{
		Left,
		ENTRE,
		RIGHT
	};

	class ActorTextRenderer
	{
	public:
		static void renderActorText(const LORD::String& txt, LORD::Vector3 pos, bool canBeOccluded, TextAlign align, float lineWidth, float lineHeight = 0.3f, float scale = 1.0f, float yaw = 0.0f, float pitch = 0.0f);
	};
}

#endif // !__HEAD_TEXT_RENDERER_HEADER__