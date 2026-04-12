#include "gui_playerList.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "Util/RedDotManager.h"
#include "GuiDef.h"
#include "game.h"


namespace BLOCKMAN {

	gui_playerList::gui_playerList()
		: gui_layout("MenuPlayer.json")
	{
		using namespace  std::placeholders;
		m_subscriptionGuard.add(UserEnterEvent::subscribe(std::bind(&gui_playerList::onUserIn, this, _1)));
		m_subscriptionGuard.add(UserOutEvent::subscribe(std::bind(&gui_playerList::onUserOut, this, _1)));
		m_subscriptionGuard.add(FriendOperationForServerEvent::subscribe(std::bind(&gui_playerList::onFriendOperationForServer, this, _1, _2)));
		m_subscriptionGuard.add(FriendOperationForAppHttpResultEvent::subscribe(std::bind(&gui_playerList::onFriendOperationForAppHttpResult, this, _1, _2)));
	}

	gui_playerList::~gui_playerList()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_playerList::onLoad()
	{
		m_playerList = getWindow<GUIListBox>("MenuPlayer-PlayerList");
		m_tipMessageView = getWindow<GUIStaticText>("MenuPlayer-Tip-Message");
		m_tipMessageView->SetText("");
	}

	void gui_playerList::onUpdate(ui32 nTimeElapse)
	{
		onInitWaitItems();
		onHandleWaitFriendOperations();
		if (m_tipShopTime == 0 || m_tipShopTime > 2000)
		{
			m_tipMessageView->SetText("");
		}
		else
		{
			m_tipShopTime += nTimeElapse;
			m_tipMessageView->SetText(m_tipMessage.c_str());
		}
	}

	void gui_playerList::onInitWaitItems()
	{
		if (!m_playerList || m_waitInitItems.empty())
		{
			return;
		}
		static i32 count = 0;
		for (size_t i = 0; i < m_waitInitItems.size(); i++)
		{
			auto user = UserManager::Instance()->findUser(m_waitInitItems[i]);
			if (!user)
			{
				continue;
			}
			if (m_playerList->getContainerWindow() && i < m_playerList->getContainerWindow()->GetChildCount())
			{
				auto view = (GuiPlayerItem*) m_playerList->getContainerWindow()->GetChildByIndex(i);
				view->setUser(user->copy());
				m_playerItemMap[m_waitInitItems[i]] = view;
			}
			else
			{
				String itemName = StringUtil::Format("MenuPlayer-PlayerList-Info-Item-%d", count++).c_str();
				user->teamViewName = itemName;
				GuiPlayerItem* playerItem = (GuiPlayerItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_PLYAER_ITEM, itemName.c_str());
				playerItem->setUser(user->copy());
				playerItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.0f, 0), UDim(0, 70.0f));
				playerItem->SetTouchable(false);
				m_playerItemMap[m_waitInitItems[i]] = playerItem;
				m_playerList->AddItem(playerItem, false);
			}
		}
		m_waitInitItems.clear();
	}

	void gui_playerList::onHandleWaitFriendOperations()
	{
		if (!m_playerList || m_waitHandleFriendOperations.empty())
		{
			return;
		}

		for (auto operation : m_waitHandleFriendOperations)
		{
			switch (operation.type)
			{
			case FriendOperationType::DELETE_FRIEND:
				//if (m_playerItemMap[operation.userId])
				//{
				//	m_playerItemMap[operation.userId]->setFriend(false);
				//	m_playerItemMap[operation.userId]->setFriendRequest(false);
				//	m_playerItemMap[operation.userId]->resetAddFriendButton();
				//}
				break;
			case FriendOperationType::REQUEST_ADD_FRIEND_FAILURE:
				m_tipShopTime = 1;
				m_tipMessage = getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_MSG_SEND_FAILURE).c_str();
				//if (m_playerItemMap[operation.userId])
				//{
				//	m_playerItemMap[operation.userId]->resetAddFriendButton();
				//}
				break;
			case FriendOperationType::AGREE_ADD_FRIEND_FAILURE:
				m_tipShopTime = 1;
				m_tipMessage = getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_MSG_AGREE_FAILURE).c_str();
				break;
			}
		}
		m_waitHandleFriendOperations.clear();
	}

	bool gui_playerList::onUserIn(vector<ui64>::type ids)
	{
		m_waitInitItems.clear();
		m_waitInitItems = ids;
		return true;
	}

	bool gui_playerList::onUserOut(const ui64 playerPlatformId)
	{
		if (m_playerItemMap[playerPlatformId])
		{
			if (m_playerList)
			{
				m_playerList->DeleteItem(getIndex(m_playerItemMap[playerPlatformId]->GetName().c_str()));
			}
			m_playerItemMap.erase(playerPlatformId);
		}

		for (vector<ui64>::const_iterator iter = m_waitInitItems.cbegin(); iter != m_waitInitItems.cend(); iter++)
		{
			if (*iter == playerPlatformId)
			{
				m_waitInitItems.erase(iter);
				break;
			}
		}
		return true;
	}

	bool gui_playerList::onFriendOperationForServer(int operationType, ui64 playerPlatformId)
	{
		FriendOperation operation = FriendOperation();
		operation.type = (FriendOperationType)operationType;
		operation.userId = playerPlatformId;
		m_waitHandleFriendOperations.push_back(operation);

		if (operationType == (int)FriendOperationType::REQUEST_ADD_FRIEND)
		{
			playSoundByType(ST_friend_invite);
			RedDotManager::hasUserRequest = true;
		}
		return true;
	}

	bool gui_playerList::onFriendOperationForAppHttpResult(int operationType, ui64 playerPlatformId)
	{
		FriendOperation operation = FriendOperation();
		operation.type = (FriendOperationType)operationType;
		operation.userId = playerPlatformId;
		m_waitHandleFriendOperations.push_back(operation);
		return true;
	}

	int gui_playerList::getIndex(String viewName)
	{
		if (m_playerList == nullptr || m_playerList->getContainerWindow() == nullptr)
		{
			return -1;
		}
		return  m_playerList->getContainerWindow()->GetChildIndex(viewName.c_str());
	}
}