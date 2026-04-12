--region *.lua
require "messages.Messages"
require "config.GameConfig"
require "config.AirplaneConfig"
require "config.ChestConfig"
require "config.WeaponConfig"

GameListener = {}

function GameListener:init()
    BaseListener.registerCallBack(GameInitEvent, GameListener.onGameInit)
end

function GameListener.onGameInit(config)
    GameConfig:init(config)
    GameMatch:initMatch()
    WeaponConfig:prepareWeapon()
    EngineWorld:stopWorldTime()
    HostApi.setMedicineHealAmount(PotionItemID.POTION_MEDICINE_PACK, GameConfig.medicinePack)
    HostApi.setMedicineHealAmount(PotionItemID.POTION_MEDICINE_POTION, GameConfig.medicinePotion)
    GameServer:setInitPos(GameConfig.initPos)
end

return GameListener
--endregion
