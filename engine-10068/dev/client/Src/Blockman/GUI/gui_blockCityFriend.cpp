#include "gui_blockCityFriend.h"
#include "RootGuiLayout.h"
#include "Util/UICommon.h"
#include "Setting/HelpSetting.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Game/BlockCity.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiBlockCityFriendItem.h"
#include "GuiDef.h"
#include "Network/ClientHttpRequest.h"
#include "Util/RankManager.h"
#include "Util/RedDotManager.h"
#include "UI/GUIImagesetManager.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIListBox.h"

using namespace LORD;

namespace BLOCKMAN
{
	void gui_blockCityFriend::initView()
	{
		if (checkGameType(ClientGameType::BlockCity))
		{
			m_fileName = "BlockCityFriend";
			m_jsonName = "BlockCityFriend.json";
			GUIImagesetManager::Instance()->CreateFromFile("blockcity_friend.json");
		}

		if (checkGameType(ClientGameType::BlockFort))
		{
			m_fileName = "BlockFortFriend";
			m_jsonName = "BlockFortFriend.json";
			GUIImagesetManager::Instance()->CreateFromFile("blockfort_friend.json");
		}
	}

	String gui_blockCityFriend::getViewName(const String& name)
	{
		return StringUtil::Format("%s-%s", m_fileName.c_str(), name.c_str());
	}

	gui_blockCityFriend::gui_blockCityFriend() :
		gui_layout("BlockCityFriend.json")
	{
		if (checkGameType({ ClientGameType::BlockCity, ClientGameType::BlockFort }))
		{
			ClientHttpRequest::loadUserRequests();
		}
	}


	gui_blockCityFriend::~gui_blockCityFriend()
	{
	}

	void gui_blockCityFriend::onShow()
	{
		ClientHttpRequest::loadUserRequests();
		ClientHttpRequest::loadFriend();
		if (m_showViewId == ViewId::APPLY && checkGameType(ClientGameType::BlockCity))
		{
			RedDotManager::hasUserRequest = false;
		}
	}

	void gui_blockCityFriend::onHide()
	{
	}

	void gui_blockCityFriend::onStart()
	{
		initView();
	}

	void gui_blockCityFriend::onLoad()
	{
		setText(getViewName("Title-Text"), "gui_blockcity_friend_list");
		setText(getViewName("Friend-List"), "gui_blockcity_friend_list");
		setText(getViewName("Apply-List"), "gui_blockcity_apply_list");

		m_friendList = getWindow<GUIListBox>(getViewName("Friend").c_str());
		m_applyList = getWindow<GUIListBox>(getViewName("Apply").c_str());
		m_radioContentMap[ViewId::FRIEND] = m_friendList;
		m_radioContentMap[ViewId::APPLY] = m_applyList;
		m_radioMap[ViewId::FRIEND] = getWindow<GUIRadioButton>(getViewName("Friend-List").c_str());
		m_radioMap[ViewId::APPLY] = getWindow<GUIRadioButton>(getViewName("Apply-List").c_str());

		for (auto item : m_radioMap)
		{
			item.second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityFriend::onRadioChange, this, std::placeholders::_1, item.first));
		}
		getWindow(getViewName("Close").c_str())->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityFriend::onClose, this, std::placeholders::_1));
		m_radioMap[ViewId::FRIEND]->SetSelected(true);

		m_subscriptionGuard.add(BlockCityFriendInviteEvent::subscribe(std::bind(&gui_blockCityFriend::onInviteClick, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateUserRequestEvent::subscribe(std::bind(&gui_blockCityFriend::onUpdateUserRequest, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateUserFriendEvent::subscribe(std::bind(&gui_blockCityFriend::onUpdateUserFriend, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateBlockCityUserRequestRankEvent::subscribe(std::bind(&gui_blockCityFriend::onUpdateUserRequestRank, this, std::placeholders::_1)));
	}

	void gui_blockCityFriend::onUpdate(ui32 nTimeElapse)
	{
		for (auto& item : m_inviteTime)
		{
			if (item.second > 0)
			{
				item.second -= nTimeElapse;
				item.second = item.second < 0 ? 0 : item.second;
			}
		}

		if (checkGameType(ClientGameType::BlockCity))
		{
			if (getWindow("BlockCityFriend-ApplyReddot"))
			{
				getWindow("BlockCityFriend-ApplyReddot")->SetVisible(RedDotManager::checkUserRequest());
			}
		}
	}

	bool gui_blockCityFriend::onClose(const EventArgs& args)
	{
		RootGuiLayout::Instance()->showMainControl();
		if (checkGameType(ClientGameType::BlockCity))
		{
			UpdateIsHaveViewShowEvent::emit(false);
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockCityFriend::isNeedLoad()
	{
		return checkGameType({ ClientGameType::BlockCity, ClientGameType::BlockFort });
	}

	bool gui_blockCityFriend::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioMap[viewId]->IsSelected())
		{
			m_showViewId = viewId;
			for (auto item : m_radioContentMap)
			{
				item.second->SetVisible(item.first == viewId);
				if (item.first == ViewId::APPLY && checkGameType(ClientGameType::BlockCity))
				{
					RedDotManager::hasUserRequest = false;
				}
			}
		}
		return true;
	}

	bool gui_blockCityFriend::onUpdateUserFriend(const vector<Friend>::type& items)
	{
		static i32 count = 0;
		m_friendList->SetScrollOffset(0);
		vector<Friend>::type onlineFriend = {};
		vector<Friend>::type offlineFriend = {};
		if (!m_friendList)
			return false;

		for (const auto& item : items)
		{
			if (item.isOnline)
			{
				onlineFriend.push_back(item);
			}
			else
			{
				offlineFriend.push_back(item);
			}
		}

		if (onlineFriend.size() > 1)
		{
			sort(onlineFriend.begin(), onlineFriend.end(), [](const Friend & f1, const Friend &f2) {
				return f1.score > f2.score;
			});
		}

		if (offlineFriend.size() > 1)
		{
			sort(offlineFriend.begin(), offlineFriend.end(), [](const Friend & f1, const Friend &f2) {
				return f1.score > f2.score;
			});
		}

		m_friendItems.clear();
		m_friendItems.insert(m_friendItems.end(), onlineFriend.begin(), onlineFriend.end());
		m_friendItems.insert(m_friendItems.end(), offlineFriend.begin(), offlineFriend.end());

		if (!m_friendList->getContainerWindow() || m_friendList->getContainerWindow()->GetChildCount() != m_friendItems.size())
		{
			m_friendList->ClearAllItem();
			for (const auto& item : m_friendItems)
			{
				String iterLayout = StringUtil::Format("BlockCityFriend-Friend-Items-%d", count++).c_str();
				GuiBlockCityFriendItem* itemView = (GuiBlockCityFriendItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_FRIEND_ITEM, iterLayout.c_str());
				if (checkGameType(ClientGameType::BlockCity))
				{
					itemView->init("BlockCityFriendItem.json");
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 80.f));
				}
				if (checkGameType(ClientGameType::BlockFort))
				{
					itemView->init("BlockFortFriendItem.json");
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 72.f));
				}
				itemView->setInviteTime(m_inviteTime[item.userId]);
				itemView->setFriend(item);
				m_friendList->AddItem(itemView, false);
			}
		}
		else
		{
			for (size_t i = 0; i < m_friendList->getContainerWindow()->GetChildCount(); i++)
			{
				auto itemView = dynamic_cast<GuiBlockCityFriendItem*>(m_friendList->getContainerWindow()->GetChildByIndex(i));
				if (checkGameType(ClientGameType::BlockCity))
				{
					itemView->init("BlockCityFriendItem.json");
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 80.f));
				}
				if (checkGameType(ClientGameType::BlockFort))
				{
					itemView->init("BlockFortFriendItem.json");
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 72.f));
				}
				itemView->setInviteTime(m_inviteTime[m_friendItems[i].userId]);
				itemView->setFriend(m_friendItems[i]);
			}
		}
		return true;
	}

	bool gui_blockCityFriend::onUpdateUserRequest(const vector<UserRequest>::type& items)
	{
		if (!m_applyList)
			return false;

		static i32 count = 0;
		m_requestItems = items;
		m_applyList->SetScrollOffset(0);
		if (!m_applyList->getContainerWindow() || m_applyList->getContainerWindow()->GetChildCount() != m_requestItems.size())
		{
			m_applyList->ClearAllItem();
			for (const auto& item : m_requestItems)
			{
				String iterLayout = StringUtil::Format("BlockCityFriend-Apply-Items-%d", count++).c_str();
				GuiBlockCityFriendItem* itemView = (GuiBlockCityFriendItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_FRIEND_ITEM, iterLayout.c_str());
				if (checkGameType(ClientGameType::BlockCity))
				{
					itemView->init("BlockCityFriendItem.json");
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 80.f));
				}
				if (checkGameType(ClientGameType::BlockFort))
				{
					itemView->init("BlockFortFriendItem.json");
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 72.f));
				}
				itemView->setRequest(item);
				m_applyList->AddItem(itemView, false);
			}
		}
		else
		{
			for (size_t i = 0; i < m_applyList->getContainerWindow()->GetChildCount(); i++)
			{
				auto itemView = dynamic_cast<GuiBlockCityFriendItem*>(m_applyList->getContainerWindow()->GetChildByIndex(i));
				if (checkGameType(ClientGameType::BlockCity))
				{
					itemView->init("BlockCityFriendItem.json");
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 80.f));
				}
				if (checkGameType(ClientGameType::BlockFort))
				{
					itemView->init("BlockFortFriendItem.json");
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 72.f));
				}
				itemView->setRequest(m_requestItems[i]);
			}
		}
		return true;
	}

	bool gui_blockCityFriend::onUpdateUserRequestRank(const vector<UserRank>::type & items)
	{
		for (auto& requestItem : m_requestItems)
		{
			for (const auto& rankItem : items)
			{
				if (rankItem.userId == requestItem.userId)
				{
					requestItem.score = rankItem.score;
					break;
				}
			}
		}
		onUpdateUserRequest(m_requestItems);
		return true;
	}

	bool gui_blockCityFriend::onInviteClick(i64 userId)
	{
		m_inviteTime[userId] = 30 * 1000;
		return false;
	}

}
