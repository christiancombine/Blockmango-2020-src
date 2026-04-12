HouseConfig = {}
HouseConfig.house = {}
HouseConfig.houseInitPos = {}

function HouseConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.level = tonumber(v.level)
        data.itemId = tonumber(v.itemId)
        data.money = tonumber(v.money)
        data.name = v.name
        data.icon = v.icon
        data.upgradeItems = StringUtil.split(v.upgradeItems, "#")
        data.fileName = v.fileName
        data.playerLevel = tonumber(v.playerLevel)
        data.length = tonumber(v.length)
        data.width = tonumber(v.width)
        data.height = tonumber(v.height)
        data.npcId = tonumber(v.npcId)
        data.npcPosX = tonumber(v.npcPosX)
        data.npcPosY = tonumber(v.npcPosY)
        data.npcPosZ = tonumber(v.npcPosZ)
        data.npcYaw = tonumber(v.npcYaw)
        self.house[data.level] = data
    end
end

function HouseConfig:initHousePos(initPos)
    self.houseInitPos = VectorUtil.newVector3i(initPos[1], initPos[2], initPos[3])
end

function HouseConfig:getHouseInfoByLevel(level)
    if self.house[level] == nil then
       return nil
    end

    return self.house[level]
end

function HouseConfig:getMaxHouseLevel()
    return #self.house
end

function HouseConfig:getUpgradeItemsByLevel(level)
    local houseInfo = self:getHouseInfoByLevel(level)
    if houseInfo == nil then
        HostApi.log("=== LuaLog: [error] HouseConfig:getUpgradeItemsByLevel : can not find houseInfo by level[" .. level .. "]")
        return nil
    end

    local items = {}
    for i, v in pairs(houseInfo.upgradeItems) do
        items[i] = {}
        local item = StringUtil.split(v, ":")
        items[i].itemId = tonumber(item[1])
        items[i].num = tonumber(item[2])
    end

    return items
end

function HouseConfig:getUpgradeMoneyByLevel(level)
    local houseInfo = self:getHouseInfoByLevel(level)
    if houseInfo == nil then
        HostApi.log("=== LuaLog: [error] HouseConfig:getHouseUpgradeMoneyByLevel : can not find houseInfo by level[" .. level .. "]")
        return 0
    end

    return houseInfo.money
end

function HouseConfig:getPlayerLevelByLevel(level)
    if self.house[level] == nil then
        return 0
    end

    return self.house[level].playerLevel
end


function HouseConfig:newRanchHouseInfo(level)

    local ranchHouseInfo = RanchHouse.new()
    ranchHouseInfo.id = level
    ranchHouseInfo.level = level
    ranchHouseInfo.needPlayerLevel = HouseConfig:getPlayerLevelByLevel(level)
    ranchHouseInfo.maxLevel = HouseConfig:getMaxHouseLevel()
    ranchHouseInfo.nextLevel = level

    ranchHouseInfo.icon = ""
    ranchHouseInfo.name = ""
    ranchHouseInfo.nextIcon = ""
    ranchHouseInfo.nextName = ""
    ranchHouseInfo.upgradePrice = 0
    ranchHouseInfo.upgradeRecipe = {}

    if level < self:getMaxHouseLevel() then
        ranchHouseInfo.nextLevel = level + 1
    end

    local info = self:getHouseInfoByLevel(level)
    if info ~= nil then
        ranchHouseInfo.upgradePrice = info.money
        ranchHouseInfo.icon = info.icon
        ranchHouseInfo.name = info.name
    end

    local nextInfo = self:getHouseInfoByLevel(level + 1)
    if nextInfo ~= nil then
        ranchHouseInfo.nextIcon = nextInfo.icon
        ranchHouseInfo.nextName = nextInfo.name
    end

    local recipeItems = HouseConfig:getUpgradeItemsByLevel(level)
    if recipeItems ~= nil then
        ranchHouseInfo.upgradeRecipe = HouseConfig:newHouseItemRecipe(recipeItems)
    end

    return ranchHouseInfo
end

function HouseConfig:newHouseItemRecipe(recipeItems)
    local items = {}
    for i, v in pairs(recipeItems) do
        local item = RanchCommon.new()
        item.itemId = tonumber(v.itemId)
        item.num = tonumber(v.num)
        item.price = 0
        for _, _item in pairs(ItemsConfig.items) do
            if _item.itemId == item.itemId then
                item.price = _item.cubePrice
            end
        end
        items[i] = item
    end

    return items
end

return HouseConfig