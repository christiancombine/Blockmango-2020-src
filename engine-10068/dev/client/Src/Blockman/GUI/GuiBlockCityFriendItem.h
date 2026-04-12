#pragma once
#ifndef __GUI_BLOCKCITY_FRIEND_ITEM_HEADER__
#define __GUI_BLOCKCITY_FRIEND_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "GUI/GuiUrlImage.h"
#include "Game/BlockCity.h"
#include "Util/FriendManager.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class UserRequest;
	class GuiBlockCityFriendItem : public CustomGuiWindow
	{
	private:
		enum class ViewId {
			DELETE_FRIEND,
			RATIFY,
			INVITE,
			VISIT,
			REFUSE,
			AGREE
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GuiUrlImage* m_icon = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_starValue = nullptr;
		GUILayout* m_friendLayout = nullptr;
		GUILayout* m_applyLayout = nullptr;
		GUIButton* m_btnDelete = nullptr;
		GUIButton* m_btnRatify = nullptr;
		GUIButton* m_btnInvite = nullptr;
		GUIButton* m_btnVisit = nullptr;
		GUIButton* m_btnRefuse = nullptr;
		GUIButton* m_btnAgree = nullptr;
		GUIStaticImage* m_offline = nullptr;
		GUIStaticImage* m_sex = nullptr;
		GUIStaticText* m_language = nullptr;

		UserRequest m_request;
		Friend m_friend;

		SubscriptionGuard m_subscriptionGuard;
		i32 m_inviteTime = 0;

	private:
		void removeComponents();

	public:
		GuiBlockCityFriendItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCityFriendItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void init(const String & jsonName);
		void setListItem(bool isFriendList);
		void setFriend(const Friend& firend);
		void setRequest(const UserRequest& request);
		void setInviteTime(i32 time) { m_inviteTime = time; }

		bool onClick(const EventArgs& args, ViewId viewId);
		bool onDeleteFriend(i64 userId);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
