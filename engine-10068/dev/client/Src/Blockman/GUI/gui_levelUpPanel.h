#pragma once
#ifndef __GUI_LEVEL_UP_PANEL_HEADER__
#define __GUI_LEVEL_UP_PANEL_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "Util/SubscriptionGuard.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_levelUpPanel : public gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_levelUpPanel();
		~gui_levelUpPanel();

		void onLoad() override;
		bool isNeedLoad() override;
	private:
		bool onCloseClick(const EventArgs & events);
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif