--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "events.GameEvents"
require "packet.GamePacketSender"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.DBDataListener"
require "listener.LogicListener"
require "events.GameEvents"
require "listener.ChestListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    DBDataListener:init()
    LogicListener:init()
    ChestListener:init()
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
    HostApi.setAllowHeadshot(true)
    HostApi.setSwordBreakBlock(true)
    ServerHelper.putBoolPrefs("IsUseCustomArmor", true)
end

ScriptMain:init()