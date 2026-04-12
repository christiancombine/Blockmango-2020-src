require "config.Area"

AirSupportConfig = {}
AirSupportConfig.airSupport = {}
AirSupportConfig.inventory = {}
AirSupportConfig.support = {}
AirSupportConfig.airSupportPosition = {}

function AirSupportConfig:init(config)
    self:initAirSupport(config.airSupport)
    self:initInventory(config.inventory)
end

function AirSupportConfig:initAirSupport(airSupport)
    self.airSupport.height = {}
    self.airSupport.type = {}
    self.airSupport.pos = {}
    self.airSupport.height[1] = airSupport.height[1]
    self.airSupport.height[2] = airSupport.height[2]
    for i, v in pairs(airSupport.type) do
        self.airSupport.type[i] = tostring(v)
    end
end

function AirSupportConfig:initInventory(config)
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
            if item.extraItems ~= nil then
                res.extraItems = {}
                for i, v in pairs(item.extraItems) do
                    local extra = {}
                    extra.id = tonumber(v.id)
                    extra.num = tonumber(v.num)
                    res.extraItems[i] = extra
                end
            end
            inv.items[index] = res
        end
        self.inventory[tostring(key)] = inv
    end
end

function AirSupportConfig:fillItem(inventory, types)
    local nextSlot = 0
    for i, type in pairs(types) do
        for key, inv in pairs(self.inventory) do
            if type == key then
                nextSlot = self:fillItemByType(inventory, nextSlot, inv.items, inv.range[1], inv.range[2])
            end
        end
    end
end

function AirSupportConfig:fillItemByType(inv, slotBegin, config, rangeLow, rangeUpper)
    local itemSlot = slotBegin
    local items = InventoryUtil:randomItem(config, rangeLow, rangeUpper)
    for i, item in pairs(items) do
        if item.num ~= 0 then
            inv:initByItem(itemSlot, item.id, item.num, 0)
            itemSlot = itemSlot + 1
        end
        if item.extraItems ~= nil then
            for j, extraItem in pairs(item.extraItems) do
                if extraItem.num ~= 0 then
                    inv:initByItem(itemSlot, extraItem.id, extraItem.num, 0)
                    itemSlot = itemSlot + 1
                end
            end
        end
    end
    return itemSlot
end

function AirSupportConfig:fillAirSupport(vec3, inv)
    local sign = VectorUtil.hashVector3(vec3)
    if self.support[sign] == nil and self.airSupport.pos[sign] ~= nil then
        inv:clearChest()
        self:fillItem(inv, self.airSupport.type)
        self.support[sign] = true
        self.airSupport.pos[sign] = nil
    end
end

function AirSupportConfig:prepareAirSupport(vec3)
    local x = tonumber(vec3.x)
    local z = tonumber(vec3.z)
    local height = self.airSupport.height
    local min = math.min(height[1], height[2])
    local max = math.max(height[1], height[2])
    for y = max, min, -1 do
        local id = EngineWorld:getBlockId(VectorUtil.newVector3i(x, y, z))
        if id ~= 0 then
            local pos = VectorUtil.newVector3i(x, y + 1, z)
            EngineWorld:setBlock(pos, BlockID.CHEST)
            self.airSupport.pos[VectorUtil.hashVector3(pos)] = true
            return pos
        end
    end
end

return AirSupportConfig