--region *.lua
require "config.GameConfig"
require "config.ShopConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    ShopConfig:prepareShop()
    GameMatch:initMatch()
    EngineWorld:stopWorldTime()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
