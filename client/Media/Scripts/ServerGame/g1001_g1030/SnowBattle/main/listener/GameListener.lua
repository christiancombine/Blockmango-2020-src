--region *.lua
GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:prepareSnow()
    GameServer:setInitPos(TeamConfig:getTeam(1).initPos)
    EngineWorld:stopWorldTime()
end

return GameListener
--endregion
