#include "Server.h"
#include "Util/Event.h"
#include "Network/RoomClient.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "LuaRegister/Template/LuaEngine.h"
#include "Util/MacroDefine.h"
#include "Test.h"
#include "Script/Setting/ScriptSetting.h"
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
#include <signal.h>
#endif

//#define _ITERATOR_DEBUG_LEVEL 0

RoomGameConfig getTestRGConfig(const char * curPath) {

	FILE* fp = fopen("serverConfig.json", "rb"); // must be "rb"
	if (!fp) {
		std::cerr << "Could not open configuration file serverConfig.json";
		exit(1);

	}

	char readBuffer[65536];  // temp buffer
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document doc;
	if (doc.ParseStream(is).HasParseError()) {
		std::cerr << "serverConfig.json Parse Error!";
		fclose(fp);
		exit(1);
	}

	fclose(fp);

	ScriptSetting::loadGameScriptDir("", false);

	RoomGameConfig cfg;
	cfg.gameId = doc["gameId"].GetString();
	cfg.gameIp = doc["gameIp"].GetString();
	cfg.serverPort = doc["serverPort"].GetInt();
	cfg.monitorAddr = doc["monitorAddr"].GetString();
	cfg.gameType = ScriptSetting::m_gameType;

	cfg.maxPlayers = doc["maxPlayers"].GetInt();
	cfg.mapID = doc["mapID"].GetString();
	cfg.userConfig = doc["userConfig"].GetString();

	cfg.propAddr = doc["propAddr"].GetString();
	cfg.rankAddr = doc["rankAddr"].GetString();
	cfg.rewardAddr = doc["rewardAddr"].GetString();

	cfg.secret = doc["secret"].GetString();
	cfg.isDebug = doc["isDebug"].GetBool();

	cfg.isChina = doc["isChina"].GetBool();
	cfg.blockymodsUrl = doc["blockymodsUrl"].GetString();

	cfg.blockmanUrl = doc["blockmanUrl"].GetString();
	cfg.blockymodsRewardAddr = doc["blockymodsRewardAddr"].GetString();
	cfg.dbIp = doc["dbIp"].GetString();

	cfg.dbUsername = doc["dbUsername"].GetString();
	cfg.dbPassword = doc["dbPassword"].GetString();
	cfg.dbName = doc["dbName"].GetString();
	cfg.redisDbIp = doc["redisDbIp"].GetString();
	cfg.redisPort = doc["redisPort"].GetInt();
	cfg.redisDbPassword = doc["redisDbPassword"].GetString();
	cfg.dbHttpServiceUrl = doc["dbHttpServiceUrl"].GetString();
	cfg.dbHttpServiceRetryUrl = doc["dbHttpServiceRetryUrl"].GetString();
	cfg.heartbeatInterval = doc["heartbeatInterval"].GetInt();
	cfg.gameRankParams = doc["gameRankParams"].GetString();
	cfg.isGameParty = doc["isGameParty"].GetBool();
	return cfg;
}

RoomGameConfig getRGConfigFromCmdline(const char* cfgJsonStr) {
	RoomGameConfig cfg;

	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->Parse(cfgJsonStr);
	if (doc->HasParseError()) {
		std::abort();
	}
	cfg.gameIp = doc->HasMember("ip") ? doc->FindMember("ip")->value.GetString() : "";
	cfg.serverPort = doc->HasMember("port") ? doc->FindMember("port")->value.GetInt() : 0;
	cfg.logDir = doc->HasMember("logdir") ? doc->FindMember("logdir")->value.GetString() : "";
	cfg.scriptDir = doc->HasMember("scriptdir") ? doc->FindMember("scriptdir")->value.GetString() : "";
	cfg.mapDir = doc->HasMember("mapdir") ? doc->FindMember("mapdir")->value.GetString() : "";
	cfg.gameId = doc->HasMember("id") ? doc->FindMember("id")->value.GetString() : "";
	cfg.mapID = doc->HasMember("mapid") ? doc->FindMember("mapid")->value.GetString() : "";
	cfg.gameName = doc->HasMember("name") ? doc->FindMember("name")->value.GetString() : "";
	cfg.monitorAddr = doc->HasMember("monitoraddr") ? doc->FindMember("monitoraddr")->value.GetString() : "";
	cfg.maxPlayers = doc->HasMember("maxplayer") ? doc->FindMember("maxplayer")->value.GetInt() : 0;
	cfg.gameType = doc->HasMember("gtype") ? doc->FindMember("gtype")->value.GetString() : "";
	cfg.propAddr = doc->HasMember("propAddr") ? doc->FindMember("propAddr")->value.GetString() : "";
	cfg.rankAddr = doc->HasMember("rankAddr") ? doc->FindMember("rankAddr")->value.GetString() : "";
	cfg.rewardAddr = doc->HasMember("rewardAddr") ? doc->FindMember("rewardAddr")->value.GetString() : "";
	cfg.blockymodsUrl = doc->HasMember("blockymodsUrl") ? doc->FindMember("blockymodsUrl")->value.GetString() : "";
	cfg.blockymodsRewardAddr = doc->HasMember("blockymodsRewardAddr") ? doc->FindMember("blockymodsRewardAddr")->value.GetString() : "";
	cfg.blockmanUrl = doc->HasMember("blockmanUrl") ? doc->FindMember("blockmanUrl")->value.GetString() : "";
	cfg.isDebug = doc->HasMember("isDebug") ? doc->FindMember("isDebug")->value.GetBool() : false;
	cfg.userConfig = doc->HasMember("userConf") ? doc->FindMember("userConf")->value.GetString() : "";
	cfg.secret = doc->HasMember("secret") ? doc->FindMember("secret")->value.GetString() : "";
	cfg.isChina = doc->HasMember("isChina") ? doc->FindMember("isChina")->value.GetBool() : false;
	cfg.heartbeatInterval = doc->HasMember("heartbeatInterval") ? doc->FindMember("heartbeatInterval")->value.GetInt() : 5;
	if (doc->HasMember("dbconfig")) {
		rapidjson::Value a = doc->FindMember("dbconfig")->value.GetObject();
		cfg.dbIp = a.HasMember("addr") ? a.FindMember("addr")->value.GetString() : "";
		cfg.dbUsername = a.HasMember("user") ? a.FindMember("user")->value.GetString() : "";
		cfg.dbPassword = a.HasMember("password") ? a.FindMember("password")->value.GetString() : "";
		cfg.dbName = a.HasMember("dbname") ? a.FindMember("dbname")->value.GetString() : "";
	}

	if (doc->HasMember("redisConfig")) {
		rapidjson::Value a = doc->FindMember("redisConfig")->value.GetObject();
		cfg.redisDbIp = a.HasMember("ip") ? a.FindMember("ip")->value.GetString() : "";
		cfg.redisDbPassword = a.HasMember("password") ? a.FindMember("password")->value.GetString() : "";
		cfg.redisPort = a.HasMember("port") ? a.FindMember("port")->value.GetInt() : 0;
	}

	cfg.dbHttpServiceUrl = doc->HasMember("gameDataServiceAddr") ? doc->FindMember("gameDataServiceAddr")->value.GetString() : "";
	cfg.dbHttpServiceRetryUrl = doc->HasMember("gameDataServiceSecondAddr") ? doc->FindMember("gameDataServiceSecondAddr")->value.GetString() : "";
	cfg.gameRankParams = doc->HasMember("gameRankParams") ? doc->FindMember("gameRankParams")->value.GetString() : "";
	cfg.isGameParty = doc->HasMember("isGameParty") ? doc->FindMember("isGameParty")->value.GetBool() : false;
	cfg.isPrivateParty = doc->HasMember("private") ? doc->FindMember("private")->value.GetBool() : false;
	cfg.maxWatch = doc->HasMember("maxWatch") ? doc->FindMember("maxWatch")->value.GetInt() : 0;
	cfg.watchMode = doc->HasMember("watchMode ") ? doc->FindMember("watchMode ")->value.GetInt() : 0;
    return cfg;
}

std::atomic_bool g_serverStopping{false};

#if LORD_PLATFORM == LORD_PLATFORM_LINUX
static void sighandler(int sig_no)
{
	if (sig_no == 10)
		Log::printLuaStack();
	LordLogInfo("Receive signal to stop server, sig_no is %d", sig_no);
	if (g_serverStopping) {
		LordLogInfo("Receive signal to stop server but server is stopping, do nothing, sig_no is %d", sig_no);
		return;
	}
	Server::Instance()->setAskedToQuit();
}
#endif

int main(int argc, char* argv[])
{
try {
		g_serverStopping = false;
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
		signal(SIGUSR1, sighandler);
#endif
		auto pserver = LordNew Server();
		RoomGameConfig rgConfig;
		if (argc == 1) {
			rgConfig = getTestRGConfig(argv[0]);
		}
		else {
			rgConfig = getRGConfigFromCmdline(argv[1]);
		}

#ifdef _DEBUG_WINDOWS
		Test t;
#endif

		pserver->init(rgConfig);
		pserver->start();
		pserver->waitForStopEvent();
		g_serverStopping = true;
		pserver->stopThread();
		pserver->unInit();

		LordDelete pserver;

#ifdef TRACK_EVENT_SUBSCRIPTION
		BLOCKMAN::EventManager::reportLeaks();
#endif
	}
	catch (const std::exception& e) {
		LuaDebug::enumStack(LuaEngine::getInstance()->getLuaState());
		LordLogError("main thread exception occur: %s", e.what());
		throw e;
	}
}
