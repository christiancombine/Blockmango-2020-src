#include "GuiBlockCityFriendItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/UICommon.h"
#include "GUI/GuiItemStack.h"
#include "Util/ClientEvents.h"
#include "ShellInterface.h"
#include "game.h"
#include "Network/ClientHttpRequest.h"
#include "Util/TipType.h"
#include "Audio/SoundSystem.h"
#include "Audio/SoundDef.h"
#include "Setting/GameTypeSetting.h"

namespace BLOCKMAN
{
	
	void GuiBlockCityFriendItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCityFriendItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCityFriendItem::GuiBlockCityFriendItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		init("BlockCityFriendItem.json");
	}

	void GuiBlockCityFriendItem::init(const String & jsonName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON(jsonName.c_str()));
		renameComponents(m_window);

		m_icon = dynamic_cast<GuiUrlImage*>(m_window->GetChildByIndex(0));
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_starValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2)->GetChildByIndex(1));
		m_friendLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(3));
		m_applyLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(4));

		m_btnDelete = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3)->GetChildByIndex(0));
		m_btnRatify = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3)->GetChildByIndex(1));
		m_btnInvite = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3)->GetChildByIndex(2));
		m_btnVisit = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3)->GetChildByIndex(3));
		m_btnRefuse = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4)->GetChildByIndex(0));
		m_btnAgree = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4)->GetChildByIndex(1));

		m_btnDelete->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityFriendItem::onClick, this, std::placeholders::_1, ViewId::DELETE_FRIEND));
		m_btnRatify->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityFriendItem::onClick, this, std::placeholders::_1, ViewId::RATIFY));
		m_btnInvite->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityFriendItem::onClick, this, std::placeholders::_1, ViewId::INVITE));
		m_btnVisit->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityFriendItem::onClick, this, std::placeholders::_1, ViewId::VISIT));
		m_btnRefuse->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityFriendItem::onClick, this, std::placeholders::_1, ViewId::REFUSE));
		m_btnAgree->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityFriendItem::onClick, this, std::placeholders::_1, ViewId::AGREE));
		m_subscriptionGuard.add(BlockCityDeleteFriendEvent::subscribe(std::bind(&GuiBlockCityFriendItem::onDeleteFriend, this, std::placeholders::_1)));

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity)
		{
			m_sex = dynamic_cast<GUIStaticImage*>(m_name->GetChildByIndex(0));
			m_language = dynamic_cast<GUIStaticText*>(m_name->GetChildByIndex(1));
			m_offline = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(5));
			m_offline->GetChildByIndex(0)->SetText(LanguageManager::Instance()->getString("gui_offline").c_str());
			m_icon->setDefaultImage("set:blockcity_friend.json image:head_icon");
		}
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockFort)
		{
			m_icon->setDefaultImage("set:blockfort_friend.json image:head");
		}
		m_icon->setChildMaterial(GUIMaterialType_CULL_BACK);
		AddChildWindow(m_window);
	}

	void GuiBlockCityFriendItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_inviteTime > 0)
		{
			m_inviteTime -= nTimeElapse;
			m_inviteTime = m_inviteTime > 0 ? m_inviteTime : 0;
		}

		//m_btnInvite->SetEnabled(m_friend.isOnline && m_inviteTime == 0);
		m_btnInvite->SetEnabled(m_inviteTime == 0);
	}

	
	void GuiBlockCityFriendItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBlockCityFriendItem::setListItem(bool isFriendList)
	{
		m_friendLayout->SetVisible(true);
		m_applyLayout->SetVisible(false);
	}

	void GuiBlockCityFriendItem::setFriend(const Friend & firend)
	{
		m_friend = firend;
		m_friendLayout->SetVisible(true);
		m_applyLayout->SetVisible(false);
		m_btnRatify->SetVisible(false);
		m_icon->setShowHttpIcon(false);
		m_name->SetText(firend.nickName.c_str());
		
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity)
		{
			m_language->SetText(StringUtil::Format(getString("gui_language").c_str(), firend.language.c_str()).c_str());
			m_sex->SetImage(firend.sex == 1 ? "set:blockcity_friend.json image:boy" : "set:blockcity_friend.json image:girl");
			m_icon->setDefaultImage("set:blockcity_friend.json image:head_icon");
			m_window->SetBackImage(firend.isOnline ? "set:blockcity_friend.json image:lonline" : "set:blockcity_friend.json image:downline");
			m_starValue->SetText(StringUtil::ToString(firend.score).c_str());
			m_offline->SetVisible(!firend.isOnline);
		}
		
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockFort)
		{
			m_name->SetVisible(true);
			m_window->SetBackImage(firend.isOnline ? "set:blockfort_friend.json image:item_back" : "set:blockfort_friend.json image:item_back_off_line");
			m_starValue->SetText(StringUtil::Format("Lv.%d", firend.score).c_str());
		}
		m_icon->setUrl(firend.picUrl);
		//m_btnInvite->SetEnabled(firend.isOnline);
	}

	void GuiBlockCityFriendItem::setRequest(const UserRequest & request)
	{
		m_request = request;
		m_friendLayout->SetVisible(false);
		m_applyLayout->SetVisible(true);
		m_icon->setShowHttpIcon(false);
		m_name->SetText(request.nickName.c_str());
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity)
		{
			m_offline->SetVisible(false);
			m_language->SetText(StringUtil::Format(getString("gui_language").c_str(), request.language.c_str()).c_str());
			m_sex->SetImage(request.sex == 1 ? "set:blockcity_friend.json image:boy" : "set:blockcity_friend.json image:girl");
			m_icon->setDefaultImage("set:blockcity_friend.json image:head_icon");
			m_starValue->SetText(StringUtil::ToString(request.score).c_str());
		}
		
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockFort)
		{
			m_name->SetVisible(false);
			m_starValue->SetText(StringUtil::Format("Lv.%d", request.score).c_str());
		}
		m_icon->setUrl(request.picUrl);
	}

	bool GuiBlockCityFriendItem::onClick(const EventArgs& args, ViewId viewId)
	{
		if (m_request.userId == 0 && m_friend.userId == 0)
		{
			return false;
		}

		switch (viewId)
		{
		case ViewId::DELETE_FRIEND:
		{
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity)
			{
				vector<BlockCityPayment>::type pays = vector<BlockCityPayment>::type();
				ShowBlockCityShowCommonTipEvent::emit(pays, (int)BlockCityTipType::BLOCKCITY_DELETE_FRIEND, "gui_blockcity_delete_friend");
				BlockCitySendDeleteFriendIdEvent::emit(m_friend.userId);
			}
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockFort)
			{
				ShowBlockFortFunctionTipEvent::emit("gui_blockcity_delete_friend", (i32)BlockFortTipType::BLOCKFORT_DELETE_FRIEND);
				BlockFortSendDeleteFriendIdEvent::emit(m_friend.userId);
			}
		}
			break;
		case ViewId::RATIFY:
			break;
		case ViewId::INVITE:
			m_inviteTime = 30 * 1000;
			BlockCityFriendInviteEvent::emit(m_friend.userId);
			ClientNetwork::Instance()->getCommonSender()->sendPlayerOperation(5, m_friend.userId, GameClient::CGame::Instance()->getPicUrl(), true, 2);
			break;
		case ViewId::VISIT:
			ClientNetwork::Instance()->getSender()->sendCallOnManor(m_friend.userId, 2, true, m_friend.score);
			break;
		case ViewId::REFUSE:
			m_btnRefuse->SetEnabled(false);
			ClientHttpRequest::friendOperation(2, m_request.userId);
			break;
		case ViewId::AGREE:
			m_btnAgree->SetEnabled(false);
			ClientHttpRequest::friendOperation(1, m_request.userId);
			break;
		}
		SoundSystem::Instance()->playEffectByType(ST_Click);
		return true;
	}

	bool GuiBlockCityFriendItem::onDeleteFriend(i64 userId)
	{
		if (userId == m_friend.userId)
		{
			m_btnDelete->SetEnabled(false);
			ClientHttpRequest::friendOperation(3, m_friend.userId);
			SoundSystem::Instance()->playEffectByType(ST_Click);
		}
		return true;
	}
}