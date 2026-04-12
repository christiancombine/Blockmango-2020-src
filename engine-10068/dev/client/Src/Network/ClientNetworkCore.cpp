#include "ClientNetworkCore.h"
#include "Util/ClientEvents.h"
#include "Util/ClientDataReport.h"
#include "Entity/TeleportPositionConverter.h"
#include "game.h"
#include "Object/Root.h"
#include "Util/PlayerPrefs.h"

ClientNetworkCore::ClientNetworkCore()
	:m_status(emDisconnect)
{
	m_rakinstance = LORD::make_shared<RaknetInstance>("", 0, std::chrono::milliseconds(0));
}

ClientNetworkCore::~ClientNetworkCore(void)
{
	tearDown();
}

void ClientNetworkCore::tearDown()
{
	if (m_destroyed)
	{
		return;
	}
	m_rakinstance->tearDown();
	m_destroyed = true;
}


void ClientNetworkCore::connect(const char *ip, ui16 port)
{
	strcpy(m_hostIP, ip);
	m_hostPort = port;

	LordLogDebug("begin to connect to game server [%s:%u]", ip, port);
	m_status = emConncting;
	m_rakinstance->connect(ip, port);
}


void ClientNetworkCore::handleConnectEvent(const RaknetInstance::ConnectEvent &connEvent)
{
	static bool isRetryConnectChangeIp = false;
	ui64 id = connEvent.rakssid;
	switch (connEvent.type)
	{
	case RaknetInstance::emConnectSuc:
	{
		LordLogInfo("_ClientNetwork_handleConnectEvent_emConnectSuc");
		m_rakssid = id;
		m_status = ClientNetworkCore::emConnected;
		m_rakinstance->enableRaknetPing();
		PlayerPrefs::Instance()->putIntPrefs("StartRetryConnectTime", 0);
		PlayerPrefs::Instance()->putIntPrefs("MaxRetryConnectTime", 10000);
		BLOCKMAN::NetworkConnectionSuccessfulEvent::emit();
		GameClient::CGame::Instance()->onProcessReport(BLOCKMAN::ClientDataReport::GAME_PROCESS_EVENT_CONNECT_SERVER);
		isRetryConnectChangeIp = false;
	}
	break;

	case RaknetInstance::emConnectFailed:
	{
		LordLogInfo("_ClientNetwork_handleConnectEvent_emConnectFailed");
		m_status = ClientNetworkCore::emConnectFailed;
		ui32 currentTime = LORD::Root::Instance()->getCurrentTime();
		int maxRetryConnectTime = PlayerPrefs::Instance()->getIntPrefs("MaxRetryConnectTime");
		if (maxRetryConnectTime == 0)
		{
			maxRetryConnectTime = 10000;
			PlayerPrefs::Instance()->putIntPrefs("MaxRetryConnectTime", maxRetryConnectTime);
		}
		int startRetryConnectTime = PlayerPrefs::Instance()->getIntPrefs("StartRetryConnectTime");
		if (startRetryConnectTime == 0)
		{
			startRetryConnectTime = currentTime;
			PlayerPrefs::Instance()->putIntPrefs("StartRetryConnectTime", startRetryConnectTime);
		}
		LordLogInfo("[ClientNetwork]MaxRetryConnectTime:%d StartRetryConnectTime:%d currentTime:%d", 
			maxRetryConnectTime, startRetryConnectTime, currentTime);
		if (currentTime - startRetryConnectTime < maxRetryConnectTime)
		{
			m_status = emRetryConnect;
			BLOCKMAN::NetworkRetryConnectionEvent::emit(false);
		}
		else
		{
			if (!isRetryConnectChangeIp && !GameClient::CGame::Instance()->enterGameWithTeam())
			{
				isRetryConnectChangeIp = true;
				BLOCKMAN::NetworkRetryConnectionEvent::emit(true);
				GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_RETRY_CONNECT, BLOCKMAN::ClientDataReport::GAME_RETRY_CONNECT_START);
			}
			else
			{
				isRetryConnectChangeIp = false;
				BLOCKMAN::NetworkConnectionFailureEvent::emit();
				GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_CONNECT_FAILS);
				GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_CONNECT_FAILS);
			}
		}
	}
	break;

	case RaknetInstance::emConnectKickOut:
	{
		LordLogInfo("_ClientNetwork_handleConnectEvent_emConnectKickOut");
		m_status = ClientNetworkCore::emDisconnect;	
		TeleportPositionConverter::ClearClientSideAnchors();
		BLOCKMAN::NetworkConnectionKickOutEvent::emit();
	}
	break;

	case RaknetInstance::emConnectTimeout:
	{
		LordLogInfo("_ClientNetwork_handleConnectEvent_emConnectTimeout");
		if (m_status != ClientNetworkCore::emLoginSuc) {
			GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_TIMEOUT);
			GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_TIMEOUT);
		}
		else {
			GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::GAME_DISCONNECT, BLOCKMAN::ClientDataReport::GAME_DISCONNECT_TIMEOUT);
		}

		if (PlayerPrefs::Instance()->getBoolPrefs("EnableReconnectNetwork"))
		{
			if (PlayerPrefs::Instance()->getIntPrefs("StartRetryConnectTime") == 0)
				PlayerPrefs::Instance()->putIntPrefs("StartRetryConnectTime", LORD::Root::Instance()->getCurrentTime());
			isRetryConnectChangeIp = true;
			PlayerPrefs::Instance()->putIntPrefs("MaxRetryConnectTime", 45000);
			m_status = emRetryConnect;
			BLOCKMAN::NetworkRetryConnectionEvent::emit(false);
		}
		else
		{
			TeleportPositionConverter::ClearClientSideAnchors();
			BLOCKMAN::NetworkConnectionTimeoutEvent::emit();
		}
	}
	break;
	case RaknetInstance::emDisconnect:
	{
		LordLogInfo("_ClientNetwork_handleConnectEvent_emDisconnect");
		m_status = ClientNetworkCore::emDisconnect;
		TeleportPositionConverter::ClearClientSideAnchors();
		BLOCKMAN::NetworkConnectionDisconnectEvent::emit();
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::GAME_DISCONNECT, BLOCKMAN::ClientDataReport::GAME_DISCONNECT_NORMAL);
	}
	break;
	default:
	{
		LordLogInfo("_ClientNetwork_handleConnectEvent_emDisconnect");
		m_status = ClientNetworkCore::emDisconnect;
		TeleportPositionConverter::ClearClientSideAnchors();
		BLOCKMAN::NetworkConnectionDisconnectEvent::emit();
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::GAME_DISCONNECT, BLOCKMAN::ClientDataReport::GAME_DISCONNECT_UNKNOWN);
	}
	break;
	}
}

int ClientNetworkCore::getRaknetPing() {
	return m_rakinstance->getRaknetPing();
}
