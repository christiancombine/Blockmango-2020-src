#include "ServerNetwork.h"
#include "ClientPeer.h"
#include "Common.h"
#include "Network/CommonDataDispatch.h"
#include "Core.h"
#include "Setting/MultiLanTipSetting.h"
#include "Network/protocol/AutoRegisterPacketS2C.h"
#include "Entity/TeleportPositionConverter.h"
#include "Chat/ChatComponent.h"
#include "Object/Root.h"
#include "Util/PlayerPrefs.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

using namespace std::placeholders;
ServerNetwork::ServerNetwork(const char *ip, short port)
{
	AutoRegisterS2C s2c;
	registerPacket100(s2c);
	registerPacket200(s2c);
	registerPacket300(s2c);
	registerPacket400(s2c);

	LordNew(CommonDataDispatch);
	LordNew(GameCommon);

	m_networkCore = LORD::make_shared<ServerNetworkCore>(ip, port);
	m_networkRecver = LORD::make_shared<ServerNetworkRecver>();
	m_packetSender = LORD::make_shared<ServerPacketSender>();
	m_commonDataSender = LORD::make_shared<ServerCommonDataSender>();

	std::shared_ptr<CommonPacketSender> pCommon = std::dynamic_pointer_cast<CommonPacketSender>(this->m_packetSender);
	GameCommon::Instance()->setCommonPacketSender(pCommon);

	m_subscriptionGuard.add(BLOCKMAN::ServerNetworkConnSuccEvent::subscribe(std::bind(&ServerNetwork::onServerNetworkConnSucc, this, _1, _2)));
	m_subscriptionGuard.add(BLOCKMAN::ServerNetworkDisconnectEvent::subscribe(std::bind(&ServerNetwork::onServerNetworkDisconnect, this, _1)));
	m_subscriptionGuard.add(BLOCKMAN::ServerNetworkTimeoutEvent::subscribe(std::bind(&ServerNetwork::onServerNetworkTimeout, this, _1)));
}

ServerNetwork::~ServerNetwork(void)
{
	m_subscriptionGuard.unsubscribeAll();
	GameCommon* pGameComm = GameCommon::Instance();
	LordSafeDelete(pGameComm);
	CommonDataDispatch* pDispatch = CommonDataDispatch::Instance();
	LordSafeDelete(pDispatch);
}

bool ServerNetwork::onServerNetworkConnSucc(const ui64 rakssid, const LORD::String& clientIp)
{
	auto player = ClientPeer::createPlayer(rakssid, clientIp);
	m_playerMap[rakssid] = player;
	if (PlayerPrefs::Instance()->getBoolPrefs("EnableReconnectNetwork"))
		m_packetRecordMap[rakssid] = LORD::make_shared<PacketRecords>();
	return true;
}

bool ServerNetwork::onServerNetworkDisconnect(const ui64 rakssid) {
	auto delPlayer = m_playerMap.find(rakssid);
	if (delPlayer != m_playerMap.end())
	{
		LordLogInfo("handleConnectEvent RaknetInstance::emDisconnect with session id=" U64FMT, rakssid);
		onDisconnect(delPlayer->second);
	}
	else
	{
		LordLogError("deleteSession but can not find player with session id=" U64FMT, rakssid);
	}
	return true;
}

bool ServerNetwork::onServerNetworkTimeout(const ui64 rakssid) {
	auto timeoutPlayer = m_playerMap.find(rakssid);
	if (timeoutPlayer != m_playerMap.end())
	{
		LordLogInfo("handleConnectEvent RaknetInstance::emConnectTimeout with session id=" U64FMT, rakssid);
		onConnectTimeout(timeoutPlayer->second);
	}
	return true;
}

String ServerNetwork::getPlayerIp(const ui64 rakssId)
{
	auto iter = m_playerMap.find(rakssId);
	if (iter != m_playerMap.end())
	{
		return iter->second->getClientIp();
	}
	else
	{
		return "";
	}
}

void ServerNetwork::host(ui32 maxSession)
{
	m_networkCore->getRaknetInstance()->recv([this](std::shared_ptr<DataPacket> packet) {m_networkRecver->recvPacket(packet); });
	m_networkCore->getRaknetInstance()->setHeartbeatCallback([]() { Server::Instance()->setRaknetState(); });
	
	m_networkCore->host(maxSession);
}

void ServerNetwork::sendPacket(std::shared_ptr<DataPacket> packet, ui64 rakssid)
{
	recordPacket(packet, rakssid);
	auto record = m_reconnectRecordMap.find(rakssid);
	if (record != m_reconnectRecordMap.end() && record->second->isReconnectSuccess)
		record->second->historyDataPackets.push_back(packet);
	else
		m_networkCore->getRaknetInstance()->send(packet, rakssid);
}

void ServerNetwork::flush()
{
	m_networkCore->getRaknetInstance()->flush();
}

void ServerNetwork::deferLoginProcess(ui64 platformUserId, std::shared_ptr<DataPacket> packet) {
	m_networkRecver->deferLoginProcess(platformUserId, packet);
}

bool ServerNetwork::hasBeenDeferredLogin(ui64 platformUserId) {
	return m_networkRecver->hasBeenDeferredLogin(platformUserId);
}

std::shared_ptr<ClientPeer> ServerNetwork::findPlayerByRakssid(ui64 ssid)
{
	std::shared_ptr<ClientPeer> pret;
	auto it = m_playerMap.find(ssid);
	if (it != m_playerMap.end())
	{
		return it->second;
	}
	return pret;
}

std::shared_ptr<ClientPeer> ServerNetwork::findPlayerByPlatformUserId(ui64 userId)
{
	for (auto player : m_playerMap)
	{
		if (player.second->getPlatformUserId() == userId)
		{
			return player.second;
		}
	}
	return nullptr;
}

std::set<ui64> ServerNetwork::getPlatformUserIds()
{
	std::set<ui64> platformUserIds;
	for (auto player : m_playerMap)
	{
		platformUserIds.insert(player.second->getPlatformUserId());
	}
	return platformUserIds;
}

int ServerNetwork::getPlayerNumber()
{
	return m_playerMap.size(); 
}

void ServerNetwork::broadCastEntityPacket(std::shared_ptr<DataPacket> packet, ui64 runtimeId)
{
	for (auto player : m_playerMap)
	{
		if (player.second->hasLogon() && !(player.second->getEntityRuntimeID() == runtimeId))
		{
			sendPacket(packet, player.second->getRakssid());
		}
	}
}

void ServerNetwork::broadCastEntityPacket(std::shared_ptr<DataPacket> packet)
{
	for (auto player : m_playerMap)
	{
		if (player.second->hasLogon())
		{
			sendPacket(packet, player.second->getRakssid());
		}
	}
}

template<typename RaknetIdList>
void ServerNetwork::multicastEntityPacket(std::shared_ptr<DataPacket> packet, const RaknetIdList & targets)
{
	for (const auto& targetId : targets)
	{
		auto iter = m_playerMap.find(targetId);
		if (iter == m_playerMap.end())
		{
			LordLogWarning("raknet id %llu is not in m_playerMap", targetId);
			continue;
		}
		if (iter->second->hasLogon())
		{
			m_networkCore->getRaknetInstance()->send(packet, targetId);
		}
	}
}

template void ServerNetwork::multicastEntityPacket(std::shared_ptr<DataPacket> packet, const LORD::vector<ui64>::type & targets);

void ServerNetwork::foreachPlayer(std::function<void(std::shared_ptr<ClientPeer>)> callBack)
{
	for (auto player : m_playerMap)
	{
		callBack(player.second);
	}
}

void ServerNetwork::onDisconnect(std::shared_ptr<ClientPeer>& clientPeer)
{
	TeleportPositionConverter::ClearServerSideAnchors(clientPeer->getRakssid());
	m_networkRecver->clearDeferLogin(clientPeer->getPlatformUserId());
	auto rakssid = clientPeer->getRakssid();
	clientPeer->logout();
	clearPlayerCache(rakssid);
}

void ServerNetwork::onConnectTimeout(std::shared_ptr<ClientPeer>& clientPeer)
{
	if (!PlayerPrefs::Instance()->getBoolPrefs("EnableReconnectNetwork"))
	{
		onDisconnect(clientPeer);
		return;
	}

	std::shared_ptr<ReconnectRecord> saveRecord = NULL;
	for (auto record : m_reconnectRecordMap)
	{
		if (record.second->reconnectUserId == clientPeer->getPlatformUserId())
		{
			saveRecord = record.second;
			saveRecord->isReconnectSuccess = false;
			m_reconnectRecordMap.erase(record.first);
			break;
		}
	}
	if (saveRecord == NULL)
		saveRecord = LORD::make_shared<ReconnectRecord>();;
	saveRecord->reconnectUserId = clientPeer->getPlatformUserId();
	if (saveRecord->startReconnectTime == 0)
		saveRecord->startReconnectTime = LORD::Root::Instance()->getCurrentTime();
	m_reconnectRecordMap[clientPeer->getRakssid()] = saveRecord;
}

void ServerNetwork::clearPlayerCache(ui64 rakssid)
{
	if (m_reconnectRecordMap.find(rakssid) != m_reconnectRecordMap.end())
		m_reconnectRecordMap.erase(rakssid);
	if (m_packetRecordMap.find(rakssid) != m_packetRecordMap.end())
		m_packetRecordMap.erase(rakssid);
	m_clientPingMap.erase(rakssid);
	m_playerMap.erase(rakssid);
}

void ServerNetwork::recordPacket(std::shared_ptr<DataPacket> packet, ui64 rakssid)
{
	auto records = m_packetRecordMap.find(rakssid);
	if (records != m_packetRecordMap.end())
	{
		PacketRecord record;
		record.sendTime = LORD::Root::Instance()->getCurrentTime();
		record.packet = packet;
		records->second->records.push_back(record);
	}
}

void ServerNetwork::closeSession(std::shared_ptr<ClientPeer>& clientPeer)
{
	m_networkCore->closeSession(clientPeer->getRakssid());
	onDisconnect(clientPeer);
}

void ServerNetwork::tick()
{
	tickReconnect();
	tickCheckPacketRecord();
	m_networkRecver->recvTick();
}

void ServerNetwork::tickReconnect()
{
	ui32 currentTime = LORD::Root::Instance()->getCurrentTime();
	int maxReconnectTime = 60000;
	for (auto record = m_reconnectRecordMap.begin(); record != m_reconnectRecordMap.end(); )
	{
		if (record->second == NULL)
		{
			record = m_reconnectRecordMap.erase(record);
			continue;
		}

		if (record->second->isReconnectSuccess)
		{
			// Resend historical data packets after successful reconnection
			int count = 0;
			while (count < 100)
			{
				if (record->second->historyDataPackets.empty())
					break;
				count++;
				auto packet = record->second->historyDataPackets.begin();
				m_networkCore->getRaknetInstance()->send(*packet, record->first);
				record->second->historyDataPackets.pop_front();;
			}

			if (record->second->historyDataPackets.empty())
			{
				record = m_reconnectRecordMap.erase(record);
			}
			else
			{
				record++;
			}
			continue;
		}

		if (currentTime - record->second->startReconnectTime >= maxReconnectTime)
		{
			// kick out the server in more than 60 seconds
			auto clientPeer = findPlayerByRakssid(record->first);
			record = m_reconnectRecordMap.erase(record);
			if (clientPeer) 
			{
				LordLogInfo("[RECONNECT_FAILED]ServerNetwork --> player reconnect failed(60s), userId:" U64FMT " rakssid:" U64FMT,
					clientPeer->getPlatformUserId(), clientPeer->getRakssid());
				closeSession(clientPeer);
			}
		}
		else
		{
			record++;
		}
	}
}

void ServerNetwork::tickCheckPacketRecord()
{
	ui32 currentTime = LORD::Root::Instance()->getCurrentTime();
	for (auto packetRecord : m_packetRecordMap)
	{
		while (true)
		{
			auto reconnectRecord = m_reconnectRecordMap.find(packetRecord.first);
			if (reconnectRecord != m_reconnectRecordMap.end())
				break;
			if (packetRecord.second->records.empty())
				break;
			auto record = packetRecord.second->records.begin();
			if (currentTime - record->sendTime < 10000)
				break;
			packetRecord.second->records.pop_front();
		}
	}
}

bool ServerNetwork::checkReconnect(ui64 platformUserId, ui64 rakssid)
{
	for (auto record = m_reconnectRecordMap.begin(); record != m_reconnectRecordMap.end(); record++)
	{
		if (record->second->reconnectUserId != platformUserId)
			continue;
		record->second->isReconnectSuccess = true;
		if (record->first != rakssid) // Use different ip to reconnect
		{
			m_reconnectRecordMap[rakssid] = record->second;
			auto packetRecords = m_packetRecordMap.find(record->first);
			if (packetRecords != m_packetRecordMap.end())
			{
				for (auto packetRecord : packetRecords->second->records)
				{
					record->second->historyDataPackets.push_back(packetRecord.packet);
				}
			}
		}
	}
	auto oldClientPeer = findPlayerByPlatformUserId(platformUserId);
	if (!oldClientPeer)
		return false;
	auto newClientPeer = findPlayerByRakssid(rakssid);
	if (newClientPeer)
	{
		newClientPeer->reconnect(oldClientPeer);
	}
	m_reconnectRecordMap.erase(oldClientPeer->getRakssid());
	return true;
}

bool ServerNetwork::kickOldLogin(ui64 platformUserId, ui64 rakssid)
{
	std::vector<std::shared_ptr<ClientPeer>> clientPeers;
	for (auto clientPeer : m_playerMap)
	{
		if (clientPeer.second->getPlatformUserId() == platformUserId 
			&& clientPeer.second->getRakssid() != rakssid )
		{
			clientPeers.push_back(clientPeer.second);
		}
	}
	for (auto clientPeer : clientPeers)
	{
		LordLogWarning("kickOldLogin of same platform uid " U64FMT ", ServerNetwork clean up, raknet id " U64FMT ", name:%s", 
			platformUserId, clientPeer->getRakssid(), clientPeer->getName().c_str());
		ServerNetwork::Instance()->getSender()->sendLoginResult(clientPeer->getRakssid(), 
			(int32_t)NETWORK_DEFINE::PacketLoginResult::emErrBeKicked);
		m_kickingOut = true;
		closeSession(clientPeer);
		m_kickingOut = false;
	}
	return clientPeers.size() > 0;
}

void ServerNetwork::closeAllSession()
{
	std::vector<std::shared_ptr<ClientPeer>> clientArr;
	for (auto i_c : m_playerMap) {
		if (i_c.second->hasLogon()) {
			clientArr.push_back(i_c.second);
		}
	}

	for (auto c : clientArr) {
		LordLogWarning("closeAllSession because server quitting, close session for uid " U64FMT ", raknet id " U64FMT ", name:%s", 
			c->getPlatformUserId(), c->getRakssid(), c->getName().c_str());

		closeSession(c);
	}
}

void ServerNetwork::broadCastMsg(ui64 targetId/*ssid*/, ui8 type, i32 time, const char *msg)
{
	ChatComponentNotification::Type chatType = static_cast<ChatComponentNotification::Type>(type);
	auto notify = LORD::make_shared<ChatComponentNotification>(String(msg), time, chatType);

	if (targetId == 0)
	{
		m_packetSender->sendNotification(notify);
	}
	else
	{
		m_packetSender->sendNotification(targetId, notify);
	}
}

void ServerNetwork::broadCastMsgByType(ui64 targetId, ui8 type, i32 time, int msgType, const char* args)
{
	std::shared_ptr<ChatComponentNotification> notify = nullptr;

	if (targetId == 0)
	{
		for (auto player : m_playerMap)
		{
			if (player.second->hasLogon())
			{
				String lan = player.second->getLanguage();
				notify = getBroadCastMsgNotify(type, time, lan, msgType, args);
				ServerNetwork::Instance()->getSender()->sendNotification(player.second->getRakssid(), notify);
			}
		}
	}
	else
	{
		auto peer = findPlayerByRakssid(targetId);
		if (peer)
		{
			String lan = peer->getLanguage();
			notify = getBroadCastMsgNotify(type, time, lan, msgType, args);
			ServerNetwork::Instance()->getSender()->sendNotification(targetId, notify);
		}
	}
}

void ServerNetwork::sendGameOver(ui64 rakssid, int msgType, int code)
{
	auto player = findPlayerByRakssid(rakssid);
	if (player)
	{
		LordLogInfo("sendGameOver,platform uid " U64FMT ", name %s, rakssId " U64FMT,
			player->getPlatformUserId(), player->getName().c_str(), rakssid);
		String lan = player->getLanguage();
		std::string msg = MultiLanTipSetting::getMessage(lan, msgType, "");
		ServerNetwork::Instance()->getSender()->sendGameover(rakssid, msg.c_str(), code);
	}
	else
	{
		LordLogError("sendGameOver fail, can't find player, rakssId " U64FMT, rakssid);
	}
}

void ServerNetwork::sendGameOverByPlatformUserId(ui64 platformUserId, const String& overMessage, int code)
{
	auto player = findPlayerByPlatformUserId(platformUserId);
	if (player)
	{
		LordLogInfo("sendGameOverByPlatformUserId,platform uid " U64FMT ", name %s, rakssId " U64FMT ", code=%d",
			player->getPlatformUserId(), player->getName().c_str(), player->getRakssid(), code);

		ServerNetwork::Instance()->getSender()->sendGameover(player->getRakssid(), overMessage.c_str(), code);
	}
	else
	{
		LordLogError("sendGameOverByPlatformUserId fail, can't find player, platformUserId " U64FMT, platformUserId);
	}
}

std::shared_ptr<ChatComponentNotification> ServerNetwork::getBroadCastMsgNotify(ui8 type, i32 time, String& lanType, int msgType, const char* args)
{
	std::string msg = MultiLanTipSetting::getMessage(lanType, msgType, args);
	ChatComponentNotification::Type chatType = static_cast<ChatComponentNotification::Type>(type);
	return LORD::make_shared<ChatComponentNotification>(String(msg.c_str()), time, chatType);
}

void ServerNetwork::broadCastGameTipByType(ui64 targetId, ui8 type, i32 time, int msgType, const char * args)
{
	ChatComponentNotification::Type chatType = static_cast<ChatComponentNotification::Type>(type);
	ChatComponentNotification *notify = LordNew ChatComponentNotification(String(args), time, chatType);

	ServerNetwork::Instance()->getSender()->sendGameTipNotification(targetId, msgType, notify);
}

void ServerNetwork::broadCastServerQuitting(const char * msg)
{
	for (auto player : m_playerMap)
	{
		if (player.second->hasLogon())
		{
			m_packetSender->sendGameover(player.second->getRakssid(), msg, 11);
		}
	}
}

ServerNetwork::SpeedCheatType ServerNetwork::speedCheatCheck(
	const ui64 raknetId, ui32 clientRecvLastPingAckTime, ui32 clientSendPingTime, ui32 currServerTime)
{
	ClientPing clientPing;
	if (m_clientPingMap.find(raknetId) != m_clientPingMap.end()) {
		clientPing = m_clientPingMap[raknetId];
	}

	PingInfo firstPing = clientPing.firstPing;
	PingInfo secondPing = clientPing.secondPing;
	if (firstPing.clientSendPingTime == 0) {
		// first ping, ignore clientRecvLastPingAckTime
		firstPing.clientSendPingTime = clientSendPingTime;
		firstPing.serverSendPingTime = currServerTime;

		clientPing.firstPing = firstPing;
	}
	else if (secondPing.clientSendPingTime == 0) {
		// second ping
		firstPing.clientRecvPingAckTime = clientRecvLastPingAckTime;
		secondPing.clientSendPingTime = clientSendPingTime;
		secondPing.serverSendPingTime = currServerTime;

		clientPing.firstPing = firstPing;
		clientPing.secondPing = secondPing;
	}
	else if (secondPing.clientRecvPingAckTime == 0) {
		// after the second ping
		secondPing.clientRecvPingAckTime = clientRecvLastPingAckTime;
        /*
		LordLogInfo("ServerNetwork::speedCheatCheck, rakssId " U64FMT ", firstPing{clientSend:%u, serverSend:%u, clientRecvAck:%u},"
			"secondPing{clientSend:%u, serverSend:%u, clientRecvAck:%u},"
			"correct case: %u(t6-t1) > %u(t5-t2) > %u(t4-t3)", raknetId,
			firstPing.clientSendPingTime, firstPing.serverSendPingTime, firstPing.clientRecvPingAckTime,
			secondPing.clientSendPingTime, secondPing.serverSendPingTime, secondPing.clientRecvPingAckTime,
			(secondPing.clientRecvPingAckTime - firstPing.clientSendPingTime),
			(secondPing.serverSendPingTime - firstPing.serverSendPingTime),
			(secondPing.clientSendPingTime - firstPing.clientRecvPingAckTime));
        */

		ServerNetwork::PingInfo nextPing;
		nextPing.clientSendPingTime = clientSendPingTime;
		nextPing.serverSendPingTime = currServerTime;

		// check speed up
		if ((secondPing.serverSendPingTime - firstPing.serverSendPingTime + 2500) <
				(secondPing.clientSendPingTime - firstPing.clientRecvPingAckTime)) 
		{
			LordLogError("ServerNetwork::speedCheatCheck, speed up detected, rakssId " U64FMT 
				", firstPing{clientSend:%u, serverSend:%u, clientRecvAck:%u},"
				"secondPing{clientSend:%u, serverSend:%u, clientRecvAck:%u},"
				"correct case: %u(t6-t1) > %u(t5-t2) > %u(t4-t3), but (t5-t2) < (t4-t3)", raknetId,
				firstPing.clientSendPingTime, firstPing.serverSendPingTime, firstPing.clientRecvPingAckTime,
				secondPing.clientSendPingTime, secondPing.serverSendPingTime, secondPing.clientRecvPingAckTime,
				(secondPing.clientRecvPingAckTime - firstPing.clientSendPingTime),
				(secondPing.serverSendPingTime - firstPing.serverSendPingTime),
				(secondPing.clientSendPingTime - firstPing.clientRecvPingAckTime));

			return SpeedCheatType::CHEAT_SPEED_UP;
		}

		// check slow down
		if ((secondPing.clientRecvPingAckTime - firstPing.clientSendPingTime + 4000) <
				(secondPing.serverSendPingTime - firstPing.serverSendPingTime)) 
		{
			LordLogError("ServerNetwork::speedCheatCheck, slow down detected, rakssId " U64FMT 
				", firstPing{clientSend:%u, serverSend:%u, clientRecvAck:%u},"
				"secondPing{clientSend:%u, serverSend:%u, clientRecvAck:%u},"
				"correct case: %u(t6-t1) > %u(t5-t2) > %u(t4-t3), but (t6-t1) < (t5-t2)", raknetId,
				firstPing.clientSendPingTime, firstPing.serverSendPingTime, firstPing.clientRecvPingAckTime,
				secondPing.clientSendPingTime, secondPing.serverSendPingTime, secondPing.clientRecvPingAckTime,
				(secondPing.clientRecvPingAckTime - firstPing.clientSendPingTime),
				(secondPing.serverSendPingTime - firstPing.serverSendPingTime),
				(secondPing.clientSendPingTime - firstPing.clientRecvPingAckTime));

			return SpeedCheatType::CHEAT_SLOW_DOWN;
		}

		clientPing.firstPing = secondPing;
		clientPing.secondPing = nextPing;
	}

	m_clientPingMap[raknetId] = clientPing;

	return SpeedCheatType::CHEAT_SPEED_NONE;
}

String ServerNetwork::getSendTotalPacketInfo()
{
	auto raknetInstance = m_networkCore->getRaknetInstance();
	return raknetInstance->getSendTotalPacketInfo();
}
