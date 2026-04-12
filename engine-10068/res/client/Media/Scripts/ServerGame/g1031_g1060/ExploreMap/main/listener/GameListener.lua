--region *.lua

require "config.GameConfig"
require "config.RankNpcConfig"
require "config.ShopConfig"
require "config.MerchantConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    ShopConfig:prepareShop()
    WalletUtils:addCoinMappings(MerchantConfig.coinMapping)
    GameConfig:prepareBlockHardness()
    EngineWorld:stopWorldTime()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
