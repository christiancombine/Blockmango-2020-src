--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.DBDataListener"
require "listener.RankListener"
require "listener.LogicListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    DBDataListener:init()
    RankListener:init()
    LogicListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setBreakBlockSoon(false)
    HostApi.setAttackCoefficientX(0.025)
    HostApi.setBlockAttr(102, 100)
    HostApi.setBlockAttr(20, 100)
    DBManager:addMustLoadSubKey(1)
end

ScriptMain:init()