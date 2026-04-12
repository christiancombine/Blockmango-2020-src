--region *.lua
require "config.GameConfig"
require "config.ShopConfig"
require "config.RankNpcConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    ShopConfig:prepareShop()
    GameServer:setInitPos(GameConfig.initPos)
    EngineWorld:stopWorldTime()
    DBManager:setGameType("g1032")
    HostApi.startRedisDBService()
    HostApi.setRedisDBHost(1)
    RankNpcConfig:setZExpireat()
    RankNpcConfig:updateRank()
    HostApi.setEntityItemLife(GameConfig.itemLifeTime * 20)
    PlayerManager:setMaxPlayer(100)
end

return GameListener
--endregion
