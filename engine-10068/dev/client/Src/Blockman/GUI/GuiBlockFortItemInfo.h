#pragma once
#ifndef __GUI_BLOCKFORT_ITEM_INFO_HEADER__
#define __GUI_BLOCKFORT_ITEM_INFO_HEADER__
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "UI/GUIListBox.h"
#include "Game/BlockFort.h"
#include "Setting/BlockFortItemSetting.h"
#include "GUI/CustomGuiWindow.h"
#include "GuiDef.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum BFItemEntrance {
		Store = 1,
		AttackBuilding
	};

	class GuiBlockFortItemInfo : public CustomGuiWindow
	{
	public:
		GuiBlockFortItemInfo(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockFortItemInfo() = default;

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_ivIcon = nullptr;
		GUIStaticText* m_tvName = nullptr;
		GUIStaticText* m_tvNameValue = nullptr;
		GUIStaticText* m_tvGainValue = nullptr;

		SubscriptionGuard m_subscriptionGuard;
		i32 m_inviteTime = 0;

	private:
		void removeComponents();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		void setItemInfo(const BlockFortItemInfo& itemInfo, BFItemEntrance entrance, bool isNeedSplit = false);
	};
}
#endif