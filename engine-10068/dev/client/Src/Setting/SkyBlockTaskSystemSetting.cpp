#include "SkyBlockTaskSystemSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{

	SkyBlockTaskSystemSetting::SkyBlockTaskSystemSetting()
	{

	}

	SkyBlockTaskSystemSetting::~SkyBlockTaskSystemSetting()
	{

	}

	void SkyBlockTaskSystemSetting::loadSetting()
	{
		loadTaskSetting();
		loadRewardSetting();
	}

	void SkyBlockTaskSystemSetting::unloadSetting()
	{
		m_task_map.clear();
		m_task_mainline_map.clear();
		m_task_reward_map.clear();
		m_task_Dare_GroUp.Tasks.clear();

	}

	void SkyBlockTaskSystemSetting::loadRewardSetting()
	{
		m_task_reward_map.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "SkyBlockTaskRewardSetting.csv").c_str();
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
			SkyBlockTaskReward pTaskReward;
			pReader->GetInt("Id", pTaskReward.Id);
			pReader->GetInt("Type", pTaskReward.Type);
			pReader->GetInt("RewardId", pTaskReward.RewardId);
			pReader->GetInt("Num", pTaskReward.Num);
			pReader->GetInt("Meta", pTaskReward.Meta);
			pReader->GetString("Icon", pTaskReward.Icon);
			if (pTaskReward.Icon == "#")
				pTaskReward.Icon = "";
			m_task_reward_map[pTaskReward.Id] = pTaskReward;
		}
		LordSafeDelete(pReader);
	}

	void SkyBlockTaskSystemSetting::loadTaskSetting()
	{
		m_task_map.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "SkyBlockTaskSetting.csv").c_str();
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
			SkyBlockTask pTask;
			pReader->GetInt("Id", pTask.Id);
			pReader->GetInt("Level", pTask.Level);
			pReader->GetInt("TaskSort", pTask.TaskSort);
			pReader->GetInt("Quality", pTask.Quality);
			pReader->GetString("Icon", pTask.Icon);
			pReader->GetString("Title", pTask.Title);
			pReader->GetInt("SortValue", pTask.SortValue);

			String RewardList = "";
			pReader->GetString("RewardIds", RewardList);
			StringArray holdItemIdArray = StringUtil::Split(RewardList, "#");
			if(holdItemIdArray.size() > 0)
				pTask.RewardIds = holdItemIdArray[0];
			pReader->GetInt("GoalNum", pTask.GoalNum);
			pTask.Current = pTask.GoalNum;
			pReader->GetInt("NextTaskId", pTask.NextTaskId);
			pReader->GetInt("TaskTypeId", pTask.TaskTypeId);
			pReader->GetInt("Remarks", pTask.Remarks);
			pReader->GetInt("RemarksMeta", pTask.RemarksMeta);
			m_task_map[pTask.Id] = pTask;
		}

		setMainlineGroup();
		LordSafeDelete(pReader);
	}

	void SkyBlockTaskSystemSetting::setMainlineGroup()
	{
		m_task_mainline_map.clear();

		for (auto iter : m_task_map)
		{
			if (iter.second.TaskSort == SKYBLOCK_TASK_TYPE_MAINLINE)
			{
				auto mainlineIter = m_task_mainline_map.find(iter.second.Level);
				if (mainlineIter != m_task_mainline_map.end())
				{
					mainlineIter->second.Tasks.push_back(iter.second);
				}
				else
				{
					SkyBlockTaskGroup pGroup;
					pGroup.Lv = iter.second.Level;
					pGroup.Tasks.push_back(iter.second);
					m_task_mainline_map[iter.second.Level] = pGroup;
				}
			}
		}
	}

	bool SkyBlockTaskSystemSetting::findTaskById(int id, SkyBlockTask& task)
	{
		for (auto iter : m_task_map)
		{
			if (iter.second.Id == id)
			{
				task = iter.second;
				return true;
			}
		}
		return false;
	}

	bool  SkyBlockTaskSystemSetting::findTaskRewardById(int id, SkyBlockTaskReward& taskReward)
	{
		for (auto iter : m_task_reward_map)
		{
			if (iter.second.Id == id)
			{
				taskReward = iter.second;
			}
		}
		return true;
	}

	bool SkyBlockTaskSystemSetting::findTaskRewardByIds(String rewardIds, list<SkyBlockTaskReward>::type& result)
	{
		StringArray ids = StringUtil::Split(rewardIds, "#");
		for (String id : ids)
		{
			SkyBlockTaskReward reward;
			if (findTaskRewardById(StringUtil::ParseI32(id), reward))
			{
				result.push_back(reward);
			}
		}
		return true;
	}

	bool SkyBlockTaskSystemSetting::findTaskGroupByLevel(int level, SkyBlockTaskGroup& group)
	{
		auto iter = m_task_mainline_map.find(level);
		if (iter != m_task_mainline_map.end())
		{
			group.Lv = iter->second.Lv;
			group.MainGroupId = iter->second.MainGroupId;
			for (auto task : iter->second.Tasks)
			{
				group.Tasks.push_back(task);
			}
			return true;
		}
		return false;
	}

	int SkyBlockTaskSystemSetting::getWaitRewardCount()
	{
		int count = 0;
		bool isFinshdFirstTask = false;
		for (auto taskGrop : m_task_mainline_map)
		{
			for (auto task : taskGrop.second.Tasks)
			{
				if (task.Status == SKYBLOCK_TASK_STATUS_NOT_GET || task.Status == SKYBLOCK_TASK_STATUS_CAN_COMMIT)
				{
					count++;
				}

				if (task.Status == SKYBLOCK_TASK_STATUS_HAVE_GOT)
					isFinshdFirstTask = true;
			}
		}

		for (auto task : m_task_Dare_GroUp.Tasks)
		{
			if (task.Status == SKYBLOCK_TASK_STATUS_NOT_GET || task.Status == SKYBLOCK_TASK_STATUS_CAN_COMMIT)
			{
				count++;
			}

			if (task.Status == SKYBLOCK_TASK_STATUS_HAVE_GOT)
				isFinshdFirstTask = true;
		}

		if (isFinshdFirstTask == false && count == 0)
			count = 1;

		return count;
	}

	bool sortMainLineTaskFunc(SkyBlockTask task1, SkyBlockTask task2)
	{
		if (task1.Status != task2.Status)
		{
			return (int)task1.Status > (int)task2.Status;
		}
		return task1.SortValue > task2.SortValue;
	}

	bool SkyBlockTaskSystemSetting::parseUpdateMainLineTaskData(const String & data, map<int, SkyBlockTaskGroup>::type & mainLineGroup)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return false;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return false;
		}
		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("Id"))
				continue;
			int Id = obj.FindMember("Id")->value.GetInt();
			SkyBlockTask task;
			if (!findTaskById(Id, task))
				continue;
			task.Current = obj.HasMember("CurNum") ? obj.FindMember("CurNum")->value.GetInt() : task.Current;
			int status = obj.FindMember("Status")->value.GetInt();
			task.Status = (SkyBlockTaskStatus)status ? (SkyBlockTaskStatus)status : task.Status;
			if (obj.HasMember("RewardId"))
			{
				String RewardId = obj.FindMember("RewardId")->value.GetString();
				task.RewardIds = RewardId;
			}
			task.HasChange = true;
			SkyBlockTaskGroup group;
			auto iter = m_task_mainline_map.find(task.Level);
			if (iter != m_task_mainline_map.end())
			{

				list<SkyBlockTask>::iterator i_task;

				for (i_task = iter->second.Tasks.begin(); i_task != iter->second.Tasks.end(); i_task++)
				{
					if (i_task->Id == task.Id)
					{
						i_task->Status = task.Status;
						i_task->HasChange = task.HasChange;
						i_task->Current = task.Current;
						break;
					}
				}
			}
		}

		for (auto &group : m_task_mainline_map)
		{
			group.second.Tasks.sort(sortMainLineTaskFunc);
		}

		for (auto group : m_task_mainline_map)
		{
			auto iter = mainLineGroup.find(group.first);
			if (iter == mainLineGroup.end())
			{
				SkyBlockTaskGroup innerGroup;
				innerGroup.Lv = group.second.Lv;
				innerGroup.MainGroupId = group.second.MainGroupId;

				for (auto task : group.second.Tasks)
				{
					innerGroup.Tasks.push_back(task);
				}
				mainLineGroup[group.first] = innerGroup;
			}
		}

		LordDelete(doc);
		return true;
	}

	bool SkyBlockTaskSystemSetting::parseUpdateDareTaskData(const String & data, SkyBlockTaskGroup& result)
	{

		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return false;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return false;
		}

		m_task_Dare_GroUp.Tasks.clear();

		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("Id"))
				continue;
			int Id = obj.FindMember("Id")->value.GetInt();
			SkyBlockTask task;
			if (!findTaskById(Id, task))
				continue;
			task.Current = obj.HasMember("CurNum") ? obj.FindMember("CurNum")->value.GetInt() : task.Current;
			int status = obj.FindMember("Status")->value.GetInt();
			task.Status = (SkyBlockTaskStatus)status ? (SkyBlockTaskStatus)status : task.Status;

			if (obj.HasMember("RewardId"))
			{
				String RewardId = obj.FindMember("RewardId")->value.GetString();
				task.RewardIds = RewardId;
			}

			task.HasChange = true;

			m_task_Dare_GroUp.Tasks.push_back(task);
		}

		result.Lv = m_task_Dare_GroUp.Lv;
		result.MainGroupId = m_task_Dare_GroUp.MainGroupId;
		for (auto task : m_task_Dare_GroUp.Tasks)
		{
			result.Tasks.push_back(task);
		}

		LordDelete(doc);
		return true;
	}


	bool SkyBlockTaskSystemSetting::getMainLineTasks(map<int, SkyBlockTaskGroup>::type& data)
	{ 
		for (auto group : m_task_mainline_map)
		{
			auto iter = data.find(group.first);
			if (iter == data.end())
			{
				SkyBlockTaskGroup innerGroup;
				innerGroup.Lv = group.second.Lv;
				innerGroup.MainGroupId = group.second.MainGroupId;

				for (auto task : group.second.Tasks)
				{
					innerGroup.Tasks.push_back(task);
				}
				
				data[group.first] = innerGroup;
			}
		}
		return true; 
	}
}