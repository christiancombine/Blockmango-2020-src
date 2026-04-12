--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.DBDataListener"
require "listener.LogicListener"
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
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setCanDamageItem(true)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
end

ScriptMain:init()