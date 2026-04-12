--GameMain
package.path = package.path ..';..\\?.lua'

require "Match"
require "task.GameTimeTask"
require "config.Define"
require "config.GameConfig"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.ChestListener"
require "listener.RankListener"
require "listener.LogicListener"
require "listener.CustomListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    ChestListener:init()
    BlockListener:init()
    RankListener:init()
    LogicListener:init()
    CustomListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setAttackCoefficientX(0.03)
    DBManager:addMustLoadSubKey(1, 2)
end

ScriptMain:init()