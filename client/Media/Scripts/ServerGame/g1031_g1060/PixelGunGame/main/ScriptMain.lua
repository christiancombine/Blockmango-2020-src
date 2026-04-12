--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "config.Define"
require "events.GameEvents"
require "packet.GamePacketSender"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.DBDataListener"
require "listener.LogicListener"
require "events.GameEvents"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    DBDataListener:init()
    LogicListener:init()
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
    HostApi.setAllowHeadshot(true)
    HostApi.setRenderYawThresholdAndScale(1.0, 1.0)
    ServerHelper.putIntPrefs("HurtProtectTime", 0)
end

ScriptMain:init()