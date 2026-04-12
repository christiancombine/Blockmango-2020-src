--region *.lua
require "config.GameConfig"

local CGamePrepareTask = class("GamePrepareTask", ITask)

function CGamePrepareTask:onTick(ticks)

    if(ticks >= GameConfig.prepareTime) then
        self:stop()
    end

    local seconds = GameConfig.prepareTime - ticks;

    if seconds >= 60 and seconds % 60 == 0 then
        MsgSender.sendMsg(IMessages:msgPvpEnableAfter(seconds / 60, IMessages.UNIT_MINUTE, false))
    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgPvpEnableAfter(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MsgSender.sendMsg(IMessages:msgPvpEnableAfter(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgPvpEnableAfter(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

end

function CGamePrepareTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGamePrepareTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:startMatch()
end

GamePrepareTask = CGamePrepareTask.new(2)

return GamePrepareTask
--endregion
