--region *.lua
require "messages.Messages"
require "config.GameConfig"

local CGameTimeTask = class("GameTimeTask", ITask)

function CGameTimeTask:onTick(ticks)

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

    GameMatch.runningSecond = ticks

    self:removeFootBlock()

    self:onGameTick(ticks)

    self:showGameData(ticks)

end


function CGameTimeTask:onGameTick(ticks)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:onLifeTick(ticks)
    end
end

function CGameTimeTask:removeFootBlock()
    GameMatch:removeFootBlock()
end

function CGameTimeTask:showGameData(ticks)
    if ticks % 2 == 0 then
        return
    end
    local f1, f2, f3, f4 = 0, 0, 0, 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isLife then
            local floor = GameConfig:getFloorNum(v.positionY)
            if floor == 1 then
                f1 = f1 + 1
            end
            if floor == 2 then
                f2 = f2 + 1
            end
            if floor == 3 then
                f3 = f3 + 1
            end
            if floor == 4 then
                f4 = f4 + 1
            end
        end
    end
    for i, v in pairs(players) do
        local floor = GameConfig:getFloorNum(v.positionY)
        MsgSender.sendBottomTipsToTarget(v.rakssid, 3, Messages:gameDataHint(f1, f2, f3, f4, floor, ticks))
    end
end

function CGameTimeTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGameTimeTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:endMatch()
end

GameTimeTask = CGameTimeTask.new(3)

return GameTimeTask

--endregion
