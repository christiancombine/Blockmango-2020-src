---
--- Created by Jimmy.
--- DateTime: 2018/6/7 0007 11:27
---
require "config.InventoryConfig"
require "data.GameOccupation"

OccupationConfig = {}
OccupationConfig.occupations = {}
OccupationConfig.occNpcs = {}

function OccupationConfig:init(occupations)
    self:initOccupations(occupations)
end

function OccupationConfig:initOccupations(occupations)
    for id, occupation in pairs(occupations) do
        local item = {}
        item.id = tonumber(occupation.id)
        item.name = occupation.name
        item.actor = occupation.actor
        item.displayName = occupation.displayName
        item.initPos = VectorUtil.newVector3(occupation.x, occupation.y, occupation.z)
        item.yaw = tonumber(occupation.yaw)
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
        item.health = tonumber(occupation.health)
        item.defense = tonumber(occupation.defense)
        item.damage = tonumber(occupation.damage)
        item.attackX = tonumber(occupation.attackX)
        item.isGetFlagBuff = tonumber(occupation.isGetFlagBuff or "0") == 1
        self.occupations[item.id] = item
    end
    self:prepareOccupation()
end

function OccupationConfig:prepareOccupation()
    for occupationId, occupation in pairs(self.occupations) do
        GameOccupation.new(occupation)
    end
end

function OccupationConfig:onAddOccNpc(occNpc)
    self.occNpcs[tostring(occNpc.entityId)] = occNpc
end

function OccupationConfig:getOccupationNpc(entityId)
    return self.occNpcs[tostring(entityId)]
end

function OccupationConfig:getSwordsmanNpc()
    for i, npc in pairs(self.occNpcs) do
        if npc.occupationId == 1 then
            return npc
        end
    end
    for i, npc in pairs(self.occNpcs) do
        return npc
    end
end

return OccupationConfig