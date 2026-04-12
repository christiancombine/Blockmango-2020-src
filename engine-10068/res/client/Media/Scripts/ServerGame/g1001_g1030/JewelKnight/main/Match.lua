require "messages.Messages"
require "config.JewelConfig"

GameMatch = {}

GameMatch.gameType = "g1024"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.AssignTeam = 2
GameMatch.Status.Running = 3
GameMatch.Status.Calculate = 4
GameMatch.Status.GameOver = 5

GameMatch.hasStartGame = false
GameMatch.isReward = false

GameMatch.gameWaitTick = 0
GameMatch.gameAssignTeamTick = 0
GameMatch.gameStartTick = 0
GameMatch.gameCalculateTick = 0
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

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifGameStart()
    end

    if self.curStatus == self.Status.AssignTeam then
        self:ifStartAssignTeam()
    end

    if self.curStatus == self.Status.Running then
        self:sendGameTeamInfo()
        self:ifGameCalculate()
        self:addResourceToWorld()
    end

    if self.curStatus == self.Status.Calculate then
        self:ifGameOver()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifCloseServer()
    end

end

function GameMatch:ifGameStart()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        if self.isFirstReady then
            MsgSender.sendMsg(IMessages:msgCanStartGame())
            MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.waitingPlayerTime, IMessages.UNIT_SECOND, false))
            self.gameWaitTick = self.curTick
            self.isFirstReady = false
        end
        if self.curTick - self.gameWaitTick > GameConfig.waitingPlayerTime or PlayerManager:isPlayerFull() then
            self:GameStart()
        else
            local seconds = GameConfig.waitingPlayerTime - (self.curTick - self.gameWaitTick)
            if seconds == 12 then
                self.prestartPlayerCount = PlayerManager:getPlayerCount()
                HostApi.sendStartGame(self.prestartPlayerCount)
            end
            if seconds <= 60 and seconds > 0 then
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

function GameMatch:sendGameTeamInfo()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isReady == true and player.hasSendTeamInfo == false then
            HostApi.changePlayerTeam(0, player.userId, player.team.id)
            player.hasSendTeamInfo = true
        end
    end
end

function GameMatch:ifGameCalculate()

    local ticks = self.curTick - self.gameStartTick

    local seconds = GameConfig.gameTime - ticks

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
        self:ifGameOver()
    end
end

function GameMatch:ifGameOverByPlayer()
    local winTeamName
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.team.isWin == true and self.isReward == false then
            winTeamName = v.team:getDisplayName()
            self.winner = v.team.id
            v:onWin()
        end
    end

    if winTeamName ~= nil then
        MsgSender.sendMsg(IMessages:msgWinnerInfo(winTeamName))
    else
        MsgSender.sendMsg(IMessages:msgNoWinner(1))
    end

    self.curStatus = self.Status.Calculate
    self.gameCalculateTick = self.curTick
end

function GameMatch:ifGameOver()
    local ticks = self.curTick - self.gameCalculateTick
    local seconds = GameConfig.gameCalculateTime - ticks

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        self.isReward = true
        self:doReward(self.winner)
        self:savePlayerRank()
        self.curStatus = self.Status.GameOver
        self.gameOverTick = self.curTick
    end
end

function GameMatch:sortPlayerRank()
    local players = PlayerManager:copyPlayers()

    table.sort(players, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end
        return a.userId > b.userId
    end)

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
    for _, player in pairs(players) do
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

    if player.team.id == winner then
        item.isWin = 1
    else
        item.isWin = 0
    end
    local players = PlayerManager:getPlayers()
    for _, v in pairs(players) do
        if v.knightCount ~= 0 then
            item.jewelKnight = v.name
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

function GameMatch:GameStart()
    HostApi.sendGameStatus(0, 1)
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    GameMatch.hasStartGame = true
    RewardManager:startGame()
    self.gameAssignTeamTick = self.curTick
    self.curStatus = self.Status.AssignTeam
end

function GameMatch:startAssignTeam()
    self:assignTeam()
    self:updateTeamResource()
    JewelConfig:Generation()
    TeamConfig:prepareTeamStatue()
    self.gameStartTick = self.curTick
    MsgSender.sendMsg(IMessages:msgGameStart())
    MsgSender.sendBottomTips(10, Messages:gameStart())
    self:startRunning()
end

function GameMatch:startRunning()
    self.curStatus = self.Status.Running
end

function GameMatch:assignTeam()
    local index = 1
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        index = index % 4
        if index == 0 then
            index = 4
        end
        player:clearInv()
        player:addItem(MerchantConfig.tools["0"].itemId, 1, 0)
        player:initTeam(index)
        player:teleportPosWithYaw(player.team.initPos, player.team.yaw)
        player:setShowName(player:buildShowName())
        index = index + 1
    end
end

function GameMatch:updateTeamResource()
    local resource = {}
    for i, team in pairs(self.teams) do
        resource[i] = {}
        resource[i].teamId = team.id
        resource[i].value = team.resource
        resource[i].maxValue = team.maxResource
    end

    HostApi.updateTeamResources(json.encode(resource))
end

function GameMatch:onPlayerQuit(player)
    if self.curStatus == self.Status.Running then
        player.team:subPlayer()
    end
    player:onQuit()
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

function GameMatch:savePlayerRank()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:saveRankData()
    end
end

function GameMatch:addResourceToWorld()
    for i, v in pairs(ResourceConfig.resource) do
        if self.curTick % v.time == 0 then
            local position = VectorUtil.newVector3(v.x, v.y, v.z)
            EngineWorld:addEntityItem(v.itemId, v.num, 0, v.life, position)
        end
    end
end

return GameMatch