SceneManager = {}

SceneManager.userManorInfo = {}
SceneManager.userBlockInfo = {}
SceneManager.userDecorationInfo = {}
SceneManager.userArchiveInfo = {}

SceneManager.callOnTarget = {}
SceneManager.errorTarget = {}

SceneManager.syncStoreInfoList = {}

function SceneManager:initUserManorInfo(userId, manorInfo)
    self.userManorInfo[tostring(userId)] = manorInfo
    ManorConfig:setLoadState(manorInfo.manorIndex, true)
end

function SceneManager:initUserBlockInfo(userId, blockInfo)
    self.userBlockInfo[tostring(userId)] = blockInfo
end

function SceneManager:initUserDecorationInfo(userId, decorationInfo)
    self.userDecorationInfo[tostring(userId)] = decorationInfo
end

function SceneManager:initUserArchiveInfo(userId, archiveInfo)
    self.userArchiveInfo[tostring(userId)] = archiveInfo
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

function SceneManager:getUserManorInfoByIndex(index)
    index = tonumber(index)
    for _, manor in pairs(self.userManorInfo) do
        if manor.manorIndex == index then
            return manor
        end
    end

    return nil
end

function SceneManager:getUserBlockInfo(userId)
    return self.userBlockInfo[tostring(userId)]
end

function SceneManager:getUserBlockInfoByIndex(index)
    index = tonumber(index)
    for _, manor in pairs(self.userBlockInfo) do
        if manor.manorIndex == index then
            return manor
        end
    end

    return nil
end

function SceneManager:getUserDecorationInfo(userId)
    return self.userDecorationInfo[tostring(userId)]
end

function SceneManager:getUserArchiveInfo(userId)
    return self.userArchiveInfo[tostring(userId)]
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

function SceneManager:deleteUserManorInfo(userId)
    if self.userManorInfo[tostring(userId)] ~= nil then
        ManorConfig:setLoadState(self.userManorInfo[tostring(userId)].manorIndex, false)
    end
    self.userManorInfo[tostring(userId)] = nil
end

function SceneManager:deleteUserBlockInfo(userId)
    self.userBlockInfo[tostring(userId)] = nil
end

function SceneManager:deleteUserDecorationInfo(userId)
    self.userDecorationInfo[tostring(userId)] = nil
end

function SceneManager:deleteArchiveInfo(userId)
    self.userArchiveInfo[tostring(userId)] = nil
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
    self:updateStoreInfo()
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
            --DBManager:removeCache(userId, {DbUtil.TAG_PLAYER})
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
                ManorLevelConfig:releaseHouseManagerByUserId(manor.userId)
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

function SceneManager:updateStoreInfo()
    for _, userId in pairs(self.syncStoreInfoList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            player:syncStoreInfo()
        end
        self:deleteStoreInfoList(userId)
    end
end

function SceneManager:releaseManor(userId)
    local manorInfo = SceneManager:getUserManorInfo(userId)
    if manorInfo ~= nil then
        manorInfo:removeFromWorld()
    end

    local blockInfo = SceneManager:getUserBlockInfo(userId)
    if blockInfo ~= nil then
        blockInfo:removeFromWorld()
    end

    local decorationInfo = SceneManager:getUserDecorationInfo(userId)
    if decorationInfo ~= nil then
        decorationInfo:removeFromWorld()
    end

    local archiveInfo = SceneManager:getUserArchiveInfo(userId)
    if archiveInfo ~= nil then
        -- todo: 如果需要删除
    end

    GameMatch:removeManorArea(userId)

    SceneManager:deleteUserManorInfo(userId)
    SceneManager:deleteUserBlockInfo(userId)
    SceneManager:deleteUserDecorationInfo(userId)
    SceneManager:deleteArchiveInfo(userId)
    HostApi.sendReleaseManor(userId)
    DBManager:removeCache(userId)
end

function SceneManager:masterOperateBlock(player, manorIndex, vec3, blockId, blockMeta, isAddBlock)
    if not player:isHeldItem() then
        return false
    end

    if not player.manor then
        return false
    end

    if not ManorConfig:isInManorOpenArea(manorIndex, player.manor.level, vec3) then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgPlaceManorLevelUp())
        return false
    end

    if not player.block then
        return false
    end

    local offset = ManorConfig:getOffsetByPos(manorIndex, vec3, true)

    if isAddBlock then
        local masterMeta = BlockConfig:getSimplifyBlockMasterMeta(blockId, blockMeta)
        local itemInfo = {
            type = 2,       -- 2 是方块
            pos = vec3,
            master = player,
            offset = offset,
            meta = blockMeta,
            blockId = blockId,
            masterMeta = masterMeta,
            manorIndex = manorIndex,
        }
        if not player:checkHasBlock(blockId, masterMeta, 1) then
            if player:getIsWaitForPayItems() then
                return false
            end

            local lackBlock = player:lackingBlock(blockId, blockMeta, 1)
            local items = {}
            table.insert(items, lackBlock)

            local buyLackItems = {
                type = 2,               -- 2 是方块
                items = items,
                price = lackBlock.price,
                priceCube = math.ceil(lackBlock.price / 100)
            }

            player:setBuyLackItems(buyLackItems)
            local lackItem = itemInfo
            lackItem.moneyPrice = lackBlock.price
            player:setLackItems(lackItem)
            return false
        end
        SceneManager:addPersonalGoods(player, player, itemInfo, false, 1)
        player:subBlock(blockId, masterMeta, 1)
    else
        local id, _ = player.block:checkHasBlock(offset)
        if id == 0 then
            return false
        end

        --判断门的情况
        if blockId ~= 64 then
            player.block:removeBlock(offset, blockId, blockMeta)
            if blockId == 75 then
                blockId = 76
            end

            if blockId == 124 then
                blockId = 123
            end

            local num = 1
            if BlockConfig:isDoubleSlabBlock(blockId) then
                blockId = BlockConfig:getSingleSlabIdByDoubleSlabId(blockId)
                num = 2
            end

            local masterMeta = BlockConfig:getSimplifyBlockMasterMeta(blockId, blockMeta)
            player:addBlock(blockId, masterMeta, num)
            player:addTempHotBar(blockId, masterMeta, player.hotBar.edit)
        else
            local sign = VectorUtil.hashVector3(vec3)
            player.tempDoorBlockNeedRemove[sign] = true
        end

        local startPos = ManorConfig:getManorPosByIndex(manorIndex, true)
        if not startPos then
            return false
        end
        if vec3.y == startPos.y then
            local manorLevel = player.manor.level
            local repairBlockId = ManorLevelConfig:getRepairBlockIdByLevel(manorLevel)
            local repairBlockMeta = ManorLevelConfig:getRepairBlockMetaByLevel(manorLevel)
            if repairBlockMeta and repairBlockId then
                EngineWorld:setBlock(vec3, repairBlockId, repairBlockMeta)
            end
            return false
        end
    end

    return true
end


--[[
-- [ don't need this for now, may be we will reuse in the future ]

function SceneManager:visitorOperateBlock(player, manorIndex, vec3, blockId, blockMeta, isAddBlock)
    local manorInfo = SceneManager:getUserManorInfoByIndex(manorIndex)
    if manorInfo == nil then
        return false
    end

    if not manorInfo.isMasterInGame then
        return false
    end

    local master = PlayerManager:getPlayerByUserId(manorInfo.userId)
    if not master then
        return false
    end

    if not master.manor then
        return false
    end

    if not master:checkPartner(player.userId) then
        return false
    end

    if not ManorConfig:isInManor(manorInfo.manorIndex, master:getPosition()) then
        return false
    end

    if not ManorConfig:isInManorOpenArea(manorIndex, master.manor.level, vec3) then
        return false
    end

    if not master.block then
        return false
    end

    local offset = ManorConfig:getOffsetByPos(manorIndex, vec3, true)

    if isAddBlock then
        local masterMeta = BlockConfig:getSimplifyBlockMasterMeta(blockId, blockMeta)
        local itemInfo = {
            type = 2,       -- 2 是方块
            pos = vec3,
            master = master,
            offset = offset,
            meta = blockMeta,
            blockId = blockId,
            masterMeta = masterMeta,
            manorIndex = manorIndex,
            --placeBlockInfo = placeBlockInfo,
        }
        if not player:checkHasBlock(blockId, masterMeta, 1) then
            if player:getIsWaitForPayItems() then
                return false
            end

            local lackBlock = player:lackingBlock(blockId, blockMeta, 1)
            local items = {}
            table.insert(items, lackBlock)

            local buyLackItems = {
                type = 2,               -- 2 是方块
                items = items,
                price = lackBlock.price,
                priceCube = math.ceil(lackBlock.price / 100)
            }

            player:setBuyLackItems(buyLackItems)
            local lackItem = itemInfo
            lackItem.moneyPrice = lackBlock.price
            player:setLackItems(lackItem)
            return false
        end
        SceneManager:addPersonalGoods(master, player, itemInfo, false, 1)
    else
        local id, _ = master.block:checkHasBlock(offset)
        if id == 0 then
            return false
        end

        -- 判断门的情况
        if blockId ~= 64 then
            master.block:removeBlock(offset, blockId, blockMeta)
            if blockId == 75 then
                blockId = 76
            end

            if blockId == 124 then
                blockId = 123
            end

            local num = 1
            if BlockConfig:isDoubleSlabBlock(blockId) then
                blockId = BlockConfig:getSingleSlabIdByDoubleSlabId(blockId)
                num = 2
            end

            local masterMeta = BlockConfig:getSimplifyBlockMasterMeta(blockId, blockMeta)
            player:addBlock(blockId, masterMeta, num)
            player:addTempHotBar(blockId, masterMeta, player.hotBar.edit)
        --else
        --    SceneManager:removeDoor(master, player, vec3, offset, blockId, blockMeta)
        end

        local startPos = ManorConfig:getManorPosByIndex(manorIndex, true)
        if not startPos then
            return false
        end
        if vec3.y == startPos.y then
            local manorLevel = manorInfo.level
            local repairBlockId = ManorLevelConfig:getRepairBlockIdByLevel(manorLevel)
            local repairBlockMeta = ManorLevelConfig:getRepairBlockMetaByLevel(manorLevel)
            if repairBlockMeta and repairBlockId then
                EngineWorld:setBlock(vec3, repairBlockId, repairBlockMeta)
            end
            return false
        end
    end

    return true
end

]]

function SceneManager:removeDoor(player, blockInfo, vec3, offset, blockId, blockMeta)
    -- 64是门,如果拆掉一扇门，另一扇也是拆掉
    local doorId = 0

    for y = -1, 1 do
        local dId = blockInfo:checkHasBlock(VectorUtil.newVector3i(offset.x, offset.y + y, offset.z))
        if dId == 64 then
            doorId = 64
            blockInfo:removeBlock(VectorUtil.newVector3i(offset.x, offset.y + y, offset.z), blockId, blockMeta)
        end
    end

    if doorId == 64 then
        if next(player.tempDoorBlockNeedRemove) then
            local tempDoor = {}
            for sign, isAddToHotBar in pairs(player.tempDoorBlockNeedRemove) do
                local pos = StringUtil.split(sign, ":")
                if not pos[1] or not pos[2] or not pos[3] then
                    break
                end
                local x = tonumber(pos[1])
                local y = tonumber(pos[2])
                local z = tonumber(pos[3])
                for posY = y - 1, y + 1 do
                    if x == vec3.x and posY == vec3.y and z == vec3.z then
                        player:addBlock(324, 0, 1)
                        if isAddToHotBar then
                            player:addTempHotBar(324, 0, player.hotBar.edit)
                        end
                    end
                    local tempSign = VectorUtil.hashVector3(VectorUtil.newVector3i(x, posY, z))
                    table.insert(tempDoor, tempSign)
                end
            end

            for _, tempSign in pairs(tempDoor) do
                if player.tempDoorBlockNeedRemove[tempSign] ~= nil then
                    player.tempDoorBlockNeedRemove[tempSign] = nil
                end
            end

        end
    end
end

function SceneManager:removeBlockFromWorld(startPos, endPos)
    local manorIndex = ManorConfig:getManorIndexByVec3(startPos)
    local manorInfo = SceneManager:getUserManorInfoByIndex(manorIndex)
    if manorInfo == nil then
        return false
    end

    EngineWorld:getWorld():fillAreaByBlockIdAndMate(VectorUtil.newVector3i(startPos.x ,startPos.y, startPos.z),
            VectorUtil.newVector3i(endPos.x, endPos.y, endPos.z) , 0, 0)

    local offset = ManorConfig:getOffsetByPos(manorIndex, startPos)
    if offset.y < 1 then
        SceneManager:fillAreaBaseBlock(startPos, endPos,manorInfo.level)
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

function SceneManager:placeSchematic(master, player, startPos, endPos)
    if not master.manor then
        return false
    end

    if not master.block then
        return false
    end

    local level = master.manor.level
    local manorIndex = master.manor.manorIndex
    if not ManorConfig:isInManorOpenArea(manorIndex, level, startPos) then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgPlaceManorLevelUp())
        return false
    end

    if not ManorConfig:isInManorOpenArea(manorIndex, level, endPos) then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 3, Messages:msgPlaceManorLevelUp())
        return false
    end

    if master.userId ~= player.userId then
        if not ManorConfig:isInManor(manorIndex, master:getPosition()) then
            return false
        end
    end

    local itemId = player:getHeldItemId()
    local drawingInfo = DrawingConfig:getDrawingByItemId(itemId)
    if not drawingInfo then
        return false
    end

    local blockData = DrawingConfig:getBlockDataByDrawingId(drawingInfo.id)
    local tmpData = {}
    for _, v in pairs(blockData) do
        local data = {
            id = v.blockId,
            meta = v.meta,
            num = v.num
        }
        table.insert(tmpData, data)
    end
    local itemInfo = {
        type = 1,       -- 1 是图纸
        yaw = 0,
        master = master,
        EndPos = endPos,
        StartPos = startPos,
        manorIndex = manorIndex,
        drawingId = drawingInfo.id,
        fileName = drawingInfo.fileName,
        pos = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
    }
    local lackBlocks = player:checkHasBlocks(tmpData)
    if next(lackBlocks.items) then
        if player:getIsWaitForPayItems() then
            return false
        end

        player:setBuyLackItems(lackBlocks)
        local lackItem = itemInfo
        lackItem.moneyPrice = lackBlocks.price
        player:setLackItems(lackItem)
        return false
    end
    SceneManager:addPersonalGoods(master, player, itemInfo, false, 1)
    -- 根据图纸配方扣除玩家背包方块
    player:subBlocks(tmpData)
    HostApi.sendPlaySound(player.rakssid, 328)
    return true
end

function SceneManager:syncDecorationAreaInfo()
    for _, v in pairs(self.userDecorationInfo) do
        v:pushDecorationAreaInfo()
    end
end

function SceneManager:addStoreInfoList(userId)
    self.syncStoreInfoList[userId] = userId
end

function SceneManager:deleteStoreInfoList(userId)
    self.syncStoreInfoList[userId] = nil
end

function SceneManager:addPersonalGoods(master, player, itemInfo, isLack, num)
    local itemType = tonumber(itemInfo.type)
    if itemType == 1 then
        local startPos = itemInfo.StartPos
        local endPos = itemInfo.EndPos
        local fileName = itemInfo.fileName
        local manorIndex = itemInfo.manorIndex
        local drawingId = itemInfo.drawingId
        local rotation  = math.floor(((player:getYaw()) % 360 + 180 + 45) % 360 / 90) + 1

        if rotation < 1 then
            rotation = 4
        end

        if rotation > 4 then
            rotation = 1
        end

        --缓存放置的模板信息，旋转时会用到
        local info = {
            startPos = startPos,
            endPos = endPos,
            fileName = fileName,
            manorIndex = manorIndex,
            rotation = rotation,
            drawingId = drawingId,
        }
        player:setTempSchematicInfo(info)
        local startOffset = ManorConfig:getOffsetByPos(manorIndex, startPos, true)
        local endOffset = ManorConfig:getOffsetByPos(manorIndex, endPos, true)

        local minX = math.min(startOffset.x, endOffset.x)
        local maxX = math.max(startOffset.x, endOffset.x)
        local minY = math.min(startOffset.y, endOffset.y)
        local maxY = math.max(startOffset.y, endOffset.y)
        local minZ = math.min(startOffset.z, endOffset.z)
        local maxZ = math.max(startOffset.z, endOffset.z)

        -- 回收范围内的方块
        for x = minX, maxX do
            for y = minY, maxY do
                for z = minZ, maxZ do
                    local id, meta = master.block:checkHasBlock(VectorUtil.newVector3i(x, y, z))
                    if id ~= 0 then
                        -- 判断门的情况
                        if id ~= 64 then
                            if id == 75 then
                                id = 76
                            end
                            if id == 124 then
                                id = 123
                            end
                            local addNum = 1
                            if BlockConfig:isDoubleSlabBlock(id) then
                                id = BlockConfig:getSingleSlabIdByDoubleSlabId(id)
                                addNum = 2
                            end

                            local masterMeta = BlockConfig:getSimplifyBlockMasterMeta(id, meta)
                            player:addBlock(id, masterMeta, addNum)
                            master.block:removeBlock(VectorUtil.newVector3i(x, y, z), id, meta)
                        else
                            local blockInfo = SceneManager:getUserBlockInfoByIndex(manorIndex)
                            if not blockInfo then
                                return
                            end
                            local offset = VectorUtil.newVector3i(x, y, z)
                            local vec3 = ManorConfig:getPosByOffset(manorIndex, offset, true)
                            local sign = VectorUtil.hashVector3(vec3)
                            player.tempDoorBlockNeedRemove[sign] = false
                            SceneManager:removeDoor(player, blockInfo, vec3, offset, id, meta)
                        end
                    end
                end
            end
        end

        SceneManager:removeBlockFromWorld(startPos, endPos)

        local pos = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
        SceneManager:createOrDestroyHouse(fileName, true, pos, 0, rotation)
        HostApi.sendShowRotateView(player.rakssid, 2, drawingId)

        if player.finish_guideId == GameConfig.inGuidePlaceTemplatePreId then
            local config = GuideConfig:getGuideConfigById(GameConfig.inGuidePlaceTemplateId)
            GuideManager:sendDataToClient(player,false, config.Lv)
            player.chooseTemplateId = 0
        end

-----------------------------------------------------------------------------------------------------
    elseif itemType == 2 then
        local blockId = tonumber(itemInfo.blockId)
        local blockMeta = tonumber(itemInfo.meta)
        local masterMeta = tonumber(itemInfo.masterMeta)
        local offset = itemInfo.offset
        if isLack then
            player:addBlock(blockId, masterMeta, num)
            return
        end

        -- 过滤门的方块
        if not BlockConfig:isDoorItemBlock(blockId) then
            blockMeta = BlockConfig:getMetaByOrientationTo180(blockId, blockMeta, master.block.orientation)
            master.block:addBlock(offset, blockId, blockMeta)
        else
            -- 门的情况在 PlaceDoorBlockEvent.registerCallBack(self.onPlaceDoorBlock) 处理
        end

----------------------------------------------------------------------------------------------------
    elseif itemType == 3 then
        local actorId = itemInfo.actorId
        local offset = itemInfo.offset
        local offsetYaw = itemInfo.offsetYaw
        local yaw = itemInfo.yaw
        local newPos = itemInfo.pos
        local master = itemInfo.master
        local isInteraction = itemInfo.isInteraction
        local userId = itemInfo.userId

        local entityId = EngineWorld:getWorld():addDecoration(newPos, yaw, userId, actorId)
        master.decoration:addDecoration(entityId, actorId, offset, offsetYaw)
        local info = {
            entityId = entityId,
            yaw = yaw,
            manorIndex = player.manor.manorIndex,
            id = actorId,
            newPos = newPos,
        }
        player:setTempDecorationInfo(info)
        if isInteraction == 1 then
            GameMatch.isDecorationChange = true
        end
        HostApi.sendShowRotateView(player.rakssid, 1, entityId)

----------------------------------------------------------------------------------------------------
    elseif itemType == 4 then
        for _, v in pairs(itemInfo.items) do
            local addNum = tonumber(v.needNum) - tonumber(v.haveNum)
            if addNum < 0 then
                addNum = 0
            end
            if v.itemMeta == -1 then
                local id = tonumber(v.id)
                player:addDecoration(id, addNum)
            else
                local id = tonumber(v.itemId)
                local meta = tonumber(v.itemMeta)
                player:addBlock(id, meta, addNum)
            end
        end

        if next(player.loadArchiveBlock) then
            for _, v in pairs(player.loadArchiveBlock) do
                player:subBlock(v.id, v.meta, v.num)
            end
        end

        if next(player.loadArchiveDecoration) then
            for d_id, d_num in pairs(player.loadArchiveDecoration) do
                player:subDecoration(d_id, d_num)
            end
        end

        if not player.archive or not player.block or not player.decoration then
            return
        end

        local blockData = player.archive:getArchiveBlockData(itemInfo.archiveId)
        local decorationData = player.archive:getArchiveDecorationData(itemInfo.archiveId)
        player.block:initBlocksData(blockData)
        player.decoration:initDecorationData(decorationData)
----------------------------------------------------------------------------------------------------
    elseif itemType == 5 then
        for _, v in pairs(itemInfo.items) do
            local addNum = tonumber(v.needNum) - tonumber(v.haveNum)
            if addNum < 0 then
                addNum = 0
            end
            player:addRoseNum(v.itemId, addNum)
        end
    end
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
        pos = VectorUtil.newVector3(pos.x, pos.y, pos.z)
        pos.y = pos.y + 1
        EngineWorld:destroySchematic(fileName, pos, xImage, zImage, 0, 1)
    end
end

function SceneManager:removePlayersOutManor(manorIndex)
    if manorIndex <= 0 then
        return
    end

    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.curManorIndex == manorIndex then
            local vec3 = ManorConfig:getInitPosByIndex(manorIndex, false)
            if vec3 then
                local yaw = ManorConfig:getInitYawByIndex(manorIndex)
                player:teleportPosWithYaw(vec3, yaw)
            end
        end
    end
end

return SceneManager