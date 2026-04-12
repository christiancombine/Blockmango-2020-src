#pragma once
#ifndef __GUI_GUN_STORE_MODULE_PANEL_HEADER__
#define __GUI_GUN_STORE_MODULE_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIGridView.h"
#include "Setting/GunStoreSetting.h"
#include "GUI/CustomGuiWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIGunStoreModulePanel : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initView();

		GUIWindow* m_window = nullptr;
		GUIGridView* m_list = nullptr;
	public:
		GUIGunStoreModulePanel(const GUIWindowType& nType, const GUIString& strName);
		~GUIGunStoreModulePanel() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void InitModules(list<StoreBlockModule*>::type modules);
		size_t GetPanelItemCount() { return m_list->GetItemCount(); };
		GUIWindow* GetPanelItem(int index) { return m_list->GetItem(index); };
	private:

	};
}

#endif