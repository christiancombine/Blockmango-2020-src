--region *.lua
require "config.GameConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    EngineWorld:stopWorldTime()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
