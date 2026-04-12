---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 15:44
---
require "config.XCarConfig"

GameManager = {}
GameManager.Cars = {}

function GameManager:onTick(ticks)
    if GameMatch.IsGameOver == false then
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            player:OnTick(ticks)
        end
        for _, car in pairs(self.Cars) do
            if type(car) == 'table' then
                car:onTick(ticks)
            end
        end
    end
end

function GameManager:getNpcByEntityId(entityId)
    return nil
end

function GameManager:getCarByEntityId(entityId)
    return self.Cars[tostring(entityId)]
end

function GameManager:prepareGame()
    self:preparePlayers()
    self:RemovePracticeCar()
end

function GameManager:prepareCars()
    XCarConfig:prepareCars()
end

function GameManager:onAddCar(car)
    self.Cars[tostring(car.entityId)] = car
end

function GameManager:removeOtherCar(entityId)
    for id, car in pairs(self.Cars) do
        if id ~= tostring(entityId) then
            car:RemoveCar()
            car = nil
        end
    end
end

function GameManager:RemovePracticeCar()
    for _, car in pairs(self.Cars) do
        if type(car) == 'table' then
            car:RemovePracticeCar()
        end
    end
end

function GameManager:preparePlayers()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.bindTeam ~= nil then
            player:OnGameStart()
            for _, merchant in pairs(GameMatch.BindMerchants) do
                merchant:syncPlayer(player)
            end
        end
    end
end

function GameManager:getCars()
    return self.Cars
end

function GameManager:onPlayerRespawn(player)
    for _, car in pairs(self.Cars) do
        if type(car) == 'table' then
            car:onPlayerRespawn(player)
        end
    end
end

return GameManager