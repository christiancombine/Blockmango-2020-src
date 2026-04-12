#include "ClientCmdMgr.h"

#include "Core.h"
#include "Memory/MemAllocDef.h"
#include "game.h"
#include "Log/Log.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/GameSettings.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Chunk/ChunkService.h"
#include "cWorld/WorldClient.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Plugins/GameClientEvents.h"
#include "GUI/RootGuiLayout.h"
#include "Util/SecTimer.h"
#include "Script/Event/LogicScriptEvents.h"

using namespace BLOCKMAN;
using namespace LORD;

String const ClientCmdMgr::CMD_STARTER = "/c";
String const ClientCmdMgr::CMD_TOGGLE_DEBUG = "toggle_debug";
String const ClientCmdMgr::CMD_SET_SMOOTH = "set_smooth";
String const ClientCmdMgr::CMD_GET_BLOCK = "get_block";
String const ClientCmdMgr::CMD_SAVE = "save";
String const ClientCmdMgr::CMD_CRASH = "crash";

ClientCmdMgr::ClientCmdMap ClientCmdMgr::m_cmdMap;

static bool setCameraScale(LORD::StringArray & argsArr)
{
	CameraMain* pCameraMain = (CameraMain*)SceneManager::Instance()->getMainCamera();
	NULL_RETURN(pCameraMain, false);
	/*pCameraMain->setScale(StringUtil::ParseFloat(argsArr[0]));*/
	return true;
}

static bool doWorkout(LORD::StringArray & argsArr)
{
	//String data = CommonDataBuilder().addIntParam("fitnessEquipId", StringUtil::ParseInt(argsArr[2])).getData();
	ClientNetwork::Instance()->getLuaCommonSender()->sendLuaCommonData("DoWorkout", "");
	return true;
}

static bool setHoldItemActor(LORD::StringArray & argsArr)
{
	auto pPlayer = Blockman::Instance()->m_pPlayer;
	pPlayer->m_holdItemActor = argsArr[0];
	pPlayer->m_holdItemChanged = true;
	return true;
}

static bool hideAllLayout(LORD::StringArray & argsArr)
{
	int id = SecTimer::createTimer([](int ticks)
	{
		RootGuiLayout::Instance()->hideAllChildren();
	});

	SecTimer::startTimerOnce(id);

	return true;
}

static bool onGamePause(LORD::StringArray & argsArr)
{
	GameClient::CGame::Instance()->OnPause();
	return true;
}

static bool onGameResume(LORD::StringArray & argsArr)
{
	GameClient::CGame::Instance()->OnResume();
	return true;
}

static bool onLuaHotUpdate(LORD::StringArray & argsArr)
{
	return SCRIPT_EVENT::LuaHotUpdateEvent::invoke(true);
}

static bool onGameActionTrigger(LORD::StringArray & argsArr)
{
	GameClient::CGame::Instance()->onGameActionTrigger(StringUtil::ParseInt(argsArr[0]));
	return true;
}

void ClientCmdMgr::init() {
	m_cmdMap[CMD_TOGGLE_DEBUG] = { 1, &ClientCmdMgr::toggleDebug };
	m_cmdMap[CMD_SET_SMOOTH] = { 1, &ClientCmdMgr::setSmooth };
	m_cmdMap[CMD_GET_BLOCK] = { 3, &ClientCmdMgr::getBlock };
	m_cmdMap[CMD_SAVE] = { 0, &ClientCmdMgr::save };
	m_cmdMap[CMD_CRASH] = { 0, &ClientCmdMgr::crash };
	m_cmdMap["setCameraScale"] = { 1, &setCameraScale };
	m_cmdMap["doWorkout"] = { 0, &doWorkout };
	m_cmdMap["setHoldItemActor"] = { 1, &setHoldItemActor };
	m_cmdMap["hide_all_layout"] = { 0, &hideAllLayout };
	m_cmdMap["GamePause"] = { 0, &onGamePause };
	m_cmdMap["GameResume"] = { 0, &onGameResume };
	m_cmdMap["LuaHotUpdate"] = { 0, &onLuaHotUpdate };
	m_cmdMap["GameActionTrigger"] = { 1, &onGameActionTrigger };
}

bool ClientCmdMgr::consumeCmd(const String& msg)
{
	if (!StringUtil::StartWith(msg, CMD_STARTER, false)) {
		return false;
	}

	StringArray arr = StringUtil::Split(msg, " ");
	if (arr.size() < 2) {
		return false;
	}

	if (arr[0] != CMD_STARTER) {
		return false;
	}

	String cmd = "";
	for (int i = 1;i < arr.size();++i)
	{
		cmd += arr[i] + "&";
	}
	cmd = cmd.substr(0, cmd.length() - 1);

	if (!SCRIPT_EVENT::ClientCmdEvent::invoke(cmd))
	{
		return true;
	}

	String cmdName = arr[1];
	if (m_cmdMap.find(cmdName) == m_cmdMap.end()) {
		return false;
	}

	if (arr.size() != m_cmdMap[cmdName].argsNum + 2) {
		return false;
	}

	auto it = arr.begin();
	++it;
	++it;
	StringArray argsArr(it, arr.end());
	return m_cmdMap[cmdName].func(argsArr);
}

void BLOCKMAN::ClientCmdMgr::onKeyUp(int key)
{
	if (key == 'i' || key == 'I')
	{
		static bool isHide = false;
		if (!isHide)
		{
			RootGuiLayout::Instance()->hideAllChildren();
			isHide = true;
		}
		else
		{
			RootGuiLayout::Instance()->showMainControl();
			RootGuiLayout::Instance()->recoverAllChildren();
			isHide = false;
		}
	}
}

void BLOCKMAN::ClientCmdMgr::onKeyDown(int key)
{
}

bool ClientCmdMgr::toggleDebug(LORD::StringArray& argsArr)
{
	bool on = false;
	if (argsArr[0] == "on") {
		on = true;
	}
	GameClient::CGame::Instance()->toggleDebugMessageShown(on);

	return true;
}

bool ClientCmdMgr::setSmooth(LORD::StringArray & argsArr)
{
	GameSettings::setMouseHistoryDuration((std::max)(0, StringUtil::ParseI32(argsArr[0], 100)));
	return true;
}

bool ClientCmdMgr::getBlock(LORD::StringArray & argsArr)
{
	int x = StringUtil::ParseInt(argsArr[0], 0);
	int y = StringUtil::ParseInt(argsArr[1], 0);
	int z = StringUtil::ParseInt(argsArr[2], 0);

	BlockPos pos(x, y, z);
	String result = "client block info: ";

	int blockID = Blockman::Instance()->getWorld()->getBlockId(pos);
	int blockMeta = Blockman::Instance()->getWorld()->getBlockMeta(pos);
	String name;
	if (blockID == 0) {
		name = "air";
	}
	else {
		name = BlockManager::sBlocks[blockID]->getUnlocalizedName();
	}

	result += StringUtil::Format(" (%d,%d,%d)id %d, meta %d, name %s", pos.x, pos.y, pos.z, blockID, blockMeta, name.c_str());

	blockID = Blockman::Instance()->getWorld()->getBlockId(pos.getPosY());
	blockMeta = Blockman::Instance()->getWorld()->getBlockMeta(pos.getPosY());
	if (blockID == 0) {
		name = "air";
	}
	else {
		name = BlockManager::sBlocks[blockID]->getUnlocalizedName();
	}
	result += StringUtil::Format("---UpperBlock id %d, meta %d, name %s", blockID, blockMeta, name.c_str());


	Blockman::Instance()->setClientDebugString(result);
	return true;
}

bool ClientCmdMgr::save(LORD::StringArray & argsArr)
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	Blockman::Instance()->getWorld()->getChunkService()->saveAllChunks(true);
	LordLogInfo("++++++++++++++++++++++++++ save success ++++++++++++++++++++++++++++");
	return true;
#else
	return false;
#endif
}

bool ClientCmdMgr::crash(LORD::StringArray & argsArr)
{
	char* p = nullptr;
	p[0] = 'c';
	return true;
}