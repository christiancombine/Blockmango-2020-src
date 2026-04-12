--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.DBDataListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.LogicListener"
require "listener.CreatureListener"
require "listener.CustomListener"
require "packet.GamePacketSender"
require "events.GameEvents"
require "listener.ChestListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    HostApi.setLavaDamageSource()
    GameListener:init()
    PlayerListener:init()
    DBDataListener:init()
    BlockListener:init()
    LogicListener:init()
    CreatureListener:init()
    CustomListener:init()
    ChestListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
    HostApi.setGunFireIsNeedBullet(false)
    HostApi.setBreakBlockSoon(false)
    HostApi.setHideArmor(true)
    ServerHelper.putBoolPrefs("SendSystemsChat", false)
    ServerHelper.putBoolPrefs("IsUseCustomArmor", true)
    PlayerManager.IsMessages = false
    HostApi.setSwordBreakBlock(true)
    -- DBManager:disableAutoCache()
end

ScriptMain:init()