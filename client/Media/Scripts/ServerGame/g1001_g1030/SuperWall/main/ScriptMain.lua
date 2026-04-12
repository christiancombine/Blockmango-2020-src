--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.CustomListener"
require "listener.LogicListener"
require "listener.RankListener"
require "listener.CreatureListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    CustomListener:init()
    LogicListener:init()
    RankListener:init()
    CreatureListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setBreakBlockSoon(false)
    HostApi.setCanDamageItem(false)
    HostApi.setNeedFoodStats(false);
    HostApi.setAttackCoefficientX(0.0)
end

ScriptMain:init()