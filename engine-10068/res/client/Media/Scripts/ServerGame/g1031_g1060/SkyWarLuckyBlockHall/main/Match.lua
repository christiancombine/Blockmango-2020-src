---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "util.DbUtil"

GameMatch = {}

GameMatch.gameType = "g1058"
GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onTick()
    end
end

function GameMatch:onPlayerQuit(player)

end

return GameMatch