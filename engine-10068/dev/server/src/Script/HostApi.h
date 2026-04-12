#pragma once

#include "Core.h"
#include "Server.h"
#include "RoomManager.h"
#include "CommodityManager.h"
#include "Util/AboutShopping.h"
#include "Object/Root.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "Network/ServerPacketSender.h"
#include "Network/RoomClient.h"
#include "Network/GameCmdMgr.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Blockman/World/BlockChangeRecorderServer.h"
#include "Chat/ChatComponent.h"
#include "Inventory/CoinManager.h"
#include "Setting/LogicSetting.h"
#include "Setting/MultiLanTipSetting.h"
#include "Network/ManorRequest.h"
#include "DB/RedisHttpRequest.h"
#include "rapidjson/document.h"
#include "Network/HttpRequest.h"
#include "Game/BirdSimulator.h"
#include "Util/NewRandom.h"
#include "Setting/ActionAreaManager.h"
#include "Setting/SceneSetting.h"
#include "Game/BlockCity.h"
#include "Blockman/World/BlockCropsManager.h"
#include "Util/PlayerPrefs.h"
#include "Entity/DamageSource.h"
#include "Item/Items.h"
#include "Game/Manor.h"
#include "Setting/RanchSetting.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

using namespace LORD;
struct HostApi
{
	static void putBoolPrefs(String key, bool value)
	{
		PlayerPrefs::Instance()->SetProperty<bool>(key.c_str(), value);
	}

	static bool getBoolPrefs(String key)
	{
		return PlayerPrefs::Instance()->GetProperty<bool>(key.c_str());
	}

	static void putIntPrefs(String key, int value)
	{
		PlayerPrefs::Instance()->SetProperty<int>(key.c_str(), value);
	}

	static int getIntPrefs(String key)
	{
		return PlayerPrefs::Instance()->GetProperty<int>(key.c_str());
	}

	static void putFloatPrefs(String key, float value)
	{
		PlayerPrefs::Instance()->SetProperty<float>(key.c_str(), value);
	}

	static int getFloatPrefs(String key)
	{
		return PlayerPrefs::Instance()->GetProperty<float>(key.c_str());
	}

	static void putStringPrefs(String key, String value)
	{
		PlayerPrefs::Instance()->SetProperty<String>(key.c_str(), value);
	}

	static String getStringPrefs(String key)
	{
		return PlayerPrefs::Instance()->GetProperty<String>(key.c_str());
	}

	static void sendPlaySound(ui64 targetId/*ssid*/, uint soundType, bool bForce = false, uint channelId = 0)
	{
		// win lose draw sound move to play on the PlayEndingAnimationEvent when bForce is false
		if (!bForce && (soundType == (int)ST_GameCompleteWin
			|| soundType == (int)ST_GameCompleteWin
			|| soundType == (int)ST_GameCompleteWin))
		{
			return;
		}
		ServerNetwork::Instance()->getSender()->sendPlaySound(targetId, soundType, channelId);
	}

	static void broadCastMsg(ui64 targetId/*ssid*/, ui8 type, i32 time, const char *msg)
	{
		ChatComponentNotification::Type chatType = static_cast<ChatComponentNotification::Type>(type);
		auto notify = LORD::make_shared<ChatComponentNotification>(String(msg), time, chatType);

		if (targetId == 0)
		{
			ServerNetwork::Instance()->getSender()->sendNotification(notify);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->sendNotification(targetId, notify);
		}
	}

	static void broadCastMsgByType(ui64 targetId, ui8 type, i32 time, int msgType, const char* args)
	{
		//ServerNetwork::Instance()->broadCastMsgByType(targetId, type, time, msgType, args);
		ServerNetwork::Instance()->broadCastGameTipByType(targetId, type, time, msgType, args);
	}

	static void sendSettlement(ui64 rakssId, String gameType, String result)
	{
		ServerNetwork::Instance()->getSender()->sendSettlement(rakssId, gameType, result);
	}

	static void sendGameover(ui64 rakssId, int msgType, int code)
	{
		ServerNetwork::Instance()->sendGameOver(rakssId, msgType, code);
	}

	static void sendGameoverByPlatformUserId(ui64 rakssId, String overMessage, int code)
	{
		ServerNetwork::Instance()->sendGameOverByPlatformUserId(rakssId, overMessage, code);
	}

	static void resetPos(ui64 id, float x, float y, float z)
	{
		if (x != x || y != y || z != z) {
			LordLogError("nan occur, resetPos------- %f, %f, %f", x, y, z);
			return;
		}

		LordLogInfo("ScriptLog-------resetpos %llx, %f, %f, %f", id, x, y, z);
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(id);
		if (clientPeer)
			clientPeer->resetPos(x, y, z);
	}

	static void resetPosWithYaw(ui64 id, float x, float y, float z, float yaw)
	{
		if (x != x || y != y || z != z || yaw != yaw) {
			LordLogError("nan occur, resetPosWithYaw------- %f, %f, %f, %f", x, y, z, yaw);
			return;
		}

		LordLogInfo("ScriptLog-------resetpos %llx, %f, %f, %f", id, x, y, z);
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(id);
		if (clientPeer)
			clientPeer->resetPos(x, y, z, yaw);
	}

	static void log(const char *log, int level = 0)
	{
		if (level == 0)
			LOG(G3LOG_DEBUG) << log;
		if (level == 1)
			LOG(G3LOG_WARNING) << log;
		if (level == 2)
			LOG(G3LOG_ERROR) << log;
	}

	static void stopServer()
	{
		Server::Instance()->stop();
	}

	static void sendStartGame(int curPlayers, bool forceEnterPlayer = false)
	{
		LordLogInfo("sendStartGame %d", curPlayers);
		RoomGameConfig cfg = Server::Instance()->getConfig();
		if (Server::Instance()->getEnableRoom())
		{
			Server::Instance()->getRoomManager()->getRoomClient()
				->sendGameStatus(cfg.gameId, cfg.maxPlayers, curPlayers,
					forceEnterPlayer ? (int)RoomGameStatus::STARTED_CAN_ENTER : (int)RoomGameStatus::STARTED);
		}
	}

	static void sendResetGame(int curPlayers)
	{
		LordLogInfo("sendResetGame %d", curPlayers);
		if (Server::Instance()->getEnableRoom())
		{
			RoomGameConfig cfg = Server::Instance()->getConfig();
			Server::Instance()->getRoomManager()->getRoomClient()
				->sendGameStatus(cfg.gameId, cfg.maxPlayers, curPlayers, (int)RoomGameStatus::PREPARING);
		}
	}

	static String curTimeString()
	{
		LORD::ui32 timestamp = LORD::Root::Instance()->getCurrentTime();
		char ibuf[20];
		memset(ibuf, 0, 20);
		sprintf(ibuf, "%d", LORD::Root::Instance()->getCurrentTime());
		return ibuf;
	}

	static void resetMap()
	{
		BlockChangeRecorderServer::Instance()->reset();
	}

	// 1 start 2 resetaddCoinMapping
	static void sendGameStatus(ui64 rakssId, int status)
	{
		ServerNetwork::Instance()->getSender()->sendGameStatus(rakssId, status);
	}

	static void sendPlayerSettlement(ui64 rakssId, String result, bool isNextServer = true)
	{
		ServerNetwork::Instance()->getSender()->sendPlayerSettlement(rakssId, result, isNextServer);
	}

	static void sendGameSettlement(ui64 rakssId, String result, bool isNextServer = true)
	{
		ServerNetwork::Instance()->getSender()->sendGameSettlement(rakssId, result, isNextServer);
	}

	static void addCoinMapping(int coinId, int itemId)
	{
		CoinManager::Instance()->addCoinMapping(coinId, itemId);
	}

	static void broadCastPlayerLifeStatus(ui64 platformUserId, bool isLife)
	{
		ServerNetwork::Instance()->getSender()->broadCastPlayerLifeStatus(platformUserId, isLife);
	}

	static void addCommodity(int index, ui8 category, Commodity commodity)
	{
		CommodityManager::Instance()->addCommodity(index, CommodityCategory(category), commodity);
	}

	static void changeCommodityList(int entityId, ui64 rakssId, int index)
	{
		CommodityManager::Instance()->addMapping(entityId, rakssId, index);
	}

	static void sendBedDestroy(int teamId)
	{
		ServerNetwork::Instance()->getSender()->broadCastBedDestroy(teamId);
	}

	static void sendRespawnCountDown(ui64 rakssId, ui32 second)
	{
		ServerNetwork::Instance()->getSender()->sendRespawnCountdown(rakssId, second);
	}

	static void setServerGameType(String gameType)
	{
		Server::Instance()->setGameType(gameType);
	}

	static void sendPlayerTeamInfo(String result)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(result.c_str());
		if (doc->HasParseError())
		{
			LordLogError("The PlayerTeamInfo HasParseError.");
			LordDelete(doc);
			return;
		}
		rapidjson::Value arr = doc->GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("teamId") || !arr[i].HasMember("userId"))
			{
				LordLogError("The PlayerTeamInfo content missed some field.");
				LordDelete(doc);
				return;
			}
			int teamId = arr[i].FindMember("teamId")->value.GetInt();
			int64_t userId = arr[i].FindMember("userId")->value.GetInt64();
			auto clientpeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
			if (clientpeer)
			{
				clientpeer->setTeamId(teamId);
				if (clientpeer->getEntityPlayer())
				{
					clientpeer->getEntityPlayer()->setTeamId(teamId);
				}
			}
		}
		LordDelete(doc);

		ServerNetwork::Instance()->getSender()->broadAllPlayerTeamInfo(result);
	}

	static void changePlayerTeam(ui64 rakssId, ui64 platformUserId, int teamId)
	{
		auto clientpeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(platformUserId);
		if (clientpeer)
		{
			clientpeer->setTeamId(teamId);
			if (clientpeer->getEntityPlayer())
			{
				clientpeer->getEntityPlayer()->setTeamId(teamId);
			}
		}
		if (rakssId == 0)
		{
			ServerNetwork::Instance()->getSender()->broadCastChangePlayerTeam(platformUserId, teamId);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->sendChangePlayerTeam(rakssId, platformUserId, teamId);
		}
	}

	static void setMaxInventorySize(int size)
	{
		LogicSetting::Instance()->setMaxInventorySize(size);
	}

	static void setMaxEnderInventorySize(int size)
	{
		LogicSetting::Instance()->setMaxEnderInventorySize(size);
	}

	static void setCanEatWhenever(bool canEat)
	{
		LogicSetting::Instance()->setCanEatWhenever(canEat);
	}

	static void setGunSetting(GunPluginSetting setting)
	{
		LogicSetting::Instance()->addGunPluginSetting(setting);
	}

	static void setPlayerKnockBackCoefficient(ui64 rakssId, float knockBackCoefficient)
	{
		auto  player = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (player && player->getEntityPlayer())
		{
			player->getEntityPlayer()->SetKnockBackCoefficient(knockBackCoefficient);
		}
	}

	static void setBulletClipSetting(int bulletId, int maxStack)
	{
		LogicSetting::Instance()->addBulletClipPluginSetting(bulletId, maxStack);
	}

	static void sendSelectRoleData(ui64 rakssId, String result)
	{
		ServerNetwork::Instance()->getSender()->sendSelectRoleData(rakssId, result);
	}

	static void setSneakShowName(bool isShowName)
	{
		LogicSetting::Instance()->setSneakShowName(isShowName);
	}

	static void setBreakBlockSoon(bool soon)
	{
		LogicSetting::Instance()->setBreakBlockSoon(soon);
	}

	static void setCanDamageItem(bool canDamageItem)
	{
		LogicSetting::Instance()->setCanDamageItem(canDamageItem);
	}

	static void showBuyRespawn(ui64 rakssId, int times)
	{
		Server::Instance()->getWorld()->showBuyRespawnToPlayer(rakssId, times);
	}

	static void setBlockAttr(int blockId, float hardness)
	{
		LogicSetting::Instance()->addBlockDynamicAttr(blockId, hardness);
	}

	static void setToolAttr(int itemId, float distance, float efficiency)
	{
		LogicSetting::Instance()->addToolItemDynamicAttr(itemId, distance, efficiency);
	}

	static void showGoNpcMerchant(ui64 rakssId, int entityId, float x, float y, float z, float yaw)
	{
		ServerNetwork::Instance()->getSender()->sendShowGoNpcMerchant(rakssId, entityId, x, y, z, yaw);
	}

	static void ZScore(std::string setName, std::string key)
	{
		//ServerRedisDB::Instance()->ZScore(setName, key);
		RedisHttpRequest::Instance()->ZScore(setName, key);
	}

	static void ZIncrBy(std::string setName, std::string key, int value)
	{
		//ServerRedisDB::Instance()->ZIncrBy(setName, key, value);
		RedisHttpRequest::Instance()->ZIncrBy(setName, key, value);
	}

	static void ZRange(std::string setName, int start, int end)
	{
		RedisHttpRequest::Instance()->ZRange(setName, start, end);
		//ServerRedisDB::Instance()->ZRange(setName, start, end);
	}

	static void ZExpireat(std::string setName, std::string timeStamp)
	{
		RedisHttpRequest::Instance()->ZExpireat(setName, timeStamp);
		//ServerRedisDB::Instance()->ZExpireat(setName, timeStamp);
	}

	static void ZRemove(std::string setName, std::string key)
	{
		RedisHttpRequest::Instance()->ZRem(setName, key);
		//ServerRedisDB::Instance()->ZRem(setName, key);
	}

	static void startRedisDBService()
	{
		Server::Instance()->startRedis();
	}

	static void setRedisDBHost(int host)
	{
		RedisHttpRequest::Instance()->setRedisDBHost(host);
	}

	static void sendRankData(ui64 rakssId, int entityId, String result)
	{
		ServerNetwork::Instance()->getSender()->sendRankData(rakssId, entityId, result);
	}

	static void setAttackCoefficientX(float x)
	{
		LogicSetting::Instance()->setAttackCoefficientX(x);
	}

	static void sendChangeAircraftUI(ui64 targetId, bool isShow)
	{
		if (targetId != 0)
		{
			ServerNetwork::Instance()->getSender()->sendChangeAircraftUI(targetId, isShow);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->broadCastChangeAircraftUI(isShow);
		}
	}

	static void setFallingSpeed(float horizontal, float vertical)
	{
		LogicSetting::Instance()->setParachuteDownSpeed(vertical);
		LogicSetting::Instance()->setParachuteMoveSpeed(horizontal);
	}

	static void setMedichineHealAmount(int id, float amount)
	{
		ItemMedichine* medichine = dynamic_cast<ItemMedichine*>(Item::getItemById(id));
		if (medichine == NULL)
			return;

		medichine->setHealAmount(amount);
	}

	static void sendMemberLeftAndKill(ui64 targetId, int left, int kill)
	{
		ServerNetwork::Instance()->getSender()->sendMemberLeftAndKill(targetId, left, kill);
	}

	static void changeSkillType(ui64 rakssId, int type, float duraction, float coldDownTime)
	{
		ServerNetwork::Instance()->getSender()->sendSkillType(rakssId, type, duraction, coldDownTime);
	}

	static void sendShowHideAndSeekBtnStatus(ui64 rakssId, bool isActorBtnStatus, bool isCameraBtnStatus, bool isOtherBtnStatus)
	{
		ServerNetwork::Instance()->getSender()->sendShowHideAndSeekBtnStatus(rakssId,isActorBtnStatus, isCameraBtnStatus, isOtherBtnStatus);
	}

	static void syncGameTimeShowUi(ui64 targetId, bool isShow, int time)
	{
		if (targetId != 0)
		{
			ServerNetwork::Instance()->getSender()->sendSyncGameTimeShowUi(targetId, isShow, time);
		}
		else
		{
			ServerNetwork::Instance()->getSender()->broadCastGameTimeShowUi(isShow, time);
		}
	}

	static void setDisableSelectEntity(bool isDisable)
	{
		LogicSetting::Instance()->setDisableSelectEntity(isDisable);
	}
	
	static void sendPickUpItemOrder(ui64 rakssId, i32 itemEntityId, i32 itemId, i32 pickUpPrice, i32 moneyType)
	{
		ServerNetwork::Instance()->getSender()->sendPickUpItemOrder(rakssId, itemEntityId, itemId, pickUpPrice, moneyType);
	}

	static void sendRefreshActor(int entityId)
	{
		ServerNetwork::Instance()->getSender()->sendDeathUnequipArmor(entityId);
	}

	static void changePlayerPerspece(ui64 rakssId, int view)
	{
		ServerNetwork::Instance()->getSender()->sendChangeEntityPerspece(rakssId, view);
	}

	static void setNeedFoodStats(bool isNeedFoodStats)
	{
		LogicSetting::Instance()->setNeedFoodStats(isNeedFoodStats);
	}

	static void updateManorInfo(ui64 rakssId, ManorInfo info, ManorInfo nextInfo, std::vector<ManorHouse> houses, std::vector<ManorFurniture> furnitures)
	{
		auto player = ServerNetwork::Instance()->findPlayerByRakssid(rakssId)->getEntityPlayer();
		if (player)
		{
			player->m_manor->setInfo(&info);
			player->m_manor->setNextInfo(&nextInfo);
			for (auto house : houses)
			{
				player->m_manor->addHouse(&house);
			}

			for (auto furniture : furnitures)
			{
				player->m_manor->addFurniture(&furniture);
			}
			
			Server::Instance()->getNetwork()->getSender()->sendUpdateManor(player);
		}
	}

	static void showPlayerOperation(ui64 rakssId, ui64 targetUserId)
	{
		Server::Instance()->getNetwork()->getSender()->sendShowPlayerOperation(rakssId, targetUserId);
	}
	
	static void updateRealTimeRankInfo(ui64 rakssId, String result)
	{
		ServerNetwork::Instance()->getSender()->sendUpdateRealTimeRankInfo(rakssId, result);
	}

	static void updateMonsterInfo(int currCheckPointNum, int surplusMonsterNum, int surplusCheckPointNum)
	{
		ServerNetwork::Instance()->getSender()->broadCastGameMonsterInfo(currCheckPointNum, surplusMonsterNum, surplusCheckPointNum);
	}

	static void updateBossBloodStrip(int currBloodStrip, int maxBloodStrip)
	{
		ServerNetwork::Instance()->getSender()->broadCastBossBloodStrip(currBloodStrip, maxBloodStrip);
	}

	static void setGunFireIsNeedBullet(bool gunIsNeedBullet)
	{
		LogicSetting::Instance()->setGunIsNeedBulletStatus(gunIsNeedBullet);
	}

	static void addMonsterSetting(MonsterSetting setting);

	static void addSkillEffect(SkillEffect effect)
	{
		LogicSetting::Instance()->addSkillEffect(effect);
	}

	static void showUpgradeResourceUI(ui64 rakssId, int costItemId, int costItemNum, int resourceId, int msgType, const char* args)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		std::string tipStr = "";
		if (clientPeer)
		{
			String lanType = clientPeer->getLanguage();
			tipStr = MultiLanTipSetting::getMessage(lanType, msgType, args);
		}
		ServerNetwork::Instance()->getSender()->showUpgradeResourceUI(rakssId, costItemId, costItemNum, resourceId, String(tipStr.c_str()));
	}

	static void sendCommonTip(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::COMMON, msg);
	}

	static void sendManorOperationTip(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::MANOR_BAN_OPERATION, msg);
	}

	static void setManorBtnVisible(ui64 rakssId, bool visible)
	{
		ServerNetwork::Instance()->getSender()->sendManorBtnVisible(rakssId, visible);
	}

	static void loadHouses(ui64 userId, const char* manorId)
	{
		ManorRequest::loadHouses(userId, manorId);
	}

	static void loadFurnitures(ui64 userId , const char* manorId)
	{
		ManorRequest::loadFurnitures(userId, manorId);
	}

	static void loadPloughInfo(ui64 userId, const char* manorId)
	{
		ManorRequest::loadPloughInfo(userId, manorId);
	}

	static void loadManorInfo(ui64 userId, bool isMaster)
	{
		ManorRequest::loadManorInfo(userId, isMaster);
	}

	static void loadManorConfig()
	{
		ManorRequest::loadManorConfig();
	}

	static void loadManorInventory(ui64 userId, const char* manorId)
	{
		ManorRequest::loadManorInventory(userId, manorId);
	}

	static void saveManorInventory(const char* manorId , const char* inventoryInfo)
	{
		ManorRequest::saveManorInventory(manorId, inventoryInfo);
	}

	static void loadGameMoney(ui64 platformUserId)
	{
		ManorRequest::loadGameMoney(platformUserId);
	}

	static void operationFurniture(ui64 userId, String manorId, i32 furnitureId, Vector3 pos, float yaw, i32 state)
	{
		ManorRequest::operationFurniture(userId, manorId, furnitureId, pos, yaw, state);
	}

	static void buildHouse(ui64 userId, String manorId, i32 houseId)
	{
		ManorRequest::buildHouse(userId, manorId, houseId);
	}

	static void loadManorBlock(ui64 userId, const char* manorId)
	{
		ManorRequest::loadManorBlock(userId, manorId);
	}

	static void saveManorBlock(const char* manorId, const char* blockInfo)
	{
		ManorRequest::saveManorBlock(manorId, blockInfo);
	}

	static void loadManorMessage(ui64 userId, i32 page)
	{
		ManorRequest::loadManorMessage(userId, page);
	}
	
	static void ploughCrop(const char* manorId, i32 row, i32 column,  String blockId, i32 fruitNum, i32 curState, i32 pluckingState, ui64 curStateTime, ui64 lastUpdateTime)
	{
		ManorRequest::ploughCrop(manorId, row, column, blockId, fruitNum, curState, pluckingState, curStateTime, lastUpdateTime);
	}

	static void ploughPlucking(const char* manorId, ui64 userId, i32 row, i32 colum, i32 num, i32 productId, ui64 pluckingTime)
	{
		ManorRequest::ploughPlucking(manorId, userId, row, colum, num, productId, pluckingTime);
	}

	static void updateBasementLife(float curLife, float maxLife)
	{
		ServerNetwork::Instance()->getSender()->broadCastBasementLife(curLife, maxLife);
	}

	static void sendCustomTip(ui64 rakssId, const char* messageLang, const char* extra)
	{
		ServerNetwork::Instance()->getSender()->sendCustomTipMsg(rakssId, messageLang, extra);
	}

	static void sendContentTip(ui64 rakssId, const char* messageLang, const char* titleLang)
	{
		ServerNetwork::Instance()->getSender()->sendContentTipMsg(rakssId, messageLang, titleLang);
	}

	static void addSkillItem(SkillItem skillItem)
	{
		LogicSetting::Instance()->addSkillItem(skillItem);
	}

	static void addSkyInfo(SkyInfo skyInfo)
	{
		SceneSetting::Instance()->addSkyInfo(skyInfo);
	}

	static void setSunAndMoonName(String sunName, String moonName)
	{
		SceneSetting::Instance()->setSunAndMoonName(sunName, moonName);
	}

	static void setCustomSceneSwitch(ui64 rakssId, int skyType, int cloudType, bool showSunAndMoon, bool showMeteor, bool showStarEffect)
	{
		if (skyType < 0 || skyType > 2) skyType = 1;
		if (cloudType < 0 || cloudType > 2) cloudType = 1;
		SceneSetting::Instance()->changeSceneSwitch((SceneSetting::RenderSkyType)skyType, (SceneSetting::RenderCloudType)cloudType, showSunAndMoon, showMeteor, showStarEffect);
		ServerNetwork::Instance()->getSender()->sendSceneSettingSwitch(rakssId, skyType, cloudType, showSunAndMoon, showMeteor, showStarEffect);
	}

	static void setSceneSwitch(int skyType, int cloudType, bool showSunAndMoon, bool showMeteor, bool showStarEffect)
	{
		if (skyType < 0 || skyType > 2) skyType = 1;
		if (cloudType < 0 || cloudType > 2) cloudType = 1;
		SceneSetting::Instance()->changeSceneSwitch((SceneSetting::RenderSkyType)skyType, (SceneSetting::RenderCloudType)cloudType, showSunAndMoon, showMeteor, showStarEffect);
	}

	static void loadManorCharmRank(i32 size)
	{
		ManorRequest::loadManorCharmRank(size);
	}

	static void loadManorPotentialRank(i32 size)
	{
		ManorRequest::loadManorPotentialRank(size);
	}

	static void loadMyManorRank(ui64 userId)
	{
		ManorRequest::loadMyManorRank(userId);
	}

	static void manorTrade(ui64 platformUserId, i32 price)
	{
		ManorRequest::trade(platformUserId, price);
	}

	static void notifyGetItem(ui64 rakssId, int itemId, int meta, int num)
	{
		ServerNetwork::Instance()->getSender()->notifyGetItem(rakssId, itemId, meta, num);
	}

	static void refreshSellItemSuc(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->refreshSellItemSuc(rakssId);
	}

	static void showBuildWarBlockBtn(ui64 userId, bool isNeedOpen)
	{
		ServerNetwork::Instance()->getSender()->sendBuildWarBlockBtn(userId, isNeedOpen);
	}

	static void showBuildWarPlayerNum(ui8 cur_num, ui8 max_num)
	{
		ServerNetwork::Instance()->getSender()->sendbroadcastBuildWarPlayNum(cur_num, max_num);
	}

	static void showBuildWarLeftTime(ui64 userId, bool show, ui16 left_time, ui32 theme)
	{
		ServerNetwork::Instance()->getSender()->sendbroadcastBuildWarLeftTime(userId, show, left_time, theme);
	}

	static void shopOpenShopByEntityId(ui64 userId, int entity_id)
	{
		ServerNetwork::Instance()->getSender()->sendOpenShopByEntityId(userId, entity_id);
	}

	static void showBuildGrade(ui64 userId, bool openOrClose, bool show, ui32 msg, ui32 grade_msg, ui8 left_time)
	{
		ServerNetwork::Instance()->getSender()->sendShowBuildGrade(userId, openOrClose, show, msg, grade_msg, left_time);
	}

	static void showBuildGuessResult(ui64 userId, bool right, int rank)
	{
		ServerNetwork::Instance()->getSender()->sendShowBuildGuessResult(userId, right, rank);
	}

	static void showBuildGuessUi(ui64 userId, int guess_room_id, String info)
	{
		ServerNetwork::Instance()->getSender()->showBuildGuessUi(userId, guess_room_id, info);
	}

	static void sendGameSettlementExtra(ui64 userId, bool guess_right, String guess_name, int guess_reward)
	{
		ServerNetwork::Instance()->getSender()->sendGameSettlementExtra(userId, guess_right, guess_name, guess_reward);
	}

	static void closeBGM(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendCloseBGM(rakssId);
	}
	
	static void updateTeamResources(String data)
	{
		ServerNetwork::Instance()->getSender()->sendTeamResourcesUpdate(data);
	}

	static void setHideClouds(bool isHide)
	{
		LogicSetting::Instance()->setHideClouds(isHide);
	}

	static void showConsumeCoinTip(ui64 rakssId, String message, int coinId, int price, String extra)
	{
		ServerNetwork::Instance()->getSender()->sendConsumeCoinTip(rakssId, message, coinId, price, extra);
	}

	static void updateBuildProgress(String name, int curProgress, int maxProgress)
	{
		ServerNetwork::Instance()->getSender()->broadCastBuildProgress(name, curProgress, maxProgress);
	}

	static void setFoodHeal(bool isFoodHeal)
	{
		LogicSetting::Instance()->setFoodHeal(isFoodHeal);
	}

	static void setEntityItemLife(int life)
	{
		LogicSetting::Instance()->setEntityItemLife(life);
	}

	static void setCanCloseChest(bool canCloseChest)
	{
		LogicSetting::Instance()->setCanCloseChest(canCloseChest);
	}

	static void setShowGunEffectWithSingle(bool showGunEffectWithSingle)
	{
		LogicSetting::Instance()->setShowGunEffectWithSingleStatus(showGunEffectWithSingle);
	}

	static void sendEnterOtherGame(ui64 rakssId, String gameType, ui64 targetId, String mapId = "")
	{
		ServerNetwork::Instance()->getSender()->sendEnterOtherGame(rakssId, gameType, targetId, mapId);
	}

	static void sendKillMsg(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendKillMsg(rakssId, data);
	}

	static void sendShowPersonalShop(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendShowPersonalShop(rakssId);
	}

	static void setAllowHeadshot(bool allowHeadshot)
	{
		LogicSetting::Instance()->setAllowHeadshotStatus(allowHeadshot);
	}

	static void sendCallOnManorResetClient(ui64 rakssId, ui64 targetId)
	{
		ServerNetwork::Instance()->getSender()->sendCallOnManorResetClient(rakssId, targetId);
	}

	static void sendRanchGain(ui64 rakssId, std::vector<RanchCommon> items)
	{
		if (items.size() == 0)
		{
			return;
		}
		vector<RanchCommon>::type newItems = vector<RanchCommon>::type();
		for (auto item : items)
		{
			newItems.push_back(item);
		}
		ServerNetwork::Instance()->getSender()->sendRanchGain(rakssId, newItems);
	}

	static void showRanchExTip(ui16 type, ui16 num)
	{
		ServerNetwork::Instance()->getSender()->sendbroadcastRanchExTip(type, num);
	}

	// type[1:yellow 2:green 3:red]; direction[1:Left 2:Right]
	static void changeCarDirection(int type, int direction)
	{
		if (type < 0 || type > 3)
			return;
		ServerNetwork::Instance()->getSender()->broadCastCarDirection(type, direction);
	}

	// type[1:yellow 2:green 3:red]; progress[0-100]
	static void changeCarProgress(int type, float progress)
	{
		if (type < 0 || type > 3)
			return;
		if (progress < 0 || progress > 100)
			return;
		ServerNetwork::Instance()->getSender()->broadCastCarProgress(type, progress);
	}

	static void showRanchExTask(bool show, String task)
	{
		ServerNetwork::Instance()->getSender()->sendbroadcastRanchExTask(show, task);
	}

	static void showRanchExCurrentItemInfo(ui64 rakssId, bool show, String itemInfo, int type, int num)
	{
		ServerNetwork::Instance()->getSender()->sendShowRanchExCurrentItemInfo(rakssId, show, itemInfo, type, num);
	}

	static void showRanchExItem(ui64 rakssId, bool show, String item)
	{
		ServerNetwork::Instance()->getSender()->sendShowRanchExItem(rakssId, show, item);
	}

	static void sendRanchOrderHelpResult(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::RANCH_ORDER_HELP_RESULT, msg);
	}

	static void sendRanchUnlockItem(ui64 rakssId, std::vector<i32> items)
	{
		if (items.empty())
		{
			return;
		}
		vector<i32>::type newItems = vector<i32>::type();
		for (auto item : items)
		{
			newItems.push_back(item);
		}

		ServerNetwork::Instance()->getSender()->sendRanchUnlockItem(rakssId, newItems);
	}

	static void sendMailReceiveResult(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::MAIL_RECEIVE_RESULT, msg);
	}

	static void sendGiveGiftResult(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::GIVE_GIFT, msg);
	}

	static void sendOpenChest(bool show, int entityId, Vector3i pos)
	{
		ServerNetwork::Instance()->getSender()->sendOpenChest(show, entityId, pos);
	}

	static void sendReleaseManor(ui64 userId)
	{
		Server::Instance()->getRoomManager()->getRoomClient()->sendUserSellManor(userId);
	}

	static void platformOrderDispose(ui64 userId, String url, String orderId, bool isConsume)
	{
		if (orderId.empty())
			return;
		LordLogInfo("HostApi::platformOrderDispose orderId [%s]", orderId.c_str());
		if (isConsume)
			HttpRequest::resumeOrder(url, orderId);
		else
			HttpRequest::cancelOrder(url, orderId, userId);
	}

	static void sendRanchWarehouseResult(ui64 rakssId, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::RANCH_STORAGE_FULL, msg);
	}

	static void sendAppExpResult(i64 userId, int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status)
	{
		ServerNetwork::Instance()->getSender()->sendAppExpResult(userId, curLv, toLv, addExp, curExp, toExp, upExp, status);
	}

	static void sendOccupationUnlock(ui64 rakssId, String msg)
	{
		ServerNetwork::Instance()->getSender()->sendOccupationUnlock(rakssId, msg);
	}

	static void sendBuyFlying(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendBuyFlying(rakssId);
	}

	static void sendKeepItemTip(ui64 rakssId, int coinType, int coin, int time)
	{
		ServerNetwork::Instance()->getSender()->sendKeepItemTip(rakssId, coinType, coin, time);
	}

	static void sendOpenChantment(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendOpenEnchantment(rakssId, data);
	}

	static void setLotteryData(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendLotteryData(rakssId, data);
	}

	static void sendLotteryResult(ui64 rakssId, String firstId, String secondId, String thirdId)
	{
		ServerNetwork::Instance()->getSender()->sendLotteryResult(rakssId, firstId, secondId, thirdId);
	}

	static void sendHideAndSeekHallResult(ui64 rakssId, String itemInfo)
	{
		ServerNetwork::Instance()->getSender()->sendHideAndSeekHallResult(rakssId, itemInfo);
	}

	static void sendShowSkyBlockInfo(ui64 rakssId, int lv, ui64 cur_exp, ui64 max_exp,int island_lv, String area, bool is_max, bool isMaster)
	{
		ServerNetwork::Instance()->getSender()->sendShowSkyBlockInfo(rakssId, lv, cur_exp, max_exp, island_lv, area, is_max, isMaster);
	}

	static void sendSkyBlockShopInfo(ui64 rakssId, int sortType, int lv, String priData)
	{
		ServerNetwork::Instance()->getSender()->sendSkyBlockShopInfo(rakssId, sortType, lv, priData);
	}

	static void setAutoShootEnable(ui64 rakssId, bool enableAutoShoot)
	{
		if (rakssId == 0)
		{
			LogicSetting::Instance()->setAutoShootEnable(enableAutoShoot);
		}
		ServerNetwork::Instance()->getSender()->sendEnableAutoShoot(rakssId, enableAutoShoot);
	}

	static void setDisarmament(ui64 rakssId, bool isDisarmament)
	{
		ServerNetwork::Instance()->getSender()->sendDisarmament(rakssId, isDisarmament);
	}

	static void setPlayerOpacity(int entityId, float opacityValue)
	{
		ServerNetwork::Instance()->getSender()->broadCastPlayerOpacity(entityId, opacityValue);
	}

	static void notifyGetGoods(ui64 rakssId, String icon, int num)
	{
		ServerNetwork::Instance()->getSender()->notifyGetGoods(rakssId, icon, num);
	}

	static void sendBirdGain(ui64 rakssId, std::vector<BirdGain> items)
	{
		if (items.size() == 0)
		{
			return;
		}
		vector<BirdGain>::type newItems = vector<BirdGain>::type();
		for (auto item : items)
		{
			newItems.push_back(item);
		}
		ServerNetwork::Instance()->getSender()->sendBirdGain(rakssId, newItems);
	}

	static void sendBirdNestOperation(ui64 rakssId, bool isUnlock, i64 birdId)
	{
		ServerNetwork::Instance()->getSender()->sendBirdNestOperation(rakssId, isUnlock, birdId);
	}

	static void sendBirdLotteryResult(ui64 rakssId, String eggBodyId,  BirdInfo  info)
	{
		ServerNetwork::Instance()->getSender()->sendBirdLotteryResult(rakssId, eggBodyId, info);
	}

	static void setThirdPersonDistance(float distance)
	{
		LogicSetting::Instance()->setThirdPersonDistance(distance);
	}

	static void sendBirdAddScore(ui64 rakssId, i32 score, i32 scoreType)
	{
		ServerNetwork::Instance()->getSender()->sendBirdAddScore(rakssId, score, scoreType);
	}

	static void sendAttackScore(ui64 rakssId, i32 score, i32 scoreType)
	{
		ServerNetwork::Instance()->getSender()->sendAttackScore(rakssId, score, scoreType);
	}

	static void sendBirdReceiveTaskResult(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, ResultCode::BIRD_RECEIVE_TASK, "");
	}

	static void sendBirdSimulatorStoreItem(ui64 rakssId, const  BLOCKMAN::BirdStore& store)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer && clientPeer->getEntityPlayer())
		{
			clientPeer->getEntityPlayer()->m_birdSimulator->setStoreItem(store);
		}
		ServerNetwork::Instance()->getSender()->syncBirdSimulatorStoreItem(rakssId, store);
	}

	static void sendBirdSimulatorTaskItem(ui64 rakssId, bool isRemove, const BLOCKMAN::BirdTask & taskItem)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer && clientPeer->getEntityPlayer())
		{
			clientPeer->getEntityPlayer()->m_birdSimulator->setTaskItem(isRemove, taskItem);
		}
		ServerNetwork::Instance()->getSender()->syncBirdSimulatorTaskItem(rakssId, isRemove, taskItem);
	}

	static void sendBirdSimulatorPersonStoreTab(ui64 rakssId, const BLOCKMAN::BirdPersonalStoreTab & storeTab)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer && clientPeer->getEntityPlayer())
		{
			clientPeer->getEntityPlayer()->m_birdSimulator->setPersonalStoreTab(storeTab);
		}
		ServerNetwork::Instance()->getSender()->syncBirdSimulatorPersonStoreTab(rakssId, storeTab);
	}

	static void sendBirdAddMessage(ui64 rakssId, const char* message)
	{
		ServerNetwork::Instance()->getSender()->sendBirdAddMessage(rakssId, message);
	}

	static int random(LORD::String name, int begin, int end)
	{
		return NewRandom::Instance()->random(begin, end, name);
	}

	static void changeGuideArrowStatus(ui64 rakssId, Vector3 arrowPos, bool flag)
	{
		ServerNetwork::Instance()->getSender()->changeGuideArrowStatus(rakssId, arrowPos, flag);
	}

	static void sendCommonTipByType(ui64 rakssId, i32 commonTipType, const char* msg)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(rakssId, 0, (ResultCode)commonTipType, msg);
	}

	static void addWallText(ui64 rakssId, i32 type, i32 timeLeft, String text, Vector3 textPos, float scale, float yaw, float pitch, std::vector<float> color)
	{
		// type: 0 -> normal message : without time countDown and text can not auto rotation,
		//       1 -> i18n message   : with time countDown and text can auto rotation,
		//		 2 -> normal message : without time countDown and text can auto rotation.
		//		 3 -> i18n message   : without time countDown and text can auto rotation.
		ServerNetwork::Instance()->getSender()->addWallText(rakssId, type, timeLeft, text, textPos, scale, yaw, pitch, color);
	}

	static void deleteWallText(ui64 rakssId, Vector3 textPos)
	{
		ServerNetwork::Instance()->getSender()->deleteWallText(rakssId, textPos);
	}

	static void showAddCarryGuide(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->showAddCarryGuide(rakssId);
	}

	static void sendBirdCheckinData(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendCheckinData(rakssId, data);
	}

	static void sendCheckinData(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendCheckinData(rakssId, data);
	}

	static void showBirdFeedGuide(ui64 rakssId)
	{
		ServerNetwork::Instance()->getSender()->showBirdFeedGuide(rakssId);
	}

	static void sendBirdShowWingTip(ui64 rakssId, i32 page)
	{
		ServerNetwork::Instance()->getSender()->sendBirdShowWingTip(rakssId, page);
	}
	static void addDefaultNotification(ui64 rakssId, String content)
	{
		ServerNetwork::Instance()->getSender()->sendDefaultNotification(rakssId, content);
	}

	static void notifyKills(ui64 rakssId, String icon, uint soundType, i32 stay, i32 animate)
	{
		ServerNetwork::Instance()->getSender()->notifyKills(rakssId, icon, soundType, stay, animate);
	}

	static void sendSupplyTip(ui64 rakssId, int type, String icon, int value)
	{
		ServerNetwork::Instance()->getSender()->sendSupplyTip(rakssId, type, icon, value);
	}

	static void sendKeepItemAndReviveCountDown(ui64 rakssId, ui32 second, int type)
	{
		ServerNetwork::Instance()->getSender()->sendKeepItemAndReviveCountDown(rakssId, second, type);
	}

	static void sendSetFlying(ui64 rakssId)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer)
		{
			auto entityPlayer = clientPeer->getEntityPlayer();
			entityPlayer->capabilities.isFlying = true;
			Server::Instance()->getNetwork()->getSender()->sendSetFlying(rakssId, entityPlayer);
			Server::Instance()->getNetwork()->getSender()->sendSetFlyingToTrackingPlayers(entityPlayer);
		}
	}

	static void setQuicklyBuildBlock(int cost, int num)
	{
		LogicSetting::Instance()->setQuicklyBuildBlockCost(cost);
		LogicSetting::Instance()->setQuicklyBuildBlockNum(num);
	}

	static void setSwordBreakBlock(bool sign)
	{
		LogicSetting::Instance()->setSwordBreakBlock(sign);
	}

	static void setToolDurable(bool sign)
	{
		LogicSetting::Instance()->setToolDurable(sign);
	}

	static void setAllowUseItemStatus(ui64 rakssId, int status)
	{
		Server::Instance()->getNetwork()->getSender()->sendAllowUseItemStatus(rakssId, status);
	}

	static void sendUpdateGuideData(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendUpdateGuideData(rakssId, data);
	}

	static void setRenderYawThresholdAndScale(float renderYawThreshold, float renderYawScale)
	{
		LogicSetting::Instance()->setRenderYawThreshold(renderYawThreshold);
		LogicSetting::Instance()->setRenderYawScale(renderYawScale);
	}

	static void setDeathAnimEffect(String deathAnimEffect)
	{
		LogicSetting::Instance()->setDeathAnimEffect(deathAnimEffect);
	}

	static void setPlayerActionArea(ui64 rakssId, std::vector<ActionArea> areas)
	{
		Server::Instance()->getNetwork()->getSender()->sendPlayerActoinArea(rakssId, areas);
	}

	static void showBlockCityCommonTip(ui64 rakssId, std::vector<BlockCityPayment> pays, i32 type, const String &msg)
	{
		vector<BlockCityPayment>::type newPays = vector<BlockCityPayment>::type();
		for (auto pay : pays)
		{
			newPays.push_back(pay);
		}
		Server::Instance()->getNetwork()->getSender()->showBlockCityCommonTip(rakssId, newPays, type, msg);
	}

	static void sendBroadcastMessage(ui64 rakssId, i32 messageType, String content)
	{
		ServerNetwork::Instance()->getSender()->sendBroadcastMessage(rakssId, messageType, content);
	}

	static void sendShowRotateView(ui64 rakssId, i32 type, i32 id)
	{
		Server::Instance()->getNetwork()->getSender()->sendShowRotateView(rakssId, type, id);
	}

	static void updateBlockCityItems(ui64 rakssId, std::vector<BlockCityStoreItem> items)
	{
		vector<BlockCityStoreItem>::type newItem = {};
		for (auto& item : items)
		{
			newItem.push_back(item);
		}

		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer && clientPeer->getEntityPlayer())
		{
			clientPeer->getEntityPlayer()->m_blockCity->updateItems(newItem);
		}
		Server::Instance()->getNetwork()->getSender()->updateBlockCityItems(rakssId, newItem);
	}

	static void syncBlockCityData(ui64 rakssId)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer && clientPeer->getEntityPlayer())
		{
			clientPeer->getEntityPlayer()->getBlockCity()->onUpdate(clientPeer->getEntityPlayer());
		}
	}

	static void setToolDurability(i32 itemId, i32 maxDamage)
	{
		Item* pItem = Item::getItemById(itemId);
		if (pItem)
		{
			if (maxDamage > 0)
			{
				pItem->setMaxDamage(maxDamage);
			}
		}
	}

	static void sendUpdateSkyBlockMainLineTaskData(ui64 rakssId, int lv, int progressesLevel, bool dareIsUnLock, String data, int taskType)
	{
		ServerNetwork::Instance()->getSender()->sendUpdateSkyBlockMainLineTaskData(rakssId, lv, progressesLevel, dareIsUnLock, data, taskType);
	}

	static void sendSkyBlockDareTaskInfo(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendSkyBlockDareTaskInfo(rakssId, data);
	}

	static void sendSetMoveState(ui64 rakssId, i32 state)
	{
		ServerNetwork::Instance()->getSender()->sendSetMoveState(rakssId, state);
	}

	static void sendSkyBlockRankData(ui64 rakssId, String data)
	{
		ServerNetwork::Instance()->getSender()->sendSkyBlockRankData(rakssId, data);
	}

	static void sendCanUseItemBucket(ui64 rakssId, bool canUseItemBucket)
	{
		ServerNetwork::Instance()->getSender()->sendCanUseItemBucket(rakssId, canUseItemBucket);
	}

	static void sendSkyblockShowAppShopPage(ui64 rakssId,i32 pageId)
	{
		ServerNetwork::Instance()->getSender()->sendSkyblockShowAppShopPage(rakssId, pageId);
	}

	static void notifyGetGoodsByItemId(ui64 rakssId, i32 itemId, i32 meta, i32 num, String img)
	{
		ServerNetwork::Instance()->getSender()->notifyGetGoodsByItemId(rakssId, itemId, meta, num, img);
	}

	static void setUseMovingEnderChest(bool isUse)
	{
		LogicSetting::Instance()->setUseMovingEnderChest(isUse);
	}

	static void setHideArmor(bool hideArmor)
	{
		LogicSetting::Instance()->setHideArmor(hideArmor);
	}

	static void showFloatTip(ui64 rakssId, String msg)
	{
		ServerNetwork::Instance()->getSender()->sendShowFloatTip(rakssId, msg);
	}

	static void deleteBlockCrops(Vector3i pos)
	{
		Server::Instance()->getBlockCropsManager()->deleteBlockCrops(pos);
	}

	static void setMaxChangeInChunk(int num)
	{
		LogicSetting::Instance()->setMaxChangeInChunk(num);
	}

	static void doThrow(int entityId, const Vector3 & launchPoint, float rotationYaw, float rotationPitch, float speed, int remainingFlyTicks);

	static void sendBlockCityPackTab(ui64 rakssId, const BLOCKMAN::BlockCityHouseShopClassify & packTab)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer && clientPeer->getEntityPlayer())
		{
			clientPeer->getEntityPlayer()->m_blockCity->setPackTab(packTab);
		}
		ServerNetwork::Instance()->getSender()->syncBlockCityPackTab(rakssId, packTab);
	}

	static void sendPlayingPraiseAction(ui64 rakssId, int duration)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
		if (clientPeer)
		{
			Server::Instance()->getNetwork()->getCommonSender()->sendPlayingPraiseAction(rakssId, clientPeer->getEntityRuntimeID(), duration);
		}
	}

	static void setSeparateCameraControl(bool isSeparate)
	{
		LogicSetting::Instance()->setSeparateCameraControl(isSeparate);
	}

	static void setUseLocalScaleInHand(bool useLocal)
	{
		LogicSetting::Instance()->setUseLocalScaleInHand(useLocal);
	}

	static void setGameWeather(int weatherType)
	{
		if (weatherType != LogicSetting::Instance()->getGameWeather())
		{
			LogicSetting::Instance()->setGameWeather(weatherType);
			if (ServerNetwork::Instance() && ServerNetwork::Instance()->getSender())
			{
				ServerNetwork::Instance()->getSender()->broadcastGameWeather(weatherType);
			}
		}
	}

	static void setLavaDamageSource()
	{
		DamageSource::lava->setDamageBypassesArmor();
		DamageSource::inFire->setDamageBypassesArmor();
	}

	static bool consumeCmd(ui64 id, const String& msg)
	{
		auto clientPeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(id);
		if (clientPeer)
			return GameCmdMgr::consumeCmd(clientPeer, msg);

		return false;
	}
};
