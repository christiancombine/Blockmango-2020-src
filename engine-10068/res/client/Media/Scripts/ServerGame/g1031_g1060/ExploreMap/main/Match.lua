---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---
require "messages.Messages"

GameMatch = {}

GameMatch.gameType = "g1040"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.hasStartGame = false

GameMatch.gameWaitTick = 0
GameMatch.gameReadyToStarTick = 0

GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
    -- self:resetGame()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    if self.curStatus == self.Status.WaitingPlayer then
        
    end
end


function GameMatch:onPlayerQuit(player)
   
end

function GameMatch:getDBDataRequire(player)

end

return GameMatch