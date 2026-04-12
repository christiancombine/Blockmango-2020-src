---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"
require "util.RewardUtil"
require "util.GameManager"
require "util.DbUtil"
require "util.BackHallManager"

GameMatch = {}

GameMatch.gameType = "g1033"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.Waiting = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.Running = 3
GameMatch.Status.GameOver = 4
GameMatch.Status.WaitClose = 5

GameMatch.PrepareTick = 0
GameMatch.RunningTick = 0
GameMatch.GameOverTick = 0
GameMatch.WaitCloseTick = 0

GameMatch.Teams = {}

GameMatch.isReward = false
GameMatch.curKills = 0

GameMatch.canMove = true

GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:ifUpdateRank()

    if self.curStatus == self.Status.Waiting then
        self:ifWaitingEnd()
    end

    if self.curStatus == self.Status.Prepare then
        self:ifPrepareEnd()
    end

    if self.curStatus == self.Status.Running then
        self:ifRunningEnd()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifGameOverEnd()
    end

    if self.curStatus == self.Status.WaitClose then
        self:ifWaitCloseEnd()
    end

    BackHallManager:onTick()
end

function GameMatch:ifWaitingEnd()
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
    local seconds = GameConfig.prepareTime - (self.curTick - self.PrepareTick);

    if seconds == 45 or seconds == 30 then
        MsgSender.sendMsg(IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
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

function GameMatch:ifRunningEnd()

    local ticks = self.curTick - self.RunningTick

    local seconds = GameConfig.gameTime - ticks

    if ticks <= GameConfig.unableMoveTick and GameMatch.canMove == true then
        GameMatch.canMove = false
        MsgSender.sendBottomTips(GameConfig.unableMoveTick - 20, Messages:msgStandToBuyTip())
    elseif ticks > GameConfig.unableMoveTick and GameMatch.canMove == false then
        GameMatch.canMove = true
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
        self:doRunningEnd()
    end
    GameManager:onTick(ticks)
end

function GameMatch:ifGameOverEnd()
    local ticks = self.curTick - self.GameOverTick
    local seconds = GameConfig.gameOverTime - ticks;
    if seconds <= 0 then
        self:doGameOverEnd()
    end
    MsgSender.sendBottomTips(3, IMessages:msgCloseServerTimeHint(seconds))
    if ticks >= GameConfig.showRankTick then
        if self.isReward then
            return
        end
        self.isReward = true
        local players = RewardUtil:sortPlayerRank()
        RewardUtil:doReward(players)
        RewardUtil:doReport(players)
    end
end

function GameMatch:doWaitingEnd()
    self.curStatus = self.Status.Prepare
    self.PrepareTick = self.curTick
end

function GameMatch:doPrepareEnd()
    self.curStatus = self.Status.Running
    self.RunningTick = self.curTick
    RewardManager:startGame()
    GameManager:prepareGame()
end

function GameMatch:doRunningEnd()
    self.curStatus = self.Status.GameOver
    self.GameOverTick = self.curTick
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onGameEnd()
    end
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
end

function GameMatch:doGameOverEnd()
    self.curStatus = self.Status.WaitClose
    self.WaitCloseTick = self.curTick
    local players = PlayerManager:copyPlayers()
    for _, player in pairs(players) do
        player:overGame()
    end
end

function GameMatch:ifWaitCloseEnd()
    if self.curTick - self.WaitCloseTick >= 10 then
        self:doWaitCloseEnd()
    end
end

function GameMatch:doWaitCloseEnd()
    GameTimeTask:pureStop()
    GameServer:closeServer()
end

function GameMatch:onPlayerQuit(player)
    GameMatch:sendAllPlayerTeamInfo()
end

function GameMatch:ifGameRunning()
    return self.curStatus == self.Status.Running
end

function GameMatch:ifGameEnd()
    return self.curStatus == self.Status.GameOver
end

function GameMatch:sendAllPlayerTeamInfo()
    local result = {}
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        local item = {}
        item.userId = tonumber(tostring(v.userId))
        item.teamId = v.teamId
        table.insert(result, item)
    end
    HostApi.sendPlayerTeamInfo(json.encode(result))
end

function GameMatch:ifUpdateRank()
    if self.curTick % 300 == 0 and PlayerManager:isPlayerExists() then
        RankNpcConfig:updateRank()
    end
end

return GameMatch