--GameMain
package.path = package.path ..';..\\?.lua'

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.LogicListener"
require "listener.RankListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    LogicListener:init()
    RankListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setCanDamageItem(false)
    HostApi.setNeedFoodStats(false)
    ReportManager:setRankType(ReportManager.RankType.Max)
end

ScriptMain:init()