#include "RoomManager.h"
#include "Script/GameServerEvents.h"
#include "Network/ServerNetwork.h"
#include "Enums/Enums.h"

#define KICK_USER_OUT_TIP_DUPLICATE_ENTER "system.message.kick.user.out.duplicate.enter"
#define KICK_USER_OUT_TIP_TIMEOUT_ENTER "system.message.kick.user.out.timeout.enter"
#define KICK_USER_OUT_TIP_MANUAL_KICK_OUT "system.message.kick.user.out.manual.kick.out"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

using namespace LORD;
RoomManager::RoomManager(const std::string& host, unsigned short port, const RoomGameConfig& rgConfig) {
	m_rgConfig = rgConfig;
	m_roomClient = LORD::make_shared<RoomClient>(host, port, this);
}

void RoomManager::tick(bool serverQuitting) {
	if (serverQuitting && !(m_roomClient->isServerQuitting())) {
		m_roomClient->setServerQuitting();
		m_roomClient->sendServerQuittingStatus(m_rgConfig.gameId, m_rgConfig.maxPlayers);
	}
	m_roomClient->tick();
}

void RoomManager::onConnected() {
	LordLogInfo("connect to room succ and begin to send connect packet, m_isFirstConnect:%s", m_isFirstConnect ? "true" : "false");
	m_roomClient->sendConnect(m_rgConfig.gameId, 1, "success", !m_isFirstConnect, m_rgConfig);
}

void RoomManager::reviseUserAttrInfoMap() {
	std::set<ui64> onlinePlatformUserIds = ServerNetwork::Instance()->getPlatformUserIds();
	ui64 currentTime = LORD::Time::Instance()->getMilliseconds() / 1000;
	std::set<ui64> stalePlatformUserIds;
	for (const auto& attrInfo : m_userAttrInfoMap) {
		if (onlinePlatformUserIds.find(attrInfo.first) != onlinePlatformUserIds.end()) {
			continue;
		}
		if (currentTime - attrInfo.second.updateTimestamp <= 30) {
			continue;
		}

		stalePlatformUserIds.insert(attrInfo.first);
	}

	for (auto id : stalePlatformUserIds) {
		m_userAttrInfoMap.erase(id);
	}
}

void RoomManager::onConnectReply() {
	LordLogInfo("recv room connect reply, m_isFirstConnect:%s", m_isFirstConnect ? "true" : "false");
	if (m_isFirstConnect) {
		m_isFirstConnect = false;
	}
	else {
		LordLogInfo("reconnect succ and recv connect reply, begin to resend game_status and sync_users packet");
		reviseUserAttrInfoMap();
		m_roomClient->sendSyncUsers(m_userAttrInfoMap, m_manorAttrInfoMap);
		m_roomClient->resendGameStatus(m_rgConfig.gameId, m_rgConfig.maxPlayers);
	}
}

void RoomManager::onUserAttr(const UserAttrInfo& attr)
{
	SCRIPT_EVENT::PlayerDynamicAttrEvent::invoke(attr, (ui64)attr.gamePartyParam.userId);

	m_userAttrInfoMap[attr.userId] = attr;
	if (attr.manorId > 0)
	{
		int32_t mapSize = m_manorAttrInfoMap.size();
		ManorAttrInfo manorInfo = ManorAttrInfo();
		manorInfo.location = attr.manorId;
		manorInfo.targetUserId = attr.targetUserId;
		m_manorAttrInfoMap[attr.targetUserId] = manorInfo;
		LordLogInfo("RoomManager::onUserAttr m_manorAttrInfoMap:  set before size [%d], set later size [%d]", mapSize, m_manorAttrInfoMap.size());
	}
}

void RoomManager::onUserManorRelease(int64_t userId)
{
	SCRIPT_EVENT::UserManorReleaseEvent::invoke(userId);
	setUserAttrInfoManorId(userId, 0);
	auto clientPeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
	if (clientPeer)
	{
		ServerNetwork::Instance()->closeSession(clientPeer);
	}
	
}

void RoomManager::onReceive(const BroadcastMessage & message)
{
	LordLogInfo("RoomManager::onReceive BroadcastMessage type = [%d]", message.type);
	switch ((BroadcastType) message.type)
	{
	case BroadcastType::RANCH_ORDER_HELP:
	{

		for (i64 targetId : message.targets)
		{
			auto clientPeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(targetId);
			if (clientPeer)
			{
				ServerNetwork::Instance()->getSender()->sendBroadcastMessage(clientPeer->getRakssid(), message.type, message.content);
			}
		}
	}
	break;
	case BroadcastType::RANCH_ORDER_FINISH:
	{
		for (i64 targetId : message.targets)
		{
			auto clientPeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(targetId);
			if (clientPeer)
			{
				SCRIPT_EVENT::RefreshPlayerRanchOrderEvent::invoke(clientPeer->getRakssid());
				ServerNetwork::Instance()->getSender()->sendBroadcastMessage(clientPeer->getRakssid(), message.type, message.content);
			}
		}
	}
		break;
	case BroadcastType::RANCH_RANK_ENTER_GAME:
		ServerNetwork::Instance()->getSender()->sendBroadcastMessage(0, message.type, message.content);
		break;
	case BroadcastType::FRIEND_INVITE:
	case BroadcastType::PRAISE:
		for (i64 targetId : message.targets)
		{
			auto clientPeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(targetId);
			if (clientPeer)
			{
				ServerNetwork::Instance()->getSender()->sendBroadcastMessage(clientPeer->getRakssid(), message.type, message.content);
			}
		}
		break;
	case BroadcastType::SKYBLOCK_FRIEND_UPDATE:
		for (i64 targetId : message.targets)
		{
			auto clientPeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(targetId);
			if (clientPeer)
			{
				SCRIPT_EVENT::SkyBlockFriendUpdateEvent::invoke((ui64)targetId, message.content);
			}
		}
		break;
	case BroadcastType::SKYBLOCK_PARTY_ALMOST_OVER:
	{
		for (i64 targetId : message.targets)
		{
			SCRIPT_EVENT::SkyBlockPartyAlmostOverEvent::invoke((ui64)targetId);
		}
		break;
	}
	case BroadcastType::SKYBLOCK_PARTY_OVER:
	{
		for (i64 targetId : message.targets)
		{
			SCRIPT_EVENT::SkyBlockPartyOverEvent::invoke((ui64)targetId);
		}
		break;
	}
	case BroadcastType::LIFTING_CHAMPION_INFO:
	{
		SCRIPT_EVENT::LiftingChampionInfoEvent::invoke(message.content);
		break;
	}
	default:
		ServerNetwork::Instance()->getSender()->sendBroadcastMessage(0, message.type, message.content);
		break;
	}
}

void RoomManager::onKickUserOut(int64_t userId, RoomKickUserOutType kickUserOutType)
{
	int gameOverCode = 0;
	String gameOverMsg;

	switch (kickUserOutType) {
	case RoomKickUserOutType::RKUOT_DUPLICATE_ENTER:
		gameOverCode = 100;
		gameOverMsg = KICK_USER_OUT_TIP_DUPLICATE_ENTER;
		break;
	case RoomKickUserOutType::RKUOT_TIMEOUT_ENTER:
		gameOverCode = 101;
		gameOverMsg = KICK_USER_OUT_TIP_TIMEOUT_ENTER;
		break;
	case RoomKickUserOutType::RKUOT_MANUAL_KICK_OUT:
		gameOverCode = 102;
		gameOverMsg = KICK_USER_OUT_TIP_MANUAL_KICK_OUT;
		break;
	}
	if (gameOverCode != 0) {
		setUserAttrInfoBeKicked(userId, true);
		ServerNetwork::Instance()->sendGameOverByPlatformUserId(userId, gameOverMsg, gameOverCode);
	}
	else {
		LordLogError("RoomManager::onKickUserOut unknown kickUserOutType = [%d]", (int)kickUserOutType);
	}
}

void RoomManager::setUserAttrInfoBeKicked(int64_t userId, bool beKicked)
{
	auto iter = m_userAttrInfoMap.find(userId);
	if (iter != m_userAttrInfoMap.end())
	{
		iter->second.beKicked = beKicked;
	}
}


bool RoomManager::UserAttrExisted(int64_t userId)
{
	if (m_userAttrInfoMap.find(userId) == m_userAttrInfoMap.end()) {
		return false;
	}
	else {
		return true;
	}
}

bool RoomManager::getUserAttrInfo(int64_t userId, UserAttrInfo& attrInfo) {
	if (m_userAttrInfoMap.find(userId) == m_userAttrInfoMap.end()) {
		return false;
	}

	attrInfo = m_userAttrInfoMap[userId];

	return true;
}

void RoomManager::setUserAttrInfoManorId(int64_t userId, int32_t manorId)
{
	int32_t mapSize = m_manorAttrInfoMap.size();
	m_manorAttrInfoMap.erase(userId);
	for (auto info : m_userAttrInfoMap)
	{
		if (info.second.targetUserId == userId)
		{
			info.second.manorId = manorId;
		}
	}
	LordLogInfo("RoomManager::setUserAttrInfoManorId m_manorAttrInfoMap: Sell manor ,  targetUserId [%lld] ,  remove before size [%d], remove later size [%d]", userId, mapSize, m_manorAttrInfoMap.size());
}

void RoomManager::setUserAttrInfoUnlockedSelectableActions(int64_t userId, vector<int32_t>::type selectableActions)
{
	auto iter = m_userAttrInfoMap.find(userId);
	if (iter != m_userAttrInfoMap.end())
	{
		iter->second.unlockedSelectableActions = selectableActions;
	}
}

UserAttrInfo* RoomManager::getUserAttrInfo(ui64 userId)
{
	auto iter = m_userAttrInfoMap.find((int64_t)userId);
	if (iter == m_userAttrInfoMap.end())
	{
		return nullptr;
	}
	return &iter->second;
}

void RoomManager::sendUserOut(ui64 userId, std::string clientPeerRequestId)
{
	if (!clientPeerRequestId.empty()) {
		getRoomClient()->sendUserOut(userId, clientPeerRequestId);
	}
}
