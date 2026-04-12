--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.EntityListener"
require "listener.LogicListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    EntityListener:init()
    LogicListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    ServerHelper.putBoolPrefs("IsBlockmanCollision", false)
end

ScriptMain:init()