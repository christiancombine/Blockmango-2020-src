#pragma once
#ifndef __GUI_BLOCKFORT_MANOR_UPGRADE_ITEM_HEADER__
#define __GUI_BLOCKFORT_MANOR_UPGRADE_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "Game/BlockFort.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockFortManorUpgradeItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_name = nullptr;
		SubscriptionGuard m_subscriptionGuard;

	private:
		void removeComponents();

	public:
		GuiBlockFortManorUpgradeItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockFortManorUpgradeItem() { m_subscriptionGuard.unsubscribeAll(); }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void setItem(i32 itemId);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
