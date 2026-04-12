GameMatch = {}

GameMatch.gameType = "g1052"

GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:addPlayerScore()
    self:kickOutErrorPlayer()
    SceneManager:onTick(ticks)
end

function GameMatch:onPlayerQuit(player)
    player:reward()
    player:onQuit()
    DBManager:removeCache(player.userId, {DbUtil.TAG_PLAYER})
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

function GameMatch:kickOutErrorPlayer()
    for _, errorTarget in pairs(SceneManager.errorTarget) do
        local callOnTargets = SceneManager:getCallOnTargets()
        for playerId, target in pairs(callOnTargets) do
            if tostring(target.userId) == tostring(errorTarget.userId) then
                local player = PlayerManager:getPlayerByUserId(playerId)
                if player ~= nil then
                    print("player[", tostring(playerId),"] has been tick out")
                    HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
                end
            end
        end
    end
end

return GameMatch