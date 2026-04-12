--region *.lua
require "messages.Messages"
require "config.GameConfig"
require "config.BlockConfig"
require "config.ChestConfig"
require "config.ToolConfig"
require "config.ShopConfig"
require "config.NpcConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    BlockConfig:prepareBlock()
    ToolConfig:prepareTool()
    GameMatch:initMatch()
    EngineWorld:stopWorldTime()
    ChestConfig:prepareChest()
    ShopConfig:prepareShop()
    NpcConfig:prepareRankNpc()
    WalletUtils:addCoinMappings(MerchantConfig.coinMapping)
    HostApi.startRedisDBService()
    GameMatch:setZExpireat()
    GameMatch:getRankTop10Players()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
