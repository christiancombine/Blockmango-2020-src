--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BirdSimulatorListener"
require "listener.BlockListener"
require "listener.CustomListener"
require "listener.DBDataListener"
require "listener.CreatureListener"
require "listener.RankListener"
ScriptMain = {}

function ScriptMain.init()
    print("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BirdSimulatorListener:init()
    BlockListener:init()
    CustomListener:init()
    DBDataListener:init()
    CreatureListener:init()
    RankListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
    HostApi.setThirdPersonDistance(5)
    ServerHelper.putBoolPrefs("IsCreatureCollision", false)
end

ScriptMain:init()