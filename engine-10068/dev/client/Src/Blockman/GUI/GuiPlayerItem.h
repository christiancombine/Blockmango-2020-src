#pragma once
#ifndef __GUI_PLAYER_ITEM_HEADER__
#define __GUI_PLAYER_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"
#include "GuiPlayerItem.h"
#include "GUI/GuiUrlImage.h"
#include "Util/UserManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiPlayerItem : public CustomGuiWindow
	{
	public:
		struct PlayerInfo {
			ui64 userId = 0;
			String userName;
			String teamName;
			String teamViewName;
			String message;
			int teamId = 0;
			int curMunber = 0;
			int totalMunber = 0;
			bool isFriend = false;
			PlayerInfo() {}
			PlayerInfo(ui64 userId, String userName, String teamName, int teamId, String teamViewName) {
				this->userId = userId;
				this->userName = userName;
				this->teamName = teamName;
				this->teamId = teamId;
				this->teamViewName = teamViewName;
				this->message = "";
				this->isFriend = false;
			}

			bool operator==(const PlayerInfo& other) const {
				return userId == other.userId;
			}
		};

		enum class ViewId
		{
			BTN_NEGLECT,
			BTN_AGREE,
			BTN_ADD_FRIEND,
			BTN_CALL_ON
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_friendIcon = nullptr;
		GUIStaticText* m_playerName = nullptr;
		GUIStaticText* m_messageView = nullptr;
		GUIButton* m_btnIgnore = nullptr;
		GUIButton* m_btnAgree = nullptr;
		GUIButton* m_btnAddFriend = nullptr;
		GUIButton* m_btnCallOn = nullptr;
		GuiUrlImage* m_icon = nullptr;
		GUIStaticImage* m_sexIcon = nullptr;
		GUIStaticImage* m_vipIcon = nullptr;
		GUIStaticText* m_desc = nullptr;

		User m_playerInfo;

		int mMyTeamId = 0;
		bool isFriendRequest = false;

	private:
		void removeComponents();

	public:
		GuiPlayerItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiPlayerItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setUser(User info);
		bool onClick(const EventArgs & events, ViewId viewId);
		void setFriendRequest(bool isRequest) { this->isFriendRequest = isRequest; }
		void setFriend(bool isFriend) { this->m_playerInfo.isFriend = isFriend; };
		void resetAddFriendButton();

	private:
		bool isMe();
		bool isShowCallOnbtn();
		String getSexIcon();
		String getVipIcon();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
