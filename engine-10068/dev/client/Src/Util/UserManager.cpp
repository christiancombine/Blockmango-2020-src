#include "UserManager.h"
#include "RedDotManager.h"
#include "Util/ClientEvents.h"
#include "game.h"
#include "Network/ClientHttpRequest.h"
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Plugins/GameClientEvents.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	UserManager::UserManager()
	{
		using namespace  std::placeholders;
		m_subscriptionGuard.add(UserInfoUpdataEvent::subscribe(std::bind(&UserManager::onUserInfoUpdate, this, _1, _2, _3, _4, _5, _6, _7)));
		m_subscriptionGuard.add(FriendOperationForServerEvent::subscribe(std::bind(&UserManager::onFriendOperationResult, this, _1, _2)));
		m_subscriptionGuard.add(FriendOperationForAppHttpResultEvent::subscribe(std::bind(&UserManager::onFriendOperationForHttpResult, this, _1, _2)));
		m_subscriptionGuard.add(UpdateUserRelationEvent::subscribe(std::bind(&UserManager::updateUserRelation, this, _1)));
	}

	UserManager::~UserManager()
	{
		for (auto& user : m_userMap)
		{
			delete user.second;
			user.second = nullptr;
		}
		m_userMap.clear();
		m_subscriptionGuard.unsubscribeAll();
	}

	void UserManager::addUser(ui64 userId, i32 vip, i32 teamId, i32 sex, const String& name, const String& teamName, const String& picUrl, const String& country, const String& lang)
	{
		if (!findUser(userId))
		{
			User* user = new User();
			user->vip = vip;
			user->userId = userId;
			user->teamId = teamId;
			user->picUrl = picUrl;
			user->country = country;
			user->language = lang;
			user->userName = name;
			user->sex = sex;
			user->teamName = teamName;
			user->isRobot = (userId & 15) == 2;

			if (user->isMe())
			{
				m_meLanguage = lang;
				m_meCountry = country;
			}
			m_userMap.insert({ userId, user });
			UserEnterEvent::emit(userSort());
			SCRIPT_EVENT::UpdateUserListEvent::invoke(1);
			//loadUserInfo(userId);
		}
	}

	User * UserManager::findUser(ui64 userId)
	{
		auto user = m_userMap.find(userId);
		if (user != m_userMap.end())
		{
			return user->second;
		}
		return nullptr;
	}

	void UserManager::removeUser(ui64 userId)
	{
		auto user = m_userMap.find(userId);
		if (user != m_userMap.end())
		{
			delete user->second;
			user->second = nullptr;
			m_userMap.erase(userId);
		}
		UserOutEvent::emit(userId);
		SCRIPT_EVENT::UpdateUserListEvent::invoke(2);
	}

	bool UserManager::checkCountry(String country, String language)
	{
		return !m_meCountry.empty() && !m_meLanguage.empty() && m_meCountry == country && m_meLanguage == language;
	}

	std::vector<ui64> UserManager::getAllUser()
	{
		std::vector<ui64> userIds;
		for (auto user : m_userMap)
		{
			userIds.push_back(user.second->userId);
		}
		return userIds;
	}

	bool UserManager::onFriendOperationResult(int operationType, ui64 userId)
	{
		auto user = findUser(userId);
		if (!user)
		{
			return false;
		}
		switch (FriendOperationType(operationType))
		{
		case FriendOperationType::AGREE_ADD_FRIEND:
			user->isSend = false;
			user->isFriend = true;
			user->isFriendRequest = false;
			break;
		case FriendOperationType::REQUEST_ADD_FRIEND:
			user->isSend = false;
			user->isFriendRequest = true;
			RedDotManager::hasUserRequest = true;
			FriendRequestEvent::emit(userId);
			break;
		case FriendOperationType::NO_FRIEND:
			user->isFriend = false;
			break;
		case FriendOperationType::IS_FRIEND:
			user->isFriend = true;
			user->isFriendRequest = false;
			break;
		case FriendOperationType::DELETE_FRIEND:
		case FriendOperationType::NEGLECT_ADD_FRIEND:
			user->isSend = false;
			user->isFriend = false;
			user->isFriendRequest = false;
			break;
		}
		UserEnterEvent::emit(userSort());
		SCRIPT_EVENT::UpdateUserListEvent::invoke(3);
		return true;
	}

	bool UserManager::onFriendOperationForHttpResult(int operationType, ui64 userId)
	{
		auto user = findUser(userId);
		if (!user)
		{
			return false;
		}
		switch (FriendOperationType(operationType))
		{
		case FriendOperationType::AGREE_ADD_FRIEND:
			user->isFriend = true;
			user->isFriendRequest = false;
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendFriendOperation(operationType, userId);
			break;
		case FriendOperationType::NO_FRIEND:
			user->isFriend = false;
			break;
		case FriendOperationType::IS_FRIEND:
			user->isFriend = true;
			user->isFriendRequest = false;
			break;
		case FriendOperationType::REQUEST_ADD_FRIEND:
			user->isSend = true;
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendFriendOperation(operationType, userId);
			break;
		case FriendOperationType::DELETE_FRIEND:
			user->isSend = false;
			user->isFriend = false;
			user->isFriendRequest = false;
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendFriendOperation(operationType, userId);
			break;
		case FriendOperationType::NEGLECT_ADD_FRIEND:
			user->isSend = false;
			user->isFriend = false;
			user->isFriendRequest = false;
			break;
		case FriendOperationType::REQUEST_ADD_FRIEND_FAILURE:
			user->isSend = false;
			break;
		}
		UserEnterEvent::emit(userSort());
		SCRIPT_EVENT::UpdateUserListEvent::invoke(3);
		return true;
	}

	bool UserManager::onUserInfoUpdate(ui64 userId, String picUrl, String country, String language, i32 sex , i32 vip, bool isFriend)
	{
		if (auto user = findUser(userId))
		{
			if (!user)
				return false;
			user->vip = vip;
			user->sex = sex;
			user->picUrl = picUrl;
			user->country = country;
			user->language = language;
			user->isFriend = isFriend;
			if (user->isMe())
			{
				m_meCountry = country;
				m_meLanguage = language;
			}
			UserEnterEvent::emit(userSort());
			SCRIPT_EVENT::UpdateUserListEvent::invoke(3);
		}
		return true;
	}

	bool UserManager::updateUserRelation(const vector<i64>::type& friends)
	{
		for (const auto & userId : friends)
		{
			if (auto user = findUser(userId))
			{
				user->isFriend = true;
			}
		}
		UserEnterEvent::emit(userSort());
		SCRIPT_EVENT::UpdateUserListEvent::invoke(3);
		return false;
	}

	void UserManager::loadUserInfo(i64 userId)
	{
		String finalUrl = StringUtil::Replace("{url}/friend/api/v1/friends/{userId}", "{url}", ClientHttpRequest::getBaseUrl().c_str());
		finalUrl = StringUtil::Replace(finalUrl, "{userId}", StringUtil::ToString(userId).c_str());
		ClientHttpRequest::asyncGet(finalUrl, map<String, String>::type(), [userId](String result, bool isSuccess) {
			ClientHttpResponse response = ClientHttpRequest::parseResponse(isSuccess, "loadUserInfo", result);
			if (response.code == 1)
			{
				Document doc = Document();
				doc.Parse(response.data.c_str());
				if (!doc.HasParseError() && doc.IsObject())
				{
					i32 vip = 0;
					i32 sex = 1;
					String picUrl = "";
					String country = "";
					String language = "";
					bool isFriend = false;

					if (doc.HasMember("sex") && !doc.FindMember("sex")->value.IsNull())
					{
						sex = doc.FindMember("sex")->value.GetInt();
					}

					if (doc.HasMember("vip") && !doc.FindMember("vip")->value.IsNull())
					{
						vip = doc.FindMember("vip")->value.GetInt();
					}

					if (doc.HasMember("picUrl") && !doc.FindMember("picUrl")->value.IsNull())
					{
						picUrl = doc.FindMember("picUrl")->value.GetString();
					}

					if (doc.HasMember("country") && !doc.FindMember("country")->value.IsNull())
					{
						country = doc.FindMember("country")->value.GetString();
					}

					if (doc.HasMember("language") && !doc.FindMember("language")->value.IsNull())
					{
						language = doc.FindMember("language")->value.GetString();
					}

					if (doc.HasMember("friend") && !doc.FindMember("friend")->value.IsNull())
					{
						isFriend = doc.FindMember("friend")->value.GetBool();
					}

					UserInfoUpdataEvent::emit(userId, picUrl, country, language, sex, vip, isFriend);
				}
			}
		});
	}

	vector<ui64>::type UserManager::userSort()
	{

		vector<User*>::type users = vector<User*>::type();

		vector<User*>::type vipUsers = vector<User*>::type();

		vector<User*>::type requestFriends = vector<User*>::type();

		for (auto& item : m_userMap)
		{
			if (item.second->isMe())
			{
				continue;
			}

			if (item.second->isFriendRequest)
			{
				requestFriends.push_back(item.second);
			}
			else
			{
				if (item.second->vip > 0)
				{
					vipUsers.push_back(item.second);
				}
				else
				{
					users.push_back(item.second);
				}
			}
		}

		sort(requestFriends.begin(), requestFriends.end(), UserVipSort());
		
		sort(vipUsers.begin(), vipUsers.end(), UserVipSort());

		/*sort(users.begin(), users.end(), [](const User* first, const User* second) {
			String nickName1 = first->userName;
			String nickName2 = second->userName;
			StringUtil::LowerCase(nickName1);
			StringUtil::LowerCase(nickName2);
			char a1 = nickName1[0];
			char a2 = nickName2[0];
			return a1 < a2;
		});*/

		/*sort(users.begin(), users.end(), [this](const User* first, const User* second) {
			bool a1 = checkCountry(first->country, first->language);
			bool a2 = checkCountry(second->country, second->language);
			return !a2;
		});*/

		vector<ui64>::type ids;
		for (auto& item : requestFriends)
		{
			ids.push_back(item->userId);
		}

		for (auto& item : vipUsers)
		{
			ids.push_back(item->userId);
		}

		for (auto& item : users)
		{
			ids.push_back(item->userId);
		}

		ids.push_back(GameClient::CGame::Instance()->getPlatformUserId());

		return ids;
	}

	bool User::isMe()
	{
		return userId == GameClient::CGame::Instance()->getPlatformUserId();
	}

	User User::copy()
	{
		User user = User();
		user.sex = sex;
		user.vip = vip;
		user.teamId = teamId;
		user.userId = userId;
		user.isSend = isSend;
		user.isFriend = isFriend;
		user.isFriendRequest = isFriendRequest;
		user.userName = userName;
		user.teamName = teamName;
		user.picUrl = picUrl;
		user.message = message;
		user.country = country;
		user.language = language;
		user.teamViewName = teamViewName;
		user.isRobot = isRobot;
		return user;
	}

}