GameSchematic = class()

function GameSchematic:ctor(manorIndex, userId)
    self.userId = userId
    self.manorIndex = manorIndex
    self.schematic = {}
end

function GameSchematic:initDataFromDB(data)
    self:initSchematicDataFromDB(data.schematic or {})
end

function GameSchematic:initSchematicDataFromDB(data)
    for key, value in pairs(data) do
        local schematic = StringUtil.split(key, ":")
        local id = tonumber(value)
        local x = tonumber(schematic[1])
        local y = tonumber(schematic[2])
        local z = tonumber(schematic[3])
        local x2 = tonumber(schematic[4])
        local y2 = tonumber(schematic[5])
        local z2 = tonumber(schematic[6])
        local offsetYaw = tonumber(schematic[7])
        local offset = VectorUtil.newVector3i(x, y, z)
        local offset2 = VectorUtil.newVector3i(x2, y2, z2)
        local pos = ManorConfig:getPosByOffset(self.manorIndex, offset, true)
        local pos2 = ManorConfig:getPosByOffset(self.manorIndex, offset2, true)
        local yaw = ManorConfig:getYawByOffsetYaw(self.manorIndex, offsetYaw)
        local schematicInfo = StoreConfig:getItemInfoById(id)
        if not schematicInfo then
            break
        end
        local minX = math.min(pos.x, pos2.x)
        local minY = math.min(pos.y, pos2.y)
        local minZ = math.min(pos.z, pos2.z)
        local placePos = VectorUtil.newVector3i(minX, minY, minZ)
        SceneManager:createOrDestroyHouse(schematicInfo.fileName, true, placePos, yaw, 0)
        self:addSchematic(id, offset, offset2, offsetYaw)
    end
end

function GameSchematic:prepareDataSaveToDB()
    local data = {
        schematic = self:prepareSchematicDataSaveToDB()
    }
    return data
end

function GameSchematic:prepareSchematicDataSaveToDB()
    local schematic = {}
    for _, v in pairs(self.schematic) do
        local pos = v.offset.x .. ":" .. v.offset.y .. ":" .. v.offset.z
        local pos2 = v.offset2.x .. ":" .. v.offset2.y .. ":" .. v.offset2.z
        local sign = pos .. ":" .. pos2 .. ":" .. v.offsetYaw
        local id = v.id
        schematic[sign] = id
    end

    return schematic
end

function GameSchematic:removeSchematicFromWorld(index, manorLevel, isRemoveAll)
    if not self.schematic[index] then
        return
    end

    local schematic = self.schematic[index]
    local pos = ManorConfig:getPosByOffset(self.manorIndex, schematic.offset, true)
    local pos2 = ManorConfig:getPosByOffset(self.manorIndex, schematic.offset2, true)
    local yaw = ManorConfig:getYawByOffsetYaw(self.manorIndex, schematic.offsetYaw)
    local schematicInfo = StoreConfig:getItemInfoById(schematic.id)
    if not schematicInfo then
        return
    end
    local minX = math.min(pos.x, pos2.x)
    local minY = math.min(pos.y, pos2.y)
    local minZ = math.min(pos.z, pos2.z)
    local removePos = VectorUtil.newVector3i(minX, minY, minZ)
    SceneManager:createOrDestroyHouse(schematicInfo.fileName, false, removePos, yaw, 0)
    if not isRemoveAll then
        local repairBlockId = ManorLevelConfig:getRepairBlockIdByLevel(manorLevel)
        local repairBlockMeta = ManorLevelConfig:getRepairBlockMetaByLevel(manorLevel)

        local repairBlockId2 = ManorLevelConfig:getRepairBlockId2ByLevel(manorLevel)
        local repairBlockMeta2 = ManorLevelConfig:getRepairBlockMeta2ByLevel(manorLevel)
        if not repairBlockMeta or not repairBlockId or not repairBlockMeta2 or not repairBlockId2 then
            return
        end

        local maxX = math.max(pos.x, pos2.x)
        local maxZ = math.max(pos.z, pos2.z)

        for x = minX, maxX do
            for z = minZ, maxZ do
                local vec3 = VectorUtil.newVector3i(x, minY, z)
                local startPos = ManorConfig:getManorPosByIndex(self.manorIndex, true)
                if startPos and vec3.y < startPos.y then
                    if ManorConfig:isInManorOpenArea(self.manorIndex, manorLevel, vec3) then
                        EngineWorld:setBlock(vec3, repairBlockId, repairBlockMeta)
                    else
                        EngineWorld:setBlock(vec3, repairBlockId2, repairBlockMeta2)
                    end
                end
            end
        end
    end
end

function GameSchematic:removeFromWorld()
    local manorInfo = SceneManager:getUserManorInfo(self.userId)
    if not manorInfo then
        return
    end

    for index, _ in pairs(self.schematic) do
        self:removeSchematicFromWorld(index, manorInfo.level, true)
    end

    local startPos, endPos = ManorConfig:getManorPosByIndex(self.manorIndex, true)
    SceneManager:fillAreaBaseBlock(startPos, endPos, manorInfo.level)
end

function GameSchematic:addSchematic(id, offset, offset2, offsetYaw)
    local data = {
        id = id,
        offset = offset,
        offset2 = offset2,
        offsetYaw = math.floor(offsetYaw),
    }
    table.insert(self.schematic, data)
end

function GameSchematic:removeSchematicData(index)
    table.remove(self.schematic, index)
end

function GameSchematic:getSchematicIndexByPos(pos)
    local offset = ManorConfig:getOffsetByPos(self.manorIndex, pos, true)
    for index, v in pairs(self.schematic) do
        if v.offset.x <= offset.x and offset.x <= v.offset2.x
        and v.offset.y < offset.y and offset.y <= v.offset2.y
        and v.offset.z <= offset.z and offset.z <= v.offset2.z then
            return index
        end
    end
    return 0
end

function GameSchematic:getSchematicByIndex(index)
    if not self.schematic[index] then
        return nil
    end
    return self.schematic[index]
end

function GameSchematic:getSchematicIndexByOffset(offset, offset2)
    for index, v in pairs(self.schematic) do
        if v.offset.x == offset.x and v.offset.y == offset.y and v.offset.z == offset.z
            and v.offset2.x == offset2.x and v.offset2.y == offset2.y and v.offset2.z == offset2.z then
            return index
        end
    end
    return 0
end

function GameSchematic:changeOffsetYawByIndex(index, offsetYaw)
    if not self.schematic[index] then
        return
    end
    self.schematic[index].offsetYaw = math.floor(offsetYaw)
end