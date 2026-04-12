-- WaitPlayerTask.lua

local CWaitPlayerTask = class("WaitingPlayerTask", ITask)
CWaitPlayerTask.totalWaitTicks = 0

function CWaitPlayerTask:onTick(ticks)

    --if PlayerManager:getPlayerCount() ~= AIManager:getAICount() then
    --    self.totalWaitTicks = self.totalWaitTicks + 1
    --    if self.totalWaitTicks == 600 then
    --        ---有真人等待超过10分钟，发送错误邮件
    --        LogUtil.log("[SCRIPT_EXCEPTION]-------wait player long time !!! current player count = "
    --                .. PlayerManager:getPlayerCount() .. ", current ai count = " .. AIManager:getAICount(), LogUtil.LogLevel.Error)
    --    end
    --end

    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) == false then
        self.isReady = false
        if ticks % 30 == 0 then
            MessagesManage:sendSystemTipsToAll(IMessages:msgWaitPlayer())
            MessagesManage:sendSystemTipsToAll(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
        end
        return
    end

    local waitTime = 0

    if ticks > GameConfig.waitingPlayerTime then
        if self.isReady == false then
            self.waitTick = ticks
            self.isReady = true
        end

        waitTime = GameConfig.waitingPlayerTime2 - (ticks - self.waitTick)
    else
        waitTime = GameConfig.waitingPlayerTime - ticks + GameConfig.waitingPlayerTime2
    end

    if waitTime <= 0 or PlayerManager:getPlayerCount() >= PlayerManager:getRoomMaxPlayer() then
        self:stop()
        return
    end

    local seconds = waitTime

    if seconds >= 60 and seconds % 60 == 0 then
        MessagesManage:sendSystemTipsToAll(IMessages:msgWaitPlayerEnough(seconds / 60, IMessages.UNIT_MINUTE, false))
    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MessagesManage:sendSystemTipsToAll(IMessages:msgWaitPlayerEnough(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MessagesManage:sendSystemTipsToAll(IMessages:msgWaitPlayerEnough(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MessagesManage:sendSystemTipsToAll(IMessages:msgWaitPlayerEnough(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

end

function CWaitPlayerTask:onCreate()
    GameMatch.curStatus = self.status
    self.isReady = false
    self.waitTick = 0
end

function CWaitPlayerTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:prepareMatch()
end

WaitingPlayerTask = CWaitPlayerTask.new(1)

return WaitingPlayerTask

-- endregion
