#ifndef __GUI_TASK_MAINLINE_TAB_HEADER__
#define __GUI_TASK_MAINLINE_TAB_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIButton.h"
#include "Setting/TaskSystemSetting.h"


using namespace LORD;

namespace BLOCKMAN
{
	class GUITaskMainLineTab : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		
		int m_tab_id = 0;
		
	public:
		GUITaskMainLineTab(const GUIWindowType& nType, const GUIString& strName);		
		~GUITaskMainLineTab() = default;
		void UpdateSelf(ui32 nTimeElapse) override;

	public:
		bool m_has_new = false;

		void Destroy() override;
		void InitMainLineTab(TaskGroup* group);
		void SetSelectTab(bool selected);
		bool OnButtonClick(const EventArgs & events);
		bool IsTabSelected();
		int GetTabId() { return m_tab_id; };
		void UnLockTab();
		void UpdateRedPoint();
	};
}
#endif 
