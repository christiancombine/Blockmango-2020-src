---
--- Created by Jimmy.
--- DateTime: 2018/6/7 0007 11:27
---
require "config.InventoryConfig"

OccupationConfig = {}
OccupationConfig.occupations = {}
OccupationConfig.unlockOccupationInfo = {}

function OccupationConfig:init(occupations)
    self:initOccupations(occupations)
end

function OccupationConfig:initOccupations(occupations)
    for id, occupation in pairs(occupations) do
        local item = {}
        item.id = id
        item.actor = occupation.actor
        item.name = occupation.name
        item.displayName = occupation.displayName
        item.inventory = InventoryConfig:getInventory(occupation.inventoryId)
        item.hint = occupation.hint
        local actors = {}
        local c_actors = StringUtil.split(occupation.actors, "#")
        for i, actor in pairs(c_actors) do
            local c_actor = StringUtil.split(actor, "=")
            local a_item = {}
            a_item.name = c_actor[1]
            a_item.id = c_actor[2]
            actors[#actors + 1] = a_item
        end
        item.actors = actors
        item.occPos = VectorUtil.newVector3(occupation.occX, occupation.occY, occupation.occZ)
        item.occYaw = tonumber(occupation.occYaw)
        item.occEffect = occupation.occEffect
        item.health = tonumber(occupation.health)
        item.defense = tonumber(occupation.defense)
        item.damage = tonumber(occupation.damage)
        item.attackX = tonumber(occupation.attackX)
        item.stealPercent = tonumber(occupation.stealPercent)
        item.itemId = tonumber(occupation.itemId)
        item.moneyItemId = tonumber(occupation.moneItemId)
        item.stayAtHomeEffects = StringUtil.split(occupation.stayAtHomeEffects, "#")
        self.occupations[item.id] = item
    end
end

function OccupationConfig:initUnlockOccupationInfo(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.singleCurrencyType = tonumber(v.singleCurrencyType)
        data.singlePrice = tonumber(v.singlePrice)
        data.perpetualCurrencyType = tonumber(v.perpetualCurrencyType)
        data.perpetualPrice = tonumber(v.perpetualPrice)
        data.radarIcon = v.radarIcon
        data.orientations1 = v.orientations1
        data.orientations2 = v.orientations2
        data.orientations3 = v.orientations3
        table.insert(self.unlockOccupationInfo, data)
    end
end

function OccupationConfig:getOccupation(occupationId)
    local occupation = self.occupations[occupationId]
    if occupation then
        return occupation
    end
    error("Occupation config error, can't find occupationId:" .. occupationId)
end

function OccupationConfig:getOccupationIdByCommonItemId(itemId)
    for _, occ in pairs(self.occupations) do
        if tonumber(occ.itemId) == tonumber(itemId) then
            return occ.id
        end
    end
    return 0
end

function OccupationConfig:getOccupationIdByMoneyItemId(itemId)
    for _, occ in pairs(self.occupations) do
        if tonumber(occ.moneyItemId) == tonumber(itemId) then
            return occ.id
        end
    end
    return 0
end

function OccupationConfig:isOccupationId(itemId)
    for _, occ in pairs(self.occupations) do
        if tonumber(occ.itemId) == tonumber(itemId) then
            return true
        end
        if tonumber(occ.moneyItemId) == tonumber(itemId) then
            return true
        end
    end

    return false
end

function OccupationConfig:getUnlockOccupationInfoById(id)
    for i, v in pairs(self.unlockOccupationInfo) do
        if tostring(v.id) == tostring(id) then
            return v
        end
    end

    return nil
end

return OccupationConfig