---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 14:24
---
require "config.OccupationConfig"
require "config.GateConfig"
require "config.BuildingConfig"
require "config.EquipBoxConfig"
require "config.PrivateResourceConfig"
require "config.PortalConfig"

DomainConfig = {}
DomainConfig.domains = {}

function DomainConfig:init(domains)
    self:initDomains(domains)
end

function DomainConfig:initDomains(domains)
    for id, c_item in pairs(domains) do
        local domain = {}
        domain.id = id
        domain.area = {}
        domain.baseFileName = c_item.baseFileName
        domain.fileName = c_item.fileName
        domain.startPos = VectorUtil.newVector3i(c_item.startX, c_item.startY, c_item.startZ)
        domain.respwanPos = VectorUtil.newVector3(c_item.respwanX, c_item.respwanY, c_item.respwanZ)
        domain.cdTime = tonumber(c_item.cd)
        domain.healHp = tonumber(c_item.hp)
        domain.buildOne = tonumber(c_item.buildOne)
        domain.buildTwo = tonumber(c_item.buildTwo)
        local startArea = { tonumber(c_item.damainStartX), tonumber(c_item.damainStartY), tonumber(c_item.damainStartZ) }
        local endArea = { tonumber(c_item.damainEndX), tonumber(c_item.damainEndY), tonumber(c_item.damainEndZ) }
        table.insert(domain.area, startArea)
        table.insert(domain.area, endArea)
        domain.occupationId = tonumber(c_item.occupationId)
        domain.occupation = OccupationConfig:getOccupation(c_item.occupationId)
        domain.gate = GateConfig:getGate(c_item.gateId)
        domain.buildings = BuildingConfig:getBuildingsByGroupId(c_item.buildingGroupId)
        domain.equipboxs = EquipBoxConfig:getEquipBoxsByGroupId(c_item.equipGroupId)
        domain.privateResource = PrivateResourceConfig:getResourses(c_item.resourceId)
        domain.portal = PortalConfig:getPortalBuildingById(c_item.portalId)
        self.domains[id] = domain
    end
end

--得到总建筑的进度值
function DomainConfig:getMaxBuildValue()
    local domainValues = {}
    for i, domain in pairs(self.domains) do
        local index = #domainValues + 1
        local totalValue = 0
        for _, building in pairs(domain.buildings) do
            totalValue = totalValue + building.value
        end
        for _, equip in pairs(domain.equipboxs) do
            totalValue = totalValue + equip.value
        end
        totalValue = totalValue + domain.gate.value
        totalValue = totalValue + domain.portal.value

        domainValues[index] = totalValue
    end
    return math.min(unpack(domainValues))
end

function DomainConfig:getDomainByOccupationId(occupationId)
    for _, domain in pairs(self.domains) do
        if domain.occupationId == occupationId then
            return domain
        end
    end
    return nil
end

return DomainConfig