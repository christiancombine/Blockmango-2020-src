require "messages.Messages"
require "util.RespawnManager"
require "util.VideoAdHelper"

GameMatch = {}

GameMatch.gameType = "g1027"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.AssignTeam = 2
GameMatch.Status.Running = 3
GameMatch.Status.GameOver = 4

GameMatch.hasStartGame = false
GameMatch.isReward = false
GameMatch.hasEndGame = false

GameMatch.gameWaitTick = 0
GameMatch.gameAssignTeamTick = 0
GameMatch.gameStartTick = 0
GameMatch.gameOverTick = 0

GameMatch.curTick = 0

GameMatch.teams = {}
GameMatch.winner = 0

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.prestartPlayerCount = 0

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:sendEnchantmentData()

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifGameStart()
    end

    if self.curStatus == self.Status.AssignTeam then
        self:ifStartAssignTeam()
    end

    if self.curStatus == self.Status.Running then
        self:ifGameOver()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifCloseServer()
    end

end

function GameMatch:ifGameStart()
    local playerCount = PlayerManager:getPlayerCount()
    if playerCount >= GameConfig.startPlayers then
        if self.isFirstReady then
            MsgSender.sendMsg(IMessages:msgCanStartGame())
            MsgSender.sendMsg(
                    IMessages:msgGameStartTimeHint(GameConfig.waitingPlayerTime, IMessages.UNIT_SECOND, false)
            )
            self.gameWaitTick = self.curTick
            self.isFirstReady = false
        end
        local maxPlayer = PlayerManager:getMaxPlayer()
        if self.curTick - self.gameWaitTick > GameConfig.waitingPlayerTime or playerCount == maxPlayer then
            self:GameStart()
        else
            local seconds = GameConfig.waitingPlayerTime - (self.curTick - self.gameWaitTick)
            if seconds == 12 then
                self.prestartPlayerCount = playerCount
                HostApi.sendStartGame(playerCount)
            end
            if seconds <= 10 and seconds > 0 then
                MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
                if seconds <= 3 then
                    HostApi.sendPlaySound(0, 12)
                else
                    HostApi.sendPlaySound(0, 11)
                end
            end
        end
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
        end
        self.isFirstReady = true
        if self.prestartPlayerCount ~= 0 then
            self.prestartPlayerCount = 0
            HostApi.sendResetGame(PlayerManager:getPlayerCount())
        end
    end
end

function GameMatch:ifStartAssignTeam()
    local seconds = GameConfig.assignTeamTime - (self.curTick - self.gameAssignTeamTick)

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, Messages:assignTeamTimeHint(seconds))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        self:startAssignTeam()
    end
end

function GameMatch:ifGameOverByPlayer()
    local lifeTeam = self:getLifeTeams()
    if lifeTeam <= 1 then
        local finalWin = true
        local winner = 0

        if self:getLifeTeams() > 1 then
            finalWin = false
        end

        local winPlayerName

        for i, v in pairs(self.teams) do
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
                v.winner = 1
            end
        end

        self:doReward(winner)

        for i, v in pairs(players) do
            v:onGameEnd(finalWin)
        end

        self.hasEndGame = true
        self.curStatus = self.Status.GameOver
        self.gameOverTick = self.curTick
    end
end

function GameMatch:ifGameOver()
    local ticks = self.curTick - self.gameStartTick

    local seconds = GameConfig.gameTime - ticks

    RespawnManager:tick()
    SnowBallConfig:generateSnowBallByTime(ticks)
    NewIslandConfig:deleteIslandWall(ticks)
    self:ifGameOverByPlayer()
    if ticks % GameConfig.hpRecoverCD == 0 then
        self:hpRecover()
    end

    if seconds % 60 == 0 and seconds / 60 > 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    end

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        self:doReward(0)
        self.curStatus = self.Status.GameOver
        self.gameOverTick = self.curTick
    end
end

function GameMatch:GameStart()
    HostApi.sendGameStatus(0, 1)
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    GameMatch.hasStartGame = true
    TeamConfig:removeFootBlock()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:addInitItems()
    end
    RewardManager:startGame()
    self.gameAssignTeamTick = self.curTick
    self.curStatus = self.Status.AssignTeam
end

function GameMatch:startAssignTeam()
    self.gameStartTick = self.curTick
    MsgSender.sendMsg(IMessages:msgGameStart())
    self:startRunning()
end

function GameMatch:startRunning()
    self.curStatus = self.Status.Running
end

function GameMatch:onPlayerQuit(player)
    player:onLogout()
    player:onQuit()
    if self.hasStartGame == true and self.hasEndGame == false then
        self:ifGameOverByPlayer()
    end
end

function GameMatch:sortPlayerRank()
    local players = PlayerManager:copyPlayers()

    table.sort(
            players,
            function(a, b)
                if a.winner ~= b.winner then
                    return a.winner > b.winner
                end

                if a.kills ~= b.kills then
                    return a.kills > b.kills
                end

                if a.deathTimes ~= b.deathTimes then
                    return a.deathTimes < b.deathTimes
                end

                return a.userId > b.userId
            end
    )

    return players
end

function GameMatch:getPlayerRank(player)
    local players = self:sortPlayerRank()
    return RankManager:getPlayerRank(players, player)
end

function GameMatch:doReward(winner)
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, winner == player.team.id, 4)
    end
    RewardManager:getQueueReward(function(data, winner)
        self:sendGameSettlement(winner)
    end, winner)
    players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:onGameEnd(player.team.isWin)
    end
end

function GameMatch:sendGameSettlement(winner)
    local Players = self:sortPlayerRank()

    local players = {}

    for rank, player in pairs(Players) do
        players[rank] = self:newSettlementItem(player, rank, winner)
    end

    for rank, player in pairs(Players) do
        local result = {}
        player.rank = rank
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
    if winner == 0 then
        if player.team:isDeath() then
            item.isWin = 0
        else
            item.isWin = 2
        end
    else
        if player.team.id == winner then
            item.isWin = 1
        else
            item.isWin = 0
        end
    end

    return item
end

function GameMatch:ifCloseServer()
    local ticks = self.curTick - self.gameOverTick
    local seconds = GameConfig.gameOverTime - ticks

    if seconds >= 60 and seconds % 60 == 0 then
        MsgSender.sendMsg(IMessages:msgCloseServerTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgCloseServerTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MsgSender.sendMsg(IMessages:msgCloseServerTimeHint(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgCloseServerTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        GameMatch:cleanPlayer()
        GameTimeTask:pureStop()
    end
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

function GameMatch:isGameOver()
    return self.curStatus == self.Status.GameOver
end

function GameMatch:getLifeTeams()
    local lifeTeam = 0
    for i, v in pairs(self.teams) do
        if v:isDeath() == false then
            lifeTeam = lifeTeam + 1
        end
    end
    return lifeTeam
end

function GameMatch:getRealLifeTeams()
    local lifeTeam = 0
    for i, v in pairs(self.teams) do
        if v.realLife then
            lifeTeam = lifeTeam + 1
        end
    end
    return lifeTeam
end

function GameMatch:getRealLifePlayerByTeamId(teamId)
    local life = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.team.id == teamId then
            if v.realLife then
                life = life + 1
            end
        end
    end
    return life
end

function GameMatch:onPlayerDie(player)
    if self:getRealLifeTeams() <= 2 then
        player.realLife = false
        local ticks = self.curTick - self.gameStartTick
        if ticks >= GameConfig.canRespawnBeforeTime then
            local players = PlayerManager:getPlayers()
            for i, v in pairs(players) do
                if v.realLife == false then
                    v:onDie()
                end
            end
            GameMatch:ifGameOverByPlayer()
            return
        end
    end
    VideoAdHelper:tryShowSkyRoyaleRespawnVideoAd(player)
    player:showBuyRespawn()
end

function GameMatch:hpRecover()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player.entityPlayerMP:heal(GameConfig.hpRecover)
    end
end

function GameMatch:sendRealRankData(player)
    local players = self:sortPlayerByKillNum()
    local index = 0
    local data = {}
    data.ranks = {}
    for i, v in pairs(players) do
        index = index + 1
        local item = {}
        item.column_1 = tostring(index)
        item.column_2 = v.name
        item.column_3 = tostring(v.kills)
        item.column_4 = tostring(v.deathTimes)
        data.ranks[index] = item
    end
    local rank = RankManager:getPlayerRank(players, player)
    local own = {}
    own.column_1 = tostring(rank)
    own.column_2 = player.name
    own.column_3 = tostring(player.kills)
    own.column_4 = tostring(player.deathTimes)
    data.own = own
    HostApi.updateRealTimeRankInfo(player.rakssid, json.encode(data))
end

function GameMatch:sortPlayerByKillNum()
    local players = PlayerManager:copyPlayers()

    table.sort(
            players,
            function(a, b)
                if a.kills ~= b.kills then
                    return a.kills > b.kills
                end

                if a.deathTimes ~= b.deathTimes then
                    return a.deathTimes < b.deathTimes
                end

                return a.userId > b.userId
            end
    )

    return players
end

function GameMatch:sendEnchantmentData()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if DBManager:isLoadDataFinished(player.userId, 1) and player.hasSendEnchantmentData == false then
            player:sendEnchantmentData()
            player.hasSendEnchantmentData = true
        end
    end
end

return GameMatch
