--region *.lua
require "config.MoneyConfig"
require "config.TeamConfig"
require "config.BlockConfig"
require "config.MerchantConfig"
require "config.EnchantMentNpcConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    ShopConfig:prepareShop()
    GameServer:setInitPos(TeamConfig:getTeam(1).initPos)
    EngineWorld:stopWorldTime()
    WalletUtils:addCoinMappings(MoneyConfig.coinMapping)
    RespawnConfig:prepareRespawnGoods()
    MoneyConfig:prepareNpc()
    TeamConfig:prepareEgg()
    MerchantConfig:prepareShopTruck()
    BlockConfig:prepareBlock()
    EnchantMentNpcConfig:prepareNpc()
end

return GameListener
--endregion
