--region *.lua
require "config.GameConfig"
require "config.RankNpcConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    EngineWorld:stopWorldTime()
    HostApi.startRedisDBService()
    RankNpcConfig:setZExpireat()
    RankNpcConfig:updateRank()
    GameServer:setInitPos(GameConfig.initPos)
    PlayerIdentityCache:disableIdentityEffect()
end

return GameListener
--endregion
