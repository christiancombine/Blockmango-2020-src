-- WaitPlayerTask.lua

local CWaitPlayerTask = class("WaitingPlayerTask", ITask)

function CWaitPlayerTask:onTick(ticks)

    if (ticks >= GameConfig.waitingPlayerTime) then
        self:stop()
        return
    end

    if PlayerManager:isPlayerFull() then
        self:stop()
        return
    end

    local seconds = GameConfig.waitingPlayerTime - ticks;

    if seconds == 12 then
        HostApi.sendStartGame(PlayerManager:getPlayerCount())
    end
    
    if seconds >= 60 and seconds % 60 == 0 then
        MsgSender.sendMsg(IMessages:msgWaitPlayerEnough(seconds / 60, IMessages.UNIT_MINUTE, false))

    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayerEnough(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MsgSender.sendMsg(IMessages:msgWaitPlayerEnough(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgWaitPlayerEnough(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

end

function CWaitPlayerTask:onCreate()
    GameMatch.curStatus = self.status
end

function CWaitPlayerTask:stop()
    SecTimer.stopTimer(self.timer)
    DisableMoveTask:start()
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    HostApi.sendGameStatus(0, 1)
    MsgSender.sendMsg(IMessages:msgGameStart())
end

WaitingPlayerTask = CWaitPlayerTask.new(1)

return WaitingPlayerTask

-- endregion
