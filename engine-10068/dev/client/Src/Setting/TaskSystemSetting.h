#pragma once
#ifndef __TASK_SYSTEM_SETTING_HEADER__
#define __TASK_SYSTEM_SETTING_HEADER__

#include "Core.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum TaskType
	{
		TASK_TYPE_NONE = 0,
		TASK_TYPE_DAILY = 1,
		TASK_TYPE_MAINLINE = 2
	};

	enum TaskStatus
	{
		TASK_STATUS_LOCK = 0,
		TASK_STATUS_HAVE_GOT = 1,
		TASK_STATUS_UNDONE = 2,
		TASK_STATUS_NOT_GET = 3
	};

	enum SkipType
	{
		SKIP_NONE = 0,
		SKIP_FIGHT = 1,
		SKIP_GUNSTORE = 2,
		SKIP_ARMOR = 3,
		SKIP_LOTTERY = 4,
	};

	enum TaskActiveType
	{
		ACTIVE_TYPE_DAY = 1, 
		ACTIVE_TYPE_WEEK = 2
	};

	enum TaskActiveStatus
	{
		ACTIVE_STATUS_UNDONE = 0,
		ACTIVE_STATUS_DONE = 1,
		ACTIVE_STATUS_RECEIVE = 2
	};

	class Task
	{
	public:
		int Id = 0;
		String Title = "";
		String Icon = "";
		TaskStatus Status = TASK_STATUS_LOCK;
		int MainGroupId = 0;
		int GoalNum = 1;
		int SortValue = 0;
		int ActiveValue = 0;
		int NextTaskId = 0;
		String RewardIds = "";
		SkipType JumpTo = SKIP_NONE;
		TaskType Type = TASK_TYPE_NONE;

		int GroupId = 0;
		int TaskLinkId = 0;
		int Current = 0;
		bool HasChange = false;
		bool ShowAd = false;
	};

	class TaskGroup
	{
	public:
		int Id = 0;
		int Lv = 0;
		int MainGroupId = 0;
		list<Task*>::type Tasks;
	};

	class TaskActive
	{
	public:
		int Id = 0;
		TaskActiveType Type = ACTIVE_TYPE_DAY;
		String Actor = "";
		int ActiveValue = 0;
		String RewardIds = "";

		TaskActiveStatus Status = ACTIVE_STATUS_UNDONE;
		bool HasChange = false;
	};

	class TaskSystemSetting
	{
	private:
		static map<int, Task*>::type m_task_map;
		static list<Task*>::type m_daily_task_list;
		static map<int, TaskGroup*>::type m_task_mainline_map;
		static map<int, TaskActive*>::type m_task_active_map;

	public:
		static void	loadSetting();
		static void	unloadSetting();

		static Task* findTaskById(int id);
		static TaskActive * findTaskActiveById(int id);
		static list<Task*>::type findMainLineTaskByMainGroupId(int groupId, int mainGroupId);
		static Task* findDailyTaskById(int id);
		static TaskGroup* findTaskGroupByGroupId(int groupId);
		static void removeDailyTaskByNextId(int id);

		static list<Task*>::type getDailyTasks();
		static map<int, TaskGroup*>::type getMainLineTasks() { return m_task_mainline_map; }
		static map<int, TaskActive*>::type getTaskActives() {return m_task_active_map;}
		static list<TaskActive*>::type getDayActiveList();
		static list<TaskActive*>::type getWeekActiveList();
		static int getWaitRewardCount();

		static bool sortTaskFunc(Task* task1, Task* task2);
		static list<Task*>::type sortTasks(list<Task*>::type tasks);

		static map<int, TaskGroup*>::type parseUpdateMainLineTaskData(const String & data);
		static list<Task*>::type parseUpdateDailyTaskData(const String & data);
		static map<int, TaskGroup*>::type sortMainLineTasks(map<int, TaskGroup*>::type map);

	private:
		static void loadTaskSetting();
		static void loadGroupSetting();
		static void loadTaskActiveSetting();
	};
}

#endif 
