---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
GameMatch = {}

GameMatch.curTick = 0
GameMatch.world = nil

function GameMatch:initMatch()
    GameTimeTask:start()
    TreasureChallenge:setSeasonRefreshTimer()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onTick()
    end
end

function GameMatch:onPlayerQuit(player)
    GameSeason:onPlayerQuit(player)
    TreasureChallenge:onPlayerLogout(player)
end

return GameMatch