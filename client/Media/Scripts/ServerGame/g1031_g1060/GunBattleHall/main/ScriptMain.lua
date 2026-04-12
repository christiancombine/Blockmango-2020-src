--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.CustomListener"
require "listener.RankListener"
require "listener.DBDataListener"
require "listener.LogicListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    CustomListener:init()
    LogicListener:init()
    RankListener:init()
    DBDataListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setGunFireIsNeedBullet(false)
    HostApi.setNeedFoodStats(false)
    DBManager:addMustLoadSubKey(1)
end

ScriptMain:init()