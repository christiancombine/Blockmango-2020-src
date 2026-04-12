
local CGameOverTask = class("GameOverTask", ITask)

function CGameOverTask:onTick(ticks)

    if (ticks >= GameConfig.gameOverTime) then
        self:stop()
        return
    end

    local seconds = GameConfig.gameOverTime - ticks;

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgCloseServerTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

end

function CGameOverTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGameOverTask:start()
    GameMatch.curStatus = self.status
    SecTimer.startTimer(self.timer)
    MessagesManage:sendSystemTipsToAll(IMessages:msgCloseServerTimeHint(GameConfig.gameOverTime, IMessages.UNIT_SECOND, false))
end

function CGameOverTask:stop()
    GameMatch:cleanPlayer()
    SecTimer.stopTimer(self.timer)
end

GameOverTask = CGameOverTask.new(4);

return GameOverTask;
--endregion
