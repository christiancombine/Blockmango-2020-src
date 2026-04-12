#include "C2SInitPacketHandles.h"
#include "Network/ClientPeer.h"
#include "Server.h"
#include "Util/LanguageKey.h"
#include "Network/ServerNetwork.h"
#include "Util/sha1.h"
#include "cpr/cpr.h"
#include "Object/Root.h"
#include "Network/HttpRequest.h"
#include "Inventory/CraftingManager.h"
#include "Script/GameServerEvents.h"
#include "Blockman/Entity/EntityTracker.h"
#include "GameVersionDefine.h"
#include "Network/protocol/S2CPackets5.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

void C2SInitPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketLogin>& packet)
{
	if (!clientPeer) {
		LordLogError("C2SPacketLogin clientPeer is nullptr, platform uid " U64FMT ", name %s",
			packet->m_platformUserId, packet->m_playerName.c_str());
		return;
	}
	clientPeer->setRequestIdFromClient(packet->m_dispatchRequestId);
	if (packet->m_isReconnect)
	{
		if (ServerNetwork::Instance()->checkReconnect(packet->m_platformUserId, clientPeer->getRakssid()))
		{
			LordLogInfo("C2SPacketLogin checkReconnect ---> reconnect success !!! platform uid " U64FMT ", name %s",
				packet->m_platformUserId, packet->m_playerName.c_str());
		}
	}

	int curVersion = EngineVersionSetting::getEngineVersion();
	bool bIsSameVersion = packet->m_engineVersion == curVersion;
	if (!bIsSameVersion)
	{
		auto resultPacket = LORD::make_shared<S2CPacketCheckCSVersionResult>();
		resultPacket->m_success = false;
		resultPacket->m_serverVersion = curVersion;
		Server::Instance()->getNetwork()->sendPacket(resultPacket, clientPeer->getRakssid());
		return;
	}

	// handle the situation that receive login packet twice
	// don't know why receive login packet twice yet
	if (clientPeer->getEntityPlayer() && clientPeer->hasLogon() && clientPeer->getPlatformUserId() == packet->m_platformUserId) {
		LordLogError("C2SPacketLogin , recv same login packet twice, session id " U64FMT ", platform uid " U64FMT ", name %s",
			 clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());
		return;
	}

	clientPeer->setLanguage(packet->m_language);
	clientPeer->setShowName(packet->m_playerName);
	clientPeer->setPlatformUserId(packet->m_platformUserId);

#if LORD_PLATFORM == LORD_PLATFORM_LINUX
	String rawString = Server::WEB_HTTP_SECRET + StringUtil::ToString((i64)packet->m_platformUserId) + StringUtil::ToString((i64)packet->m_gameTimestamp).c_str();
	SHA1 sha1;
	sha1.update(rawString.c_str());
	const std::string hash = sha1.final();
	LordLogInfo("m_gameToken____[%s] ====str____[%s]", packet->m_gameToken.c_str(), hash.c_str());

	if (packet->m_gameToken != (String)hash.c_str())
	{
		ServerNetwork::Instance()->getSender()->sendLoginResult(clientPeer->getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrTokenWrong);
		LordLogError("C2SPacketLogin token wrong %s!=%s, session id " U64FMT ", platform uid " U64FMT ", name %s",
			packet->m_gameToken.c_str(), hash.c_str(), clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());

		return;
	}
#endif

	if (Server::Instance()->askedToQuit()) {
		ServerNetwork::Instance()->getSender()->sendLoginResult(clientPeer->getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrServerQuitting);
		LordLogError("C2SPacketLogin , game server asked to quit, session id " U64FMT ", platform uid " U64FMT ", name %s",
			clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());

		return;
	}

	if (Server::Instance()->getEnableRoom())
	{
		bool recvedLatestAttr = false;

		UserAttrInfo attrInfo;
		bool attrExisted = Server::Instance()->getRoomManager()->getUserAttrInfo(packet->m_platformUserId, attrInfo);
		if (attrExisted)
		{
			if (attrInfo.isTest // is test player, no need check dispatch request id
				|| (attrInfo.requestId == std::string(packet->m_dispatchRequestId.c_str()) && !attrInfo.beKicked) )
			{
				recvedLatestAttr = true;
			}
		}
		
		if (!recvedLatestAttr)
		{
			if (ServerNetwork::Instance()->hasBeenDeferredLogin(packet->m_platformUserId)) {
				ServerNetwork::Instance()->getSender()->sendLoginResult(clientPeer->getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrWaitRoleAttrFail);
				LordLogError("C2SPacketLogin, latest user attr from room not recved, session id " U64FMT ", platform uid " U64FMT 
					", name %s, packet->m_dispatchRequestId=%s, attrInfo.requestId=%s",
					clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str(), 
					packet->m_dispatchRequestId.c_str(), attrExisted ? attrInfo.requestId.c_str() : "attrNotExisted");

				return;
			} else {
				ServerNetwork::Instance()->deferLoginProcess(packet->m_platformUserId, packet);

				LordLogWarning("C2SPacketLogin, recv login packet but can not find latest user attr,deferLoginProcess, session id "
					U64FMT ", platform uid " U64FMT ", name %s, packet->m_dispatchRequestId=%s, attrInfo.requestId=%s",
					clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str(),
					packet->m_dispatchRequestId.c_str(), attrExisted ? attrInfo.requestId.c_str() : "attrNotExisted");

				return;
			}
		}
	}

	{
		LORD::String playerIp = Server::Instance()->getNetwork()->getPlayerIp(clientPeer->getRakssid());
		LordLogInfo("C2SPacketLogin token correct, session id:" U64FMT ", platformUid:" U64FMT ", name:%s, clientIp:%s, requestId:%s",
			clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str(), playerIp.c_str(),
			packet->m_dispatchRequestId.c_str());

		auto pworld = Server::Instance()->getWorld();
		static size_t playerCount = 0;
		Vector3i spawnPos = Server::Instance()->getInitPos();
		Vector3iProxy pos;
		if (!SCRIPT_EVENT::PlayerUseInitSpawnPositionEvent::invoke(clientPeer->getPlatformUserId(), &pos))
		{
			spawnPos = Vector3i(pos.x, pos.y, pos.z);
		}

		EntityPlayerMP *pPlayer = LordNew EntityPlayerMP(pworld, packet->m_playerName, spawnPos);
		pPlayer->setRaknetID(clientPeer->getRakssid());
		UserAttrInfo attrInfo;
		attrInfo.sex = 1; // default, 1:boy, 2:girl
		attrInfo.defaultIdle = 0;

		bool attrExisted = false;
		if (Server::Instance()->getEnableRoom())
		{
			attrExisted = Server::Instance()->getRoomManager()->getUserAttrInfo(packet->m_platformUserId, attrInfo);
			if (attrExisted)
			{
				pPlayer->m_sex = attrInfo.sex;
				pPlayer->m_vip = attrInfo.vip;
				pPlayer->m_faceID = attrInfo.faceId;
				pPlayer->m_hairID = attrInfo.hairId;
				pPlayer->m_topsID = attrInfo.topsId;
				pPlayer->m_pantsID = attrInfo.pantsId;
				pPlayer->m_shoesID = attrInfo.shoesId;
				pPlayer->m_glassesId = attrInfo.glassesId;
				pPlayer->m_scarfId = attrInfo.scarfId;
				pPlayer->m_wingId = attrInfo.wingId;
				pPlayer->m_hatId = attrInfo.hatId;
				pPlayer->m_decorate_hatId = attrInfo.decoratehatId;
				pPlayer->m_armId = attrInfo.armId;
				pPlayer->m_tailId = attrInfo.tailId;
				pPlayer->m_extra_wingId = attrInfo.extrawingId;
				pPlayer->m_footHaloId = attrInfo.footHaloId;
				pPlayer->m_backEffectId = attrInfo.backEffectId;
				pPlayer->m_crownId = attrInfo.crownId;
				pPlayer->m_skinColor = attrInfo.skinColor;
				pPlayer->m_defaultIdle = attrInfo.defaultIdle;
				pPlayer->m_headEffectId = attrInfo.headEffectId;
				pPlayer->m_bagId = attrInfo.bagId;
				pPlayer->m_suitsId = attrInfo.suitsId;
				pPlayer->m_actionLockedInfoMap.clear();
				for (int i = 0; i < attrInfo.unlockedSelectableActions.size(); ++i)
				{
					pPlayer->m_actionLockedInfoMap[attrInfo.unlockedSelectableActions[i]] = false;
				}
			}
		}
		auto sender = Server::Instance()->getNetwork()->getSender();

		Vector3i adjustPos = Vector3i((int)pPlayer->position.x, (int)pPlayer->position.y, (int)pPlayer->position.z);
		sender->sendGameInfo(clientPeer->getRakssid(), pPlayer->entityId, adjustPos,
			CraftingManager::Instance()->getRecipeNameList(), pworld->getWorldInfo().isTimeStopped(), pPlayer->m_sex, Server::Instance()->getGameType(), pPlayer->m_defaultIdle, attrInfo.unlockedSelectableActions, pPlayer->m_vip);

		if (attrExisted) {
			sender->sendUserAttr(clientPeer->getRakssid(), pPlayer->entityId, attrInfo, pPlayer);

			LordLogInfo("C2SPacketLogin, send user attr info, session id " U64FMT ", platform uid " U64FMT ", name %s,"
				"entityId[%d],faceId[%d],hairId[%d],topsId[%d],pantsId[%d],shoesId[%d],glassesId[%d],"
				"scarfId[%d], wingId[%d], hatId[%d], tailId[%d], decoratehatId[%d],armId[%d],extrawingId[%d],footHaloId[%d],skinColor r[%f] , skinColor g[%f] , skinColor b[%f] , skinColor a[%f]",
				clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str(), pPlayer->entityId, attrInfo.faceId, attrInfo.hairId, attrInfo.topsId, attrInfo.pantsId, attrInfo.shoesId, attrInfo.glassesId, attrInfo.scarfId, attrInfo.wingId, attrInfo.hatId, attrInfo.tailId, attrInfo.decoratehatId, attrInfo.armId, attrInfo.extrawingId, attrInfo.footHaloId, attrInfo.skinColor.r, attrInfo.skinColor.g, attrInfo.skinColor.b, attrInfo.skinColor.a);
		}
		else {
			LordLogError("C2SPacketLogin, get user attr info fail and did not send user attr, session id " U64FMT ", platform uid " U64FMT ", name %s",
				clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str());
		}

		sender->sendSyncWorldTime(clientPeer->getRakssid(), pworld->getWorldTime());
		if (pworld->getWorldInfo().isRaining())
		{
			sender->sendSyncWorldWeather(clientPeer->getRakssid(), "rain", pworld->getWorldInfo().getRainTime());
		}
		if (pworld->getWorldInfo().isThundering())
		{
			sender->sendSyncWorldWeather(clientPeer->getRakssid(), "thunder", pworld->getWorldInfo().getThunderTime());
		}
		//sender->sendUpdateBlocks(clientPeer->getRakssid());
		sender->sendAllSetSignTexts(clientPeer->getRakssid());
		sender->sendUpdateShop(clientPeer->getRakssid());

		Server::Instance()->getNetwork()->kickOldLogin(packet->m_platformUserId, clientPeer->getRakssid());

		clientPeer->onSpawn(packet->m_playerName, pPlayer);
		clientPeer->setPlatformUserId(packet->m_platformUserId);
		LordLogInfo("C2SPacketLogin begin lua event, session id " U64FMT ", platform uid " U64FMT ", name %s, entityId %d",
			clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str(), pPlayer->entityId);
		clientPeer->login(packet->m_clientInfo, packet->m_enableIndie, packet->m_gameTimeToday);
		LordLogInfo("C2SPacketLogin end lua event, session id " U64FMT ", platform uid " U64FMT ", name %s, entityId %d",
			clientPeer->getRakssid(), packet->m_platformUserId, packet->m_playerName.c_str(), pPlayer->entityId);
		pworld->spawnEntityInWorld(pPlayer);
		SCRIPT_EVENT::PlayerFirstSpawnEvent::invoke(clientPeer.get());
		EntityTracker::Instance()->forceTeleport(pPlayer->entityId);
		pPlayer->forceTeleportStart();

	}
}

void C2SInitPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketRebirth>& packet)
{
	LordAssert(clientPeer != NULL);
	clientPeer->rebirth();
}

void C2SInitPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketUpdateUserMoney>& packet)
{
	if (!clientPeer)
		return;
	clientPeer->loadUserMoney();
}
