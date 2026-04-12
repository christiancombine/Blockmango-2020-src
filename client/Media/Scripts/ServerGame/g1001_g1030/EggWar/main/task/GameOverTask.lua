--region *.lua
require "config.GameConfig"

local CGameOverTask = class("GameOverTask", ITask)

function CGameOverTask:onTick(ticks)

    if (ticks >= GameConfig.gameOverTime) then
        self:stop()
        return
    end

    local seconds = GameConfig.gameOverTime - ticks

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

function CGameOverTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGameOverTask:start()
    GameMatch.curStatus = self.status
    SecTimer.startTimer(self.timer)
    MsgSender.sendMsg(IMessages:msgCloseServerTimeHint(GameConfig.gameOverTime, IMessages.UNIT_SECOND, false))
end

function CGameOverTask:stop()
    GameMatch:cleanPlayer()
    SecTimer.stopTimer(self.timer)
end

GameOverTask = CGameOverTask.new(4)

return GameOverTask
--endregion
