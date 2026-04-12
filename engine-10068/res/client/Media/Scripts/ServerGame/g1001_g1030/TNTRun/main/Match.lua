--Match.lua
require "messages.Messages"

GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.GameTime = 3
GameMatch.Status.GameOver = 4

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.gameType = "g1010"
GameMatch.startWait = false
GameMatch.hasStartGame = false
GameMatch.hasEndGame = false

GameMatch.runningSecond = 0
GameMatch.hasDestory = false

function GameMatch:prepareMatch()
    MsgSender.sendMsg(Messages:gamePrepareHint(GameConfig.prepareTime))
    GamePrepareTask:start()
end

function GameMatch:startMatch()
    self.hasStartGame = true
    RewardManager:startGame()
    if PlayerManager:isPlayerEmpty() then
        MsgSender.sendMsg(IMessages:msgGameOverByLittle())
        self:endMatch()
    else
        MsgSender.sendMsg(Messages:gameStartHint())
        GameTimeTask:start()
        self:assignationRole()
    end
end

function GameMatch:assignationRole()
    local randomTimes = 0
    while self.hasDestory == false and randomTimes < 10 do
        local player = self:getRandomPlayer()
        if player ~= nil and player.role == GamePlayer.ROLE_PENDING then
            player:becomeRole(GamePlayer.ROLE_DESTROYER)
            self.hasDestory = true
        end
        randomTimes = randomTimes + 1
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_PENDING then
            v:becomeRole(GamePlayer.ROLE_RUNNING_MAN)
        end
    end
end

function GameMatch:getRandomPlayer()
    local random = math.random(1, PlayerManager:getPlayerCount())
    local index = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        index = index + 1
        if index == random then
            return v
        end
    end
    return nil
end

function GameMatch:sendGameSettlement()
    local Players = self:sortPlayerRank()

    local players = {}

    for rank, player in pairs(Players) do
        players[rank] = self:newSettlementItem(player, rank)
    end

    for rank, player in pairs(Players) do
        local result = {}
        result.players = players
        result.own = self:newSettlementItem(player, rank)
        HostApi.sendGameSettlement(player.rakssid, json.encode(result), true)
    end
end

function GameMatch:newSettlementItem(player, rank)
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
    if player.isLife or rank == 1 then
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
    local players = self:sortPlayerRank()
    return RankManager:getPlayerRank(players, player)
end

function GameMatch:isGameStart()
    return self.hasStartGame
end

function GameMatch:removeFootBlock()
    local time = tonumber(HostApi.curTimeString())
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if time - v.lastMoveTime >= 150 then
            v:removeFootBlocks()
        end
    end
end

function GameMatch:endMatch()

    GameTimeTask:pureStop()

    local finalWin = false

    if (PlayerManager:getPlayerCount() >= 1) then
        finalWin = true
    end

    local winPlayerName
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if finalWin and v.isLife then
            winPlayerName = v.name
        end
    end

    if finalWin and winPlayerName ~= nil then
        MsgSender.sendMsg(Messages:msgWinnerInfo(winPlayerName))
    else
        MsgSender.sendMsg(IMessages:msgNoWinner(0))
    end

    if finalWin then
        for i, v in pairs(players) do
            v:onWin()
        end
    end

    self:doReward()

    for i, v in pairs(players) do
        if v.isLife then
            v:onGameEnd(finalWin)
        end
    end
    self.hasEndGame = true
    GameOverTask:start()
end

function GameMatch:doReward()
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, false)
    end
    RewardManager:getQueueReward(function(data)
        self:sendGameSettlement()
    end)
end

function GameMatch:cleanPlayer()
    self:doReport()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame(false, true)
    end
end

function GameMatch:doReport()
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        ReportManager:reportUserData(player.userId, 0, rank, 1)
    end
    GameServer:closeServer()
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

function GameMatch:ifGameOver()
    if self.getLifePlayer() <= 0 then
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
