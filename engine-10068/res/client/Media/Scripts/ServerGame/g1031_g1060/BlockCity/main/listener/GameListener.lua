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
    CannonConfig:prepareCannons()
    VehicleConfig:prepareVehicle()
    HardnessConfig:prepareBlockHardness()
    GameMatch:getLastWeekRank()
	CurrencyExConfig:prepareCurrencyExShop()
    PublicFacilitiesConfig:preparePublicChairs()
    PublicFacilitiesConfig:createActor(ActorType.Elevator)
    PublicFacilitiesConfig:createActor(ActorType.Portal)
end

return GameListener
--endregion
