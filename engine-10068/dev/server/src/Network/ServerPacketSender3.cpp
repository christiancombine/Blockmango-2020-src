#include "ServerPacketSender.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Inventory/IInventory.h"
#include "Blockman/Entity/EntityTracker.h"
#include "Script/GameServerEvents.h"
#include "Setting/AppItemsSetting.h"
#include "Game/BirdSimulator.h"
#include "Game/BlockCity.h"
#include "Network/protocol/S2CPackets3.h"
#include "Entity/EntityDecoration.h"
#include "Setting/SceneSetting.h"
#include "Blockman/World/BlockChangeRecorderServer.h"

using namespace BLOCKMAN;

#pragma region

void ServerPacketSender::syncBirdSimulatorStoreItem(ui64 rakssId, const BLOCKMAN::BirdStore & store)
{
	auto packet = LORD::make_shared<S2CPacketSyncBirdSimulatorStoreItem>();
	packet->store = store;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncBirdSimulatorTaskItem(ui64 rakssId, bool isRemove, const BLOCKMAN::BirdTask & taskItem)
{
	auto packet = LORD::make_shared<S2CPacketSyncBirdSimulatorTaskItem>();
	packet->taskItem = taskItem;
	packet->isRemove = isRemove;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncBirdSimulatorPersonStoreTab(ui64 rakssId, const BLOCKMAN::BirdPersonalStoreTab & storeTab)
{
	auto packet = LORD::make_shared<S2CPacketSyncBirdSimulatorPersonStoreTab>();
	packet->storeTab = storeTab;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendBirdAddMessage(ui64 rakssId, String message)
{
	auto packet = LORD::make_shared<S2CPacketBirdAddMessage>();
	packet->message = message;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::changeGuideArrowStatus(ui64 rakssId, Vector3 arrowPos, bool flag)
{
	auto packet = LORD::make_shared<S2CPacketChangeGuideArrowStatus>();
	packet->arrowPos = arrowPos;
	packet->flag = flag;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::addWallText(ui64 rakssId, i32 type, i32 timeLeft, String text, Vector3 textPos, float scale, float yaw, float pitch, std::vector<float> color)
{
	auto packet = LORD::make_shared<S2CPacketAddWallText>();
	packet->type = type;
	packet->timeLeft = timeLeft;
	packet->text = text;
	packet->textPos = textPos;
	packet->scale = scale;
	packet->yaw = yaw;
	packet->pitch = pitch;
	packet->r = color[0];
	packet->g = color[1];
	packet->b = color[2];
	packet->a = color[3];

	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::deleteWallText(ui64 rakssId, Vector3 textPos)
{
	auto packet = LORD::make_shared<S2CPacketDeleteWallText>();
	packet->textPos = textPos;
	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendSetPlayerGlide(ui64 rakssId, BLOCKMAN::EntityPlayer * player)
{
	auto packet = LORD::make_shared<S2CPacketSetPlayerGlide>();
	packet->entityId = player->entityId;
	packet->isGlide = player->m_isGlide;
	packet->isEquipWing = player->m_isEquipWing;
	packet->glideMoveAddition = player->m_glideMoveAddition;
	packet->glideDropResistance = player->m_glideDropResistance;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastSetPlayerGlide(BLOCKMAN::EntityPlayer * player)
{
	auto packet = LORD::make_shared<S2CPacketSetPlayerGlide>();
	packet->entityId = player->entityId;
	packet->isGlide = player->m_isGlide;
	packet->isEquipWing = player->m_isEquipWing;
	packet->glideMoveAddition = player->m_glideMoveAddition;
	packet->glideDropResistance = player->m_glideDropResistance;
	sendPacketToTrackingPlayers(player->entityId, packet, true);
}

void ServerPacketSender::showAddCarryGuide(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketShowAddCarryGuide>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCheckinData(ui64 rakssId, const String & data)
{
	auto packet = LORD::make_shared<S2CPacketBirdCheckinData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::showBirdFeedGuide(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketShowBirdFeedGuide>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendBirdShowWingTip(ui64 rakssId, i32 page)
{
	auto packet = LORD::make_shared<S2CPacketBirdShowWingTip>();
	packet->page = page;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastAddWallText(String text, Vector3 textPos, float scale, float yaw, float pitch, float r, float g, float b, float a)
{
	auto packet = LORD::make_shared<S2CPacketAddWallText>();
	packet->text = text;
	packet->textPos = textPos;
	packet->scale = scale;
	packet->yaw = yaw;
	packet->pitch = pitch;
	packet->r = r;
	packet->g = g;
	packet->b = b;
	packet->a = a;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastDeleteWallText(Vector3 textPos)
{
	auto packet = LORD::make_shared<S2CPacketDeleteWallText>();
	packet->textPos = textPos;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendDefaultNotification(ui64 rakssId, String content)
{
	auto packet = LORD::make_shared<S2CPacketDefaultNotification>();
	packet->content = content;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::notifyKills(ui64 rakssId, String icon, uint soundType, i32 stay, i32 animate)
{
	auto packet = LORD::make_shared<S2CPacketNotifyKills>();
	packet->icon = icon;
	packet->soundType = soundType;
	packet->stay = stay;
	packet->animate = animate;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSupplyTip(ui64 rakssId, int type, String icon, int value)
{
	auto packet = LORD::make_shared<S2CPacketSupplyTip>();
	packet->type = type;
	packet->icon = icon;
	packet->value = value;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendKeepItemAndReviveCountDown(ui64 rakssId, ui32 second, int type)
{
	auto packet = LORD::make_shared<S2CPacketKeepItemAndReviveCountDown>();
	packet->m_second = second;
	packet->type = type;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendChangeQuicklyBuildBlockStatus(i64 rakssId, bool allowStatus, bool currStatus)
{
	auto packet = LORD::make_shared<S2CPacketChangeQuicklyBuildBlockStatus>();
	packet->m_allowStatus = allowStatus;
	packet->m_currStatus = currStatus;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerAppleFresh(ui64 rakssId, int time)
{
	auto packet = LORD::make_shared<S2CPacketAppleFresh>();
	packet->time = time;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastPickUpEntityItem(int playerEntityId, int itemEntityId)
{
	auto packet = LORD::make_shared<S2CPacketPickUpEntityItem>();
	packet->m_playerEntityId = playerEntityId;
	packet->m_itemEntityId = itemEntityId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendPlayingSelectableActionToTrackingPlayers(int entityId, int actionId, String langKey)
{
	auto packet = LORD::make_shared<S2CPacketPlayingSelectableAction>();
	packet->m_entityId = entityId;
	packet->m_actionId = actionId;
	packet->m_langKey = langKey;
	sendPacketToTrackingPlayers(entityId, packet);
}

void ServerPacketSender::sendAllowUseItemStatus(ui64 rakssId, int status)
{
	auto packet = LORD::make_shared<S2CPacketAllowUseItemStatus>();
	packet->m_status = status;
	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendUpdateSelectableAction(i64 rakssId, const vector<int>::type& selectableAction)
{
	auto packet = LORD::make_shared<S2CPacketUpdateSelectableAction>();
	packet->m_selectableAction = selectableAction;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendAppItemsSettingInfo(ui64 rakssid, ui64 platformUserId)
{
	UserAttrInfo attrInfo;
	int vip = 0;
	if (Server::Instance()->getEnableRoom() && Server::Instance()->getRoomManager()->getUserAttrInfo(platformUserId, attrInfo))
	{
		vip = attrInfo.vip;
	}

	auto packet = LORD::make_shared<S2CPacketAppItemsSettingInfo>();
	packet->m_actionInfoList = AppItemsSetting::Instance()->getActionInfoList(vip);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendSceneSettingInfo(ui64 rakssid)
{
	auto packet = LORD::make_shared<S2CPacketSceneSettingInfo>();
	packet->skyInfoList = SceneSetting::Instance()->getSkyInfoList();
	packet->sunName = SceneSetting::Instance()->getSunName();
	packet->moonName = SceneSetting::Instance()->getMoonName();
	packet->skyType = SceneSetting::Instance()->getSkyType();
	packet->cloudType = SceneSetting::Instance()->getCloudType();
	packet->showSunAndMoon = SceneSetting::Instance()->showSunAndNoon();
	packet->showMeteor = SceneSetting::Instance()->showMeteor();
	packet->showStarEffect = SceneSetting::Instance()->showStarEffect();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendSceneSettingSwitch(ui64 rakssid, int skyType, int cloudType, bool showSunAndMoon, bool showMeteor, bool showStarEffect)
{
	auto packet = LORD::make_shared<S2CPacketSceneSettingSwitch>();
	packet->skyType = skyType;
	packet->cloudType = cloudType;
	packet->showSunAndMoon = showSunAndMoon;
	packet->showMeteor = showMeteor;
	packet->showStarEffect = showStarEffect;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendUpdateGuideData(i64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketUpdateGuideData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendPlayerActoinArea(ui64 rakssId, const std::vector<BLOCKMAN::ActionArea>& areas)
{
	auto packet = LORD::make_shared<S2CPacketPlayerActionArea>();
	for (const auto& item : areas)
	{
		packet->m_areas.push_back(item);
	}
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncBlockCity(BLOCKMAN::EntityPlayer * player, BLOCKMAN::SyncBlockCityType syncType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p && p->m_blockCity)
	{
		switch ((SyncBlockCityType)syncType)
		{
		case SyncBlockCityType::CITY_MANOR:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityManor>();
			packet->manorManager = *player->m_blockCity->getManorManager();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockCityType::CITY_PLAYER_INFO:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityPlayerInfo>();
			packet->playerInfo = *player->m_blockCity->getPlayerInfo();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockCityType::CITY_STORE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityStore>();
			packet->stores = player->m_blockCity->getStore();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockCityType::CITY_MANORINFO:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityManorInfo>();
			packet->manorInfo = *player->m_blockCity->getManorInfo();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockCityType::CITY_BUYLACKITEMS:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityBuyLackItems>();
			packet->lackItems = *player->m_blockCity->getBuyLackItems();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockCityType::CITY_HOUSE_SHOP:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityHouseShop>();
			packet->shops = player->m_blockCity->getHouseShop();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockCityType::CITY_MANOR_AREA:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityManorArea>();
			packet->manorAreas = player->m_blockCity->getManorArea();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockCityType::CITY_ELEVATOR_AREA:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityElevatorArea>();
			packet->elevatorAreas = player->m_blockCity->getElevatorArea();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockCityType::CITY_WEEK_REWARD:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityWeekReward>();
			packet->weekRewards = player->m_blockCity->getWeekReward();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockCityType::CITY_ARCHIVE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockCityArchive>();
			packet->archive = *player->m_blockCity->getArchive();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		}
	}
}

void ServerPacketSender::syncEntityDecoration(ui64 rakssId, BLOCKMAN::EntityDecoration * entity)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityDecoration>();
	packet->userId = entity->m_userId;
	packet->entityId = entity->hashCode();
	packet->nodeSize = (i32)entity->getNodes().size();
	packet->decorationId = entity->getDecorationId();
	packet->isPlayAction = entity->isPlayAction();
	for (auto item : entity->getNodes())
	{
		packet->nodeKeys.push_back(item.first);
		packet->nodeValues.push_back(item.second);
		if (item.second > 0)
		{
			if (auto plyaer = entity->world->getPlayerByPlatformUserId(item.second))
			{
				syncTakeDecoration(0, plyaer->hashCode(), plyaer->getDecorationEntity() ? plyaer->getDecorationEntity()->hashCode() : 0);
			}
		}
	}
	if (rakssId == 0)
	{
		sendPacketToTrackingPlayers(entity->entityId, packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::syncTakeDecoration(ui64 rakssId, i32 playerEntityId, i32 decorationEntityId)
{
	auto packet = LORD::make_shared<S2CPacketSyncTakeDecoration>();
	packet->playerEntityId = playerEntityId;
	packet->decorationEntityId = decorationEntityId;
	if (rakssId == 0)
	{
		sendPacketToTrackingPlayers(playerEntityId, packet, true);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::showBlockCityCommonTip(ui64 rakssId, const vector<BLOCKMAN::BlockCityPayment>::type& pays, i32 type, const String & msg)
{
	auto packet = LORD::make_shared<S2CPacketShowBlockCityCommonTip>();
	packet->pays = pays;
	packet->type = type;
	packet->msg = msg;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendShowRotateView(ui64 rakssId, i32 type, i32 id)
{
	auto packet = LORD::make_shared<S2CPacketShowRotateView>();
	packet->id = id;
	packet->type = type;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::updateBlockCityItems(ui64 rakssId, const vector<BLOCKMAN::BlockCityStoreItem>::type & items)
{
	auto packet = LORD::make_shared<S2CPacketuUpdateBlockCityItems>();
	packet->items = items;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendRevertBlocks(ui64 rakssId, vector<Vector3i>::type blocks)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBlocks>();
	packet->updates.reserve(blocks.size());
	for (const auto& pos : blocks)
	{
		packet->updates.emplace_back(pos, Server::Instance()->getWorld()->getBlockId(pos), Server::Instance()->getWorld()->getBlockMeta(pos));
	}
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCommonTip(ui64 rakssId, String tips, i32 type)
{
	auto packet = LORD::make_shared<S2CPacketCommonTip>();
	packet->type = type;
	packet->tips = tips;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);

}

void ServerPacketSender::sendPlaceBlocks(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketPlaceBlocks>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCraftItemUpdate(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketCraftItemUpdate>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendUpdateSkyBlockMainLineTaskData(i64 rakssId, int lv, int progressesLevel, bool dareIsUnLock, String data, int taskType)
{
	auto packet = LORD::make_shared<S2CPacketUpdateSkyBlockMainLineTaskData>();
	packet->lv = lv;
	packet->progressesLevel = progressesLevel;
	packet->dareIsUnLock = dareIsUnLock;
	packet->data = data;
	packet->taskType = taskType;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSkyBlockDareTaskInfo(i64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPackeSkyBlockDareTaskInfo>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSkyBlockShopInfo(i64 rakssId, int sortType, int lv, String priData)
{
	auto packet = LORD::make_shared<S2CPacketShowSkyShopInfo>();
	packet->sortType = sortType;
	packet->lv = lv;
	packet->priData = priData;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSkyBlockRankData(i64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPackeSkyBlockRankData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendCanUseItemBucket(ui64 rakssId, bool canUseItemBucket)
{
	auto packet = LORD::make_shared<S2CPacketCanUseItemBucket>();
	packet->canUseItemBucket = canUseItemBucket;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSkyblockShowAppShopPage(ui64 rakssId, i32 pageId)
{
	auto packet = LORD::make_shared<S2CPackeSkyBlockShowAppShopPage>();
	packet->pageId = pageId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::notifyGetGoodsByItemId(ui64 rakssId, i32 itemId, i32 meta, i32 num, String img)
{
	auto packet = LORD::make_shared<S2CPackeNotifyGetGoodsByItemId>();
	packet->itemId = itemId;
	packet->meta = meta;
	packet->num = num;
	packet->img = img;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSetMoveState(ui64 rakssid, i32 state)
{
	auto packet = LORD::make_shared<S2CPacketSetMoveState>();
	packet->state = state;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::syncBlockFort(BLOCKMAN::EntityPlayer * player, BLOCKMAN::SyncBlockFortType syncType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p && p->m_blockFort)
	{
		switch (syncType)
		{
		case SyncBlockFortType::FORT_MANORINFO:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockFortManorInfo>();
			packet->manorInfo = *player->m_blockFort->getManorInfo();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockFortType::FORT_MANORMANAGER:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockFortManorManager>();
			packet->manorManager = *player->m_blockFort->getManorManager();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		case SyncBlockFortType::FORT_PLAYERINFO:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockFortPlayerInfo>();
			packet->playerInfo = *player->m_blockFort->getPlayerInfo();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;

		case SyncBlockFortType::FORT_STORE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBlockFortStore>();
			packet->stores = player->m_blockFort->getStore();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID());
		}
		break;
		}
	}
}

void ServerPacketSender::updateBlockFortItem(ui64 rakssId, i32 tabId, BLOCKMAN::BlockFortStoreItem* item)
{
	auto packet = LORD::make_shared<S2CPacketuUpdateBlockFortItems>();
	packet->items = *item;
	packet->tabId = tabId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::showBlockFortBuildingInfo(ui64 rakssId, i32 id, i32 index)
{
	auto packet = LORD::make_shared<S2CPacketuShowBlockFortBuildingInfo>();
	packet->id = id;
	packet->index = index;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendShowFloatTip(ui64 rakssId, const String &msg)
{
	auto packet = LORD::make_shared<S2CPacketShowFloatTip>();
	packet->msg = msg;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncBlockCityPackTab(ui64 rakssId, const  BLOCKMAN::BlockCityHouseShopClassify& packTab)
{
	auto packet = LORD::make_shared<S2CPacketSyncBlockCityPackTab>();
	packet->packTab = packTab;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendSetPlayerElevator(ui64 rakssId, BLOCKMAN::EntityPlayer * player)
{
	auto packet = LORD::make_shared<S2CPacketSetPlayerElevator>();
	packet->entityId = player->hashCode();
	packet->isUse = player->m_isUseElevator;
	packet->speed = player->m_elevatorSpeed;
	packet->arriveFloor = player->m_elevatorArriveFloor;
	packet->elevatorId = player->m_elevatorId;
	packet->arriveHeight = player->m_elevatorArriveHeight;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}
void ServerPacketSender::sendCreatureTargetId(ui64 rakssId, int targetId, int entityId)
{
	auto packet = LORD::make_shared<S2CPacketSyncCreatureTargetId>();
	packet->EntityId = entityId;
	packet->TargetId = targetId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastVehicleHornSound(const Vector3 &pos, int entityId)
{
	auto packet = LORD::make_shared<S2CPacketPlayVehicleHornSound>();
	packet->pos = pos;
	sendPacketToTrackingPlayers(entityId, packet, false);
}

void ServerPacketSender::sendPlayerDBDataReady(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketDBDataReady>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastBindEntity(int parentEntityId, int childEntityId, const String& nodeName, int idleActionId)
{
	auto packet = LORD::make_shared<S2CPacketSyncBindEntity>();
	packet->parentEntityId = parentEntityId;
	packet->childEntityId = childEntityId;
	packet->nodeName = nodeName;
	packet->idleActionId = idleActionId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastUnbindEntity(int childEntityId)
{
	auto packet = LORD::make_shared<S2CPacketSyncUnbindEntity>();
	packet->childEntityId = childEntityId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSyncEntityConnection(ui64 rakssid, BLOCKMAN::Entity* pEntity)
{
	if (!pEntity || pEntity->isDead)
		return;

	int parentId = pEntity->getParentEntityId();
	vector<int>::type childrenId = pEntity->getChildrenEntityId();

	if (parentId <= 0 && childrenId.size() <= 0)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncEntityConnection>();
	packet->myEntityId = pEntity->entityId;
	packet->parentEntityId = parentId;
	packet->nodeName = pEntity->getParentActorConnectiveBone();
	packet->idleActionId = pEntity->getIdleActionId();
	packet->childrenEntityId = childrenId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendFlyingMountMovement(
	ui64 destRaknetId, int entityId, const Vector3& position, const Quaternion& orientation, const Quaternion& viewOrientation)
{
	auto packet = LORD::make_shared<S2CPacketFlyingMountMovement>();
	packet->entityId = entityId;
	packet->position = position;
	packet->bodyOrientW = orientation.w;
	packet->bodyOrientX = orientation.x;
	packet->bodyOrientY = orientation.y;
	packet->bodyOrientZ = orientation.z;
	packet->viewOrientW = viewOrientation.w;
	packet->viewOrientX = viewOrientation.x;
	packet->viewOrientY = viewOrientation.y;
	packet->viewOrientZ = viewOrientation.z;
	Server::Instance()->getNetwork()->sendPacket(packet, destRaknetId);
}

void ServerPacketSender::sendGetOnFlyingMount(ui64 destRaknetId, int entityId, int mountId)
{
	auto packet = LORD::make_shared<S2CPacketMountFlyingMount>();
	packet->entityId = entityId;
	packet->mountId = mountId;
	Server::Instance()->getNetwork()->sendPacket(packet, destRaknetId);
}

void ServerPacketSender::broadcastGetOnFlyingMount(int entityId, int mountId)
{
	auto packet = LORD::make_shared<S2CPacketMountFlyingMount>();
	packet->entityId = entityId;
	packet->mountId = mountId;
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::broadcastGetOffFlyingMount(int entityId)
{
	auto packet = LORD::make_shared<S2CPacketMountFlyingMount>();
	packet->entityId = entityId;
	packet->mountId = 0;
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::sendGetOnFloatingMount(ui64 destRaknetId, int entityId, int mountId)
{
	auto packet = LORD::make_shared<S2CPacketMountFloatingMount>();
	packet->entityId = entityId;
	packet->mountId = mountId;
	Server::Instance()->getNetwork()->sendPacket(packet, destRaknetId);
}

void ServerPacketSender::broadcastGetOnFloatingMount(int entityId, int mountId)
{
	auto packet = LORD::make_shared<S2CPacketMountFloatingMount>();
	packet->entityId = entityId;
	packet->mountId = mountId;
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::broadcastGetOffFloatingMount(int entityId)
{
	auto packet = LORD::make_shared<S2CPacketMountFloatingMount>();
	packet->entityId = entityId;
	packet->mountId = 0;
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::sendFloatingMountMovement(ui64 destRaknetId, int entityId, const Vector3& position, float bodyYaw, float viewYaw, float pitch)
{
	auto packet = LORD::make_shared<S2CPacketFloatingMountMovement>();
	packet->entityId = entityId;
	packet->position = position;
	packet->bodyYaw = bodyYaw;
	packet->viewYaw = viewYaw;
	packet->pitch = pitch;
	Server::Instance()->getNetwork()->sendPacket(packet, destRaknetId);
}

void ServerPacketSender::syncUserRelation(ui64 rakssId, vector<i64>::type friends)
{
	auto packet = LORD::make_shared<S2CPacketUserRelation>();
	packet->friends = friends;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadcastGameWeather(int weatherType)
{
	auto packet = LORD::make_shared<S2CPacketSyncGameWeather>();
	packet->weatherType = weatherType;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastUpdateBlock(Vector3i position, int entityId)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBlocks>();
	auto world = Server::Instance()->getWorld();
	packet->updates.emplace_back(position, world->getBlockId(position), world->getBlockMeta(position));
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet, entityId);
}

void ServerPacketSender::sendUpdateBlocks(ui64 rakssid)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBlocks>();
	const auto& positions = BlockChangeRecorderServer::Instance()->getChangedPositions();
	packet->updates.reserve(positions.size());
	auto world = Server::Instance()->getWorld();
	for (const auto& position : positions)
	{
		packet->updates.emplace_back(position, world->getBlockId(position), world->getBlockMeta(position));
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::resetUpdateBlocks()
{
	auto blocks = BlockChangeRecorderServer::Instance()->getChangedPositions();

	EntityPlayers& players = Server::Instance()->getWorld()->getPlayers();

	for (EntityPlayers::iterator it = players.begin(); it != players.end(); ++it)
	{
		EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(it->second);
		if (!pPlayer)
			continue;

		pPlayer->addUpdateBlocks(blocks);
	}
}

void ServerPacketSender::sendUpdateBlocksWithDistance(ui64 rakssid, const std::vector<NETWORK_DEFINE::S2CPacketUpdateBlockInfo>& block_info)
{
	if (block_info.size() <= 0) return;

	if (block_info.size() <= 10)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateBlocks>();
		packet->updates.reserve(block_info.size());
		for (auto block : block_info)
		{
			packet->updates.emplace_back(Vector3i(block.x, block.y, block.z), (int)block.getBlockId(), (int)block.getBlockMeta());
		}
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
		return;
	}

	auto packet = LORD::make_shared<S2CPacketUpdateBlockChunks>();
	for (const auto& info : block_info)
	{
		i16 chunkX = info.x >> 4;
		i16 chunkZ = info.z >> 4;

		ui8 offsetX = info.x - (chunkX << 4);
		ui8 offsetY = (ui8)info.y;
		ui8 offsetZ = info.z - (chunkZ << 4);

		ui8 offsetXZ = (offsetX << 4) | (offsetZ & 0xf);

		bool isAdd = false;
		for (auto& item : packet->updates)
		{
			if (item.chunkX == chunkX && item.chunkZ == chunkZ)
			{
				isAdd = true;
				bool isItemAdd = false;
				for (auto& block : item.blocks)
				{
					if (block.y == offsetY)
					{
						isItemAdd = true;
						block.infos.push_back(NETWORK_DEFINE::UpdateBlockInfo(offsetXZ, info.blockMetaAndId));
					}
				}

				if (!isItemAdd)
				{
					NETWORK_DEFINE::TempBlockInfo block = NETWORK_DEFINE::TempBlockInfo(offsetY);
					block.infos.push_back(NETWORK_DEFINE::UpdateBlockInfo(offsetXZ, info.blockMetaAndId));
					item.blocks.push_back(block);
				}

			}
		}

		if (!isAdd)
		{
			NETWORK_DEFINE::TempChunk chunk = NETWORK_DEFINE::TempChunk(chunkX, chunkZ);
			NETWORK_DEFINE::TempBlockInfo block = NETWORK_DEFINE::TempBlockInfo(offsetY);
			block.infos.push_back(NETWORK_DEFINE::UpdateBlockInfo(offsetXZ, info.blockMetaAndId));
			chunk.blocks.push_back(block);
			packet->updates.push_back(chunk);
		}
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendPlayerChangeApple(ui64 rakssId, int cur, int max)
{
	auto packet = LORD::make_shared<S2CPacketChangeApple>();
	packet->cur = cur;
	packet->max = max;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastActorNpcSuspended(int entityId, bool suspended, Vector3 pos)
{
	auto packet = LORD::make_shared<S2CPacketActorNpcSuspended>();
	packet->entityId = entityId;
	packet->suspended = suspended;
	packet->pos = pos;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastActorNpcHeadInfoMultiLang(int entityId, String msg, int lv, int time)
{
	auto packet = LORD::make_shared<S2CPacketSessionNpcHeadInfoMultiLang>();
	packet->entityId = entityId;
	packet->msg = msg;
	packet->lv = lv;
	packet->time = time;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSessionNpcHeadInfoTimeLang(ui64 rakssId, int entityId, String msg, int lv, int time)
{
	auto packet = LORD::make_shared<S2CPacketSessionNpcHeadInfoMultiLang>();
	packet->entityId = entityId;
	packet->msg = msg;
	packet->lv = lv;
	packet->time = time;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendAttackScore(ui64 rakssId, i32 score, i32 scoreType)
{
	auto packet = LORD::make_shared<S2CPacketAttackScore>();
	packet->score = score;
	packet->scoreType = scoreType;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

#pragma endregion