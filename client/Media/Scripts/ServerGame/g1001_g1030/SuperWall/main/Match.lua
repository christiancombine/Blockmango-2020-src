require "messages.Messages"
require "data.GameTeam"
require "util.RewardUtil"
require "util.GameManager"
require "util.SkillManager"

GameMatch = {}

GameMatch.gameType = "g1029"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.SelectRole = 3
GameMatch.Status.Running = 4
GameMatch.Status.GameOver = 5

GameMatch.Teams = {}

GameMatch.hasStartGame = false
GameMatch.hasEndGame = false

GameMatch.isReward = false
GameMatch.isEndFinish = false

GameMatch.gamePrepareTick = 0
GameMatch.gameSelectRoleTick = 0
GameMatch.gameStartTick = 0
GameMatch.gameOverTick = 0

GameMatch.curTick = 0

GameMatch.winner = 0

GameMatch.curStatus = GameMatch.Status.Init

function GameMatch:initMatch()
    GameTimeTask:start()
    self:initTeams()
end

function GameMatch:initTeams()
    local c_teams = TeamConfig:getTeams()
    for _, c_team in pairs(c_teams) do
        GameMatch.Teams[c_team.ID] = GameTeam.new(c_team)
    end
end

function GameMatch:onTick(ticks)
    self.curTick = ticks

    self:ifUpdateRank()

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifWaitingPlayerEnd()
    end

    if self.curStatus == self.Status.Prepare then
        self:ifPrepareEnd()
    end

    if self.curStatus == self.Status.SelectRole then
        self:ifSelectRoleEnd()
    end

    if self.curStatus == self.Status.Running then
        self:ifGameOver()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifCloseServer()
    end

end

function GameMatch:ifWaitingPlayerEnd()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        self:doWaitingEnd()
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
            MsgSender.sendMsg(IMessages:longWaitingHint())
        end
    end
end

function GameMatch:ifPrepareEnd()

    local seconds = GameConfig.prepareTime - (self.curTick - self.gamePrepareTick);

    if seconds == 45 or seconds == 30 then
        MsgSender.sendMsg(IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
    end

    if seconds == 12 then
        HostApi.sendStartGame(PlayerManager:getPlayerCount())
    end

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        self:doPrepareEnd()
    end
end

function GameMatch:ifSelectRoleEnd()
    local seconds = GameConfig.gamePrepareTime - (self.curTick - self.gameSelectRoleTick)

    if seconds == GameConfig.gamePrepareTime - 3 then
        MsgSender.sendCenterTips(3, Messages:msgCollectTip())
    end

    if seconds == 10 then
        MsgSender.sendCenterTips(3, Messages:msgOnly10SecondsWallCollapsedTip())
    end

    if seconds <= 10 and seconds > 0 then
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    self:addHP()
    if seconds <= 0 then
        self:doStartGame()
    end
end

function GameMatch:ifGameOver()

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
        self:doGameOver()
    end

    GameManager:onTick(ticks)
    SkillManager:onTick(ticks)
    self:addHP()
end

function GameMatch:doWaitingEnd()
    self.curStatus = self.Status.Prepare
    self.gamePrepareTick = self.curTick
    MsgSender.sendMsg(IMessages:msgCanStartGame())
    MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.prepareTime, IMessages.UNIT_SECOND, false))
end

function GameMatch:doPrepareEnd()
    self.curStatus = self.Status.SelectRole
    self.gameSelectRoleTick = self.curTick
    RewardManager:startGame()
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    HostApi.sendGameStatus(0, 1)
    GameMatch.hasStartGame = true
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:teleTeamPosAndYaw()
    end
    GameManager:prepareGame()
    MsgSender.sendCenterTips(3, Messages:msgWallPrepareCollapsedTip())
end

function GameMatch:doStartGame()
    self.curStatus = self.Status.Running
    self.gameStartTick = self.curTick
    MsgSender.sendMsg(IMessages:msgGameStart())
    AreaOfEliminateConfig:destoryAreaOfEliminate()
    MsgSender.sendCenterTips(3, Messages:msgWallHasCollapsedTip())
end

function GameMatch:ifGameOverByTeam()
    local lifes = self:getLifeTeams()
    if lifes <= 1 then
        self:doGameOver()
    end
end

function GameMatch:onPlayerQuit(player)
    player:onLogout()
    player:onQuit()
end

function GameMatch:doGameOver()
    if self.isEndFinish then
        return
    end

    self.isEndFinish = true
    self.curStatus = self.Status.GameOver
    self.gameOverTick = self.curTick
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onGameEnd()
    end

    local winner = RewardUtil:getWinnerTeam()
    if winner == 0 then
        MsgSender.sendMsg(IMessages:msgNoWinner(1))
    else
        local wins = GameMatch.Teams[winner]
        MsgSender.sendMsg(IMessages:msgWinnerInfo(wins:getDisplayName()))
    end

    for _, player in pairs(players) do
        if player:getTeamId() == winner then
            player:onWin()
        end
    end

    SkillManager:clearSkill()
    GameManager:onGameEnd()
end

function GameMatch:ifCloseServer()
    local ticks = self.curTick - self.gameOverTick
    local seconds = GameConfig.gameOverTime - ticks
    if seconds <= 0 then
        GameMatch:cleanPlayer()
        GameTimeTask:pureStop()
    end

    if ticks >= 7 then
        if self.isReward then
            return
        end
        self.isReward = true
        local players = RewardUtil:sortPlayerRank()
        RewardUtil:doReward(players)
        RewardUtil:doReport(players)
    end

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
end

function GameMatch:cleanPlayer()
    self:doReport()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame()
    end
end

function GameMatch:doReport()
    local players = RewardUtil:sortPlayerRank()
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
    for i, team in pairs(self.Teams) do
        if team:isDeath() == false then
            lifeTeam = lifeTeam + 1
        end
    end
    return lifeTeam
end

function GameMatch:addHP()
    if os.time() % 5 == 0 then
        local players = PlayerManager:getPlayers()
        for i, player in pairs(players) do
            player:addHP()
        end
    end
end

function GameMatch:showMsgTowerCollapsed(teamid)
    local players = PlayerManager:getPlayers()
    for index, player in pairs(players) do
        if player:getTeamId() == teamid then
            MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:msgTowerCollapsedTip())
            MsgSender.sendMsgToTarget(player.rakssid, Messages:msgTowerCollapsedTip())
        end
    end
end

function GameMatch:showMsgBasementUnderAttack(teamid)
    local players = PlayerManager:getPlayers()
    for index, player in pairs(players) do
        if player:getTeamId() == teamid then
            MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:msgBasementUnderAttackTip())
        end
    end
end

function GameMatch:showMsgBasementCollapsed(teamid)
    local players = PlayerManager:getPlayers()
    for index, player in pairs(players) do
        if player:getTeamId() == teamid then
            MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:msgBasementCollapsedTip())
            MsgSender.sendMsgToTarget(player.rakssid, Messages:msgBasementCollapsedTip())
        end
    end
end

function GameMatch:ifUpdateRank()
    if self.curTick % 300 == 0 and PlayerManager:isPlayerExists() then
        RankNpcConfig:updateRank()
    end
end

function GameMatch:getGameRunningTime()
    return self.curTick - self.gameOverTick
end

return GameMatch