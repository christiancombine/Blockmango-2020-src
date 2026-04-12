--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "config.Define"
require "events.GameEvents"
require "packet.GamePacketSender"
require "task.GameTimeTask"
require "task.WaitUpgradeQueue"
require "listener.GameListener"
require "listener.DBDataListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.CustomListener"
require "listener.GunStoreListener"
require "listener.TaskListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    DBDataListener:init()
    BlockListener:init()
    CustomListener:init()
    GunStoreListener:init()
    TaskListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
    HostApi.setRenderYawThresholdAndScale(1.0, 1.0)
end

ScriptMain:init()