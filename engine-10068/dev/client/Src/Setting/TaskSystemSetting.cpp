#include "TaskSystemSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<int, Task*>::type TaskSystemSetting::m_task_map = {};
	list<Task*>::type TaskSystemSetting::m_daily_task_list = {};
	map<int, TaskGroup*>::type TaskSystemSetting::m_task_mainline_map = {};
	map<int, TaskActive*>::type TaskSystemSetting::m_task_active_map = {};

	void TaskSystemSetting::loadSetting()
	{
		loadTaskSetting();
		loadGroupSetting();
		loadTaskActiveSetting();
	}

	void TaskSystemSetting::unloadSetting()
	{
		for (auto iter : m_task_map)
		{
			delete iter.second;
		}
		m_task_map.clear();
		m_daily_task_list.clear();
		for (auto iter : m_task_mainline_map)
		{
			delete iter.second;
		}
		m_task_mainline_map.clear();
		for (auto iter : m_task_active_map)
		{
			delete iter.second;
		}
		m_task_active_map.clear();
	}

	void TaskSystemSetting::loadTaskSetting()
	{
		Task* pTask = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "TaskSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pTask = new Task();
			pReader->GetInt("Id", pTask->Id);
			pReader->GetInt("GroupId", pTask->MainGroupId);
			pReader->GetString("Icon", pTask->Icon);
			pReader->GetString("Title", pTask->Title);
			pReader->GetInt("ActiveValue", pTask->ActiveValue);
			int Skip = 0;
			pReader->GetInt("JumpTo", Skip);
			pTask->JumpTo = (SkipType)Skip;
			pReader->GetInt("SortValue", pTask->SortValue);
			pReader->GetString("RewardIds", pTask->RewardIds);
			pReader->GetInt("GoalNum", pTask->GoalNum);
			pTask->Current = pTask->GoalNum;
			pReader->GetInt("NextTaskId", pTask->NextTaskId);
			int ShowAd = 0;
			pReader->GetInt("ShowAd", ShowAd);
			pTask->ShowAd = ShowAd == 1;
			m_task_map[pTask->Id] = pTask;
		}
		
		LordSafeDelete(pReader);
	}

	void TaskSystemSetting::loadGroupSetting()
	{
		TaskGroup* pGroup = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "TaskGroupSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			int Type = 0;
			pReader->GetInt("TaskType", Type);
			if (((TaskType)Type) != TASK_TYPE_MAINLINE)
			{
				continue;
			}
			pGroup = new TaskGroup();
			pReader->GetInt("Id", pGroup->Id);
			pReader->GetInt("MaxLevel", pGroup->Lv);
			pReader->GetInt("MainGroupId", pGroup->MainGroupId);
			pGroup->Tasks = findMainLineTaskByMainGroupId(pGroup->Id, pGroup->MainGroupId);
			m_task_mainline_map[pGroup->Id] = pGroup;
		}
		
		LordSafeDelete(pReader);
	}

	void TaskSystemSetting::loadTaskActiveSetting()
	{
		TaskActive* pTaskActive = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "TaskActiveSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pTaskActive = new TaskActive();
			pReader->GetInt("Id", pTaskActive->Id);
			int Type = 1;
			pReader->GetInt("Type", Type);
			pTaskActive->Type = (TaskActiveType)Type;
			pReader->GetString("Actor", pTaskActive->Actor);
			pReader->GetInt("ActiveValue", pTaskActive->ActiveValue);
			pReader->GetString("RewardIds", pTaskActive->RewardIds);
			m_task_active_map[pTaskActive->Id] = pTaskActive;
		}
		
		LordSafeDelete(pReader);
	}

	Task * TaskSystemSetting::findTaskById(int id)
	{
		for (auto iter : m_task_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	TaskActive * TaskSystemSetting::findTaskActiveById(int id)
	{
		for (auto iter : m_task_active_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	list<Task*>::type TaskSystemSetting::findMainLineTaskByMainGroupId(int groupId, int mainGroupId)
	{
		list<Task*>::type result;
		for (auto iter : m_task_map)
		{
			if (iter.second->MainGroupId == mainGroupId)
			{
				auto task = iter.second;
				task->GroupId = groupId;
				int task_link_id = task->Id;
				task->TaskLinkId = task_link_id;
				while (task->NextTaskId != 0)
				{
					task = findTaskById(task->NextTaskId);
					if (task == nullptr)
					{
						task = iter.second;
						break;
					}
					task->GroupId = groupId;
					task->TaskLinkId = task_link_id;
				}
				result.push_back(task);
			}
		}
		return result;
	}

	Task * TaskSystemSetting::findDailyTaskById(int id)
	{
		for (auto task : m_daily_task_list)
		{
			if (task->Id == id)
			{
				return task;
			}
		}
		return nullptr;
	}

	TaskGroup * TaskSystemSetting::findTaskGroupByGroupId(int groupId)
	{
		auto iter = m_task_mainline_map.find(groupId);
		if (iter != m_task_mainline_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	void TaskSystemSetting::removeDailyTaskByNextId(int id)
	{
		for (auto task : m_daily_task_list)
		{
			if (task->NextTaskId == id)
			{
				m_daily_task_list.remove(task);
				break;
			}
		}
	}

	list<Task*>::type TaskSystemSetting::getDailyTasks()
	{
		return sortTasks(m_daily_task_list);
	}

	list<TaskActive*>::type TaskSystemSetting::getDayActiveList()
	{
		list<TaskActive*>::type result;
		for (auto iter : m_task_active_map)
		{
			if (iter.second->Type == ACTIVE_TYPE_DAY)
			{
				result.push_back(iter.second);
			}
		}
		return result;
	}

	list<TaskActive*>::type TaskSystemSetting::getWeekActiveList()
	{
		list<TaskActive*>::type result;
		for (auto iter : m_task_active_map)
		{
			if (iter.second->Type == ACTIVE_TYPE_WEEK)
			{
				result.push_back(iter.second);
			}
		}
		return result;
	}

	int TaskSystemSetting::getWaitRewardCount()
	{
		int count = 0;
		for (auto task : m_task_map)
		{
			if (task.second->Status == TASK_STATUS_NOT_GET)
			{
				count++;
			}
		}
		for (auto task : m_task_active_map)
		{
			if (task.second->Status == ACTIVE_STATUS_DONE)
			{
				count++;
			}
		}
		return count;
	}

	bool TaskSystemSetting::sortTaskFunc(Task* task1, Task* task2)
	{
		if (task1->Status != task2->Status)
		{
			return (int)task1->Status > (int)task2->Status;
		}
		return task1->SortValue > task2->SortValue;
	}

	list<Task*>::type TaskSystemSetting::sortTasks(list<Task*>::type tasks)
	{
		tasks.sort(sortTaskFunc);
		return tasks;
	}

	map<int, TaskGroup*>::type TaskSystemSetting::parseUpdateMainLineTaskData(const String & data)
	{
		if (data.length() == 0)
			return m_task_mainline_map;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return m_task_mainline_map;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return m_task_mainline_map;
		}
		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("Id"))
				continue;
			int Id = obj.FindMember("Id")->value.GetInt();
			Task* task = findTaskById(Id);
			if (!task)
				continue;
			task->Current = obj.HasMember("CurNum") ? obj.FindMember("CurNum")->value.GetInt() : task->Current;
			int status = obj.FindMember("Status")->value.GetInt();
			task->Status = (TaskStatus)status ? (TaskStatus)status : task->Status;
			task->HasChange = true;

			auto group = findTaskGroupByGroupId(task->GroupId);
			if (group != nullptr)
			{
				for (auto i_task : group->Tasks)
				{
					if (i_task->TaskLinkId == task->TaskLinkId)
					{
						group->Tasks.remove(i_task);
						group->Tasks.push_back(task);
						break;
					}
				}
			}
		}
		LordDelete(doc);
		return sortMainLineTasks(m_task_mainline_map);
	}

	list<Task*>::type  TaskSystemSetting::parseUpdateDailyTaskData(const String & data)
	{
		if (data.length() == 0)
			return m_daily_task_list;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return m_daily_task_list;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return m_daily_task_list;
		}
		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("Id"))
				continue;
			int Id = obj.FindMember("Id")->value.GetInt();
			Task* task = findTaskById(Id);
			if (!task)
				continue;
			int status = obj.FindMember("Status")->value.GetInt();
			task->Status = (TaskStatus)status ? (TaskStatus)status : task->Status;
			task->Current = obj.HasMember("CurNum") ? obj.FindMember("CurNum")->value.GetInt() : task->Current;
			task->HasChange = true;
			Task* d_task = findDailyTaskById(Id);
			if (!d_task)
			{
				m_daily_task_list.push_back(task);
			}
			removeDailyTaskByNextId(Id);
		}
		LordDelete(doc);
		return sortTasks(m_daily_task_list);
	}

	map<int, TaskGroup*>::type TaskSystemSetting::sortMainLineTasks(map<int, TaskGroup*>::type map)
	{
		for (auto group : map)
		{
			group.second->Tasks = sortTasks(group.second->Tasks);
		}
		return map;
	}

}