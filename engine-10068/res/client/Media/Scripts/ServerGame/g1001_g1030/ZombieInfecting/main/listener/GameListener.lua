--region *.lua
require "config.GameConfig"
require "config.ShopConfig"
require "config.InventoryConfig"
require "config.WeaponConfig"
require "messages.Messages"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    ShopConfig:prepareShop()
    WeaponConfig:prepareWeapon()
    GameMatch:initMatch()
    GameServer:setInitPos(GameConfig.initPos)
    EngineWorld:stopWorldTime()
    InventoryUtil:prepareChest(GameMatch:getCurGameScene().inventoryConfig.pos)
end

return GameListener
--endregion
