--region *.lua

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    EngineWorld:stopWorldTime()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
