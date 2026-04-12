SceneManager = {}

SceneManager.userManorInfo = {}
SceneManager.userSchematicInfo = {}
SceneManager.userDecorationInfo = {}

SceneManager.callOnTarget = {}
SceneManager.errorTarget = {}

function SceneManager:initUserManorInfo(userId, manorInfo)
    self.userManorInfo[tostring(userId)] = manorInfo
    ManorConfig:setLoadState(manorInfo.manorIndex, true)
end

function SceneManager:initUserSchematicInfo(userId, schematicInfo)
    self.userSchematicInfo[tostring(userId)] = schematicInfo
end

function SceneManager:initUserDecorationInfo(userId, decorationInfo)
    self.userDecorationInfo[tostring(userId)] = decorationInfo
end

function SceneManager:initCallOnTarget(userId, targetUserId)
    self.callOnTarget[tostring(userId)] = {}
    self.callOnTarget[tostring(userId)].userId = targetUserId
    self.callOnTarget[tostring(userId)].time = 0
end

function SceneManager:initErrorTarget(userId)
    self.errorTarget[tostring(userId)] = {}
    self.errorTarget[tostring(userId)].userId = userId
    self.errorTarget[tostring(userId)].time = 0
end

function SceneManager:getUserManorInfo(userId)
    return self.userManorInfo[tostring(userId)]
end

function SceneManager:getUserSchematicInfo(userId)
    return self.userSchematicInfo[tostring(userId)]
end

function SceneManager:getUserDecorationInfo(userId)
    return self.userDecorationInfo[tostring(userId)]
end

function SceneManager:getTargetByUid(userId)
    if self.callOnTarget[tostring(userId)] ~= nil then
        return self.callOnTarget[tostring(userId)].userId
    end

    return nil
end

function SceneManager:getCallOnTargets()
    return self.callOnTarget
end

function SceneManager:getErrorTarget(userId)
    if self.errorTarget[tostring(userId)] ~= nil then
        return self.errorTarget[tostring(userId)].userId
    end

    return nil
end

function SceneManager:getErrorTargets()
    return self.errorTarget
end

function SceneManager:getUserManorInfoByIndex(index)
    index = tonumber(index)
    for _, manor in pairs(self.userManorInfo) do
        if manor.manorIndex == index then
            return manor
        end
    end

    return nil
end

function SceneManager:deleteUserManorInfo(userId)
    if self.userManorInfo[tostring(userId)] ~= nil then
        ManorConfig:setLoadState(self.userManorInfo[tostring(userId)].manorIndex, false)
    end
    self.userManorInfo[tostring(userId)] = nil
end

function SceneManager:deleteUserSchematicInfo(userId)
    self.userSchematicInfo[tostring(userId)] = nil
end

function SceneManager:deleteUserDecorationInfo(userId)
    self.userDecorationInfo[tostring(userId)] = nil
end

function SceneManager:deleteCallOnTarget(userId)
    self.callOnTarget[tostring(userId)] = nil
end

function SceneManager:deleteErrorTarget(userId)
    self.errorTarget[tostring(userId)] = nil
end

function SceneManager:onTick(ticks)
    self:updateCallOnTargetTime()
    self:updateErrorTargetTime()
    self:updateManorTime()
end

function SceneManager:updateCallOnTargetTime()
    for userId, v in pairs(self.callOnTarget) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            v.time = v.time + 1
        else
            v.time = 0
        end

        if v.time > GameConfig.manorReleaseTime then
            self:deleteCallOnTarget(userId)
            if player then
                HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
            end
        end
    end
end

function SceneManager:updateErrorTargetTime()
    for userId, v in pairs(self.errorTarget) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            print("errorTarget : userId[".. tostring(userId) .."]  --  time[".. v.time .."] ")
            v.time = v.time + 1
        else
            v.time = 0
        end
        if v.time > 10 then
            self:deleteErrorTarget(userId)
        end
    end
end

function SceneManager:updateManorTime()
    for i, manor in pairs(self.userManorInfo) do
        if manor:getMasterState() then
            manor:refreshTime()
        else
            manor:checkVisitor()

            if manor:isEmptyVisitor() then
                manor:addTime(1)
            else
                manor:refreshTime()
            end

            if manor:getTime() >= GameConfig.manorReleaseTime then
                local manorIndex = manor.manorIndex
                SceneManager:releaseManor(manor.userId)

                local manorInfo = ManorConfig:getManorInfoByIndex(manorIndex)
                if manorInfo then
                    local rotation  = math.floor((manorInfo.orientation % 360 + 180 + 45) % 360 / 90) + 1
                    local pos = ManorConfig:getManorPosByIndex(manorIndex, true)
                    SceneManager:createOrDestroyHouse(GameConfig.groundName, true, pos, 0, rotation)
                end

                local player = PlayerManager:getPlayerByUserId(manor.userId)
                if player then
                    HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
                end
                print(" release Manor:", manorIndex)
            end
        end
    end
end

function SceneManager:releaseManor(userId)
    local manorInfo = SceneManager:getUserManorInfo(userId)
    if manorInfo ~= nil then
        manorInfo:removeFromWorld()
    end

    local schematicInfo = SceneManager:getUserSchematicInfo(userId)
    if schematicInfo ~= nil then
        schematicInfo:removeFromWorld()
    end

    local decorationInfo = SceneManager:getUserDecorationInfo(userId)
    if decorationInfo ~= nil then
        decorationInfo:removeFromWorld()
    end

    SceneManager:deleteUserManorInfo(userId)
    SceneManager:deleteUserSchematicInfo(userId)
    SceneManager:deleteUserDecorationInfo(userId)
    HostApi.sendReleaseManor(userId)
    DBManager:removeCache(userId)
end

function SceneManager:createOrDestroyHouse(fileName, isAdd, pos, yaw, rotate)
    local rotation = rotate
    if rotation == 0 then
        rotation = math.floor((yaw % 360 + 180 + 45) % 360 / 90) + 1
        if rotation < 1 then
            rotation = 4
        end
        if rotation > 4 then
            rotation = 1
        end
    end
    local xImage = false
    local zImage = false
    if rotation == 1 then
        xImage = false
        zImage = false
    elseif rotation == 2 then
        xImage = true
        zImage = false
    elseif rotation == 3 then
        xImage = true
        zImage = true
    elseif rotation == 4 then
        xImage = false
        zImage = true
    end
    if isAdd then
        EngineWorld:createSchematic(fileName, pos, xImage, zImage, true, 0, 0, 1)
    else
        EngineWorld:destroySchematic(fileName, pos, xImage, zImage, 0, 1)
    end
end

function SceneManager:fillAreaBaseBlock(startPos, endPos, manorLevel)
    local repairBlockId = ManorLevelConfig:getRepairBlockIdByLevel(manorLevel)
    local repairBlockMeta = ManorLevelConfig:getRepairBlockMetaByLevel(manorLevel)
    if not repairBlockMeta or not repairBlockId then
        return
    end
    EngineWorld:getWorld():fillAreaByBlockIdAndMate(VectorUtil.newVector3i(startPos.x ,startPos.y, startPos.z),
            VectorUtil.newVector3i(endPos.x, startPos.y, endPos.z) , repairBlockId, repairBlockMeta)
end

function SceneManager:levelUpFillAreaBaseBlock(preStartPos, preEndPos, curStartPos, curEndPos, manorLevel)
    local repairBlockId = ManorLevelConfig:getRepairBlockIdByLevel(manorLevel)
    local repairBlockMeta = ManorLevelConfig:getRepairBlockMetaByLevel(manorLevel)
    if not repairBlockMeta or not repairBlockId then
        return
    end

    local curPosMinX = math.min(curStartPos.x, curEndPos.x)
    local curPosMaxX = math.max(curStartPos.x, curEndPos.x)
    local curPosMinZ = math.min(curStartPos.z, curEndPos.z)
    local curPosMaxZ = math.max(curStartPos.z, curEndPos.z)

    local preMinX = math.min(preStartPos.x, preEndPos.x)
    local preMaxX = math.max(preStartPos.x, preEndPos.x)
    local preMinZ = math.min(preStartPos.z, preEndPos.z)
    local preMaxZ = math.max(preStartPos.z, preEndPos.z)

    for x = curPosMinX, curPosMaxX do
        for z = curPosMinZ, curPosMaxZ do
            if not(preMinX <= x and x <= preMaxX and preMinZ <= z and z <= preMaxZ) then
                EngineWorld:setBlock(VectorUtil.newVector3i(x, curStartPos.y, z), repairBlockId, repairBlockMeta)
            end
        end
    end
end

return SceneManager