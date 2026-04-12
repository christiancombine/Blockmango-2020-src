
local CGameTimeTask = class("GameTimeTask", ITask)
CGameTimeTask.GameStage = 0

function CGameTimeTask:onTick(ticks)
    if (ticks >= GameConfig.gameTime) then
        self:stop()
    end

    if ticks == tonumber(GameConfig:getStatusTime(self.GameStage + 1)) then
        self.GameStage = self.GameStage + 1
        GameResourceManager:onGameStageChange(self.GameStage)
    end
    self:onGameTick(ticks)
    self:cannotPlaceBlock(ticks)
    GoldIngotPrivilege:onTick(ticks)
    AddHPGain:onTick(ticks)

    self.tick = ticks
    self.timerKey = 0

    BackHallManager:onTick()
    WorldInfo:onTick(ticks)
    BuffManager:onTick(ticks)
    GameMatch:checkInArea()
    for _, team in pairs(GameMatch.Teams) do
        team:getCurPlayerCount()
    end
end

function CGameTimeTask:cannotPlaceBlock(ticks)
    local list = GameMatch.cannotPlaceBlock

    if #list <= 0 then
        return
    end

    local i = 1
    while i <= #list do
        EngineWorld:setBlock(list[i], 0, 0, 3, true)
        table.remove(list, i)
    end
end

function CGameTimeTask:onGameTick(ticks)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife or player.realLife then

            if GameConfig.autoAddHp[1].countDown > 0 and ticks % GameConfig.autoAddHp[1].countDown == 0 and not player.banCureBuff then
                player:addHealth(GameConfig.autoAddHp[1].addHpValue)
            end

            RuneManager:onTick(player, ticks)

            player.isRespawn = false
        end
    end
end

function CGameTimeTask:stop()
    SecTimer.stopTimer(self.timer)
    GameMatch:endMatch()
end

function CGameTimeTask:onCreate()
    GameMatch.curStatus = self.status
    WorldInfo:init()
    GameResourceManager:onGameStart()
end

GameTimeTask = CGameTimeTask.new(3)

return GameTimeTask

--endregion
