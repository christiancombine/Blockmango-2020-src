
ChestConfig = {}

ChestConfig.chest = {}

ChestConfig.inventory = {}

function ChestConfig:init(config)
    self:initChest(config.chest)
    self:initInventory(config.inventory)
end

function ChestConfig:initChest(config)
    for i, v in pairs(config) do
        local chest = {}
        chest.name = tonumber(v.name)
        chest.itemId = tonumber(v.itemId) or "54"
        chest.initPos = VectorUtil.newVector3i(v.initPos[1], v.initPos[2], v.initPos[3])
        chest.yaw = tonumber(v.initPos[4])
        chest.resetTime = {}
        chest.resetTime[1] = v.resetTime[1]
        chest.resetTime[2] = v.resetTime[2]
        chest.threat = tonumber(v.threat)
        chest.merit = tonumber(v.merit)

        chest.type = {}
        for j, type in pairs(v.type) do
            if type ~= nil then
                chest.type[j] = tostring(type)
            end
        end

        self.chest[VectorUtil.hashVector3(chest.initPos)] = chest
    end
end

function ChestConfig:initInventory(config)
    for key, inventory in pairs(config) do
        local inv = {}
        inv.range = {}
        inv.range[1] = tonumber(inventory.range[1])
        inv.range[2] = tonumber(inventory.range[2])
        inv.items = {}
        for index, item in pairs(inventory.items) do
            local res = {}
            res.id = tonumber(item.id)
            res.randomRange = {}
            res.randomRange[1] = tonumber(item.randomRange[1])
            res.randomRange[2] = tonumber(item.randomRange[2])
            res.probability = tonumber(item.probability)
            inv.items[index] = res
        end
        self.inventory[tostring(key)] = inv
    end
end

function ChestConfig:fillItem(inventory, types)
    local nextSlot = 0
    for i, type in pairs(types) do
        for key, inv in pairs(self.inventory) do
            if type == key then
                nextSlot = self:fillItemByType(inventory, nextSlot, inv.items, inv.range[1], inv.range[2])
            end
        end
    end
end

function ChestConfig:fillItemByType(inv, slotBegin, config, rangeLow, rangeUpper)
    local itemSlot = slotBegin
    local items = InventoryUtil:randomItem(config, rangeLow, rangeUpper)
    for i, v in pairs(items) do
        if v.num ~= 0 then
            inv:initByItem(itemSlot, v.id, v.num, 0)
            itemSlot =  itemSlot + 1
        end
    end
    return itemSlot
end

function ChestConfig:prepareChest()
    for i, v in pairs(self.chest) do
        EngineWorld:setBlock(v.initPos, v.itemId, v.yaw)
    end
end

function ChestConfig:getChest(vec3)
    return self.chest[VectorUtil.hashVector3(vec3)]
end

return ChestConfig