--region *.lua

local CGameTimeTask = class("GameTimeTask", ITask)

function CGameTimeTask:onTick(ticks)
    --local time1 = os.clock()
    WorldTimeManager:onTick()
    MonsterManager:onTick(ticks)
    BuffManager:onTick(ticks)
    ActorManager:onTick(ticks)
    SupplyManager:onTick(ticks)
    RobotManager:onTick(ticks)
    MusicBoxManager:onTick(ticks)
    MerchantManager:onTick(ticks)
    HangUpManager:onTick(ticks)
    ChestManager:onTick(ticks)

    for i, vPlayer in pairs(PlayerManager:getPlayers()) do
        vPlayer:checkPlayerAttribute()
    end
    if ticks % 60 == 0 then
        GameTitle:updateSurvivalTimeTitle()
    end
    if ticks % 5 == 0 then
        local players = PlayerManager:getPlayers()
        for i, player in pairs(players) do
            player:syncVipInfo()
            GameStore:syncSupplyData(player)
        end
    end

    for i, vPlayer in pairs(PlayerManager:getPlayers()) do
        vPlayer:addLifeTime()
    end
    --GameMatch:gameOver()
    --HostApi.log(string.format("CGameTimeTask:onTick %f", (os.clock() - time1)))
end

function CGameTimeTask:onCreate()
    GameMatch.curStatus = self.status
end

function CGameTimeTask:stop()
    SecTimer.stopTimer(self.timer)
end

GameTimeTask = CGameTimeTask.new(4)

return GameTimeTask

--endregion
