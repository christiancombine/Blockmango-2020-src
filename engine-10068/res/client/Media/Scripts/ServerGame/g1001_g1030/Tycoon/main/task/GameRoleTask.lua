require "config.GameConfig"

local CGameRoleTask = class("GameRoleTask", ITask)

function CGameRoleTask:onTick(ticks)
    if (ticks >= GameConfig.selectRoleTime) then
        self:stop()
        return
    end

    local seconds = GameConfig.selectRoleTime - ticks

    if seconds == 12 then
        HostApi.sendStartGame(PlayerManager:getPlayerCount())
    end

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

    self:checkAllPlayersSelectedHero()
end

function CGameRoleTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGameRoleTask:stop()
    SecTimer.stopTimer(self.timer)
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    HostApi.sendGameStatus(0, 1)
    GameMatch:startMatch()
end

function CGameRoleTask:checkAllPlayersSelectedHero()
    local isAllSelected = true
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.occupationId == 0 then
            isAllSelected = false
        end
    end
    if isAllSelected == true then
        self:stop()
        return
    end
end

GameRoleTask = CGameRoleTask.new(GameMatch.Status.SelectRole)

return GameRoleTask
