---
--- Created by Jimmy.
--- DateTime: 2018/8/28 0028 10:40
---
OccupationConfig = {}
OccupationConfig.occupations = {}

function OccupationConfig:init(occupations)
    self:initOccupations(occupations)
end

function OccupationConfig:initOccupations(occupations)
    for _, occupation in pairs(occupations) do
        local item = {}
        item.index = occupation.index
        item.id = occupation.id
        item.level = tonumber(occupation.level)
        item.name = occupation.name
        item.content = occupation.content
        item.image = occupation.image
        item.displayName = occupation.displayName
        item.inventory = InventoryConfig:getInventory(occupation.inventoryId)
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
        item.speed = tonumber(occupation.speed)
        item.health = tonumber(occupation.health)
        item.attack = tonumber(occupation.attack)
        item.defense = tonumber(occupation.defense)
        item.defenseX = tonumber(occupation.defenseX)
        item.price = tonumber(occupation.price)
        item.togglePrice = tonumber(occupation.togglePrice)
        item.goldOfKill = tonumber(occupation.goldOfKill)
        item.goldOfCompensation = tonumber(occupation.goldOfCompensation)
        item.itemIdOfKill = tonumber(occupation.itemIdOfKill)
        item.itemNumOfKill = tonumber(occupation.itemNumOfKill)
        item.type = occupation.type
        item.regeneratesHP=tonumber(occupation.regeneratesHP)
        self.occupations[#self.occupations + 1] = item
    end
    table.sort(self.occupations, function(a, b)
        return a.index < b.index
    end)
end

function OccupationConfig:initNormalOccupation()
    local occupations = {}
    for _, occupation in pairs(self.occupations) do
        if occupation.level == 0 then
            occupations[occupation.id] = 0
        end
    end
    return occupations
end

function OccupationConfig:getOccupation(occupationId, level)
    for _, occupation in pairs(self.occupations) do
        if occupation.id == occupationId and occupation.level == level then
            return occupation
        end
    end
    return nil
end

return OccupationConfig