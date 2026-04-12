--region *.lua

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    EngineWorld:stopWorldTime()
    GameTimeTask:start()
    SupplyManager:initRefreshSupply()
    GameMatch:setMaxGameRunTime()
end

return GameListener
--endregion
