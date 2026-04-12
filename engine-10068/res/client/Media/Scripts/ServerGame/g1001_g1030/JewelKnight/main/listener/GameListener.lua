require "config.GameConfig"
require "config.ShopConfig"
require "config.ToolConfig"
require "config.SessionNpcConfig"
require "config.RankNpcConfig"
require "config.MerchantConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    EngineWorld:stopWorldTime()
    ShopConfig:prepareShop()
    BlockConfig:prepareBlock()
    BlockConfig:prepareMine()
    ToolConfig:prepareTool()
    SessionNpcConfig:prepareNpc()
    MerchantConfig:prepareMerchant()
    MerchantConfig:prepareUpgradeMerchants()
    HostApi.startRedisDBService()
    RankNpcConfig:setZExpireat()
    RankNpcConfig:updateRank()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
