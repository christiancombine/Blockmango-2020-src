---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"

GameMatch = {}

GameMatch.gameType = "g1000"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.Waiting = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.Running = 3
GameMatch.Status.GameOver = 4

GameMatch.PrepareTick = 0
GameMatch.RunningTick = 0
GameMatch.GameOverTick = 0

GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks

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

end

function GameMatch:ifWaitingEnd()
    if PlayerManager:getPlayerCount() >= GameConfig.startPlayers then
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

    local seconds = GameConfig.gameTime - ticks;

    if seconds % 60 == 0 and seconds / 60 > 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    end

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds <= 0 then
        self:doRunningEnd()
    end

end

function GameMatch:ifGameOverEnd()

    local seconds = GameConfig.gameOverTime - (self.curTick - self.GameOverTick);

    MsgSender.sendBottomTips(3, IMessages:msgGameOverHint(seconds))

    if seconds <= 0 then
        self:doGameOverEnd()
    end
end

function GameMatch:doWaitingEnd()
    self.curStatus = self.Status.Prepare
    self.PrepareTick = self.curTick

end

function GameMatch:doPrepareEnd()
    self.curStatus = self.Status.Running
    self.gameStartTick = self.curTick
end

function GameMatch:doRunningEnd()
    self.curStatus = self.Status.GameOver
    self.GameOverTick = self.curTick
end

function GameMatch:doGameOverEnd()
    GameServer:closeServer()
end

function GameMatch:onPlayerQuit(player)

end

return GameMatch