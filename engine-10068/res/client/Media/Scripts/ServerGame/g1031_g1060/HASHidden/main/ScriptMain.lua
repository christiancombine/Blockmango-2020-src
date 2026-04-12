--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.LogicListener"
require "listener.RankListener"
require "listener.CustomListener"
require "listener.DBDataListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    LogicListener:init()
    DBDataListener:init()
    RankListener:init()
    CustomListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    RewardManager:setGameType("g1037")
    HostApi.setGunFireIsNeedBullet(false)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
end

ScriptMain:init()