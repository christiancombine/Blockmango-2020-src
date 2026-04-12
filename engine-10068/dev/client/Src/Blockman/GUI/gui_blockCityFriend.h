#pragma once
#ifndef __GUI_BLOCKCITY_FRIEND_HEADER__
#define __GUI_BLOCKCITY_FRIEND_HEADER__
#include "GUI/gui_layout.h"
#include "Util/FriendManager.h"

using namespace LORD;

namespace LORD {
	class GUIRadioButton;
	class GUIListBox;
}

namespace BLOCKMAN
{

	class Friend;
	class UserRank;
	class gui_blockCityFriend : public gui_layout
	{
	private:
		enum ViewId
		{
			CLOSE,
			FRIEND,
			APPLY,
		};

	private:
		void initView();
		String getViewName(const String& name);

	public:
		gui_blockCityFriend();
		~gui_blockCityFriend();
		void onShow() override;
		void onHide() override;
		void onStart() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClose(const EventArgs& args);
		bool isNeedLoad() override;
		bool isForceUpdate() override { return true; }
		bool onRadioChange(const EventArgs & events, ViewId viewId);

	private:
		bool onUpdateUserFriend(const vector<Friend>::type& items);
		bool onUpdateUserRequest(const vector<UserRequest>::type& items);
		bool onUpdateUserRequestRank(const vector<UserRank>::type& items);
		bool onInviteClick(i64 userId);

	private:
		GUIListBox* m_friendList = nullptr;
		GUIListBox* m_applyList = nullptr;
		vector<Friend>::type m_friendItems;
		vector<UserRequest>::type m_requestItems;
		map<i64, i32>::type m_inviteTime;
		map<ViewId, GUIRadioButton*>::type m_radioMap;
		map<ViewId, GUIListBox*>::type m_radioContentMap;

		String m_fileName = "";
		ViewId m_showViewId = ViewId::CLOSE;

	};
}
#endif
