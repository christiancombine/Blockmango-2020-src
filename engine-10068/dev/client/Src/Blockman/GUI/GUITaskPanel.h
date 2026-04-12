#ifndef __GUI_TASK_PANEL_HEADER__
#define __GUI_TASK_PANEL_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "GUITaskPanel.h"
#include "GUITaskInfoItem.h"
#include "Setting/TaskSystemSetting.h"
#include "UI/GUIListBox.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUITaskPanel : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		list<GUITaskInfoItem*>::type m_tasks;
		GUIListBox* m_list = nullptr;
		int m_panel_id = 0;

		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUITaskPanel(const GUIWindowType& nType, const GUIString& strName);
		~GUITaskPanel() = default;
		void UpdateSelf(ui32 nTimeElapse) override;

	private:
		GUITaskInfoItem* getTaskByIndex(int index);
		GUITaskInfoItem* createTaskView(Task* task);

	public:
		void Destroy() override;
		void InitMainLineTasks(int id) { m_panel_id = id; };
		int GetPanelId() { return m_panel_id; };
		void ShowMainLineTasks(TaskGroup* group);
		void ShowDailyTasks(list<Task*>::type tasks);
		void UpdateTasks(list<Task*>::type tasks);
		
	};
}

#endif