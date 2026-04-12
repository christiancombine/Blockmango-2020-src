PathConfig = {}

PathConfig.Config = {}

function PathConfig:init(paths)
    for _, path in pairs(paths) do
        local item = {}
        item.ID = tonumber(path.ID)
        item.startYaw = tonumber(path.startYaw)
        local startPos = StringUtil.split(path.startPos, ",")
        item.startPos = VectorUtil.newVector3(startPos[1], startPos[2], startPos[3])
        local endPos = StringUtil.split(path.endPos, ",")
        item.endPos = VectorUtil.newVector3(endPos[1], endPos[2], endPos[3])
        item.area = self:getArea(path.areaRange)
        item.spawnPos, item.spawnYaw = self:parseSpawnPos(path.spawnPos)
        item.blockIds = {}
        for index, id in pairs(StringUtil.split(path.blockIds, "#")) do
            item.blockIds[index] = tonumber(id)
        end
        self.Config[item.ID] = item
    end
end

function PathConfig:parseSpawnPos(str)
    local spawnPos = {}
    local spawnYaw = {}
    local posStrs = StringUtil.split(str, "#")
    for _, posStr in pairs(posStrs) do
        local pos = StringUtil.split(posStr, ",")
        table.insert(spawnPos, VectorUtil.newVector3(pos[1], pos[2], pos[3]))
        table.insert(spawnYaw, pos[4])
    end
    return spawnPos, spawnYaw
end

function PathConfig:getArea(areaRange)
    local spawnPosStr = StringUtil.split(areaRange, "#")
    local pos1 = StringUtil.split(spawnPosStr[1], ",")
    local pos2 = StringUtil.split(spawnPosStr[2], ",")
    local area = IArea.new()
    area:init(tonumber(pos1[1]), tonumber(pos2[1]), 0, 0, tonumber(pos1[2]), tonumber(pos2[2]))
    return area
end

function PathConfig:getConfigById(id)
    return self.Config[tonumber(id)]
end

PathConfig:init(FileUtil.getConfigFromCsv("setting/Path.csv"))