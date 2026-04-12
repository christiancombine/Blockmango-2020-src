#ifndef __GUI_SKYBLOCK_SUPEME_PRIVILEGE_HEADER__
#define __GUI_SKYBLOCK_SUPEME_PRIVILEGE_HEADER__

#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIGridView.h"
#include "Setting/SkyBlockPrivilegeShopSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiSkyBlockSupemePrivilege : public gui_layout
	{
	public:
		GuiSkyBlockSupemePrivilege();
		~GuiSkyBlockSupemePrivilege();
		SubscriptionGuard m_subscriptionGuard;
		void onLoad() override;
		void setGood(const SkyBlockPrivilegeGoods good);
		bool onButtonClick(const EventArgs & events);
		bool updateShopItemDetail(int index);
		SkyBlockPrivilegeGoods m_good;

		GUIButton* m_buy_btn = nullptr;
		GUIGridView* m_reward_gridview = nullptr;
	};

}
#endif 