--region *.lua
require "config.GameConfig"

local CFinalFightStartTask = class("FinalFightStartTask", ITask)

function CFinalFightStartTask:onTick(ticks)

    if (ticks >= GameConfig.ffStartTime) then
        self:stop()
        return
    end

    local seconds = GameConfig.ffStartTime - ticks;

    if seconds >= 60 and seconds % 60 == 0 then
        MsgSender.sendMsg(IMessages:msgFightGameStartTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgFightGameStartTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MsgSender.sendMsg(IMessages:msgFightGameStartTimeHint(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgFightGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end
end

function CFinalFightStartTask:onCreate()
    GameMatch.curStatus = self.status
end

function CFinalFightStartTask:stop()
    SecTimer.stopTimer(self.timer)
    MsgSender.sendMsg(IMessages:msgFightGameStart())
    FinalFightTimeTask:start()
    GameMatch.allowPvp = true
end

function CFinalFightStartTask:start()
    MsgSender.sendMsg(IMessages:msgFightGameStart())
    SecTimer.startTimer(self.timer)
    GameMatch.curStatus = self.status
end


FinalFightStartTask = CFinalFightStartTask.new(5)

return FinalFightStartTask
--endregion
