---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 15:44
---
require "config.TowerConfig"
require "config.BasementConfig"
require "config.OccupationNpcConfig"
require "config.MineResourceConfig"
require "data.GameOccupation"

GameManager = {}
GameManager.towers = {}
GameManager.ruins = {}
GameManager.basements = {}
GameManager.mines = {}
GameManager.occupations = {}

function GameManager:onTick(ticks)
    for _, tower in pairs(self.towers) do
        tower:onTick(ticks)
    end
    for _, ruins in pairs(self.ruins) do
        ruins:onTick(ticks)
    end
    for _, basement in pairs(self.basements) do
        basement:onTick(ticks)
    end
end

function GameManager:getNpcByEntityId(entityId)
    if self:getTower(entityId) then
        return self:getTower(entityId)
    end
    if self:getRuins(entityId) then
        return self:getRuins(entityId)
    end
    if self:getBasements(entityId) then
        return self:getBasements(entityId)
    end
    return nil
end

function GameManager:getAttackNpc(entityId)
    if self:getTower(entityId) then
        return self:getTower(entityId)
    end
    if self:getBasements(entityId) then
        return self:getBasements(entityId)
    end
    return nil
end

function GameManager:prepareGame()
    self:prepareOccupation()
    self:prepareBasements()
    self:prepareTowers()
    self:updateBasementLife()
    MineResourceConfig:prepareMines()
end

function GameManager:prepareOccupation()
    for _, occupation in pairs(OccupationNpcConfig.npcs) do
        GameOccupation.new(occupation)
    end
end

function GameManager:prepareBasements()
    for i, basement in pairs(BasementConfig.basements) do
        GameBasement.new(basement)
    end
end

function GameManager:prepareTowers()
    for _, tower in pairs(TowerConfig.towers) do
        GameTower.new(tower)
    end
end

function GameManager:onAddTower(tower)
    self.towers[tostring(tower.entityId)] = tower
end

function GameManager:onRemoveTower(tower)
    self.towers[tostring(tower.entityId)] = nil
end

function GameManager:getTower(entityId)
    return self.towers[tostring(entityId)]
end

function GameManager:onAddRuins(ruins)
    self.ruins[tostring(ruins.entityId)] = ruins
end

function GameManager:onRemoveRuins(ruins)
    self.ruins[tostring(ruins.entityId)] = nil
end

function GameManager:getRuins(entityId)
    return self.ruins[tostring(entityId)]
end

function GameManager:onAddMineResource(mine)
    self.mines[#self.mines + 1] = mine
end

function GameManager:onBreakBlock(player, blockPos, blockId)
    for _, mine in pairs(self.mines) do
        mine:breakBlock(player, blockPos, blockId)
    end
end

function GameManager:onAddBasements(basements)
    self.basements[tostring(basements.entityId)] = basements
end

function GameManager:onRemoveBasements(basements)
    self.basements[tostring(basements.entityId)] = nil
end

function GameManager:getBasements(entityId)
    return self.basements[tostring(entityId)]
end

function GameManager:updateBasementLife()
    local resource = {}
    for _, basement in pairs(self.basements) do
        local index = #resource + 1
        resource[index] = {}
        resource[index].teamId = basement.teamId
        resource[index].value = basement.hp
        resource[index].maxValue = basement.maxhp
    end
    HostApi.updateTeamResources(json.encode(resource))
end

function GameManager:onGameEnd()
    self:clearTowers()
    self:clearRuins()
    self:clearBasements()
end

function GameManager:clearTowers(teamId)
    for _, tower in pairs(self.towers) do
        if teamId == nil or tower.teamId == teamId then
            tower:onClear()
        end
    end
end

function GameManager:clearRuins(teamId)
    for _, rurns in pairs(self.ruins) do
        if teamId == nil or rurns.teamId == teamId then
            rurns:onClear()
        end
    end
end

function GameManager:clearBasements()
    for _, basement in pairs(self.basements) do
        basement:onClear()
    end
end

return GameManager