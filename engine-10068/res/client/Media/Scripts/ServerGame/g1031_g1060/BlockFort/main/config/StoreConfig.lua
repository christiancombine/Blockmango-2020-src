StoreConfig = {}
StoreConfig.allItems = {}
StoreConfig.allPrice = {}

BuildingType = {
    Fort = 1,
    Turret = 2,
    Wall = 3,
}

MoneyType = {
    Cube = 1,
    Gold = 2,
}

InStore = {
    Yes = 1,
    No = 0,
}

function StoreConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.weight = tonumber(v.weight)
        data.id = tonumber(v.id)
        data.itemId = tonumber(v.itemId)
        data.level = tonumber(v.level)
        data.length = tonumber(v.length)
        data.width = tonumber(v.width)
        data.height = tonumber(v.height)
        data.fileName = (v.fileName ~= "@@@" and {v.fileName} or {""})[1]
        data.meta = (v.meta and {v.meta} or {0})[1]
        data.type = tonumber(v.type)
        data.limit = tonumber(v.limit)
        data.groupId = tonumber(v.groupId)
        data.exp = tonumber(v.exp)
        data.moneyType = tonumber(v.moneyType)
        data.inStore = tonumber(v.inStore)
        self.allItems[data.id] = data
    end
end

function StoreConfig:initPrice(config)
    for _, v in pairs(config) do
        local id = tonumber(v.Id)
        local price = tonumber(v.Price)
        local cube = tonumber(v.Cube)
        self.allPrice[id .. ":" .. MoneyType.Gold] = price
        self.allPrice[id .. ":" .. MoneyType.Cube] = cube
    end
end

function StoreConfig:getItemInfoById(id)
    id = tonumber(id)
    return self.allItems[id]
end

function StoreConfig:getItemPriceById(id, moneyType)
    id = tonumber(id)
    moneyType = tonumber(moneyType)
    return self.allPrice[id .. ":" .. moneyType]
end

function StoreConfig:getItemInfoByItemId(itemId)
    itemId = tonumber(itemId)
    for _, v in pairs(self.allItems) do
        if itemId == v.itemId then
            return v
        end
    end
    return nil
end

function StoreConfig:getIdsByLevel(level)
    local data = {}
    level = tonumber(level)
    for _, v in pairs(self.allItems) do
        if v.level == level then
            table.insert(data, v.id)
        end
    end
    return data
end

return StoreConfig