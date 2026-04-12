require "config.AreaConfig"
require "config.ManorConfig"
require "data.GameArea"

GameManor = class()

function GameManor:ctor(manorIndex, userId)
    self.manorIndex = manorIndex
    self.userId = userId
    self.name = "Rancher"
    self.area = {}
    self.unlockedArea = {}
    self.waitForUnlock = {}
    self.unlockedAreaCount = 1
    self.areaNpcEntity = {}
    self.canUnlockArea = {}
    self:initArea()
    self:initUnlockedArea()
    self:updateCanUnlockArea()
    self:updateManorNpcPos(self.canUnlockArea)
    self:updateManorNpcInfo()
end

function GameManor:updateManorNpcPos(canUnlockArea)
    for i, v in pairs(canUnlockArea) do
        local npcPos = self:getAreaNpcPosByAreaId(v.id)
        if npcPos ~= nil then
            if self.areaNpcEntity[v.id] == nil then
                self:createNpcToWorld(npcPos, 0, v.id, 1, AreaConfig.landNpcName, AreaConfig.landNpcActorName)
                self:addTemplateByAreaId(v.id)
            end
        else
            HostApi.log("=== LuaLog: [error] GameManor:updateManorNpcPos: unlock area npc pos is nil ...")
        end
    end
end

function GameManor:removeAllManorFromWorld()
    for i, v in pairs(self.area) do
        if self.areaNpcEntity[v.id] ~= nil then
            self:removeTemplateByAreaId(v.id)
            if self.areaNpcEntity[v.id].entityId ~= nil then
                EngineWorld:removeEntity(self.areaNpcEntity[v.id].entityId)
            end
            self.areaNpcEntity[v.id] = nil
        end
    end

    local pos = ManorConfig:getStartPosByManorIndex(self.manorIndex)
    if pos ~= nil then
        pos.x = pos.x - 1
        pos.y = pos.y - 1
        pos.z = pos.z - 1
        EngineWorld:createSchematic(GameConfig.manorFileName, pos)
    end
end

function GameManor:prepareDataSaveToDB()
    local data = {
        waitForUnlock = self:prepareWaitForUnlockToDB(),
        unlockedArea = self:prepareUnlockedAreaToDB(),
        unlockedAreaCount = self.unlockedAreaCount,
        name = self.name
    }

    --HostApi.log("GameManor:prepareDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GameManor:prepareWaitForUnlockToDB()
    local waitForUnlock = {}
    local index = 1
    for i, v in pairs(self.waitForUnlock) do
        waitForUnlock[index] = {}
        waitForUnlock[index] = v
        index = index + 1
    end
    return waitForUnlock
end

function GameManor:prepareUnlockedAreaToDB()
    local unlockedArea = {}
    local index = 1
    for i, v in pairs(self.unlockedArea) do
        unlockedArea[index] = {}
        unlockedArea[index] = v
        index = index + 1
    end
    return unlockedArea
end

function GameManor:initDataFromDB(data)
    self.unlockedAreaCount = data.unlockedAreaCount or 0
    self:initWaitForUnlockAreaFromDB(data.waitForUnlock or {})
    self:initUnlockedAreaFromDB(data.unlockedArea or {})
    self:initNameFromDB(data.name or "Rancher")
    self:updateCanUnlockArea()
end

function GameManor:initUnlockedAreaFromDB(unlockedArea)
    for i, v in pairs(unlockedArea) do
        if self.unlockedArea[v] == nil then
            self.unlockedArea[v] = v
            self:clearAreaBlockById(v)
            if self.areaNpcEntity[v] ~= nil then
                EngineWorld:removeEntity(self.areaNpcEntity[v].entityId)
                self.areaNpcEntity[v] = nil
            end
        end
    end

    for _, v in pairs(unlockedArea) do
        local newCanUnlockArea = self:findNewCanUnlockAreaById(v)
        self:updateManorNpcPos(newCanUnlockArea)
        self:updateManorNpcInfo()
    end
end

function GameManor:initWaitForUnlockAreaFromDB(waitForUnlock)
    for i, v in pairs(waitForUnlock) do
        local data = {}
        data.id = v.id
        data.startTime = v.startTime
        data.totalTime = v.totalTime
        self.waitForUnlock[v.id] = data
        self:addTemplateByAreaId(v.id)
        if self.areaNpcEntity[v.id] ~= nil then
            EngineWorld:removeEntity(self.areaNpcEntity[v.id].entityId)
        end

        local npcPos = self:getAreaNpcPosByAreaId(v.id)
        if npcPos ~= nil then
            self:createNpcToWorld(npcPos, 0, v.id, 2, AreaConfig.waitForUnlockLandNpcName, AreaConfig.waitForUnlockLandNpcActorName)
        end
    end
end

function GameManor:setName(name)
    self.name = name
end

function GameManor:initNameFromDB(name)
    self:setName(name)
end

function GameManor:createNpcToWorld(vec3, yaw, areaId, status, landNpcName, landNpcActorName)
    self.areaNpcEntity[areaId] = {}
    self.areaNpcEntity[areaId].entityId = EngineWorld:getWorld():addLandNpc(vec3, 0, self.userId, areaId, landNpcName, landNpcActorName, "", "")
    self.areaNpcEntity[areaId].status = status
end

function GameManor:updateManorNpcInfo()
    local expandInfo = AreaConfig:getExpandInfoByCount(self.unlockedAreaCount)
    if expandInfo == nil then
        return false
    end

    local recipe = {}
    local reward = {}

    local money = RanchCommon.new()
    money.itemId = 900001
    money.num = expandInfo.money
    money.price = 0
    recipe[1] = money

    local prosperity = RanchCommon.new()
    prosperity.itemId = 900002
    prosperity.num = expandInfo.prosperity
    prosperity.price = 0
    recipe[2] = prosperity

    local recipeIndex = 3
    for i, v in pairs(expandInfo.expandItems) do
        if tonumber(v.itemId) ~= 0 and tonumber(v.num) ~= 0 then
            local _recipe = RanchCommon.new()
            _recipe.itemId = tonumber(v.itemId)
            _recipe.num = tonumber(v.num)
            _recipe.price = 0
            for _, _item in pairs(ItemsConfig.items) do
                if _item.itemId == _recipe.itemId then
                    _recipe.price = _item.cubePrice
                end
            end
            recipe[recipeIndex] = _recipe
            recipeIndex = recipeIndex + 1
        end
    end

    local rewardIndex = 0
    for i, v in pairs(expandInfo.rewardItems) do
        if tonumber(v.itemId) ~= 0 and tonumber(v.num) ~= 0 then
            local _reward = RanchCommon.new()
            _reward.itemId = tonumber(v.itemId)
            _reward.num = tonumber(v.num)
            _reward.price = 0
            for _, _item in pairs(ItemsConfig.items) do
                if _item.itemId == _reward.itemId then
                    _reward.price = _item.cubePrice
                end
            end
            rewardIndex = rewardIndex + 1
            reward[rewardIndex] = _reward
        end
    end

    for i, v in pairs(self.areaNpcEntity) do
        if v.status == 1 then
            EngineWorld:getWorld():updateLandNpc(v.entityId, v.status, expandInfo.money, expandInfo.time * 1000, expandInfo.time * 1000, recipe, reward)
        end
    end

    for i, v in pairs(self.waitForUnlock) do
        EngineWorld:getWorld():updateLandNpc(self.areaNpcEntity[v.id].entityId, 2, 0, v.totalTime * 1000, (v.totalTime - (os.time() - v.startTime)) * 1000, {}, {})
    end
end

function GameManor:initArea()
    for i, v in pairs(AreaConfig.area) do
        local startPos = VectorUtil.newVector3(v.startPosX, v.startPosY, v.startPosZ)
        local endPos = VectorUtil.newVector3(v.endPosX, v.endPosY, v.endPosZ)
        self.area[v.id] = GameArea.new(v.id, startPos, endPos)
    end
end

function GameManor:addTemplateByAreaId(id)
    local area = AreaConfig.area[id]
    if area == nil then
        HostApi.log("=== LuaLog: GameManor:addTemplateByAreaId : can not find area info for areaId[" .. id .. "]")
        return false
    end
    local startAreaPos = self:getAreaStartPosByAreaId(id)
    local endAreaPos = self:getAreaEndPosByAreaId(id)

    local x = math.min(startAreaPos.x, endAreaPos.x)
    local y = math.min(startAreaPos.y, endAreaPos.y)
    local z = math.min(startAreaPos.z, endAreaPos.z)

    local pos = VectorUtil.newVector3i(x, y, z)
    EngineWorld:createSchematic(area.fileName, pos)
end

function GameManor:removeTemplateByAreaId(id)
    local area = AreaConfig.area[id]
    if area == nil then
        HostApi.log("=== LuaLog: GameManor:addTemplateByAreaId : can not find area info for areaId[" .. id .. "]")
        return false
    end
    local startAreaPos = self:getAreaStartPosByAreaId(id)
    local endAreaPos = self:getAreaEndPosByAreaId(id)

    local x = math.min(startAreaPos.x, endAreaPos.x)
    local y = math.min(startAreaPos.y, endAreaPos.y)
    local z = math.min(startAreaPos.z, endAreaPos.z)

    local pos = VectorUtil.newVector3i(x, y, z)
    EngineWorld:destroySchematic(area.fileName, pos)
end

function GameManor:clearAreaBlockById(id)
    local area = AreaConfig.area[id]
    if area == nil then
        HostApi.log("=== LuaLog: GameManor:deleteTemplateByAreaId : can not find area info for areaId[" .. id .. "]")
        return false
    end
    local startAreaPos = self:getAreaStartPosByAreaId(id)
    local endAreaPos = self:getAreaEndPosByAreaId(id)

    if startAreaPos ~= nil and endAreaPos ~= nil then
        local startPos = VectorUtil.newVector3i(startAreaPos.x, startAreaPos.y, startAreaPos.z)
        local endPos = VectorUtil.newVector3i(endAreaPos.x, endAreaPos.y, endAreaPos.z)
        EngineWorld:getWorld():fillAreaByBlockIdAndMate(startPos, endPos, 0, 0)
    end
end

function GameManor:initUnlockedArea()
    for i, v in pairs(AreaConfig.m_initUnlockedArea) do
        if self.areaNpcEntity[v] ~= nil then
            EngineWorld:removeEntity(self.areaNpcEntity[v].entityId)
        end
        self.unlockedArea[v] = v
        self:clearAreaBlockById(v)
    end
end

function GameManor:addWaitForUnlockAreaById(id, totalTime)
    local data = {}
    data.id = id
    data.startTime = os.time()
    data.totalTime = totalTime
    self.waitForUnlock[id] = data
    EngineWorld:removeEntity(self.areaNpcEntity[id].entityId)
    local npcPos = self:getAreaNpcPosByAreaId(id)
    if npcPos ~= nil then
        self:createNpcToWorld(npcPos, 0, id, 2, AreaConfig.waitForUnlockLandNpcName, AreaConfig.waitForUnlockLandNpcActorName)
    end

    self.unlockedAreaCount = self.unlockedAreaCount + 1
end

function GameManor:updateUnlockedAreaStateById(id)
    self.waitForUnlock[id] = nil
    self.unlockedArea[id] = id
    self:clearAreaBlockById(id)
    EngineWorld:removeEntity(self.areaNpcEntity[id].entityId)
    self.areaNpcEntity[id] = nil

    local newCanUnlockArea = self:findNewCanUnlockAreaById(id)
    self:updateManorNpcPos(newCanUnlockArea)
    self:updateManorNpcInfo()
    self:updateCanUnlockArea()
end

function GameManor:updateCanUnlockArea()
    local canUnlockArea = {}
    for _, _area in pairs (self.area) do
        for _, unlockedId in pairs(self.unlockedArea) do
            if self.area[unlockedId].topLine == _area.downLine then
                canUnlockArea[_area.id] = _area
            end
            if self.area[unlockedId].downLine == _area.topLine then
                canUnlockArea[_area.id] = _area
            end
            if self.area[unlockedId].leftLine == _area.rightLine then
                canUnlockArea[_area.id] = _area
            end
            if self.area[unlockedId].rightLine == _area.leftLine then
                canUnlockArea[_area.id] = _area
            end
        end
    end

    for index, area in pairs(canUnlockArea) do
        for _, unlockedId in pairs(self.unlockedArea) do
            if unlockedId == area.id then
                canUnlockArea[area.id] = nil
            end
        end
    end

    self.canUnlockArea = canUnlockArea
end

function GameManor:findNewCanUnlockAreaById(id)
    local canUnlockArea = {}
    for _, _area in pairs (self.area) do
        if self.area[id].topLine == _area.downLine then
            canUnlockArea[_area.id] = _area
        end
        if self.area[id].downLine == _area.topLine then
            canUnlockArea[_area.id] = _area
        end
        if self.area[id].leftLine == _area.rightLine then
            canUnlockArea[_area.id] = _area
        end
        if self.area[id].rightLine == _area.leftLine then
            canUnlockArea[_area.id] = _area
        end
    end

    for index, area in pairs(canUnlockArea) do
        for _, unlockedId in pairs(self.unlockedArea) do
            if unlockedId == area.id then
                canUnlockArea[area.id] = nil
            end
        end
    end

    for index, area in pairs(canUnlockArea) do
        for _, unlocked in pairs(self.canUnlockArea) do
            if unlocked.id == area.id then
                canUnlockArea[area.id] = nil
            end
        end
    end

    for index, area in pairs(canUnlockArea) do
        for _, v in pairs(self.waitForUnlock) do
            if v.id == area.id then
                canUnlockArea[area.id] = nil
            end
        end
    end

    return canUnlockArea
end

function GameManor:getAreaStartPosByAreaId(areaId)
    local manor = ManorConfig.manor[self.manorIndex]
    if manor ~= nil then
        local area = AreaConfig.area[areaId]
        -- 相对坐标转换成绝对坐标
        return VectorUtil.newVector3(
            tonumber(area.startPosX + manor.manorStartPosX),
            tonumber(area.startPosY + manor.manorStartPosY),
            tonumber(area.startPosZ + manor.manorStartPosZ)
        )
    end

    return nil
end

function GameManor:getAreaEndPosByAreaId(areaId)
    local manor = ManorConfig.manor[self.manorIndex]
    if manor ~= nil then
        local area = AreaConfig.area[areaId]
        -- 相对坐标转换成绝对坐标
        return VectorUtil.newVector3(
            tonumber(area.endPosX + manor.manorStartPosX - 1),
            tonumber(area.endPosY + manor.manorStartPosY - 1),
            tonumber(area.endPosZ + manor.manorStartPosZ - 1)
        )
    end

    return nil
end

function GameManor:getAreaNpcPosByAreaId(areaId)
    local manor = ManorConfig.manor[self.manorIndex]
    if manor ~= nil then
        local area = AreaConfig.area[areaId]
        -- 相对坐标转换成绝对坐标
        return VectorUtil.newVector3(
            tonumber(area.npcPosX + manor.manorStartPosX),
            tonumber(area.npcPosY + manor.manorStartPosY),
            tonumber(area.npcPosZ + manor.manorStartPosZ)
        )
    end

    return nil
end

function GameManor:getManorStartPos()
    local manor = ManorConfig.manor[self.manorIndex]
    if manor ~= nil then
        -- 使用绝对坐标
        return VectorUtil.newVector3(
            tonumber(manor.manorStartPosX),
            tonumber(manor.manorStartPosY),
            tonumber(manor.manorStartPosZ)
        )
    end

    return nil
end

function GameManor:getManorEndPos()
    local manor = ManorConfig.manor[self.manorIndex]
    if manor ~= nil then
        -- 使用绝对坐标
        return VectorUtil.newVector3(
            tonumber(manor.manorEndPosX),
            tonumber(manor.manorEndPosY),
            tonumber(manor.manorEndPosZ)
        )
    end

    return nil
end

function GameManor:isInManor(vec3)
    local startPos = self:getManorStartPos()
    local endPos = self:getManorEndPos()

    if startPos ~= nil and endPos ~= nil then
        local minX = math.min(startPos.x, endPos.x)
        local minY = math.min(startPos.y, endPos.y)
        local minZ = math.min(startPos.z, endPos.z)

        local maxX = math.max(startPos.x, endPos.x)
        local maxY = math.max(startPos.y, endPos.y)
        local maxZ = math.max(startPos.z, endPos.z)

        return vec3.x >= minX and vec3.x <= maxX
        and vec3.y >= minY and vec3.y <= maxY
        and vec3.z >= minZ and vec3.z <= maxZ
    end

    return false
end

function GameManor:isInUnlockedArea(vec3)
    for _, unlockedId in pairs(self.unlockedArea) do
        local areaStartPos = self:getAreaStartPosByAreaId(unlockedId)
        local areaEndPos = self:getAreaEndPosByAreaId(unlockedId)
        if areaStartPos ~= nil and areaEndPos ~= nil then
            local minX = math.min(areaStartPos.x, areaEndPos.x)
            local minY = math.min(areaStartPos.y, areaEndPos.y)
            local minZ = math.min(areaStartPos.z, areaEndPos.z)

            local maxX = math.max(areaStartPos.x, areaEndPos.x) + 1
            local maxY = math.max(areaStartPos.y, areaEndPos.y)
            local maxZ = math.max(areaStartPos.z, areaEndPos.z) + 1

            if vec3.x >= minX and vec3.x <= maxX
            and vec3.y >= minY and vec3.y <= maxY
            and vec3.z >= minZ and vec3.z <= maxZ then
                return true
            end
        end
    end

    return false
end

function GameManor:getAreaIdByEntityId(entityId)
    for i, v in pairs(self.areaNpcEntity) do
        if v.entityId == entityId then
            return i
        end
    end

    return 0
end

