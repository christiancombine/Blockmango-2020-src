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
    ShopConfig:prepareShop()
    WalletUtils:addCoinMappings(GameConfig.coinMapping)
    GameMatch:initMatch()
    if GameConfig.isAllDay then
        EngineWorld:stopWorldTime()
    end
    HostApi.startRedisDBService()
    RankNpcConfig:setZExpireat()
    RankNpcConfig:updateRank()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
