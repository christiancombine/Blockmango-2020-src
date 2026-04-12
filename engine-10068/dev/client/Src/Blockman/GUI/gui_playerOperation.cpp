#include "gui_playerOperation.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Game/Manor.h"
#include "ShellInterface.h"
#include "game.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/GameTypeSetting.h"
#include "Util/FriendManager.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_playerOperation::gui_playerOperation()
		: gui_layout("PlayerOperation.json")
	{

	}

	gui_playerOperation::~gui_playerOperation()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_playerOperation::onLoad()
	{
		getWindow<GUIButton>("PlayerOperation-Add-Friend")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this,std::placeholders::_1, ViewId::BTN_ADD_FRIEND ));
		getWindow<GUIButton>("PlayerOperation-Give-Rose")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_GIVE_ROSE));
		getWindow<GUIButton>("PlayerOperation-Call-On")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_CALL_ON));
		getWindow<GUIButton>("PlayerOperation-Greetings")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_GREETINGS));

		getWindow("PlayerOperation-Add-Friend-Name")->SetText(getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND));
		getWindow("PlayerOperation-Give-Rose-Name")->SetText(getString(LanguageKey::GUI_MANOR_GIVE_ROSE));
		getWindow("PlayerOperation-Call-On-Name")->SetText(getString(LanguageKey::GUI_MANOR_CALL_ON));
		getWindow("PlayerOperation-Greetings-Name")->SetText(getString(LanguageKey::GUI_PLAYER_OPERATION_GREETINGS));


		setText("PlayerOperation-BlockCity-AddFriend-Name", LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND);
		setText("PlayerOperation-BlockCity-Visit-Name", LanguageKey::GUI_MANOR_CALL_ON);
		setText("PlayerOperation-BlockCity-Inviter-Name", "gui_blockcity_invite");
		setText("PlayerOperation-BlockCity-Remove-Name", "gui_blockcity_remove_out");
		setText("PlayerOperation-BlockCity-Race-Name", "gui_blockcity_race");

		getWindow<GUIButton>("PlayerOperation-BlockCity-AddFriend")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_ADD_FRIEND));
		getWindow<GUIButton>("PlayerOperation-BlockCity-Visit")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_CALL_ON));
		getWindow<GUIButton>("PlayerOperation-BlockCity-Inviter")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_INVITER));
		getWindow<GUIButton>("PlayerOperation-BlockCity-Remove")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_REMOVE));
		getWindow<GUIButton>("PlayerOperation-BlockCity-Race")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_RACE_INVITE));

		setText("PlayerOperation-BlockFort-AddFriend-Name", LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND);
		setText("PlayerOperation-BlockFort-Visit-Name", LanguageKey::GUI_MANOR_CALL_ON);
		setText("PlayerOperation-BlockFort-Invite-Name", "gui_blockcity_invite");
		setText("PlayerOperation-BlockFort-MoveOut-Manor-Name", "gui_blockcity_remove_out");

		getWindow<GUIButton>("PlayerOperation-BlockFort-AddFriend")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_ADD_FRIEND));
		getWindow<GUIButton>("PlayerOperation-BlockFort-Visit")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_CALL_ON));
		getWindow<GUIButton>("PlayerOperation-BlockFort-Invite")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_INVITER));
		getWindow<GUIButton>("PlayerOperation-BlockFort-MoveOut-Manor")->subscribeEvent(EventButtonClick, std::bind(&gui_playerOperation::onClick, this, std::placeholders::_1, ViewId::BTN_REMOVE));

		m_subscriptionGuard.add(ServerSyncGameTypeEvent::subscribe(std::bind(&gui_playerOperation::onGameTypeUpdate, this)));
		m_subscriptionGuard.add(BlockCityRaceBtnEvent::subscribe(std::bind(&gui_playerOperation::onBlockCityRaceBtnUpdate, this, std::placeholders::_1)));
		onGameTypeUpdate();
	}

	void gui_playerOperation::onUpdate(ui32 nTimeElapse)
	{
		for (auto& invite : m_inviteMap)
		{
			for (auto& item : invite.second)
			{
				if (item.second > 0)
				{
					item.second -= nTimeElapse;
					item.second = item.second < 0 ? 0 : item.second;
					if (invite.first == m_targetUserId && item.first == ViewId::BTN_RACE_INVITE && item.second == 0)
					{
						setRaceVehileStatusEvent::emit(false);
					}
				}
			}
		}

		getWindow<GUIButton>("PlayerOperation-BlockCity-Inviter")->SetEnabled(m_inviteMap[m_targetUserId][ViewId::BTN_INVITER] == 0);
		getWindow<GUIButton>("PlayerOperation-BlockCity-Race")->SetEnabled(m_inviteMap[m_targetUserId][ViewId::BTN_RACE_INVITE] == 0);
		getWindow<GUIButton>("PlayerOperation-BlockCity-AddFriend")->SetEnabled(FriendManager::findFriendById(m_targetUserId) ? false : true);
	}

	bool gui_playerOperation::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_ADD_FRIEND:
			GameClient::CGame::Instance()->getShellInterface()->onFriendOperation(2, m_targetUserId);
			break;
		case ViewId::BTN_GIVE_ROSE:
			ClientNetwork::Instance()->getSender()->sendGiveRose(m_targetUserId);
			break;
		case ViewId::BTN_CALL_ON:
			ClientNetwork::Instance()->getSender()->sendCallOnManor(m_targetUserId, 3, FriendManager::findFriendById(m_targetUserId) ? true : false, 0);
			break;
		case ViewId::BTN_INVITER:
		case ViewId::BTN_RACE_INVITE:
		{
			m_inviteMap[m_targetUserId][viewId] = 30 * 1000;
			ClientNetwork::Instance()->getCommonSender()->sendPlayerOperation(int(viewId), m_targetUserId, GameClient::CGame::Instance()->getPicUrl(), FriendManager::findFriendById(m_targetUserId) ? true : false, 3);
			setRaceVehileStatusEvent::emit(true);
		}
			break;
		case ViewId::BTN_REMOVE:
			ClientNetwork::Instance()->getCommonSender()->sendPlayerOperation(8, m_targetUserId, "");
			break;
		case ViewId::BTN_GREETINGS:
			Blockman::Instance()->m_pPlayer->setInsertUpperAction(ActionState::AS_GREETINGS, 1100);
			LORD::vector<String>::type args;
			args.push_back(Blockman::Instance()->m_pPlayer->getEntityName());
			ClientNetwork::Instance()->getSender()->sendPlayerInteractionActionMesage(m_targetUserId, LanguageKey::GUI_PLAYER_OPERATION_GREETINGS_MESSAGE, args);
			String msg = StringUtil::Format(getString("gui_player_operation_greetings_message_for_me").c_str(), m_targetUserName.c_str());
			ChatMessageReceiveEvent::emit(msg,false);
			break;
		}
		RootGuiLayout::Instance()->hidePlayerOperation();
		return true;
	}

	bool gui_playerOperation::onGameTypeUpdate()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::Tiny_Town:
			getWindow<GUIButton>("PlayerOperation-Greetings")->SetVisible(false);
			getWindow("PlayerOperation-Content")->SetVisible(true);
			getWindow("PlayerOperation-BlockCity")->SetVisible(false);
			getWindow("PlayerOperation-BlockFort")->SetVisible(false);
			break;
		case ClientGameType::Jewel_Knight:
			getWindow<GUIButton>("PlayerOperation-Give-Rose")->SetVisible(false);
			getWindow<GUIButton>("PlayerOperation-Call-On")->SetVisible(false);

			getWindow("PlayerOperation-Content")->SetVisible(true);
			getWindow("PlayerOperation-BlockCity")->SetVisible(false);
			getWindow("PlayerOperation-BlockFort")->SetVisible(false);
			break;
		case ClientGameType::BlockCity:
			getWindow("PlayerOperation-Content")->SetVisible(false);
			getWindow("PlayerOperation-BlockCity")->SetVisible(true);
			getWindow("PlayerOperation-BlockFort")->SetVisible(false);
			break;
		case ClientGameType::BlockFort:
			getWindow("PlayerOperation-Content")->SetVisible(false);
			getWindow("PlayerOperation-BlockCity")->SetVisible(false);
			getWindow("PlayerOperation-BlockFort")->SetVisible(true);
			break;
		default:
			getWindow<GUIButton>("PlayerOperation-Give-Rose")->SetVisible(false);
			getWindow<GUIButton>("PlayerOperation-Call-On")->SetVisible(false);
			getWindow<GUIButton>("PlayerOperation-Greetings")->SetVisible(false);

			getWindow("PlayerOperation-Content")->SetVisible(true);
			getWindow("PlayerOperation-BlockCity")->SetVisible(false);
			getWindow("PlayerOperation-BlockFort")->SetVisible(false);
			break;
		}
		return true;
	}

	bool gui_playerOperation::onBlockCityRaceBtnUpdate(i64 targetUserId)
	{
		m_inviteMap[targetUserId][ViewId::BTN_RACE_INVITE] = 0;
		setRaceVehileStatusEvent::emit(false);
		return true;
	}

	void gui_playerOperation::setRemoveBtnEnabled(bool isEnabled)
	{
		if (checkGameType(ClientGameType::BlockCity))
		{
			getWindow<GUIButton>("PlayerOperation-BlockCity-Remove")->SetEnabled(isEnabled);
		}
		if (checkGameType(ClientGameType::BlockFort))
		{
			getWindow<GUIButton>("PlayerOperation-BlockFort-MoveOut-Manor")->SetEnabled(isEnabled);
		}
	}
}
