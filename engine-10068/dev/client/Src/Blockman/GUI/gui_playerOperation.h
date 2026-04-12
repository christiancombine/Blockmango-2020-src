#pragma once
#ifndef __GUI_PLAYER_OPERATION_HEADER__
#define __GUI_PLAYER_OPERATION_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_playerOperation : public  gui_layout
	{
		enum class ViewId
		{
			BTN_ADD_FRIEND = 1,
			BTN_GIVE_ROSE = 2,
			BTN_CALL_ON = 3,
			BTN_GREETINGS = 4,
			BTN_INVITER = 5,
			BTN_UNRATIFY = 6,
			BTN_REMOVE = 7,
			BTN_RACE_INVITE = 9,
		};

	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_playerOperation();
		virtual ~gui_playerOperation();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isForceUpdate() override { return true; };
		bool onClick(const EventArgs & events, ViewId viewId);
		void setTargetUserId(ui64 targetId) {
			this->m_targetUserId = targetId;
		}
		void setTargetUserName(String name) {
			m_targetUserName = name;
		}

		void setRemoveBtnEnabled(bool isEnabled);

	private:
		bool onGameTypeUpdate();
		bool onBlockCityRaceBtnUpdate(i64 targetUserId);

	private:
		ui64 m_targetUserId = -1;
		String m_targetUserName = "";
		map<i64, map<ViewId, i32>::type>::type m_inviteMap;
	};
}

#endif