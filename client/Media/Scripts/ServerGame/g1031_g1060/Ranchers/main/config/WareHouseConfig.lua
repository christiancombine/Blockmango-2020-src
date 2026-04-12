WareHouseConfig = {}
WareHouseConfig.wareHouse = {}
WareHouseConfig.wareHouseInitPos = {}
WareHouseConfig.wareHouseInitYaw = 0

function WareHouseConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.level = tonumber(v.level)
        data.capacity = tonumber(v.capacity)
        data.money = tonumber(v.money)
        data.moneyType = tonumber(v.moneyType)
        data.upgradeItems = StringUtil.split(v.upgradeItems, "#")
        data.actorId = tonumber(v.actorId)
        self.wareHouse[data.level] = data
    end
end

function WareHouseConfig:initWareHousePos(initPos)
    self.wareHouseInitPos = VectorUtil.newVector3(initPos[1], initPos[2], initPos[3])
    self.wareHouseInitYaw = initPos[4]
end

function WareHouseConfig:getWareHouseInfoByLevel(level)
    if self.wareHouse[level] ~= nil then
        return self.wareHouse[level]
    end

    return nil
end

function WareHouseConfig:getMaxLevel()
    return #self.wareHouse
end

function WareHouseConfig:getUpgradeMoneyByLevel(level)
    local wareHouseInfo = WareHouseConfig:getWareHouseInfoByLevel(level)
    if wareHouseInfo == nil then
        HostApi.log("=== LuaLog: [error] GameWareHouse:getUpgradeMoneyByLevel: can not find wareHouseInfo by level[".. level .."]")
        return 0
    end

    return wareHouseInfo.money
end

function WareHouseConfig:getUpgradeItemsByLevel(level)
    local wareHouseInfo = WareHouseConfig:getWareHouseInfoByLevel(level)
    if wareHouseInfo == nil then
        HostApi.log("=== LuaLog: [error] GameWareHouse:getUpgradeItemsByLevel: can not find wareHouseInfo by level[".. level .."]")
        return nil
    end

    local items = {}
    for i, v in pairs(wareHouseInfo.upgradeItems) do
        items[i] = {}
        local item = StringUtil.split(v, ":")
        items[i].itemId = tonumber(item[1])
        items[i].num = tonumber(item[2])
    end

    return items
end


function WareHouseConfig:newRanchWareHouseInfo(level, wareHouseItems)

    local ranchWareHouseInfo = RanchStorage.new()
    ranchWareHouseInfo.level = level
    ranchWareHouseInfo.maxLevel = WareHouseConfig:getMaxLevel()
    ranchWareHouseInfo.capacity = 0
    ranchWareHouseInfo.nextCapacity = 0
    ranchWareHouseInfo.upgradePrice = 0
    ranchWareHouseInfo.items = {}
    ranchWareHouseInfo.upgradeRecipe = {}

    local wareHouseInfo = self.wareHouse[level]
    if wareHouseInfo ~= nil then
        ranchWareHouseInfo.capacity = wareHouseInfo.capacity
        ranchWareHouseInfo.upgradePrice = wareHouseInfo.money
    end

    local nextWareHouseInfo = self.wareHouse[level + 1]
    if nextWareHouseInfo ~= nil then
        ranchWareHouseInfo.nextCapacity = nextWareHouseInfo.capacity
    end

    if wareHouseItems ~= nil then
        ranchWareHouseInfo.items = WareHouseConfig:newWareHouseItems(wareHouseItems)
    end

    local upgradeItems = WareHouseConfig:getUpgradeItemsByLevel(level)
    if upgradeItems ~= nil then
        ranchWareHouseInfo.upgradeRecipe = WareHouseConfig:newWareHouseItemRecipe(upgradeItems)
    end

    return ranchWareHouseInfo
end

function WareHouseConfig:newWareHouseItemRecipe(recipeItems)
    local items = {}
    local index = 0
    for i, v in pairs(recipeItems) do
        if tonumber(v.itemId) ~= 0 and tonumber(v.num) ~= 0 then
            local item = RanchCommon.new()
            item.itemId = tonumber(v.itemId)
            item.num = tonumber(v.num)
            item.price = 0
            for _, _item in pairs(ItemsConfig.items) do
                if _item.itemId == item.itemId then
                    item.price = _item.cubePrice
                end
            end
            index = index + 1
            items[index] = item
        end
    end

    return items
end

function WareHouseConfig:newWareHouseItems(wareHouseItems)
    local items = {}
    local index = 0
    for i, v in pairs(wareHouseItems) do
        if tonumber(i) ~= 0 and tonumber(v) ~= 0 then
            local item = RanchCommon.new()
            item.itemId = tonumber(i)
            item.num = tonumber(v)
            item.price = 0
            for _, _item in pairs(ItemsConfig.items) do
                if _item.itemId == item.itemId then
                    item.price = _item.price
                end
            end
            index = index + 1
            items[index] = item
        end
    end

    return items
end

return WareHouseConfig