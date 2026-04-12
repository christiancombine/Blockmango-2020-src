#ifndef __GAME_CMD_MGR__
#define __GAME_CMD_MGR__
#include <map>
#include "Core.h"
#include "Object/Singleton.h"
#include "Util/StringUtil.h"
#include "Memory/MemAllocDef.h"

class ClientPeer;

namespace BLOCKMAN
{
	class GameCmdMgr  {
	public:
		static LORD::String CMD_STARTER;
		static LORD::String CMD_TELEPORT;
		static LORD::String CMD_ADD_ITEM;
		static LORD::String CMD_ENCHANT;
		static LORD::String CMD_SET_WORLD_TIME;
		static LORD::String CMD_SET_WATCH_MODE;
		static LORD::String CMD_SET_ALLOW_FLYING;
		static LORD::String CMD_SET_MODEL;
		static LORD::String CMD_GET_BLOCK;
		static LORD::String CMD_ADD_ENTITY;
		static LORD::String CMD_SET_BLOCK;
		static LORD::String CMD_START_PLANE;
		static LORD::String CMD_CREATE_HOUSE;
		static LORD::String CMD_FILL_BLOCK;
		static LORD::String CMD_SHUTDOWN_SERVER;
		static LORD::String CMD_CREATE_MAP;
		static LORD::String CMD_ADD_EXP;
		static LORD::String CMD_ADD_RUNE;
		static LORD::String CMD_ADD_MONSTERG1049;
		static LORD::String CMD_ADD_KILL_SELF;
		static LORD::String CMD_CHANGE_WEATHER;
		struct CmdCfg {
			int argsNum;
			std::function<bool(std::shared_ptr<ClientPeer>&, LORD::StringArray&)> func;
		};
		typedef std::map<LORD::String, CmdCfg> GameCmdMap;

	public:
		static void init();
		static bool consumeCmd(std::shared_ptr<ClientPeer>&  clientPeer, const LORD::String & msg);
		static void setCmdUsable(bool canUse) { m_useCmd = canUse; }

	private:
		static bool teleport(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool getBlock(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool addItem(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool enchant(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool setWorldTime(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool setWatchMode(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool setAllowFlying(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool setModel(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool addEntity(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool setBlock(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool startPlane(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool createHouse(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool fillBlock(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool ShutdownServer(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool createMap(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool addExp(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool addRune(std::shared_ptr<ClientPeer>&  clientPeer, LORD::StringArray& argsArr);
		static bool addMonster1049(std::shared_ptr<ClientPeer> & clientPeer, LORD::StringArray &argsArr);
		static bool killSelf(std::shared_ptr<ClientPeer> & clientPeer, LORD::StringArray &argsArr);
		static bool changeWeather(std::shared_ptr<ClientPeer> & clientPeer, LORD::StringArray &argsArr);
	private:
		static GameCmdMap m_cmdMap;
		static bool m_useCmd;
	};
}
#endif // __GAME_CMD_MGR__
