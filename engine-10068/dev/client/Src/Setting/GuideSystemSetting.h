#pragma once
#ifndef __GUIDE_SYSTEM_SETTING_HEADER__
#define __GUIDE_SYSTEM_SETTING_HEADER__

#include "Core.h"
#include "World/WorldEffectManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum GuideStatus
	{
		STATUS_LOCKED = 0,
		STATUS_NOT_TRIGGER,
		STATUS_ACTIVATED,
		STATUS_DONE,
	};

	enum ConditionType
	{
		CONDITION_NONE = 0,
		CONDITION_ARRIVE_AREA,
		CONDITION_CLICK,
		CONDITION_ENTER_GAME,
		CONDITION_COMPLETE_GUIDE,
		CONDITION_UI_POP,
	};

	enum EventType
	{
		EVENT_NONE = 0,
		EVENT_POP_UI,
		EVENT_TRANSFER,
		EVENT_SCREEN_EFFECT, 
		EVENT_GUIDE_LINK,
		EVENT_WORLD_EFFECT,
		EVENT_SHOW_TIP,
		EVENT_CHOOSE_PAPER,
	};

	enum Indie
	{
		INDIE_COMMON = 0,
		INDIE_INDIE,
		INDIE_PIXEL,
	};

	class ConditionItem
	{
	public:
		int Id = 0;
		ConditionType Type = CONDITION_NONE;
		String Remark = "";
		String PosStart = "";
		String PosEnd = "";
	};

	class EventItem
	{
	public:
		int Id = 0;
		EventType Type = EVENT_NONE;
		String Remark = "";
		String Pos = "";
	};

	class GuideItem
	{
	public:
		int Id = 0;
		bool IsCompel = false;
		int ResetId = 0;
		int SkipId = 0;
		GuideStatus Status = STATUS_LOCKED;
		ConditionItem* Trigger = NULL;
		EventItem* Event = NULL;
		ConditionItem* Complete = NULL;
		int Lv = 0;
	};

	class GuideSystemSetting
	{
	private:
		static map<int, ConditionItem*>::type m_condition_map;
		static map<int, EventItem*>::type m_event_map;
		static map<int, GuideItem*>::type m_guide_map;
		static list<GuideItem*>::type m_under_way_guide_list;
		static map<int, SimpleEffect*>::type m_world_effect_map;
		static int m_max_lv;

	public:
		static void	loadSetting();
		static void	unloadSetting();

	private:
		static void	loadGuideSetting();
		static void	loadConditionSetting();
		static void	loadEventSetting();

	public:
		static ConditionItem* findConditionById(int id);
		static EventItem* findEventById(int id);
		static GuideItem* findGuideById(int id);
		static void parseGuideData(const String & data);
		static void setGuideStatus(int id, GuideStatus status);
		static void updateGuideStatus(int id, GuideStatus status);
		static void updateUnderWayGuideList(GuideItem* guide);
		static void unlockGuides(int cur_lv);
		static void sendGuideToServer(GuideItem* guide);
		static void onGuideStatusChange(GuideItem* guide);
		static void onGuideTriggered(GuideItem* guide);
		static void onGuideTriggered(int guideId);
		static void onGuideCompleted(String remark);
		static void onPlayerMove(Vector3 pos);
		static bool isInArea(Vector3 pos, Vector3 startPos, Vector3 endPos);
		static void onEnterGame();
		static void removeWorldEffect(int id);
		static void setClickConditon(GuideItem* guide);
		static void onPlayerSkip(int skipId, int guideId);
		static void onGuideDone(int guideId);
		static void onWindowShown(String name);
	};
}
#endif 
