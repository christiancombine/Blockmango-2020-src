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

    self:onGameTick(ticks)
    self:showGameData(seconds)
end

function CGameTimeTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGameTimeTask:onGameTick(ticks)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:onLifeTick(ticks)
    end
end

function CGameTimeTask:showGameData(seconds)
    local players = PlayerManager:getPlayerCount() .. "/" .. GameMatch.totalPlayers;
    local c_players = PlayerManager:getPlayers()
    for i, v in pairs(c_players) do
        MsgSender.sendBottomTipsToTarget(v.rakssid, 3, Messages:msgGameData(players, seconds, v.score))
    end
end

function CGameTimeTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:endMatch()
end

GameTimeTask = CGameTimeTask.new(3)

return GameTimeTask

--endregion
