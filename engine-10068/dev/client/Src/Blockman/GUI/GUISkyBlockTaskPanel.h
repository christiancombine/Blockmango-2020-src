#ifndef __GUI_SKYBLOCK_TASK_PANEL_HEADER__
#define __GUI_SKYBLOCK_TASK_PANEL_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "GUISkyBlockTaskPanel.h"
#include "GUISkyBlockTaskInfoItem.h"
#include "Setting/SkyBlockTaskSystemSetting.h"
#include "UI/GUIListBox.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISkyBlockTaskPanel : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		list<GUISkyBlockTaskInfoItem*>::type m_tasks;
		GUIListBox* m_list = nullptr;
		int m_panel_id = 0;

		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUISkyBlockTaskPanel(const GUIWindowType& nType, const GUIString& strName);
		~GUISkyBlockTaskPanel() = default;
		void UpdateSelf(ui32 nTimeElapse) override;

	private:
		GUISkyBlockTaskInfoItem* getTaskByIndex(int index);
		GUISkyBlockTaskInfoItem* createTaskView(SkyBlockTask task);
		bool m_have_cube_ad = false;

	public:
		void Destroy() override;
		void InitMainLineTasks(int id) { m_panel_id = id; };

		//void InitDareTasks(int id) { m_panel_id = id; };
		int GetPanelId() { return m_panel_id; };
		void ShowMainLineTasks(SkyBlockTaskGroup group);
		void ShowDareTasks(SkyBlockTaskGroup, bool have_cube_ad);
		void UpdateTasks(list<SkyBlockTask>::type tasks);

	};
}

#endif