GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.init = 0
GameMatch.Status.GameTime = 4

GameMatch.curStatus = GameMatch.Status.init

GameMatch.gameType = "g1051"
GameMatch.startWait = false
GameMatch.firstKill = false

GameMatch.allowMove = true

GameMatch.MaxRunTime = 86400

function GameMatch:startMatch()
    RewardManager:startGame()

    MsgSender.sendMsg(IMessages:msgGameStart())
    GameTimeTask:start()
    --HostApi.sendPlaySound(0, Define.mainBGM)
end

function GameMatch:sortPlayerRank()
    local players = PlayerManager:copyPlayers()

    table.sort(players, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end
        if a.gold ~= b.gold then
            return a.gold > b.gold
        end
        return a.userId > b.userId
    end)

    return players
end

function GameMatch:getPlayerRank(player)
    local players = self:sortPlayerRank();
    local rank = 0
    for i, v in pairs(players) do
        rank = rank + 1
        if v.rakssid == player.rakssid then
            return rank
        end
    end
    return rank
end

function GameMatch:onPlayerQuit(player)
    player:onQuit()
    BuffManager:removePlayerAllBuff(player.userId)
    player:addSettlementGold()
end

function GameMatch:setMaxGameRunTime()
    GameServer:setMaxRunTime(GameMatch.MaxRunTime)
    --print(GameServer.MaxRunTime)
end

return GameMatch

--endregion
