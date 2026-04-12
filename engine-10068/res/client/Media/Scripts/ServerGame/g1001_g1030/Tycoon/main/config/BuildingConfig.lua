---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 14:35
---

BuildingConfig = {}
BuildingConfig.buildingGroups = {}

function BuildingConfig:init(buildings)
    self:initBuildings(buildings)
end

function BuildingConfig:initBuildings(buildings)
    for id, c_item in pairs(buildings) do
        local buildings = {}
        local groupId = c_item.groupId
        if self.buildingGroups[groupId] ~= nil then
            buildings = self.buildingGroups[groupId]
        end
        local item = {}
        item.id = id
        item.startPos = VectorUtil.newVector3i(c_item.startX, c_item.startY, c_item.startZ)
        item.fileName = c_item.fileName
        item.value = tonumber(c_item.value)
        item.price = tonumber(c_item.price)
        item.npcActor = c_item.npcActor
        item.name = c_item.name
        item.npcPos = VectorUtil.newVector3(c_item.npcX, c_item.npcY, c_item.npcZ)
        item.npcYaw = tonumber(c_item.npcYaw)
        item.buildingIds = StringUtil.split(c_item.buildingIds, "#")
        item.rewardNum = tonumber(c_item.rewardNum)
        item.rewardMeta = tonumber(c_item.rewardMeta)
        item.rewardId = tonumber(c_item.rewardId)
        buildings[#buildings + 1] = item
        self.buildingGroups[groupId] = buildings
    end
end

function BuildingConfig:getBuildingsByGroupId(groupId)
    local buildings = self.buildingGroups[groupId]
    if buildings then
        return buildings
    end
    error("Building config error, can't find groupId:" .. groupId)
end

return BuildingConfig