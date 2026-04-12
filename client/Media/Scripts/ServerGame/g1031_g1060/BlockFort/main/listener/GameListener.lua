require "config.GameConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    GameServer:setInitPos(GameConfig.initPos)
    PlayerManager:setMaxPlayer(100)
    HardnessConfig:prepareBlockHardness()
    SessionNpcConfig:prepareNpc()
end

return GameListener
