#pragma once
#ifndef __GUI_GUN_STORE_MODULE_ITEM_HEADER__
#define __GUI_GUN_STORE_MODULE_ITEM_HEADER__

#include "gui_layout.h"
#include "UI/GUIGridView.h"
#include "Setting/GunStoreSetting.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "GUI/CustomGuiWindow.h"
using namespace LORD;

namespace BLOCKMAN
{
	class GUIGunStoreModuleItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initView();

		GUIWindow* m_window = nullptr;
		StoreBlockModule* m_data = NULL;
		GUIStaticImage* m_status_box = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIButton* m_use_line = nullptr;
		GUILayout* m_hint_new = nullptr;
		GUILayout* m_price = nullptr;
		GUIStaticImage* m_price_icon = nullptr;
		GUIStaticText* m_price_text = nullptr;
		GUILayout* m_lock = nullptr;
		GUIButton* m_lock_level = nullptr;
		GUIStaticImage* m_tag = nullptr;

	public:
		GUIGunStoreModuleItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIGunStoreModuleItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void InitModuleItem(StoreBlockModule* module);

	private:
		bool OnItemChick(const EventArgs & events);

	};
}

#endif
