#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class Bulletin {
	public:
		int id = 0;
		String name = "";
		String actorName = "";
		String actorBody = "";
		String actorBodyId = "";
		String awaitAnimate = "";
		float length = 0;
		float width = 0;
		float height = 0;
		i32 lineNum = 0;
		float offsetTop = 0.f;
		float offsetWidth = 0.f;
		float lineSpace = 0.f;
		float lineHeight = 0.f;
		float lineWidth = 0.f;
		bool isCanBeCollided = false;
	};

	class BulletinSetting
	{
	public:
		static bool loadSetting(bool bIsClient, bool isMap = false);
		static void unloadSetting();

	public:
		static Bulletin* findBulletinById(int id);

	private:
		static String getPath(bool bIsClient, bool isMap);

		static std::map<int, Bulletin*> m_bulletinMap;
	};
}
