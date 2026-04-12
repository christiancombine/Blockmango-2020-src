--region *.lua
require "data.GameRank"
local GameRank = require "data.GameRank"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    HostApi.startRedisDBService()
    HostApi.setMaxInventorySize(9)
    HostApi.setBreakBlockSoon(false)

    GameConfig:init(config)
    ShopConfig:prepareShop()
    GameRank:init()
    GameMatch:init()
    MerchantConfig:prepareMerchant()
    EngineWorld:stopWorldTime()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
