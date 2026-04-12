GameDecoration = class()

function GameDecoration:ctor(manorIndex, userId)
    self.manorIndex = manorIndex
    self.userId = userId
    self.decoration = {}
    self.decorationScore = 0
end

function GameDecoration:initDataFromDB(data)
    self:initDecorationData(data.decoration or {})
end

function GameDecoration:initDecorationData(data)
    for key, value in pairs(data) do
        local decoration = StringUtil.split(key, ":")
        local id = tonumber(value)
        local x = tonumber(decoration[1])
        local y = tonumber(decoration[2])
        local z = tonumber(decoration[3])
        local offsetYaw = tonumber(decoration[4])

        local decorationInfo = DecorationConfig:getDecorationInfo(id)
        if decorationInfo then
            local offset = VectorUtil.newVector3(x, y, z)
            local pos = ManorConfig:getPosByOffset(self.manorIndex, offset, false)
            local yaw = ManorConfig:getYawByOffsetYaw(self.manorIndex, offsetYaw)

            -- 解决放置装饰时不同朝向产生的偏移
            local orientation = ManorConfig:getOrientationByIndex(self.manorIndex)
            if orientation == 270 then
                pos.x = pos.x + 1
            end

            if orientation == 90 then
                pos.z = pos.z + 1
            end

            local entityId = EngineWorld:getWorld():addDecoration(pos, yaw, self.userId, id)
            self:addDecoration(entityId, id, offset, offsetYaw)
        end
    end
    GameMatch.isDecorationChange = true
end

function GameDecoration:prepareDataSaveToDB()
    local data = {
        decoration = self:prepareDecorationDataSaveToDB()
    }
    return data
end

function GameDecoration:prepareDecorationDataSaveToDB()
    local decoration = {}
    for _, v in pairs(self.decoration) do
        local decorationInfo = DecorationConfig:getDecorationInfo(v.id)
        if decorationInfo then
            local pos = v.offset.x .. ":" .. v.offset.y .. ":" .. v.offset.z .. ":" .. v.offsetYaw
            local id = v.id
            decoration[pos] = id
        end
    end

    return decoration
end

function GameDecoration:removeFromWorld()
    for _, v in pairs(self.decoration) do
        EngineWorld:removeEntity(v.entityId)
    end
end

function GameDecoration:addDecoration(entityId, id, offset, offsetYaw)
    self.decoration[entityId] = {
        entityId = entityId,
        id = id,
        offset = offset,
        offsetYaw = offsetYaw,
    }
    self:addDecorationScore(id)
end

function GameDecoration:updateDecorationYaw(entityId, offsetYaw)
    if not self.decoration[entityId] then
        return
    end

    self.decoration[entityId].offsetYaw = offsetYaw
end

function GameDecoration:updateDecorationPos(entityId, offset)
    if not self.decoration[entityId] then
        return
    end

    self.decoration[entityId].offset = offset
end

function GameDecoration:removeDecoration(entityId)
    if not self:checkHasDecoration(entityId) then
        return
    end

    EngineWorld:removeEntity(entityId)
    local id = self.decoration[entityId].id
    self.decoration[entityId] = nil
    self:subDecorationScore(id)
end

function GameDecoration:checkHasDecoration(entityId)
    return self.decoration[entityId]
end

function GameDecoration:addDecorationScore(id)
    local score = DecorationConfig:getScore(id)
    self.decorationScore = self.decorationScore + score
end

function GameDecoration:subDecorationScore(id)
    local score = DecorationConfig:getScore(id)
    self.decorationScore = self.decorationScore - score
    if self.decorationScore < 0 then
        self.decorationScore = 0
    end
end

function GameDecoration:getDecorationScore()
    return self.decorationScore
end

function GameDecoration:pushDecorationAreaInfo()
    for _, v in pairs(self.decoration) do
        local yaw = ManorConfig:getYawByOffsetYaw(self.manorIndex, v.offsetYaw)
        local newPos = ManorConfig:getPosByOffset(self.manorIndex, v.offset)
        local decorationInfo = DecorationConfig:getDecorationInfo(v.id)
        if decorationInfo and decorationInfo.isInteraction == 1 then
            local x = 0
            local z = 0

            if yaw % 180 == 0 then
                x = decorationInfo.length / 2
                z = decorationInfo.width / 2
            else
                x = decorationInfo.width / 2
                z = decorationInfo.length / 2
            end

            local orientation = ManorConfig:getOrientationByIndex(self.manorIndex)
            if orientation == 270 then
                newPos.x = newPos.x + 1
            end

            if orientation == 90 then
                newPos.z = newPos.z + 1
            end

            local startPos = VectorUtil.newVector3(newPos.x - x  , newPos.y, newPos.z - z )
            local endPos = VectorUtil.newVector3(newPos.x + x , newPos.y + decorationInfo.height + 1, newPos.z + z )
            AreaInfoManager:push(v.entityId, AreaType.Decoration, decorationInfo.tipNor, decorationInfo.tipPre, decorationInfo.imageNor, decorationInfo.imagePre, false, startPos, endPos)
        end
    end
end