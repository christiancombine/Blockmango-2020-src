---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 15:44
---
require "config.BasicConfig"
require "config.DomainConfig"
require "config.OccupationConfig"
require "config.PublicResourceConfig"
require "data.GameDomain"
require "data.GameOccupation"
require "data.GamePublicResource"
require "data.GameGetPublicResource"

GameManager = {}
GameManager.fastest = nil
GameManager.basicNum = 1
GameManager.isGoBackPortal = false
GameManager.isOpenExtraPortal = false
-- data
GameManager.publicResources = {}
GameManager.domains = {}
GameManager.occs = {}

GameManager.perpareResources = {}
GameManager.extraPortals = {}
GameManager.getPortals = {}

-- npc
GameManager.occupations = {}
GameManager.guards = {}
GameManager.gateBuilders = {}
GameManager.getResources = {}
GameManager.promoteResources = {}
GameManager.getEquipBoxs = {}
GameManager.equipBoxBuilders = {}
GameManager.portals = {}
GameManager.goBackPortals = {}
GameManager.buildings = {}

function GameManager:prepare()
    for id, occupation in pairs(OccupationConfig.occupations) do
        local domain = DomainConfig:getDomainByOccupationId(tonumber(occupation.id))
        if domain ~= nil then
            self.occs[id] = GameOccupation.new(domain, occupation.occPos, occupation.occYaw, occupation, occupation.occEffect)
        end
    end
    for id, publicResource in pairs(PublicResourceConfig.publicResources) do
        self.publicResources[id] = GamePublicResource.new(publicResource)
    end
end

function GameManager:start()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.occupationId == 0 then
            if self:selectPriorityOcc(player) then
                for _, occ in pairs(self.occs) do
                    if occ.curSelectNum < occ.selectedMaxNum then
                        occ:onPlayerSelected(player, false)
                        break
                    end
                end
            end
        end
    end
    for _, occ in pairs(self.occs) do
        for _, domain in pairs(occ.domains) do
            if self.basicNum <= BasicConfig.basicNum then
                self.domains[#self.domains + 1] = domain
                domain.basicId = self.basicNum
                domain:onCreate(domain.basicId)
                self.basicNum = self.basicNum + 1
            end
        end
        occ:onRemove()
    end
end

--检测是否可以选择具有优先级的职业（氪金购买的职业）
function GameManager:selectPriorityOcc(player)
    if #player.supOccIds > 0 then
        for _, occ in pairs(self.occs) do
            for _, id in pairs(player.supOccIds) do
                if tonumber(occ.config.id) == tonumber(id) then
                    if occ.curSelectNum < occ.selectedMaxNum then
                        occ:onPlayerSelected(player, false)
                        return false
                    end
                end
            end
        end
    end
    return true
end

function GameManager:onTick(ticks)
    for id, domain in pairs(self.domains) do
        domain:onTick(ticks)
        for _, equipbox in pairs(domain.equipboxs) do
            equipbox:onTick()
        end
    end

    for id, publicResource in pairs(self.publicResources) do
        publicResource:onTick(ticks)
    end

    for _, resource in pairs(self.perpareResources) do
        resource:onTick(ticks)
    end

    if #self.getPortals ~= 0 then
        for _, portal in pairs(self.getPortals) do
            portal:onTick(ticks)
        end
    end

    if GameManager.isOpenExtraPortal then
        for _, extraPortal in pairs(self.extraPortals) do
            extraPortal:onTick(ticks)
        end
    end
end

function GameManager:getDomains()
    return self.domains
end

function GameManager:getPublicResources()
    return self.publicResources
end

function GameManager:addPublicResources(addOutput, addMaxStore)
    if #self.publicResources ~= 0 then
        for id, publicResource in pairs(self.publicResources) do
            publicResource:promoteResource(addOutput, addMaxStore)
        end
    end
end

function GameManager:subPublicResources(subOutput, subMaxStore)
    if #self.publicResources ~= 0 then
        for id, publicResource in pairs(self.publicResources) do
            publicResource:reduceResource(subOutput, subMaxStore)
        end
    end
end

function GameManager:getNpcByEntityId(entityId)
    local npc = self:getOccupation(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getGuard(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getGateBuilder(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getGetResource(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getPromoteResource(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getGetEquipBox(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getEquipBoxBuilder(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getBuilding(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getPortal(entityId)
    if npc ~= nil then
        return npc
    end
    npc = self:getGoBackPortal(entityId)
    if npc ~= nil then
        return npc
    end
    return nil
end

function GameManager:onAddOccupation(occupation)
    self.occupations[tostring(occupation.entityId)] = occupation
end

function GameManager:onRemoveOccupation(occupation)
    self.occupations[tostring(occupation.entityId)] = nil
end

function GameManager:getOccupation(entityId)
    return self.occupations[tostring(entityId)]
end

function GameManager:onAddGetResource(resource)
    self.getResources[tostring(resource.entityId)] = resource
end

function GameManager:onRemoveGetResource(resource)
    self.getResources[tostring(resource.entityId)] = nil
end

function GameManager:getGetResource(entityId)
    return self.getResources[tostring(entityId)]
end

function GameManager:onAddPromoteResource(resource)
    self.promoteResources[tostring(resource.entityId)] = resource
end

function GameManager:onRemovePromoteResource(resource)
    self.promoteResources[tostring(resource.entityId)] = nil
end

function GameManager:getPromoteResource(entityId)
    return self.promoteResources[tostring(entityId)]
end

function GameManager:onAddGuard(guard)
    self.guards[tostring(guard.entityId)] = guard
end

function GameManager:onRemoveGuard(guard)
    self.guards[tostring(guard.entityId)] = nil
end

function GameManager:getGuard(entityId)
    return self.guards[tostring(entityId)]
end

function GameManager:onAddGateBuilder(builder)
    self.gateBuilders[tostring(builder.entityId)] = builder
end

function GameManager:onRemoveGateBuilder(builder)
    self.gateBuilders[tostring(builder.entityId)] = nil
end

function GameManager:getGateBuilder(entityId)
    return self.gateBuilders[tostring(entityId)]
end

function GameManager:onAddGetEquipBox(equipbox)
    self.getEquipBoxs[tostring(equipbox.entityId)] = equipbox
end

function GameManager:onRemoveGetEquipBox(equipbox)
    self.getEquipBoxs[tostring(equipbox.entityId)] = nil
end

function GameManager:getGetEquipBox(entityId)
    return self.getEquipBoxs[tostring(entityId)]
end

function GameManager:onAddEquipBoxBuilder(builder)
    self.equipBoxBuilders[tostring(builder.entityId)] = builder
end

function GameManager:onRemoveEquipBoxBuilder(builder)
    self.equipBoxBuilders[tostring(builder.entityId)] = nil
end

function GameManager:getEquipBoxBuilder(entityId)
    return self.equipBoxBuilders[tostring(entityId)]
end

function GameManager:onAddBuilding(building)
    self.buildings[tostring(building.entityId)] = building
end

function GameManager:onRemoveBuilding(building)
    self.buildings[tostring(building.entityId)] = nil
end

function GameManager:getBuilding(entityId)
    return self.buildings[tostring(entityId)]
end

function GameManager:onAddPortal(portal)
    self.portals[tostring(portal.entityId)] = portal
end

function GameManager:onRemovePortal(portal)
    self.portals[tostring(portal.entityId)] = nil
end

function GameManager:getPortal(entityId)
    return self.portals[tostring(entityId)]
end

function GameManager:onAddGoBackPortal(portal)
    self.goBackPortals[tostring(portal.entityId)] = portal
end

function GameManager:onRemoveGoBackPortal(portal)
    self.goBackPortals[tostring(portal.entityId)] = nil
end

function GameManager:getGoBackPortal(entityId)
    return self.goBackPortals[tostring(entityId)]
end

function GameManager:showFastestBuilding()
    local domains = {}
    for _, domain in pairs(self.domains) do
        domains[#domains + 1] = domain
    end
    table.sort(domains, function(a, b)
        if a:getBuildProgress() ~= b:getBuildProgress() then
            return a:getBuildProgress() > b:getBuildProgress()
        end
        if a:getBuildTime() ~= b:getBuildTime() then
            return a:getBuildTime() < b:getBuildTime()
        end
        return a.domainId < b.domainId
    end)
    if self.fastest == domains[1] and domains[1].needSync == false then
        return
    end
    self.fastest = domains[1]
    if self.fastest.player ~= nil then
        local name = self.fastest.player.name .. "-" .. self.fastest.player.occName
        HostApi.updateBuildProgress(name, self.fastest:getBuildProgress(), self.fastest:getMaxProgress())
    end
end

return GameManager