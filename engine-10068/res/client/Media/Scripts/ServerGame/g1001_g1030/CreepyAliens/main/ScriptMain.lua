--GameMain
package.path = package.path .. ';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.DBDataListener"
require "listener.RankListener"
require "listener.CreatureListener"
require "listener.LogicListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    DBDataListener:init()
    RankListener:init()
    CreatureListener:init()
    LogicListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setNeedFoodStats(false);
    HostApi.setAttackCoefficientX(0.0)
    HostApi.setGunFireIsNeedBullet(false)
    ServerHelper.putBoolPrefs("IsCreatureCollision", false)
    DBManager:addMustLoadSubKey(1)
end

ScriptMain:init()