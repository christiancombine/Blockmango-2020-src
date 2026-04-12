-- WaitPlayerTask.lua

local CWaitPlayerTask = class("WaitingPlayerTask", ITask)

function CWaitPlayerTask:onTick(ticks)
    MsgSender.sendTopTips(2, Messages:tryStage())

    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        GameMatch.timer = GameMatch.timer + 1
        if (GameMatch.timer >= GameConfig.waitingTime) then
            self:stop()
            return
        end

        if ticks % GameConfig.waitingTime == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
            MsgSender.sendMsg(Messages:longWaitingHint())
        end

        if  PlayerManager:isPlayerFull() then
            self:stop()
            return
        end

        local seconds = GameConfig.waitingTime - GameMatch.timer
        if seconds >= 60 and seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayerEnough(seconds / 60, IMessages.UNIT_MINUTE, false))

        elseif seconds >= 60 and seconds % 30 == 0 then
            if seconds % 60 == 0 then
                MsgSender.sendMsg(IMessages:msgWaitPlayerEnough(seconds / 60, IMessages.UNIT_MINUTE, false))
            else
                MsgSender.sendMsg(IMessages:msgWaitPlayerEnough(seconds / 60, IMessages.UNIT_MINUTE, true))
            end
        elseif seconds <= 10 and seconds > 0 then
            MsgSender.sendBottomTips(2, IMessages:msgWaitPlayerEnough(seconds, IMessages.UNIT_SECOND, false))
            if seconds <= 3 then
                HostApi.sendPlaySound(0, 12)
            else
                HostApi.sendPlaySound(0, 11)
            end
        end
    else
        GameMatch.timer = 0
        if ticks % GameConfig.waitingTime == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
            MsgSender.sendMsg(Messages:longWaitingHint())
        end
    end
end

function CWaitPlayerTask:onCreate()
    GameMatch.curStatus = self.status
end

function CWaitPlayerTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:prepareMatch()
end

WaitingPlayerTask = CWaitPlayerTask.new(GameMatch.Status.WaitingPlayer)

return WaitingPlayerTask

-- endregion
