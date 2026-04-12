#ifndef __GUI_SKYBLOCK_TASK_MAINLINE_TAB_HEADER__
#define __GUI_SKYBLOCK_TASK_MAINLINE_TAB_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIButton.h"
#include "Setting/SkyBlockTaskSystemSetting.h"


using namespace LORD;

namespace BLOCKMAN
{
	class GUISkyBlockTaskMainLineTab : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		using BaseClass = CustomGuiWindow;
		void removeComponents();

		int m_tab_lv = 0;
		int m_lv = 0;
		int m_progressesLv = 0;

	public:
		GUISkyBlockTaskMainLineTab(const GUIWindowType& nType, const GUIString& strName);
		~GUISkyBlockTaskMainLineTab() = default;
		void UpdateSelf(ui32 nTimeElapse) override;

	public:
		bool m_has_new = false;

		void Destroy() override;
		void InitMainLineTab(const SkyBlockTaskGroup group);
		void SetSelectTab(bool selected);
		bool OnButtonClick(const EventArgs & events);
		bool IsTabSelected();
		int GetTabId() { return m_tab_lv; };
		void UnLockTab();
		void UpdateRedPoint();
		void UpdateDatra(int lv,int progressesLv);
		bool getTabIsLock();
	};
}
#endif 
