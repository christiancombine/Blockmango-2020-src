--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "config.GameConfig"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.ChestListener"
require "listener.BlockListener"

require "task.WaitingPlayerTask"
require "task.GamePrepareTask"
require "task.DisableMoveTask"
require "task.GameTimeTask"
require "task.FinalFightStartTask"
require "task.FinalFightTimeTask"
require "task.GameOverTask"
require "listener.CustomListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    ChestListener:init()
    BlockListener:init()
    CustomListener:init()
    BaseMain:setGameType(GameMatch.gameType)
end

ScriptMain:init()