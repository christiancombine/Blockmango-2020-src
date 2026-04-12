ItemsConfig = {}
ItemsConfig.items = {}

ItemsConfig.DIAMOND_REFRESH_RANCH_ORDER = 1031001
ItemsConfig.DIAMOND_BUILDING_SPEED_UP = 1031002
ItemsConfig.DIAMOND_WAREHOUSE_UPGRADE = 1031003
ItemsConfig.DIAMOND_FINISH_HELP = 1031004

function ItemsConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.itemId = tonumber(v.itemId)
        data.price = tonumber(v.price)
        data.cubePrice = tonumber(v.cubePrice)
        data.exp = tonumber(v.exp)
        data.needTime = tonumber(v.needTime)
        data.level = tonumber(v.level)
        data.recipe = StringUtil.split(v.recipe, "#")
        self.items[data.itemId] = data
    end
end

function ItemsConfig:getItemInfoById(id)
    if self.items[id] ~= nil then
        return self.items[id]
    end

    return nil
end

function ItemsConfig:getRecipeItemsById(id)
    local items = {}
    if self.items[id] == nil then
        return items
    end

    for i, v in pairs(self.items[id].recipe) do
        local data = {}
        local item = StringUtil.split(v, ":")
        data.itemId = tonumber(item[1])
        data.num = tonumber(item[2])
        table.insert(items, data)
    end
    return items
end

function ItemsConfig:getNeedTimeByItemId(id)
    if self.items[id] == nil then
        return 0
    end

    return self.items[id].needTime
end

function ItemsConfig:getExpByItemId(id)
    if self.items[id] == nil then
        return 0
    end

    return self.items[id].exp
end

function ItemsConfig:getCubePriceByItemId(id)
    if self.items[id] == nil then
        return 0
    end

    return self.items[id].cubePrice
end

function ItemsConfig:getItemLevelByItemId(itemId)
    if self.items[itemId] ~= nil then
        return self.items[itemId].level
    end

    return 0
end

return ItemsConfig