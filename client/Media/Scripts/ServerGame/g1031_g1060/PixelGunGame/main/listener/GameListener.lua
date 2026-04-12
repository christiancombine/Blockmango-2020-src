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
    BaseMain:setGameType(GameMatch.gameType)
    RewardManager:setGameType("g1042")
    DBManager:setGameType(GameConfig.hallGameType)
    PlayerManager:setMaxPlayer(100)
end

return GameListener
--endregion
