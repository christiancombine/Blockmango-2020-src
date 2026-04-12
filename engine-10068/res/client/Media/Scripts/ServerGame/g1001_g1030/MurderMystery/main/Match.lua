--Match.lua
require "messages.Messages"

GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.AssignRole = 2
GameMatch.Status.Running = 3
GameMatch.Status.GameOver = 4
GameMatch.Status.WaitingReset = 5

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.gameType = "g1009"

GameMatch.gameWaitTick = 0
GameMatch.gameAssignRoleTick = 0
GameMatch.gameStartTick = 0
GameMatch.gameOverTick = 0
GameMatch.gameTelportTick = 0

GameMatch.curGameScene = nil

GameMatch.MORE_MURDERER_ODDS_GROWTH = 10
GameMatch.moreOneMurdererOdds = 0
GameMatch.isFirstReady = true
GameMatch.isReward = false

GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:startAssignRole()
    HostApi.sendGameStatus(0, 1)
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    self.gameAssignRoleTick = self.curTick
    self.curStatus = self.Status.AssignRole
    self.isReward = false
    self.curGameScene = SceneConfig:randomScene()
    MsgSender.sendMsg(IMessages:msgGameStart())
    MsgSender.sendMsg(Messages:assignRoleTimeHint(GameConfig.assignRoleTime))
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:teleRandomPos()
    end
end

function GameMatch:getCurGameScene()
    if self.curGameScene == nil then
        self.curGameScene = SceneConfig:randomScene()
    end
    return self.curGameScene
end

function GameMatch:startGame()
    RewardManager:startGame()
    if self:assignRole() then
        self.gameStartTick = self.curTick
        self.curStatus = self.Status.Running
        self:resetRecord()
        MsgSender.sendMsg(IMessages:msgGameStart())
    else
        self:resetGame(false)
    end
end

function GameMatch:assignRole()
    local pendings = self:getPlayerNumForRole(GamePlayer.ROLE_PENDING)
    local murders = 0
    if pendings >= 8 then
        murders = 2
    else
        murders = 1
    end
    local sheriffs = 1
    local mins = murders + sheriffs + 1
    if pendings < mins then
        MsgSender.sendMsg(IMessages:msgGameOverByLittle())
        return false
    else
        if pendings > mins then
            local odds = self.moreOneMurdererOdds
            local random = math.random(1, 100)
            if random <= odds then
                murders = murders + 1
                self.moreOneMurdererOdds = 0
            end
        end
    end
    local players = PlayerManager:getPlayers()
    while (pendings > 0) do
        for i, v in pairs(players) do
            if murders > 0 then
                if v.role == GamePlayer.ROLE_PENDING then
                    local odds = murders / pendings
                    local random = (math.random(1, 100) + v.murdererOddsAdd) / 100
                    if random <= odds then
                        if v:becomeRole(GamePlayer.ROLE_MURDERER) then
                            v.murdererOddsAdd = 0
                            murders = murders - 1
                            pendings = pendings - 1
                        end
                    end
                end
            elseif sheriffs > 0 then
                if v.role == GamePlayer.ROLE_PENDING then
                    local odds = sheriffs / pendings
                    local random = math.random(1, 100) / 100
                    if random <= odds then
                        if v:becomeRole(GamePlayer.ROLE_SHERIFF) then
                            v.murdererOddsAdd = 0
                            sheriffs = sheriffs - 1
                            pendings = pendings - 1
                        end
                    end
                end
            else
                if v.role == GamePlayer.ROLE_PENDING then
                    if v:becomeRole(GamePlayer.ROLE_CIVILIAN) then
                        v.murdererOddsAdd = v.murdererOddsAdd + GamePlayer.BECOME_MURDERER_ODDS_GROWTH
                        pendings = pendings - 1
                    end
                end
            end
        end
    end
    self:sendAllPlayerTeamInfo()
    return true
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

function GameMatch:doReward(role)
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, self.isSameTeam(player.role, role), 3)
    end
    RewardManager:getQueueReward(function(data, role)
        self:sendGameSettlement(role)
    end, role)
    players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:onGameEnd(self.isSameTeam(role, player.role))
    end
end

function GameMatch.isSameTeam(role1, role2)
    if role1 == role2 then
        return true
    end
    if (role1 == GamePlayer.ROLE_SHERIFF and role2 == GamePlayer.ROLE_CIVILIAN) or
            (role1 == GamePlayer.ROLE_CIVILIAN and role2 == GamePlayer.ROLE_SHERIFF) then
        return true
    end
    return false
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
    if self.isSameTeam(player.role, winner) then
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

function GameMatch:resetPlayer(report)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:reset(report)
    end
end

function GameMatch:resetRecord()
    self.recordNum = 1
    self.record = {}
end

function GameMatch:resetMap()
    HostApi:resetMap()
    self:getCurGameScene():reset()
end

function GameMatch:resetGame(report)
    self:resetPlayer(report)
    self:resetMap()
    self.gameTelportTick = self.curTick
    self.curStatus = self.Status.WaitingReset
    HostApi.sendGameStatus(0, 2)
    HostApi.sendResetGame(PlayerManager:getPlayerCount())
    MsgSender.sendMsg(Messages:gameResetTimeHint())
    MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.prepareTime, IMessages.UNIT_SECOND, false))
end

function GameMatch:ifStartAssignRole()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        if self.isFirstReady then
            MsgSender.sendMsg(IMessages:msgCanStartGame())
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
    local seconds = GameConfig.assignRoleTime - (self.curTick - self.gameAssignRoleTick);

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, Messages:assignRoleTimeHint(seconds))
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

    self:showGameData()

    self:addMoneyToWorld(ticks)

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

    if seconds <= 0 then
        self:doCivilianWin()
    end

end

function GameMatch:onPlayerLive(ticks)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:onLive(ticks)
    end
end

function GameMatch:showGameData()
    if self.curTick % 10 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player.isInGame == false then
            MsgSender.sendBottomTipsToTarget(player.rakssid, 12, IMessages:msgGameRunning())
        end
    end
end

function GameMatch:addMoneyToWorld(ticks)
    local moneyPos = self:getCurGameScene().moneyPos
    for i, v in pairs(moneyPos) do
        if ticks % v.tick == 0 then
            EngineWorld:addEntityItem(v.id, 1, 0, v.life, v.pos)
        end
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

function GameMatch:onTick(ticks)

    self.curTick = ticks

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifStartAssignRole()
    end

    if self.curStatus == self.Status.AssignRole then
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

function GameMatch:getLifeTeams()
    local lifeTeam = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_MURDERER then
            lifeTeam = lifeTeam + 1
        end
        if v.role == GamePlayer.ROLE_CIVILIAN then
            lifeTeam = lifeTeam + 1
        end
    end
    return lifeTeam
end

function GameMatch:ifGameOverByPlayer()
    local hasCivilian, hasMurderer = false, false
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_MURDERER then
            hasMurderer = true
        end
        if v.role == GamePlayer.ROLE_CIVILIAN then
            hasCivilian = true
        end
    end
    if hasCivilian == false and hasMurderer then
        self:doMurdererWin()
        return true
    end
    if hasCivilian and hasMurderer == false then
        self:doCivilianWin()
        return true
    end
    return false
end

function GameMatch:doMurdererWin()
    self.gameOverTick = self.curTick
    self.curStatus = self.Status.GameOver
    MsgSender.sendMsg(IMessages:msgWinnerInfo(Messages:getRoleName(GamePlayer.ROLE_MURDERER)))
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_MURDERER then
            v:onWin()
        end
    end
    self:doReward(GamePlayer.ROLE_MURDERER)
    self.moreOneMurdererOdds = 0
end

function GameMatch:doCivilianWin()
    if self.isReward == false then
        self.isReward = true
        self.gameOverTick = self.curTick
        self.curStatus = self.Status.GameOver
        MsgSender.sendMsg(IMessages:msgWinnerInfo(Messages:getRoleName(GamePlayer.ROLE_CIVILIAN) .. "&" .. Messages:getRoleName(GamePlayer.ROLE_SHERIFF)))
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            if v.role == GamePlayer.ROLE_CIVILIAN or v.role == GamePlayer.ROLE_SHERIFF then
                v:onWin()
            end
        end
        self:doReward(GamePlayer.ROLE_CIVILIAN)
        self.moreOneMurdererOdds = self.moreOneMurdererOdds + GameMatch.MORE_MURDERER_ODDS_GROWTH
    end
end

function GameMatch:isGameStart()
    return self.curStatus == self.Status.Running
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

function GameMatch:transferSheriff(sheriff)
    local civilians = self:getPlayerNumForRole(GamePlayer.ROLE_CIVILIAN)
    if civilians <= 0 then
        return
    end
    local random = math.random(1, civilians)
    local index = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_CIVILIAN then
            index = index + 1
            if index == random then
                v:becomeRole(GamePlayer.ROLE_SHERIFF)
                self:sendPlayerChangeTeam(v, GamePlayer.ROLE_SHERIFF)
                self:sendPlayerChangeTeam(sheriff, GamePlayer.ROLE_CIVILIAN)
                break
            end
        end
    end
end

function GameMatch:sendPlayerChangeTeam(player, role)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isInGame then
            HostApi.changePlayerTeam(v.rakssid, player.userId, role)
        end
    end
end

function GameMatch:onPlayerQuit(player)
    if player.role == GamePlayer.ROLE_SHERIFF then
        GameMatch:transferSheriff(player)
    end
    if self:isGameStart() then
        self:ifGameOverByPlayer()
    end
end

return GameMatch

--endregion