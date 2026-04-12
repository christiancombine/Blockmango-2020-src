---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---
require "messages.Messages"
require "util.DbUtil"

GameMatch = {}

GameMatch.gameType = "g1037"

GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
    LotteryUtil:setRandomSeed()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:onTick()
    end

    self:ifUpdateRank()
end

function GameMatch:onPlayerQuit(player)

end

function GameMatch:ifUpdateRank()
    if self.curTick % 300 == 0 and PlayerManager:isPlayerExists() then
        RankNpcConfig:updateRank()
    end
end

return GameMatch