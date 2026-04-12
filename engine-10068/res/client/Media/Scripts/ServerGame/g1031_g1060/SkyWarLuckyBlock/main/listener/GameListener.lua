--region *.lua
require "config.GameConfig"
require "config.BlockHardnessConfig"

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
    RewardManager:setGameType(GameMatch.gameType)
    DBManager:setGameType(GameMatch.gameType)
    PlayerManager:setMaxPlayer(100)
    BlockHardnessConfig:prepareBlock()
    RespawnConfig:prepareRespawnGoods()
    HostApi.setSneakShowName(true)
end

return GameListener
--endregion
