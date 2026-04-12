--Match.lua
require "util.DbUtil"
require "util.BackHallManager"
require "manager.MonsterManager"
require "util.RechargeUtil"

GameMatch = {}

GameMatch.gameType = "g1049"
GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:addPlayerScore()
    BackHallManager:onTick()
    MonsterManager:onTick()
    ChestConfig:onTick()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onTick()
    end
end

function GameMatch:onPlayerQuit(player)
    player:reward()
	GameAnalytics:design(player.userId, 0, { "g1049StayPvpTime", player.pvpTime} )
	GameAnalytics:design(player.userId, 0, { "g1049StayPveTime", player.stay_time - player.pvpTime} )
end

function GameMatch:addPlayerScore()
    if self.curTick == 0 or self.curTick % 60 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:addScore(10)
    end
end

function GameMatch:checkIfChristmasFinish()
    local activityTime = os.time() - GameConfig.christmasFinishTime
    if activityTime < 0 then
        return true
    else
        return false
    end
end

return GameMatch