--region *.lua
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
        MsgSender.sendBottomTips(3, IMessages:msgTeleportToDeadPlace(seconds))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    --self:showGameData()

end

function CGameTimeTask:showGameData()
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

function CGameTimeTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGameTimeTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:startFinalFight()
end

GameTimeTask = CGameTimeTask.new(4)

return GameTimeTask

--endregion
