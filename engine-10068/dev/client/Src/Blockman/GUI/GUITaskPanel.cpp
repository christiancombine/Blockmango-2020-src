#include "GUITaskPanel.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "Setting/TaskSystemSetting.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIListBox.h"
#include "GUI/GUITaskInfoItem.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	GUITaskPanel::GUITaskPanel(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("TaskPanel.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
	}

	void GUITaskPanel::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUITaskPanel::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUITaskPanel::ShowMainLineTasks(TaskGroup* group)
	{
		if (group->Tasks.empty())
		{
			return;
		}

		if (!m_tasks.empty())
		{
			UpdateTasks(group->Tasks);
			return;
		}

		if (!m_list)
		{
			GUIString ListName = StringUtil::Format("TaskPanel-Task-List-%d", group->Id).c_str();
			m_list = (GUIListBox*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LIST, ListName);
			m_list->SetSize({ {1.0f, 0.0f}, {1.0f, 0.0f} });
			m_list->SetInterval(4.0f);
			m_window->AddChildWindow(m_list);
		}

		for (auto task : group->Tasks)
		{
			createTaskView(task);
		}
	}

	void GUITaskPanel::ShowDailyTasks(list<Task*>::type tasks)
	{
		if (tasks.empty())
		{
			return;
		}

		if (!m_tasks.empty())
		{
			UpdateTasks(tasks);
			return;
		}

		if (!m_list)
		{
			GUIString ListName = StringUtil::Format("TaskPanel-Task-List").c_str();
			m_list = (GUIListBox*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LIST, ListName);
			m_list->SetSize({ {1.0f, 0.0f}, {1.0f, 0.0f} });
			m_list->SetInterval(4.0f);
			m_window->AddChildWindow(m_list);
		}

		for (auto task : tasks)
		{
			createTaskView(task);
		}
	}


	GUITaskInfoItem* GUITaskPanel::createTaskView(Task * task)
	{
		static unsigned task_index = 0;
		task_index++;
		GUIString TaskName = StringUtil::Format("TaskPanel-DailyTasks-%d", task_index).c_str();
		GUITaskInfoItem* pTask = (GUITaskInfoItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_TASK_INFO_ITEM, TaskName);
		pTask->SetSize({ {1.0f, 0.0f}, {0.0f, 85.0f} });
		m_list->AddItem(pTask, false);
		pTask->InitTask(task);
		m_tasks.push_back(pTask);
		return pTask;
	}

	void GUITaskPanel::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUITaskPanel::UpdateTasks(list<Task*>::type tasks)
	{
		size_t count = tasks.size();
		auto iter = tasks.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (iter == tasks.end())
				break;
			GUITaskInfoItem* task = getTaskByIndex(i);
			if (task)
			{
				task->InitTaskItem(*iter);
			}
			else
			{
				createTaskView(*iter);
			}
			iter++;
		}
	}

	GUITaskInfoItem* GUITaskPanel::getTaskByIndex(int index)
	{

		size_t count = m_tasks.size();
		auto iter = m_tasks.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (i == index)
			{
				return *iter;
			}
			iter++;
		}
		return nullptr;
	}

}
