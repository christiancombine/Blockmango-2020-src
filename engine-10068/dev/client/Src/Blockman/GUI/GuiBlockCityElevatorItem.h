#pragma once
#ifndef __GUI_BLOCKCITY_ELEVATOR_ITEM_HEADER__
#define __GUI_BLOCKCITY_ELEVATOR_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Game/BlockCity.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockCityElevatorItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticText* m_floorIdText = nullptr;
		GUIStaticImage* m_image = nullptr;

		SubscriptionGuard m_subscriptionGuard;
		i32 m_floorId = 0;
		i32 m_curFloorId = 0;

	private:
		void removeComponents();

	public:
		GuiBlockCityElevatorItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCityElevatorItem() { m_subscriptionGuard.unsubscribeAll(); }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		bool onClick(const EventArgs& args);
		void setItem(const String& item, i32 index, i32 curFloorId);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
