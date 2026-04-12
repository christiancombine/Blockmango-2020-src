#pragma once
#ifndef __GUI_SELECTABLE_ACTION_ITEM_HEADER__
#define __GUI_SELECTABLE_ACTION_ITEM_HEADER__

#include "GUI/CustomGuiWindow.h"
#include "GUI/gui_renderItems.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiSelectableActionItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIStaticImage* m_mask = nullptr;
		int m_actionId = 0;
		bool m_isLocked = true;
		bool m_isMoved = false;

	public:
		GuiSelectableActionItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiSelectableActionItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		bool onItemTouchMove(const EventArgs & events);
		bool onItemTouchUp(const EventArgs & events);
		bool onItemTouchRelease(const EventArgs & events);

		void setAction(int actionId, bool isLocked = true);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	private:
		void removeComponents();
		void updateIcon();
	};

}
#endif
