---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"

GameMatch = {}

GameMatch.gameType = "g1017"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.AssignRole = 2
GameMatch.Status.ChangeActor = 3
GameMatch.Status.Running = 4
GameMatch.Status.GameOver = 5
GameMatch.Status.WaitingReset = 6

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.gameWaitTick = 0
GameMatch.gameAssignRoleTick = 0
GameMatch.gameChangeActorTick = 0
GameMatch.gameStartTick = 0
GameMatch.gameOverTick = 0
GameMatch.gameTelportTick = 0

GameMatch.curTick = 0

GameMatch.isFirstReady = true
GameMatch.isReward = false

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks

    self:ifUpdateRank()

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifStartAssignRole()
    end

    if self.curStatus == self.Status.AssignRole then
        self:ifAssignRoleEnd()
    end

    if self.curStatus == self.Status.ChangeActor then
        self:ifGameStart()
    end

    if self.curStatus == self.Status.Running then
        self:ifGameOver()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifTelportEnd()
    end

    if self.curStatus == self.Status.WaitingReset then
        self:ifResetEnd()
    end

end

function GameMatch:ifUpdateRank()
    if self.curTick % 300 == 0 and PlayerManager:isPlayerExists() then
        RankNpcConfig:updateRank()
    end
end

function GameMatch:ifAssignRoleEnd()
    local seconds = GameConfig.assignRoleTime - (self.curTick - self.gameAssignRoleTick);

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, Messages:assignRoleTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds <= 0 then
        self:assignRoleEnd()
    end
end

function GameMatch:assignRoleEnd()
    if self:assignRole() then
        self.gameChangeActorTick = self.curTick
        self.curStatus = self.Status.ChangeActor
        self:resetRecord()
        MsgSender.sendMsg(IMessages:msgGameStart())
        self:showChangeAndHideHint(GameConfig.changeActorTime)
        HostApi.sendPlaySound(0, 10019)
    else
        self:resetGame(false)
    end
end

function GameMatch:showChangeAndHideHint(seconds)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_HIDE then
            if seconds > 10 then
                MsgSender.sendMsgToTarget(v.rakssid, Messages:changeAndHideHint(seconds))
            else
                MsgSender.sendBottomTipsToTarget(v.rakssid, seconds, Messages:changeAndHideHint(seconds))
            end
        elseif v.role == GamePlayer.ROLE_SEEK then
            if seconds > 10 then
                MsgSender.sendMsgToTarget(v.rakssid, Messages:waitChangeAndHideHint(seconds))
            else
                MsgSender.sendBottomTipsToTarget(v.rakssid, seconds, Messages:waitChangeAndHideHint(seconds))
            end
        end
    end
end

function GameMatch:startAssignRole()
    HostApi.sendGameStatus(0, 1)
    self.isReward = false
    RewardManager:startGame()
    self.gameAssignRoleTick = self.curTick
    self.curStatus = self.Status.AssignRole
    self.curGameScene = SceneConfig:randomScene()

    MsgSender.sendMsg(Messages:assignRoleTimeHint(GameConfig.assignRoleTime))
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:teleScenePos()
    end

    self:getCurGameScene():prepareActorNpc()
end

function GameMatch:getCurGameScene()
    if self.curGameScene == nil then
        self.curGameScene = SceneConfig:randomScene()
    end
    return self.curGameScene
end

function GameMatch:startGame()
    self.gameStartTick = self.curTick
    self.curStatus = self.Status.Running
    self:unlockSeek()
    self:showHide()
    HostApi.syncGameTimeShowUi(0, true, GameConfig.gameTime)
end

function GameMatch:assignRole()
    local pendings = self:getPlayerNumForRole(GamePlayer.ROLE_PENDING)
    local seeks = GameConfig:getSeekNum(pendings)
    if seeks + 1 > pendings then
        return false
    end
    local players = PlayerManager:getPlayers()
    while pendings > 0 do
        for i, v in pairs(players) do
            if v.role == GamePlayer.ROLE_PENDING then
                if seeks > 0 then
                    local odds = seeks / pendings
                    local random = math.random(1, 100) / 100
                    if random <= odds then
                        if v.multiSeek < 1 then
                            if v:becomeRole(GamePlayer.ROLE_SEEK) then
                                seeks = seeks - 1
                                pendings = pendings - 1
                            end
                        end
                    end
                else
                    if v:becomeRole(GamePlayer.ROLE_HIDE) then
                        pendings = pendings - 1
                    end
                end
            end
        end
    end
    self:teleRolePos()
    self:rebuildShowName()
    self:sendAllPlayerTeamInfo()
    self:lockSeek()
    self:hideHide()
    return true
end

function GameMatch:lockSeek()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_SEEK then
            v:setCameraLocked(true)
        end
    end
end

function GameMatch:unlockSeek()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_SEEK then
            v:setCameraLocked(false)
        end
    end
end

function GameMatch:hideHide()
    local players = PlayerManager:getPlayers()
    for i, p1 in pairs(players) do
        for j, p2 in pairs(players) do
            if p1.role == GamePlayer.ROLE_HIDE and p2.role == GamePlayer.ROLE_SEEK then
                p1:changeInvisible(p2.rakssid, true)
            end
        end
    end
end

function GameMatch:showHide()
    local players = PlayerManager:getPlayers()
    for i, p1 in pairs(players) do
        for j, p2 in pairs(players) do
            if p1.role == GamePlayer.ROLE_HIDE and p2.role == GamePlayer.ROLE_SEEK then
                p1:changeInvisible(p2.rakssid, false)
            end
        end
    end
end

function GameMatch:teleRolePos()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:teleRolePos()
    end
end

function GameMatch:rebuildShowName()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:sendBuildShowName(players)
    end
end

function GameMatch:sendAllPlayerTeamInfo()
    local result = {}
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isInGame then
            local item = {}
            item.userId = tonumber(tostring(v.userId))
            item.teamId = v.role
            table.insert(result, item)
        end
    end
    HostApi.sendPlayerTeamInfo(json.encode(result))
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
        HostApi.sendGameSettlement(player.rakssid, json.encode(result), false)
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
    if player.role == winner then
        item.isWin = 1
    else
        item.isWin = 0
    end
    return item
end

function GameMatch:sortPlayerRank()
    local players = {}
    local c_players = PlayerManager:getPlayers()
    for i, player in pairs(c_players) do
        if player.role ~= GamePlayer.ROLE_PENDING then
            table.insert(players, player)
        end
    end

    table.sort(players, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end
        return a.userId > b.userId
    end)

    return players
end

function GameMatch:getPlayerRank(player)
    local players = self:sortPlayerRank();
    return RankManager:getPlayerRank(players, player)
end

function GameMatch:resetPlayer(report)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:setCameraLocked(false)
    end
    for i, v in pairs(players) do
        v:reset(report)
        HostApi.sendShowHideAndSeekBtnStatus(v.rakssid, false, false, false)
    end
end

function GameMatch:resetRecord()
    self.recordNum = 1
    self.record = {}
end

function GameMatch:resetMap()
    HostApi:resetMap()
end

function GameMatch:resetGame(report)
    self:resetPlayer(report)
    self:resetMap()
    self.gameTelportTick = self.curTick
    self.curStatus = self.Status.WaitingReset
    HostApi.sendGameStatus(0, 2)
    HostApi.sendResetGame(PlayerManager:getPlayerCount())
    HostApi.syncGameTimeShowUi(0, false, 0)
    MsgSender.sendMsg(Messages:gameResetTimeHint())
end

function GameMatch:ifStartAssignRole()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        if self.isFirstReady then
            MsgSender.sendMsg(IMessages:msgCanStartGame())
            MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.prepareTime, IMessages.UNIT_SECOND, false))
            self.gameWaitTick = self.curTick
            self.isFirstReady = false
        end
        if self.curTick - self.gameWaitTick > GameConfig.prepareTime then
            self:startAssignRole()
        else
            local seconds = GameConfig.prepareTime - (self.curTick - self.gameWaitTick)
            if seconds <= 10 and seconds > 0 then
                MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
                if seconds <= 3 then
                    HostApi.sendPlaySound(0, 12);
                else
                    HostApi.sendPlaySound(0, 11);
                end
            end
            if seconds == 3 then
                HostApi.sendStartGame(PlayerManager:getPlayerCount())
            end
        end
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
        end
        self.isFirstReady = true
    end
end

function GameMatch:ifGameStart()

    local seconds = GameConfig.changeActorTime - (self.curTick - self.gameChangeActorTick);

    if seconds <= 10 and seconds > 0 then
        self:showChangeAndHideHint(seconds)
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds <= 0 then
        self:startGame()
    end
end

function GameMatch:ifGameOver()

    local ticks = self.curTick - self.gameStartTick

    local seconds = GameConfig.gameTime - ticks;

    self:addRandomPropToWorld(ticks)

    self:onPlayerLive(ticks)

    if seconds % 20 == 0 then
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            if v.role == GamePlayer.ROLE_PENDING then
                MsgSender.sendMsgToTarget(v.rakssid, 82)
            end
        end
    end

    if seconds % 60 == 0 and seconds / 60 > 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
        if seconds <= 10 and seconds > 0 then
            MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds, IMessages.UNIT_SECOND, false))
            if seconds <= 3 then
                HostApi.sendPlaySound(0, 12);
            else
                HostApi.sendPlaySound(0, 11);
            end
        end
    end

    if seconds == GameConfig.invincibleTime then
        MsgSender.sendMsg(Messages:last45Second(seconds))
        MsgSender.sendCenterTips(3, Messages:last45Second(seconds))
    end

    if seconds <= 0 then
        self:doHideWin()
    end

end

function GameMatch:onPlayerLive(ticks)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:onLive(ticks)
    end
end

function GameMatch:addRandomPropToWorld(ticks)
    self:getCurGameScene().produceConfig:addPropToWorld(ticks)
end

function GameMatch:ifTelportEnd()

    local seconds = GameConfig.gameOverTime - (self.curTick - self.gameOverTick);

    MsgSender.sendBottomTips(3, Messages:gameOverHint(seconds))

    if seconds <= 0 then
        self:resetGame()
    end
end

function GameMatch:ifResetEnd()
    self.curStatus = self.Status.WaitingPlayer
    self.isFirstReady = true
    self:ifStartAssignRole()
end

function GameMatch:getLifeTeams()
    local lifeTeam = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_SEEK and v.isLife then
            lifeTeam = lifeTeam + 1
        end
        if v.role == GamePlayer.ROLE_HIDE and v.isLife then
            lifeTeam = lifeTeam + 1
        end
    end
    return lifeTeam
end

function GameMatch:ifGameOverByPlayer()
    local hasHide, hasSeek = false, false
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_SEEK and v.isLife then
            hasSeek = true
        end
        if v.role == GamePlayer.ROLE_HIDE and v.isLife then
            hasHide = true
        end
    end
    if hasHide == false and hasSeek then
        self:doSeekWin()
        return true
    end
    if hasHide and hasSeek == false then
        self:doHideWin()
        return true
    end
    return false
end

function GameMatch:getGameLastTime()
    return GameConfig.gameTime - (self.curTick - self.gameStartTick)
end

function GameMatch:getGameRunningTime()
    return self.curTick - self.gameStartTick
end

function GameMatch:doSeekWin()
    if self.isReward == false then
        self.isReward = true
        self.gameOverTick = self.curTick
        self.curStatus = self.Status.GameOver
        MsgSender.sendMsg(IMessages:msgWinnerInfo(Messages:getRoleName(GamePlayer.ROLE_SEEK)))
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            if v.role == GamePlayer.ROLE_SEEK then
                v:onWin()
            end
        end
        self:doReward(GamePlayer.ROLE_SEEK)
        self:getCurGameScene():reset()
    end
end

function GameMatch:doHideWin()
    if self.isReward == false then
        self.isReward = true
        self.gameOverTick = self.curTick
        self.curStatus = self.Status.GameOver
        MsgSender.sendMsg(IMessages:msgWinnerInfo(Messages:getRoleName(GamePlayer.ROLE_HIDE)))
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            if v.role == GamePlayer.ROLE_HIDE then
                v:onWin()
            end
        end
        self:doReward(GamePlayer.ROLE_HIDE)
        self:getCurGameScene():reset()
    end
end

function GameMatch:doReward(role)
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, role == player.role, 2)
    end
    RewardManager:getQueueReward(function(data, role)
        self:sendGameSettlement(role)
    end, role)
    players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:onGameEnd(role == player.role)
    end
    self:doReport()
end

function GameMatch:doReport()
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        if player.isReport == false then
            ReportManager:reportUserData(player.userId, player.scorePoints[ScoreID.KILL], rank, 1)
            player.isReport = true
        end
    end
end

function GameMatch:randomLifeHidePlayer()
    local num = self:getPlayerNumForRole(GamePlayer.ROLE_HIDE)
    if num == 0 then
        return nil
    end
    math.randomseed(os.clock() * 1000000)
    local index = math.random(1, num)
    local current = 0
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player.role == GamePlayer.ROLE_HIDE and player.isLife and player.isInGame then
            current = current + 1
        end
        if current == index then
            return player
        end
    end
    return nil
end

function GameMatch:isGameStart()
    return self.curStatus == self.Status.Running
end

function GameMatch:isChangeModel()
    return self.curStatus == self.Status.ChangeActor
end

function GameMatch:isCanChangeModel()
    return self.curStatus > self.Status.AssignRole
end

function GameMatch:isGameOver()
    return self.curStatus == self.Status.GameOver
end

function GameMatch:getInGamePlayerNum()
    local num = 0;
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isInGame then
            num = num + 1
        end
    end
    return num
end

function GameMatch:getPlayerNumForRole(role)
    local num = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == role and v.isInGame and v.isLife then
            num = num + 1
        end
    end
    return num
end

function GameMatch:onPlayerQuit(player)
    self:ifGameOverByPlayer()
end

return GameMatch