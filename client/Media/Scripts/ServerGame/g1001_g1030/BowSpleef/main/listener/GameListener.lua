--region *.lua

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameServer:setInitPos(GameConfig.initPos[1])
    EngineWorld:stopWorldTime()
end

return GameListener
--endregion
