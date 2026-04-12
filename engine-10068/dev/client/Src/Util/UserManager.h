#pragma once
#include "Core.h"
#include "Util/SubscriptionGuard.h"
#include <vector>
using namespace LORD;

namespace BLOCKMAN {

	enum class FriendOperationType {
		NONE,
		NO_FRIEND,
		IS_FRIEND,
		AGREE_ADD_FRIEND,
		REQUEST_ADD_FRIEND,
		DELETE_FRIEND,
		NEGLECT_ADD_FRIEND,

		//error code
		AGREE_ADD_FRIEND_FAILURE = 10000,
		REQUEST_ADD_FRIEND_FAILURE
	};

	class User {
	public:
		i32 sex = 0;
		i32 vip = 0;
		i32 teamId = 0;
		ui64 userId = 0;
		bool isSend = false;
		bool isFriend = false;
		bool isFriendRequest = false;
		String userName = "";
		String teamName = "";
		String picUrl = "";
		String message = "";
		String country = "";
		String language = "";
		String teamViewName = "";
		bool isRobot = false;

	public:
		User() {}
		bool isMe();
		User copy();
		bool operator==(const User& other) const {
			return userId == other.userId;
		}
	};

	struct UserVipSort
	{
		bool operator()(const User* first, const User* second)
		{
			return first->vip > second->vip;
		}
	};

	class UserManager : public Singleton<UserManager>, public ObjectAlloc
	{
	public:
		UserManager();
		~UserManager();

	public:
		void addUser(ui64 userId, i32 vip, i32 teamId, i32 sex, const String& name, const String& teamName, const String& picUrl, const String& country, const String& lang);
		User* findUser(ui64 userId);
		void removeUser(ui64 userId);
		bool checkCountry(String country, String language);
		std::vector<ui64> getAllUser();

	private:
		bool onFriendOperationResult(int operationType, ui64 userId);
		bool onFriendOperationForHttpResult(int operationType, ui64 userId);
		bool onUserInfoUpdate(ui64 userId, String picUrl, String country, String language, i32 sex, i32 vip, bool isFriend);

		bool updateUserRelation(const vector<i64>::type& friends);
		void loadUserInfo(i64 userId);
		vector<ui64>::type userSort();
		
	private:
		SubscriptionGuard m_subscriptionGuard;
		map<ui64, User*>::type m_userMap;
		String m_meCountry = "";
		String m_meLanguage = "";
	};

};