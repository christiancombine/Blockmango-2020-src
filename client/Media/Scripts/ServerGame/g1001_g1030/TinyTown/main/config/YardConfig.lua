require "config.ManorNpcConfig"

YardConfig = {}
YardConfig.yard = {}
YardConfig.maxLevel = 0
YardConfig.defaultIcon = ""

function YardConfig:init(config)
    self:initYard(config.yard)
    self.maxLevel = tonumber(config.maxLevel)
    self.defaultIcon = tostring(config.defaultIcon)
end

function YardConfig:initYard(config)
    for i, v in pairs(config) do
        local yard = {}
        yard.id = tonumber(v.id)
        yard.level = tonumber(v.level)
        yard.orientation = tonumber(v.orientation)

        yard.xLength = tonumber(v.xLength)
        yard.yLength = tonumber(v.yLength)
        yard.zLength = tonumber(v.zLength)

        yard.xOffset = tonumber(v.xOffset)
        yard.yOffset = tonumber(v.yOffset)
        yard.zOffset = tonumber(v.zOffset)

        yard.fenceName = tostring(v.fenceName)

        yard.coinId = tonumber(v.price[1])
        yard.blockymodsPrice = tonumber(v.price[2])
        yard.blockmanPrice = tonumber(v.price[3])

        yard.nextCoinId = tonumber(v.upgradePrice[1])
        yard.nextBlockymodsPrice = tonumber(v.upgradePrice[2])
        yard.nextBlockmanPrice = tonumber(v.upgradePrice[3])

        yard.addUpgradeArea = tonumber(v.addUpgradeArea)
        yard.addThumbUpNum = tonumber(v.addThumbUpNum)

        yard.addCharmValue = tonumber(v.addCharmValue)

        yard.addSeedNum = tonumber(v.addSeedNum)
        yard.currSeedNum = tonumber(v.currSeedNum)
        yard.totalSeedNum = tonumber(v.totalSeedNum)

        yard.gardenXLength = tonumber(v.gardenXLength)
        yard.gardenYLength = tonumber(v.gardenYLength)
        yard.gardenZLength = tonumber(v.gardenZLength)

        yard.gardenXOffset = tonumber(v.gardenXOffset)
        yard.gardenYOffset = tonumber(v.gardenYOffset)
        yard.gardenZOffset = tonumber(v.gardenZOffset)

        yard.gardenItemId = tonumber(v.gardenItemId)
        yard.gardenItemMeta = tonumber(v.gardenItemMeta)

        yard.seed = {}
        for j, s in pairs(v.seedID) do
            s.itemId = tonumber(s.itemId)
            yard.seed[j] = s
        end
        self.yard[i] = yard
    end
end

function YardConfig:getYardById(id)
    for i, v in pairs(self.yard) do
        if id == v.id then
            return v
        end
    end
    return nil
end

function YardConfig:getYardByLevel(level, manorIndex)
    local orientation = 1
    if manorIndex > 5 then
        orientation = 2
    end

    for i, v in pairs(self.yard) do
        if level == v.level and orientation == v.orientation then
            return v
        end
    end
    return nil
end

function YardConfig:isInYard(manorIndex, yardLevel, vec3)
    local yard = self:getYardByLevel(yardLevel, manorIndex)
    if yard == nil then
        return false
    end

    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor == nil then
        return false
    end

    local minX = manor.landPosition.minX + yard.xOffset
    local minY = manor.landPosition.minY + yard.yOffset
    local minZ = manor.landPosition.minZ + yard.zOffset

    local maxX = minX + yard.xLength - 1
    local maxY = minY + yard.yLength
    local maxZ = minZ + yard.zLength - 1

    return vec3.x >= minX and vec3.x <= maxX
    and vec3.y >= minY and vec3.y <= maxY
    and vec3.z >= minZ and vec3.z <= maxZ
end

function YardConfig:getFencePosition(manorIndex, yardLevel)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    local yard = YardConfig:getYardByLevel(yardLevel, manorIndex)
    if manor ~= nil and yard ~= nil then
        local x = manor.landPosition.minX + yard.xOffset
        local y = manor.landPosition.minY + 1
        local z = manor.landPosition.minZ + yard.zOffset
        return VectorUtil.newVector3i(x, y, z)
    else
        return VectorUtil.newVector3i(0, 0, 0)
    end
end

function YardConfig:getGardenPosition(manorIndex, yardLevel)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    local yard = YardConfig:getYardByLevel(yardLevel, manorIndex)
    if manor ~= nil and yard ~= nil then
        local minX = manor.landPosition.minX + yard.gardenXOffset
        local minY = manor.landPosition.minY
        local minZ = manor.landPosition.minZ + yard.gardenZOffset

        local maxX = minX + yard.gardenXLength - 1
        local maxY = minY + yard.gardenYLength - 1
        local maxZ = minZ + yard.gardenZLength - 1
        return VectorUtil.newVector3i(minX, minY, minZ) , VectorUtil.newVector3i(maxX, maxY, maxZ)
    end
    HostApi.log("==== LuaLog: YardConfig:getGardenPosition  manor = nil or yard = nil")
    return nil, nil
end

function YardConfig:getOffsetOfSeed(manorIndex, yardLevel, vec3)
    local startPosition, endPosition = YardConfig:getGardenPosition(manorIndex, yardLevel)
    if startPosition ~= nil and endPosition ~= nil then
        -- offset从1开始算，所以要加1
        local xOffset = vec3.x - startPosition.x + 1
        local yOffset = vec3.y - startPosition.y
        local zOffset = vec3.z - startPosition.z + 1
        return VectorUtil.newVector3i(xOffset, yOffset, zOffset)
    else
        return VectorUtil.newVector3i(0, 0, 0)
    end
end

function YardConfig:getSeedPositionByOffset(manorIndex, yardLevel, offsetVec3)
    local startPosition, endPosition = YardConfig:getGardenPosition(manorIndex, yardLevel)
    if startPosition ~= nil and endPosition ~= nil then
        -- offset从1开始算， 所以实际坐标要减1
        local x = startPosition.x + offsetVec3.x - 1
        local y = startPosition.y + offsetVec3.y
        local z = startPosition.z + offsetVec3.z - 1
        return VectorUtil.newVector3i(x, y, z)
    else
        return VectorUtil.newVector3i(0, 0, 0)
    end
end

function YardConfig:getOffsetByVec3i(manorIndex, vec3)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor ~= nil then
        local xOffset = vec3.x - manor.landPosition.minX
        local yOffset = vec3.y - manor.landPosition.minY
        local zOffset = vec3.z - manor.landPosition.minZ

        return VectorUtil.newVector3i(xOffset, yOffset, zOffset)
    else
        return VectorUtil.newVector3i(0, 0, 0)
    end
end

function YardConfig:getOffsetByVec3(manorIndex, vec3)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor ~= nil then
        local xOffset = vec3.x - manor.landPosition.minX
        local yOffset = vec3.y - manor.landPosition.minY
        local zOffset = vec3.z - manor.landPosition.minZ

        return VectorUtil.newVector3(xOffset, yOffset, zOffset)
    else
        return VectorUtil.ZERO
    end
end

function YardConfig:getVec3iByOffset(manorIndex, offsetVec3)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor ~= nil then
        local x = manor.landPosition.minX + offsetVec3.x
        local y = manor.landPosition.minY + offsetVec3.y
        local z = manor.landPosition.minZ + offsetVec3.z

        return VectorUtil.newVector3i(x, y, z)
    else
        return VectorUtil.newVector3i(0, 0, 0)
    end
end

function YardConfig:getVec3ByOffset(manorIndex, offsetVec3)
    offsetVec3 = offsetVec3 or VectorUtil.ZERO
    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor ~= nil then
        local x = manor.landPosition.minX + offsetVec3.x
        local y = manor.landPosition.minY + offsetVec3.y
        local z = manor.landPosition.minZ + offsetVec3.z

        return VectorUtil.newVector3(x, y, z)
    end
    return VectorUtil.ZERO
end

function YardConfig:UpGradeYard(player)
    if player.yardLevel <= self.maxLevel then
        player.yardLevel = player.yardLevel + 1
    end
end

function YardConfig:buildFence(manorIndex, yardLevel)
    local vec3 = YardConfig:getFencePosition(manorIndex, yardLevel)
    local yard = YardConfig:getYardByLevel(yardLevel, manorIndex)
    if yard ~= nil then
        local fileName = yard.fenceName
        EngineWorld:createSchematic(fileName, vec3)
    else
        HostApi.log("==== LuaLog: YardConfig:buildFence: yard = nil , build fence failure . ")
    end
end

function YardConfig:destroyFence(manorIndex, yardLevel)
    local vec3 = YardConfig:getFencePosition(manorIndex, yardLevel)
    local yard = YardConfig:getYardByLevel(yardLevel, manorIndex)
    if yard ~= nil then
        local fileName = yard.fenceName
        EngineWorld:destroySchematic(fileName, vec3)
    else
        HostApi.log("==== LuaLog: YardConfig:destroyFence: yard id nil, destroy fence failure")
    end
end

function YardConfig:isInGarden(manorIndex, yardLevel, vec3)
    local startPosition, endPosition = YardConfig:getGardenPosition(manorIndex, yardLevel)
    if startPosition ~= nil and endPosition ~= nil then
        return vec3.x >= startPosition.x and vec3.x <= endPosition.x
        and vec3.y >= startPosition.y and vec3.y <= endPosition.y
        and vec3.z >= startPosition.z and vec3.z <= endPosition.z
    else
        return false
    end
end

function YardConfig:buildGarden(manorIndex, yardLevel)
    local startPosition, endPosition = YardConfig:getGardenPosition(manorIndex, yardLevel)
    local yard = YardConfig:getYardByLevel(yardLevel, manorIndex)
    if yard ~= nil and startPosition ~= nil and endPosition ~= nil then
        local repairItemId = yard.gardenItemId
        local repairMeta = yard.gardenItemMeta
        EngineWorld:getWorld():fillAreaByBlockIdAndMate(startPosition, endPosition, repairItemId, repairMeta)
    else
        HostApi.log("==== LuaLog: YardConfig:buildGarden: yard = nil or startPosition = nil or endPosition = nil , build Garden failure . ")
    end
end

function YardConfig:destroyGarden(manorIndex, yardLevel)
    local startPosition, endPosition = YardConfig:getGardenPosition(manorIndex, yardLevel)
    if startPosition ~= nil and endPosition ~= nil then
        local repairItemId = GameConfig.repairItemId
        local repairMeta = GameConfig.repairItemMeta
        EngineWorld:getWorld():fillAreaByBlockIdAndMate(startPosition, endPosition, repairItemId, repairMeta)
    else
        HostApi.log("==== LuaLog: YardConfig:destroyGarden: startPosition = nil or endPosition = nil, destroy Garden failure")
    end
end

function YardConfig:rebuildGardenSeeds(player)
    local blockCrops = GameMatch.blockCrops[tostring(player.userId)]
    if blockCrops ~= nil then
        for i, v in pairs(blockCrops) do
            local position = YardConfig:getSeedPositionByOffset(player.manorIndex, player.yardLevel, v.offsetVec3)
            EngineWorld:getWorld():setCropsBlock(player.userId, position, v.blockId, v.curState, v.curStateTime, v.fruitNum, v.lastUpdateTime, 1)
        end
    else
        HostApi.log("==== LuaLog: YardConfig:rebuildGardenSeeds GameMatch.blockCrops [" .. (tostring(player.userId)) .. "] is nil, rebuild garden seeds failure .")
    end

end

function YardConfig:sendPlayerToTelePosition(manorIndex, yardLevel)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor == nil then
        HostApi.log("==== LuaLog: YardConfig:sendPlayerToTelePosition: manor is nil send player failure ")
        return
    end
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if YardConfig:isInYard(manorIndex, yardLevel, player:getPosition()) then
            player:leaveVehicle()
            player:teleportPos(manor.telePosition)
            HostApi.log("==== LuaLog:  YardConfig:sendPlayerToTelePosition : player[" .. tostring(player.userId) .. "] has teleport to manorIndex[" .. manorIndex .."] ")
        end
    end
end

return YardConfig