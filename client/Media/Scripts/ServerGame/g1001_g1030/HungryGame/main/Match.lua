--Match.lua
require "util.VideoAdHelper"

GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.DisableMove = 2
GameMatch.Status.Prepare = 3
GameMatch.Status.GameTime = 4
GameMatch.Status.FinaFightStart = 5
GameMatch.Status.FinaFightTime = 6
GameMatch.Status.GameOver = 7

GameMatch.curStatus = GameMatch.Status.init

GameMatch.gameType = "g1001"
GameMatch.startWait = false
GameMatch.hasStartGame = false
GameMatch.hasEndGame = false
GameMatch.firstKill = false

GameMatch.allowMove = false
GameMatch.allowPvp = false

function GameMatch:prepareMatch()
    self.allowMove = true

    MsgSender.sendMsg(IMessages:msgReadyHint(GameConfig.prepareTime))
    GamePrepareTask:start()
end

function GameMatch:startMatch()
    self.allowPvp = true
    self.hasStartGame = true
    RewardManager:startGame()

    if PlayerManager:getPlayerCount() <= 1 then
        MsgSender.sendMsg(IMessages:msgGameOverByLittle())
        self:endMatch()
    else
        MsgSender.sendMsg(IMessages:msgGameStart())
        GameTimeTask:start()
        HostApi.sendPlaySound(0, 10017)
    end
end

function GameMatch:startFinalFight()
    self:setFinalPos()
    self.allowPvp = false
    FinalFightStartTask:start()
end

function GameMatch:endMatch()

    GameTimeTask:pureStop()

    local finalWin = true
    local winner = 0
    if (self:getLifePlayer() > 1) then
        finalWin = false
    end

    local winPlayerName;

    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if finalWin and v.isLife then
            winPlayerName = v.name
            winner = v.rakssid
        end
    end

    if finalWin and winPlayerName ~= nil then
        MsgSender.sendMsg(IMessages:msgWinnerInfo(winPlayerName))
    else
        MsgSender.sendMsg(IMessages:msgNoWinner(0))
    end

    if finalWin then
        for i, v in pairs(players) do
            v:onWin()
        end
    end

    self:doReward(winner)

    for i, v in pairs(players) do
        if v.isLife then
            v:onGameEnd(finalWin)
        end
    end
    self.hasEndGame = true
    GameOverTask:start()
end

function GameMatch:doReward(winner)
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, winner == player.rakssid)
    end
    RewardManager:getQueueReward(function(data, winner)
        self:sendGameSettlement(winner)
    end, winner)
end

function GameMatch:sendGameSettlement(winner)
    local Players = self:sortPlayerRank()

    local players = {}

    for rank, player in pairs(Players) do
        players[rank] = self:newSettlementItem(player, rank, winner)
    end

    for rank, player in pairs(Players) do
        local result = {}
        result.players = players
        result.own = self:newSettlementItem(player, rank, winner)
        HostApi.sendGameSettlement(player.rakssid, json.encode(result), true)
    end
end

function GameMatch:newSettlementItem(player, rank, winner)
    local item = {}
    item.name = player.name
    item.rank = rank
    item.points = player.scorePoints
    item.gold = player.gold
    item.available = player.available
    item.hasGet = player.hasGet
    item.vip = player.vip
    item.kills = player.kills
    item.adSwitch = player.adSwitch or 0
    if item.gold <= 0 then --or player.isSingleReward then
        item.adSwitch = 0
    end
    if player.rakssid == winner then
        item.isWin = 1
    else
        item.isWin = 0
    end
    return item
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
    return RankManager:getPlayerRank(players, player)
end

function GameMatch:cleanPlayer()
    self:doReport()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame()
    end
end

function GameMatch:doReport()
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        ReportManager:reportUserData(player.userId, player.kills, rank, 1)
    end
    GameServer:closeServer()
end

function GameMatch:setFinalPos()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:teleffPos()
    end
end

function GameMatch:getLifePlayer()
    local life = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isLife then
            life = life + 1
        end
    end
    return life
end

function GameMatch:getRealLifePlayer()
    local life = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.realLife then
            life = life + 1
        end
    end
    return life
end

function GameMatch:ifGameOver()
    if self:getLifePlayer() <= 1 then
        GamePrepareTask:pureStop()
        GameTimeTask:pureStop()
        FinalFightStartTask:pureStop()
        FinalFightTimeTask:pureStop()
        self:endMatch()
    end
end

function GameMatch:onPlayerDie(player)
    if self:getRealLifePlayer() <= 2 then
        player.realLife = false
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            if v.realLife == false then
                v:onDie()
            end
        end
        GameMatch:ifGameOver()
        return
    end
    VideoAdHelper:tryShowRespawnVideoAd(player)
    player:showBuyRespawn()
end

function GameMatch:onPlayerQuit(player)
    player:onLogout()
    player:onQuit()
    if self.hasStartGame == true and self.hasEndGame == false then
        self:ifGameOver()
    end
end

return GameMatch

--endregion
