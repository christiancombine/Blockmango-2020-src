#pragma once
#ifndef __GUI_PIXEL_GUN_SIGN_IN_ITEM_HEADER__
#define __GUI_PIXEL_GUN_SIGN_IN_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/SignInSetting.h"
#include "Setting/RewardSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIPixelGunSignInItem : public CustomGuiWindow
	{
	private:
		SignInItem * m_data = nullptr;
		GUIWindow* m_window = nullptr;
		GUIButton* m_Bg = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initView();

		bool onItemClick(const EventArgs & events);

	public:
		GUIPixelGunSignInItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIPixelGunSignInItem() = default;

		void setSignInItem(SignInItem* data);
	};
}

#endif