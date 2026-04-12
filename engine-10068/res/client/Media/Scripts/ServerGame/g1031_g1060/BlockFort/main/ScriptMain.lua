--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.DBDataListener"
require "listener.CustomListener"
require "listener.ActivityListener"
require "events.GameEvents"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    DBDataListener:init()
    CustomListener:init()
    ActivityListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setCanDamageItem(false)
    HostApi.setNeedFoodStats(false)
    DBManager:disableAutoCache()
end

ScriptMain:init()