#include "game.h"
#include "Core.h"
#include "Common.h"
#include "ShellInterface.h"
#include "GameVersionDefine.h"
#include "GLES2Render.h"
#include "iniFileObj.h"
#include "External/cjson.h"
#include "Object/Root.h"
#include "Memory/MemTracker.h"
#include "Audio/SoundSystem.h"
#include "Audio/FModAudioEngine.h"
#include "Resource/ResourceManager.h"
#include "Resource/ResourcePack.h"
#include "Inventory/CoinManager.h"
#include "Misc/IdMapping.h"
#include "World/GameSettings.h"
#include "Chunk/ChunkService.h"
#include "Network/ClientHttpRequest.h"
#include "cTrigger/TriggerModuleClient.h"
#include "cBlock/BlockLoaderClient.h"
#include "cWorld/Blockman.h"
#include "cWorld/RankManager.h"
#include "cWorld/MapManager.h"
#include "cWorld/ImageManager.h"
#include "cBlock/cBlockManager.h"
#include "Util/ClientCmdMgr.h"
#include "Plugins/PluginManager.h"
#include "Plugins/GameClientEvents.h"
#include "Script/Setting/ScriptSetting.h"
#include "LuaRegister/Template/LuaEngine.h"

#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Render/RenderGlobal.h"

#include "Util/QueryObject.h"
#include "Util/ClientEvents.h"
#include "Util/md5.h"
#include "Util/LanguageManager.h"
#include "Util/Event.h"
#include "Util/HttpPromise.h"
#include "Util/Concurrent/Promise.h"
#include "Util/ClientDataReport.h"
#include "Util/SkillEffectManager.h"
#include "Util/FriendManager.h"
#include "Util/RedDotManager.h"
#include "Util/MailManager.h"
#include "Util/UserManager.h"
#include "Util/LanguageKey.h"
#include "Util/PlayerPrefs.h"
#include "Util/SecTimer.h"

#include "UI/GUISystem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIWindow.h"
#include "UI/Touch.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIRenderText.h"
#include "UI/GUIRenderManager.h"
#include "GUI/gui_tipDialog.h"
#include "Setting/MonsterSetting.h"
#include "Setting/TeamInfoSetting.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/LogicSetting.h"
#include "Setting/AppItemsSetting.h"
#include "Setting/SceneSetting.h"
#include "Setting/ScorePointSetting.h"
#include "Setting/DoorSetting.h"
#include "Setting/CarSetting.h"
#include "Setting/FilterWord.h"
#include "Setting/ActionStateSetting.h"
#include "Setting/RankLangSetting.h"
#include "Setting/MultiLanTipSetting.h"
#include "Setting/ActorSizeSetting.h"
#include "Setting/GunExtraAttrSetting.h"
#include "Setting/GrenadeSetting.h"
#include "Setting/RanchSetting.h"
#include "Setting/GuideSetting.h"
#include "Setting/CropsSetting.h"
#include "Setting/SkillSetting.h"
#include "Setting/BulletinSetting.h"
#include "Setting/GatherToolSetting.h"
#include "Setting/BirdItemSetting.h"
#include "Setting/FruitsSetting.h"
#include "Setting/HelpSetting.h"
#include "Setting/PixelGunGiftBagSetting.h"
#include "Setting/ModuleBlockSetting.h"
#include "Setting/ActionAreaManager.h"
#include "Setting/BlockCitySetting.h"
#include "Setting/DecorationSetting.h"
#include "Setting/BlockSetting.h"
#include "Setting/RecipeTabSetting.h"
#include "Setting/FurnaceSetting.h"
#include "Setting/SellItemSetting.h"
#include "Setting/AbandonItemSetting.h"
#include "Setting/BlockFortItemSetting.h"
#include "Setting/StoreTabSetting.h"
#include "Setting/CustomItemSetting.h"
#include "Setting/CustomArmorSetting.h"
#include "Setting/AdvertSetting.h"
#include "Setting/MountSetting.h"
#include "Setting/FunctionSetting.h"
#include "Util/CommonEvents.h"
#include "cWorld/WorldClient.h"
#include "Setting/BlockCityPortalSetting.h"
#include "Setting/ConfigurableBlockSetting.h"
#include "Entity/TeleportPositionConverter.h"
#include "GameAnalytics/GameAnalyticsRequest.h"
#include "GameAnalytics/GameAnalyticsSetting.h"
#include "Util/CPUTimer.h"

#if (LORD_PLATFORM != LORD_PLATFORM_WINDOWS)
#include <unistd.h>
#else
#include "Setting/TestPlayerSetting.h"
#endif

using namespace LORD;
using namespace BLOCKMAN;
unsigned int g_framedt = 0;

namespace GameClient
{
	CGame* CGame::Instance()
	{
		return ms_pSingleton;
	}

	CGame::CGame()
		: m_bInited(false)
		, m_enterBackFlag(false)
		, m_easyDownloadHandle(NULL)
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#if _DEBUG
		, m_fps(100)
#else
		, m_fps(1000)
#endif
#else
		, m_fps(35)
#endif
		, m_debugMessageShown(false)
		, m_needUpdateNum(0)
		, m_curUpdateIndex(0)
		, m_totalUpdateSize(1)
		, m_curDownloadSize(0)
		, m_updateLogHandle(NULL)
		, m_updateState(-1)
		, m_shellInterface(NULL)
		, m_hasGetServerInfo(false)
		, m_tickCount(0)
		, m_bNetGame(false)
		, m_baseUrl("http://dev.mods.sandboxol.cn")
	{
		maxFrameTime = 1000 / m_fps;
	}

	CGame::~CGame()
	{
#ifdef LORD_MEMORY_TRACKER
		LORD::MemTracker::get().reportLeaks();
#endif
#ifdef TRACK_EVENT_SUBSCRIPTION
		BLOCKMAN::EventManager::reportLeaks();
#endif
	}

	void CGame::InitGame(const char* strRootPath, const char* strConfigPath, int width, int height, unsigned int hWnd, LORD::SoundEngine* pSoundEngine, const char* strMapPath)
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		auto player = TestPlayerSetting().getStartPlayer();
		if (player.userId == 0)
			LordLogFatal("Could not find a usable test player, please add test player in form Setting/TestPlayer.csv");
		
		String lockFile = StringUtil::ToString(player.userId) + ".lock";
		if (!PathUtil::IsFileExist(lockFile))
			CreateFile(lockFile.c_str(), 0, 0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
		
		ScriptSetting::loadGameScriptDir("", false, false);
		InitGame("test", player.userId, player.userToken.c_str(), ScriptSetting::m_serverIp.c_str(),
			ScriptSetting::m_serverPort, 123456789, player.langCode.c_str(), ScriptSetting::m_gameType.c_str(),
			ScriptSetting::m_mapName.c_str(), "http://static.sandboxol.cn/mini-game-map/map-debug.zip",
			strRootPath, strConfigPath, width, height, hWnd, "windows_disp_request_id", pSoundEngine, strMapPath);
#else
		InitGame("test", 4195328, "5283be6918214e267d870e2dfa1bcd54ad46f76d", "192.168.1.100", 19130, 123456789, 
			"zh_CN",  "g1001","m101", "http://static.sandboxol.cn/mini-game-map/m101.zip", strRootPath, 
			strConfigPath, width, height, hWnd, "self_client_disp_request_id", pSoundEngine , strMapPath);
#endif
	}

	void CGame::InitGame(const char * nickName, ui64 userId, const char * token, const char * ip, int port, ui64 gameTimestamp,
		const char * lang, const char* gameType, const char* mapId, const char* mapUrl, const char * strRootPath,
		const char * strConfigPath, int width, int height, unsigned int hWnd, const char* dispatchRequestId,
		LORD::SoundEngine * pSoundEngine/* = NULL*/, const char* strMapPath/* = NULL*/, bool isNewStartMode/* = false*/, bool enterGameWithTeam/* = false*/)
	{
		m_nickName = nickName;
		m_platformUserId = userId;
		m_gameToken = token;
		m_gameIP = ip;
		m_gamePort = port;
		m_gameTimestamp = gameTimestamp;
		m_lang = lang;
		m_mapId = mapId;
		m_mapUrl = mapUrl;
		m_rootPath = strRootPath;
		m_configPath = strConfigPath;
		m_width = width;
		m_height = height;
		m_hWnd = hWnd;
		m_dispatchRequestId = dispatchRequestId;
		m_soundEngine = pSoundEngine;
		m_mapPath = strMapPath ? strMapPath :  PathUtil::ConcatPath(m_rootPath, "map/");
		m_playerName = m_nickName;
		m_gameType = gameType;
		m_isNewStartMode = isNewStartMode;
		m_enterGameWithTeam = enterGameWithTeam;
		m_dbDataReady = false;

		// parse mapNameReal from mapUrl
		std::string mapUrlStr(mapUrl);
		std::string mapNameReal("");

		if (mapUrlStr.find_last_of("/") != mapUrlStr.npos)
		{
			mapNameReal = mapUrlStr.substr(mapUrlStr.find_last_of("/") + 1);
		}

		if (mapNameReal.find(".zip") != mapNameReal.npos)
		{
			mapNameReal = mapNameReal.substr(0, mapNameReal.find(".zip"));
		}

		m_mapNameReal = mapNameReal;

		String worldName = "";
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		worldName = m_mapId;
#else
		std::string mapIdStr(m_mapId.c_str());
		worldName = (mapIdStr + "/" + mapNameReal).c_str();
#endif

		if (m_bInited)
		{
			return;
		}

		LordNew(Root);
		setReceivePingTime(0);
		Root::Instance()->setWriteablePath(strConfigPath);
		Root::Instance()->setMapPath(m_mapPath);
		Root::RootCfg cfg;
		cfg.rootPath = strRootPath;
		cfg.resCfgFile = cfg.rootPath + "resource.cfg";
		cfg.bEditorMode = false;
		cfg.bFixedAspect = false;
		cfg.bClientMode = true;
		cfg.fAspectRadio = 9.0f / 16.0f;

		if (m_gameDestroyedBefore) {
			// remain client log of last game when reset game
			cfg.bForceLogAppend = true;
		}
		else {
			cfg.bForceLogAppend = false;
		}

		Root::Instance()->initialize(cfg);
		Root::Instance()->setWorldName(worldName);
	
		loadScriptRes();

		LoadSetting();

		UThread::setThisThreadName("MainTickThread");
		UThread::setThisThreadPriority(ThreadPriority::HIGH);
		LordLogInfo("MainTickThread started, id: %d", UThread::getThisThreadNativeId());

		LordNew BLOCKMAN::PlayerPrefs();
		LordNew BLOCKMAN::LanguageManager(String(strRootPath));
		BLOCKMAN::LanguageManager::Instance()->loadLocaleData(lang);
		LordNew(BLOCKMAN::GameCommon);
		LordNew BLOCKMAN::LogicSetting();
		LordNew BLOCKMAN::FunctionSetting();
		LordNew BLOCKMAN::AppItemsSetting();
		LordNew BLOCKMAN::CoinManager();
		LordNew GameClient::MapManager();
		LordNew GameClient::RankManager();
		(LordNew TriggerModuleClient)->initialize();
		LordNew BlockLoaderClient();
		LordNew BLOCKMAN::SkillEffectManager();
		LordNew GameClient::ImageManager();
	
		BLOCKMAN::LogicSetting::Instance()->setGameType(GameTypeSetting::GetGameType(m_gameType));
		BLOCKMAN::LogicSetting::Instance()->setEnableIndie(m_enableIndie);

		GUISystem::Instance()->setAdateSize(true);
		GUISystem::Instance()->setStandardCoordinate(Vector2(1280, 720));
		GUISystem::Instance()->setCurrentLanguage(lang);
		SoundSystem::Instance()->SetSoundEngine(new FModAudioEngine());

		Renderer::RenderCfg renderCfg;
		renderCfg.windowHandle = hWnd;
		renderCfg.screenWidth = width;
		renderCfg.screenHeight = height;

		Root::Instance()->initRenderer(LordNew GLES2Renderer, renderCfg);
		Root::Instance()->onSize(width, height);
		reportGLESVersion();

		LordNew QueryObjectManager;
		QueryObjectManager::Instance()->Initialize();

		LordNew IniFileObj;

		BLOCKMAN::Blockman::initialize();
		m_blockman = LordNew BLOCKMAN::Blockman();
		m_blockman->m_worldName = worldName;
		loadScript();
		m_blockman->init();
		LordNew CustomItemSetting();

		SCRIPT_EVENT::GameInitEvent::invoke(gameType);

		m_subscriptionGuard.add(NetworkRetryConnectionEvent::subscribe(std::bind(&CGame::retryConnect, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ClientOnGroundCheatEvent::subscribe(std::bind(&CGame::clientOnGroundCheatDetected, this)));

		m_network = LORD::make_shared<ClientNetwork>();

		m_receivePingTime = 0;
		m_sendLoginTime = 0;

		m_initGameTime = LORD::Root::Instance()->getCurrentTime();
		m_mapStartDownloadTime = 0;
		m_mapDownloadedTime = 0;
		m_recvedLoginResultTime = 0;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		char ibuf[10];
		sprintf(ibuf, "%d", LORD::Root::Instance()->getCurrentTime());
		m_playerName += ibuf;

		if (false) // use the map in specified dir when develop
#else
		if (!GameClient::MapManager::Instance()->mapExistsAndValid(m_mapId, m_mapNameReal))
#endif
		{
			// only record the time of first try to download map
			m_mapStartDownloadTime = LORD::Root::Instance()->getCurrentTime();
			this->downloadMap();
		}
		else 
		{
			onDownloadMapSuccess();
		}

		m_isReseting = false;
		LordLogInfo("initGame::nickName[%s], userId[%s], ip[%s], port[%d], lang[%s], gameType[%s], mapId[%s], mapPath[%s], rootPath[%s] configPath[%s]", 
			nickName, StringUtil::ToString(userId).c_str(), ip, port, lang, gameType, m_mapId.c_str(), m_mapPath.c_str(), m_rootPath.c_str(), m_configPath.c_str());
		LordLogInfo("initGame::requestId[%s]", m_dispatchRequestId.c_str());
	}

	bool CGame::clientOnGroundCheatDetected() {
		dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_CLIENT_ON_GROUND_CHEAT_DETECTED);
		dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_CLIENT_ON_GROUND_CHEAT_DETECTED);
		GameoverEvent::emit("onGround cheat detected");
		if (m_network)
		{
			m_network->logout();
		}
		return true;
	}

	std::string CGame::timeToStepStr(ui32 timeInMilliseconds)
	{
		ui32 step = 0;
		std::string stepStr = "";
		ui32 timeInSeconds = timeInMilliseconds / 1000;

		if (timeInSeconds / 2 <= 4) { // timeInSeconds < 10, step by 2
			step = (ui32)((timeInSeconds / 2 + 1) * 2);
			stepStr = std::to_string(step);
		}
		else {
			if (timeInSeconds < 20) {  //timeInSeconds < 20, step by 5
				step = (ui32)((timeInSeconds / 5 + 1) * 5);
				stepStr = std::to_string(step);
			}
			else { // timeInSeconds > 20
				stepStr ="20+";
			}
		}

		return stepStr;
	}

	std::string CGame::generateEnterGameTimeStr()
	{
		std::string timeStr = "";

		if (m_mapStartDownloadTime == 0) {
			timeStr.append("0");
		}
		else {
			timeStr.append(timeToStepStr(m_mapDownloadedTime - m_mapStartDownloadTime));
		}

		timeStr.append("-");
		timeStr.append(timeToStepStr(m_recvedLoginResultTime - m_initGameTime));

		return timeStr;
	}

	void CGame::loadScriptRes()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		ScriptSetting::loadAllGameResource();
#endif
		// load common resource
		String commonCfgPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media", "Scripts", "ClientGame", "Common", "res/resource.cfg");
		Root::Instance()->loadResourceByConfig("Media/Scripts/ClientGame/Common", commonCfgPath);

		String path = ScriptSetting::loadGameScriptDir(m_gameType, true);
		if (!path.empty())
		{
			// add game_items.json and game_items.png
			String gameItemPath = PathUtil::ConcatPath(path, "res/item");
			ResourceGroupManager::Instance()->addResourceLocation(gameItemPath.c_str(), "FileSystem", "General");
			// add font config and *.ttf(font file)
			String gameFontPath = PathUtil::ConcatPath(path, "res/font");
			ResourceGroupManager::Instance()->addResourceLocation(gameFontPath.c_str(), "FileSystem", "General");
#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
			// load game resource
			String cfgPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), path, "res", "resource.cfg");
			if (PathUtil::IsFileExist(cfgPath))
				Root::Instance()->loadResourceByConfig(path, cfgPath);
#endif
		}
	}

	void CGame::loadScript()
	{
		if (!m_loadCommonPluginFinished)
		{
			String commonPluginDir = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media", "Scripts", "ClientGame", "Common");
			PluginManager::loadScript(commonPluginDir.c_str(), (commonPluginDir + "/base/BaseMain.lua").c_str());
			m_loadCommonPluginFinished = true;
		}

		// load game plugin
		String path = ScriptSetting::loadGameScriptDir(m_gameType, true);
		if (!path.empty())
		{
			// load game base plugin
			String pluginBasePath = PathUtil::ConcatPath(Root::Instance()->getRootPath(),
				StringUtil::Replace(path, "ClientGame", "BaseGame"), "main");

			String loaderPath = PathUtil::ConcatPath(pluginBasePath, "Loader.lua");
			if (PathUtil::IsFileExist(loaderPath))
				LuaEngine::getInstance()->Load(loaderPath.c_str());

			LuaEngine::getInstance()->addScriptDir(pluginBasePath.c_str());

			// load game plugin
			String pluginPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), path, "main");
			PluginManager::loadScript(pluginPath.c_str(), (pluginPath + "/PluginMain.lua").c_str());
			LordLogInfo("Blockman::onGameReady GamePluginDir pluginPath[%s] ", pluginPath.c_str());
		}

	}

	void CGame::unloadScript()
	{
		String path = ScriptSetting::loadGameScriptDir(m_gameType, true);
		if (!path.empty())
		{
			// unload game base plugin
			String pluginBasePath = PathUtil::ConcatPath(Root::Instance()->getRootPath(),
				StringUtil::Replace(path, "ClientGame", "BaseGame"), "main");
			LuaEngine::getInstance()->removeScriptDir(pluginBasePath.c_str());

			// unload game plugin
			String pluginPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), path, "main");
			LuaEngine::getInstance()->removeScriptDir(pluginPath.c_str());
		}
	}

	void CGame::resetGame()
	{
		bool isChina = Root::Instance()->isChina();
		DestroyGame();
		InitGame(m_nickName.c_str(), m_platformUserId, m_gameToken.c_str(), m_gameIP.c_str(), m_gamePort, m_gameTimestamp, m_lang.c_str(), m_gameType.c_str(),
			m_mapId.c_str(), m_mapUrl.c_str(), m_rootPath.c_str(), m_configPath.c_str(), m_width, m_height, m_hWnd, m_dispatchRequestId.c_str(), m_soundEngine, m_mapPath.c_str(), m_isNewStartMode);
		Root::Instance()->setChina(isChina);
	}

	void CGame::updateTestStartParam()
	{
		// ---> windows use test server ip, port and map
		ScriptSetting::loadGameScriptDir(m_gameType, true);
		m_gameIP = ScriptSetting::m_serverIp.c_str();
		m_gamePort = ScriptSetting::m_serverPort;
		m_mapId = ScriptSetting::m_mapName.c_str();
	}

	void CGame::reportEnterGameTime()
	{
		std::string gameTimeStr = generateEnterGameTimeStr();
		dataReportNotType(ClientDataReport::ENTER_GAME_TIME, gameTimeStr.c_str());
	}

	void CGame::onProcessReport( LORD::String event)
	{
		if (m_shellInterface && needReportProcess)
		{
			m_shellInterface->onAppActionTrigger(16, StringUtil::Format(event.c_str(), m_gameType.c_str()));
		}
	}

	void CGame::downloadMap()
	{
		m_subscriptionGuard.add(MapManagerDownloadProgress::subscribe(std::bind(&CGame::onDownloadMapProgress, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(MapManagerDownloadSucc::subscribe(std::bind(&CGame::onDownloadMapSuccess, this)));
		m_subscriptionGuard.add(MapManagerDownloadFail::subscribe(std::bind(&CGame::onDownloadMapFailure, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	
		GameClient::MapManager::Instance()->downloadMap(m_mapId.c_str(), m_mapUrl.c_str(), m_mapNameReal.c_str());
		onProcessReport(ClientDataReport::GAME_PROCESS_EVENT_DOWNLOAD_MAP);
	}

	bool CGame::onDownloadMapProgress(int progress, uint32_t fileSize)
	{
		BLOCKMAN::DownloadMapProgressEvent::emit((float)((float)progress/100), (float)fileSize);

		return true;
	}

	bool CGame::onDownloadMapFailure(int failureCode, const std::string& failureMessage, bool hasRetried)
	{
		if (!hasRetried) {
			dataReportNotType(BLOCKMAN::ClientDataReport::DOWNLOAD_MAP_FAIL, std::to_string(failureCode).c_str());
			LordLogError("CGame::onDownloadMapFailure first time, errCode:[%d], errMsg:[%s], retry to download map, m_mapId[%s], m_mapUrl[%s], m_mapNameReal[%s]", failureCode, failureMessage.c_str(), m_mapId.c_str(), m_mapUrl.c_str(), m_mapNameReal.c_str());
		}
		else
		{
			BLOCKMAN::DownloadMapFailureEvent::emit();
			LordLogError("CGame::onDownloadMapFailure second time, errCode:[%d], errMsg:[%s]", failureCode, failureMessage.c_str());
			dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_MAP_DOWNLOAD_FAILS);
			dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_MAP_DOWNLOAD_FAILS);
			dataReportNotType(BLOCKMAN::ClientDataReport::DOWNLOAD_MAP_FAIL, std::to_string(failureCode).c_str());
		}

		return true;
	}

	bool CGame::onDownloadMapSuccess()
	{
		m_mapDownloadedTime = LORD::Root::Instance()->getCurrentTime();
		LordLogInfo("CGame::onDownloadMapSuccess, download map use time:%u", m_mapDownloadedTime - m_mapStartDownloadTime);
		m_bInited = true;
		BLOCKMAN::DownloadMapSuccessEvent::emit(m_isNewStartMode);
		auto mapDir = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName());
		LordLogInfo("CGame::onDownloadMapSuccess, begin to use map, mapDir:%s", mapDir.c_str());

		// load game custom blocks
		String gameSettingDir = StringUtil::Replace(ScriptSetting::m_scriptPath, "ClientGame", "GameSetting");
		gameSettingDir = PathUtil::ConcatPath(Root::Instance()->getRootPath(), gameSettingDir);
		if (BLOCKMAN::IdMapping::readConfig(PathUtil::ConcatPath(gameSettingDir, "id_mappings.ini")) != -1)
			BLOCKMAN::BlockLoaderClient::Instance()->loadCustomBlocksInMap(gameSettingDir);
		// load map custom blocks
		BLOCKMAN::IdMapping::readConfig(PathUtil::ConcatPath(mapDir, "id_mappings.ini"));
		BLOCKMAN::BlockLoaderClient::Instance()->loadCustomBlocksInMap(mapDir);	

		BLOCKMAN::LanguageManager::Instance()->loadMapData(Root::Instance()->getMapPath(), m_blockman->m_worldName);
		BLOCKMAN::ClientHttpRequest::loadUserInfo(m_platformUserId);
		loadMapDynamicRes();
		SceneSetting::Instance()->loadSetting(true);
		BLOCKMAN::Blockman::Instance()->m_globalRender->applySceneSwitchChanges();
		SCRIPT_EVENT::GameLoadMapSuccessEvent::invoke(mapDir, Root::Instance()->getRootPath());
		LordLogInfo("onDownloadMapSuccess");
		if (m_isNewStartMode) m_shellInterface->onMapDownloadSuccess();
		else m_network->connect(m_gameIP.c_str(), m_gamePort);

		return true;
	}

	void CGame::connectServer(const char * ip, int port, const char * dispatchRequestId)
	{
		if (!m_network)
			m_network = LORD::make_shared<ClientNetwork>();
		m_network->connect(ip, port);
		m_gamePort = port;
		m_gameIP = ip;
		m_dispatchRequestId = dispatchRequestId;
		onProcessReport(ClientDataReport::GAME_PROCESS_EVENT_CONNECT_DISPATCH);
		LordLogInfo("CGame::connectServer==========================ip[%s], port[%d], requestId[%s]", ip, port, dispatchRequestId);
	}

	void CGame::loadMapDynamicRes()
	{
		String path = "";
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Blockman::Instance()->m_worldName, "dynamic");
#else
		path = PathUtil::ConcatPath(Blockman::Instance()->m_worldName, "dynamic");
#endif
		ResourceGroupManager::Instance()->addResourceLocation(path.c_str(), "FileSystem", "General");

		MultiLanTipSetting::loadMapSetting(true);
		ActorSizeSetting::loadMapSetting(true);
		CropsSetting::loadMapSetting(true);
		RecipeTabSetting::Instance()->loadSetting(true);
		FurnaceSetting::Instance()->loadSetting(true);
		SellItemSetting::Instance()->loadSetting(true);
		AbandonItemSetting::Instance()->loadSetting(true);
		BlockSetting::Instance()->loadSetting(true);
		CustomArmorSetting::Instance()->loadSetting(true);
		CustomItemSetting::Instance()->loadMapSetting();
		GuideSetting::loadSetting();
		GatherToolSetting::loadSetting(true);
		BulletinSetting::loadSetting(true, true);
		BirdItemSetting::loadSetting();
		HelpSetting::loadSetting();
		FruitsSetting::loadSetting(true);
		BlockCitySetting::loadSetting();
		SoundSetting::loadSetting();
		DecorationSetting::loadSetting(true);
		LordNew BlockFortItemSetting(true);
		LordNew StoreTabSetting();
		LordNew BlockCityPortalSetting();
		LordNew ConfigurableBlockSetting(true);
		cBlockManager::cInitializeOnDownloadMap();
		cBlockManager::cRegistBlockItemsOnDownloadMap();
	}

	bool CGame::retryConnect(bool isChangeIp)
	{
		if (isChangeIp)
		{
			resetGameAddr(m_platformUserId, m_gameType, m_mapId, m_gameIP);
		}
		return true;
	}

	void CGame::useProp(const char * propId)
	{
		if (m_network && m_network->isConnected()) 
		{
			m_network->getSender()->sendUseProp(propId);
		}
	}

	int CGame::getPing()
	{
		if (m_network && m_network->isConnected() && m_blockman->isGameReady())
		{
			ui32 nowTime = Root::Instance()->getCurrentTime();
			// !!! do not change the number 5000  !!!
			// !!! speed cheat detect rely on 5000 !!!
			if (m_receivedPingAck && nowTime > m_receivePingTime && nowTime - m_receivePingTime >= 5000)
			{
				m_receivedPingAck = false;
				m_network->getSender()->sendPing(nowTime, m_receivePingTime);

				//LordLogDebug("logic ping:%d, raknet ping:%d", m_ping, getRaknetPing());
			}

			return m_ping;
		}
		return -1;
	}

	int CGame::getRaknetPing()
	{
		if (m_network && m_network->isConnected() && m_blockman->isGameReady()) {
			return m_network->getRaknetPing();
		}
		else {
			return -1;
		}
	}

	void CGame::recordRecvedLoginResultTime() {
		if (m_recvedLoginResultTime == 0) {
			m_recvedLoginResultTime = Root::Instance()->getCurrentTime();
		}
	}

	void CGame::dataReport(String mainEvent)
	{
		if (m_shellInterface)
		{
			m_shellInterface->dataReport(mainEvent, m_gameType + " " + m_errorCode);
		}
	}

	void CGame::dataReport(String mainEvent, String childEvent)
	{
		if (m_shellInterface)
		{

			if (childEvent.length() > 0 && childEvent.length() <= 3 && m_errorCode != BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_OVER)
			{
				m_errorCode = childEvent;
			}

			if (m_errorCode == BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_OVER && ( childEvent == "4" || childEvent == "5" || childEvent == "6")) {
				return;
			}

			m_shellInterface->dataReport(mainEvent, m_gameType + " " + childEvent);

		}
	}

	void CGame::dataReportNotType(String mainEvent, String childEvent)
	{
		if (m_shellInterface)
		{
			m_shellInterface->dataReport(mainEvent, childEvent);
		}
	}

	void CGame::reportGLESVersion()
	{
		auto version = glGetString(GL_VERSION);
		if (version && m_shellInterface)
		{
			m_shellInterface->onAppActionTrigger(14, (const char*)version);
		}
	}

	void CGame::onError(int errorCode)
	{
		if (m_shellInterface)
		{
			m_shellInterface->onError(errorCode);
		}
	}

	void CGame::onFriendOperationForAppHttpResult(int operationType, ui64 userId)
	{
		FriendOperationForAppHttpResultEvent::emit(operationType, userId);
	}

	bool CGame::exitGame(String params)
	{
		LordLogDebug("CGame::exitGame() params = %s", params.c_str());
#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
		GameClient::MapManager::Instance()->delelteMap(m_mapId, m_mapUrl);
#endif
		if (m_network && m_network->isConnected())
		{
			m_network->logout();
		}
		if (m_shellInterface)
		{
			dataReport(ClientDataReport::QUIT_GAME);
			m_shellInterface->gameExit(params);
		}
		return true;
	}

	void CGame::retryEnter(int code, String message)
	{
		if (m_retryEnterTimes > 3)
		{
			switch (code)
			{
			case 2:
				dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_START);
				dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_START);
				GameoverEvent::emit(message);
				if (m_network)
					m_network->logout();
				break;
			case 3:
				dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_ROOM_FULL);
				dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_ROOM_FULL);
				GameoverEvent::emit(message);
				if (m_network)
					m_network->logout();
				break;
			}
		}
		else
		{
			switch (code)
			{
			case 2:
				dataReport(BLOCKMAN::ClientDataReport::GAME_RETRY_CONNECT, BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_START_RETRY);
				resetGame(m_gameType, m_platformUserId, m_mapId);
				break;
			case 3:
				dataReport(BLOCKMAN::ClientDataReport::GAME_RETRY_CONNECT, BLOCKMAN::ClientDataReport::ERROR_CODE_ROOM_FULL_RETRY);
				resetGame(m_gameType, m_platformUserId, m_mapId);
				break;
			}
			m_retryEnterTimes++;
			LordLogInfo("CGame::retryEnter errorCode = [%d], retryEnterTimes = [%d], mapId = [%s]", code, m_retryEnterTimes, m_mapId.c_str());
		}
	}

	void CGame::resetGame(String dispatchInfo)
	{
		auto builder = CommonDataBuilder().fromData(dispatchInfo);
		if (!builder.hasParam("ip") || !builder.hasParam("port") || !builder.hasParam("mapId") ||
			!builder.hasParam("mapUrl") || !builder.hasParam("requestId") || !builder.hasParam("gameType"))
			return;

		String gameIP = builder.getStringParam("ip");
		int gamePort = builder.getIntParam("port");
#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS			
		if (gameIP == m_gameIP && gamePort == m_gamePort)
		{
			LordLogWarning("Playing a game on the current server, no need to reset the game");
			return;
		}
#endif
		RootGuiLayout::Instance()->showLoadPage();
		DownloadMapProgressEvent::emit(0.0f, 0.0f);
		unloadScript();
		m_isReseting = true;
		m_mapUrl = builder.getStringParam("mapUrl");
		m_dispatchRequestId = builder.getStringParam("requestId");
		m_gameType = builder.getStringParam("gameType");
		m_isNewStartMode = false;

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS	
		updateTestStartParam();
		if (gameIP != "127.0.0.1") // use remote ip
		{
			m_gameIP = gameIP;
			m_gamePort = gamePort;
		}
#else
		m_gameIP = gameIP;
		m_gamePort = gamePort;
		m_mapId = builder.getStringParam("mapId");
#endif
	}

	void CGame::resetGame(ui64 targetId)
	{
		resetGame(m_gameType, targetId, "");
	}

	void CGame::resetGame(String gameType, ui64 targetId, String mapId)
	{
		resetGameAddr(targetId, gameType, mapId, "");
	}

	void CGame::resetGameAddr(ui64 targetId, String gameType, String mapId, String addr)
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		unloadScript();
		m_gameType = gameType;
		m_isReseting = true;
		m_isNewStartMode = false;
		updateTestStartParam();
#endif	

		if (m_shellInterface)
		{
			needReportProcess = false;
			m_shellInterface->resetGameAddr(targetId, gameType, mapId, addr);
		}
	}

	void CGame::resetGameResult(int result)
	{
		LordLogError("CGame::resetGameResult code [%d].", result);
		if (result !=6 && (!m_network || !m_network->isAlive()))
		{
			AllocationGameFailure::emit();
			GameClient::CGame::Instance()->dataReport(ClientDataReport::GAME_ERROR_CODE, ClientDataReport::ERROR_CODE_ALLOCATION_GAME_MAP_FAILURE);
			GameClient::CGame::Instance()->dataReportNotType(ClientDataReport::ENTER_GAME_STATUS, ClientDataReport::ERROR_CODE_ALLOCATION_GAME_MAP_FAILURE);
			return;
		}

		switch (result)
		{
		case 1:
			break;
		case 2:
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_MANOR_CALL_ON_FAILURE);
			break;
		case 3:
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_MANOR_TP_FAILURE);
			break;
		case 4:
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_manor_call_on_failure_vistors_full");
			break;
		case 5:
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_manor_call_on_failure_version_not_matched");
			break;
		case 6:
			if (!m_network || !m_network->isAlive())
			{
				BLOCKMAN::NetworkConnectionFailureEvent::emit();
				GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_CONNECT_FAILS);
				GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_CONNECT_FAILS);
			}
			else
			{
				LordLogInfo("CGame::resetGameResult ======================================= %d", result);
			}
			break;
		case 7:
			if (!m_network || !m_network->isAlive())
			{
				AllocationGameFailure::emit();
				GameClient::CGame::Instance()->dataReport(ClientDataReport::GAME_ERROR_CODE, ClientDataReport::ERROR_CODE_ALLOCATION_GAME_MAP_FAILURE);
				GameClient::CGame::Instance()->dataReportNotType(ClientDataReport::ENTER_GAME_STATUS, ClientDataReport::ERROR_CODE_ALLOCATION_GAME_MAP_FAILURE);
			} 
			else
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_loading_page_await_allocation_game_failure");
			}
			break;
		default:
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_MANOR_OPERATION_FAILURE);
			break;
		}
	}

	void CGame::onRechargeResult(int type, int result)
	{
		ClientNetwork::Instance()->getSender()->sendRechargeResult(type, result);
		if (result == 1)
		{
			ClientNetwork::Instance()->getSender()->sendUpdateUserMoney();
		}
	}

	void CGame::buyActionResult(int type, int result)
	{
		if (result == 1)
		{
			BLOCKMAN::BuyActionSuccessEvent::emit(type);
			ClientNetwork::Instance()->getSender()->sendUpdateUserSelectableAction();
		}
		else if (result == 5006)
		{
			GameClient::CGame::Instance()->getShellInterface()->onRecharge((int)RechargeType::RECHARGE_TYPE_CUBE);
		}
	}

	void CGame::DestroyGame()
	{
		LordLogInfo("CGame::DestroyGame Start");

		SCRIPT_EVENT::GameDestroyEvent::invoke(true);
		unloadScript();

		PlayerPrefs::Instance()->init();

		if (m_network && m_network->isConnected())
			m_network->logout();
		
		TeleportPositionConverter::ClearClientSideAnchors();

        if(m_blockman && m_blockman->m_pWorld && m_blockman->m_pWorld->m_pChunkService)
            m_blockman->m_pWorld->m_pChunkService->cancelAsyncGets();

		while (!BLOCKMAN::PROMISE::callbackQueue.empty())
		{
			BLOCKMAN::PROMISE::runCallbacks();
			// sleep 0.1s for workthread finish all the task.
			#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				Sleep(100);
			#else
				usleep(100000);
			#endif
		}
#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
		GameClient::MapManager::Instance()->delelteMap(m_mapId, m_mapUrl);
#endif
		m_subscriptionGuard.unsubscribeAll();
		UnloadSetting();
		LordSafeDelete(m_blockman);
		BLOCKMAN::Blockman::unInitialize();
		BLOCKMAN::HttpPromise::uninit();

		LordDelete IniFileObj::Instance();
		LordDelete QueryObjectManager::Instance();
		LordDelete BLOCKMAN::GameCommon::Instance();
		LordDelete BLOCKMAN::LanguageManager::Instance();
		LordDelete GameClient::MapManager::Instance();
		LordDelete BLOCKMAN::CoinManager::Instance();
		LordDelete BLOCKMAN::LogicSetting::Instance();
		LordDelete BLOCKMAN::AppItemsSetting::Instance();
		LordDelete GameClient::RankManager::Instance();
		LordDelete BLOCKMAN::SkillEffectManager::Instance();
		LordDelete GameClient::ImageManager::Instance();
		LordDelete BLOCKMAN::FunctionSetting::Instance();
		LordDelete DataCache::Instance();

		Root::Instance()->destroy();
		LordDelete Root::Instance();
		LordDelete(BlockLoaderClient::Instance());
		TriggerModule::Instance()->uninitialize();
		LordDelete(TriggerModule::Instance());
		LordDelete BLOCKMAN::PlayerPrefs::Instance();
		
		m_hasGetServerInfo = false;
		m_network = nullptr;
		m_bInited = false;
		m_avgFpsCount = 0;
		m_avgFps = 0;

		m_gameDestroyedBefore = true;
		LordLogInfo("CGame::DestroyGame End");
	}

	void CGame::MainTick()
	{
		if (m_isReseting)
		{
			resetGame();
			return;
		}
		LORD::CPUTimer::StartNewFrame();
		PROFILE_CPU("CGame::MainTick");
		m_tickCount++;
		BLOCKMAN::HttpPromise::tickAll();
		GameClient::MapManager::Instance()->tick();
		{
			static LORD::ui32 lastTime = LORD::Root::Instance()->getCurrentTime();
			LORD::ui32 curTime = LORD::Root::Instance()->getCurrentTime();
			g_framedt = int(LORD::Math::Min(curTime - lastTime, (LORD::ui32)100) /** ActorManager::Instance()->GetSpeedUp()*/);
			lastTime = curTime;

			if (m_network->getStatus() == ClientNetworkCore::NetStatus::emRetryConnect)
				RetryNetworkConnection();
			if (m_network->getStatus() != ClientNetworkCore::NetStatus::emConnectFailed)
				m_network->handleIncomeData();

			SecTimer::update();

			if (m_hasGetServerInfo)
			{
				GameLogicTick();
				getPing();
			
#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
				if (m_tickCount % 30 == 0)
				{
					if (m_network && !m_network->isAlive() && m_errorCode != BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_OVER)
					{
						static bool sentError = false;
						if (!sentError) {
							sentError = true;
							if (m_network->isConnected())
							{
								dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_CONNECT_ERROR);
								dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_CONNECT_ERROR);
								LordLogInfo("NetworkConnectionDisconnectEvent->m_network->isAlive() false");
							}
						}
					}

				}
#endif
			}
			
			static bool sentLoginFail = false;
			ui32 time = LORD::Root::Instance()->getCurrentTime();

			if (m_sendLoginTime > 0 && time - m_sendLoginTime > 15 * 1000 && !sentLoginFail)
			{
				sentLoginFail = true;
				if (m_network && m_network->isConnected())
				{
					BLOCKMAN::LoginResponseTimeoutEvent::emit();
					if (hasGetServerInfo()) {
						dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_VALIDATION_TIMEOUT);
						dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_VALIDATION_TIMEOUT);
					}
					else {
						dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_NOT_RECV_GAME_INFO);
						dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_NOT_RECV_GAME_INFO);
					}
					LordLogError("MainTick user data  validation timeout");
				}
				else
				{
					static bool isRetryConnect = false;
					if (!isRetryConnect)
					{
						isRetryConnect = true;
						retryConnect(false);
						LordLogError("MainTick retry connect");
					}
					else
					{
						isRetryConnect = false;
						BLOCKMAN::NetworkConnectionDisconnectEvent::emit();
						dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_CONNECTION_LOST);
					}
				}
			}
			

			RenderTick();
			{
				PROFILE_CPU("m_clientAsyncCall.realCall");
				m_clientAsyncCall.realCall();
			}
			{
				PROFILE_CPU("SoundSystem::Instance()->updateEngine()");
				SoundSystem::Instance()->updateEngine();
				m_blockman->m_mainBegin = m_blockman->m_asyncEnd;
			}

			if (m_network->getStatus() != ClientNetworkCore::NetStatus::emConnectFailed)
			{
				m_network->flush();
			}

			if (m_hasGetServerInfo)
			{
				++m_avgFpsCount;
				m_avgFps += (Root::Instance()->getFPS() - m_avgFps) / m_avgFpsCount;
			}

#if LORD_PLATFORM != LORD_PLATFORM_LINUX && LORD_PLATFORM != LORD_PLATFORM_WINDOWS
			// report lua SCRIPT_EXCEPTION
			String scriptError = Log::getScriptError();
			if (!scriptError.empty())
			{
				m_shellInterface->dataReportError(scriptError);
				Log::clearScriptError();
			}
#endif

			LORD::ui32 endTime = LORD::Root::Instance()->getCurrentTime();
			g_framedt = endTime - curTime;
			if (g_framedt < maxFrameTime)
			{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				/*Sleep(maxFrameTime - g_framedt);*/
				Sleep(maxFrameTime - g_framedt);
#else
				usleep((maxFrameTime - g_framedt) * 1000);
#endif
			}

			if (g_framedt > 100)
			{
				LordLogWarning("MainTick======================== frame time [%d]", g_framedt);
			}

			//if (m_tickCount % 100 == 0  && m_network)
			//{
			//	m_network->printColletingPacket();
			//}
		}
	}

	void CGame::RetryNetworkConnection()
	{
		m_network = nullptr;
		m_network = LORD::make_shared<ClientNetwork>();
		m_network->connect(m_gameIP.c_str(), m_gamePort);
		LordLogWarning("game:network on retry connect.");
	}

	void CGame::GameLogicTick()
	{
		if (m_enterBackFlag)
			return;
		PROFILE_CPU("CGame::GameLogicTick");
		if (1 == TouchManager::Instance()->getSceneTouchCount()) {
			Touch* touch = TouchManager::Instance()->getSceneTouch1();
			if (m_blockman && m_blockman->m_gameSettings)
			{
				if (touch && touch->isLongTouchBegin() && m_blockman->m_gameSettings->userAction != BLOCKMAN::GameSettings::ACTION_LONG_TOUCH_BEGIN) {
					m_blockman->m_gameSettings->userAction = BLOCKMAN::GameSettings::ACTION_LONG_TOUCH_BEGIN;
					/* added by wanglei, for debug
					Vector2 startPoint = touch->getStartPoint();
					Vector2 currPoint = touch->getTouchPoint();
					LordLogDebug("long touch , startPoint=(%f,%f),currPoint=(%f,%f), lenSqr=%f",
						startPoint.x, startPoint.y, currPoint.x, currPoint.y, (currPoint - startPoint).lenSqr());*/
				}
			}
		}

		LORD::ui32 dt = g_framedt;// Root::Instance()->getFrameTime();
		if (!m_enterBackFlag)
			m_blockman->tick(float(dt) / 1000.f);
	}

	void CGame::RenderTick()
	{
		if (m_enterBackFlag)
			return;
		PROFILE_CPU("CGame::RenderTick");
		FrameState frameState = Root::Instance()->frameState();
		Root::Instance()->resetFrameState();
			
		// update
		Root::Instance()->frameMove(g_framedt);

		{
			PROFILE_CPU("SCRIPT_EVENT::GameMainTickEvent");
			SCRIPT_EVENT::GameMainTickEvent::invoke(Root::Instance()->getFrameTime());
		}
		if (m_hasGetServerInfo && m_debugMessageShown)
		{
			PROFILE_CPU("CGame::RenderTick.showDebugMessage");
			float offsetX = PlayerPrefs::Instance()->getFloatPrefs("DebugInfoRenderOffsetX");
			float offsetY = PlayerPrefs::Instance()->getFloatPrefs("DebugInfoRenderOffsetY");

			// use one batch to render all the debug strings.
			vector<FontBuffer>::type buffer;

			GUIFont* pFont = GUIFontManager::Instance()->GetFont("HT16");
			// render debug info.
			String playerInfo = m_blockman->getPlayerString();
			Renderer::Instance()->renderText(playerInfo.c_str(), Vector2(offsetX, 60 + 40 + offsetY), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);

			String traceInfo = m_blockman->getRayTraceString();
			Renderer::Instance()->renderText(traceInfo.c_str(), Vector2(offsetX, 80 + 40 + offsetY), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);

			String strFrame = StringUtil::Format("FPS:%d, Ping:%d, NetPing:%d, IP:%s, Port:%d, Batch:%d, Triangles:%d",
				Root::Instance()->getFPS(), getPing(), m_network ? m_network->getRaknetPing() : 0, m_gameIP.c_str(), m_gamePort, frameState.getDrawCalls(), frameState.getTriangleNum());
			Renderer::Instance()->renderText(strFrame.c_str(), Vector2(offsetX, 100 + 40 + offsetY), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);

			String mcDebugInfo = m_blockman->getDebugString();
			Renderer::Instance()->renderText(mcDebugInfo.c_str(), Vector2(offsetX, 120 + 40 + offsetY), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);

			String cpuInfo = m_blockman->getAsyncString();
			Renderer::Instance()->renderText(cpuInfo.c_str(), Vector2(offsetX, 140 + 40 + offsetY), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);

			String clientBlockInfo = m_blockman->getClientDebugString();
			Renderer::Instance()->renderText(clientBlockInfo.c_str(), Vector2(offsetX, 160 + 40 + offsetY), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);

			String serverBlockInfo = m_blockman->getServerDebugString();
			Renderer::Instance()->renderText(serverBlockInfo.c_str(), Vector2(offsetX, 220 + 40 + offsetY), pFont, Color(0.6f, 1.f, 0.6f), Vector2(1.0f, 1.0f), true, &buffer);


			GUIRenderText::DrawTextBuffer(pFont, buffer);
		}

		// render
		Renderer::Instance()->renderScene();

		if (SceneSetting::Instance()->useCustomBackgroundColor())
		{
			Color bgColor;
			int hour, minute;
			m_blockman->m_pWorld->getCurrWorldTime(hour, minute);
			if (SceneSetting::Instance()->getBackgroundColor(bgColor, hour, minute))
			{
				Renderer::BGCOLOR = bgColor;
			}
		}

		Renderer::Instance()->present();
		Root::Instance()->threadRespond();

		if (m_hasGetServerInfo)
		{
			m_blockman->m_asyncBegin = LORD::Time::Instance()->getMicroseconds();
			m_blockman->finishAsync(); // m_visualTessThread->WaitForFinish(0xFFFFFFFF);
			m_blockman->m_asyncEnd = LORD::Time::Instance()->getMicroseconds();
			m_blockman->caculateCpuTime();
		}

	}

	void CGame::onExpResult(int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status)
	{
		if (m_shellInterface)
		{
			m_shellInterface->onExpResult(curLv, toLv, addExp, curExp, toExp, upExp, status);
		}
	}

	void CGame::onWatchAdResult(int type, String params, int code)
	{
		LordLogDebug("onWatchAdResult, type %d, params %s, code %d", type, params.c_str(), code);
		ClientNetwork::Instance()->getSender()->sendWatchAdFinished(type, params, code);
	}

	void CGame::onGameActionTrigger(int type, String params)
	{
		LordLogDebug("onGameActionTrigger: %d   params: %s", type, params.c_str());
		switch ((GameActionType)type)
		{
		case GameActionType::GAME_ACTION_REFRESH_DRESS:
			UpdatePlayerDressEvent::emit();
			break;
		case GameActionType::GAME_ACTION_ENABLE_INDIE:
			m_enableIndie = true;
			if (BLOCKMAN::LogicSetting::Instance())
				BLOCKMAN::LogicSetting::Instance()->setEnableIndie(true);
			break;
		case GameActionType::GAME_ACTION_DISABLE_RECHARGE:
			BLOCKMAN::DisableRechargeEvent::emit();
			break;
		case GameActionType::GAME_ACTION_SHOW_VIDEO_AD:
			BLOCKMAN::VideoAdVisibleEvent::emit(true);
			if (PlayerPrefs::Instance())
				PlayerPrefs::Instance()->putBoolPrefs("ShowVideoAd", true);
			break;
		case GameActionType::GAME_ACTION_HIDE_VIDEO_AD:
			BLOCKMAN::VideoAdVisibleEvent::emit(false);
			if (PlayerPrefs::Instance())
				PlayerPrefs::Instance()->putBoolPrefs("ShowVideoAd", false);
			break;
		case GameActionType::GAME_ACTION_SHOW_EMAIL_RED:
			BLOCKMAN::EmailRedPointVisibleEvent::emit(true);
			break;
		case GameActionType::GAME_ACTION_HIDE_EMAIL_RED:
			BLOCKMAN::EmailRedPointVisibleEvent::emit(false);
			break;
		case GameActionType::GAME_ACTION_REFRESH_PROPS:
			if (ClientNetwork::Instance())
				ClientNetwork::Instance()->getCommonSender()->sendRefreshProps();
			break;
		case GameActionType::GAME_ACTION_EXTRA_PARAMS:
		{
			auto builder = CommonDataBuilder().fromData(params);
			bool isChina = builder.getBoolParam("isChina");
			if (Root::Instance())
				Root::Instance()->setChina(isChina);
			break;
		}	
		default:
			break;
		}
		SCRIPT_EVENT::GameActionTriggerEvent::invoke(type, params);
	}

	void CGame::OnSize(int nWidth, int nHeight)
	{
		if (nWidth != 0 && nHeight != 0)
		{
			m_width = nWidth;
			m_height = nHeight;
			Root::Instance()->onSize(nWidth, nHeight);
		}
	}

	void CGame::OnPause()
	{
		LordLogDebug("CGame::OnPause()");
		if (m_bInited)
		{
			if (SoundSystem::Instance())
			{
				SoundSystem::Instance()->RemoveSoundEngine();
				SoundSystem::Instance()->setSoundPause(true);
			}
			SCRIPT_EVENT::GamePauseEvent::invoke(true);
		}
		m_enterBackFlag = true;
		GUIWindowManager::Instance()->SetEnterBack(m_enterBackFlag);
	}

	void CGame::OnResume()
	{
		LordLogDebug("CGame::OnResume()");
		if (m_bInited)
		{
			if (SoundSystem::Instance() && SoundSystem::Instance()->isSoundOn())
			{
				SoundSystem::Instance()->SetSoundEngine(new FModAudioEngine());
				SoundSystem::Instance()->setSoundPause(false);
				SoundSystem::Instance()->playBackgroundMusic();
			}
			SCRIPT_EVENT::GameResumeEvent::invoke(true);
		}
		m_enterBackFlag = false;
		GUIWindowManager::Instance()->SetEnterBack(m_enterBackFlag);
	}

	int CGame::CheckVersion(const char* strRootPath)
	{
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		return 0;
#endif
		m_rootPath = strRootPath;

		InitConfig(strRootPath);

		FILE* file = NULL;

		String strOldVersionFile = strRootPath;
		strOldVersionFile += m_gameConfig.versionFileName;
		StringArray oldVersion;
		StringArray newVersion;

		int nRet = 0;

		// old version
		file = fopen(strOldVersionFile.c_str(), "rb");

		if (file)
		{
			fseek(file, 0, SEEK_END);
			long lSize = ftell(file);
			rewind(file);

			char* buff = (char*)LordMalloc(sizeof(char)*lSize + 1);
			buff[lSize] = 0;

			long result = fread(buff, 1, lSize, file);
			if (result != lSize)
			{
				LordAssert("InitConfig reading error");
			}

			fclose(file);

			cJSON* root = cJSON_Parse(buff);
			cJSON* version = cJSON_GetObjectItem(root, "version");

			m_localVersion = version->valuestring;

			oldVersion = StringUtil::Split(version->valuestring, ".");
			cJSON_Delete(root);

			LordSafeFree(buff);
		}
		else
		{
			nRet = -1;

			return nRet;
		}

		// download new version file
		CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
		m_easyDownloadHandle = curl_easy_init();

		String src = m_gameConfig.versionFileName;
		String dest = strRootPath;
		dest += m_gameConfig.versionFileName + ".tmp";

		bool downloadVersionOK = false;

		String newFilelistMd5 = "";

		while (!downloadVersionOK)
		{
			code = DownloadFile(src.c_str(), dest.c_str(), 10, NULL);

			if (code == CURLE_OK)
			{
				// ���� check version �ļ��ֶ�


				// new version
				file = fopen(dest.c_str(), "rb");

				if (file)
				{
					fseek(file, 0, SEEK_END);
					long lSize = ftell(file);
					rewind(file);

					char* buff = (char*)LordMalloc(sizeof(char)*lSize + 1);
					buff[lSize] = 0;

					long result = fread(buff, 1, lSize, file);
					if (result != lSize)
					{
						LordAssert("InitConfig reading error");
					}

					fclose(file);

					cJSON* root = cJSON_Parse(buff);

					if (root)
					{
						cJSON* version = cJSON_GetObjectItem(root, "version");
						cJSON* filelistmd5 = cJSON_GetObjectItem(root, "filelistmd5");

						if (version && filelistmd5)
						{
							m_serverVersion = version->valuestring;
							newVersion = StringUtil::Split(version->valuestring, ".");
							newFilelistMd5 = filelistmd5->valuestring;

							if (newVersion.size() == 4 && (!newFilelistMd5.empty()))
							{
								downloadVersionOK = true;
							}
						}

						cJSON_Delete(root);
					}

					LordSafeFree(buff);
				}
			}
		}

		if (nRet >= 0)
		{
			// �����汾�Ŷ����ˣ��Ƚ�һ��
			LordAssert(oldVersion.size() == 4 && newVersion.size() == 4);

			int nOldVersion[4] = { 0 };
			int nNewVersion[4] = { 0 };

			for (int i = 0; i < 4; ++i)
			{
				nOldVersion[i] = StringUtil::ParseInt(oldVersion[i]);
				nNewVersion[i] = StringUtil::ParseInt(newVersion[i]);
			}

			if (nNewVersion[0] > nOldVersion[0])
			{
				nRet = 1;
			}
			else if (nNewVersion[0] == nOldVersion[0] &&
				nNewVersion[1] > nOldVersion[1])
			{
				nRet = 1;
			}
			else if (nNewVersion[0] == nOldVersion[0] &&
				nNewVersion[1] == nOldVersion[1] &&
				nNewVersion[2] > nOldVersion[2])
			{
				nRet = 2;
			}
			else if (nNewVersion[0] == nOldVersion[0] &&
				nNewVersion[1] == nOldVersion[1] &&
				nNewVersion[2] == nOldVersion[2] &&
				nNewVersion[3] > nOldVersion[3])
			{
				nRet = 2;
			}
			else if (nNewVersion[0] == nOldVersion[0] &&
				nNewVersion[1] == nOldVersion[1] &&
				nNewVersion[2] == nOldVersion[2] &&
				nNewVersion[3] == nOldVersion[3])
			{
				nRet = 0;
			}
			else
			{
				nRet = 3;
			}
		}

		if (nRet == 2)
		{
			src = m_gameConfig.fileListName;
			dest = strRootPath;
			dest += m_gameConfig.fileListName + ".tmp";
			String oldFileList = strRootPath;
			oldFileList += m_gameConfig.fileListName;

			code = DownloadFile(src.c_str(), dest.c_str(), 10, newFilelistMd5.c_str());
			if (code == CURLE_OK)
			{
				int result = CompareFilelist(oldFileList.c_str(), dest.c_str());
				if (result < 0)
				{
					nRet = -5;
				}
			}
			else
			{
				nRet = -4;
			}
		}
		curl_easy_cleanup(m_easyDownloadHandle);
		m_easyDownloadHandle = NULL;
		curl_global_cleanup();

		return nRet;
	}

	size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
	{
		FILE* filehandle = (FILE*)userp;
		size_t handlesize = size * nmemb;
		fwrite(buffer, size, nmemb, filehandle);
		return handlesize;
	}

	void CGame::LoadSetting()
	{
		LordLogDebug("CGame::LoadSetting()");
		SoundSetting::loadSetting();
		TeamInfoSetting::loadSetting();
		EngineVersionSetting::loadSetting();
		ScorePointSetting::loadSetting();
		DoorSetting::loadSetting(true);
		CarSetting::loadSetting(true);
		MountSetting::loadSetting(true);
		FilterWord::loadSetting();
		ActionStateSetting::loadSetting();
		RankLangSetting::loadSetting();
		RealTimeRankLangSetting::loadSetting();
		MultiLanTipSetting::loadSetting(true);
		ActorSizeSetting::loadSetting(true);
		GrenadeSetting::loadSetting(true);
		RanchSetting::loadSetting(true);
		CropsSetting::loadSetting(true);
		FriendManager::init();
		RedDotManager::init();
		MailManager::release();
		BulletinSetting::loadSetting(true, false);
        //MonsterSetting::loadMonsterSetting();
		LordNew UserManager();
		LordNew BlockSetting();
		LordNew RecipeTabSetting();
		LordNew CustomArmorSetting();
		LordNew FurnaceSetting();
		LordNew SellItemSetting();
		LordNew AdvertSetting();
		AdvertSetting::Instance()->setGameType(this->Instance()->getGameType());
		LordNew SceneSetting();
		LordNew AbandonItemSetting();
		LordNew GameAnalyticsSetting();
		LordNew GameAnalyticsRequest();
	}

	void CGame::UnloadSetting()
	{
		LordLogDebug("CGame::UnloadSetting()");
		SoundSetting::unloadSetting();
		TeamInfoSetting::unloadSetting();
		GunSetting::unloadSetting();
		BulletClipSetting::unloadSetting();
		ScorePointSetting::unloadSetting();
		DoorSetting::unloadSetting();
		MountSetting::unloadSetting();
		CarSetting::unloadSetting();
		FilterWord::unloadSetting();
		ActionStateSetting::unloadSetting();
		RankLangSetting::unloadSetting();
		RealTimeRankLangSetting::unloadSetting();
		MultiLanTipSetting::unloadSetting();
		ActorSizeSetting::unloadSetting();
		GunExtraAttrSetting::unloadSetting();
		GrenadeSetting::unloadSetting();
		RanchSetting::unloadSetting();
		CropsSetting::unloadSetting();
		FriendManager::release();
		RedDotManager::release();
		MailManager::release();
		GuideSetting::unloadSetting();
		SkillSetting::unloadSetting();
		SkillItemSetting::unloadSetting();
		BulletinSetting::unloadSetting();
		GatherToolSetting::unloadSetting();
		BirdItemSetting::unloadSetting();
		FruitsSetting::unloadSetting();
		HelpSetting::unloadSetting();
		PixelGunGiftBagSetting::unloadSetting();
		ActionAreaManager::unloadSetting();
		BlockCitySetting::unloadSetting();
		DecorationSetting::unloadSetting();
        MonsterSetting::unloadSetting();
		LordDelete(UserManager::Instance());
		BlockSetting::Instance()->unloadSetting();
		LordDelete BlockSetting::Instance();
		RecipeTabSetting::Instance()->unloadSetting();
		LordDelete RecipeTabSetting::Instance();
		CustomArmorSetting::Instance()->unloadSetting();
		LordDelete CustomArmorSetting::Instance();
		FurnaceSetting::Instance()->unloadSetting();
		LordDelete FurnaceSetting::Instance();
		SellItemSetting::Instance()->unloadSetting();
		LordDelete SellItemSetting::Instance();
		SceneSetting::Instance()->unloadSetting();
		LordDelete SceneSetting::Instance();
		LordDelete(BlockFortItemSetting::Instance());
		LordDelete(StoreTabSetting::Instance());
		LordDelete(CustomItemSetting::Instance());
		LordDelete(AdvertSetting::Instance());
		LordDelete(BlockCityPortalSetting::Instance());
		AbandonItemSetting::Instance()->unloadSetting();
		LordDelete AbandonItemSetting::Instance();
		LordDelete ConfigurableBlockSetting::Instance();
		LordDelete GameAnalyticsSetting::Instance();
		LordDelete GameAnalyticsRequest::Instance();
	}

	void CGame::InitConfig(const char* strRootPath)
	{
		char value[256];
		memset(value, 0, 256);
		String filename = strRootPath;
		filename += "game.ini";

		FILE* file = NULL;

		file = fopen(filename.c_str(), "rb");

		if (file)
		{
			fseek(file, 0, SEEK_END);
			long lSize = ftell(file);
			rewind(file);

			char* buff = (char*)LordMalloc(sizeof(char)*lSize + 1);
			buff[lSize] = 0;

			long result = fread(buff, 1, lSize, file);
			if (result != lSize)
			{
				LordAssert("InitConfig reading error");
			}

			fclose(file);

			StringArray keyValueArray = StringUtil::Split(buff, "\n");

			for (size_t i = 0; i < keyValueArray.size(); ++i)
			{
				size_t pos = keyValueArray[i].find("=");
				String strKey = keyValueArray[i].substr(0, pos);
				String strValue = keyValueArray[i].substr(pos + 1);
				strKey = StringUtil::RemoveCharactors(strKey, ' ');
				strKey = StringUtil::RemoveCharactors(strKey, '\r');
				strValue = StringUtil::RemoveCharactors(strValue, ' ');
				strValue = StringUtil::RemoveCharactors(strValue, '\r');

				if (strKey == "httpServer")
				{
					m_gameConfig.updateServerAddress = strValue;

					m_gameConfig.serverList = StringUtil::Split(m_gameConfig.updateServerAddress, ";");
				}
				else if (strKey == "versionFile")
				{
					m_gameConfig.versionFileName = strValue;
				}
				else if (strKey == "fileListName")
				{
					m_gameConfig.fileListName = strValue;
				}
			}

			LordSafeFree(buff);
		}
	}

	std::string FileDigest(const std::string& file, const std::string& pureName)
	{
		std::ifstream in(file.c_str(), std::ios::binary);
		if (!in)
		{
			return "";
		}

		MD5 md5;
		std::streamsize length;
		char buffer[1024];
		while (!in.eof())
		{
			in.read(buffer, 1024);
			length = in.gcount();
			if (length > 0)
			{
				md5.update(buffer, static_cast<size_t>(length));
			}
		}

		in.close();
		return md5.toString();
	}

	CURLcode CGame::DownloadFile(const char* srcPath, const char* destPath, long timeout, const char* md5)
	{
		LordAssert(m_easyDownloadHandle);

		CURLcode code = CURLE_FAILED_INIT;
		long args = 0;
		bool downloadOK = false;

		do
		{
			if (m_gameConfig.nCurrentServerIndex >= m_gameConfig.serverList.size())
			{
				m_gameConfig.nCurrentServerIndex = 0;
			}

			m_gameConfig.updateServerAddress = m_gameConfig.serverList[m_gameConfig.nCurrentServerIndex];

			String serverPath = m_gameConfig.updateServerAddress + srcPath;

			FILE* pFile = NULL;
			pFile = fopen(destPath, "wb");

			if (pFile)
			{
				curl_easy_setopt(m_easyDownloadHandle, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(m_easyDownloadHandle, CURLOPT_WRITEDATA, pFile);
				curl_easy_setopt(m_easyDownloadHandle, CURLOPT_URL, serverPath.c_str());
				//curl_easy_setopt(m_easyDownloadHandle, CURLOPT_FOLLOWLOCATION, 1);
				curl_easy_setopt(m_easyDownloadHandle, CURLOPT_TIMEOUT, timeout);

				code = curl_easy_perform(m_easyDownloadHandle);

				code = curl_easy_getinfo(m_easyDownloadHandle, CURLINFO_HTTP_CODE, &args);

				fclose(pFile);
				pFile = NULL;


				if (code == CURLE_OK && args < 400) {

					if (md5)
					{
						// check md5
						std::string downloadmd5 = FileDigest(destPath, destPath);

						downloadOK = (downloadmd5 == md5);
					}
					else
					{
						downloadOK = true;
					}
				}
				else {

					outputLogFormate(m_updateLogHandle, "[update]:download file error code [%d] src [%s] dest [%s] \n", args, serverPath.c_str(), destPath);

					++m_gameConfig.nCurrentServerIndex;

				}
			}
			else
			{
				// �ļ���ʧ�ܣ��������ǲ����ܵģ���������
				outputLogFormate(m_updateLogHandle, "[update]:download file error src [%s] dest [%s] \n", serverPath.c_str(), destPath);

				return code;
			}

		} while (downloadOK != true);

		return code;
	}

	int CGame::CompareFilelist(const char* oldlist, const char* newlist)
	{
		int result = ReadFilelist(oldlist, m_oldfilelist);
		result = ReadFilelist(newlist, m_newfilelist);

		for (FILE_LIST_TYPE::iterator iter = m_newfilelist.begin(); iter != m_newfilelist.end(); ++iter)
		{
			FILE_LIST_TYPE::iterator oldIter = m_oldfilelist.find(iter->second.filename);
			if (oldIter != m_oldfilelist.end())
			{
				if (oldIter->second.md5 != iter->second.md5)
				{
					// modify
					updateFileData data;
					data.filename = iter->second.filename;
					data.path = iter->second.path;
					data.md5 = iter->second.md5;
					data.isPak = iter->second.isPak;
					data.operate = 2;
					data.size = iter->second.size;

					m_needUpdatefile.push_back(data);
				}
			}
			else
			{
				// add
				updateFileData data;
				data.filename = iter->second.filename;
				data.path = iter->second.path;
				data.md5 = iter->second.md5;
				data.isPak = iter->second.isPak;
				data.operate = 1;
				data.size = iter->second.size;

				m_needUpdatefile.push_back(data);
			}
		}

		for (FILE_LIST_TYPE::iterator iter = m_oldfilelist.begin(); iter != m_oldfilelist.end(); ++iter)
		{
			FILE_LIST_TYPE::iterator newIter = m_newfilelist.find(iter->second.filename);
			if (newIter == m_newfilelist.end())
			{
				// delete
				updateFileData data;
				data.filename = iter->second.filename;
				data.path = iter->second.path;
				data.md5 = iter->second.md5;
				data.isPak = iter->second.isPak;
				data.operate = 3;
				data.size = 0; // delete no need to download

				m_needUpdatefile.push_back(data);
			}
		}

		m_oldfilelist.clear();
		m_newfilelist.clear();
		m_needUpdateNum = m_needUpdatefile.size();

		m_totalUpdateSize = 0;
		for (size_t i = 0; i < m_needUpdatefile.size(); ++i)
		{
			m_totalUpdateSize += m_needUpdatefile[i].size;
		}

		return result;
	}

	int CGame::ReadFilelist(const char* filelistPath, FILE_LIST_TYPE& filelist)
	{
		FILE* file = NULL;
		file = fopen(filelistPath, "rb");
		int result = 0;

		if (file)
		{
			fseek(file, 0L, SEEK_END);
			unsigned int nFileReadSize = ftell(file);
			unsigned char* RealFileBuffer = (unsigned char*)LordMalloc(nFileReadSize);
			fseek(file, 0L, SEEK_SET);
			fread(RealFileBuffer, nFileReadSize, 1, file);
			fclose(file); file = NULL;

			unsigned char* buff = RealFileBuffer;
			long lSize = nFileReadSize;

			ResourcePack packTools;
			unsigned int nRealSize = ((unsigned int*)RealFileBuffer)[0];

			unsigned char* originBuffZipStart = RealFileBuffer + sizeof(unsigned int);
			unsigned char* lpszOut = (unsigned char*)LordMalloc(nRealSize);
			if (nFileReadSize >= 8)
			{
				unsigned int unzipSize = nRealSize;
				int nRet = packTools.Zlib_UnCompress(lpszOut, &unzipSize, originBuffZipStart, nFileReadSize - sizeof(unsigned int));

				lSize = nRealSize;

				if (0 != nRet)
				{
					outputLogFormate(m_updateLogHandle, "uncompress file list error: %s", filelistPath);

					LordFree(RealFileBuffer);
					LordFree(lpszOut);
					return -1;
				}

				buff = lpszOut;
			}

			// -------------------------��ʼ����filelist�ļ�-------------------
			unsigned char* s = buff;
			unsigned char* e = buff;
			for (int i = 0; i < lSize; ++i)
			{
				char c = buff[i];

				if (c == '\n')
				{
					e = buff + i;

					char* linebuff = (char*)LordMalloc(e - s + 1);
					memcpy(linebuff, s, e - s);
					linebuff[e - s] = 0;
					StringArray filedata = StringUtil::Split(linebuff, "|");
					//LordAssert(filedata.size() == 5);
					updateFileData data;
					data.filename = filedata[0];
					data.path = filedata[1];
					data.md5 = filedata[2];
					data.isPak = StringUtil::ParseBool(filedata[3]);
					if (filedata.size() >= 5)
					{
						data.size = StringUtil::ParseInt(filedata[4]);
					}
					else
					{
						data.size = 0;
					}
					data.operate = 0;

					LORD::map<LORD::String, updateFileData>::type::iterator iter = filelist.find(data.filename);
					if (iter != filelist.end())
					{
						String debug = StringUtil::Format("%s %s, %s %s \n", data.filename.c_str(), data.path.c_str(), iter->second.filename.c_str(), iter->second.path.c_str());
					}

					filelist[data.filename] = data;
					LordSafeFree(linebuff);
					s = e + 1;
					e = s;
				}
			}

			LordFree(RealFileBuffer);
			LordFree(lpszOut);
		}
		else
		{
			result = -1;
		}

		return result;
	}

	int CGame::DownloadAllFiles(const char* strRootPath)
	{
		int result = 0;

		CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
		m_easyDownloadHandle = curl_easy_init();

		String strTempPath = GetUpdateTempDir(strRootPath);

		if (!PathUtil::IsDirExist(strTempPath))
		{
			PathUtil::CreateDir(strTempPath);
		}
		else
		{
			PathUtil::DelFilesInDir(strTempPath);
		}
		m_curUpdateIndex = 0;

		m_curDownloadSize = 0;

		for (size_t i = 0; i < m_needUpdatefile.size(); ++i)
		{
			int operate = m_needUpdatefile[i].operate;
			if (operate == 1 ||
				operate == 2)
			{
				// add
				// modify
				// need download
				String localFilePath = strTempPath + m_needUpdatefile[i].filename;

				m_needUpdatefile[i].path = StringUtil::Replace(m_needUpdatefile[i].path, "\\", "/");
				String serverFilePath = m_needUpdatefile[i].path + m_needUpdatefile[i].filename;

				code = DownloadFile(serverFilePath.c_str(), localFilePath.c_str(), 10, m_needUpdatefile[i].md5.c_str());

				outputLogFormate(m_updateLogHandle, "[update]: download file %s\n", localFilePath.c_str());

				m_curDownloadSize += m_needUpdatefile[i].size;

				if (code != CURLE_OK)
				{
					result = -1;
				}
			}
			m_curUpdateIndex = i + 1;
		}

		curl_easy_cleanup(m_easyDownloadHandle);
		m_easyDownloadHandle = NULL;
		curl_global_cleanup();

		return result;
	}

	int CGame::UpdateFiles()
	{
		m_updateState = 0;

		String logpath = m_rootPath + "updatelog.txt";
		m_updateLogHandle = fopen(logpath.c_str(), "wb");

		const char* strRootPath = m_rootPath.c_str();

		int result = DownloadAllFiles(strRootPath);

		m_updateState = 1;

		if (result == 0)
		{
			m_curUpdateIndex = 0;
			m_needUpdateNum = m_needUpdatefile.size();

			// copy to dir and write to pak
			ResourcePack pakHandle;
			String strPakPath = strRootPath;
			strPakPath += "Media.pak";

			String strUpdateTemp = GetUpdateTempDir(strRootPath);

			if (PathUtil::IsFileExist(strPakPath))
			{
				pakHandle.OpenFile(strPakPath.c_str());
				pakHandle.SetArchivePath(strUpdateTemp.c_str());

				for (size_t i = 0; i < m_needUpdatefile.size(); ++i)
				{
					if (m_needUpdatefile[i].operate == 1)
					{
						// add
						if (m_needUpdatefile[i].isPak == true)
						{
							pakHandle.AddZlibFile(m_needUpdatefile[i].filename.c_str());

							outputLogFormate(m_updateLogHandle, "[update]:[add file] pak file %s\n", m_needUpdatefile[i].filename.c_str());
						}
						else
						{
							String src = strUpdateTemp + m_needUpdatefile[i].filename;
							String dest = strRootPath;
							String path = m_needUpdatefile[i].path;

							path = dest + path;


							path = StringUtil::Replace(path, "\\", "/");

							if (!PathUtil::IsDirExist(path))
							{
								PathUtil::CreateDir(path);
							}

							dest = path + m_needUpdatefile[i].filename;

							PathUtil::CopyFilePath(src, dest);

							outputLogFormate(m_updateLogHandle, "[update]:[add file] single file %s\n", m_needUpdatefile[i].filename.c_str());
						}
					}
					else if (m_needUpdatefile[i].operate == 2)
					{
						// modify
						if (m_needUpdatefile[i].isPak == true)
						{
							pakHandle.DelFile(m_needUpdatefile[i].filename.c_str());
							pakHandle.AddZlibFile(m_needUpdatefile[i].filename.c_str());

							outputLogFormate(m_updateLogHandle, "[update]:[modify file] pak file %s\n", m_needUpdatefile[i].filename.c_str());
						}
						else
						{
							String src = strUpdateTemp + m_needUpdatefile[i].filename;
							String dest = strRootPath;
							String path = m_needUpdatefile[i].path;
							int pos = path.find("Media");
							if (pos == 0)
							{
								path = dest + path.substr(6);
							}
							else
							{
								path = dest + path;
							}

							path = StringUtil::Replace(path, "\\", "/");

							if (!PathUtil::IsDirExist(path))
							{
								PathUtil::CreateDir(path);
							}

							dest = path + m_needUpdatefile[i].filename;

							PathUtil::CopyFilePath(src, dest);

							outputLogFormate(m_updateLogHandle, "[update]:[modify file] single file %s\n", m_needUpdatefile[i].filename.c_str());
						}
					}
					else if (m_needUpdatefile[i].operate == 3)
					{
						// delete
						// modify
						if (m_needUpdatefile[i].isPak == true)
						{
							pakHandle.DelFile(m_needUpdatefile[i].filename.c_str());

							outputLogFormate(m_updateLogHandle, "[update]:[delete file] pak file %s\n", m_needUpdatefile[i].filename.c_str());
						}
						else
						{
							String dest = strRootPath;
							String path = m_needUpdatefile[i].path;
							int pos = path.find("Media");
							if (pos == 0)
							{
								path = dest + path.substr(6);
							}
							else
							{
								path = dest + path;
							}

							path = StringUtil::Replace(path, "\\", "/");

							dest = path + m_needUpdatefile[i].filename;

							PathUtil::DelPath(dest);

							outputLogFormate(m_updateLogHandle, "[update]:[delete file] single file %s\n", m_needUpdatefile[i].filename.c_str());
						}
					}
					else
					{
					}

					m_curUpdateIndex = i + 1;
				}

				pakHandle.CloseFile();
			}
			else
			{
				if (CopyAllFiles(strUpdateTemp, strRootPath) == 0)
				{
					//LordLogError("replace files of Update failed! \n");
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "replace files of Update failed!");
					MessageBox(0, buf, 0, 0);
#endif
				}

				if (m_updateLogHandle)
				{
					fclose(m_updateLogHandle);
					m_updateLogHandle = NULL;
				}

				return -1;
			}

			String updateTempDir = GetUpdateTempDir(strRootPath);
			PathUtil::DelPath(updateTempDir);
			String strFilelist = strRootPath;
			strFilelist += m_gameConfig.fileListName;
			PathUtil::DelPath(strFilelist);

			String strTmpFilelist = strFilelist + ".tmp";
			PathUtil::RenameFile(strTmpFilelist, strFilelist);

			String strVersionFile = strRootPath;
			strVersionFile += m_gameConfig.versionFileName;
			PathUtil::DelPath(strVersionFile);

			String strTmpVersionFile = strVersionFile + ".tmp";

			PathUtil::RenameFile(strTmpVersionFile, strVersionFile);
		}
		else
		{
			return -1;
		}
		m_needUpdatefile.clear();

		if (m_updateLogHandle)
		{
			fclose(m_updateLogHandle);
			m_updateLogHandle = NULL;
		}

		return 1;
	}

	LORD::String CGame::GetUpdateTempDir(const char* strRootPath)
	{
		String strTempPath = strRootPath;
		strTempPath += "updateTemp/";

		return strTempPath;
	}

	float CGame::GetUpdatePercent()
	{
		if (m_needUpdateNum == 0)
		{
			return 1.0f;
		}
		else
		{
			return (float)m_curUpdateIndex / (float)m_needUpdateNum;
		}
	}

	int CGame::CopyAllFiles(const LORD::String& srcRoot, const LORD::String& dstRoot)
	{
		for (size_t i = 0; i < m_needUpdatefile.size(); ++i)
		{
			String filepath = m_needUpdatefile[i].path + m_needUpdatefile[i].filename;
			if (m_needUpdatefile[i].operate == 1 || m_needUpdatefile[i].operate == 2)
			{
				if (!PathUtil::CopyFilePath(srcRoot + m_needUpdatefile[i].filename, dstRoot + filepath))
				{
					//LordLogError("replace file failed! %s \n", m_needUpdatefile[i].filename.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "replace file failed! %s", m_needUpdatefile[i].filename.c_str());
					MessageBox(0, buf, 0, 0);
#endif
					return 0;
				}
			}
			else if (m_needUpdatefile[i].operate == 3)//delete dst file
			{
				if (!PathUtil::DelPath(dstRoot + filepath))
				{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "delete old file failed! may be it is not found! %s", m_needUpdatefile[i].filename.c_str());
					MessageBox(0, buf, 0, 0);
#endif
				}
			}

			m_curUpdateIndex = i + 1;
		}
		return 1;
	}

	void CGame::handleTouchClick(float x, float y)
	{
		Vector2 outPosition;
		GUISystem::Instance()->LogicPositinToScreenPosition(Vector2(x, y), outPosition);
		float PosX[] = { outPosition.x };
		float PosY[] = { outPosition.y };
		intptr_t ids[] = { 19940703 };
		handleTouchesBegin(1, ids, PosX, PosY);
		handleTouchesEnd(1, ids, PosX, PosY);
	}

	bool CGame::handleTouchesBegin(int num, intptr_t ids[], float xs[], float ys[])
	{
		bool touchOnUI = false;
		try
		{
			intptr_t id = 0;
			float x = 0.0f;
			float y = 0.0f;
			int unusedIndex = 0;

			//LordLogError("handleTouchesBegin ---------!");

			// 			LORD::vector<Touch*>::type sceneTouchList;
			// 			sceneTouchList.clear();

			for (int i = 0; i < num; ++i)
			{
				id = ids[i];
				x = xs[i];
				y = ys[i];

				Touch* touch = TouchManager::Instance()->getTouch(id);

				if (touch == NULL)
				{
					// it is a new touch
					ui32 time = Root::Instance()->getCurrentTime();
					Vector2 outPosition;

					GUISystem::Instance()->AdaptPosition(Vector2(x, y), outPosition);

					GUIWindow* pTarget = GUISystem::Instance()->GetTargetGUIWindow(outPosition);
					if (pTarget && !TouchManager::Instance()->hasGUITouch(pTarget->GetName()))
					{
						touchOnUI = true;
						touch = TouchManager::Instance()->createTouch(id, TOUCH_TYPE_UI, time);

						if (touch)
						{
							touch->setWindowName(pTarget->GetName());
							touch->setTouchPoint(outPosition);
							GUISystem::Instance()->handleTouchDown(touch);
						}
					}
					else
					{
						if (!ClientNetwork::Instance()->isConnected())
							return false;
						int sceneTouchCount = TouchManager::Instance()->getSceneTouchCount();
						if (sceneTouchCount > 0) {
							LordLogInfo("handleTouchesBegin, sceneTouchCount > 0");
							return touchOnUI;
						}
						if (TouchManager::Instance()->hasMutexTouchWithScene())
						{
							LordLogInfo("handleTouchesBegin, hasMutexTouchWithScene");
							return touchOnUI;
						}
						touch = TouchManager::Instance()->createTouch(id, TOUCH_TYPE_SCENE, time);
						if (touch)
						{
							static float tolerance = m_displayDensity * 10;
							touch->setTouchPoint(Vector2(x, y), tolerance);
							if (m_blockman && m_blockman->m_gameSettings)
							{
								m_blockman->m_gameSettings->beginMouseMove(x, y);
								BLOCKMAN::BreakBlockUiManageEvent::emit(true, outPosition.x, outPosition.y);
							}
						}
					}
				}
				else
				{
					LordLogError("handleTouchesBegin error touch already exist!");
				}
			}
		}
		catch (...)
		{
			Exception e;
			e.setMessage("undefined exception");
		}
		return touchOnUI;
	}

	bool CGame::handleTouchesMove(int num, intptr_t ids[], float xs[], float ys[])
	{
		bool nResult = false;
		try
		{
			intptr_t id = 0;
			float x = 0.0f;
			float y = 0.0f;

			//LordLogError("handleTouchesMove ---------!");

			// 			LORD::vector<Touch*>::type sceneTouchList;
			// 			sceneTouchList.clear();

			for (int i = 0; i < num; ++i)
			{
				id = ids[i];
				x = xs[i];
				y = ys[i];

				Touch* touch = TouchManager::Instance()->getTouch(id);
				if (touch)
				{
					Vector2 uiPosition;
					GUISystem::Instance()->AdaptPosition(Vector2(x, y), uiPosition);
					if (touch->getType() == TOUCH_TYPE_UI)
					{
						nResult = true;
						touch->setTouchPoint(uiPosition);

						if (!touch->isMoved())
						{
							continue;
						}

						Vector2 outPosition;
						GUISystem::Instance()->AdaptPosition(Vector2(x, y), outPosition);
						GUIWindow* pTarget = GUISystem::Instance()->GetTargetGUIWindow(outPosition);
						if (pTarget) //&& pTarget->GetName() != touch->getWindowName())
						{
							touch->setMutexWithScene(true);
							if (touch->getMutexWithScene() && TouchManager::Instance()->getSceneTouchCount() > 0)
							{
								continue;
							}
							if (pTarget->GetName() != touch->getWindowName())
							{
								GUISystem::Instance()->handleTouchDown(touch);
							}
							GUISystem::Instance()->handleTouchMove(touch);
							touch->setWindowName(pTarget->GetName());
							touch->setTouchPoint(outPosition);
						}
						else
						{
							if (!ClientNetwork::Instance()->isConnected())
								return false;
							Touch* sceneTouch1 = TouchManager::Instance()->getSceneTouch1();
							GUISystem::Instance()->handleTouchUp(touch);
							if (!sceneTouch1)
							{
								touch->setTouchType(TOUCH_TYPE_SCENE);
							}
						}
					}
					else if (touch->getType() == TOUCH_TYPE_SCENE)
					{
						if (!ClientNetwork::Instance()->isConnected())
							return false;
						static float tolerance = m_displayDensity * 8;
						touch->setTouchPoint(Vector2(x, y), tolerance);
						if (m_blockman && m_blockman->m_gameSettings)
						{
							m_blockman->m_gameSettings->setMousePos(x, y);
							BLOCKMAN::BreakBlockUiManageEvent::emit(true, uiPosition.x, uiPosition.y);
						}
					}
					else
					{
						LordLogError("handleTouchesMove error, unknown touch type occur, type=%d", (int)(touch->getType()));
					}
				}
			}

			int sceneTouchCount = TouchManager::Instance()->getSceneTouchCount();

			if (sceneTouchCount == 1 && TouchManager::Instance()->getSceneTouch1())
			{
				Vector2 offset = TouchManager::Instance()->getSceneTouch1()->getTouchPoint() - TouchManager::Instance()->getSceneTouch1()->getStartPoint();
				if (offset.len() > 10)
				{
					//LuaEngine::Instance()->handleTouchMove(TouchManager::Instance()->getSceneTouch1());
				}
			}
			else if (sceneTouchCount > 1)
			{
				//LuaEngine::Instance()->handleMultiTouchMove(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
			}
			else
			{
			}
		}
		catch (...)
		{
			Exception e;
			e.setMessage("undefined exception");
			// LuaEngine::Instance()->exceptionHandle(e);
		}

		return nResult;
	}

	bool CGame::handleTouchesEnd(int num, intptr_t ids[], float xs[], float ys[])
	{
		bool nResult = false;
		try
		{
			intptr_t id = 0;
			float x = 0.0f;
			float y = 0.0f;

			//LordLogError("handleTouchesEnd ---------!");

			bool deleteflag[TouchManager::MAX_TOUCHES] = { false };

			int sceneEndCount = 0;
			Touch* sceneEndTouch = NULL;

			for (int i = 0; i < num; ++i)
			{
				id = ids[i];
				x = xs[i];
				y = ys[i];

				Touch* touch = TouchManager::Instance()->getTouch(id);
				if (touch)
				{
					deleteflag[i] = true;
					Vector2 uiPosition;

					//LordLogDebug("touch end?x %f, y %f", x, y);

					GUISystem::Instance()->AdaptPosition(Vector2(x, y), uiPosition);
					if (touch->getType() == TOUCH_TYPE_UI)
					{
						nResult = true;
						touch->setTouchPoint(uiPosition);
						GUISystem::Instance()->handleTouchUp(touch);
					}
					else if (touch->getType() == TOUCH_TYPE_SCENE)
					{
						static float tolerance = m_displayDensity * 10;
						touch->setTouchPoint(Vector2(x, y), tolerance);
						if (m_blockman && m_blockman->m_gameSettings)
						{
							m_blockman->m_gameSettings->endMouseMove(x, y);
						}
						BLOCKMAN::BreakBlockUiManageEvent::emit(false, 0.0f, 0.f);
						++sceneEndCount;
						sceneEndTouch = touch;
					}
					else
					{
						LordLogError("handleTouchesEnd error, unknown touch type occur, type=%d", (int)(touch->getType()));
					}
				}
				else
				{
					LordLogError("handleTouchesEnd error touch not exist!");
				}
			}

			int sceneTouchCount = TouchManager::Instance()->getSceneTouchCount();

			if (sceneEndCount == 1 && sceneTouchCount == 1 && TouchManager::Instance()->getSceneTouch1())
			{
				if (m_blockman && m_blockman->m_gameSettings)
				{
					//LuaEngine::Instance()->handleTouchUp(TouchManager::Instance()->getSceneTouch1());
					if (sceneEndTouch->isClick()) {
						m_blockman->m_gameSettings->userAction = BLOCKMAN::GameSettings::ACTION_CLICK;
					}
					else if (m_blockman->m_gameSettings->userAction == BLOCKMAN::GameSettings::ACTION_LONG_TOUCH_BEGIN) {
						m_blockman->m_gameSettings->userAction = BLOCKMAN::GameSettings::ACTION_LONG_TOUCH_END;
					}
				}
			}
			else if (sceneTouchCount > 1)
			{
				LordLogError("handleTouchesEnd error, sceneTouchCount > 1, should not occur");
				if (sceneEndCount == 1)
				{
					if (sceneEndTouch == TouchManager::Instance()->getSceneTouch1())
					{
						//LuaEngine::Instance()->handleMultiTouchUpOne(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
					}
					else
					{
						//LuaEngine::Instance()->handleMultiTouchUpOne(TouchManager::Instance()->getSceneTouch2(), TouchManager::Instance()->getSceneTouch1());
					}
				}
				else
				{
					//LuaEngine::Instance()->handleMultiTouchUp(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
				}
			}

			for (int i = 0; i < num; ++i)
			{
				if (deleteflag[i])
				{
					deleteflag[i] = false;
					id = ids[i];
					TouchManager::Instance()->destroyTouch(id);
				}
			}
		}
		catch (...)
		{
			Exception e;
			e.setMessage("undefined exception");
			//LuaEngine::Instance()->exceptionHandle(e);
		}

		return nResult;
	}

	bool CGame::handleTouchesCancel(int num, intptr_t ids[], float xs[], float ys[])
	{
		bool nResult = false;

		try
		{
			intptr_t id = 0;
			float x = 0.0f;
			float y = 0.0f;

			bool deleteflag[TouchManager::MAX_TOUCHES] = { false };

			int sceneEndCount = 0;
			Touch* sceneEndTouch = NULL;

			for (int i = 0; i < num; ++i)
			{
				id = ids[i];
				x = xs[i];
				y = ys[i];

				Touch* touch = TouchManager::Instance()->getTouch(id);

				if (touch)
				{
					deleteflag[i] = true;
					Vector2 uiPosition;
					GUISystem::Instance()->AdaptPosition(Vector2(x, y), uiPosition);
					if (touch->getType() == TOUCH_TYPE_UI)
					{
						nResult = true;
						touch->setTouchPoint(uiPosition);
						GUISystem::Instance()->handleTouchCancel(touch);
					}
					else if (touch->getType() == TOUCH_TYPE_SCENE)
					{
						touch->setTouchPoint(Vector2(x, y));

						sceneEndTouch = touch;
						++sceneEndCount;
						BLOCKMAN::BreakBlockUiManageEvent::emit(false, 0.f, 0.f);
					}
					else
					{
					}
				}
			}

			int sceneTouchCount = TouchManager::Instance()->getSceneTouchCount();

			if (sceneEndCount == 1 && sceneTouchCount == 1)
			{
				//LuaEngine::Instance()->handleTouchCancel(TouchManager::Instance()->getSceneTouch1());
			}
			else if (sceneTouchCount > 1)
			{
				if (sceneEndCount == 1)
				{
					if (sceneEndTouch == TouchManager::Instance()->getSceneTouch1())
					{
						//LuaEngine::Instance()->handleMultiTouchCancelOne(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
					}
					else
					{
						//LuaEngine::Instance()->handleMultiTouchCancelOne(TouchManager::Instance()->getSceneTouch2(), TouchManager::Instance()->getSceneTouch1());
					}
				}
				else
				{
					//LuaEngine::Instance()->handleMultiTouchCancel(TouchManager::Instance()->getSceneTouch1(), TouchManager::Instance()->getSceneTouch2());
				}
			}
			else
			{
			}

			if (m_blockman && m_blockman->m_gameSettings)
			{
				m_blockman->m_gameSettings->userAction = BLOCKMAN::GameSettings::ACTION_NULL;
			}
	
			for (int i = 0; i < num; ++i)
			{
				if (deleteflag[i])
				{
					deleteflag[i] = false;
					id = ids[i];
					TouchManager::Instance()->destroyTouch(id);
				}
			}
		}
		catch (...)
		{
			Exception e;
			e.setMessage("undefined exception");
			//LuaEngine::Instance()->exceptionHandle(e);
		}

		return nResult;
	}

	void CGame::handleKeyDown(int key)
	{
		KeyEventManager::Instance()->onKeyDown(key);
		BLOCKMAN::KeyBinding::setKeyBindState(key, true);
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		BLOCKMAN::ClientCmdMgr::onKeyDown(key);
#endif
	}

	void CGame::handleKeyUp(int key)
	{
		KeyEventManager::Instance()->onKeyUp(key);
		BLOCKMAN::KeyBinding::setKeyBindState(key, false);
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		BLOCKMAN::ClientCmdMgr::onKeyUp(key);
#endif
	}

	void CGame::handleMouseMove(int x, int y)
	{
		KeyEventManager::Instance()->onMouseMove(x, y);
		if (m_blockman && m_blockman->m_gameSettings)
		{
			m_blockman->m_gameSettings->setMousePos(float(x), float(y));
		}
	}

	void CGame::GetPhoneType(const char* strPhoneType)
	{
		Root::Instance()->deviceFeatures().phoneType() = strPhoneType;
	}

	const String& CGame::GetPhoneInfo() const
	{
		return Root::Instance()->deviceFeatures().phoneType();
	}

	const String& CGame::GetGPUInfo() const
	{
		return Root::Instance()->deviceFeatures().rendererName();
	}


	void CGame::printLuaStack()
	{

	}

	void CGame::handleMotionBegin()
	{

	}

	void CGame::handleMotionCancelled()
	{

	}

	void CGame::handleMotionEnded()
	{

	}

	ClientGameType CGame::GetGameType()
	{
		return BLOCKMAN::LogicSetting::Instance()->getGameType();
	}

	float CGame::getFrameTime()
	{
		return (float)g_framedt;
	}

	void CGame::hideRechargeBtn()
	{
		BLOCKMAN::DisableRechargeEvent::emit();
	}

	void CGame::setGameType(String gameType)
	{
		this->m_gameType = gameType.length() > 0 ? gameType : m_gameType;
		CarSetting::m_sbIsCarFree = GameTypeSetting::IsCarFree(m_gameType);
		LogicSetting::Instance()->setGameType(GameTypeSetting::GetGameType(m_gameType));
		BLOCKMAN::ServerSyncGameTypeEvent::emit();
		if (GetGameType() == ClientGameType::BirdSimulator)
		{
			BLOCKMAN::Blockman::Instance()->setPerspece(1);
		}
	}
}
