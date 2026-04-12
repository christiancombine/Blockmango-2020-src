#include "ClientPeer.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Blockman/Entity/EntityPlayerAI.h"
#include "World/World.h"
#include "Server.h"
#include "ServerPacketSender.h"
#include "ServerNetwork.h"
#include "HttpRequest.h"
#include "ManorRequest.h"
#include "Script/GameServerEvents.h"
#include "Network/RoomClient.h"
#include "Util/LanguageKey.h"
#include "LuaRegister/Template/LuaEngine.h"
#include "Blockman/Entity/EntityTracker.h"
#include "rapidjson/document.h"
#include "Setting/MultiLanTipSetting.h"
#include "BM_Container_def.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntityFlyingMount.h"
#include "Entity/EntityFloatingMount.h"
#include "Game/Manor.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "Blockman/World/BlockCropsManager.h"
#include "Network/protocol/S2CPackets1.h"
#include "Util/StringUtil.h"
#include "Util/PlayerPrefs.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Object/Root.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

using namespace rapidjson;

static const char* DIAMOND_BLUES = "diamonds";
static const char* DIAMOND_GOLDS = "gDiamonds";

ClientPeer::ClientPeer(ui64 rakssid, const LORD::String& clientIp)
	: m_rakSsid(rakssid)
	, m_clientIp(clientIp)
	, m_logon(false)
	, m_entityPlayer(NULL)
	, currentTicks(0)
	, keepAliveRandomID(0)
	, keepAliveTimeSent(0)
	, ticksOfLastKeepAlive(0)
	, chatSpamThresholdCount(0)
	, creativeItemCreationSpamThresholdTally(0)
	, lastPos(Vector3::ZERO)
	, hasMoved(true)
{
	respawnPos = Server::Instance()->getInitPos();
}

ClientPeer::~ClientPeer()
{

}

void ClientPeer::removePlayerEntity()
{
	if (NULL != m_entityPlayer)
	{
		auto pWorld = Server::Instance()->getWorld();
		pWorld->removePlayerEntityDangerously(m_entityPlayer);
		LordLogInfo("player %s has destroyed", m_name.c_str());

		auto entityPlayerAI = dynamic_cast<EntityPlayerAI*>(m_entityPlayer);
		if (entityPlayerAI)
		{
			entityPlayerAI->setClientPeer(nullptr);
		}
	}
}

int ClientPeer::getEntityRuntimeID()
{
	if (NULL != m_entityPlayer)
		return m_entityPlayer->entityId;
	else
		return 0;
}

void ClientPeer::setTeamInfo(int teamId, String teamName)
{
	setTeamId(teamId);
	setTeamName(teamName);
	if (m_entityPlayer)
	{
		m_entityPlayer->setTeamId(teamId);
		m_entityPlayer->setTeamName(teamName);
	}
}

bool ClientPeer::isVisitor()
{
	return isPioneer() ? (m_platformUserId & 15) == 1 : ((m_platformUserId >> 15) & 1) == 1;
}

void ClientPeer::doAttack(int targetEngityRuntimeId, float targetX, float targetY, float targetZ)
{
	LordLogInfo("handlePacket_AttackPacket from %s, targetId=%d, attacker watch mode:%s", 
		m_name.c_str(), targetEngityRuntimeId, m_entityPlayer->isWatchMode() ? "true" : "false");
	
	auto targetEntity  = Server::Instance()->getWorld()->getEntity(targetEngityRuntimeId);
	if (!targetEntity)
	{
		LordLogError("handlePacket AttackPacket from %s, target entity not found", m_name.c_str());
		return;
	}

	EntityPlayerMP* targetPlayer = dynamic_cast<EntityPlayerMP*>(targetEntity);
	if (targetPlayer)
	{
		if (targetPlayer->isOnVehicle())
			return;

		Vector3 clientTargetPos(targetX, targetY, targetZ);

		if (clientTargetPos.distanceTo(lastPos) > 18
			&& !targetPlayer->isDead && !m_entityPlayer->isDead
			&& !targetPlayer->isForceTeleport() && !m_entityPlayer->isForceTeleport())
		{
			int attackCheatTimes = m_entityPlayer->getIntProperty("AttackCheatTimes") + 1;
			m_entityPlayer->setIntProperty("AttackCheatTimes", attackCheatTimes);
			if (attackCheatTimes >= 3)
			{
				LordLogError("handlePacket_AttackPacket from %s, attack the player cheat detected", m_name.c_str());
				ServerNetwork::Instance()->getSender()->sendGameover(getRakssid(), "attack the player cheat detected", 12);
			}
			return;
		}
		m_entityPlayer->setIntProperty("AttackCheatTimes", 0);

		if (clientTargetPos.distanceTo(targetPlayer->position) > 3)
		{
			LordLogError("handlePacket_AttackPacket from %s, target entity pos changed", m_name.c_str());
			ServerNetwork::Instance()->getSender()->syncPlayerMovement(this->getRakssid(), targetEngityRuntimeId);
			return;
		}

		bool attacked = m_entityPlayer->attackTargetEntityWithCurrentItem(targetPlayer);
		if (!attacked)
		{
			LordLogInfo("handlePacket_AttackPacket from %s, attack fail", m_name.c_str());
		}
		return;
	}

	EntityFlyingMount* targetFlyingMount = dynamic_cast<EntityFlyingMount*>(targetEntity);
	if (targetFlyingMount)
	{
		targetPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(targetFlyingMount->getRiderId()));
		if (targetPlayer)
		{
			bool attacked = m_entityPlayer->attackTargetEntityWithCurrentItem(targetPlayer);
			if (!attacked)
			{
				LordLogInfo("handlePacket_AttackPacket from %s, attack fail", m_name.c_str());
			}
		}
		return;
	}

	EntityFloatingMount* targetFloatingMount = dynamic_cast<EntityFloatingMount*>(targetEntity);
	if (targetFloatingMount)
	{
		targetPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(targetFloatingMount->getRiderId()));
		if (targetPlayer)
		{
			bool attacked = m_entityPlayer->attackTargetEntityWithCurrentItem(targetPlayer);
			if (!attacked)
			{
				LordLogInfo("handlePacket_AttackPacket from %s, attack fail", m_name.c_str());
			}
		}
		return;
	}
}

void ClientPeer::sendLoginResult(bool suc, int teamId, String teamName, int curPlayer, int maxPlayer)
{
	if (suc)
	{
		LordLogInfo("ClientPeer::sendLoginResult succ and sendUserIn to room, name[%s] platform uid[" U64FMT "] entityID[%d] rankID[" U64FMT "] teamId[%d] teamName[%s]",
			m_name.c_str(), m_platformUserId, m_entityPlayer->entityId, getRakssid(), teamId, teamName.c_str());
		setTeamInfo(teamId, teamName);
		ServerNetwork::Instance()->getSender()->sendLoginResult(getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emSucc);
		UserAttrInfo attrInfo;
		if (Server::Instance()->getEnableRoom())
		{
			Server::Instance()->getRoomManager()->getUserAttrInfo(this->m_platformUserId, attrInfo);
			Server::Instance()->getRoomManager()->getRoomClient()->sendUserIn(this->m_platformUserId, attrInfo.requestId);
		}
		ServerNetwork::Instance()->getSender()->sendUserIn(0, getPlatformUserId(), getName(), teamId, teamName, curPlayer, maxPlayer, attrInfo);
		for (auto player : ServerNetwork::Instance()->getPlayers())
		{
			if (player.second->getRakssid() != this->getRakssid() && player.second->m_logon)
			{
				UserAttrInfo info;
				if (Server::Instance()->getEnableRoom())
				{
					Server::Instance()->getRoomManager()->getUserAttrInfo(player.second->m_platformUserId, info);
				}

				ServerNetwork::Instance()->getSender()->sendUserIn(getRakssid(), player.second->getPlatformUserId(), 
					player.second->getName(), player.second->getTeamId(), player.second->getTeamName(), curPlayer, maxPlayer, info);
			}
		}

		checkUserRelation();
	}
	else
	{
		LordLogError("ClientPeer::sendLoginResult fail and do not sendUserIn to room, name[%s] platform uid[" U64FMT "] entityID[%d] rankID[" U64FMT "] teamId[%d] teamName[%s]",
			m_name.c_str(), m_platformUserId, m_entityPlayer->entityId, getRakssid(), teamId, teamName.c_str());
		ServerNetwork::Instance()->getSender()->sendLoginResult(getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrGetUserAttrFail);
	}
}

void ClientPeer::movePlayer(float toX, float toY, float toZ, float toYaw, float toPitch, bool onGround, bool toMoving, bool rotating, bool fromServer/* = false*/)
{
	if (!m_entityPlayer || m_entityPlayer->playerConqueredTheEnd)
		return;

	if (!fromServer && (m_entityPlayer->isOnVehicle() || m_entityPlayer->isOnAircraft()))
		return;

	float dy = toY - lastPos.y;
	float dx = toX - lastPos.x;
	float dz = toZ - lastPos.z;
	float moveLenSqrXZ = dx * dx + dz * dz;
	float moveLenSqr = moveLenSqrXZ + dy * dy;

	resetLogicSpeedUpCheatIfNeed(moveLenSqrXZ, onGround, fromServer);
	resetInTheAirCheatIfNeed(onGround, fromServer);

	if (m_entityPlayer->isForceTeleport() && !fromServer)
	{
		if (moveLenSqr > 27)
		{
			LordLogInfo(StringUtil::Format("Player move when the force teleport. player name = %s",
				getName().c_str()).c_str());
			EntityTracker::Instance()->forceTeleport(m_entityPlayer->entityId);
			return;
		}
	}
	m_entityPlayer->forceTeleportEnd();

	bool moving = toMoving;
	float maxMoveDis = 3.0f; // incase of knockback distance: 3
	if (PlayerPrefs::Instance()->HasProperty("MaxMoveDis"))
	{
		maxMoveDis = PlayerPrefs::Instance()->GetProperty<float>("MaxMoveDis");
	}
	if (m_entityPlayer->hurtTime > 0 || !m_entityPlayer->onGround || !onGround)
	{
		maxMoveDis = 3000.0f;
	}

	if (!hasMoved)
	{
		if (moveLenSqr > 0.1 || m_entityPlayer->onGround != onGround) // 0.316 * 0.316 = 0.1
		{
			hasMoved = true;
		}
	}

	if (hasMoved)
	{
		lastPos = m_entityPlayer->position;
		Vector3 playerPos = m_entityPlayer->position;
		float yaw = m_entityPlayer->rotationYaw;
		float pitch = m_entityPlayer->rotationPitch;

		if (moving && toY == -999.0f)
		{
			moving = false;
		}


		if (moving)
		{
			playerPos.x = toX;
			playerPos.y = toY;
			playerPos.z = toZ;

			if (Math::Abs(toX) > 3.2E7f || Math::Abs(toZ) > 3.2E7f)
			{
				// kickPlayerFromServer("Illegal position");
				return;
			}
		}

		if (rotating)
		{
			yaw = toYaw;
			pitch = toPitch;
		}

		//m_entityPlayer->triggerEntityUpdate();
		m_entityPlayer->m_yPositionDecrement = 0.0F;
		m_entityPlayer->setPositionAndRotation(lastPos, yaw, pitch);

		Vector3 entityMotion = playerPos - m_entityPlayer->position;
		float motionLenSqr = entityMotion.x * entityMotion.x + entityMotion.z * entityMotion.z;

		/*LordLogInfo("movePlayer, current server pos=(%.2f, %.2f, %.2f) onGround=%d, to pos=(%.2f, %.2f, %.2f) onGround=%d,"
			"entityMotion=(%.2f,%.2f,%.2f),motionLenSqr=%.2f, moveLenSqrXZ=%.2f, maxMoveDis=%.2f,fromServer:%d",

			m_entityPlayer->position.x, m_entityPlayer->position.y, m_entityPlayer->position.z, m_entityPlayer->onGround ? 1 : 0,
			toX, toY, toZ, onGround ? 1 : 0, entityMotion.x, entityMotion.y, entityMotion.z, motionLenSqr, moveLenSqrXZ, maxMoveDis,
			fromServer ? 1 : 0);*/

		if (!fromServer && motionLenSqr > maxMoveDis && !m_entityPlayer->isOnVehicle() && !m_entityPlayer->isOnAircraft() && !m_entityPlayer->isWatchMode())
		{
			setPlayerLocation(lastPos, m_entityPlayer->rotationYaw, m_entityPlayer->rotationPitch);
			EntityTracker::Instance()->forceTeleport(m_entityPlayer->entityId);
			LordLogError("movePlayer failed, move too fast, want to go(%f,%f,%f), set it to last pos(%f,%f,%f), motionLenSqr=%f, playerName=%s", 
				playerPos.x, playerPos.y, playerPos.z, lastPos.x, lastPos.y, lastPos.z, motionLenSqr, getName().c_str());
			return;
		}

		World* pWorld = m_entityPlayer->world;

		float extend = 0.0625F;
		bool noCollisionBeforeMove = pWorld->getCollidingBoundingBoxes
				(m_entityPlayer, m_entityPlayer->boundingBox.contract(extend)).empty();

		if (m_entityPlayer->onGround && !onGround && entityMotion.y > 0.0f)
		{
			m_entityPlayer->jump();
			entityMotion.y = 0;
		} 
		
		if (!m_entityPlayer->onGround) {
			entityMotion.y = 0;
		}
		
		m_entityPlayer->onGround = onGround;
		m_entityPlayer->moveEntity(entityMotion);
		m_entityPlayer->addMovementStat(entityMotion);
		m_entityPlayer->movementTimes();
		if (m_entityPlayer->canMovement() == false)
			m_entityPlayer->enableMovement();

		Vector3 serverToGoPos = m_entityPlayer->position;
		Vector3 detalPos = playerPos - serverToGoPos;

		if (!fromServer)
		{
			m_entityPlayer->fallDistance -= detalPos.y;
		}

		detalPos.y = 0.0f;

		motionLenSqr = detalPos.lenSqr();
		bool wrongMove = false;

		if (motionLenSqr > maxMoveDis)
		{
			wrongMove = true;
			// mcServer.getLogAgent().logWarning(playerEntity.getCommandSenderName() + " moved wrongly!");
		}
		
		m_entityPlayer->setPositionAndRotation(playerPos, yaw, pitch);
		if (fromServer)
			lastPos = playerPos;

		bool collidedAfterMoved = !(pWorld->getCollidingBoundingBoxes(m_entityPlayer, m_entityPlayer->boundingBox.contract(extend)).empty());

		 if ((!fromServer && noCollisionBeforeMove && (wrongMove || collidedAfterMoved) && !m_entityPlayer->isOnAircraft() && !m_entityPlayer->isWatchMode()) 
			 || !SCRIPT_EVENT::WrongMoveEvent::invoke(m_entityPlayer->entityId, playerPos))
		{
			setPlayerLocation(lastPos, yaw, pitch); 
			EntityTracker::Instance()->forceTeleport(m_entityPlayer->entityId);
			LordLogError("playerId=%d, playerName=%s, movePlayer failed, something wrong happened, set it to last pos, last pos(%f,%f,%f),"
				"client want to go to (%f,%f,%f),server can only go to(%f,%f,%f), detalPos=(%f,%f,%f),"
				"diffLenSqr=%f, moving:%s, wrongMove:%s,collidedAfterMoved=%s", m_entityPlayer->entityId, m_entityPlayer->getShowName().c_str(),
				lastPos.x, lastPos.y, lastPos.z, playerPos.x, playerPos.y, playerPos.z, serverToGoPos.x, serverToGoPos.y, serverToGoPos.z,
				detalPos.x, detalPos.y, detalPos.z, motionLenSqr, (moving ? "true" : "false"),
				(wrongMove ? "true" : "false"), (collidedAfterMoved ? "true" : "false"));
			return;
		}

		 if (checkLogicSpeedUpCheat(moveLenSqrXZ, onGround, fromServer) && SCRIPT_EVENT::NeedLogicSpeedUpCheatEvent::invoke(true)) {
			 ServerNetwork::Instance()->getSender()->sendGameover(m_rakSsid, "logic speed up cheat detected", 10);
			 LordLogError("movePlayer failed, %s logic speed up hack, sessionid " U64FMT ", platform id:" U64FMT,
				 getName().c_str(), m_rakSsid, getPlatformUserId());

			 return;
		 }

		m_entityPlayer->onGround = onGround;
		m_entityPlayer->updateFlyingState(m_entityPlayer->position.y - dy, onGround);

		// check on air cheat: set y value on client and lock it
		if (checkInTheAirCheat(onGround, fromServer)) {
			ServerNetwork::Instance()->getSender()->sendGameover(m_rakSsid, "in the air cheat detected", 9);
			LordLogError("ClientPeer::movePlayer, %s in the air hack detected, sessionid " U64FMT ", platform id:" U64FMT,
				getName().c_str(), m_rakSsid, getPlatformUserId());
		}
	}
	else if (currentTicks % 20 == 0)
	{
		setPlayerLocation(lastPos, m_entityPlayer->rotationYaw, m_entityPlayer->rotationPitch);
	}
}


bool ClientPeer::resetLogicSpeedUpCheatIfNeed(float moveLenSqr, bool onGround, bool fromServer)
{
	if (fromServer || moveLenSqr <= 5.2 || !onGround || !m_entityPlayer->onGround
		|| m_entityPlayer->isOnVehicle() || m_entityPlayer->isOnAircraft()
		|| m_entityPlayer->isWatchMode() || m_entityPlayer->getIsFlying())
	{
		m_longMoveBeginTime = 0;
		m_longMoveCnt = 0;
		return true;
	}

	return false;
}

bool ClientPeer::checkLogicSpeedUpCheat(float moveLenSqr, bool onGround, bool fromServer)
{
	if (resetLogicSpeedUpCheatIfNeed(moveLenSqr, onGround, fromServer)) {
		return false;
	}

	bool logicSpeedUpCheat = false;

	ui32 now = Root::Instance()->getCurrentTime();
	if (m_longMoveBeginTime == 0 || now - m_longMoveBeginTime > 1000/*ms*/) {
		m_longMoveBeginTime = now;
		m_longMoveCnt = 0;
	}

	m_longMoveCnt++;

	if (now - m_longMoveBeginTime <= 1000/*ms*/) {
		if (m_longMoveCnt >= 20) {
			logicSpeedUpCheat = true;
			m_longMoveBeginTime = 0;
			m_longMoveCnt = 0;
		}
	}

	return logicSpeedUpCheat;
}

bool ClientPeer::inTheAirCheatIgnoreState() {

	/*LordLogInfo("flying:%d, watchMode:%d, onVehicle:%d, onAircraft:%d,"
		"parachuting:%d, onDecoration:%d, inWater:%d, onLadder:%d, inWeb:%d",
		m_entityPlayer->getIsFlying() ? 1 : 0, m_entityPlayer->isWatchMode() ? 1 : 0, m_entityPlayer->isOnVehicle() ? 1 : 0,
		m_entityPlayer->isOnAircraft() ? 1 : 0, m_entityPlayer->isParachute() ? 1 : 0, m_entityPlayer->isOnDecoration() ? 1 : 0,
		m_entityPlayer->isInWater() ? 1 : 0, m_entityPlayer->isOnLadder() ? 1 : 0, m_entityPlayer->inWeb() ? 1 : 0);*/

	if (m_entityPlayer->getIsFlying()
		|| m_entityPlayer->isWatchMode()
		|| m_entityPlayer->isOnVehicle()
		|| m_entityPlayer->isOnAircraft()
		|| m_entityPlayer->isParachute()
		|| m_entityPlayer->isOnDecoration()
		|| m_entityPlayer->isInWater()
		|| m_entityPlayer->isOnLadder()
		|| m_entityPlayer->inWeb()
		|| m_entityPlayer->isUseElevator()
		|| m_entityPlayer->isGlide()
		)
	{
		return true;
	}

	return false;
}

bool ClientPeer::resetInTheAirCheatIfNeed(bool onGround, bool fromServer) {
	if (inTheAirCheatIgnoreState() || onGround || fromServer || m_entityPlayer->position.y < 0.0f) {
		m_inTheAirBeginTime = 0;
		m_inTheAirCnt = 0;
		return true;
	}

	return false;
}

bool ClientPeer::checkInTheAirCheat(bool onGround, bool fromServer) {
	if (resetInTheAirCheatIfNeed(onGround, fromServer)) {
		return false;
	}
	else {
		ui32 now = Root::Instance()->getCurrentTime();
		if (m_inTheAirBeginTime == 0) {
			m_inTheAirBeginTime = now;
			m_inTheAirCnt = 0;
		}
		m_inTheAirCnt++;

		if (m_inTheAirCnt >= 100 && now - m_inTheAirBeginTime >= 20000/*milliseconds*/) {
			m_inTheAirBeginTime = 0;
			m_inTheAirCnt = 0;
			return true;
		}
	}

	return false;
}

void ClientPeer::onSpawn(String name, EntityPlayerMP *mpPlayer)
{
	m_name = name;
	m_entityPlayer = mpPlayer;
	m_entityPlayer->setShowName(m_showName);
	lastPos = m_entityPlayer->position;
}

void ClientPeer::login(String clientInfo, bool enableIndie, int gameTimeToday)
{
	m_logon = true;

	Document doc = Document();
	doc.Parse(clientInfo.c_str());
	if (!doc.HasParseError())
	{
		m_versionCode = doc.HasMember("version_code") ? StringUtil::ToString(doc.FindMember("version_code")->value.GetInt()) : "1";
		m_packageName = doc.HasMember("package_name") ? doc.FindMember("package_name")->value.GetString() : "com.sandboxol.blockymods";
	}

	if (SCRIPT_EVENT::PlayerLoginEvent::invoke(this, clientInfo, enableIndie, gameTimeToday))
		ServerNetwork::Instance()->getSender()->sendPlayerDBDataReady(getRakssid());

	loadUserMoney();
	if (!LuaEngine::getInstance()->isLoadSuc()) {
		this->sendLoginResult(true, 0, "", 1, 8);
		ServerNetwork::Instance()->getSender()->sendSystemsChat(LanguageKey::SYSTEM_MESSAGE_PLAYER_ENTER_SERVER, getName(), 0);
	}
}

void ClientPeer::logout()
{
	if (!m_logon) {
		LordLogWarning("player unlogon, no need to logout");
		return;
	}

	m_logon = false;
	int delayFreeTicks = 60;

	if (SCRIPT_EVENT::PlayerLogoutEvent::invoke(getRakssid()))
	{
		if (!LuaEngine::getInstance()->isLoadSuc())
		{
			ServerNetwork::Instance()->getSender()->sendSystemsChat(LanguageKey::SYSTEM_MESSAGE_PLAYER_EXIT_SERVER, getName(), 0);
		}

		ServerNetwork::Instance()->getSender()->sendUserOut(getPlatformUserId());
		String name = getName();
		LordLogInfo("ClientPeer::logout, sessionId_" U64FMT "_name_%s, platform id=" U64FMT, getRakssid(), name.c_str(), getPlatformUserId());

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
		if (Server::Instance()->getEnableRoom())
		{
			Server::Instance()->getRoomManager()->sendUserOut(this->m_platformUserId, std::string(this->m_requestIdFromClient.c_str()));
		}
#endif	

		if (this->m_platformUserId == 0)
		{
			LordLogWarning("sending user out to room, platformId=" U64FMT ",m_logon=%s",
				this->m_platformUserId, (this->m_logon ? "true" : "false"));
		}
	}

	removePlayerEntity();

	if (m_entityPlayer) {
		m_entityPlayer->setLogout(true);
		m_entityPlayer->setDelayTicksToFree(delayFreeTicks);
		m_entityPlayer = nullptr;
	}
}

void ClientPeer::reconnect(std::shared_ptr<ClientPeer> clientPeer)
{
	m_logon = true;
	setLanguage(clientPeer->getLanguage());
	setPlatformUserId(clientPeer->getPlatformUserId());
	setClientReady(clientPeer->clientUiIsReady() ? 1 : 0);
	m_showName = clientPeer->getShowName();
	m_packageName = clientPeer->getPackageName();
	m_versionCode = clientPeer->getVersionCode();
	lastPos = clientPeer->lastPos;
	respawnPos = clientPeer->respawnPos;
	SCRIPT_EVENT::PlayerReconnectSuccessEvent::invoke(this);
}

void ClientPeer::resetPos(float x, float y, float z, float yaw)
{
	auto player = getEntityPlayer();
	NULL_RETURN_VOID(player);

	player->motion = Vector3(0.0f, 0.0f, 0.0f);
	if (yaw != 361)
	{
		player->rotationYaw = yaw;
	}

	if (player->isOnVehicle())
	{
		player->getOffVehicle();
		LordLogInfo("player[%s] reset pos get off vehicle", player->getEntityName().c_str());
	}

	if (player->isOnDecoration())
	{
		player->takeOffDecoration();
		LordLogInfo("player[%s] reset pos get off decoration", player->getEntityName().c_str());
	}

	if (player->isOnFlyingMount())
	{
		player->getOffFlyingMount();
		LordLogInfo("player[%s] reset pos get off flying mount", player->getEntityName().c_str());
	}

	if (player->isOnFloatingMount())
	{
		player->getOffFloatingMount();
		LordLogInfo("player[%s] reset pos get off floating mount", player->getEntityName().c_str());
	}

	//setPlayerLocation({ x, y, z }, player->rotationYaw, player->rotationPitch);
	movePlayer(x, y, z, player->rotationYaw, player->rotationPitch, player->onGround, true, true, true);
	EntityTracker::Instance()->forceTeleport(player->entityId);
	player->forceTeleportStart();
}

void ClientPeer::setShowName(String name, ui64 targetId)
{
	if (m_entityPlayer != NULL) {
		if (targetId == 0)
		{
			m_showName = name;
			m_entityPlayer->setShowName(m_showName);
			Server::Instance()->getNetwork()->getSender()->broadCastChangePlayerInfo(m_entityPlayer);
		}
		else
		{
			m_entityPlayer->setShowName(name);
			Server::Instance()->getNetwork()->getSender()->sendChangePlayerInfo(targetId, m_entityPlayer);
			m_entityPlayer->setShowName(m_showName);
		}
	}
}

void ClientPeer::setPlayerLocation(const Vector3& pos, float yaw, float pitch)
{
	hasMoved = false;
	lastPos = pos;
	m_entityPlayer->setPositionAndRotation(pos, yaw, pitch);
	m_entityPlayer->recordOldPos();
}

void ClientPeer::setPlatformUserId(ui64 platformUserId)
{
	this->m_platformUserId = platformUserId;
	if (m_entityPlayer)
	{
		m_entityPlayer->setPlatformUserId(platformUserId);
	}
}

void ClientPeer::setRequestIdFromClient(String requestId)
{
	this->m_requestIdFromClient = requestId;
}

void ClientPeer::rebirth()
{
	EntityPlayerMP *pPlayer = getEntityPlayer();
	if (!pPlayer) 
	{
		LordLogError("rebirth before login ?, session id " U64FMT ", name %s, hasLogon:%s",
			getRakssid(), getName(), hasLogon() ? "true" : "false");
		return;
	}

	auto pworld = Server::Instance()->getWorld();
	Vector3i spawnPos = getRespawnPos();

	String oldShowName = pPlayer->getShowName();

	int oldSex = pPlayer->m_sex;
	int oldVip = pPlayer->m_vip;
	int oldDefaultIdle = pPlayer->m_defaultIdle;
	bool oldCanBuildBlockQuickly = pPlayer->m_canBuildBlockQuickly;

	// save old wallet data
	i64 diamondBlues = -1;
	i64 diamondGolds = -1;
	i64 golds = -1, currency = -1;
	Wallet* wallet = pPlayer->m_wallet;
	if (wallet)
	{
		diamondBlues = wallet->m_diamondBlues;
		diamondGolds = wallet->m_diamondGolds;
		golds = wallet->getGolds();
		currency = wallet->getCurrency();
	}

	Server::Instance()->getWorld()->removeEntityFromChunk(pPlayer);

	pPlayer->reconstruct(spawnPos);
	pPlayer->setShowName(oldShowName);
	pPlayer->m_sex = oldSex;
	pPlayer->m_vip = oldVip;
	pPlayer->setRaknetID(getRakssid());
	pPlayer->setPlatformUserId(getPlatformUserId());
	pPlayer->setTeamName(getTeamName());
	pPlayer->setTeamId(getTeamId());
	pPlayer->m_defaultIdle = oldDefaultIdle;

	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->sendRebirth(getRakssid(), pPlayer->entityId, spawnPos, oldSex, oldShowName, oldDefaultIdle, pPlayer->m_vip);

	UserAttrInfo attrInfo;
	attrInfo.sex = 1; // default, 1:boy, 2:girl
	if (Server::Instance()->getEnableRoom())
	{
		bool attrExisted = Server::Instance()->getRoomManager()->getUserAttrInfo(getPlatformUserId(), attrInfo);
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
			pPlayer->m_headEffectId = attrInfo.headEffectId;
			pPlayer->m_skinColor = attrInfo.skinColor;
			pPlayer->m_defaultIdle = attrInfo.defaultIdle;
			pPlayer->m_bagId = attrInfo.bagId;
			pPlayer->m_suitsId = attrInfo.suitsId;
			pPlayer->m_actionLockedInfoMap.clear();
			for (int i = 0; i < attrInfo.unlockedSelectableActions.size(); ++i)
			{
				pPlayer->m_actionLockedInfoMap[attrInfo.unlockedSelectableActions[i]] = false;
			}
		}
		if (attrExisted) 
		{
			sender->sendUserAttr(getRakssid(), pPlayer->entityId, attrInfo, pPlayer);

			LordLogInfo("ClientPeer::rebirth, send user attr info, session id " U64FMT ", platform uid " U64FMT ", name %s,"
				"entityId[%d],faceId[%d],hairId[%d],topsId[%d],pantsId[%d],shoesId[%d],glassesId[%d],"
				"scarfId[%d], wingId[%d], hatId[%d],tailId[%d], decoratehatId[%d], armId[%d],extrawingId[%d],footHaloId[%d],  skinColor r[%f] , skinColor g[%f] , skinColor b[%f] , skinColor a[%f]",
				getRakssid(), getPlatformUserId(), getName().c_str(), pPlayer->entityId, attrInfo.faceId, attrInfo.hairId, attrInfo.topsId,
				attrInfo.pantsId, attrInfo.shoesId, attrInfo.glassesId, attrInfo.scarfId, attrInfo.wingId, attrInfo.hatId, attrInfo.tailId, attrInfo.decoratehatId, attrInfo.armId, attrInfo.extrawingId, attrInfo.footHaloId, attrInfo.skinColor.r, attrInfo.skinColor.g, attrInfo.skinColor.b, attrInfo.skinColor.a);
		}
		else 
		{
			LordLogError("ClientPeer::rebirth, get user attr info fail and did not send user attr, session id " U64FMT ", platform uid " U64FMT ", name %s",
				getRakssid(), getPlatformUserId(), getName().c_str());
		}
	}

	LordLogInfo("ClientPeer::rebirth name[%s] entityID[%d] rankID[%lld]",
		getName().c_str(), pPlayer->entityId, getRakssid());

	onSpawn(getName(), pPlayer);
	SCRIPT_EVENT::PlayerRespawnEvent::invoke(this);
	pworld->spawnEntityInWorld(pPlayer);
	// reset wallet data
	pPlayer->setCurrency(currency);
	pPlayer->m_wallet->m_diamondBlues = diamondBlues;
	pPlayer->m_wallet->m_diamondGolds = diamondGolds;
	pPlayer->m_wallet->setGolds(golds);
	sender->sendInitMoney(getRakssid(), diamondBlues, diamondGolds, golds);
	EntityTracker::Instance()->forceTeleport(pPlayer->entityId);
	pPlayer->forceTeleportStart();

	pPlayer->m_canBuildBlockQuickly = oldCanBuildBlockQuickly;
	if (pPlayer->m_canBuildBlockQuickly)
		sender->sendChangeQuicklyBuildBlockStatus(getRakssid(), true, false);

	SCRIPT_EVENT::PlayerAfterRespawnEvent::invoke(this);
}

void ClientPeer::onTick()
{
	// todo  send init blocks
	++currentTicks;

	if (currentTicks - ticksOfLastKeepAlive > 20L)
	{
		ticksOfLastKeepAlive = currentTicks;
		// keepAliveTimeSent = Root::Instance()->getCurrentTime() / 1000000L;
		// keepAliveRandomID = randomGenerator.nextInt();
		// sendPacketToPlayer(new Packet0KeepAlive(keepAliveRandomID));
	}

	if (chatSpamThresholdCount > 0)
	{
		--chatSpamThresholdCount;
	}

	if (creativeItemCreationSpamThresholdTally > 0)
	{
		--creativeItemCreationSpamThresholdTally;
	}
}

void ClientPeer::doGunShootTarget(ui32 tracyType, ui32 hitEntityID, const PACKET_DETAIL::Vector3& hitpos,
	bool isHeadshot, const GunSetting* pGunSetting)
{
	EntityPlayer* targetPlayerEntity = nullptr;
	EntityVehicle* targetVehicleEntity = nullptr;
	EntityCreature* targetCreatureEntity = nullptr;
	EntityActorNpc* targetActorNpcEntity = nullptr;
	EntityFlyingMount* targetFlyingMountEntity = nullptr;
	EntityFloatingMount* targetFloatingMountEntity = nullptr;

	switch (tracyType)
	{
	case ENTITY_PLAYER_TRACE_TYPE:
		targetPlayerEntity = dynamic_cast<EntityPlayer*>(Server::Instance()->getWorld()->getPlayerEntity(hitEntityID));
		if (targetPlayerEntity) {
			m_entityPlayer->shootTargetByGun(targetPlayerEntity, hitpos, isHeadshot, pGunSetting);
		}
		break;
	case ENTITY_VEHICLE_TRACE_TYPE:
		targetVehicleEntity = dynamic_cast<EntityVehicle*>(Server::Instance()->getWorld()->getEntity(hitEntityID));
		if (targetVehicleEntity) {
			targetVehicleEntity->shootTargetByGun(m_entityPlayer, hitpos, pGunSetting);
		}
		break;
	case ENTITY_CREATUREAI_TRACE_TYPE:
		targetCreatureEntity = dynamic_cast<EntityCreatureAI*>(Server::Instance()->getWorld()->getEntity(hitEntityID));
		if (targetCreatureEntity) {
			m_entityPlayer->attackCreatureEntity(targetCreatureEntity, (int)PLAYER_ATTACK_CREATURE_GUN);
		}
		break;
	case ENTITY_ACTOR_NPC_TRACE_TYPE:
		targetActorNpcEntity = dynamic_cast<EntityActorNpc*>(Server::Instance()->getWorld()->getEntity(hitEntityID));
		if (targetActorNpcEntity) {
			m_entityPlayer->attackActorNpc(targetActorNpcEntity, PLAYER_ATTACK_GUN);
		}
		break;
	case ENTITY_FLYING_MOUNT_TRACE_TYPE:
		targetFlyingMountEntity = dynamic_cast<EntityFlyingMount*>(Server::Instance()->getWorld()->getEntity(hitEntityID));
		if (targetFlyingMountEntity) {
			targetPlayerEntity = dynamic_cast<EntityPlayer*>(Server::Instance()->getWorld()->getPlayerEntity(targetFlyingMountEntity->getRiderId()));
			if (targetPlayerEntity) {
				m_entityPlayer->shootTargetByGun(targetPlayerEntity, hitpos, isHeadshot, pGunSetting);
			}
		}
		break;
	case ENTITY_FLOATING_MOUNT_TRACE_TYPE:
		targetFloatingMountEntity = dynamic_cast<EntityFloatingMount*>(Server::Instance()->getWorld()->getEntity(hitEntityID));
		if (targetFloatingMountEntity) {
			targetPlayerEntity = dynamic_cast<EntityPlayer*>(Server::Instance()->getWorld()->getPlayerEntity(targetFloatingMountEntity->getRiderId()));
			if (targetPlayerEntity) {
				m_entityPlayer->shootTargetByGun(targetPlayerEntity, hitpos, isHeadshot, pGunSetting);
			}
		}
		break;
	default:
		break;
	}

}

void ClientPeer::reloadBullet()
{
	m_entityPlayer->reloadBullet();
}


void ClientPeer::loadUserMoney()
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("load use money error: url is null");
		return;
	}
	HttpRequest::loadUserMoney(url, m_platformUserId,
		[](i64 platformUserId, String result) {
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}
			Document* doc = new Document;
			doc->Parse(result.c_str());
			auto player = peer->getEntityPlayer();
			if (doc->HasParseError())
			{	
				delete doc;
				doc = nullptr;
				return;
			}
			if (doc->HasMember("code"))
			{
				int code = doc->FindMember("code")->value.GetInt();
				if (code != 1)
				{
					delete doc;
					doc = nullptr;
					return;
				}
				if (doc->HasMember("data"))
				{
					auto data = doc->FindMember("data")->value.GetObject();
					i64 diamondBlues = 0;
					i64 diamondGolds = 0;
					i64 golds = 0;
					if (data.HasMember(DIAMOND_BLUES))
					{
						diamondBlues = data.FindMember(DIAMOND_BLUES)->value.GetInt64();
						player->m_wallet->m_diamondBlues = diamondBlues;
					}
					if (data.HasMember(DIAMOND_GOLDS))
					{
						diamondGolds = data.FindMember(DIAMOND_GOLDS)->value.GetInt64();
						player->m_wallet->m_diamondGolds = diamondGolds;
					}
					if (data.HasMember("golds"))
					{
						golds = data.FindMember("golds")->value.GetInt64();
						player->m_wallet->setGolds(golds);
					}
					
					ServerNetwork::Instance()->getSender()->sendInitMoney(peer->getRakssid(), diamondBlues, diamondGolds, golds);
				}
			}
			delete doc;
			doc = nullptr;
	});
}

void ClientPeer::saveCacheRequest(InterruptType interruptType, VarList & args)
{
	m_interruptType = interruptType;
	m_requestCache = args;
}

void ClientPeer::doPay(i32 currencyType, i32 uniqueId, i32 price, std::function<void(i64, String, bool)> fun, bool skipRemind)
{
	auto pPlayer = getEntityPlayer();
	NULL_RETURN_VOID(pPlayer);
	if (!skipRemind && currencyType == 1)
	{
		if (!checkDiamondBlues(price))
		{
			saveCacheRequest(InterruptType_doPay, VarList() << currencyType << uniqueId << price << fun);
			return;
		}
	}
	HttpRequest::pay(getHttpUrl(), currencyType, uniqueId, price, m_platformUserId, fun);
}

void ClientPeer::doConsumeDiamonds(i32 uniqueId, i32 diamonds, String remark, bool isConsume, String url)
{
	doPay(1, uniqueId, diamonds,
		[isConsume, remark](i64 platformUserId, String result, bool isSuccess)
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}
		//bool paySuccess = payResult(result, isSuccess);
		OrderPayResult payResult = peer->payResultByConsume(platformUserId, isSuccess, isConsume, result);
		if (payResult.code == 1)
		{
			SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(peer->getRakssid(), true, "pay.success", remark, payResult.orderId);
		}
		else
		{
			LordLogError("ClientPeer::consumeDiamonds : pay fail, code =  %d , remark = %s ", payResult.code, remark.c_str());
			SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(peer->getRakssid(), false, "pay.failed", remark, "");
		}
	}, false);
}

bool ClientPeer::checkDiamondBlues(int price)
{
	if (m_bNolongerRemind)
		return true;
	auto pPlayer = getEntityPlayer();
	NULL_RETURN(pPlayer, false);
	Wallet* wallet = pPlayer->m_wallet;
	if (wallet && wallet->m_diamondBlues >= price)
		return true;
	//String data = CommonDataBuilder().addI64Param("needDiamondGolds", price - wallet->m_diamondBlues).getData();
	//ServerNetwork::Instance()->getCommonSender()->sendServerCommonData(m_rakSsid, "ShowDiamondBlueLackTips", "", data);
	return false;
}

void ClientPeer::doPickupItemPay(i32 itemId, i32 itemMeta, i32 addNum, i32 price, i32 entityItemId, i32 currencyType)
{
	doPay(currencyType, itemId, price,
		[itemId, itemMeta, addNum, price, entityItemId](i64 platformUserId, String result, bool isSuccess)
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}

		bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
		SCRIPT_EVENT::PlayerBuyPickupItemResultEvent::invoke(
			peer->getRakssid(), paySuccess, itemId, itemMeta, addNum, price, entityItemId);

		if (paySuccess)
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), itemId, ResultCode::PAY_SUCCESS, "pay success");
			ServerNetwork::Instance()->getSender()->notifyGetItem(peer->getRakssid(), itemId, itemMeta, addNum);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
			LordLogInfo("pickup Item [%d] pay failure", itemId);
		}
	}, false);
}

void ClientPeer::doBuyGoods(i32 uniqueId, i32 coinId, i32 price, std::function<void(i64, String, bool)> fun)
{
	auto pPlayer = getEntityPlayer();
	NULL_RETURN_VOID(pPlayer);
	doPay(coinId, uniqueId, price, fun, true);
}

void ClientPeer::doBuyRespawn(i32 uniqueId, i32 price, std::function<void(i64, String, bool)> fun)
{
	auto pPlayer = getEntityPlayer();
	NULL_RETURN_VOID(pPlayer);
	doPay(1, uniqueId, price, fun, false);
}

void ClientPeer::doBuyChangeActor(i32 uniqueId, i32 price, std::function<void(i64, String, bool)> fun)
{
	auto pPlayer = getEntityPlayer();
	NULL_RETURN_VOID(pPlayer);
	doPay(1, uniqueId, price, fun, false);
}

void ClientPeer::doManorPay(String webPayUrl, String dataServerPayUrl, i32 currencyType, i32 uniqueId, i32 price, ui64 platformUserId, std::function<void(String, bool)> fun)
{
	if (currencyType == 1)
	{
		if (!checkDiamondBlues(price))
		{
			saveCacheRequest(InterruptType_doManorPay, VarList() << webPayUrl << dataServerPayUrl << currencyType << uniqueId << price << platformUserId << fun);
			return;
		}
	}
	ManorRequest::pay(webPayUrl, dataServerPayUrl, currencyType, uniqueId, price, platformUserId, fun);
}

void ClientPeer::checkUserRelation()
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("load use money error: url is null");
		return;
	}

	vector<i64>::type users;
	for (const auto & user:  m_entityPlayer->world->getPlayers())
	{
		if (user.second->getPlatformUserId() != getPlatformUserId() )
		{
			users.push_back(user.second->getPlatformUserId());
		}
	}

	if (users.size() == 0)
	{
		return;
	}

	HttpRequest::checkUserRelation(url, m_platformUserId, users, [](i64 platformUserId, String result) {
		
		HttpResponse response = ManorRequest::parseResponse(true, "checkUserRelation", result);
		if (response.code == 1)
		{
			Document  doc = Document();
			doc.Parse(response.data.c_str());
			vector<i64>::type friends;
			if (doc.IsArray())
			{
				for (const auto& item : doc.GetArray())
				{
					friends.push_back(item.GetInt64());
				}
			}

			if (friends.size() == 0)
			{
				return;
			}

			auto player = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (auto player = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId))
			{
				ServerNetwork::Instance()->getSender()->syncUserRelation(player->getRakssid(), friends);
			}

			vector<i64>::type me;
			me.push_back(platformUserId);
			for (const auto& user: friends)
			{
				if (auto player = ServerNetwork::Instance()->findPlayerByPlatformUserId(user))
				{
					ServerNetwork::Instance()->getSender()->syncUserRelation(player->getRakssid(), me);
				}
			}
		}
	});
}

void ClientPeer::buyGoods(i32 groupIndex, i32 goodsIndex, i32 goodsId)
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("buy goods error: url is null");
		return;
	}

	Shop* shop = Server::Instance()->getWorld()->getShop();
	NULL_RETURN_VOID(shop);

	if (groupIndex < 0 || groupIndex >= shop->getGoods().size())
		return;
	auto group = shop->getGoods()[groupIndex];
	if (goodsIndex < 0 || goodsIndex >= group.getGoods().size())
		return;
	auto goods = group[goodsIndex];
	if (goods.uniqueId != goodsId)
	{
		LordLogError("goods uniqueId is invalid, goodsId:", goodsId);
		return;
	}

	if (!goods.canBuy())
	{
		ServerNetwork::Instance()->getSender()->sendShoppingResult(getRakssid(), 2, "inventory shortage");
		return;
	}

	Wallet* wallet = getEntityPlayer()->m_wallet;
	if (!wallet)
	{
		LordLogError("wallet is null");
		return;
	}

	ui64 money = wallet->getMoneyByCoinId(goods.coinId);
	i32 price = goods.blockymodsPrice;
	if (money < price && goods.coinId <= 3)
	{
		ServerNetwork::Instance()->getSender()->sendShoppingResult(getRakssid(), 3, "not enough money");
		return;
	}

	if (!getEntityPlayer()->getInventory())
	{
		return;
	}

	Item* pItem = Item::getItemById(goods.itemId);
	if (pItem)
	{
		if (!getEntityPlayer()->getInventory()->isCanAddItem(goods.itemId, goods.itemMeta, goods.itemNum))
		{
			ServerNetwork::Instance()->getSender()->sendShoppingResult(this->getRakssid(), 4, "player inventory full");
			return;
		}
	}

	if (!getEntityPlayer()->canAddBulletItem(goods.itemId))
	{
		ServerNetwork::Instance()->getSender()->sendShoppingResult(this->getRakssid(), 4, "has no gun");
		return;
	}

	std::function<void()> func = [=]() {
		IntProxy msgType;
		msgType.value = 0;
		BoolProxy isAddItem;
		isAddItem.value = true;
		if (!SCRIPT_EVENT::PlayerBuyGoodsEvent::invoke(getRakssid(), group.getCategory(), goods.itemId, &msgType, &isAddItem))
		{
			if (msgType.value != 0)
			{
				auto lang = getLanguage();
				std::string msg = MultiLanTipSetting::getMessage(lang, msgType.value, "");
				ServerNetwork::Instance()->getSender()->sendShoppingResult(this->getRakssid(), 5, msg.c_str());
			}
			return;
		}
		shop->buyGoods(groupIndex, goodsIndex);
		if (goods.coinId <= 2)
		{
			doBuyGoods(goods.uniqueId, goods.coinId, price,
				[=](i64 platformUserId, String result, bool isSuccess) {
				Shop* shop = Server::Instance()->getWorld()->getShop();
				if (!shop)
				{
					return;
				}

				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (!peer || !peer->getEntityPlayer())
				{
					shop->resumeBuyGoods(groupIndex, goodsIndex);
					return;
				}

				if (!isSuccess)
				{
					shop->resumeBuyGoods(groupIndex, goodsIndex);
					ServerNetwork::Instance()->getSender()->sendShoppingResult(peer->getRakssid(), 6, "buy failed");
					return;
				}
				Document* doc = new Document;
				doc->Parse(result.c_str());
				if (doc->HasParseError())
				{
					delete doc;
					doc = nullptr;
					shop->resumeBuyGoods(groupIndex, goodsIndex);
					ServerNetwork::Instance()->getSender()->sendShoppingResult(peer->getRakssid(), 6, "buy failed");
					return;
				}
				if (doc->HasMember("code"))
				{
					int code = doc->FindMember("code")->value.GetInt();
					if (code != 1)
					{
						delete doc;
						doc = nullptr;
						shop->resumeBuyGoods(groupIndex, goodsIndex);
						ServerNetwork::Instance()->getSender()->sendShoppingResult(peer->getRakssid(), 6, "buy failed");
						return;
					}
					if (doc->HasMember("data"))
					{
						auto player = peer->getEntityPlayer();
						auto group = shop->getGoods()[groupIndex];
						auto goods = group[goodsIndex];
						if (goods.itemId >= 10000)
						{
							player->addCurrency(goods.itemNum);
							ManorRequest::trade(player->getPlatformUserId(), goods.itemNum);
						}
						else
						{
							if (isAddItem.value)
							{
								if (SCRIPT_EVENT::PlayerBuyGoodsAddItemEvent::invoke(peer->getRakssid(), goods.itemId, goods.itemNum, goods.itemMeta))
								{
									player->addItem(goods.itemId, goods.itemNum, goods.itemMeta);
								}
								ServerNetwork::Instance()->getSender()->notifyGetItem(peer->getRakssid(), goods.itemId, goods.itemMeta, goods.itemNum);
							}
						}
						SCRIPT_EVENT::PlayerBuyGoodsSuccessEvent::invoke(peer->getRakssid(), group.getCategory(), goods.itemId);
						auto data = doc->FindMember("data")->value.GetObject();
						i64 diamondBlues = 0;
						i64 diamondGolds = 0;
						i64 golds = 0;
						if (data.HasMember(DIAMOND_BLUES))
						{
							diamondBlues = data.FindMember(DIAMOND_BLUES)->value.GetInt64();
							player->m_wallet->m_diamondBlues = diamondBlues;
						}
						if (data.HasMember(DIAMOND_GOLDS))
						{
							diamondGolds = data.FindMember(DIAMOND_GOLDS)->value.GetInt64();
							player->m_wallet->m_diamondGolds = diamondGolds;
						}
						if (data.HasMember("golds"))
						{
							golds = data.FindMember("golds")->value.GetInt64();
							player->m_wallet->setGolds(golds);
						}
						ServerNetwork::Instance()->getSender()->sendInitMoney(peer->getRakssid(), diamondBlues, diamondGolds, golds);
						if (data.HasMember("orderId"))
						{
							auto orderId = data.FindMember("orderId")->value.GetString();
							HttpRequest::resumeOrder(peer->getHttpUrl(), orderId);
							ServerNetwork::Instance()->getSender()->sendShoppingResult(peer->getRakssid(), code, String(orderId));
						}
					}
				}
				delete doc;
				doc = nullptr;
			});
		}
		else
		{
			auto player = getEntityPlayer();
			player->subCurrency(price);
			player->addItem(goods.itemId, goods.itemNum, goods.itemMeta);
			ManorRequest::trade(player->getPlatformUserId(), -price);
			ServerNetwork::Instance()->getSender()->sendShoppingResult(this->getRakssid(), 6, "success");
			ServerNetwork::Instance()->getSender()->notifyGetItem(this->getRakssid(), goods.itemId, goods.itemMeta, goods.itemNum);
		}
	};

	if (goods.coinId == 1)
	{
		if (!checkDiamondBlues(price))
		{
			saveCacheRequest(InterruptType_doBuyGoods, VarList() << func);
			return;
		}
	}

	func();
}

void ClientPeer::buyRespawn(bool isRespawn, i32 uniqueId, i32 index)
{
	Shop* shop = Server::Instance()->getWorld()->getShop();
	if (!shop)
	{
		SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(getRakssid(), 0);
		return;
	}

	shop->removeRespawnRecord(getRakssid());

	if (index == -1)
	{
		SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(getRakssid(), 0);
		return;
	}

	bool canBuy = true;
	if (!isRespawn)
	{
		canBuy = false;
	}

	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("buy respawn error: url is null");
		canBuy = false;
	}

	RespawnGoods goods = shop->getRespawnGoods()[index];
	if (goods.uniqueId == -1 || goods.uniqueId != uniqueId)
	{
		canBuy = false;
	}

	ui64 money = getEntityPlayer()->m_wallet->getMoneyByCoinId(goods.coinId);
	i32 price = goods.blockymodsPrice;
	if (money < price)
	{
		canBuy = false;
	}

	if (!canBuy)
	{
		SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(getRakssid(), 0);
		return;
	}

	doBuyRespawn(goods.uniqueId, price,
		[](i64 platformUserId, String result, bool isSuccess) {
		
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}

		bool buySuccess = true;
		if (!isSuccess)
		{
			buySuccess = false;
		}
		else
		{
			Document* doc = new Document;
			doc->Parse(result.c_str());
			if (doc->HasParseError())
			{
				buySuccess = false;
			}
			if (doc->HasMember("code"))
			{
				int code = doc->FindMember("code")->value.GetInt();
				if (code != 1)
				{
					buySuccess = false;
				}
				if (code == 1 && doc->HasMember("data"))
				{
					auto player = peer->getEntityPlayer();
					if (player->isDead)
					{
						peer->rebirth();
					}
					else
					{
						player->heal(player->getMaxHealth());
						SCRIPT_EVENT::PlayerRespawnEvent::invoke(peer.get());
					}

					auto data = doc->FindMember("data")->value.GetObject();
					i64 diamondBlues = 0;
					i64 diamondGolds = 0;
					i64 golds = 0;
					if (data.HasMember(DIAMOND_BLUES))
					{
						diamondBlues = data.FindMember(DIAMOND_BLUES)->value.GetInt64();
						player->m_wallet->m_diamondBlues = diamondBlues;
					}
					if (data.HasMember(DIAMOND_GOLDS))
					{
						diamondGolds = data.FindMember(DIAMOND_GOLDS)->value.GetInt64();
						player->m_wallet->m_diamondGolds = diamondGolds;
					}
					if (data.HasMember("golds"))
					{
						golds = data.FindMember("golds")->value.GetInt64();
						player->m_wallet->setGolds(golds);
					}
					ServerNetwork::Instance()->getSender()->sendInitMoney(peer->getRakssid(), diamondBlues, diamondGolds, golds);					
					if (data.HasMember("orderId"))
					{
						auto orderId = data.FindMember("orderId")->value.GetString();
						HttpRequest::resumeOrder(peer->getHttpUrl(), orderId);
					}
				}
			}
			delete doc;
			doc = nullptr;
		}
		SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(peer->getRakssid(), buySuccess ? 1 : 0);
	});
}

void ClientPeer::buyChangeActor(bool isCanChangeActor, int changeActorCount, int needMoneyCount)
{
	if (changeActorCount == 0 || needMoneyCount == 0 || isCanChangeActor)
	{
		this->getEntityPlayer()->setChangePlayerActor(isCanChangeActor, changeActorCount, needMoneyCount);
	}
	else 
	{
		String url = getHttpUrl();
		if (url.length() == 0)
		{
			LordLogError("buy change actor error: url is null");
			SCRIPT_EVENT::PlayerBuyActorResultEvent::invoke(this->getRakssid(), false);
		}

		doBuyChangeActor(changeActorCount, needMoneyCount,
			[](i64 platformUserId, String result, bool isSuccess) {

			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			if (!isSuccess)
			{
				return;
			}

			Document* doc = new Document;
			doc->Parse(result.c_str());
			if (!doc->HasParseError() && doc->HasMember("code"))
			{
				int code = doc->FindMember("code")->value.GetInt();
				if (code == 1 && doc->HasMember("data"))
				{
					auto player = peer->getEntityPlayer();
					auto data = doc->FindMember("data")->value.GetObject();
					i64 diamondBlues = 0;
					i64 diamondGolds = 0;
					i64 golds = 0;
					if (data.HasMember(DIAMOND_BLUES))
					{
						diamondBlues = data.FindMember(DIAMOND_BLUES)->value.GetInt64();
						player->m_wallet->m_diamondBlues = diamondBlues;
					}
					if (data.HasMember(DIAMOND_GOLDS))
					{
						diamondGolds = data.FindMember(DIAMOND_GOLDS)->value.GetInt64();
						player->m_wallet->m_diamondGolds = diamondGolds;
					}
					if (data.HasMember("golds"))
					{
						golds = data.FindMember("golds")->value.GetInt64();
						player->m_wallet->setGolds(golds);
					}
					ServerNetwork::Instance()->getSender()->sendInitMoney(peer->getRakssid(), diamondBlues, diamondGolds, golds);
					if (data.HasMember("orderId"))
					{
						auto orderId = data.FindMember("orderId")->value.GetString();
						String url = peer->getHttpUrl();
						if (url.length() == 0)
						{
							LordLogError("resume order error: url is null");
						}
						HttpRequest::resumeOrder(url, orderId);
					}
					player->setChangePlayerActor(true, 1, 0);
				}
			}
			delete doc;
			doc = nullptr;
		});
	}
}

void ClientPeer::pickupItemPay(i32 itemId, i32 itemMeta, i32 addNum, i32 price, i32 entityItemId, i32 currencyType)
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("pickup Item pay error: url is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}

	Wallet* wallet = getEntityPlayer()->m_wallet;
	if (!wallet)
	{
		LordLogError("wallet is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}
	ui64 money = wallet->getMoneyByCoinId(currencyType);
	if (money < price)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::LACK_MONEY, "not enough money");
		return;
	}

	if (!getEntityPlayer()->getInventory() || !getEntityPlayer()->getInventory()->isCanAddItem(itemId, itemMeta, addNum))
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::INVENTORY_FULL, "inventory is full");
		return;
	}

	doPickupItemPay(itemId, itemMeta, addNum, price, entityItemId, currencyType);
}

bool ClientPeer::checkParam(String url, i32 currencyType, i32 price)
{
	if (url.length() == 0)
	{
		LordLogError("check param: url is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return false;
	}

	Wallet* wallet = getEntityPlayer()->m_wallet;
	if (!wallet)
	{
		LordLogError("wallet is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return false;
	}
	ui64 money = wallet->getMoneyByCoinId(currencyType);
	if (money < price)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::LACK_MONEY, "not enough money");
		return false;
	}
	return true;
}

void ClientPeer::upgradeManor()
{
	String url = getHttpUrl();
	String dataServerUrl = Server::Instance()->getDataServerUrl();

	if (dataServerUrl.length() == 0)
	{
		LordLogError("upgrade manor  pay error: dataServerUrl is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}

	ManorInfo* info = getEntityPlayer()->m_manor->getNextInfo();
	if (info && info->level > info->maxLevel)
	{
		return;
	}
	if (info && checkParam(url, (i32)info->currencyType, info->price))
	{
		i32 currencyType = (i32)info->currencyType;
		i32 price = info->price;
		i32 level = info->level;

		i64 platformUserId = m_platformUserId;
		doManorPay(url, dataServerUrl, currencyType, level, price, m_platformUserId,
			[price, currencyType, platformUserId, dataServerUrl](String result, bool isSuccess)
		{
			bool paySuccess = false;
			i32  exchangePrice = price;
			if ((i32)currencyType <=2 )
			{
				IntProxy rate;
				rate.value = price;
				SCRIPT_EVENT::GetMoneyExchangeRateEvent::invoke(&rate);
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer && peer->getEntityPlayer())
				{
					paySuccess = peer->payResult(platformUserId, result, isSuccess);
				}
				
				exchangePrice = rate.value;
			}
			else
			{
				paySuccess = ManorRequest::manorPayResult(platformUserId, result, isSuccess);
			}
			
			if (paySuccess)
			{
				ManorRequest::upgradeManor(dataServerUrl, platformUserId, exchangePrice, 
					[platformUserId, exchangePrice](String result, bool isSuccess) 
				{
					String logTag = StringUtil::Format("upgradeManor(UserId=%lld)", platformUserId);
					HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);

					auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
					if (!peer || !peer->getEntityPlayer())
					{
						if (response.code != 1) {
							ManorRequest::trade(platformUserId, exchangePrice);
						}
						return;
					}

					SCRIPT_EVENT::PlayerUpdateManorResultEvent::invoke(peer->getRakssid(), response.code == 1, response.data);
					if (response.code == 1)
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_SUCCESS, "pay success");
					}
					else
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::COMMON, LanguageKey::GUI_MANOR_UPGRADE_FAILURE);
						//Refund for payment failure
						ManorRequest::trade(platformUserId, exchangePrice);
					}
				});
			}
			else
			{
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer) {
					ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
				}
			}
		});
	}
}

void ClientPeer::buyHouse(i32 houseId)
{
	String url = getHttpUrl();
	String dataServerUrl = Server::Instance()->getDataServerUrl();
	String manorId = getEntityPlayer()->m_manor->getManorId();
	if (dataServerUrl.length() == 0 || manorId.length() == 0)
	{
		LordLogError("buy house  pay error: dataServerUrl is null or manorId is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}
	//TODO
	ManorHouse* info = getEntityPlayer()->m_manor->findHouseById(houseId);
	if (info && checkParam(url, (i32)info->currencyType, info->price) )
	{
		i32 currencyType = (i32)info->currencyType;
		i32 price = info->price;
		i32 itemId = info->id;
		i32 charm = info->charm;
		i64 platformUserId = m_platformUserId;

		doManorPay(url, dataServerUrl, currencyType, itemId, price, m_platformUserId,
			[price, currencyType, platformUserId, dataServerUrl, manorId, itemId, charm](String result, bool isSuccess)
		{
			bool paySuccess = false;
			i32  exchangePrice = price;
			if (currencyType <= 2)
			{
				IntProxy rate;
				rate.value = price;
				SCRIPT_EVENT::GetMoneyExchangeRateEvent::invoke(&rate);
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer && peer->getEntityPlayer())
				{
					paySuccess = peer->payResult(platformUserId, result, isSuccess);
				}

				exchangePrice = rate.value;
			}
			else
			{
				paySuccess = ManorRequest::manorPayResult(platformUserId, result, isSuccess);
			}

			if (paySuccess)
			{
				
				ManorRequest::buyHouse(dataServerUrl, manorId, itemId, exchangePrice, charm, 
					[platformUserId, manorId, exchangePrice, paySuccess, itemId, price, charm](String result, bool isSuccess)
				{
					String logTag = StringUtil::Format("buyHouse(UserId=%lld , manorId[%s])", platformUserId, manorId.c_str());
					HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
					auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
					if (!peer || !peer->getEntityPlayer())
					{
						if (response.code != 1) {
							ManorRequest::trade(platformUserId, exchangePrice);
						}
						return;
					}
					if (response.code == 1)
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_SUCCESS, "pay success");
						SCRIPT_EVENT::PlayerBuyHouseResultEvent::invoke(peer->getRakssid(), paySuccess, itemId, price, charm);
					}
					else
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
						//Refund for payment failure
						ManorRequest::trade(platformUserId, exchangePrice);
					}
				});
			}
			else
			{
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer) {
					ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
				}
			}
		});
	}
}

void ClientPeer::buyFurniture(i32 furnitureId)
{
	String url = getHttpUrl();
	String dataServerUrl = Server::Instance()->getDataServerUrl();
	String manorId = getEntityPlayer()->m_manor->getManorId();
	if (dataServerUrl.length() == 0 || manorId.length() == 0)
	{
		LordLogError("buy furniture  pay error: dataServerUrl is null or manorId is null");
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
		return;
	}

	ManorFurniture* info = getEntityPlayer()->m_manor->findFurnitureById(furnitureId);
	
	if (info && checkParam(url, (i32)info->currencyType, info->price))
	{
		i32 currencyType = (i32)info->currencyType;
		i32 price = info->price;
		i32 itemId = info->id;
		i32 charm = info->charm;
		i64 platformUserId = m_platformUserId;

		doManorPay(url, dataServerUrl, currencyType, itemId, price, m_platformUserId,
			[platformUserId, price, currencyType, dataServerUrl, manorId, itemId, charm](String result, bool isSuccess)
		{
			bool paySuccess = false;
			i32  exchangePrice = price;
			if (currencyType <= 2)
			{
				IntProxy rate;
				rate.value = price;
				SCRIPT_EVENT::GetMoneyExchangeRateEvent::invoke(&rate);
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer && peer->getEntityPlayer()) {
					paySuccess = peer->payResult(platformUserId, result, isSuccess);
				}
				exchangePrice = rate.value;
			}
			else
			{
				paySuccess = ManorRequest::manorPayResult(platformUserId, result, isSuccess);
			}

			if (paySuccess)
			{
				ManorRequest::buyFurniture(dataServerUrl, manorId, itemId, exchangePrice, charm, 
					[platformUserId, manorId, paySuccess, itemId, price, exchangePrice](String result, bool isSuccess)
				{
					String logTag = StringUtil::Format("buyFurniture(UserId=%lld , manorId[%s])", platformUserId, manorId.c_str());
					HttpResponse  response = ManorRequest::parseResponse(isSuccess, logTag, result);
					auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
					if (!peer || !peer->getEntityPlayer()) {
						if (response.code != 1) {
							ManorRequest::trade(platformUserId, exchangePrice);
						}
						return;
					}
					if (response.code == 1)
					{
						SCRIPT_EVENT::PlayerBuyFurnitureResultEvent::invoke(peer->getRakssid(), paySuccess, itemId, price);
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_SUCCESS, "pay success");
					}
					else
					{
						ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
						//Refund for payment failure
						ManorRequest::trade(platformUserId, exchangePrice);
					}
				});
			}
			else
			{
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (peer) {
					ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
				}
			}
		});
	}
}

void ClientPeer::consumeDiamonds(i32 uniqueId, i32 diamonds, String remark, bool isConsume)
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(getRakssid(), false, "url.nil", remark, "");
		return;
	}
	Wallet* wallet = getEntityPlayer()->m_wallet;
	if (!wallet)
	{
		SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(getRakssid(), false, "wallet.nil", remark, "");
		return;
	}
	ui64 money = wallet->getMoneyByCoinId(1);
	if (money < diamonds)
	{
		SCRIPT_EVENT::ConsumeDiamondsEvent::invoke(getRakssid(), false, "diamonds.not.enough", remark, "");
		return;
	}

	doConsumeDiamonds(uniqueId, diamonds, remark, isConsume, url);
}

void ClientPeer::consumeGolds(i32 uniqueId, i32 golds, String remark, bool isConsume)
{
	String url = getHttpUrl();
	if (url.length() == 0)
	{
		SCRIPT_EVENT::ConsumeGoldsEvent::invoke(getRakssid(), false, "url.nil", remark, "");
		return;
	}
	Wallet* wallet = getEntityPlayer()->m_wallet;
	if (!wallet)
	{
		SCRIPT_EVENT::ConsumeGoldsEvent::invoke(getRakssid(), false, "wallet.nil", remark, "");
		return;
	}
	ui64 money = wallet->getMoneyByCoinId(2);
	if (money < golds)
	{
		SCRIPT_EVENT::ConsumeGoldsEvent::invoke(getRakssid(), false, "golds.not.enough", remark, "");
		return;
	}

	doPay(2, uniqueId, golds,
		[isConsume, remark](i64 platformUserId, String result, bool isSuccess)
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}
		//bool paySuccess = payResult(result, isSuccess);
		OrderPayResult payResult = peer->payResultByConsume(platformUserId, isSuccess, isConsume, result);
		if (payResult.code == 1)
		{
			SCRIPT_EVENT::ConsumeGoldsEvent::invoke(peer->getRakssid(), true, "pay.success", remark, payResult.orderId);
		}
		else
		{
			LordLogError("ClientPeer::ConsumeGoldsEvent : pay fail, code =  %d , remark = %s ", payResult.code, remark.c_str());
			SCRIPT_EVENT::ConsumeGoldsEvent::invoke(peer->getRakssid(), false, "pay.failed", remark, "");
		}
	}, true);
}

void ClientPeer::buyRanchItem(i32 itemId, i32 addNum, i32 price, i32 currencyType)
{
	String url = getHttpUrl();
	

	BoolProxy canSave;
	canSave.value = false;

	IntProxy totalPrice;
	totalPrice.value = 0;

	std::vector<RanchCommon> items = std::vector<RanchCommon>();
	items.push_back(RanchCommon(itemId, addNum, 0));

	SCRIPT_EVENT::CanRanchStorageSaveItemsEvent::invoke(getRakssid(), items, currencyType, &totalPrice, &canSave);

	if (canSave.value == false)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), itemId, ResultCode::RANCH_STORAGE_FULL, "gui_ranch_storage_full_operation_failure");
		return;
	}

	bool check = checkParam(url, currencyType, totalPrice.value);
	if (!check)
	{
		return;
	}

	if (currencyType < 3)
	{
		doPay(currencyType, 1031010, totalPrice.value,
			[items, itemId](i64 platformUserId, String result, bool isSuccess)
		{
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}
			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerBuyRanchItemResultEvent::invoke(peer->getRakssid(), paySuccess, items);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("buy ranch Item [%d] pay failure", itemId);
			}
		}, false);
	}
	else 
	{
		getEntityPlayer()->subCurrency(totalPrice.value);
		SCRIPT_EVENT::PlayerBuyRanchItemResultEvent::invoke(getRakssid(), true, items);
	}
}

void ClientPeer::buyRanchBuildItem(i32 itemId, i32 addNum, i32 type, i32 currencyType)
{
	String url = getHttpUrl();

	IntProxy totalPrice;
	totalPrice.value = 0;

	IntProxy moneyType;
	moneyType.value = 3;

	BoolProxy isSuccess;
	isSuccess.value = false;

	SCRIPT_EVENT::GetRanchBuildItemCostEvent::invoke(getRakssid(), type, itemId, addNum, &isSuccess, &moneyType, &totalPrice);

	if (isSuccess.value == false)
	{
		return;
	}

	bool check = checkParam(url, moneyType.value, totalPrice.value);
	if (!check)
	{
		return;
	}

	if (moneyType.value < 3)
	{
		doPay(moneyType.value, 1031012, totalPrice.value,
			[type, itemId, addNum](i64 platformUserId, String result, bool isSuccess)
		{
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerBuyRanchBuildItemResultEvent::invoke(peer->getRakssid(), type, paySuccess, itemId, addNum);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), itemId, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("buy ranch Item [%d] pay failure", itemId);
			}
		}, false);
	}
	else
	{
		getEntityPlayer()->subCurrency(totalPrice.value);
		SCRIPT_EVENT::PlayerBuyRanchBuildItemResultEvent::invoke(getRakssid(), type, true, itemId, addNum);
	}
	
}

void ClientPeer::ranchShortcut(i32 landCode , i32 currencyType, i32 totalPrice)
{
	String url = getHttpUrl();
	bool check = checkParam(url, currencyType, totalPrice);
	if (!check)
	{
		return;
	}
	if (currencyType < 3)
	{
		doPay(currencyType, 1031009, totalPrice,
			[landCode](i64 platformUserId, String result, bool isSuccess) {
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerRanchShortcutEvent::invoke(peer->getRakssid(), paySuccess, landCode);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), landCode, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("ranch short cut landCode[%d] pay failure", landCode);
			}
		}, false);
	}
	else
	{
		getEntityPlayer()->subCurrency(totalPrice);
		SCRIPT_EVENT::PlayerRanchShortcutEvent::invoke(getRakssid(), true, landCode);
	}
}

void ClientPeer::buyRanchItems(vector<RanchCommon>::type items)
{
	i32 currencyType = 1;
	
	String url = getHttpUrl();


	BoolProxy canSave;
	canSave.value = false;

	IntProxy totalPrice;
	totalPrice.value = 0;

	std::vector<RanchCommon> ranchCommonItems = std::vector<RanchCommon>();
	for (auto item : items)
	{
		ranchCommonItems.push_back(item);
	}

	SCRIPT_EVENT::CanRanchStorageSaveItemsEvent::invoke(getRakssid(), ranchCommonItems, currencyType, &totalPrice, &canSave);

	if (canSave.value == false)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(getRakssid(), 1, ResultCode::RANCH_STORAGE_FULL, "gui_ranch_storage_full_operation_failure");
		return;
	}

	bool check = checkParam(url, currencyType, totalPrice.value);
	if (!check)
	{
		return;
	}

	if (currencyType < 3)
	{
		doPay(currencyType, 1031006, totalPrice.value,
			[ranchCommonItems](i64 platformUserId, String result, bool isSuccess) {
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerBuyRanchItemResultEvent::invoke(peer->getRakssid(), paySuccess, ranchCommonItems);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 1, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("buy ranch Items pay failure");
			}
		}, false);
	}
	else
	{
		getEntityPlayer()->subCurrency(totalPrice.value);
		SCRIPT_EVENT::PlayerBuyRanchItemResultEvent::invoke(getRakssid(), true, ranchCommonItems);
	}

}

void ClientPeer::ranchBuildQueueOperation(i32 entityId, i32 type, i32 queueId, i32 productId)
{	
	String url = getHttpUrl();

	if (type == 1)
	{
		// production
		SCRIPT_EVENT::PlayerRanchBuildingQueueProductEvent::invoke(getRakssid(), entityId, queueId, productId);
		return;
	}

	if (type == 2)
	{
		//speed up
		IntProxy money;
		money.value = 0;

		BoolProxy isSuccess;
		isSuccess.value = false;

		SCRIPT_EVENT::GetRanchBuildingSpeedUpCostEvent::invoke(getRakssid(), entityId, queueId, productId, &isSuccess, &money);

		if (isSuccess.value == false)
		{
			return;
		}

		if (money.value == 0)
		{
			SCRIPT_EVENT::PlayerRanchBuildingQueueSpeedUpEvent::invoke(getRakssid(), entityId, queueId, productId, true);
			return;
		}

		bool check = checkParam(url, 1, money.value);
		if (!check)
		{
			return;
		}

		doPay(1, 1031011, money.value,
			[entityId, queueId, productId](i64 platformUserId, String result, bool isSuccess) 
		{
			auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
			if (!peer || !peer->getEntityPlayer())
			{
				return;
			}

			bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
			SCRIPT_EVENT::PlayerRanchBuildingQueueSpeedUpEvent::invoke(peer->getRakssid(), entityId, queueId, productId, paySuccess);
			if (!paySuccess)
			{
				ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 1, ResultCode::PAY_FAILURE, "pay failure");
				LordLogInfo("ranch building speed up failure");
			}
		}, false);
		return;
	}

	if (type == 3)
	{
		// unlockQueue

		IntProxy totalPrice;
		totalPrice.value = 0;

		IntProxy moneyType;
		moneyType.value = 3;

		BoolProxy isSuccess;
		isSuccess.value = false;

		SCRIPT_EVENT::GetRanchBuildQueueUnlockCostEvent::invoke(getRakssid(), entityId, queueId, &isSuccess, &moneyType, &totalPrice);

		if (isSuccess.value == false)
		{
			return;
		}

		bool check = checkParam(url, moneyType.value, totalPrice.value);
		if (!check)
		{
			return;
		}

		if (moneyType.value < 3)
		{
			doPay(moneyType.value, 1031008, totalPrice.value,
				[entityId, queueId, productId](i64 platformUserId, String result, bool isSuccess)
			{
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				if (!peer || !peer->getEntityPlayer())
				{
					return;
				}

				bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
				SCRIPT_EVENT::PlayerRanchBuildingQueueUnlockEvent::invoke(peer->getRakssid(), entityId, queueId, productId, paySuccess);
				if (!paySuccess)
				{
					ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 1, ResultCode::PAY_FAILURE, "pay failure");
					LordLogInfo("ranch building queue unlock cost failure");
				}
			}, false);
		}
		else
		{
			getEntityPlayer()->subCurrency(totalPrice.value);
			SCRIPT_EVENT::PlayerRanchBuildingQueueUnlockEvent::invoke(getRakssid(), entityId, queueId, productId, true);
		}
	}

	if (type == 4)
	{
		//building product speed up 
		SCRIPT_EVENT::PlayerRanchBuildingProductionSpeedUpEvent::invoke(getRakssid(), entityId, queueId, productId);
	}
}

void ClientPeer::speedUpCrop(Vector3i blockPos, i32 blockId, i32 totalPrice)
{
	String url = getHttpUrl();
	bool check = checkParam(url, 1, totalPrice);
	if (!check)
	{
		return;
	}

	doPay(1, 1031005, totalPrice,
		[blockId, blockPos](i64 platformUserId, String result, bool isSuccess) 
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}

		bool paySuccess = peer->payResult(platformUserId, result, isSuccess);
		
		if (!paySuccess)
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), blockId, ResultCode::PAY_FAILURE, "pay failure");
			LordLogInfo("ranch speedUpCrop [%d] pay failure", blockId);
		}
		else 
		{
			BlockPos bos = BlockPos(blockPos.x, blockPos.y, blockPos.z);
			Server::Instance()->getBlockCropsManager()->speedUpCrop(platformUserId, bos);
		}
	}, false);
}

void ClientPeer::useCubeFinshOrder(i32 orderId, i32 index)
{
	IntProxy money;
	money.value = 0;

	BoolProxy isSuccess;
	isSuccess.value = false;

	SCRIPT_EVENT::GetRanchTaskItemsCostEvent::invoke(getRakssid(), orderId, index, &isSuccess, &money);

	if (isSuccess.value == false)
	{
		return;
	}

	String url = getHttpUrl();
	bool check = checkParam(url, 1, money.value);
	if (!check)
	{
		return;
	}

	doPay(1, 1031007, money.value,
		[orderId, index](i64 platformUserId, String result, bool isSuccess) 
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}

		OrderPayResult payResult = peer->payResultByConsume(platformUserId, isSuccess, false, result);

		if (payResult.code != 1)
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), orderId, ResultCode::PAY_FAILURE, "pay failure");
			LordLogInfo("useCubeFinshOrder [%d] pay failure", orderId);
		}
		else
		{
			SCRIPT_EVENT::PlayerBuyRanchTaskItemsResultEvent::invoke(peer->getRakssid(), orderId, index, payResult.orderId);
		}
	}, false);
}

void ClientPeer::buyQuicklyBuildBlock(i32 cost)
{
	if (!m_entityPlayer || m_entityPlayer->m_canBuildBlockQuickly)
	{
		return;
	}

	String url = getHttpUrl();
	bool check = checkParam(url, 1, cost);
	if (!check)
	{
		return;
	}

	doPay(1, 300, cost,
		[this](i64 platformUserId, String result, bool isSuccess)
	{
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}

		OrderPayResult payResult = peer->payResultByConsume(platformUserId, isSuccess, false, result);

		if (payResult.code == 1)
		{
			m_entityPlayer->m_canBuildBlockQuickly = true;
			ServerNetwork::Instance()->getSender()->sendChangeQuicklyBuildBlockStatus(peer->getRakssid(), true, true);			
		}
		else
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(peer->getRakssid(), 0, ResultCode::PAY_FAILURE, "pay failure");
			LordLogInfo("buyQuicklyBuildBlock pay failure");
		}
	}, false);
}

void ClientPeer::updateUserSelectableAction()
{
	if (!m_entityPlayer)
	{
		return;
	}

	String url = getHttpUrl();
	if (url.length() == 0)
	{
		LordLogError("update user selectable action error: url is null");
		return;
	}

	HttpRequest::getUserAction(url, m_platformUserId,
		[](i64 platformUserId, String result) {
		auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (!peer || !peer->getEntityPlayer())
		{
			return;
		}
		Document* doc = new Document;
		doc->Parse(result.c_str());
		auto player = peer->getEntityPlayer();
		if (doc->HasParseError())
		{
			delete doc;
			doc = nullptr;
			return;
		}
		if (doc->HasMember("code"))
		{
			int code = doc->FindMember("code")->value.GetInt();
			if (code != 1)
			{
				delete doc;
				doc = nullptr;
				return;
			}
			if (doc->HasMember("data"))
			{
				auto data = doc->FindMember("data")->value.GetObject();
				if (data.HasMember("selectable_action") && !data.FindMember("selectable_action")->value.IsNull())
				{
					String selectableActionsStr = data.FindMember("selectable_action")->value.GetString();
					StringArray arry = StringUtil::Split(selectableActionsStr, "-");
					vector<int>::type selectableActions;
					for (int i = 0; i < arry.size(); ++i)
					{
						selectableActions.push_back(std::stoi(arry[i].c_str()));
					}
					if (selectableActions.size() < player->m_actionLockedInfoMap.size())
					{
						delete doc;
						doc = nullptr;
						LordLogError("HttpRequest_getUserAction: action amount is less and less after buying");
						return;
					}
					else
					{
						player->m_actionLockedInfoMap.clear();
						for (int i = 0; i < selectableActions.size(); ++i)
						{
							player->m_actionLockedInfoMap[selectableActions[i]] = false;
						}
						Server::Instance()->getRoomManager()->setUserAttrInfoUnlockedSelectableActions(platformUserId, selectableActions);
						ServerNetwork::Instance()->getSender()->sendUpdateSelectableAction(peer->getRakssid(), selectableActions);
					}
				}
			}
		}
		delete doc;
		doc = nullptr;
	});
}

void ClientPeer::tryAskVehicle(int entityId, bool onOrOff, bool isAdReward)
{
	auto player = getEntityPlayer();
	if (!player)
		return;

	auto vehicle = dynamic_cast<EntityVehicle*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (!vehicle)
		return;

	if (onOrOff)
	{
		if (player->getOnVehicle(vehicle, isAdReward, !vehicle->hasDriver()))
		{
			SCRIPT_EVENT::PlayerTakeOnOrOffVehicleEvent::invoke(getRakssid(), entityId, onOrOff);
		}
	}
	else
	{
		player->getOffVehicle();
		player->setPosition(vehicle->position + Vector3(0, 0.5f, 0));
		Vector3 pos = player->position;
		setPlayerLocation(pos, player->rotationYaw, player->rotationPitch);
		SCRIPT_EVENT::PlayerTakeOnOrOffVehicleEvent::invoke(getRakssid(), entityId, onOrOff);
	}
}

void ClientPeer::doCacheRequest()
{
	if (m_requestCache.size() == 0)
	{
		return;
	}
	switch (m_interruptType)
	{
	case InterruptType_doPay:
	{
		i32 currencyType = m_requestCache.get<i32>(0);
		i32 uniqueId = m_requestCache.get<i32>(1);
		i32 price = m_requestCache.get<i32>(2);
		std::function<void(i64, String, bool)> fun = m_requestCache.get<std::function<void(i64, String, bool)>>(3);
		HttpRequest::pay(getHttpUrl(), currencyType, uniqueId, price, m_platformUserId, fun);
	}
	break;
	case InterruptType_doBuyGoods:
	{
		std::function<void()> fun = m_requestCache.get<std::function<void()>>(0);
		fun();
	}
	break;
	case InterruptType_doManorPay:
	{
		String webPayUrl = m_requestCache.get<String>(0);
		String dataServerPayUrl = m_requestCache.get<String>(1);
		i32 currencyType = m_requestCache.get<i32>(2);
		i32 uniqueId = m_requestCache.get<i32>(3);
		i32 price = m_requestCache.get<i32>(4);
		ui64 platformUserId = m_requestCache.get<ui64>(5);
		std::function<void(String, bool)> fun = m_requestCache.get<std::function<void(String, bool)>>(6);
		ManorRequest::pay(webPayUrl, dataServerPayUrl, currencyType, uniqueId, price, platformUserId, fun);
	}
	break;
	default:
		break;
	}
	m_requestCache.clear();
}

String ClientPeer::getLogString() const
{
	LORD::StringStream ss;
	ss << "name:" << m_name << ", session id:" << m_rakSsid << ", platform id:" << m_platformUserId;
	return ss.str();
}

bool ClientPeer::payResult(i64 platformUserId, String result, bool isSuccess)
{
	OrderPayResult r = payResultByConsume(platformUserId, isSuccess, true, result);
	return r.code == 1;
}

ClientPeer::OrderPayResult ClientPeer::payResultByConsume(i64 platformUserId, bool isSuccess, bool isConsume, String result)
{
	OrderPayResult resultInfo = OrderPayResult();
	resultInfo.isConsume = isConsume;
	if (isSuccess)
	{
		Document* doc = new Document;
		doc->Parse(result.c_str());
		if (doc->HasParseError())
		{
			resultInfo.code = 0;
		}
		if (doc->HasMember("code"))
		{
			resultInfo.code = doc->FindMember("code")->value.GetInt();
			if (resultInfo.code == 1 && doc->HasMember("data"))
			{
				// called in aysnc callback, need to check peer and player
				auto peer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
				auto data = doc->FindMember("data")->value.GetObject();
				if (peer && peer->getEntityPlayer())
				{
					auto player = this->getEntityPlayer();
					i64 diamondBlues = 0;
					i64 diamondGolds = 0;
					i64 golds = 0;
					if (data.HasMember(DIAMOND_BLUES))
					{
						diamondBlues = data.FindMember(DIAMOND_BLUES)->value.GetInt64();
						player->m_wallet->m_diamondBlues = diamondBlues;
					}
					if (data.HasMember(DIAMOND_GOLDS))
					{
						diamondGolds = data.FindMember(DIAMOND_GOLDS)->value.GetInt64();
						player->m_wallet->m_diamondGolds = diamondGolds;
					}
					if (data.HasMember("golds"))
					{
						golds = data.FindMember("golds")->value.GetInt64();
						player->m_wallet->setGolds(golds);
					}
					ServerNetwork::Instance()->getSender()->sendInitMoney(getRakssid(), diamondBlues, diamondGolds, golds);
				}
				if (data.HasMember("orderId"))
				{
					resultInfo.orderId = data.FindMember("orderId")->value.GetString();
					String url = getHttpUrl();
					if (url.length() == 0)
					{
						LordLogError("resume order error: url is null");
					}

					if (isConsume)
					{
						HttpRequest::resumeOrder(url, resultInfo.orderId);
					}
				}
			}
		}
		delete doc;
		doc = nullptr;
	}
	return resultInfo;
}

void ClientPeer::updateModuleBlockData(String data)
{
	if (data.empty())
	{
		return;
	}

	StringUtil::SplitString(m_mapModuleBlockData, data.c_str());

	auto sendPk = LORD::make_shared<S2CPacketSyncClientPeerProp>();
	sendPk->propertyList = data;
	Server::Instance()->getNetwork()->sendPacket(sendPk, getRakssid());
}

String ClientPeer::getHttpUrl()
{
	RoomGameConfig config = Server::Instance()->getConfig();
	return config.blockymodsUrl.c_str();
}

bool ClientPeer::isPioneer()
{
	return true;
}

void ClientPeer::stopLaserGun()
{
	Server::Instance()->getNetwork()->getSender()->broadCastStopLaserGun(m_entityPlayer->entityId);
}
