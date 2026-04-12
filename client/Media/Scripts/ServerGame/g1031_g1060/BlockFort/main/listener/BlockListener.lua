BlockListener = {}

function BlockListener:init()
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    PlaceItemTemplateEvent.registerCallBack(self.onPlaceItemTemplate)
    PlaceItemDecorationEvent.registerCallBack(self.onPlaceItemDecoration)
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return false
    end

    if not ManorConfig:isInManor(player.manorIndex, vec3) then
        return false
    end

    if not player.schematic then
        return false
    end

    local schematicIndex = player.schematic:getSchematicIndexByPos(vec3)
    if schematicIndex == 0 then
        return false
    end

    local schematicInfo = player.schematic.schematic[schematicIndex]
    if not schematicInfo then
        return false
    end

    PacketSender:getSender():showBlockFortBuildingInfo(player.rakssid, schematicInfo.id, schematicIndex)
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return false
    end

    local manorIndex = ManorConfig:getManorIndexByVec3(toPlacePos)
    if manorIndex == 0 then
        return false
    end

    if itemId > 900 then
        return true
    end

    return false
end

function BlockListener.onPlaceItemTemplate(userId, fileName, startPos, endPos)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return false
    end
    if not player.manor then
        return false
    end
    if not player.schematic then
        return false
    end
    local startIndex = ManorConfig:getManorIndexByVec3(startPos)
    if startIndex == 0 then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    local endIndex = ManorConfig:getManorIndexByVec3(endPos)
    if endIndex == 0 then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    if startIndex ~= endIndex then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    local level = player.manor.level
    local manorIndex = player.manor.manorIndex
    if not ManorConfig:isInManorOpenArea(manorIndex, level, startPos) then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    if not ManorConfig:isInManorOpenArea(manorIndex, level, endPos) then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    local startPosY = VectorUtil.newVector3i(startPos.x, startPos.y + 1, startPos.z)
    if not ManorConfig:checkAreaIsAirBlock(startPosY, endPos) then
        return false
    end
    local itemId = player:getHeldItemId()
    local schematicInfo = StoreConfig:getItemInfoByItemId(itemId)
    if not schematicInfo then
        return false
    end
    if not player:checkHasBagItem(schematicInfo.id, 1) then
        return false
    end
    local pos = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
    SceneManager:createOrDestroyHouse(schematicInfo.fileName, true, pos, player:getYaw(), 0)
    HostApi.sendShowRotateView(player.rakssid, 2, schematicInfo.id)
    local offset = ManorConfig:getOffsetByPos(player.manorIndex, startPos, true)
    local offset2 = ManorConfig:getOffsetByPos(player.manorIndex, endPos, true)
    local offsetYaw = ManorConfig:getOffsetYaw(player.manorIndex, player:getYaw())

    local minOffset, maxOffset = ManorConfig:sortPos(offset, offset2, true)
    player.schematic:addSchematic(schematicInfo.id, minOffset, maxOffset, offsetYaw)
    player:subItem(schematicInfo.id, 1)
    local info = {
        startPos = startPos,
        endPos = endPos,
        fileName = schematicInfo.fileName,
        manorIndex = manorIndex,
        yaw = player:getYaw(),
        id = schematicInfo.id,
    }
    player:setTempSchematicInfo(info)
    HostApi.sendPlaySound(player.rakssid, 328)
    return true
end

function BlockListener.onPlaceItemDecoration(userId, actorId, actorType, newPos, yaw, startPos, endPos)
    endPos.x = endPos.x - 1
    endPos.z = endPos.z - 1

    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return false
    end
    local decorationInfo = StoreConfig:getItemInfoById(actorId)
    if not decorationInfo then
        return false
    end
    local startIndex = ManorConfig:getManorIndexByVec3(startPos)
    if startIndex == 0 then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    local endIndex = ManorConfig:getManorIndexByVec3(endPos)
    if endIndex == 0 then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    if startIndex ~= endIndex then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    if not player.decoration then
        return false
    end
    local level = player.manor.level
    if not ManorConfig:isInManorOpenArea(player.manorIndex, level, startPos) then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    if not ManorConfig:isInManorOpenArea(player.manorIndex, level, endPos) then
        HostApi.showFloatTip(player.rakssid, "tip_blockfort_operation_in_manor")
        return false
    end
    for x = startPos.x, endPos.x do
        for z = startPos.z, endPos.z do
            local blockPos = VectorUtil.newVector3i(x, startPos.y - 1, z)
            local blockId = EngineWorld:getBlockId(blockPos)
            if blockId == 0 then
                return false
            end
        end
    end
    for x = startPos.x, endPos.x do
        for y = startPos.y, endPos.y do
            for z = startPos.z, endPos.z do
                local blockPos = VectorUtil.newVector3i(x, y, z)
                local blockId = EngineWorld:getBlockId(blockPos)
                if blockId ~= 0 then
                    return false
                end
            end
        end
    end
    local offset = ManorConfig:getOffsetByPos(player.manorIndex, newPos, false)
    local offsetYaw = ManorConfig:getOffsetYaw(player.manorIndex, yaw)
    local orientation = ManorConfig:getOrientationByIndex(player.manorIndex)

    -- 解决放置装饰时不同朝向产生的偏移
    if orientation == 90 then
        offset.x = offset.x + 1
    end
    if orientation == 270 then
        offset.z = offset.z + 1
    end
    if not player:checkHasBagItem(actorId, 1) then
        return false
    end
    local entityId = EngineWorld:getWorld():addDecoration(newPos, yaw, userId, actorId)
    player.decoration:addDecoration(entityId, actorId, offset, offsetYaw)
    player:subItem(actorId, 1)
    local info = {
        entityId = entityId,
        yaw = yaw,
        manorIndex = player.manor.manorIndex,
        id = actorId,
        newPos = newPos,
    }
    player:setTempDecorationInfo(info)
    HostApi.sendShowRotateView(player.rakssid, 1, entityId)
    return true
end

function BlockListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    return false
end


return BlockListener
