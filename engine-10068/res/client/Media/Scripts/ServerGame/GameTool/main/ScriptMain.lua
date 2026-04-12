--GameMain
package.path = package.path ..';..\\?.lua';

require "listener.GameListener"

ScriptMain = {}

function ScriptMain.init()
    HostApi.log("ScriptMain.init")
    GameListener:init()
    BaseMain:setGameType("g9999")
end

ScriptMain:init()