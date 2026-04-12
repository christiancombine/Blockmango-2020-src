GameDecoration = class()

function GameDecoration:ctor(manorIndex, userId)
    self.manorIndex = manorIndex
    self.userId = userId
    self.decoration = {}
end

function GameDecoration:initDataFromDB(data)
    self:initDecorationDataFromDB(data.decoration or {})
end

function GameDecoration:initDecorationDataFromDB(data)
    for key, value in pairs(data) do
        local decoration = StringUtil.split(key, ":")
        local id = tonumber(value)
        local x = tonumber(decoration[1])
        local y = tonumber(decoration[2])
        local z = tonumber(decoration[3])
        local offsetYaw = tonumber(decoration[4])
        local decorationInfo = StoreConfig:getItemInfoById(id)
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
        local decorationInfo = StoreConfig:getItemInfoById(v.id)
        if decorationInfo then
            local pos = v.offset.x .. ":" .. v.offset.y .. ":" .. v.offset.z .. ":" .. v.offsetYaw
            local id = v.id
            decoration[pos] = id
        end
    end

    return decoration
end

function GameDecoration:addDecoration(entityId, id, offset, offsetYaw)
    self.decoration[entityId] = {
        entityId = entityId,
        id = id,
        offset = offset,
        offsetYaw = offsetYaw,
    }
end

function GameDecoration:updateDecorationYaw(entityId, offsetYaw)
    if not self.decoration[entityId] then
        return
    end

    self.decoration[entityId].offsetYaw = offsetYaw
end

function GameDecoration:removeFromWorld()
    for _, v in pairs(self.decoration) do
        EngineWorld:removeEntity(v.entityId)
    end
end

function GameDecoration:getDecorationByEntityId(entityId)
    return self.decoration[entityId]
end

function GameDecoration:removeDecorationFromWorld(entityId)
    EngineWorld:removeEntity(entityId)
end

function GameDecoration:removeDecorationData(entityId)
    self.decoration[entityId] = nil
end