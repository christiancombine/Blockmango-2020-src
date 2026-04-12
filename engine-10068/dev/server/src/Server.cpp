#include "Server.h"
#include "World/World.h"
#include "World/GameSettings.h"
#include "Chunk/Chunk.h"
#include "World/Section.h"
#include "World/WorldSettings.h"
#include "Block/BlockManager.h"
#include "Util/Concurrent/Promise.h"
#include "Util/Facing.h"
#include "TileEntity/TileEntity.h"
#include "Entity/Attributes.h"
#include "Entity/DamageSource.h"
#include "Entity/Enchantment.h"
#include "Entity/EntityLivingBase.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/Potion.h"
#include "Item/PotionManager.h"
#include "Inventory/InventoryPlayer.h"
#include "Item/ItemStack.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Inventory/Container.h"
#include "Inventory/CraftingManager.h"
#include "TileEntity/TileEntityInventory.h"
#include "Object/Root.h"
#include "Network/ServerNetwork.h"
#include "Network/GameCmdMgr.h"
#include <iostream>
#include "Global.h"
#include "Util/CommonEvents.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Item/ItemCrafter.h"
#include "Util/SecTimer.h"
#include "Script/GameServerEvents.h"
#include "RoomManager.h"
#include "Script/ScriptManager.h"
#include "Script/Setting/ScriptSetting.h"
#include "Util/UThread.h"
#include "Setting/MultiLanTipSetting.h"
#include "Misc/IdMapping.h"
#include "Block/BlockLoader.h"
#include "GameVersionDefine.h"
#include "Setting/DoorSetting.h"
#include "Setting/CarSetting.h"
#include "Setting/LogicSetting.h"
#include "Setting/AppItemsSetting.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/ActorSizeSetting.h"
#include "Setting/MonsterSetting.h"
#include "Setting/CropsSetting.h"
#include "Setting/GunExtraAttrSetting.h"
#include "Setting/GrenadeSetting.h"
#include "GameAnalytics/GameAnalyticsSetting.h"
#include "Inventory/CoinManager.h"
#include "Blockman/Trigger/TriggerModuleServer.h"
#include "Behaviac/behaviac_generated/types/behaviac_types.h"
#include "DB/RedisHttpRequest.h"
#include "Common.h"
#include "Setting/RanchSetting.h"
#include "Setting/BulletinSetting.h"
#include "Setting/FruitsSetting.h"
#include "Util/NewRandom.h"
#include "Util/PlayerPrefs.h"
#include "Setting/DecorationSetting.h"
#include "Setting/SceneSetting.h"
#include "Setting/BlockSetting.h"
#include "Setting/RecipeTabSetting.h"
#include "Setting/FurnaceSetting.h"
#include "Setting/SellItemSetting.h"
#include "Setting/AbandonItemSetting.h"
#include "Setting/MountSetting.h"
#include "Setting/CustomArmorSetting.h"
#include "Item/Recipes.h"
#include "Chat/ChatComponent.h"
#include "Setting/ModuleBlockSetting.h"
#include "Entity/PathFinderHelper.h"
#include "Setting/ConfigurableBlockSetting.h"
#include "Setting/TestPlayerSetting.h"
#include "Setting/CustomItemSetting.h"
#include "BtTreeController.h"

using namespace BLOCKMAN;

#if LORD_PLATFORM == LORD_PLATFORM_LINUX
#include <unistd.h>
#include <string.h>
#endif

#define SERVER_QUITTING_FIRST_TIP_KEY "system.message.server.quitting.first.time"
#define SERVER_QUITTING_SECOND_TIP_KEY "system.message.server.quitting.second.time"
#define SERVER_QUITTING_THIRD_TIP_KEY "system.message.server.quitting.third.time"
#define SERVER_QUITTING_TIP_KEY "system.message.server.quitting"

#define PROJECT_ROOT_PATH "blockmango-client"

#define DISABLE_ROOM 0

#define INIT_POS_X 4
#define INIT_POS_Y 60
#define INIT_POS_Z -17

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

const String Server::WEB_HTTP_SECRET = "pq0194mxoqfh48L362G6R09T737E273X";

#if LORD_PLATFORM == LORD_PLATFORM_LINUX
std::string Server::getCurrDir() {
	char buf[1024] = { 0 };
	if (!getcwd(buf, 1024)) {
		LordLogError("getcwd fail, error: %s", strerror(errno));
		std::abort();
	}
	std::string currDir = buf;

	return currDir;
}
#endif

std::string Server::getGameName()
{
#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
	return "gameName-debug";
#endif

	if (!ScriptSetting::m_mapName.empty())
	{
		return ScriptSetting::m_mapName;
	}
	else
	{
		return "gameName-debug";
	}
}

Server::Server()
{
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
    signal(SIGPIPE, SIG_IGN);

    sigset_t signal_mask;
    sigemptyset (&signal_mask);
    sigaddset (&signal_mask, SIGPIPE);
    int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
    if (rc != 0) {
        printf("block sigpipe error\n");
    }
	m_enableRoom =true;
#else
	m_enableRoom = (DISABLE_ROOM == 0);
#endif
	
}

Server::~Server()
{
}

void Server::initRecipes()
{
	RecipeTabSetting::Instance()->loadRecipe();
}

void Server::initializeWorldModule()
{
	UThread::setThisThreadName("EntryThread");
	UThread::setThisThreadPriority(ThreadPriority::HIGH);
	LordLogInfo("EntryThread started, id: %d", UThread::getThisThreadNativeId());

	BlockManager::initialize();
	TileEntity::initialize();

	SharedMonsterAttributes::initialize();
	DamageSource::initialize();
	Enchantment::initialize();
	EnchantmentHelper::initialize();
	EntityLivingBase::initialize();

	ToolMaterial::initialize();
	ArmorMaterial::initialize();
	Potion::initialize();
	PotionManager::initialize();
	Item::initialize();

	CraftingManager* pCrafting = LordNew CraftingManager();
	initRecipes();
	LordNew ItemCrafter;
	PotionHelper::initialize();
	TileEntityBeacon::initialize();
	Facing::initialize();
	GameCmdMgr::init();
	FurnaceRecipes::smelting()->loadRecipes();
}

void Server::unInitializeWorlModule()
{
	Facing::unInitialize();
	LordDelete(FurnaceRecipes::smelting());
	ItemCrafter* pItemCrafter = ItemCrafter::Instance();
	LordSafeDelete(pItemCrafter);
	CraftingManager* pCrafting = CraftingManager::Instance();
	LordSafeDelete(pCrafting);

	Item::unInitialize();
	PotionManager::unInitialize();
	Potion::unInitialize();
	ArmorMaterial::unInitialize();
	ToolMaterial::unInitialize();

	EntityLivingBase::unInitialize();
	EnchantmentHelper::unInitalize();
	Enchantment::unInitialize();
	DamageSource::unInitialize();
	SharedMonsterAttributes::unInitialize();

	BlockManager::unInitialize();
}

void Server::init(const RoomGameConfig& rgConfig)
{
	tickPerHeartBeat = rgConfig.heartbeatInterval;
	m_config = rgConfig;
	m_initPos.x = INIT_POS_X;
	m_initPos.y = INIT_POS_Y;
	m_initPos.z = INIT_POS_Z;

	LordNew(Root);
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	Root::Instance()->setWriteablePath(""); // use current working dir
#else
	String logDir = String(m_config.logDir.c_str());
	if (!PathUtil::IsEndWithSeperator(logDir)) {
		logDir += "/";
	}
	Root::Instance()->setWriteablePath(logDir.c_str());
#endif

	Root::RootCfg cfg;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	cfg.rootPath = String(".\\");
#else 
	cfg.rootPath = String("./");
#endif
	cfg.resCfgFile = cfg.rootPath + "resource.cfg";
	cfg.bEditorMode = false;
	cfg.bFixedAspect = false;
	cfg.bClientMode = false;
	cfg.bForceLogAppend = false;
	cfg.fAspectRadio = 9.0f / 16.0f;

	Root::Instance()->initialize(cfg);

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	m_config.scriptDir = PathUtil::ConcatPath(ScriptSetting::loadGameScriptDir("", false), "main").c_str();
	m_config.gameName = Server::getGameName();
	String mapDir = cfg.rootPath;
	mapDir += "\\map\\";
	mapDir += m_config.gameName.c_str();
	mapDir += "\\";
	m_config.mapDir = mapDir.c_str();
#else
	String mapDir = String(m_config.mapDir.c_str());
	if (!PathUtil::IsEndWithSeperator(mapDir)) {
		mapDir += "/";
	}
#endif
	LordLogInfo("using map dir:%s", mapDir.c_str());
	Root::Instance()->setMapPath(mapDir);

	initSetting();
	initializeWorldModule();
	LordLogInfo("current engine version: %d", EngineVersionSetting::getEngineVersion());
	m_config.debugPrint();

	LordNew PlayerPrefs();
	(LordNew TriggerModuleServer)->initialize();

	LordNew BlockLoader();
	// load game custom blocks
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	String engineSettingPath = "Media/Setting";
	String gameSettingDir = StringUtil::Replace(ScriptSetting::m_scriptPath, "ServerGame", "GameSetting");
#else
	String engineSettingPath = "";
	ScriptSetting::loadGameScriptDir(Server::Instance()->getConfig().gameType.c_str(), true, false);
	String gameSettingDir = StringUtil::Replace(ScriptSetting::m_scriptPath, "ClientGame", "GameSetting");
	gameSettingDir = StringUtil::Replace(gameSettingDir, "Media/Scripts/", "");
#endif
	gameSettingDir = PathUtil::ConcatPath(Root::Instance()->getRootPath(), gameSettingDir);
	if (BLOCKMAN::IdMapping::readConfig(PathUtil::ConcatPath(gameSettingDir, "id_mappings.ini")) != -1)
		BLOCKMAN::BlockLoader::Instance()->loadCustomBlocksInMap(gameSettingDir);
	// load map custom blocks
	IdMapping::readConfig(PathUtil::ConcatPath(mapDir, "id_mappings.ini"));
	BlockLoader::Instance()->loadCustomBlocksInMap(mapDir);

	LordNew PathFinderHelper();
	LordNew NewRandom();
	LordNew CoinManager();
	LordNew LogicSetting();
	LordNew AppItemsSetting();
	LordNew CustomItemSetting();
	LordNew BtTreeController();
	initBehaviac();

	m_tickThread = LORD::make_shared<UThread>("ServerTickThread", &Server::tick, this);
	m_tickThread->setPriority(ThreadPriority::HIGH);

	String scriptDir = String(m_config.scriptDir.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	String scriptBaseDir = StringUtil::Replace(scriptDir, "ServerGame", "BaseGame");
	String loaderPath = PathUtil::ConcatPath(scriptBaseDir, "Loader.lua");
	if (PathUtil::IsFileExist(loaderPath))
		LuaEngine::getInstance()->Load(loaderPath.c_str());
#else 
	String loaderPath = PathUtil::ConcatPath(scriptDir, "Loader.lua");
	if (PathUtil::IsFileExist(loaderPath))
		LuaEngine::getInstance()->Load(loaderPath.c_str());
#endif
	
	String tmpStr = String(m_config.gameName.c_str());
	m_serverWorld = ServerWorld::createWorld(tmpStr);

	tmpStr = String(m_config.monitorAddr.c_str());
	StringArray arr = StringUtil::Split(tmpStr, ":");

	if (m_enableRoom)
	{
		m_roomManager = LORD::make_shared<RoomManager>(std::string(arr[0].c_str()), StringUtil::ParseInt(arr[1]), m_config);
		m_roomManager->tick(askedToQuit());
	}

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	if (scriptDir.length() > 6)
	{
		String commonPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media", "Scripts", "ServerGame", "Common");
		ScriptManager::Instance()->loadScript(commonPath.c_str(), false);

		String scriptBaseDir = StringUtil::Replace(scriptDir, "ServerGame", "BaseGame");
		LuaEngine::getInstance()->addScriptDir(scriptBaseDir.c_str());

		ScriptManager::Instance()->loadScript(scriptDir.c_str(), true);
		m_scriptLoaded = true;
		LordLogInfo("script loadded on windows!!!!!!");
		LordLogInfo("using script base dir:%s", scriptBaseDir.c_str());
		LordLogInfo("using script dir:%s", scriptDir.c_str());
	}
	else
	{
		LordLogInfo("script disable on windows!!!!!!");
	}
#else // LORD_PLATFORM != LORD_PLATFORM_WINDOWS
	if (scriptDir.length() > 0 && !m_config.isDebug)
	{
		ScriptManager::Instance()->loadScript(scriptDir.c_str(), true);
		m_scriptLoaded = true;
		LordLogInfo("script loadded on linux!!!!!!");
		LordLogInfo("using script dir:%s", scriptDir.c_str());
	}
	else
	{
		LordLogInfo("script disable on linux!!!!!!");
	}
#endif // LORD_PLATFORM

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	// ---> windows use test server ip and port
	m_config.gameIp = ScriptSetting::m_serverIp.c_str();
	m_config.serverPort = ScriptSetting::m_serverPort;
#endif

	m_serverNetwork = LORD::make_shared<ServerNetwork>(m_config.gameIp.c_str(), m_config.serverPort);
	m_pBlockDoorManager = LORD::make_shared<BlockDoorManager>(m_serverWorld);
	m_pBlockCropsManager = LORD::make_shared<BlockCropsManager>(m_serverWorld);
	m_pBlockFruitsManager = LORD::make_shared<BlockFruitsManager>(m_serverWorld);
	m_redisHttpRequest = LORD::make_shared<RedisHttpRequest>(m_config.dbHttpServiceUrl);

	std::string configPath = mapDir.c_str();
	configPath += "config.yml";

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	auto players = TestPlayerSetting().getPlayers();
	m_config.maxPlayers = players.size();
#endif

	SCRIPT_EVENT::GameInitEvent::invoke(configPath.c_str(), "v1", m_serverWorld, m_config);

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	// create game lock file, to support the launch of other games
	String lockFile = m_gameType + ".lock";
	if (!PathUtil::IsFileExist(lockFile))
		CreateFile(lockFile.c_str(), 0, 0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
#endif

	initTestAttr();

	m_subscriptionGuard.add(BLOCKMAN::ServerNetworkTimeoutEvent::subscribe([this](ui64 rakssid)-> bool {
		return true;
	}));
}

void Server::initTestAttr()
{
	if (!m_enableRoom)
		return;

	if (!EngineVersionSetting::canUseCmd()) // release server return
		return;

	auto players = TestPlayerSetting().getPlayers();
	int index = 0;
	for (auto player : players)
	{
		UserAttrInfo attrInfo = UserAttrInfo();
		attrInfo.isTest = true;
		attrInfo.userId = player.userId;
		attrInfo.picUrl = player.picUrl.c_str();
		attrInfo.team = player.team;
		attrInfo.regionId = 1001;
		attrInfo.targetUserId = player.targetUserId;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		attrInfo.manorId = player.manorId;
		attrInfo.isRobot = player.isRobot;
		attrInfo.robotId = player.robotId;
		attrInfo.followEnterType = player.followEnterType;
#else
		attrInfo.manorId = -1;
#endif
		attrInfo.sex = (index % 2 == 0) ? 1 : 2;
		if (attrInfo.sex == 1)
		{
			attrInfo.faceId = Server::Instance()->getConfig().isChina ? 26 : 1;
			attrInfo.hairId = Server::Instance()->getConfig().isChina ? 19 : 1;
			attrInfo.topsId = Server::Instance()->getConfig().isChina ? 5 : 1;
			attrInfo.pantsId = Server::Instance()->getConfig().isChina ? 5 : 1;
			attrInfo.shoesId = Server::Instance()->getConfig().isChina ? 24 : 1;
		}
		else
		{
			attrInfo.faceId = Server::Instance()->getConfig().isChina ? 12 : 1;
			attrInfo.hairId = Server::Instance()->getConfig().isChina ? 13 : 1;
			attrInfo.topsId = Server::Instance()->getConfig().isChina ? 5 : 1;
			attrInfo.pantsId = Server::Instance()->getConfig().isChina ? 5 : 1;
			attrInfo.shoesId = Server::Instance()->getConfig().isChina ? 23 : 1;
		}
		attrInfo.defaultIdle = 0;
		attrInfo.unlockedSelectableActions = { 1, 2, 11, 12, 27 };
		m_roomManager->onUserAttr(attrInfo);
		index++;
	}
}

void Server::unInit()
{
	uninitSetting();
	BtTreeController::Instance()->clearBtTrees();
	LordDelete(BlockLoader::Instance());
	LordDelete CoinManager::Instance();
	LordDelete LogicSetting::Instance();
	LordDelete AppItemsSetting::Instance();
	m_serverWorld->destroy();
	LordSafeDelete(m_serverWorld);
	this->unInitializeWorlModule();
	TriggerModule::Instance()->uninitialize();
	LordDelete(TriggerModule::Instance());
	Root* root = Root::Instance();
	root->destroy();
	LordSafeDelete(root);
	LordDelete PathFinderHelper::Instance();
	LordDelete PlayerPrefs::Instance();
	LordDelete BtTreeController::Instance();
	uninitBehaviac();
}

void Server::stopThread()
{
	m_tickThread->stopSync();
	//m_serverDB->stop();
	//m_serverRedisDB->stop();
}

void Server::start()
{
	this->m_tickThread->start();
	LordLogInfo("ServerTickThread started, id: %d", m_tickThread->getNativeId());
	m_serverNetwork->host(1000);
}

void Server::waitForStopEvent()
{
	if (m_enableRoom)
	{
		RoomGameConfig* rg = m_roomManager->getRoomGameConfig();
		m_roomManager->getRoomClient()->sendGameStatus(
			rg->gameId,
			rg->maxPlayers, 0,
			static_cast<int>(RoomGameStatus::PREPARING)
		);
		LordLogInfo("send game status preparing to room server");
	}

	m_stopEvent.wait();
}

void Server::checkAskedToQuit() {
	if (!askedToQuit()) {
		return;
	}

	ui32 now = LORD::Root::Instance()->getCurrentTime();
	if (m_askedToQuitTime == 0) {
		m_askedToQuitTime = now;
	}
	
	if (m_askedToQuitTipCnt == 0 && m_askedToQuitTime != 0) {
		m_serverNetwork->broadCastMsg(0, (ui8)ChatComponentNotification::Type::TIMING_CENTER, 5, SERVER_QUITTING_FIRST_TIP_KEY);
		m_askedToQuitTipCnt++;
		LordLogInfo("server quitting, first tip message sent");
	}
	else if (m_askedToQuitTipCnt == 1 && now - m_askedToQuitTime > 30000/*ms*/) {
		m_serverNetwork->broadCastMsg(0, (ui8)ChatComponentNotification::Type::TIMING_CENTER, 5, SERVER_QUITTING_SECOND_TIP_KEY);
		m_askedToQuitTipCnt++;
		LordLogInfo("server quitting, second tip message sent");
	} 
	else if (m_askedToQuitTipCnt == 2 && now - m_askedToQuitTime > 50000/*ms*/) {
		m_serverNetwork->broadCastMsg(0, (ui8)ChatComponentNotification::Type::TIMING_CENTER, 5, SERVER_QUITTING_THIRD_TIP_KEY);
		m_askedToQuitTipCnt++;
		LordLogInfo("server quitting, third tip message sent");
	}
	else if (m_askedToQuitTipCnt == 3 && now - m_askedToQuitTime > 60000/*ms*/) {
		m_serverNetwork->broadCastServerQuitting(SERVER_QUITTING_TIP_KEY);
		m_askedToQuitTipCnt++;
		LordLogInfo("server quitting, ask client to logout");
	}
	else if (m_askedToQuitTipCnt == 4 && now - m_askedToQuitTime > 65000/*ms*/) {
		m_serverNetwork->closeAllSession();
		m_serverNetwork->setAskToQuit();
		BtTreeController::Instance()->clearBtTrees();
	}
}

void Server::tick(StopFlag shouldStop)
{
	int tick = 0;
	try {
		// todo  complate tick 
		const LORD::ui32 expectedFrameTime = 50; // ms
		LORD::ui32 startTime = 0;
		LORD::ui32 endTime = 0;
		while (!shouldStop)
		{
			tick++;

			if (tick % 16 == 0) {
				checkAskedToQuit();
			}

			sendHeartBeat();
			Root::Instance()->serverTick();
			startTime = LORD::Root::Instance()->getCurrentTime();	
			PROMISE::runCallbacks();
			LORD::ui32 t0 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(startTime, t0, 10, "PROMISE::runCallbacks");

			if (m_enableRoom) {
				m_roomManager->tick(askedToQuit());
			}

			LORD::ui32 t1 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t0, t1, 10, "m_roomManager::tick");

			m_serverNetwork->tick();
			LORD::ui32 t2 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t1, t2, 30, "m_serverNetwork::tick");

			m_serverWorld->tick();
			SkillItemSetting::onUpdate();
			LORD::ui32 t3 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t2, t3, 60, "m_serverWorld::tick");

			m_pBlockDoorManager->tick();
			LORD::ui32 t4 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t3, t4, 10, "m_pBlockDoorManager::tick");

			m_pBlockCropsManager->updateCrops();
			LORD::ui32 t5 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t4, t5, 10, "m_pBlockCropsManager::updateCrops");

			LORD::ui32 t6 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t5, t6, 10, "m_mysqlHttpRequest::updateWriteRequest");

			m_redisHttpRequest->UpdateWriteRequest();
			LORD::ui32 t7 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t6, t7, 10, "m_redisHttpRequest::updateWriteRequest");

			SecTimer::update();
			LORD::ui32 t8 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t7, t8, 10, "SecTimer::update");

			m_pBlockFruitsManager->updateFruits();
			LORD::ui32 t9 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t8, t9, 10, "m_pBlockFruitsManager::updateFruits");

			LORD::ui32 t10 = LORD::Root::Instance()->getCurrentTime();
			behaviac::Workspace::GetInstance()->SetDoubleValueSinceStartup(startTime);
			behaviac::Workspace::GetInstance()->SetFrameSinceStartup(tick);
			BtTreeController::Instance()->onUpdate();
			GameCommon::logIfExpired(t9, t10, 20, "BtTreeController::onUpdate");

			int asyncTaskProcessedCnt = 0;
			bool asyncTaskDelayed = false;
			while (m_asyncCall.realCall()) {
				++asyncTaskProcessedCnt;

				LORD::ui32 ct = LORD::Root::Instance()->getCurrentTime();
				if (ct - t10 >= 60) {
					asyncTaskDelayed = true;
					break;
				}
			}
			if (asyncTaskDelayed) {
				LordLogWarning("async task delayed, asyncTaskProcessedCnt=%d, asyncTaskLeftNum=%u", 
					asyncTaskProcessedCnt, m_asyncCall.size());
			}

			m_serverNetwork->flush();

			endTime = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t10, endTime, 60, "m_asyncCall.realCall");
			
			m_serverFrameTime = endTime - startTime;
			if (m_enableRoom) {
				if (m_serverFrameTime >= 500) {
					m_roomManager->getRoomClient()->sendServerFrameTime(m_serverFrameTime);
				}
			}
			if (m_serverFrameTime > expectedFrameTime) {
				if (m_serverFrameTime >= 60)
				{
					LordLogWarning("===runtime=== Server::tick slow, frame time %u ms", m_serverFrameTime);
				}
			} else if (m_serverFrameTime < expectedFrameTime) {
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				Sleep(expectedFrameTime - m_serverFrameTime);
#else
				usleep((expectedFrameTime - m_serverFrameTime) * 1000);
#endif
				m_serverFrameTime = expectedFrameTime;
			}
			endTime = LORD::Root::Instance()->getCurrentTime();
			m_serverFrameTime = endTime - startTime;
		}
		m_serverNetwork->printDataPacket();

		uninitMainLogicThread();

		this->m_stopEvent.setAll(); 
	}
	catch (const std::exception& e) {
		LordLogError("tick thread exception occur: %s", e.what());
		throw;
	}
}

void Server::sendHeartBeat()
{
	ui32 nowTime = LORD::Root::Instance()->getCurrentTime();
	if (nowTime - m_lastSendHeartBeatTime > ui32(tickPerHeartBeat * 1000))
	{
		if (!m_isRaknetAlive)
		{
			LordLogError("m_isRaknetAlive == false, stop sending HeartBeat to room service!!!!");
			return;
		}
		m_lastSendHeartBeatTime = nowTime;
		m_isRaknetAlive = false;
		int count = m_serverNetwork->getPlayerNumber();
		if (m_enableRoom)
		{
			m_roomManager->getRoomClient()->sendHeartBeat(count);
		}

#if LORD_PLATFORM == LORD_PLATFORM_LINUX
		LordLogInfo("sendHeartBeat to room, tickPerHeartBeat %d, player count %d, game_id: %s",
			tickPerHeartBeat, count, m_config.gameId.c_str());
#endif	
	}
}

void Server::initBehaviac()
{
	static bool hasInit = false;
	if (hasInit)
		return;
#ifdef _DEBUG_BEHAVIAC
	behaviac::Config::SetLogging(true);
	behaviac::Config::SetSocketing(true);
	behaviac::Config::SetSocketPort(60636);
	behaviac::Config::SetSocketBlocking(true);
#else
	behaviac::Config::SetProfiling(false);
	behaviac::Config::SetLogging(false);
	behaviac::Config::SetLoggingFlush(false);
	behaviac::Config::SetSocketing(false);
	behaviac::Config::SetSocketBlocking(false);
	behaviac::Config::SetHotReload(false);
#endif
	std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "bt/exported")).c_str();
	behaviac::Workspace::GetInstance()->SetFilePath(path.c_str());
	behaviac::Workspace::GetInstance()->SetFileFormat(behaviac::Workspace::EFF_xml);
	LordLogInfo("BEHAVIAC_BUILD_CONFIG_STR is %s", BEHAVIAC_BUILD_CONFIG_STR);

//#ifdef _DEBUG
//	behaviac::Config::SetProfiling(false);
//	behaviac::Config::SetLogging(false);
//	behaviac::Config::SetSocketing(false);
//	behaviac::Config::SetHotReload(false);
//#endif

	hasInit = true;
}

void Server::uninitBehaviac()
{
	behaviac::Workspace::GetInstance()->Cleanup();
}

void Server::uninitMainLogicThread()
{
	if (m_redisHttpRequest)
	{
		m_redisHttpRequest->uninitDB();
	}
}

void Server::stop()
{
	if (m_enableRoom)
	{
		m_roomManager->getRoomClient()->sendDisconnect(m_config.gameId);
		LordLogInfo("send disconnect to room, game_id: %s", m_config.gameId.c_str());
	}

	setStopEvent();
}

void Server::startRedis()
{
	//m_serverRedisDB->initDB();
	m_redisHttpRequest->initDB();
}

void Server::setInitPos(const Vector3i & pos)
{
	m_initPos.x = pos.x;
	m_initPos.y = pos.y;
	m_initPos.z = pos.z;
}

void Server::initSetting()
{
	MultiLanTipSetting::loadSetting(false);
	MultiLanTipSetting::loadMapSetting(false);
	EngineVersionSetting::loadSetting();
	GameCmdMgr::setCmdUsable(EngineVersionSetting::canUseCmd());
	g3::log_levels::setHighest(EngineVersionSetting::getLogLevel());
	DoorSetting::loadSetting(false);
	CarSetting::loadSetting(false);
	MountSetting::loadSetting(false);
	CropsSetting::loadSetting(false);
	CropsSetting::loadMapSetting(false);
	ActorSizeSetting::loadSetting(false);
	ActorSizeSetting::loadMapSetting(false);
	GrenadeSetting::loadSetting(false);
	RanchSetting::loadSetting(false);
	BulletinSetting::loadSetting(false, false);
	BulletinSetting::loadSetting(false, true);
	FruitsSetting::loadSetting(false);
	GameAnalyticsSetting::loadSetting(false);
	DecorationSetting::loadSetting(false);
	LordNew BlockSetting();
	BlockSetting::Instance()->loadSetting(false);
	LordNew RecipeTabSetting();
	RecipeTabSetting::Instance()->loadSetting(false);
	LordNew FurnaceSetting();
	FurnaceSetting::Instance()->loadSetting(false);
	LordNew SellItemSetting();
	SellItemSetting::Instance()->loadSetting(false);
	LordNew CustomArmorSetting();
	CustomArmorSetting::Instance()->loadSetting(false);
	LordNew SceneSetting();
	SceneSetting::Instance()->loadSetting(false);
	LordNew AbandonItemSetting();
	AbandonItemSetting::Instance()->loadSetting(false);
	ModuleBlockSetting::loadSetting(false);
	LordNew ConfigurableBlockSetting(false);
}

void Server::uninitSetting()
{
	MultiLanTipSetting::unloadSetting();
	GunSetting::unloadSetting();
	BulletClipSetting::unloadSetting();
	DoorSetting::unloadSetting();
	MountSetting::unloadSetting();
	CarSetting::unloadSetting();
	MonsterSetting::unloadSetting();
	CropsSetting::unloadSetting();
	SkillSetting::unloadSetting();
	SkillItemSetting::unloadSetting();

	ActorSizeSetting::unloadSetting();
	
	GunExtraAttrSetting::unloadSetting();
	GrenadeSetting::unloadSetting();
	
	RanchSetting::unloadSetting();
	BulletinSetting::unloadSetting();
	FruitsSetting::unloadSetting();

	GameAnalyticsSetting::unloadSetting();
	DecorationSetting::unloadSetting();

	LordDelete CustomItemSetting::Instance();
	BlockSetting::Instance()->unloadSetting();
	LordDelete BlockSetting::Instance();
	RecipeTabSetting::Instance()->unloadSetting();
	LordDelete RecipeTabSetting::Instance();
	FurnaceSetting::Instance()->unloadSetting();
	LordDelete FurnaceSetting::Instance();
	SellItemSetting::Instance()->unloadSetting();
	LordDelete SellItemSetting::Instance();
	CustomArmorSetting::Instance()->unloadSetting();
	LordDelete CustomArmorSetting::Instance();
	SceneSetting::Instance()->unloadSetting();
	LordDelete SceneSetting::Instance();
	ModuleBlockSetting::unloadSetting();
	AbandonItemSetting::Instance()->unloadSetting();
	LordDelete AbandonItemSetting::Instance();
	LordDelete ConfigurableBlockSetting::Instance();
}

void Server::setGameType(String& gameType)
{
	m_gameType = gameType;
	CarSetting::m_sbIsCarFree = GameTypeSetting::IsCarFree(m_gameType);
	GameAnalyticsSetting::setGameType(m_gameType);
	LogicSetting::Instance()->setGameType(GameTypeSetting::GetGameType(gameType));
}

ClientGameType Server::getClientGameType()
{
	return LogicSetting::Instance()->getGameType();
}

void Server::setServerInfo(std::string serverInfo)
{
	m_serverInfo = serverInfo;
}

 String Server::getDataServerUrl()
{
	return m_config.blockymodsRewardAddr.c_str();
}
