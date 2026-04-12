--region *.lua
require "config.GameConfig"
require "messages.Messages"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    EngineWorld:stopWorldTime()
    WalletUtils:addCoinMappings(GameConfig.coinMapping)
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
