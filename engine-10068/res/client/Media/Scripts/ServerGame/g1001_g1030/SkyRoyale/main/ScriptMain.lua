--GameMain
package.path = package.path ..';..\\?.lua';

require "Match"
require "task.GameTimeTask"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.LogicListener"
require "listener.ChestListener"
require "listener.DBDataListener"
require "listener.CustomListener"

ScriptMain = {}

function ScriptMain.init()
    GameListener:init()
    PlayerListener:init()
    BlockListener:init()
    LogicListener:init()
    ChestListener:init()
    DBDataListener:init()
    CustomListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setNeedFoodStats(false)
    HostApi.setHideClouds(true)
    HostApi.setSneakShowName(true)
    HostApi.setFoodHeal(false)
    HostApi.setCanCloseChest(false)
    DBManager:addMustLoadSubKey(1)
end

ScriptMain:init()