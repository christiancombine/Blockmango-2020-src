--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "config.Define"
require "events.GameEvents"
require "packet.GamePacketSender"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.DBDataListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.CustomListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    DBDataListener:init()
    BlockListener:init()
    CustomListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
end

ScriptMain:init()