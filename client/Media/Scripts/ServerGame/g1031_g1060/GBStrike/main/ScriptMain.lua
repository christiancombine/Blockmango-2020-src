--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.LogicListener"
require "listener.DBDataListener"
require "listener.RankListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    DBDataListener:init()
    LogicListener:init()
    RankListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    RewardManager:setGameType("g1032")
    HostApi.setAllowHeadshot(true)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
    DBManager:addMustLoadSubKey(1)
end

ScriptMain:init()