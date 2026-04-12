#ifndef __GUI_WALKING_DEAD_STORE_TAB_HEADER__
#define __GUI_WALKING_DEAD_STORE_TAB_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticText.h"
#include "Setting/WalkingDeadStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIWalkingDeadStoreTab : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		GUIStaticText* name = nullptr;

		int m_tab_id = 0;
		WalkingDeadStoreTabType m_tab_type = GeneralTabType;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIWalkingDeadStoreTab(const GUIWindowType& nType, const GUIString& strName);
		~GUIWalkingDeadStoreTab() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void InitStoreTab(WalkingDeadStoreTab* tab);
		void SetTabSelected(bool selected);
		bool IsTabSelected();
		bool OnTabSelected(const EventArgs & events);

		int GetTabId() { return m_tab_id; }
		WalkingDeadStoreTabType GetTabType() { return m_tab_type; }
	};
}

#endif