require "config.ManorNpcConfig"

FurnitureConfig = {}
FurnitureConfig.furniture = {}
FurnitureConfig.furnitureShow = {}
FurnitureConfig.putStatus = {}
FurnitureConfig.putStatus.START = 0
FurnitureConfig.putStatus.SURE = 1
FurnitureConfig.putStatus.CANCEL = 2
FurnitureConfig.putStatus.EDIT_START = 3
FurnitureConfig.putStatus.EDIT_SURE = 4
FurnitureConfig.putStatus.EDIT_CANCEL = 5

function FurnitureConfig:init(config)
    self:initFurniture(config.furniture)
    self:initFurnitureShow(config.furnitureShow)
end

function FurnitureConfig:initFurnitureShow(config)
    for i, v in pairs(config) do
        local furnitureShow = {}
        furnitureShow.actor = tostring(v.actor)
        furnitureShow.bodyId = tonumber(v.bodyId)
        furnitureShow.position = VectorUtil.newVector3(v.position[1], v.position[2], v.position[3])
        furnitureShow.yaw = tonumber(v.position[4])
        self.furnitureShow[i] = furnitureShow
    end
end

function FurnitureConfig:prepareFurnitureShow()
    for i, v in pairs(self.furnitureShow) do
        EngineWorld:addSessionNpc(v.position, v.yaw, ManorNpcConfig.SessionType_MANOR_FURNITURE, v.actor, function(entity)
            entity:setActorBodyId(v.bodyId or "")
        end)
    end
end

function FurnitureConfig:initFurniture(config)
    for i, v in pairs(config) do
        local furniture = {}
        furniture.id = tonumber(v.id)
        furniture.name = tostring(v.name)
        furniture.icon = tostring(v.icon)
        furniture.coinId = tonumber(v.price[1])
        furniture.blockymodsPrice = tonumber(v.price[2])
        furniture.blockmanPrice = tonumber(v.price[3])
        furniture.charmValue = tonumber(v.charmValue)
        furniture.bodyId = tostring(v.bodyId)
        self.furniture[i] = furniture
    end
end

function FurnitureConfig:getFurnitureById(id)
    for i, v in pairs(self.furniture) do
        if v.id == id then
            return v
        end
    end
    return nil
end

function FurnitureConfig:newFurniture(player)
    local furnitures = {}
    for i, v in pairs(self.furniture) do
        local furniture = ManorFurniture.new()
        furniture.price = v.blockymodsPrice
        furniture.id = v.id
        furniture.icon = v.icon
        furniture.name = v.name
        furniture.charm = v.charmValue
        furniture.currencyType = v.coinId
        furniture.operationType = 0
        for j, f in pairs(player.payedFurniture) do
            if j == v.id then
                furniture.operationType = 1
            end
        end

        for j, f in pairs(player.placeFurniture) do
            if j == v.id then
                furniture.operationType = 2
            end
        end

        furnitures[i] = furniture
    end

    return furnitures
end

function FurnitureConfig:savePlayerFurnitureInfo(furnitureId, player, offsetVec3, yaw)
    local data = {}
    data.tempId = furnitureId
    data.offsetVec3 = offsetVec3
    data.yaw = yaw
    player:initPlaceFurnitureInfo(data)
end

function FurnitureConfig:isFurniturePayed(player, furnitureId)
    for i, v in pairs(player.payedFurniture) do
        if furnitureId == v then
            return true
        end
    end

    return false
end

function FurnitureConfig:placeFurnitureStart(player, furniture)
    if YardConfig:isInYard(player.manorIndex, player.yardLevel, player:getPosition()) == false then
        HostApi.sendManorOperationTip(player.rakssid, "gui_manor_place_and_build_message")
        return
    end
    player.positionBeforePlaceFurniture = player:getPosition()
    EngineWorld:changePlayerActor(player, furniture.name, furniture.bodyId)
end

function FurnitureConfig:placeFurnitureSure(player, furnitureId, furniture)
    if YardConfig:isInYard(player.manorIndex, player.yardLevel, player:getPosition()) == false then
        player:restoreActor()
        HostApi.sendManorOperationTip(player.rakssid, "gui_manor_place_and_build_message")
        return
    end
    local furnitures = GameMatch.furnitureEntityId[tostring(player.userId)]
    if furnitures ~= nil then
        if furnitures[furnitureId] ~= nil then
            if furnitures[furnitureId].entityId ~= nil then
                EngineWorld:removeEntity(furnitures[furnitureId].entityId)
                GameMatch.furnitureEntityId[tostring(player.userId)][furnitureId].entityId = nil
            end
        end
    end
    local vec3 = player:getPosition()
    local yaw = player:getYaw()
    local entityId = EngineWorld:addSessionNpc(vec3, yaw, ManorNpcConfig.SessionType_MANOR_FURNITURE, furniture.name, function(entity)
        entity:setActorBodyId(furniture.bodyId or "")
        entity:setSessionContent(tostring(furnitureId) or "")
    end)

    local offsetVec3 = YardConfig:getOffsetByVec3(player.manorIndex, vec3)
    local data = {}
    data.entityId = entityId
    data.actorName = furniture.name
    data.offsetVec3 = offsetVec3
    data.yaw = yaw
    data.tempId = furnitureId
    data.state = 1
    data.price = furniture.blockymodsPrice
    GameMatch:initFurnitureEntityId(player.userId, data)

    FurnitureConfig:savePlayerFurnitureInfo(furnitureId, player, offsetVec3, yaw)
    HostApi.operationFurniture(player.userId, player.manorId, furnitureId, offsetVec3, yaw, 1)
    player:restoreActor()
    ManorNpcConfig:updateManorInfo(player)
end

function FurnitureConfig:placeFurnitureCancel(player)
    player:restoreActor()
end

function FurnitureConfig:editFurnitureStart(player, furnitureId, furniture)
    local furnitures = GameMatch.furnitureEntityId[tostring(player.userId)]
    if furnitures ~= nil then
        if furnitures[furnitureId] ~= nil then
            if furnitures[furnitureId].entityId ~= nil then
                player.placeFurnitureEditTemp = furnitures[furnitureId]
                EngineWorld:removeEntity(furnitures[furnitureId].entityId)
                GameMatch.furnitureEntityId[tostring(player.userId)][furnitureId].entityId = nil
            end
        end
    end
    local vec3 = YardConfig:getVec3ByOffset(player.manorIndex, player.placeFurnitureEditTemp.offsetVec3)
    player.positionBeforePlaceFurniture = vec3
    player:teleportPos(vec3)
    EngineWorld:changePlayerActor(player, furniture.name, furniture.bodyId)
end

function FurnitureConfig:editFurnitureSure(player, furnitureId, furniture)
    if YardConfig:isInYard(player.manorIndex, player.yardLevel, player:getPosition()) == false then
        local vec3 = YardConfig:getVec3ByOffset(player.manorIndex, player.placeFurnitureEditTemp.offsetVec3)
        local yaw = player.placeFurnitureEditTemp.yaw
        local entityId = EngineWorld:addSessionNpc(vec3, yaw, ManorNpcConfig.SessionType_MANOR_FURNITURE, furniture.name, function(entity)
            entity:setActorBodyId(furniture.bodyId or "")
            entity:setSessionContent(tostring(furnitureId) or "")
        end)

        local data = {}
        data.entityId = entityId
        data.actorName = furniture.name
        data.offsetVec3 = player.placeFurnitureEditTemp.offsetVec3 or VectorUtil.ZERO
        data.yaw = yaw
        data.tempId = furnitureId
        data.state = 1
        data.price = furniture.blockymodsPrice
        GameMatch:initFurnitureEntityId(player.userId, data)

        FurnitureConfig:savePlayerFurnitureInfo(furnitureId, player, data.offsetVec3, yaw)
        HostApi.operationFurniture(player.userId, player.manorId, furnitureId, data.offsetVec3, yaw, 1)
        player:restoreActor()
        player:teleportPos(player.positionBeforePlaceFurniture)
        player.placeFurnitureEditTemp = nil
        ManorNpcConfig:updateManorInfo(player)
        HostApi.sendManorOperationTip(player.rakssid, "gui_manor_place_and_build_message")
        return
    end

    local vec3 = player:getPosition()
    local yaw = player:getYaw()
    local offsetVec3 = YardConfig:getOffsetByVec3(player.manorIndex, vec3)
    local entityId = EngineWorld:addSessionNpc(vec3, yaw, ManorNpcConfig.SessionType_MANOR_FURNITURE, furniture.name, function(entity)
        entity:setActorBodyId(furniture.bodyId or "")
        entity:setSessionContent(tostring(furnitureId) or "")
    end)

    local data = {}
    data.entityId = entityId
    data.actorName = furniture.name
    data.offsetVec3 = offsetVec3
    data.yaw = yaw
    data.tempId = furnitureId
    data.state = 1
    data.price = furniture.blockymodsPrice
    GameMatch:initFurnitureEntityId(player.userId, data)

    FurnitureConfig:savePlayerFurnitureInfo(furnitureId, player, offsetVec3, yaw)
    HostApi.operationFurniture(player.userId, player.manorId, furnitureId, offsetVec3, yaw, 1)
    player:restoreActor()
    player.placeFurnitureEditTemp = nil
    ManorNpcConfig:updateManorInfo(player)
end

function FurnitureConfig:editFurnitureCancel(player, furnitureId, furniture)
    if not player.placeFurnitureEditTemp then
        return
    end
    local vec3 = YardConfig:getVec3ByOffset(player.manorIndex, player.placeFurnitureEditTemp.offsetVec3)
    local yaw = player.placeFurnitureEditTemp.yaw
    local entityId = EngineWorld:addSessionNpc(vec3, yaw, ManorNpcConfig.SessionType_MANOR_FURNITURE, furniture.name, function(entity)
        entity:setActorBodyId(furniture.bodyId or "")
        entity:setSessionContent(tostring(furnitureId) or "")
    end)

    local data = {}
    data.entityId = entityId
    data.actorName = furniture.name
    data.offsetVec3 = player.placeFurnitureEditTemp.offsetVec3
    data.yaw = yaw
    data.tempId = furnitureId
    data.state = 1
    data.price = furniture.blockmanPrice
    data.price = furniture.blockymodsPrice
    GameMatch:initFurnitureEntityId(player.userId, data)

    player.placeFurnitureEditTemp = nil
    player:restoreActor()
    player:teleportPos(player.positionBeforePlaceFurniture)
end

function FurnitureConfig:recycleFurniture(player)
    local furnitures = GameMatch.furnitureEntityId[tostring(player.userId)]
    if furnitures ~= nil then
        for i, v in pairs(furnitures) do
            EngineWorld:removeEntity(v.entityId)
        end
        GameMatch.furnitureEntityId[tostring(player.userId)] = nil
        -- HostApi.log("==== LuaLog: FurnitureConfig:recycleFurniture .. GameMatch.furnitureEntityId [" ..tostring(player.userId) .."] has deleted ! ")
    end

    if player.placeFurniture ~= nil then
        for i, v in pairs(player.placeFurniture) do
            player.payedFurniture[i] = i
            local vec3 = VectorUtil.newVector3(0, 0, 0)
            HostApi.operationFurniture(player.userId, player.manorId, i, vec3, 0, 0)
        end
        player.placeFurniture = {}
        -- HostApi.log("==== LuaLog: FurnitureConfig:recycleFurniture .. player placeFurniture [" ..tostring(player.userId) .."] has deleted ! ")
    end
    ManorNpcConfig:updateManorInfo(player)
end

return FurnitureConfig