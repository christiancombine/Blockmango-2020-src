require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    PlaceItemTemplateEvent.registerCallBack(self.onPlaceItemTemplate)
    PlaceItemDecorationEvent.registerCallBack(self.onPlaceItemDecoration)
    SchemeticPlaceBlockEvent.registerCallBack(self.onSchemeticPlaceBlock)
    PlaceDoorBlockEvent.registerCallBack(self.onPlaceDoorBlock)
    BlockCanHarvestEvent.registerCallBack(self.onBlockCanHarvest)
    PlaceSlabBlockEvent.registerCallBack(self.onPlaceSlabBlock)
    BlockNeighborChangeEvent.registerCallBack(self.onBlockNeighborChange)
    BlockLeavesBreakEvent.registerCallBack(self.onBlockLeavesBreak)
    BlockRedStoneLightPoweredEvent.registerCallBack(self.onBlockRedStoneLightPoweredEvent)
    BlockPlacedByEvent.registerCallBack(self.onBlockPlacedBy)
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3)
	local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return false
    end

    local manorIndex = ManorConfig:getManorIndexByVec3(vec3)
    if manorIndex == 0 then
        return false
    end

    if player.manorIndex == manorIndex then
        -- master
        return SceneManager:masterOperateBlock(player, manorIndex, vec3, blockId, blockMeta, false)
    else
        -- visitor   [do not support for this version]
        --return SceneManager:visitorOperateBlock(player, manorIndex, vec3, blockId, blockMeta, false)
        return false
    end
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

    if player.manorIndex == manorIndex then
        -- master
        return SceneManager:masterOperateBlock(player, manorIndex, toPlacePos, itemId, meta, true)
    else
        -- visitor    [do not support for this version]
        --return SceneManager:visitorOperateBlock(player, manorIndex, toPlacePos, itemId, meta, true)
        return false
    end
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

function BlockListener.onPlaceItemTemplate(userId, fileName, startPos, endPos)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return false
    end

    local startIndex = ManorConfig:getManorIndexByVec3(startPos)
    if startIndex == 0 then
        return false
    end

    local endIndex = ManorConfig:getManorIndexByVec3(endPos)
    if endIndex == 0 then
        return false
    end

    if startIndex ~= endIndex then
        return false
    end

    if player.manorIndex == startIndex then
        -- master
        return SceneManager:placeSchematic(player, player, startPos, endPos)
    else
        -- partner
        local manorInfo = SceneManager:getUserManorInfoByIndex(startIndex)
        if not manorInfo then
            return false
        end

        if not manorInfo.isMasterInGame then
            return false
        end

        local master = PlayerManager:getPlayerByUserId(manorInfo.userId)
        if not master then
            return false
        end

        if not master:checkPartner(player.userId) then
            return false
        end

        return SceneManager:placeSchematic(master, player, startPos, endPos)
    end

    return true
end

function BlockListener.onPlaceItemDecoration(userId, actorId, actorType, newPos, yaw, startPos, endPos)
    endPos.x = endPos.x - 1
    --endPos.y = endPos.y - 1
    endPos.z = endPos.z - 1

    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return false
    end

    local decorationInfo = DecorationConfig:getDecorationInfo(actorId)
    if not decorationInfo then
        return false
    end

    local startIndex = ManorConfig:getManorIndexByVec3(startPos)
    if startIndex == 0 then
        return false
    end

    local endIndex = ManorConfig:getManorIndexByVec3(endPos)
    if endIndex == 0 then
        return false
    end

    if startIndex ~= endIndex then
        return false
    end

    if not player.decoration then
        return false
    end

    local level = player.manor.level
    if not ManorConfig:isInManorOpenArea(player.manorIndex, level, startPos) then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgPlaceManorLevelUp())
        return false
    end

    if not ManorConfig:isInManorOpenArea(player.manorIndex, level, endPos) then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgPlaceManorLevelUp())
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

    if player.manorIndex == startIndex then
        -- master
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

        local lackDecoration = player:checkHasDecoration(actorId, 1)
        local itemInfo = {
            type = 3,       -- 3是装饰
            yaw = yaw,
            pos = newPos,
            master = player,
            offset = offset,
            actorId = actorId,
            offsetYaw = offsetYaw,
            userId = player.userId,
            manorIndex = player.manorIndex,
            isInteraction = decorationInfo.isInteraction
        }
        if next(lackDecoration.items) then
            if player:getIsWaitForPayItems() then
                return false
            end

            player:setBuyLackItems(lackDecoration)
            local lackItem = itemInfo
            lackItem.moneyPrice = lackDecoration.price
            player:setLackItems(lackItem)
            return false
        end
        SceneManager:addPersonalGoods(player, player, itemInfo, false, 1)
        player:subDecoration(actorId, 1)
    else
        -- visitor
    end

    return true

end

function BlockListener.onSchemeticPlaceBlock(rakssid, vec3, blockId, meta, placeBlockParam)
    if blockId <= 0 then
        return true
    end

    for _, manor in pairs(ManorConfig.manor) do
        if manor.isLoad == true and ManorConfig:isInManor(manor.id, vec3) then
            local manorInfo = SceneManager:getUserManorInfoByIndex(manor.id)
            if manorInfo then
                local blockInfo = SceneManager:getUserBlockInfo(manorInfo.userId)
                if blockInfo then
                    local offset = ManorConfig:getOffsetByPos(manor.id, vec3, true)
                    meta = BlockConfig:getMetaByOrientationTo180(blockId, meta, blockInfo.orientation)
                    blockInfo:addBlock(offset, blockId, meta)
                end
            end
        end
    end

    return true
end

function BlockListener.onPlaceDoorBlock(userId, blockId, blockMeta, vec3)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local manorIndex = player.curManorIndex
    local offset = ManorConfig:getOffsetByPos(manorIndex, vec3, true)

    local manor = SceneManager:getUserManorInfoByIndex(manorIndex)
    if not manor then
        return
    end

    local block = SceneManager:getUserBlockInfo(manor.userId)
    if not block then
        return
    end

    blockMeta = BlockConfig:getMetaByOrientationTo180(blockId, blockMeta, block.orientation)
    block:addBlock(offset, blockId, blockMeta)
end

function BlockListener.onBlockCanHarvest(blockId)
    return blockId ~= 79
end

function BlockListener.onPlaceSlabBlock(userId, blockId, blockMeta, vec3)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local manorIndex = player.curManorIndex
    local offset = ManorConfig:getOffsetByPos(manorIndex, vec3, true)

    local manor = SceneManager:getUserManorInfoByIndex(manorIndex)
    if not manor then
        return
    end

    local block = SceneManager:getUserBlockInfo(manor.userId)
    if not block then
        return
    end

    local id , meta = block:checkHasBlock(offset)
    if id ~= 0 then
        block:removeBlock(offset, id, meta)

        blockMeta = BlockConfig:getMetaByOrientationTo180(blockId, blockMeta, block.orientation)
        block:addBlock(offset, blockId, blockMeta)
    end

    local bId = EngineWorld:getBlockId(VectorUtil.newVector3i(vec3.x, vec3.y + 1, vec3.z))
    local bMeta = EngineWorld:getBlockMeta(VectorUtil.newVector3i(vec3.x, vec3.y + 1, vec3.z))
    if bId == 0 then
        block:removeBlock(VectorUtil.newVector3i(offset.x, offset.y + 1, offset.z), id, meta)
    else
        bMeta = BlockConfig:getMetaByOrientationTo180(bId, bMeta, block.orientation)
        block:addBlock(VectorUtil.newVector3i(offset.x, offset.y + 1, offset.z), bId, bMeta)
    end
end

function BlockListener.onBlockNeighborChange(vec3, id, meta)
    local manorIndex = ManorConfig:getManorIndexByVec3(vec3)
    if manorIndex == 0 then
        return
    end

    local blockInfo = SceneManager:getUserBlockInfoByIndex(manorIndex)
    if not blockInfo then
        return
    end

    local player = PlayerManager:getPlayerByUserId(blockInfo.userId)
    if not player then
        return
    end

    local offset = ManorConfig:getOffsetByPos(manorIndex, vec3, true)

    if id == 64 then
        SceneManager:removeDoor(player, blockInfo, vec3, offset, id, meta)
        return
    end

    if id == 75 then
        id = 76
    end

    if id == 124 then
        id = 123
    end

    local checkBlockId = blockInfo:checkHasBlock(offset)
    if checkBlockId ~= 0 then
        local masterMeta = BlockConfig:getSimplifyBlockMasterMeta(id, meta)
        player:addBlock(id, masterMeta, 1)
        blockInfo:removeBlock(offset, id, meta)
    end
end

function BlockListener.onBlockLeavesBreak(blockId)
    return false
end

function BlockListener.onBlockPlacedBy(vec3, id, meta)
    local manorIndex = ManorConfig:getManorIndexByVec3(vec3)
    if manorIndex == 0 then
        return
    end

    local blockInfo = SceneManager:getUserBlockInfoByIndex(manorIndex)
    if not blockInfo then
        return
    end

    local offset = ManorConfig:getOffsetByPos(manorIndex, vec3, true)
    local checkBlockId = blockInfo:checkHasBlock(offset)
    if checkBlockId ~= 0 then
        meta = BlockConfig:getMetaByOrientationTo180(checkBlockId, meta, blockInfo.orientation)
        blockInfo:blockPlacedBy(offset, id, meta)
    end
end

function BlockListener.onBlockRedStoneLightPoweredEvent(blockId, pos)
    return false
end

return BlockListener
--endregion
