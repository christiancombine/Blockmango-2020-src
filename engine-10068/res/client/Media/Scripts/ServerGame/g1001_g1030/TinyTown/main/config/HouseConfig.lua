
HouseConfig = {}
HouseConfig.house = {}

function HouseConfig:init(config)
    self:initHouse(config.house)
end

function HouseConfig:initHouse(config)
    for i, v in pairs(config) do
        local house = {}
        house.id = tonumber(v.id)
        house.icon = tostring(v.icon)
        house.fileName = tostring(v.fileName)
        house.cdTime = tonumber(v.cdTime)
        house.orientation = tonumber(v.orientation)
        house.yardLevel = tonumber(v.yardLevel)
        house.xLength = tonumber(v.xLength)
        house.yLength = tonumber(v.yLength)
        house.zLength = tonumber(v.zLength)
        house.xOffset = tonumber(v.xOffset)
        house.yOffset = tonumber(v.yOffset)
        house.zOffset = tonumber(v.zOffset)
        house.coinId = tonumber(v.price[1])
        house.blockymodsPrice = tonumber(v.price[2])
        house.blockmanPrice = tonumber(v.price[3])
        house.charmValue = tonumber(v.charmValue)
        house.repairDeep = tonumber(v.repairDeep)
        self.house[i] = house
    end
end

function HouseConfig:getHouseByOrientation(orientation)
    local orientationHouse = {}
    local index = 0
    for i, v in pairs(self.house) do
        if v.orientation == orientation then
            index = index + 1
            orientationHouse[index] = v
        end
    end
    return orientationHouse
end

function HouseConfig:isCanUnlock(id, player)
    for i, v in pairs(self.house) do
        if id == v.id and player.yardLevel >= v.yardLevel then
            return true
        end
    end
    return false
end

function HouseConfig:getHouseById(id)
    for i, v in pairs(self.house) do
        if v.id == id then
            return v
        end
    end
    return nil
end

function HouseConfig:newManorHouse(player, manorIndex)
    local houses = {}
    local orientation = 1
    if manorIndex > 5 then
        orientation = 2
    end

    local orientationHouse = self:getHouseByOrientation(orientation)

    for i, v in pairs(orientationHouse) do
        local house = ManorHouse.new()
        house.price = v.blockymodsPrice
        house.id = v.id
        house.unlockLevel = v.yardLevel
        house.cdTime = v.cdTime
        house.icon = v.icon
        house.name = v.fileName
        house.charm = v.charmValue
        house.isBuy = false
        for j, h in pairs(player.unlockHouse) do
            if j == v.id then
                house.isBuy = true
            end
        end
        house.currencyType = v.coinId
        houses[i] = house
    end

    return houses
end

function HouseConfig:getFileNameAndPosition(manorIndex, currentHouseId)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor == nil then
        return nil, nil
    end

    local house = self:getHouseById(currentHouseId)
    if house == nil then
        return nil, nil
    end

    local x = manor.landPosition.minX  + house.xOffset
    local y = manor.landPosition.minY  + house.yOffset
    local z = manor.landPosition.minZ  + house.zOffset

    local filePath = house.fileName
    local startPosition = VectorUtil.newVector3i(x, y, z)
    return filePath, startPosition
end

function HouseConfig:getRepairPosition(manorIndex, currentHouseId)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor == nil then
        return nil, nil
    end

    local house = self:getHouseById(currentHouseId)
    if house == nil then
        return nil, nil
    end

    local startX = manor.landPosition.minX + house.xOffset
    local startY = manor.landPosition.minY
    local startZ = manor.landPosition.minZ + house.zOffset

    local endX = startX + house.xLength
    local endY = startY - house.repairDeep
    local endZ = startZ + house.zLength

    local startPosition = VectorUtil.newVector3i(startX, startY, startZ)
    local endPosition = VectorUtil.newVector3i(endX, endY, endZ)

    return startPosition, endPosition
end

function HouseConfig:buildHouse(manorIndex, currentHouseId)
    local filePath, startPosition = HouseConfig:getFileNameAndPosition(manorIndex, currentHouseId)
    EngineWorld:createSchematic(filePath, startPosition)
    HostApi.log("==== LuaLog: HouseConfig:buildHouse successful")
end

function HouseConfig:destroyHouse(manorIndex, currentHouseId)
    local filePath, startPosition = HouseConfig:getFileNameAndPosition(manorIndex, currentHouseId)
    if filePath ~= nil and startPosition ~= nil then
        EngineWorld:destroySchematic(filePath, startPosition)
    else
        HostApi.log("==== LuaLog: HouseConfig:destroyHouse: [filePath = nil or startPosition = nil]")
    end
end

function HouseConfig:repairLandBeforeBuild(manorIndex, currentHouseId)
    local startPosition, endPosition = HouseConfig:getRepairPosition(manorIndex, currentHouseId)
    if startPosition ~= nil and endPosition ~= nil then
        local repairItemId = GameConfig.repairItemId
        local repairMeta = GameConfig.repairItemMeta
        EngineWorld:getWorld():fillAreaByBlockIdAndMate(startPosition, endPosition, repairItemId, repairMeta)
    else
        HostApi.log("==== LuaLog: HouseConfig:repairLandBeforeBuild: [startPosition = nil or endPosition = nil]")
    end
end

function HouseConfig:destroySignPost(manorIndex)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor == nil then
        HostApi.log("==== LuaLog: HouseConfig:destroySignPost : manor is nil, destroy Sign Post failure")
        return
    end
    EngineWorld:setBlockToAir(manor.signPosition)
end

function HouseConfig:recycleBlock(player)
    local temporaryStore = GameMatch.temporaryStore[tostring(player.userId)]
    if temporaryStore ~= nil then
        for i, v in pairs(temporaryStore) do
            if v.blockId ~= 0 then
                local vec3 = YardConfig:getVec3iByOffset(player.manorIndex, v.offsetVec3)
                EngineWorld:setBlockToAir(vec3)
                player:addItem(v.blockId, 1, v.blockMeta)
            end
        end
    end
    GameMatch.temporaryStore[tostring(player.userId)] = nil
    -- HostApi.log("==== LuaLog: HouseConfig:recycleBlock: [" .. tostring(player.userId) .. "] recycle Block success")

end

return HouseConfig