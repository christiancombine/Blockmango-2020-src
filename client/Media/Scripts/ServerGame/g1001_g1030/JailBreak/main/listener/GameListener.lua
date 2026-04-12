--region *.lua
require "messages.Messages"
require "config.GameConfig"
require "config.ShopConfig"
require "config.WeaponConfig"
require "config.ChestConfig"
require "config.MerchantConfig"
require "config.VehicleConfig"
require "config.StrongboxConfig"
require "config.RankNpcConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    WeaponConfig:prepareWeapon()
    ShopConfig:prepareShop()
    ChestConfig:prepareChest()
    BlockConfig:prepareAutoChangeBlock()
    VehicleConfig:prepareVehicle()
    DoorConfig:prepareDoor()
    StrongboxConfig:prepareStrongbox()
    WalletUtils:addCoinMappings(MerchantConfig.coinMapping)
    RankNpcConfig:prepareRankNpc()
    GameMatch:initMatch()
    EngineWorld:stopWorldTime()
    HostApi.startRedisDBService()
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
