--Match.lua
GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.GameTime = 3
GameMatch.Status.GameOver = 4

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.gameType = "g1011"
GameMatch.startWait = false
GameMatch.hasStartGame = false
GameMatch.hasEndGame = false
GameMatch.curDeaths = 0

GameMatch.Teams = {}

function GameMatch:prepareMatch()
    MsgSender.sendMsg(IMessages:msgReadyHint(GameConfig.prepareTime))
    GamePrepareTask:start()
end

function GameMatch:startMatch()
    self.hasStartGame = true
    RewardManager:startGame()
    if (GameMatch:getLifeTeams() <= 1) then
        MsgSender.sendMsg(IMessages:msgGameOverByLittle())
        self:endMatch()
    else
        MsgSender.sendMsg(IMessages:msgGameStart())
        GameTimeTask:start()
        HostApi.sendPlaySound(0, 10006)
    end
end

function GameMatch:isGameStart()
    return self.hasStartGame
end

function GameMatch:getLifeTeams()
    local lifeTeam = 0
    for i, v in pairs(self.Teams) do
        if v:isDeath() == false then
            lifeTeam = lifeTeam + 1
        end
    end
    return lifeTeam
end

function GameMatch:endMatch()

    GameTimeTask:pureStop()

    local finalWin = true
    local winner = 0
    if (self:getLifeTeams() > 1) then
        finalWin = false
    end

    local winPlayerName;

    for i, v in pairs(self.Teams) do
        if finalWin and v:isDeath() == false then
            winPlayerName = v:getDisplayName()
            winner = v.id
        end
    end

    if finalWin and winPlayerName ~= nil then
        MsgSender.sendMsg(IMessages:msgWinnerInfo(winPlayerName))
    else
        MsgSender.sendMsg(IMessages:msgNoWinner(1))
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.team.id == winner then
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
        UserExpManager:addUserExp(player.userId, player.team.id == winner, 2)
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
    if player.team.id == winner then
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
    local isWin = self:getLifeTeams() == 1
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame(false, isWin)
    end
end

function GameMatch:doReport()
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        ReportManager:reportUserData(player.userId, player.kills, rank, 1)
    end
    GameServer:closeServer()
end

function GameMatch:ifGameOver()
    local lifeTeam = self:getLifeTeams()
    if lifeTeam <= 1 then
        GamePrepareTask:pureStop()
        GameTimeTask:pureStop()
        self:endMatch()
    end
end

-- player died or logout 
function GameMatch:onPlayerQuit(player)
    player:onLogout()
    player:onQuit()
    if self.hasStartGame == true and self.hasEndGame == false then
        self:ifGameOver()
    end
end

return GameMatch

--endregion
