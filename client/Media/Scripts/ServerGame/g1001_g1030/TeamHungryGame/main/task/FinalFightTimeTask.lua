--region *.lua
require "config.GameConfig"

local CFinalFightTimeTask = class("FinalFightTimeTask", ITask)

function CFinalFightTimeTask:onTick(ticks)

    if (ticks >= GameConfig.ffTime) then
        self:stop()
        return
    end

    local seconds = GameConfig.ffTime - ticks;

    if seconds >= 60 and seconds % 60 == 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    self:showGameData()

end

function CFinalFightTimeTask:onCreate()
    GameMatch.curStatus = self.status
end

function CFinalFightTimeTask:showGameData()
    local msg = ""
    for i, v in pairs(GameMatch.Teams) do
        msg = msg .. v:getDisplayStatus() .. TextFormat.colorWrite .. " - "
    end
    if #msg ~= 0 then
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            local pmsg = msg .. v.team.color .. " T : " .. v.team.name .. TextFormat.colorEnd
            MsgSender.sendBottomTipsToTarget(v.rakssid, 3, pmsg)
        end
    end
end

function CFinalFightTimeTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:endMatch()
end

FinalFightTimeTask = CFinalFightTimeTask.new(6)

return FinalFightTimeTask

--endregion
