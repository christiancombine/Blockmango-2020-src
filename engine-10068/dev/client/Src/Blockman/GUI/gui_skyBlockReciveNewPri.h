#pragma once
#ifndef __GUI_SKYBLOCK_RECIVE_NEW_PRI__
#define __GUI_SKYBLOCK_RECIVE_NEW_PRI__

#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"
using namespace LORD;

namespace LORD {
	class GUILayout;
	class GUIGridView;
	class GUIStaticText;
}

namespace BLOCKMAN
{
	struct SkyBlockPrivilegeReward;
	class gui_skyBlockReciveNewPri : public gui_layout
	{

	public:
		gui_skyBlockReciveNewPri();
		~gui_skyBlockReciveNewPri();

		SubscriptionGuard m_subscriptionGuard;
		GUIGridView* m_itemGridView;
		GUIStaticText* m_pri_name;
		GUIStaticText* m_pri_des;
		void onLoad() override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);
		void parseUpdateData(String data);
		bool updateShopItemDetail(const SkyBlockPrivilegeReward item, int index);
		bool onCloseClick(const EventArgs & events);
		bool onReciveClick(const EventArgs & events);

	private:
		GUILayout* m_items = nullptr;
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}

	};
}

#endif