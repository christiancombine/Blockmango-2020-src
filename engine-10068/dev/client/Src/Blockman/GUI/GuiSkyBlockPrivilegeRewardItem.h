#pragma once
#ifndef __GUI_SKYBLOCK_PRIVILEGE_REWARD_HEADER__
#define __GUI_SKYBLOCK_PRIVILEGE_REWARD_HEADER__

#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticImage.h"
#include "Setting/SkyBlockPrivilegeShopSetting.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUILayout.h"



using namespace LORD;
namespace BLOCKMAN
{
	class GuiSkyBlockPrivilegeRewardItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

	public:
		SubscriptionGuard m_subscriptionGuard;

		GUIStaticImage* m_item_icon = nullptr;
		GUIStaticImage* m_select_img = nullptr;
		GUIStaticImage* m_have_img = nullptr;
	private:
		void removeComponents();

	public:
		GuiSkyBlockPrivilegeRewardItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiSkyBlockPrivilegeRewardItem() = default;

		void setGoods(SkyBlockPrivilegeReward goods, int groupIndex, int fromPanel);

		bool onButtonClick(const EventArgs & events);
		SkyBlockPrivilegeReward	m_goods;
		int m_index = 1;
		int m_fromPanel;
		
	};

}
#endif
