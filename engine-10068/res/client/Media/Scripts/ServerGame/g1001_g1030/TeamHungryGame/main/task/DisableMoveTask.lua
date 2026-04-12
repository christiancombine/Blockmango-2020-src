--region *.lua
require "config.GameConfig"

local CDisableMoveTask = class("DisableMoveTask", ITask)

function CDisableMoveTask:onTick(ticks)

    if (ticks >= GameConfig.disableMoveTime) then
        self:stop()
        return
    end

    local seconds = GameConfig.disableMoveTime - ticks;

    if seconds >= 60 and seconds % 60 == 0 then
        MsgSender.sendMsg(IMessages:msgGameStartTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgGameStartTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MsgSender.sendMsg(IMessages:msgGameStartTimeHint(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end
end

function CDisableMoveTask:onCreate()
    GameMatch.curStatus = self.status
end

function CDisableMoveTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:prepareMatch()
end

DisableMoveTask = CDisableMoveTask.new(2)

return DisableMoveTask


--endregion
