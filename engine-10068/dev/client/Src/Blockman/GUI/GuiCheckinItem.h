#pragma once
#ifndef __GUI_CHECKIN_ITEM_HEADER__
#define __GUI_CHECKIN_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"
#include "Game/BirdSimulator.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct CheckinItem
	{
		i32 id = 0;
		i32 num = 0;
		i32 state = 0;
		String icon = "";
		String name = "";
	};

	class GuiCheckinItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIButton* m_tvTitle = nullptr;
		GUIStaticText* m_tvNum = nullptr;
		GUIStaticText* m_tvName = nullptr;
		GUIStaticImage* m_ivIcon = nullptr;
		GUIStaticImage* m_ivDone = nullptr;
		GUIStaticImage* m_ivBackDone = nullptr;

	private:
		void removeComponents();

	public:
		GuiCheckinItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiCheckinItem() = default;
		void init(const String & jsonName);
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setBirdCheckin(const CheckinItem& item);
		void setBlockCityCheckin(const CheckinItem& item);

	private:
		bool onCheckin(const EventArgs&);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	};

}

#endif
