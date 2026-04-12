require "config.GameConfig"
require "config.ShopConfig"
require "config.ManorConfig"
require "config.HardnessConfig"
require "config.SessionNpcConfig"
require "config.VehicleConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    GameServer:setInitPos(GameConfig.initPos)
    EngineWorld:stopWorldTime()
    ShopConfig:prepareShop()
    ManorConfig:prepareGate()
    HardnessConfig:prepareBlockHardness()
    VehicleConfig:prepareVehicle()
    SessionNpcConfig:prepareNpc()
    RanchersWeb:init()
    HostApi.startRedisDBService()
    RanchersRankManager:updateRankData()
    RanchersWeb:getClanProsperity(function (data)
        if data ~= nil then
            RanchersRankManager:setClanData(data)
        end
    end)
    PlayerManager:setMaxPlayer(100)
end

return GameListener
--endregion
