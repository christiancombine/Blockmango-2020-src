---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "util.RewardUtil"
require "util.SkillManager"
require "messages.Messages"
require "data.GameTeam"
require "config.MoneyConfig"

GameMatch = {}

GameMatch.gameType = "g1022"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.Waiting = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.SelectRole = 3
GameMatch.Status.Running = 4
GameMatch.Status.GameOver = 5
GameMatch.Status.Reset = 6

GameMatch.WaitingTick = 0
GameMatch.PrepareTick = 0
GameMatch.SelectRoleTick = 0
GameMatch.RunningTick = 0
GameMatch.GameOverTick = 0
GameMatch.ResetTick = 0

GameMatch.Teams = {}

GameMatch.curTick = 0
GameMatch.curStatus = GameMatch.Status.Init
GameMatch.curGameScene = nil

GameMatch.isFirstReady = false
GameMatch.isReady = false

function GameMatch:initMatch()
    GameTimeTask:start()
    GameMatch.curStatus = self.Status.Waiting
end

function GameMatch:onTick(ticks)
    self.curTick = ticks

    if self.curStatus == self.Status.Waiting then
        self:ifWaitingEnd()
    end

    if self.curStatus == self.Status.Prepare then
        self:ifPrepareEnd()
    end

    if self.curStatus == self.Status.SelectRole then
        self:ifSelectRoleEnd()
    end

    if self.curStatus == self.Status.Running then
        self:ifRunningEnd()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifGameOverEnd()
    end

    if self.curStatus == self.Status.Reset then
        self:ifResetEnd()
    end

end

function GameMatch:ifWaitingEnd()

    if self.isFirstReady then
        self.isFirstReady = false
        self.isReady = true
        self.WaitingTick = self.curTick
    end

    if self.isReady then

        local seconds = GameConfig.waitingTime - (self.curTick - self.WaitingTick);

        if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
            self:prepareGame()
        end

        if seconds <= 0 then
            if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
                self:prepareGame()
            else
                self.WaitingTick = self.curTick
                MsgSender.sendMsg(IMessages:msgWaitPlayer())
                MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
                MsgSender.sendMsg(Messages:longWaitingHint())
            end
        end
    end

end

function GameMatch:ifPrepareEnd()

    local seconds = GameConfig.prepareTime - (self.curTick - self.PrepareTick);

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds <= 0 or PlayerManager:isPlayerFull() then
        self:startSelectRole()
    end

end

function GameMatch:ifSelectRoleEnd()
    local seconds = GameConfig.selectRoleTime - (self.curTick - self.SelectRoleTick);

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, Messages:msgSelectRoleTime(seconds))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds % 10 == 0 then
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player.occupationId == 0 then
                MsgSender.sendBottomTipsToTarget(player.rakssid, 3, Messages:msgNoRoleHint())
            end
        end
    end

    if seconds <= 0 then
        self:doStartGame()
    end
end

function GameMatch:ifRunningEnd()

    local ticks = self.curTick - self.RunningTick

    local seconds = GameConfig.gameTime - ticks;

    self:getCurGameScene():onTick(ticks)
    self:tryGiveMoneyToPlayers(ticks)
    SkillManager:onTick(ticks)

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

    if seconds % 30 == 0 then
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            player:onGetFlagAfterHint()
        end
    end

    if seconds <= 0 then
        self:doGameOver()
    end

end

function GameMatch:tryGiveMoneyToPlayers(ticks)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        MoneyConfig:tryGiveMoneyToPlayer(ticks, player)
    end
end

function GameMatch:ifGameOverEnd()
    local seconds = GameConfig.gameOverTime - (self.curTick - self.GameOverTick);

    MsgSender.sendBottomTips(3, IMessages:msgGameOverHint(seconds))

    if seconds <= 0 then
        self:resetGame()
    end
end

function GameMatch:ifResetEnd()
    self.curStatus = self.Status.Waiting
    self.isFirstReady = true
end

function GameMatch:prepareGame()
    self.curStatus = self.Status.Prepare
    self.PrepareTick = self.curTick
    MsgSender.sendMsg(IMessages:msgCanStartGame())
    MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.prepareTime, IMessages.UNIT_SECOND, false))
end

function GameMatch:startSelectRole()
    self.curStatus = self.Status.SelectRole
    self.SelectRoleTick = self.curTick
    RewardManager:startGame()
    self:telePlayerToSelectRole()
    MsgSender.sendMsg(Messages:msgSelectRole(GameConfig.selectRoleTime))
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    HostApi.sendGameStatus(0, 1)
end

function GameMatch:telePlayerToSelectRole()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:teleSelectRole()
    end
end

function GameMatch:doStartGame()
    self.curStatus = self.Status.Running
    self.RunningTick = self.curTick
    self.curGameScene = SceneConfig:randomScene()
    MsgSender.sendMsg(Messages:selectMapHint(self.curGameScene.name))
    self:selectNormalOcc()
    self:assginTeam()
    self:getCurGameScene():prepareMerchants()
    self:telePlayerToScene()
    MsgSender.sendTopTips(GameConfig.gameTime, Messages:msgScore(GameMatch.Teams))
end

function GameMatch:selectNormalOcc()
    local npc = OccupationConfig:getSwordsmanNpc()
    if npc then
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player.occupationId == 0 then
                npc:onPlayerSelected(player)
            end
        end
    end
end

function GameMatch:assginTeam()
    local teams = self:getCurGameScene():getTeams()
    for id, team in pairs(teams) do
        self.Teams[id] = GameTeam.new(team)
    end
    local PlayerCount = PlayerManager:getPlayerCount()
    local MaxRandom = 300
    while PlayerCount > 0 and MaxRandom > 0 do
        local random = math.random(1, PlayerCount)
        local index = 0
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player.team == nil then
                index = index + 1
                if index == random then
                    player:setTeam(self:getMinPlayerTeam())
                    PlayerCount = PlayerCount - 1
                    break
                end
            end
        end
        MaxRandom = MaxRandom - 1
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.team == nil then
            player:setTeam(self:getMinPlayerTeam())
        end
    end
    self:sendAllPlayerTeamInfo()
end

function GameMatch:sendAllPlayerTeamInfo()
    local result = {}
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isInGame then
            local item = {}
            item.userId = tonumber(tostring(player.userId))
            item.teamId = player.team.id
            table.insert(result, item)
        end
    end
    HostApi.sendPlayerTeamInfo(json.encode(result))
end

function GameMatch:getMinPlayerTeam()
    local min
    for id, team in pairs(self.Teams) do
        if min == nil then
            min = team
        end
        if min.curPlayers > team.curPlayers then
            min = team
        end
    end
    return min
end

function GameMatch:telePlayerToScene()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:teleScenePos()
    end
end

function GameMatch:resetGame()
    self.curStatus = self.Status.Reset
    self:resetTeam()
    self:resetPlayer()
    HostApi.resetMap()
    SkillManager:clearSkill()
    MsgSender.sendMsg(IMessages:msgGameResetHint())
    MsgSender.sendTopTips(1, " ")
    HostApi.sendGameStatus(0, 2)
    HostApi.sendResetGame(PlayerManager:getPlayerCount())
end

function GameMatch:resetTeam()
    for id, team in pairs(self.Teams) do
        team:reset()
    end
end

function GameMatch:resetPlayer()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:reset()
    end
end

function GameMatch:getGameRunningTime()
    return self.curTick - self.RunningTick
end

function GameMatch:getCurGameScene()
    if self.curGameScene == nil then
        self.curGameScene = SceneConfig:randomScene()
    end
    return self.curGameScene
end

function GameMatch:ifGameOverByFlag()
    local flagStations = GameMatch:getCurGameScene():getFlagStations()
    local marks = {}
    for index, flagStation in pairs(flagStations) do
        if marks[flagStation.teamId] == nil then
            marks[flagStation.teamId] = 0
        end
        if flagStation.hasFlag == false then
            marks[flagStation.teamId] = marks[flagStation.teamId] + 1
        end
    end
    local hasWinner = false
    for teamId, mark in pairs(marks) do
        if mark == 0 then
            hasWinner = true
            break
        end
    end
    if hasWinner then
        self:doGameOver()
    end
end

function GameMatch:doGameOver()
    self.curStatus = self.Status.GameOver
    self.GameOverTick = self.curTick
    self:getCurGameScene():reset()
    local players = RewardUtil:sortPlayerRank()
    if #players == 0 then
        return
    end
    RewardUtil:doReward(players)
    RewardUtil:doReport(players)
end

function GameMatch:isSelectRole()
    return self.curStatus == self.Status.SelectRole
end

function GameMatch:isGameRunning()
    return self.curStatus == self.Status.Running
end

function GameMatch:onPlayerQuit(player)
    if self:isGameRunning() then
        self:getCurGameScene():onPlayerQuit(player)
    end
    player:onQuit()
    if self:isGameRunning() and PlayerManager:isPlayerEmpty() then
        self:doGameOver()
    end
end

function GameMatch:getDBDataRequire(player)
    DBManager:getPlayerData(player.userId, 1)
end

function GameMatch:savePlayerData(player, immediate)
    if player == nil then
        return
    end
    if player.isReady == false then
        return
    end
    local data = {}
    data.money = player.money
    data.fashions = player.fashions
    DBManager:savePlayerData(player.userId, 1, data, immediate)
end

return GameMatch