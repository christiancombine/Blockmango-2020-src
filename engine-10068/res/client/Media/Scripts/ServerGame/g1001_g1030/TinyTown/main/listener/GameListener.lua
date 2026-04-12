--region *.lua
require "config.GameConfig"
require "config.ManorNpcConfig"
require "config.ShopConfig"
require "config.FurnitureConfig"
require "config.VehicleConfig"
require "config.BlockConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    WalletUtils:addCoinMappings(MerchantConfig.coinMapping)
    ManorNpcConfig:prepareManorNpc()
    ManorNpcConfig:prepareSessionNpc()
    ManorNpcConfig:prepareRankNpc()
    ShopConfig:prepareShop()
    FurnitureConfig:prepareFurnitureShow()
    VehicleConfig:prepareVehicle()
    BlockConfig:prepareBlockHardness()
    HostApi.loadManorConfig()
    HostApi.loadManorCharmRank(30)
    HostApi.loadManorPotentialRank(30)
    PlayerManager:setMaxPlayer(100)
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
