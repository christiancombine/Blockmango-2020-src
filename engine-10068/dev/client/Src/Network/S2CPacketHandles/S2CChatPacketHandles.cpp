#include "S2CChatPacketHandles.h"
#include "Network/ClientNetwork.h"
#include "Entity/Entity.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/EntityOtherPlayerMP.h"
#include "World/World.h"
#include "Entity/EntityPlayer.h"

#include "game.h"

#include "Network/DataCache.h"
#include "Network/ClientNetwork.h"
#include "Chat/ChatComponent.h"
#include "Util/LanguageManager.h"
#include "Util/ClientEvents.h"
#include "Setting/TeamInfoSetting.h"
#include "Setting/MultiLanTipSetting.h"

using namespace BLOCKMAN;

void S2CChatPacketHandles::handlePacket(std::shared_ptr<S2CPacketChatString>& packet)
{
	std::shared_ptr<ChatComponentString> chatString = packet->m_chatComponent;
	if (chatString)
	{
		auto dataCache = ClientNetwork::Instance()->getDataCache();
		auto pPlayer = dataCache->getPlayerByServerId(chatString->sendId);
		if (pPlayer)
		{
			int teamId = pPlayer->getTeamId();
			LordLogInfo("ChatComponentString tean id %d", teamId);
			String nameColor = "";
			ClientGameType gameType = GameClient::CGame::Instance()->GetGameType();
			if (teamId != 0)
			{
				const TeamInfo& teamInfo = TeamInfoSetting::getTeamInfo((int)gameType, teamId);
				nameColor = teamInfo.TeamColor.c_str();
			}
			ChatMessageReceiveEvent::emit(StringUtil::Format("%s<%s>▢FFFFFFFF %s", nameColor.c_str(), pPlayer->getEntityName().c_str(), chatString->msg.c_str()), true);
			pPlayer->setSayMessage(StringUtil::Format("▢FF000000%s", chatString->msg.c_str()), 10.f);
		}
		else
		{
			ChatMessageReceiveEvent::emit(StringUtil::Format("<%s> %s", chatString->payerNickName.c_str(), chatString->msg.c_str()),true);
		}
	}
}

void S2CChatPacketHandles::handlePacket(std::shared_ptr<S2CPacketChatTranslation>& packet)
{
	std::shared_ptr<ChatComponentTranslation> tanslation = packet->m_chatComponent;
	if (tanslation)
	{
		String msg = LanguageManager::Instance()->getString(tanslation->key);
		if (msg.length() == 0) { return; }
		vector<FormatArgs>::type formatArgs = tanslation->formatArgs;
		vector<FormatArgs>::iterator iter;
		for (iter = formatArgs.begin(); iter != formatArgs.end(); iter++)
		{
			FormatArgs args = *iter;
			switch (args.type)
			{
			case FormatArgs::Type::INT:
				msg = StringUtil::Format(msg.c_str(), args.intValue);
				break;
			case FormatArgs::Type::PLAYER_ID:
			{
				auto dataCache = ClientNetwork::Instance()->getDataCache();
				auto pPlayer = dataCache->getPlayerByServerId(args.playerId);
				if (pPlayer)
				{
					msg = StringUtil::Format(msg.c_str(), pPlayer->getEntityName().c_str());
				}
				break;
			}
			case FormatArgs::Type::STRING:
				msg = StringUtil::Format(msg.c_str(), args.strValue.c_str());
				break;
			case FormatArgs::Type::CHAT_COMPONENT:
				break;
			default:
				msg = StringUtil::Format(msg.c_str(), args.strValue.c_str());
			}
		}
		ChatMessageReceiveEvent::emit(msg,false);
	}
}

void S2CChatPacketHandles::handlePacket(std::shared_ptr<S2CPacketChatNotification>& packet)
{
	std::shared_ptr<ChatComponentNotification> chatNotification = packet->m_chatComponent;
	if (chatNotification && chatNotification->msgContext.length() > 0)
	{
		chatNotification->msgContext = LanguageManager::Instance()->getString(chatNotification->msgContext);

		switch (chatNotification->type)
		{
		case ChatComponentNotification::Type::NATURE:
			ChatMessageReceiveEvent::emit(chatNotification->msgContext,false);
			break;
		case ChatComponentNotification::Type::TIMING_TOP:
			TopSystemNotificationEvent::emit(chatNotification->msgContext, chatNotification->keepTime);
			break;
		case ChatComponentNotification::Type::TIMING_BOTTOM:
			BottomSystemNotificationEvent::emit(chatNotification->msgContext, chatNotification->keepTime);
			break;
		case ChatComponentNotification::Type::TIMING_CENTER:
			CenterSystemNotificationEvent::emit(chatNotification->msgContext, chatNotification->keepTime);
			break;
		case ChatComponentNotification::Type::TEAM_RESOURCES_NOTIFICATION:
			TeamResourcesShowNotificationEvent::emit(chatNotification->msgContext, chatNotification->keepTime);
			break;
		case ChatComponentNotification::Type::TIMING_RIGHT:
			BirdAddMessageEvent::emit(chatNotification->msgContext);
			break;
		}
	}
}

void S2CChatPacketHandles::handlePacket(std::shared_ptr<S2CPacketGameTipNotification>& packet)
{
	ChatComponentNotification* chatNotification = dynamic_cast<ChatComponentNotification*>(packet->m_chatComponent.get());
	if (chatNotification == NULL)
		return;

	String lanType = LanguageManager::Instance()->getCurrentLanguage();
	chatNotification->msgContext = LanguageManager::Instance()->getString(chatNotification->msgContext);
	String msg = MultiLanTipSetting::getMessage(lanType, packet->m_tipType, chatNotification->msgContext.c_str()).c_str();

	if (msg.length() > 0)
	{
		switch (chatNotification->type)
		{
		case ChatComponentNotification::Type::NATURE:
			ChatMessageReceiveEvent::emit(msg, false);
			break;
		case ChatComponentNotification::Type::TIMING_TOP:
			TopSystemNotificationEvent::emit(msg, chatNotification->keepTime);
			break;
		case ChatComponentNotification::Type::TIMING_BOTTOM:
			BottomSystemNotificationEvent::emit(msg, chatNotification->keepTime);
			break;
		case ChatComponentNotification::Type::TIMING_CENTER:
			CenterSystemNotificationEvent::emit(msg, chatNotification->keepTime);
			break;
		case ChatComponentNotification::Type::TEAM_RESOURCES_NOTIFICATION:
			TeamResourcesShowNotificationEvent::emit(msg, chatNotification->keepTime);
			break;
		case ChatComponentNotification::Type::TIMING_RIGHT:
			BirdAddMessageEvent::emit(msg);
			break;
		}
	}
}

void S2CChatPacketHandles::handlePacket(std::shared_ptr<S2CPacketKillMsg>& packet)
{
	ReceiveKillMsgEvent::emit(packet->data);
}

void S2CChatPacketHandles::handlePacket(std::shared_ptr<S2CPacketDefaultNotification>& packet)
{
	ReceiveNotificationEvent::emit(0, packet->content);
}