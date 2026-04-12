--region *.lua
require "config.GameConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    GameServer:setInitPos(GameConfig.initPos)
    EngineWorld:stopWorldTime()
end

return GameListener
--endregion
