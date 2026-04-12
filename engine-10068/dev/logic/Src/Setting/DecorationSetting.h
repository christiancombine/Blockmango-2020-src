#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class  Decoration {
	public:
		i32 id = 0;
		i32 type = 0;
		i32 itemId = 0;
		i32 actionId = 0;
		i32 interactionType = 0;

		float width = 0;
		float length = 0;
		float height = 0;

		String icon = "";
		String name = "";
		String actorName = "";
		String actorBody = "";
		String actorBodyId = "";
		String awaitAnimate = "";
		String useAnimate = "";
		String meshName = "";
		bool isCanCollided = false;
		vector<String>::type m_nodes;
		map<String, String>::type awaitAnimates;
	};

	class DecorationSetting
	{
	public:
		static bool loadSetting(bool isClient);
		static void unloadSetting();

	public:
		static  Decoration* findItem(i32 id);
		static  Decoration* findItemByItemId(i32 itemIdid);
		static bool canPlayAction(i32 id);
		static String getAction(i32 id, map<String, ui64>::type nodes);

	private:
		static std::map<i32, Decoration*> m_decorationMap;
	};
}
