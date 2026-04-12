GameAccelerateItems = class()

function GameAccelerateItems:ctor()
    self.items = {}
    self.isEmpty = false
end

function GameAccelerateItems:initItems()
    for i, v in pairs(AccelerateItemsConfig.items) do
        local data = {}
        data.itemId = v.itemId
        data.num = v.num
        self.items[data.itemId] = data
    end
end

function GameAccelerateItems:initDataFromDB(data)
    self:initAccelerateItemsFromDB(data.accelerateItems or {})
end

function GameAccelerateItems:initAccelerateItemsFromDB(items)
    local count = 0
    for i, v in pairs(items) do
        local data = {}
        data.itemId = v.itemId
        data.num = v.num
        count = count + 1
        self.items[data.itemId] = data
    end

    if count == 0 then
        self.isEmpty = true
    end
end

function GameAccelerateItems:prepareDataSaveToDB()
    local data = {
        accelerateItems = self:prepareAccelerateItemsToDB()
    }

    --HostApi.log("GameAccelerateItems:prepareDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GameAccelerateItems:prepareAccelerateItemsToDB()
    local items = {}
    for i, v in pairs(self.items) do
        local data = {}
        data.itemId = v.itemId
        data.num = v.num
        table.insert(items, data)
    end

    return items
end

function GameAccelerateItems:useFreeAccelerateByItemId(itemId)
    for i, v in pairs(self.items) do
        if v.itemId == itemId and v.num > 0 then
            v.num = v.num - 1
            if v.num <= 0 then
                self.items[itemId] = nil
            end
            return true
        end
    end

    return false
end

function GameAccelerateItems:initRanchShortcutFree()
    local items = {}
    local index = 1
    for i, v in pairs(self.items) do
        local data = {}
        data[1] = v.itemId
        data[2] = v.num
        items[index] = data
        index = index + 1
    end
    return items
end