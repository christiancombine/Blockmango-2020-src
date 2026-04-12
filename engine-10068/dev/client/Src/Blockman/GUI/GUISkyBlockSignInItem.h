#pragma once
#ifndef __GUI_SKYBLOCK_SIGN_IN_ITEM_HEADER__
#define __GUI_SKYBLOCK_SIGN_IN_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "Setting/SkyblockSignInSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISkyBlockSignInItem : public CustomGuiWindow
	{
	private:
		SkyBlockSignInInfo m_data;
		GUIWindow* m_window = nullptr;
		GUIButton* m_Bg = nullptr;
		GUIStaticImage* m_small_title_panle = nullptr;
		GUIStaticText* m_big_title_panle = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initView();

		bool onItemClick(const EventArgs & events);

	public:
		GUISkyBlockSignInItem(const GUIWindowType& nType, const GUIString& strName);
		~GUISkyBlockSignInItem() = default;

		void setSignInItem(SkyBlockSignInInfo data);
	};
}

#endif