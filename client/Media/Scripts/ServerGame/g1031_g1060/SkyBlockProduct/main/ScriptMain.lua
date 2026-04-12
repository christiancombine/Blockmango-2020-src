--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.DBDataListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.LogicListener"
require "listener.CustomListener"
require "packet.GamePacketSender"
require "events.GameEvents"
ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    DBDataListener:init()
    BlockListener:init()
    LogicListener:init()
    CustomListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
    HostApi.setGunFireIsNeedBullet(false)
    HostApi.setBreakBlockSoon(false)
    HostApi.setHideArmor(true)
    HostApi.setBlockAttr(20, 100)
    DBManager:disableAutoCache()
    ServerHelper.putBoolPrefs("SendSystemsChat", false)
    ServerHelper.putBoolPrefs("IsUseCustomArmor", true)
    PlayerManager.IsMessages = false
end

ScriptMain:init()