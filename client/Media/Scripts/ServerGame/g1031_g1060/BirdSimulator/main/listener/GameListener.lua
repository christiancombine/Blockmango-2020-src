require "config.GameConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)

    GameConfig:init(config)
    GameMatch:initMatch()
    EngineWorld:stopWorldTime()
    PlayerManager:setMaxPlayer(100)

    MonsterConfig:prepareMonster()
    ActorNpcConfig:prepareNpc()
    FieldConfig:prepareDoorActor()
    CannonConfig:prepareCannons()

    HostApi.startRedisDBService()
    RankConfig:setZExpireat()
    RankConfig:updateRank()
    GameServer:setInitPos(GameConfig.initPos)

end

return GameListener
--endregion
