--region *.lua

require "config.GameConfig"
require "config.ShopConfig"
require "config.MerchantConfig"
require "config.ToolConfig"
require "config.TaskConfig"
require "config.SessionNpcConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    ToolConfig:prepareTool()
    TaskConfig:prepareTask()
    ShopConfig:prepareShop()
    WalletUtils:addCoinMappings(MerchantConfig.coinMapping)
    GameConfig:prepareBlockHardness()
    GameServer:setInitPos(GameConfig.initPos)
    EngineWorld:stopWorldTime()
    SessionNpcConfig:prepareNpc()
    DBManager:setGameType("g1031")
end

return GameListener
--endregion
