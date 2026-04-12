#include "GUISkyBlockTaskPanel.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "Setting/SkyBlockTaskSystemSetting.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIListBox.h"
#include "GUI/GUISkyBlockTaskInfoItem.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	GUISkyBlockTaskPanel::GUISkyBlockTaskPanel(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SkyBlockTaskPanel.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
	}

	void GUISkyBlockTaskPanel::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISkyBlockTaskPanel::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUISkyBlockTaskPanel::ShowMainLineTasks(SkyBlockTaskGroup group)
	{
		if (group.Tasks.empty())
		{
			return;
		}

		if (!m_tasks.empty())
		{
			UpdateTasks(group.Tasks);
			return;
		}

		if (!m_list)
		{
			GUIString ListName = StringUtil::Format("SkyBlockTaskPanel-Task-List-%d", group.Lv).c_str();
			m_list = (GUIListBox*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LIST, ListName);
			m_list->SetSize({ {1.0f, 0.0f}, {1.0f, 0.0f} });
			m_list->SetInterval(20.0f);
			m_window->AddChildWindow(m_list);
		}

		for (auto task : group.Tasks)
		{
			createTaskView(task);
		}
	}	

	void GUISkyBlockTaskPanel::ShowDareTasks(SkyBlockTaskGroup group, bool have_cube_ad)
	{
		m_have_cube_ad = have_cube_ad;
		if (!m_tasks.empty())
		{
			UpdateTasks(group.Tasks);
			return;
		}

		if (!m_list)
		{
			GUIString ListName = StringUtil::Format("SkyBlockDareTaskPanel-Task-List-%d", group.Lv).c_str();
			m_list = (GUIListBox*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LIST, ListName);
			m_list->SetSize({ {1.0f, 0.0f}, {1.0f, 0.0f} });
			m_list->SetInterval(15.0f);
			m_window->AddChildWindow(m_list);
		}
		else
		{
			m_list->ClearAllItem();
		}

		for (auto task : group.Tasks)
		{
			createTaskView(task);
		}
	}

	GUISkyBlockTaskInfoItem* GUISkyBlockTaskPanel::createTaskView(SkyBlockTask task)
	{
		static unsigned task_index = 0;
		task_index++;
		GUIString TaskName = StringUtil::Format("SkyBlockTaskPanel-DailyTasks-%d", task_index).c_str();
		GUISkyBlockTaskInfoItem* pTask = (GUISkyBlockTaskInfoItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_TASK_INFO_ITEM, TaskName);
		pTask->SetSize({ {0.9f, 0.0f}, {0.0f, 100.0f} });
		pTask->SetHorizontalAlignment(HA_CENTRE);
		m_list->AddItem(pTask, false);
		pTask->InitTask(task);
		m_tasks.push_back(pTask);
		return pTask;
	}

	void GUISkyBlockTaskPanel::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUISkyBlockTaskPanel::UpdateTasks(list<SkyBlockTask>::type tasks)
	{
		size_t count = tasks.size();
		auto iter = tasks.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (iter == tasks.end())
				break;
			GUISkyBlockTaskInfoItem* task = getTaskByIndex(i);
			if (task)
			{
				task->InitTaskItem(*iter, m_have_cube_ad);
			}
			else
			{
				createTaskView(*iter);
			}
			iter++;
		}
	}

	GUISkyBlockTaskInfoItem* GUISkyBlockTaskPanel::getTaskByIndex(int index)
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
