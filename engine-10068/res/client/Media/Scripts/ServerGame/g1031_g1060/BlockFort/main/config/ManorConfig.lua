ManorConfig = {}
ManorConfig.manor = {}

function ManorConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.x1 = tonumber(v.x1)
        data.y1 = tonumber(v.y1)
        data.z1 = tonumber(v.z1)
        data.x2 = tonumber(v.x2)
        data.y2 = tonumber(v.y2)
        data.z2 = tonumber(v.z2)
        data.initPosX = tonumber(v.initPosX)
        data.initPosY = tonumber(v.initPosY)
        data.initPosZ = tonumber(v.initPosZ)
        data.initYaw = tonumber(v.initYaw)
        data.orientation = tonumber(v.orientation)
        data.rx = tonumber(v.rx)
        data.ry = tonumber(v.ry)
        data.rz = tonumber(v.rz)
        data.nameX = tonumber(v.nameX)
        data.nameY = tonumber(v.nameY)
        data.nameZ = tonumber(v.nameZ)
        data.isLoad = false
        self.manor[data.id] = data
    end
end

function ManorConfig:setLoadState(index, state)
    if self.manor[tonumber(index)] ~= nil then
        self.manor[tonumber(index)].isLoad = state
    end
end

function ManorConfig:getLoadState(index)
    if self.manor[tonumber(index)] ~= nil then
        return self.manor[tonumber(index)].isLoad
    end

    return false
end

function ManorConfig:getManorInfoByIndex(index)
    if self.manor[tonumber(index)] ~= nil then
        return self.manor[tonumber(index)]
    end

    return nil
end

function ManorConfig:getManorPosByIndex(index, isInt)
    if self.manor[tonumber(index)] ~= nil then
        local manor = self.manor[tonumber(index)]

        local minX = math.min(manor.x1, manor.x2)
        local maxX = math.max(manor.x1, manor.x2)
        local minY = math.min(manor.y1, manor.y2)
        local maxY = math.max(manor.y1, manor.y2)
        local minZ = math.min(manor.z1, manor.z2)
        local maxZ = math.max(manor.z1, manor.z2)

        if isInt then
            return VectorUtil.newVector3i(minX, minY, minZ), VectorUtil.newVector3i(maxX, maxY, maxZ)
        end

        return VectorUtil.newVector3(minX, minY, minZ), VectorUtil.newVector3(maxX, maxY, maxZ)
    end

    print("manor.csv can not find manorPos where index =", index)
    return nil, nil
end

function ManorConfig:getOpenManorPos(level, index, isInt)
    local offsetStart, offsetEnd = ManorLevelConfig:getOffsetPosByLevel(level)
    if offsetStart ~= nil and offsetEnd ~= nil then
        local startPos = ManorConfig:getPosByOffset(index, offsetStart, isInt)
        local endPos = ManorConfig:getPosByOffset(index, offsetEnd, isInt)
        return startPos, endPos
    end

    print("manor.csv can not find OpenManorPos where index =", index)
    return nil, nil
end

function ManorConfig:getInitPosByIndex(index, isInt)
    if self.manor[tonumber(index)] ~= nil then

        local manor = self.manor[tonumber(index)]
        if isInt then
            return VectorUtil.newVector3i(manor.initPosX, manor.initPosY, manor.initPosZ)
        end

        return VectorUtil.newVector3(manor.initPosX, manor.initPosY, manor.initPosZ)
    end

    print("manor.csv can not find initPos where index =", index)
    return nil
end

function ManorConfig:getInitYawByIndex(index)
    if self.manor[tonumber(index)] ~= nil then
        return self.manor[tonumber(index)].initYaw
    end

    print("manor.csv can not find initYaw where index =", index)
    return 0
end

function ManorConfig:getOrientationByIndex(index)
    if self.manor[tonumber(index)] ~= nil then
        return self.manor[tonumber(index)].orientation
    end

    print("manor.csv can not find orientation where index =", index)
    return 0
end

function ManorConfig:getRelativeStartPos(index)
    local manor = self.manor[tonumber(index)]
    if manor == nil then
        print("manor.csv can not find relativeStartPos where index =", index)
        return nil
    end

    return VectorUtil.newVector3(manor.rx, manor.ry, manor.rz)
end

function ManorConfig:getOffsetByPos(index, pos, isInt)
    local manor = self.manor[tonumber(index)]
    local rStartPos = ManorConfig:getRelativeStartPos(index)
    if manor == nil or rStartPos == nil then
        if isInt then
            return VectorUtil.newVector3i(0, 0, 0)
        else
            return VectorUtil.newVector3(0, 0, 0)
        end
    end

    local x = 0
    local z = 0
    local y = pos.y - rStartPos.y

    local orientation = ManorConfig:getOrientationByIndex(index)
    if orientation == 0 then
        x = rStartPos.x - pos.x
        z = rStartPos.z - pos.z
    elseif orientation == 90 then
        x = rStartPos.z - pos.z
        z = pos.x - rStartPos.x
    elseif orientation == 180 then
        x = pos.x - rStartPos.x
        z = pos.z - rStartPos.z
    elseif orientation == 270 then
        x = pos.z - rStartPos.z
        z = rStartPos.x - pos.x
    end

    if isInt then
        return VectorUtil.newVector3i(x, y, z)
    end

    return VectorUtil.newVector3(x, y, z)
end

function ManorConfig:getPosByOffset(index, offset, isInt)
    local manor = self.manor[tonumber(index)]
    local rStartPos = ManorConfig:getRelativeStartPos(index)
    if manor == nil or rStartPos == nil then
        if isInt then
            return VectorUtil.newVector3i(0, 0, 0)
        else
            return VectorUtil.newVector3(0, 0, 0)
        end
    end

    local x = 0
    local z = 0
    local y = rStartPos.y + offset.y

    local orientation = ManorConfig:getOrientationByIndex(index)
    if orientation == 0 then
        x = rStartPos.x - offset.x
        z = rStartPos.z - offset.z
    elseif orientation == 90 then
        x = rStartPos.x + offset.z
        z = rStartPos.z - offset.x
    elseif orientation == 180 then
        x = rStartPos.x + offset.x
        z = rStartPos.z + offset.z
    elseif orientation == 270 then
        x = rStartPos.x - offset.z
        z = rStartPos.z + offset.x
    end

    if isInt then
        return VectorUtil.newVector3i(x, y, z)
    end

    return VectorUtil.newVector3(x, y, z)
end

function ManorConfig:getOffsetYaw(index, yaw)
    local orientation = ManorConfig:getOrientationByIndex(index)
    local offsetYaw = yaw + 180 - orientation
    if offsetYaw < 0 then
        offsetYaw = offsetYaw + 360
    end
    if offsetYaw > 360 then
        offsetYaw = offsetYaw - 360
    end

    return offsetYaw
end

function ManorConfig:getYawByOffsetYaw(index, offsetYaw)
    local orientation = ManorConfig:getOrientationByIndex(index)
    local yaw = offsetYaw + orientation - 180
    if yaw < 0 then
        yaw = yaw + 360
    end
    if yaw > 360 then
        yaw = yaw - 360
    end
    return yaw
end

function ManorConfig:isInManor(index, vec3)
    local startPos, endPos = ManorConfig:getManorPosByIndex(index,false)
    if startPos ~= nil and endPos ~= nil then
        return vec3.x >= startPos.x and vec3.x <= endPos.x
                and vec3.y >= startPos.y and vec3.y <= endPos.y
                and vec3.z >= startPos.z and vec3.z <= endPos.z
    end

    return false
end

function ManorConfig:isInManorOpenArea(index, level, vec3)
    local offsetStart, offsetEnd = ManorLevelConfig:getOffsetPosByLevel(level)
    if offsetStart ~= nil and offsetEnd ~= nil then
        local startPos = ManorConfig:getPosByOffset(index, offsetStart, false)
        local endPos = ManorConfig:getPosByOffset(index, offsetEnd, false)

        local minX = math.min(startPos.x, endPos.x)
        local maxX = math.max(startPos.x, endPos.x)
        local minY = math.min(startPos.y, endPos.y)
        local maxY = math.max(startPos.y, endPos.y)
        local minZ = math.min(startPos.z, endPos.z)
        local maxZ = math.max(startPos.z, endPos.z)

        return vec3.x >= minX and vec3.x <= maxX
                and vec3.y >= minY and vec3.y <= maxY
                and vec3.z >= minZ and vec3.z <= maxZ
    end

    return false
end

function ManorConfig:getNamePos(index)
    if self.manor[tonumber(index)] == nil then
        return nil
    end

    local manor = self.manor[tonumber(index)]
    return VectorUtil.newVector3(manor.nameX, manor.nameY, manor.nameZ)
end

function ManorConfig:enterManorArea(player, vec3)
    if player.curManorIndex ~= 0 then
        if ManorConfig:getLoadState(player.curManorIndex) == true then
            if ManorConfig:isInManor(player.curManorIndex, vec3) then
                return
            end
        end
        player.curManorIndex = 0
        return
    end

    if player.curManorIndex == 0 then
        for _, manor in pairs(self.manor) do
            if manor.isLoad == true and ManorConfig:isInManor(manor.id, vec3) then
                player.curManorIndex = manor.id
                local manorInfo = SceneManager:getUserManorInfoByIndex(manor.id)
                if manorInfo ~= nil then
                    manorInfo:visitorInHouse(player.userId)
                end
            end
        end
    end
end

function ManorConfig:getManorIndexByVec3(vec3)
    for _, v in pairs(self.manor) do
        if ManorConfig:isInManor(v.id, vec3) then
            return v.id
        end
    end

    return 0
end


function ManorConfig:checkAreaIsAirBlock(startPos, endPos)
    local minX = math.min(startPos.x, endPos.x)
    local maxX = math.max(startPos.x, endPos.x)
    local minY = math.min(startPos.y, endPos.y)
    local maxY = math.max(startPos.y, endPos.y)
    local minZ = math.min(startPos.z, endPos.z)
    local maxZ = math.max(startPos.z, endPos.z)

    for x = minX, maxX do
        for y = minY, maxY do
            for z = minZ, maxZ do
                local blockPos = VectorUtil.newVector3i(x, y, z)
                local blockId = EngineWorld:getBlockId(blockPos)
                if blockId ~= 0 then
                    return false
                end
            end
        end
    end

    return true
end

function ManorConfig:sortPos(pos1, pos2, isInt)
    local minX = math.min(pos1.x, pos2.x)
    local maxX = math.max(pos1.x, pos2.x)
    local minY = math.min(pos1.y, pos2.y)
    local maxY = math.max(pos1.y, pos2.y)
    local minZ = math.min(pos1.z, pos2.z)
    local maxZ = math.max(pos1.z, pos2.z)
    if isInt then
        return VectorUtil.newVector3i(minX, minY, minZ), VectorUtil.newVector3i(maxX, maxY, maxZ)
    else
        return VectorUtil.newVector3(minX, minY, minZ), VectorUtil.newVector3(maxX, maxY, maxZ)
    end
end

return ManorConfig