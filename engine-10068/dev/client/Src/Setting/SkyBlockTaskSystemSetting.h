#pragma once
#ifndef __SKYBLOCK_TASK_SYSTEM_SETTING_HEADER__
#define __SKYBLOCK_TASK_SYSTEM_SETTING_HEADER__

#include "Core.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum SkyBlockTaskType
	{
		SKYBLOCK_TASK_TYPE_NONE = 0,
		SKYBLOCK_TASK_TYPE_MAINLINE = 1,
		SKYBLOCK_TASK_TYPE_DRAE = 2
	};

	enum SkyBlockTaskStatus
	{
		SKYBLOCK_TASK_STATUS_LOCK = 0,
		SKYBLOCK_TASK_STATUS_HAVE_GOT = 1,
		SKYBLOCK_TASK_STATUS_UNDONE = 2,
		SKYBLOCK_TASK_STATUS_NOT_GET = 3,
		SKYBLOCK_TASK_STATUS_CAN_COMMIT = 4,
		SKYBLOCK_TASK_STATUS_AD = 5
	};

	enum SkyBlockSkipType
	{
		SKYBLOCK_SKIP_NONE = 0,
		SKYBLOCK_SKIP_FIGHT = 1,
		SKYBLOCK_SKIP_GUNSTORE = 2,
		SKYBLOCK_SKIP_ARMOR = 3,
		SKYBLOCK_SKIP_LOTTERY = 4,
	};

	enum SkyBlockDareTaskQuality
	{
		SKYBLOCK_DARE_TASK_LEVEL_1 = 1,
		SKYBLOCK_DARE_TASK_LEVEL_2 = 2,
		SKYBLOCK_DARE_TASK_LEVEL_3 = 3,
		SKYBLOCK_DARE_TASK_LEVEL_4 = 4,
	};

	enum SkyBlockTaskDetailType
	{
		SKYBLOCK_COLLECT = 101,
		SKYBLOCK_CREATE = 102,
		SKYBLOCK_STONE_MACHINE = 103,
		SKYBLOCK_UNLIMITE_WATER = 104,
		SKYBLOCK_PLANTE_TREE = 105,
		SKYBLOCK_EXPAND_ISLANDS = 106,
		SKYBLOCK_LOOK_AD = 107,
		SKYBLOCK_KILL_MONSTER = 108,
		SKYBLOCK_HAVE_PRODUCTS = 109,
		SKYBLOCK_UP_PRODUCTS = 110,
		SKYBLOCK_PRODUCTS_LEVEL = 111,
		SKYBLOCK_KILL_PLAYER = 112
	};

	class SkyBlockTask
	{
	public:
		int Id = 0;
		String Title = "";
		String Icon = "";
		SkyBlockTaskStatus Status = SKYBLOCK_TASK_STATUS_LOCK;
		int Level = 0;
		int GoalNum = 1;
		int SortValue = 0;
		int NextTaskId = 0;
		String RewardIds = "";
		SkyBlockTaskType Type = SKYBLOCK_TASK_TYPE_NONE;
		int TaskTypeId = 0;
		int Remarks = 0;
		int RemarksMeta = 0;
		int Current = 0;
		bool HasChange = false;

		int Weight = 0;
		int TaskSort = SKYBLOCK_TASK_TYPE_DRAE;
		int Quality = SKYBLOCK_DARE_TASK_LEVEL_1;
	};

	bool sortMainLineTaskFunc(SkyBlockTask task1, SkyBlockTask task2);

	class SkyBlockTaskReward
	{
	public:
		int Id = 0;
		int Type = 0;
		int RewardId = 0;
		int Num = 0;
		int Meta = 0;
		String Icon = "";
	};

	class SkyBlockTaskGroup
	{
	public:
		int Lv = 1;
		int MainGroupId = 0;
		list<SkyBlockTask>::type Tasks;
	};

	class SkyBlockTaskSystemSetting : public Singleton<SkyBlockTaskSystemSetting>, public ObjectAlloc
	{
	private:
		map<int, SkyBlockTask>::type m_task_map;
		map<int, SkyBlockTaskGroup>::type m_task_mainline_map;
		map<int, SkyBlockTaskReward>::type m_task_reward_map;
		SkyBlockTaskGroup  m_task_Dare_GroUp;

	public:
		SkyBlockTaskSystemSetting();
		~SkyBlockTaskSystemSetting();
		void loadSetting();
		void loadRewardSetting();
		void unloadSetting();

		bool findTaskById(int id, SkyBlockTask& task);
		bool findTaskRewardById(int id, SkyBlockTaskReward& taskReward);
		bool findTaskRewardByIds(String rewardIds, list<SkyBlockTaskReward>::type& reward);
		bool findTaskGroupByLevel(int level, SkyBlockTaskGroup& taskGroup);

		bool getMainLineTasks(map<int, SkyBlockTaskGroup>::type& data);
		int getWaitRewardCount();

		// static bool sortTaskFunc(SkyBlockTask task1, SkyBlockTask task2);
		//list<SkyBlockTask>::type sortTasks(list<SkyBlockTask>::type &stasks);

		bool parseUpdateMainLineTaskData(const String & data, map<int, SkyBlockTaskGroup>::type& mainLineGroup);
		bool parseUpdateDareTaskData(const String & data, SkyBlockTaskGroup& result);
		//map<int, SkyBlockTaskGroup>::type sortMainLineTasks(map<int, SkyBlockTaskGroup>::type &cur_map);

	private:
		void loadTaskSetting();
		void setMainlineGroup();
	};
}

#endif 
