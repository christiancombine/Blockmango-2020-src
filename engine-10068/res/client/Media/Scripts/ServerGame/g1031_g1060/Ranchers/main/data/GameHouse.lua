GameHouse = class()

function GameHouse:ctor(manorIndex)
    self.manorIndex = manorIndex
    self.houseLevel = 1
    self.house = {}
    self.isInWorld = false
    self:initFirstHousePos()
end

function GameHouse:initDataFromDB(data)
    self:initHouseLevelFromDB(data.houseLevel or 1)
    self:initHouseOffsetFromDB(data.offset or { x = 0, y = 0, z = 0 })
end

function GameHouse:initHouseLevelFromDB(houseLevel)
    self.houseLevel = houseLevel
end

function GameHouse:initHouseOffsetFromDB(offset)
    self:setHouseOffset(VectorUtil.newVector3i(offset.x, offset.y, offset.z))
end

function GameHouse:prepareDataSaveToDB()
    local data = {
        houseLevel = self.houseLevel,
        offset = self:prepareOffsetDataToDB()
    }
    --HostApi.log("GameHouse:prepareDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GameHouse:prepareOffsetDataToDB()
    local offset = {}
    offset.x = self.house.offset.x
    offset.y = self.house.offset.y
    offset.z = self.house.offset.z
    return offset
end

function GameHouse:setHouseOffset(vec3i)
    self.house.offset = {}
    self.house.offset = vec3i
end

function GameHouse:setHouseToWorld()
    local houseInfo = HouseConfig:getHouseInfoByLevel(self.houseLevel)
    if houseInfo == nil then
        HostApi.log("=== LuaLog: [error] GameHouse:setHouseToWorld: can not find house info for level[" .. self.houseLevel .. "]")
        return false
    end

    if self.house.offset.x == 0 and self.house.offset.y == 0 and self.house.offset.z == 0 then
        return false
    end

    local pos = ManorConfig:getVec3iByOffset(self.manorIndex, self.house.offset)
    if pos ~= nil then
        EngineWorld:createSchematic(houseInfo.fileName, pos)
    end

    local npcInfo = SessionNpcConfig:getNpcInfoById(houseInfo.npcId)
    if npcInfo == nil then
        HostApi.log("=== LuaLog: [error] GameHouse:setHouseToWorld : can not find npcInfo by id[" .. houseInfo.npcId .. "]")
        return false
    end

    local npcOffset = {}
    npcOffset.x = self.house.offset.x + houseInfo.npcPosX
    npcOffset.y = self.house.offset.y + houseInfo.npcPosY
    npcOffset.z = self.house.offset.z + houseInfo.npcPosZ

    local message = {}
    message.title = npcInfo.title
    message.tips = {}
    message.interactionType = npcInfo.interactionType
    message.sureContent = npcInfo.sureContent
    for i, v in pairs(npcInfo.message) do
        message.tips[i] = v
    end

    local npcPos = ManorConfig:getVec3ByOffset(self.manorIndex, npcOffset)
    if npcPos ~= nil then
        local entityId = EngineWorld:addSessionNpc(npcPos, houseInfo.npcYaw, SessionNpcConfig.MULTI_TIP_INTERACTION_NPC, npcInfo.actor, function(entity)
            entity:setNameLang(npcInfo.name or "")
            entity:setSessionContent(json.encode(message) or "")
        end)
        self.house.npcEntityId = entityId
    end
    self.isInWorld = true
end

function GameHouse:removeHouseFromWorld(houseLevel)
    local houseInfo = HouseConfig:getHouseInfoByLevel(houseLevel)
    if houseInfo == nil then
        HostApi.log("=== LuaLog: [error] GameHouse:removeHouseFromWorld: can not find house info for level[" .. houseLevel .. "]")
        return false
    end
    local pos = ManorConfig:getVec3iByOffset(self.manorIndex, self.house.offset)
    if pos ~= nil then
        EngineWorld:destroySchematic(houseInfo.fileName, pos)
        local endPos = VectorUtil.newVector3i(pos.x + houseInfo.length - 1, pos.y, pos.z + houseInfo.width - 1)
        EngineWorld:getWorld():fillAreaByBlockIdAndMate(pos, endPos, 2, 0)
    end

    if self.house.npcEntityId ~= nil then
        EngineWorld:removeEntity(self.house.npcEntityId)
        self.house.npcEntityId = nil
    end

    self.house.offset = VectorUtil.newVector3i(0, 0, 0)
    self.isInWorld = false
end

function GameHouse:initFirstHousePos()
    self:setHouseOffset(HouseConfig.houseInitPos)
end

function GameHouse:initRanchHouse()
    return HouseConfig:newRanchHouseInfo(self.houseLevel)
end

function GameHouse:upgrade()
    local maxLevel = HouseConfig:getMaxHouseLevel()
    self.houseLevel = self.houseLevel + 1
    if self.houseLevel > maxLevel then
        self.houseLevel = maxLevel
    end
end

function GameHouse:getHouseStartPosAndEndPos()
    local offset = self.house.offset
    if offset == nil then
        return nil, nil
    end

    local houseInfo = HouseConfig:getHouseInfoByLevel(self.houseLevel)
    if houseInfo == nil then
        HostApi.log("=== LuaLug: [error] GameHouse:getHouseStartPosAndEndPos : can not get houseInfo by level[" .. self.houseLevel .. "]")
        return nil, nil
    end

    local startPos = ManorConfig:getVec3iByOffset(self.manorIndex, offset)
    if startPos == nil then
        return nil, nil
    end

    local endPos = VectorUtil.newVector3i(startPos.x + houseInfo.length, startPos.y + houseInfo.height, startPos.z + houseInfo.width)
    return startPos, endPos
end

function GameHouse:getItemIdOfHouse()
    local houseInfo = HouseConfig:getHouseInfoByLevel(self.houseLevel)
    if houseInfo == nil then
        HostApi.log("=== LuaLug: [error] GameHouse:getItemsOfHouse : can not find houseInfo for level[" .. self.houseLevel .. "]")
        return 0
    end

    return houseInfo.itemId
end

function GameHouse:getFileNameOfHouse()
    local houseInfo = HouseConfig:getHouseInfoByLevel(self.houseLevel)
    if houseInfo == nil then
        HostApi.log("=== LuaLug: [error] GameHouse:getItemsOfHouse : can not find houseInfo for level[" .. self.houseLevel .. "]")
        return nil
    end

    return houseInfo.fileName
end
