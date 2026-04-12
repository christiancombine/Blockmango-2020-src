#include "GuiPlayerItem.h"
#include "ShellInterface.h"
#include "game.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Util/UserManager.h"
#include "Util/ClientDataReport.h"
#include "Setting/GameTypeSetting.h"
#include "Setting/FunctionSetting.h"
#include "Network/ClientHttpRequest.h"

namespace BLOCKMAN
{
	void GuiPlayerItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiPlayerItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiPlayerItem::GuiPlayerItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("MenuPlayerItem.json"));
		renameComponents(m_window);
		m_friendIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_playerName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_sexIcon = dynamic_cast<GUIStaticImage*>(m_playerName->GetChildByIndex(0));
		m_vipIcon = dynamic_cast<GUIStaticImage*>(m_playerName->GetChildByIndex(1));
		m_messageView = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_btnIgnore = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_btnAgree = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));
		m_btnAddFriend = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		m_btnCallOn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(6));
		m_icon = dynamic_cast<GuiUrlImage*>(m_window->GetChildByIndex(7));
		m_desc = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(8));
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain)
		{
			m_btnCallOn->SetArea(UDim(0, -154), UDim(0, 0), UDim(0, 117), UDim(0, 54));
		}
		m_btnIgnore->subscribeEvent(EventButtonClick, std::bind(&GuiPlayerItem::onClick, this, std::placeholders::_1, ViewId::BTN_NEGLECT));
		m_btnAgree->subscribeEvent(EventButtonClick, std::bind(&GuiPlayerItem::onClick, this, std::placeholders::_1, ViewId::BTN_AGREE));
		m_btnAddFriend->subscribeEvent(EventButtonClick, std::bind(&GuiPlayerItem::onClick, this, std::placeholders::_1 ,ViewId::BTN_ADD_FRIEND));
		m_btnCallOn->subscribeEvent(EventButtonClick, std::bind(&GuiPlayerItem::onClick, this, std::placeholders::_1, ViewId::BTN_CALL_ON));
		m_btnAddFriend->SetText(getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND));
		m_btnAgree->SetText(getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_BTN_AGREE));
		m_btnIgnore->SetText(getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_BTN_IGNORE));
		m_btnCallOn->SetText(getString(LanguageKey::GUI_MANOR_CALL_ON));
		m_messageView->SetText(getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_MSG));
		m_icon->setDefaultImage("set:user_item.json image:pic");
		m_icon->setChildMaterial(GUIMaterialType_CULL_NONE);
		AddChildWindow(m_window);
	}

	void GuiPlayerItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		m_playerName->SetText(m_playerInfo.userName.c_str());
		m_friendIcon->SetVisible(m_playerInfo.isFriend || isMe());
		m_friendIcon->SetImage(isMe() ? "set:user_item.json image:me" : "set:new_gui_material.json image:menu_player_item_friend_icon");
		m_btnCallOn->SetVisible(isShowCallOnbtn());
		if (m_playerInfo.isFriend)
		{
			m_playerName->SetTextColor(Color(0.647059f, 0.913726f, 0.364706f));	
		} 
		else
		{
			m_playerName->SetTextColor(m_playerInfo.isFriendRequest ? Color(1.0f, 0.235294f, 0.196078f) : Color(0.92549f, 0.870588f, 0.788235f));
		}
		m_messageView->SetVisible(m_playerInfo.isFriendRequest && !isMe());
		m_btnIgnore->SetVisible(m_playerInfo.isFriendRequest  && !isMe());
		m_btnAgree->SetVisible(m_playerInfo.isFriendRequest && !isMe());
		m_btnAddFriend->SetVisible(!m_playerInfo.isFriend && !m_playerInfo.isFriendRequest && !isMe() && FunctionSetting::Instance()->enabledFriend());
		m_btnAddFriend->SetText(getString(m_playerInfo.isSend ? LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_MSG_SENT : LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND));
		m_btnAddFriend->SetEnabled(!m_playerInfo.isSend);
	}

	void GuiPlayerItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiPlayerItem::setUser(User info)
	{
		m_playerInfo = info;
		m_desc->SetText("");
		m_icon->setDefaultImage("set:user_item.json image:pic");
		m_icon->setUrl(info.picUrl);
		m_vipIcon->SetImage(getVipIcon().c_str());
		m_sexIcon->SetImage(getSexIcon().c_str());
		m_desc->SetText((!isMe() && UserManager::Instance()->checkCountry(info.country, info.language)) ? getString("gui_compatriot") : "");
	}

	bool GuiPlayerItem::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_NEGLECT:
			m_playerInfo.isFriendRequest = false;
			FriendOperationForAppHttpResultEvent::emit(6, m_playerInfo.userId);
			GameClient::CGame::Instance()->dataReportNotType(ClientDataReport::GAME_FRIEND_IGNORE, "2");
			m_btnAddFriend->SetText(getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND));
			m_btnAddFriend->SetEnabled(true);
			break;
		case ViewId::BTN_AGREE:
			GameClient::CGame::Instance()->dataReportNotType(ClientDataReport::GAME_FRIEND_AGREE, "2");
			GameClient::CGame::Instance()->getShellInterface()->onFriendOperation(1, m_playerInfo.userId);
			break;
		case ViewId::BTN_ADD_FRIEND:
			//GameClient::CGame::Instance()->getShellInterface()->onFriendOperation(2, m_playerInfo.userId);
			if (m_playerInfo.isRobot)
			{
				FriendOperationForAppHttpResultEvent::emit(4, m_playerInfo.userId);
			}
			else
			{
				ClientHttpRequest::friendOperation(4, m_playerInfo.userId);
				GameClient::CGame::Instance()->dataReportNotType(ClientDataReport::GAME_FRIEND_ADD, "");
				m_btnAddFriend->SetText(getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_MSG_SENT));
				m_btnAddFriend->SetEnabled(false);
			}
			break;
		case ViewId::BTN_CALL_ON:
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendCallOnManor(m_playerInfo.userId);
			break;
		}
		return true;
	}

	void GuiPlayerItem::resetAddFriendButton()
	{
		m_btnAddFriend->SetText(getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND));
		m_btnAddFriend->SetEnabled(true);
	}

	bool GuiPlayerItem::isMe()
	{
		return m_playerInfo.userId == GameClient::CGame::Instance()->getPlatformUserId();
	}

	bool GuiPlayerItem::isShowCallOnbtn()
	{
		return ((GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_playerInfo.isFriend) 
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain) && m_playerInfo.userId != GameClient::CGame::Instance()->getPlatformUserId();
	}

	String GuiPlayerItem::getSexIcon()
	{
		switch (m_playerInfo.sex)
		{
		case 1:
			return "set:user_item.json image:boy";
		case 2:
			return "set:user_item.json image:girl";
		default:
			return "set:user_item.json image:boy";
		}
	}

	String GuiPlayerItem::getVipIcon()
	{
		switch (m_playerInfo.vip)
		{
		case 1:
			return "set:user_item.json image:vip_1";
		case 2:
			return "set:user_item.json image:vip_2";
		case 3:
			return "set:user_item.json image:vip_3";
		case 4:
			return "set:user_item.json image:vip_4";
		default:
			return "";
		}
	}
}