--region *.lua

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameServer:setInitPos(GameConfig.initPos[1])
    EngineWorld:stopWorldTime()
    InventoryUtil:prepareChest(GameConfig.inventory[1].pos)
    RespawnConfig:prepareRespawnGoods()
end

return GameListener
--endregion
