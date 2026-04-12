#include "ServerPacketSender.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Blockman/Entity/EntityTracker.h"
#include "Chat/ChatComponent.h"
#include "Script/GameServerEvents.h"
#include "Setting/MultiLanTipSetting.h"
#include "Entity/EntityActorCannon.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Network/protocol/S2CPackets2.h"
#include "Entity/TeleportPositionConverter.h"
#include "Util/PlayerPrefs.h"

using namespace BLOCKMAN;

#pragma region

void ServerPacketSender::sendDeathUnequipArmor(i32 entityId)
{
	auto packet = LORD::make_shared<S2CPacketDeathUnequipArmor>();
	packet->entityId = entityId;
	EntityPlayerMP* player = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(entityId));
	if (player)
	{
		player->inventory->clearPrev();
	}
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChangeEntityPerspece(ui64 rakssId, int View)
{
	auto packet = LORD::make_shared<S2CPacketSyncChangeEntityPerspece>();
	packet->View = View;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerInvisible(ui64 rakssId, i32 entityId, bool isInvisible)
{
	auto packet = LORD::make_shared<S2CPacketPlayerInvisible>();
	packet->isInvisible = isInvisible;
	packet->entityId = entityId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPlayerInvisible(i32 entityId, bool isInvisible)
{
	auto packet = LORD::make_shared<S2CPacketPlayerInvisible>();
	packet->isInvisible = isInvisible;
	packet->entityId = entityId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendPlayerChangeHeart(ui64 rakssId, int hp, int maxHp)
{
	auto packet = LORD::make_shared<S2CPacketChangeHeart>();
	packet->hp = hp;
	packet->maxHp = maxHp;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerBossStripWithIcon(ui64 rakssId, String name, int curProgress, int maxProgress, String iconPath)
{
	auto packet = LORD::make_shared<S2CPacketBuildProgress>();
	packet->name = name;
	packet->curProgress = curProgress;
	packet->maxProgress = maxProgress;
	packet->iconPath = iconPath;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncSessionNpc(ui64 rakssId, BLOCKMAN::EntitySessionNpc * npc)
{
	auto packet = LORD::make_shared<S2CPacketSyncSessionNpc>();
	auto player = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
	ui64 userId = npc->isPerson() && player ? player->getPlatformUserId() : 0;
	packet->entityId = npc->entityId;
	packet->isPerson = npc->isPerson();
	packet->sessionType = npc->getSessionType();
	packet->name = npc->getName();
	packet->timeLeft = npc->getTimeLeftByUser(userId);
	packet->nameLang = npc->getNameLangByUser(userId);
	packet->actorName = npc->getActorNameByUser(userId);
	packet->actorBody = npc->getActorBodyByUser(userId);
	packet->actorAction = npc->getActorActionByUser(userId);
	packet->actorBodyId = npc->getActorBodyIdByUser(userId);
	packet->sessionContent = npc->getSessionContentByUser(userId);
	packet->effectName = npc->getEffectNameByUser(userId);
	packet->isCanCollided = npc->isCanCollidedByUser(userId);
	packet->isSuspended = npc->getSuspended();

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
	else
	{
		sendPacketToTrackingPlayers(npc->entityId, packet);
	}

}

void ServerPacketSender::sendUpdateManor(BLOCKMAN::EntityPlayerMP* player)
{
	Manor* manor = player->m_manor;
	if (manor && manor->getInfo() && manor->getNextInfo())
	{
		auto packet = LORD::make_shared<S2CPacketUpdateManor>();
		packet->m_manorInfo = *manor->getInfo();
		packet->m_nextManorInfo = *manor->getNextInfo();

		for (ManorHouse* item : manor->getHouses())
		{
			packet->m_manorHouses.push_back(*item);
		}

		for (ManorFurniture* item : manor->getFurnitures())
		{
			packet->m_manorFurnitures.push_back(*item);
		}

		for (ManorMessage* item : manor->getMessages())
		{
			packet->m_manorMessages.push_back(*item);
		}

		Server::Instance()->getNetwork()->sendPacket(packet, player->getRaknetID());
	}

}

void ServerPacketSender::sendUpdateRealTimeRankInfo(ui64 rakssId, String result)
{
	auto packet = LORD::make_shared<S2CPacketUpdateRealTimeRankInfo>();
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastGameMonsterInfo(int currCheckPointNum, int surplusMonsterNum, int surplusCheckPointNum)
{
	auto packet = LORD::make_shared<S2CPacketUpdateGameMonsterInfo>();
	packet->currCheckPointNum = currCheckPointNum;
	packet->surplusMonsterNum = surplusMonsterNum;
	packet->surplusCheckPointNum = surplusCheckPointNum;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastBossBloodStrip(int currBloodStrip, int maxBloodStrip)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBossBloodStrip>();
	packet->currBloodStrip = currBloodStrip;
	packet->maxBloodStrip = maxBloodStrip;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSwitchablePropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketSwitchablePropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendUpgradePropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketUpgradePropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPersonalShopTip(ui64 rakssId, String tip)
{
	auto packet = LORD::make_shared<S2CPacketPersonalShopTip>();
	packet->tip = tip;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCreateHouseFromSchematic(const String& fileName, Vector3i start_pos, const LORD::map<PACKET_DETAIL::Vector3i, bool>::type& placeCoverResult, bool xImage, bool zImage,
	bool createOrDestroy, bool coverBlock, ui64 rakssid/* = 0*/, int blockId/* = 0*/, int offset)
{
	auto packet = LORD::make_shared<S2CPacketCreateHouseFromSchematic>();
	packet->start_pos = start_pos;
	packet->fileName = fileName;
	packet->xImage = xImage;
	packet->zImage = zImage;
	packet->createOrDestroy = createOrDestroy;
	packet->coverBlock = coverBlock;
	packet->rakssid = rakssid;
	packet->blockId = blockId;
	packet->offset = offset;
	for (auto p : placeCoverResult)
	{
		packet->placeCoverResultVector3i.emplace_back(p.first);
		packet->placeCoverResultBool.emplace_back(p.second);
	}
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendfillAreaByBlockIdAndMate(Vector3i start_pos, Vector3i end_pos, int block_id, int mate)
{
	auto packet = LORD::make_shared<S2CPacketFillAreaByBlockIdAndMate>();
	packet->start_pos = start_pos;
	packet->end_pos = end_pos;
	packet->block_id = block_id;
	packet->mate = mate;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendShowPlayerOperation(ui64 rakssId, ui64 targetUserId)
{
	auto pree = Server::Instance()->getNetwork()->findPlayerByPlatformUserId(targetUserId);
	auto packet = LORD::make_shared<S2CPacketShowPlayerOperation>();
	packet->m_targetUserId = targetUserId;
	packet->m_targetUserName = pree ? pree->getName() : "";
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendUpdateEntityCreature(i64 rakssId, BLOCKMAN::EntityCreature* creature)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityCreature>();
	packet->entityId = creature->entityId;
	packet->actorName = creature->getActorName();
	packet->creatureEntityType = creature->getMonsterType();
	packet->nameLang = creature->getNameLang();
	packet->keepJumping = creature->m_keepJumping;
	packet->onGround = creature->onGround;
	packet->canFly = creature->canFly;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCreatureEntityAction(ui64 rakssId, int entityId, int actionStatus, int baseAction, int PreBaseAction)
{
	auto packet = LORD::make_shared<S2CPacketSyncCreatureEntityAction>();
	packet->entityId = entityId;
	packet->actionStatus = actionStatus;
	packet->baseAction = baseAction;
	packet->preBaseAction = PreBaseAction;
	if (rakssId == 0)
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendEntitySpeedAdditionLevel(i64 rakssId, int entityId, int speedLevel)
{
	auto packet = LORD::make_shared<S2CPacketEntitySpeedLevel>();
	packet->entityId = entityId;
	packet->speedLevel = speedLevel;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncEntitySpeedAdditionLevel(int entityId, int speedLevel)
{
	auto packet = LORD::make_shared<S2CPacketEntitySpeedLevel>();
	packet->entityId = entityId;
	packet->speedLevel = speedLevel;
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::sendPlayerExpInfo(i64 rakssId, int level, float exp, int maxExp)
{
	auto packet = LORD::make_shared<S2CPacketPlayerExpInfo>();
	packet->level = level;
	packet->exp = exp;
	packet->maxExp = maxExp;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::showUpgradeResourceUI(ui64 rakssId, int costItemId, int costItemNum, int resourceId, String tipStr)
{
	auto packet = LORD::make_shared<S2CPacketShowUpgradeResourceUI>();
	packet->itemId = costItemId;
	packet->itemNum = costItemNum;
	packet->resourceId = resourceId;
	packet->tipString = tipStr;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendManorBtnVisible(ui64 rakssId, bool visible)
{
	auto packet = LORD::make_shared<S2CPacketManorBtnVisible>();
	packet->visible = visible;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerInventory(int entityId, bool bForce/* = false*/)
{
	EntityPlayerMP* player = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(entityId));
	if (player)
	{
		sendPlayerInventory(player->getRaknetID(), player->getInventory(), bForce);
	}
}

void ServerPacketSender::sendCallOnManorResetClient(ui64 rakssId, ui64 targetUserId)
{
	auto packet = LORD::make_shared<S2CPacketCallOnManorResetClient>();
	packet->m_targetUserId = targetUserId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendUpdateManorOwner(ui64 rakssid)
{
	auto packet = LORD::make_shared<S2CPacketUpdateManorOwner>();
	auto world = Server::Instance()->getWorld();
	for (auto owner : world->m_manorControl->getOwners())
	{
		packet->m_owners.push_back(owner);
	}
	if (rakssid > 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
		return;
	}
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastBasementLife(float curLife, float maxLife)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBasementLife>();
	packet->curLife = curLife;
	packet->maxLife = maxLife;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastPlayerMovementInput(int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerMovementInput>();
	packet->playerEntityId = playerEntityId;
	packet->forward = pPlayer->movementInput->moveForward;
	packet->strafe = pPlayer->movementInput->moveStrafe;
	sendPacketToTrackingPlayers(playerEntityId, packet, false);
}

void ServerPacketSender::broadCastPlayerRotation(int playerEntityId, bool includeSelf)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerRotation>();
	packet->playerEntityId = playerEntityId;
	float pitch = Math::Clamp(Math::WrapDegree(pPlayer->rotationPitch), -90.f, 90.f);
	unsigned char fixedLengthPitch = static_cast<unsigned char>((pitch + 90) / 180 * 255);
	packet->rotationPitch = fixedLengthPitch;
	float yaw = Math::WrapDegree(pPlayer->rotationYaw);
	unsigned char fixedLengthYaw = static_cast<unsigned char>((yaw + 180) / 360 * 256);
	packet->rotationYaw = fixedLengthYaw;
	sendPacketToTrackingPlayers(playerEntityId, packet, includeSelf);
}

void ServerPacketSender::broadCastPlayerMovement(int playerEntityId, bool includeSelf)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;
	if (includeSelf) {
		sendPlayerTeleport(pPlayer, pPlayer->getRaknetID());
	}

	const auto& targetPlayers = EntityTracker::Instance()->getTrackingPlayersOf(playerEntityId);
	for (const auto& pair : targetPlayers)
	{
		sendPlayerTeleport(pPlayer, dynamic_cast<EntityPlayerMP*>(pair.second)->getRaknetID());
	}
}

void ServerPacketSender::broadCastPlayerJumpChanged(int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerJumpChanged>();
	packet->playerEntityId = playerEntityId;
	packet->isJump = pPlayer->movementInput->jump;
	sendPacketToTrackingPlayers(playerEntityId, packet, false);
}

void ServerPacketSender::broadCastPlayerSprintChanged(int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerSprintChanged>();
	packet->playerEntityId = playerEntityId;
	packet->isSprintInput = pPlayer->movementInput->isSprintInput;
	sendPacketToTrackingPlayers(playerEntityId, packet, false);
}


void ServerPacketSender::syncEntityPlayerMovementInput(ui64 rakssId, int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerMovementInput>();
	packet->playerEntityId = playerEntityId;
	packet->forward = pPlayer->movementInput->moveForward;
	packet->strafe = pPlayer->movementInput->moveStrafe;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPlayerDownChanged(int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerDownChanged>();
	packet->playerEntityId = playerEntityId;
	packet->isDown = pPlayer->movementInput->down;
	sendPacketToTrackingPlayers(playerEntityId, packet, false);
}

void ServerPacketSender::broadCastPlayerSetFlyingState(int playerEntityId, bool isFlying)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerSetFlyingState>();
	packet->playerEntityId = playerEntityId;
	packet->isFlying = isFlying;
	sendPacketToTrackingPlayers(playerEntityId, packet);
}

void ServerPacketSender::sendGameTipNotification(ui64 rakssid, int msgType, ChatComponentNotification * notification)
{
	auto packet = LORD::make_shared<S2CPacketGameTipNotification>();
	packet->m_tipType = msgType;
	packet->m_chatComponent.reset(notification);

	if (rakssid == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
	}
}

void ServerPacketSender::sendPlayerTeleport(const EntityPlayerMP* player, ui64 targetRaknetId)
{
	auto targetPlayer = ServerNetwork::Instance()->findPlayerByRakssid(targetRaknetId);
	if (!targetPlayer || !targetPlayer->hasLogon())
		return;

	auto packet = LORD::make_shared<S2CPacketEntityPlayerTeleport>();
	auto relativePos = TeleportPositionConverter::ConvertToRelativeAndUpdateAnchor(
		player->entityId, targetRaknetId, player->position);
	packet->m_entityId = player->entityId;
	packet->m_x = relativePos.x;
	packet->m_y = relativePos.y;
	packet->m_z = relativePos.z;
	Server::Instance()->getNetwork()->sendPacket(packet, targetRaknetId);
}

void ServerPacketSender::syncPlayerMovement(ui64 rakssId, int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;
	sendPlayerTeleport(pPlayer, rakssId);
}

void ServerPacketSender::syncPlayerDownChanged(ui64 rakssId, int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerDownChanged>();
	packet->playerEntityId = playerEntityId;
	packet->isDown = pPlayer->movementInput->down;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncPlayerJumpChanged(ui64 rakssId, int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerJumpChanged>();
	packet->playerEntityId = playerEntityId;
	packet->isJump = pPlayer->movementInput->jump;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCustomTipMsg(ui64 rakssId, String messageLang, String extra)
{
	auto packet = LORD::make_shared<S2CPacketCustomTipMsg>();
	packet->messageLang = messageLang;
	packet->extra = extra;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendContentTipMsg(ui64 rakssId, String messageLang, String titleLang)
{
	auto packet = LORD::make_shared<S2CPacketContentTipMsg>();
	packet->messageLang = messageLang;
	packet->titleLang = titleLang;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendEntityPlayerOccupation(ui64 rakssId, int entityId, int occupation)
{
	auto packet = LORD::make_shared<S2CPacketEntityPlayerOccupation>();
	packet->occupation = occupation;
	packet->entityId = entityId;
	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
	else
	{
		sendPacketToTrackingPlayers(entityId, packet, true);
	}
}

void ServerPacketSender::sendBuildWarBlockBtn(ui64 rakssId, bool isNeedOpen)
{
	auto packet = LORD::make_shared<S2CPacketShowBuildWarBlockBtn>();
	packet->isNeedOpen = isNeedOpen;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendbroadcastBuildWarPlayNum(ui8 cur_num, ui8 max_num)
{
	auto packet = LORD::make_shared<S2CPacketShowBuildWarPlayNum>();
	packet->cur_num = cur_num;
	packet->max_num = max_num;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendbroadcastBuildWarLeftTime(ui64 rakssId, bool show, ui16 left_time, ui32 theme)
{
	auto packet = LORD::make_shared<S2CPacketShowBuildWarLeftTime>();
	packet->show = show;
	packet->left_time = left_time;

	auto clientPeer = Server::Instance()->getNetwork()->findPlayerByRakssid(rakssId);
	if (clientPeer)
	{
		String lan = clientPeer->getLanguage();
		std::string msg = MultiLanTipSetting::getMessage(lan, theme, "");
		packet->theme = msg.c_str();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendOpenChest(bool show, int entityId, Vector3i pos)
{
	sendCloseContainer(entityId, show, pos);
}

void ServerPacketSender::sendOpenShopByEntityId(ui64 rakssId, int entityId)
{
	auto packet = LORD::make_shared<S2CPacketOpenShopByEntityId>();
	packet->entityId = entityId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendShowBuildGrade(ui64 rakssId, bool openOrClose, bool show, ui32 msg, ui32 grade_msg, ui8 left_time)
{
	auto packet = LORD::make_shared<S2CPacketShowGrade>();
	packet->openOrClose = openOrClose;
	packet->show = show;
	auto clientPeer = Server::Instance()->getNetwork()->findPlayerByRakssid(rakssId);
	if (clientPeer)
	{
		String lan = clientPeer->getLanguage();
		std::string show_msg = MultiLanTipSetting::getMessage(lan, msg, "");

		if (!openOrClose && grade_msg > 0)
		{
			show_msg = MultiLanTipSetting::getMessage(lan, grade_msg, "");
		}

		packet->msg = show_msg.c_str();

		packet->left_time = left_time;
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendShowBuildGuessResult(ui64 rakssId, bool right, int rank)
{
	auto packet = LORD::make_shared<S2CPacketShowGuessResult>();
	packet->right = right;
	packet->rank = rank;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::showBuildGuessUi(ui64 rakssId, int guess_room_id, String info)
{
	auto packet = LORD::make_shared<S2CPacketShowGuessUi>();
	packet->info = info;
	packet->guess_room_id = guess_room_id;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}


void ServerPacketSender::sendGameSettlementExtra(ui64 rakssId, bool guess_right, String guess_name, int guess_reward)
{
	auto packet = LORD::make_shared<S2CPacketGameSettlementExtra>();
	packet->guess_right = guess_right;
	packet->guess_name = guess_name;
	packet->guess_reward = guess_reward;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendUnlockedCommodity(ui64 rakssid, int merchantId, int itemId, int meta)
{
	auto packet = LORD::make_shared<S2CPacketUnlockedCommodity>();
	packet->merchantId = merchantId;
	packet->itemId = itemId;
	packet->meta = meta;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendInitUnlockedCommodity(ui64 rakssId, std::vector<int> unlockedCommodity)
{
	if (unlockedCommodity.size() == 2)
	{
		auto packet = LORD::make_shared<S2CPacketInitUnlockedCommodity>();
		packet->itemId = unlockedCommodity[0];
		packet->meta = unlockedCommodity[1];
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendTeamResourcesUpdate(String data)
{
	auto packet = LORD::make_shared<S2CPacketTeamResourcesUpdate>();
	packet->m_data = data;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendCloseBGM(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketCloseBGM>();
	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendSetArmItem(ui64 rakssId, int itemId)
{
	auto packet = LORD::make_shared<S2CPacketSetArmItem>();
	packet->itemId = itemId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastActorNpcContent(int entityId, String content)
{
	auto packet = LORD::make_shared<S2CPacketActorNpcContent>();
	packet->entityId = entityId;
	packet->content = content;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastBuildProgress(String name, int curProgress, int maxProgress)
{
	auto packet = LORD::make_shared<S2CPacketBuildProgress>();
	packet->name = name;
	packet->curProgress = curProgress;
	packet->maxProgress = maxProgress;
	packet->iconPath = "";
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendEnchantmentPropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketEnchantmentPropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerEnableMovement(int entityId)
{
	auto packet = LORD::make_shared<S2CPacketEnableMovement>();
	packet->entityId = entityId;
	sendPacketToTrackingPlayers(entityId, packet, false);
}

void ServerPacketSender::sendConsumeCoinTip(ui64 rakssId, String message, int coinId, int price, String extra)
{
	auto packet = LORD::make_shared<S2CPacketConsumeCoinTip>();
	packet->message = message;
	packet->coinId = coinId;
	packet->price = price;
	packet->extra = extra;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSuperPropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketSuperPropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCannonActorInfo(i64 rakssId, BLOCKMAN::EntityActorCannon* pCannon)
{
	auto packet = LORD::make_shared<S2CSyncCannonActorInfo>();
	packet->entityId = pCannon->entityId;
	packet->actorName = pCannon->getActorName();
	packet->headName = pCannon->getHeadName();
	packet->skillName = pCannon->getSkillName();
	packet->haloEffectName = pCannon->getHaloEffectName();
	packet->isCanObstruct = pCannon->isCanObstruct();
	packet->content = pCannon->getContent();
	packet->body = pCannon->getBody();
	packet->bodyId = pCannon->getBodyId();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncEntityBlockman(ui64 rakssId, BLOCKMAN::EntityBlockman* blockman)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityBlockmanInfo>();
	packet->entityId = blockman->entityId;
	packet->blockmanType = (int)blockman->getBlockmanType();
	packet->nameLang = blockman->getNameLang();
	packet->actorName = blockman->getActorName();
	packet->actorBody = blockman->getActorBody();
	packet->actorBodyId = blockman->getActorBodyId();
	packet->maxSpeed = blockman->getCurrentMaxSpeed();
	switch (blockman->getBlockmanType()) {
	case EntityBlockmanType::BLOCKMAN_EMPTY:
	{
		auto entity = dynamic_cast<EntityBlockmanEmpty*>(blockman);
		if (entity)
		{
			packet->curSpeed = entity->getCurSpeed();
			packet->addSpeed = entity->getSpeedAddtion();
			packet->isRecordRoute = entity->isNeedRecordRoute();
			packet->longHitTimes = entity->getLongHitTimes();
			entity->setNeedSyncInfo(false);
		}
	}
	break;
	}
	if (rakssId == 0)
		sendPacketToTrackingPlayers(blockman->entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);

}

void ServerPacketSender::syncBlockmanMoveAttr(ui64 rakssId, BLOCKMAN::EntityBlockman* blockman)
{
	auto entity = dynamic_cast<EntityBlockmanEmpty*>(blockman);
	if (entity)
	{
		auto packet = LORD::make_shared<S2CPacketBlockmanMoveAttr>();
		packet->entityId = entity->entityId;
		packet->pushX = entity->pushX;
		packet->pushZ = entity->pushZ;
		packet->position = entity->position;
		packet->motion = entity->motion;
		packet->yaw = entity->rotationYaw;
		if (rakssId == 0)
			sendPacketToTrackingPlayers(blockman->entityId, packet, true);
		else
			Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendNotification(ui64 rakssid, std::shared_ptr<ChatComponentNotification> chatNotification)
{
	auto packet = LORD::make_shared<S2CPacketChatNotification>();
	packet->m_chatType = 0;
	packet->m_chatComponent = chatNotification;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendNotification(std::shared_ptr<ChatComponentNotification> chatNotification)
{
	auto packet = LORD::make_shared<S2CPacketChatNotification>();
	packet->m_chatType = 0;
	packet->m_chatComponent = chatNotification;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSystemsChat(String key, String playerName, int chatType)
{
	if (!PlayerPrefs::Instance()->getBoolPrefs("SendSystemsChat"))
	{
		return;
	}

	auto packet = LORD::make_shared<S2CPacketChatTranslation>();
	auto chatComponentTranslation = LORD::make_shared<ChatComponentTranslation>();
	FormatArgs args = FormatArgs(playerName);
	chatComponentTranslation->key = key;
	chatComponentTranslation->formatArgs.push_back(args);
	packet->m_chatType = chatType;
	packet->m_chatComponent = chatComponentTranslation;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChat(std::shared_ptr<ChatComponentString> chatString)
{
	auto packet = LORD::make_shared<S2CPacketChatString>();
	packet->m_chatType = 1;
	packet->m_chatComponent = chatString;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChat(ui64 targetRaknetID, std::shared_ptr<ChatComponentString> chatString)
{
	auto packet = LORD::make_shared<S2CPacketChatString>();
	packet->m_chatType = 1;
	packet->m_chatComponent = chatString;
	Server::Instance()->getNetwork()->sendPacket(packet, targetRaknetID);
}

void ServerPacketSender::sendChat(std::shared_ptr<ChatComponentTranslation> chatTranslation)
{
	auto packet = LORD::make_shared<S2CPacketChatTranslation>();
	packet->m_chatType = 0;
	packet->m_chatComponent = chatTranslation;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChat(ui64 targetRaknetID, std::shared_ptr<ChatComponentTranslation> chatTranslation)
{
	auto packet = LORD::make_shared<S2CPacketChatTranslation>();
	packet->m_chatType = 0;
	packet->m_chatComponent = chatTranslation;
	Server::Instance()->getNetwork()->sendPacket(packet, targetRaknetID);
}

void ServerPacketSender::sendPlayerChangeDefense(ui64 rakssId, int defense, int maxDefense)
{
	auto packet = LORD::make_shared<S2CPacketChangeDefense>();
	packet->defense = defense;
	packet->maxDefense = maxDefense;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerChangeAttack(ui64 rakssId, int attack, int maxAttack)
{
	auto packet = LORD::make_shared<S2CPacketChangeAttack>();
	packet->attack = attack;
	packet->maxAttack = maxAttack;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPlayerSpecialJump(int playerEntityId, float yFactor, float xFactor, float zFactor)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerSpecialJump>();
	packet->playerEntityId = playerEntityId;
	packet->yFactor = yFactor;
	packet->xFactor = xFactor;
	packet->zFactor = zFactor;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendEntityAnimateToTackingPlayers(NETWORK_DEFINE::PacketAnimateType animateType, int entityId, int holdItemId,
	int32_t value/* = 0*/, bool includeSelf/* = false*/)
{
	auto animatePacket = LORD::make_shared<S2CPacketAnimate>();
	animatePacket->m_animateType = animateType;
	animatePacket->m_entityRuntimeId = entityId;
	animatePacket->m_value = value;
	animatePacket->m_holdItemId = holdItemId;
	sendPacketToTrackingPlayers(entityId, animatePacket, includeSelf);
}

void ServerPacketSender::syncEntityPlayerAnimate(ui64 rakssId, NETWORK_DEFINE::PacketAnimateType animateType, int entityId, int32_t value, int holdItemId)
{
	auto animatePacket = LORD::make_shared<S2CPacketAnimate>();
	animatePacket->m_animateType = animateType;
	animatePacket->m_entityRuntimeId = entityId;
	animatePacket->m_value = value;
	animatePacket->m_holdItemId = holdItemId;
	Server::Instance()->getNetwork()->sendPacket(animatePacket, rakssId);
}

void ServerPacketSender::broadCastPlayerSetSpYaw(int playerEntityId, bool isSpYaw, float radian)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerSetSpYaw>();
	packet->playerEntityId = playerEntityId;
	packet->isSpYaw = isSpYaw;
	packet->radian = radian;
	sendPacketToTrackingPlayers(playerEntityId, packet);
}

void ServerPacketSender::sendbroadcastRanchExTip(ui16 type, ui16 num)
{
	auto packet = LORD::make_shared<S2CPacketShowRanchExTip>();
	packet->type = type;
	packet->num = num;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet, true);
}

void ServerPacketSender::sendbroadcastRanchExTask(bool show, String task)
{
	auto packet = LORD::make_shared<S2CPacketShowRanchExTask>();
	packet->show = show;
	packet->task = task;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet, true);
}

void ServerPacketSender::sendShowRanchExCurrentItemInfo(ui64 rakssId, bool show, String itemInfo, int type, int num)
{
	auto packet = LORD::make_shared<S2CPacketShowRanchExCurrentItemInfo>();
	packet->show = show;
	packet->itemInfo = itemInfo;
	packet->num = num;
	packet->type = type;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendShowRanchExItem(ui64 rakssId, bool show, String item)
{
	auto packet = LORD::make_shared<S2CPacketShowRanchExItem>();
	packet->show = show;
	packet->item = item;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendAddSimpleEffect(String effectName, Vector3 position, float yaw, int duration, int targetId, float scale)
{
	auto packet = LORD::make_shared<S2CPacketAddSimpleEffect>();
	packet->effectName = effectName;
	packet->position = position;
	packet->yaw = yaw;
	packet->duration = duration;
	packet->scale = scale;
	Server::Instance()->getNetwork()->sendPacket(packet, targetId);
}

void ServerPacketSender::broadCastAddSimpleEffect(String effectName, Vector3 position, float yaw, int duration, float scale)
{
	auto packet = LORD::make_shared<S2CPacketAddSimpleEffect>();
	packet->effectName = effectName;
	packet->position = position;
	packet->yaw = yaw;
	packet->duration = duration;
	packet->scale = scale;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastRemoveSimpleEffect(String effectName)
{
	auto packet = LORD::make_shared<S2CPacketRemoveSimpleEffect>();
	packet->effectName = effectName;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastCarDirection(int type, int direction)
{
	auto packet = LORD::make_shared<S2CPacketCarDirection>();
	packet->type = type;
	packet->direction = direction;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastCarProgress(int type, float progress)
{
	auto packet = LORD::make_shared<S2CPacketCarProgress>();
	packet->type = type;
	packet->progress = progress;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::syncEntityFrozenTime(ui64 rakssId, int entityId, float time)
{
	auto packet = LORD::make_shared<S2CPacketEntityFrozenTime>();
	packet->entityId = entityId;
	packet->time = time;
	if (rakssId == 0)
		sendPacketToTrackingPlayers(entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

#pragma endregion