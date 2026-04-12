--region *.lua

local CGameTimeTask = class("GameTimeTask", ITask)

function CGameTimeTask:onTick(ticks)

    self.runTick = ticks

    if (ticks >= GameConfig.gameTime) then
        self:stop()
    end

    local seconds = GameConfig.gameTime - ticks;

    if seconds >= 60 and seconds % 60 == 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    GameMatch:onTick(ticks)

end

function CGameTimeTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGameTimeTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:doGameOver()
end

GameTimeTask = CGameTimeTask.new(GameMatch.Status.GameTime)

return GameTimeTask

--endregion
