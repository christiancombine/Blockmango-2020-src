#ifndef __ACTOR_SIZE_SETTING_HEADER__
#define __ACTOR_SIZE_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class ActorSize
	{
	public:
		float width = 0.0f;
		float length = 0.0f;
		float height = 0.0f;
		float scale = 1.0f;
	public:
		ActorSize() :
			width(0.f), length(0.f), height(0.f), scale(1.0f)
		{}

		ActorSize(float w, float l, float h, float s = 1.0f) :
			width(w), length(l), height(h), scale(s)
		{}
	};

	class ActorSizeSetting
	{
	public:
		static bool	loadSetting(bool isClient);
		static bool loadMapSetting(bool isClient);
		static void	unloadSetting();
		static ActorSize* getActorSize(String actorName);
		static bool isHaveActor(String actorName);
	private:
		static std::map<String, ActorSize*> m_ActorSizeMap;
	};
}

#endif // !__ACTOR_SIZE_SETTING_HEADER__
