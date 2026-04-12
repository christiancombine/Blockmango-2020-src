--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "config.GameConfig"
require "config.RankConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    DBManager:setGameType("g1048")
    HostApi.startRedisDBService()
    PlayerManager:setMaxPlayer(100)
    BlockSettingConfig:prepareBlockHardness()
    ToolDurableConfig:prepareToolDurable()
    RankConfig:setZExpireat()
    RankConfig:updateRank()
    SkyBlockWeb:init()
    GameParty:getAllPartyList()
end

return GameListener
--endregion