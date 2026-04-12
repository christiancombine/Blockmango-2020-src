--region *.lua
require "config.GameConfig"
require "config.RankNpcConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    RespawnConfig:prepareRespawnGoods()
    ShopConfig:prepareShop()
    GameMatch:initMatch()
    EngineWorld:stopWorldTime()
    HostApi.startRedisDBService()
    RankNpcConfig:setZExpireat()
    RankNpcConfig:updateRank()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
