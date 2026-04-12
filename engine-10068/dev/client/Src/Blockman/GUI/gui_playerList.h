#pragma once
#ifndef __GUI_PLAYER_LIST_HEADER__
#define __GUI_PLAYER_LIST_HEADER__

#include "Core.h"
#include "UI/GUIWindow.h"
#include "UI/GUIListBox.h"
#include "GuiPlayerItem.h"
#include "GUI/gui_layout.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIStaticText.h"
#include "Util/UserManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	class PlayerItemInfo
	{
	public:
		ui64 userId = 0;
		String name = "";
		String teamName = "";
		int teamId = 0;
	};

	class FriendOperation
	{
	public:
		ui64 userId = 0;
		FriendOperationType type = FriendOperationType::NONE;
	};

	class gui_playerList : public gui_layout
	{
	public:
		gui_playerList();
		~gui_playerList();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onInitWaitItems();
		void onHandleWaitFriendOperations();

		bool onUserIn(vector<ui64>::type ids);
		bool onUserOut(const ui64 playerPlatformId);

		bool onFriendOperationForServer(int operationType, ui64 playerPlatformId);
		bool onFriendOperationForAppHttpResult(int operationType, ui64 playerPlatformId);

	private:
		int getIndex(String viewName);
	private:
		map<ui64, GuiPlayerItem*>::type m_playerItemMap;
		vector<ui64>::type m_waitInitItems;
		list<FriendOperation>::type m_waitHandleFriendOperations;

		SubscriptionGuard m_subscriptionGuard;
		GUIListBox* m_playerList = nullptr;
		GUIStaticText* m_tipMessageView = nullptr;
		ui32 m_tipShopTime = 0;
		String m_tipMessage;
	};
}
#endif
