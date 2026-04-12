--GameMain
package.path = package.path ..';..\\?.lua'

require "Match"
require "task.GameTimeTask"
require "config.GameConfig"
require "listener.GameListener"
require "listener.PlayerListener"
require "listener.BlockListener"
require "listener.ChestListener"
require "listener.LogicListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    PlayerListener:init()
    ChestListener:init()
    BlockListener:init()
    LogicListener:init()
    BaseMain:setGameType(GameMatch.gameType)
    HostApi.setAttackCoefficientX(0.03)
    HostApi.setShowGunEffectWithSingle(true)
    HostApi.setSceneSwitch(1, 0, true, false, false)
end

ScriptMain:init()