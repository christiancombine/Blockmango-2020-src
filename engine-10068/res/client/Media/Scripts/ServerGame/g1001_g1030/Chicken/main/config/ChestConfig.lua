require "config.WeaponConfig"

ChestConfig = {}

ChestConfig.chest = {}
ChestConfig.chestPos = {}
ChestConfig.inventory = {}
ChestConfig.playerChestCache = {}


function ChestConfig:init(config)
    self:initChest(config.chest)
    self:initInventory(config.inventory)
end

function ChestConfig:initChest(config)
    for i, v in pairs(config) do
        local chest = {}
        chest.initPos = VectorUtil.newVector3i(v.initPos[1], v.initPos[2], v.initPos[3])
        chest.yaw = tonumber(v.initPos[4])
        chest.type = {}
        for j, type in pairs(v.type) do
            if type ~= nil then
                chest.type[j] = tostring(type)
            end
        end
        chest.itemId = tonumber(v.itemId) or "54"

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

function ChestConfig:fillChest(vec3, inv)
    local sign = VectorUtil.hashVector3(vec3)
    if self.chestPos[sign] == nil and self.chest[sign] ~= nil then
        inv:clearChest()
        self:fillItem(inv, self.chest[sign].type)
        self.chestPos[sign] = true
        self.chest[sign] = nil
    end
end

function ChestConfig:prepareChest()
    for i, v in pairs(self.chest) do
        EngineWorld:setBlock(v.initPos, v.itemId, v.yaw)
    end
end

function ChestConfig:getChest(vec3)
    return self.chest[VectorUtil.hashVector3(vec3)]
end

function ChestConfig:setPlayerChest(vec3, inv)
    local items = inv.item
    local cache = {}
    if items ~= nil then
        cache.items = {}
        for i, item in pairs(items) do
            cache.items[i] = item
        end
    end
    local armors = inv.armor
    if armors ~= nil then
        cache.armors = {}
        for i, item in pairs(armors) do
            cache.armors[i] = item
        end
    end
    local guns = inv.gun
    if guns ~= nil then
        cache.guns = {}
        for i, item in pairs(guns) do
           cache.guns[i] = item
        end
    end

    self.playerChestCache[VectorUtil.hashVector3(vec3)] = cache
end

function ChestConfig:fillPlayerChest(vec3, inv)
    local cache = self.playerChestCache[VectorUtil.hashVector3(vec3)]
    if cache ~= nil then
        local index = 0
        if cache.items ~= nil then
            for i, v in pairs(cache.items) do
                if v.num ~= 0 then
                    inv:initByItem(index, v.id, v.num, 0)
                    index = index + 1
                end
            end
        end

        if cache.armors ~= nil then
            for i, v in pairs(cache.armors) do
                if v.num ~= 0 then
                    inv:initByItem(index, v.id, v.num, 0)
                    index = index + 1
                end
            end
        end

        if cache.guns ~= nil then
            for i, v in pairs(cache.guns) do
                --inv:initByGunItem(index, v.id, v.num, v.meta, v.bullets)
                if v.num ~= 0 then
                    inv:initByItem(index, v.id, v.num, 0)
                    index = index + 1
                end
                if v.bullets ~= 0 then
                    local bulletId = WeaponConfig:getBulletIdByWeapon(v.id)
                    if v.bullets ~= 0 then
                        inv:initByItem(index, bulletId, v.bullets, 0)
                        index = index + 1
                    end
                end
            end
        end

        self.playerChestCache[VectorUtil.hashVector3(vec3)] = nil
    end
end

return ChestConfig