#ifndef __GUI_PIXEL_GUN_ACTIVE_CHEST_HEADER__
#define __GUI_PIXEL_GUN_ACTIVE_CHEST_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/TaskSystemSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIPixelGunActiveChest : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		TaskActive* m_data = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIPixelGunActiveChest(const GUIWindowType& nType, const GUIString& strName);
		~GUIPixelGunActiveChest() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetTaskActive(TaskActive* active);

	private:
		bool onOpenClick(const EventArgs & events);

	};
}

#endif