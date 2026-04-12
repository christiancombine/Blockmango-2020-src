GameField = class()

function GameField:ctor(manorIndex)
    self.manorIndex = manorIndex
    self.fields = {}
    self.crops = {}
    self.roads = {}
end

function GameField:initDataFromDB(data, userId)
    self:initFieldsFromDB(data.fields or {})
    self:initCropsFromDB(data.crops or {}, userId)
end

function GameField:initRoadsDataFromDB(data, userId)
    self:initRoadsFromDB(data.roads or {})
end

function GameField:removeAllFieldAndRoadsFromWorld()
    for i, v in pairs(self.crops) do
        local vec3 = ManorConfig:getVec3iByOffset(self.manorIndex, v.offset)
        if vec3 ~= nil then
            EngineWorld:setBlockToAir(vec3)
        end
    end

    for i, v in pairs(self.fields) do
        local vec3 = ManorConfig:getVec3iByOffset(self.manorIndex, v)
        if vec3 ~= nil then
            EngineWorld:setBlock(vec3, 2)
        end
    end

    for i, v in pairs(self.roads) do
        local vec3 = ManorConfig:getVec3iByOffset(self.manorIndex, v)
        if vec3 ~= nil then
            EngineWorld:setBlockToAir(vec3)
        end
    end
end

function GameField:initFieldsFromDB(fields)
    for i, v in pairs(fields) do
        local offset = VectorUtil.newVector3i(v.x, v.y, v.z)
        local pos = ManorConfig:getVec3iByOffset(self.manorIndex, offset)
        if pos ~= nil then
            EngineWorld:setBlock(pos, 60)
        end
        local sign = VectorUtil.hashVector3(offset)
        self.fields[sign] = offset
    end
end

function GameField:initCropsFromDB(crops, userId)
    for i, v in pairs(crops) do
        local offset = VectorUtil.newVector3i(v.x, v.y, v.z)
        local pos = ManorConfig:getVec3iByOffset(self.manorIndex, offset)
        if pos ~= nil then
            EngineWorld:getWorld():setCropsBlock(userId, pos, v.blockId, v.state, v.curStateTime, 0, v.lastUpdateTime, v.residueHarvestNum)
        end
        local sign = VectorUtil.hashVector3(offset)
        self.crops[sign] = {}
        self.crops[sign].offset = offset
        self.crops[sign].blockId = v.blockId
        self.crops[sign].curStateTime = v.curStateTime
        self.crops[sign].state = v.state
        self.crops[sign].lastUpdateTime = v.lastUpdateTime
        self.crops[sign].residueHarvestNum = v.residueHarvestNum
    end
end

function GameField:initRoadsFromDB(roads)
    for i, v in pairs(roads) do
        local offset = VectorUtil.newVector3i(v.x, v.y, v.z)
        local pos = ManorConfig:getVec3iByOffset(self.manorIndex, offset)
        if pos ~= nil then
            EngineWorld:setBlock(pos, 255)
        end
        local sign = VectorUtil.hashVector3(offset)
        self.roads[sign] = offset
    end
end

function GameField:initFieldPos()
    for i, v in pairs(ManorConfig.fieldInitPos) do
        local pos = ManorConfig:getVec3iByOffset(self.manorIndex, v.pos)
        if pos ~= nil then
            EngineWorld:setBlock(pos, 60)
            local sign = VectorUtil.hashVector3(v.pos)
            self.fields[sign] = v.pos
        end
    end
end

function GameField:initRoadsPos()
    for i, v in pairs(ManorConfig.roadsInitPos) do
        local pos = ManorConfig:getVec3iByOffset(self.manorIndex, v.pos)
        if pos ~= nil then
            EngineWorld:setBlock(pos, 255)
            local sign = VectorUtil.hashVector3(v.pos)
            self.roads[sign] = v.pos
        end
    end
end

function GameField:prepareDataSaveToDB()
    local data = {
        fields = self:prepareFieldToDB(),
        crops = self:prepareCropsToDB()
    }
    --HostApi.log("GameField:prepareDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GameField:prepareRoadsDataSaveToDB()
    local data = {
        roads = self:prepareRoadsToDB(),
    }
    --HostApi.log("GameField:prepareRoadsDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GameField:prepareFieldToDB()
    local fields = {}
    local index = 1
    for i, v in pairs(self.fields) do
        fields[index] = {}
        fields[index].x = v.x
        fields[index].y = v.y
        fields[index].z = v.z
        index = index + 1
    end
    return fields
end

function GameField:prepareCropsToDB()
    local crops = {}
    local index = 1
    for i, v in pairs(self.crops) do
        crops[index] = {}
        crops[index].x = v.offset.x
        crops[index].y = v.offset.y
        crops[index].z = v.offset.z
        crops[index].blockId = v.blockId
        crops[index].curStateTime = v.curStateTime
        crops[index].state = v.state
        crops[index].lastUpdateTime = v.lastUpdateTime
        crops[index].residueHarvestNum = v.residueHarvestNum
        index = index + 1
    end
    return crops
end

function GameField:prepareRoadsToDB()
    local roads = {}
    local index = 1
    for i, v in pairs(self.roads) do
        roads[index] = {}
        roads[index].x = v.x
        roads[index].y = v.y
        roads[index].z = v.z
        index = index + 1
    end
    return roads
end

function GameField:initRanchPlant(fieldLevel, currentFieldNum)
    local fieldInfo = FieldLevelConfig:getFieldInfoByFiledLevel(fieldLevel)
    local index = 0
    local data = {}
    index = 1
    if fieldInfo ~= nil then
        local info = RanchBuild.new()
        info.id = 500000
        info.price = fieldInfo.money
        info.num = currentFieldNum
        info.maxNum = fieldInfo.maxFieldNum
        info.needLevel = fieldInfo.playerLevel
        data[index] = info
        index = index + 1
    end

    local seedIndex = 1
    local seeds = {}
    for i, v in pairs(SeedLevelConfig.seeds) do
        seeds[seedIndex] = v
        seedIndex = seedIndex + 1
    end

    table.sort(seeds, function (a, b)
        return a.seedId < b.seedId
    end)

    for i, v in pairs(seeds) do
        local seedInfo = RanchBuild.new()
        seedInfo.id = v.seedId
        seedInfo.price = v.price
        seedInfo.num = 0
        seedInfo.maxNum = 1
        seedInfo.needLevel = v.playerLevel
        data[index] = seedInfo
        index = index + 1
    end

    return data
end

function GameField:placeField(vec3)
    local sign = VectorUtil.hashVector3(vec3)
    self.fields[sign] = vec3
end

function GameField:placeCrops(offset, blockId, residueHarvestNum)
    local sign = VectorUtil.hashVector3(offset)
    self.crops[sign] = {}
    self.crops[sign].offset = offset
    self.crops[sign].blockId = blockId
    self.crops[sign].state = 0
    self.crops[sign].curStateTime = 0
    self.crops[sign].lastUpdateTime = os.time()
    self.crops[sign].residueHarvestNum = residueHarvestNum
end

function GameField:placeRoad(vec3)
    local sign = VectorUtil.hashVector3(vec3)
    self.roads[sign] = vec3
end

function GameField:updateCrops(crops)
    local sign = VectorUtil.hashVector3(crops.offset)
    if self.crops[sign] == nil then
        self.crops[sign] = {}
    end
    self.crops[sign].offset = crops.offset
    self.crops[sign].blockId = crops.blockId
    self.crops[sign].state = crops.state
    self.crops[sign].curStateTime = crops.curStateTime
    self.crops[sign].lastUpdateTime = crops.lastUpdateTime
    self.crops[sign].residueHarvestNum = crops.residueHarvestNum
end

function GameField:breakField(vec3)
    local sign = VectorUtil.hashVector3(vec3)
    self.fields[sign] = nil
end

function GameField:breakCrops(vec3)
    local sign = VectorUtil.hashVector3(vec3)
    self.crops[sign] = nil
end

function GameField:breakRoad(vec3)
    local sign = VectorUtil.hashVector3(vec3)
    self.roads[sign] = nil
end

function GameField:getRoadsNum()
    local index = 0
    for i, v in pairs(self.roads) do
        index = index + 1
    end

    return index
end