--GameMain
package.path = package.path .. ';..\\?.lua';

require "Match"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.CustomListener"
require "listener.LogicListener"
require "listener.RankListener"
require "listener.DBDataListener"
require "task.GameTimeTask"
require "task.GameOverTask"
require "task.GamePrepareTask"
require "task.GameRoleTask"
require "task.WaitingPlayerTask"
require "config.GameConfig"
require "packet.GamePacketSender"

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
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
    HostApi.setShowGunEffectWithSingle(true)
    DBManager:addMustLoadSubKey(1)
end

ScriptMain:init()