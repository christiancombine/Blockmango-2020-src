GameManor = class()


function GameManor:ctor(manorIndex, userId)
    self.userId = userId
    self.manorIndex = manorIndex or 0
    self:reset()
    self.release_status = Define.manorRelese.Not
end

function GameManor:reset()
    self.time = 0
    self.visitor = {}
    self.manorBlock = {}
    self.updateBlock = {}
    self.furnace = {}
    self.furnaceFill = {}
    self.furnaceNum = 0
    self.chest = {}
    self.chestIsFill = {}
    self.boxNum = 0
    self.isInitBox = false
    self.treeInfo = {}
    self.dirtInfo = {}
    self.cropsInfo = {}
    self.posOriginX = 0
    self.posOriginY = 0
    self.posOriginZ = 0
    self.flowingHigh = {}
    self.waterSource = {}
    self.OtherSchmetic = {}
    self.OtherSchmeticDelete = {}
    self.attachUpdateBlock = {}
    self.loading = false
    self.newKeyLock = {}
    self.signPost = {}
    self.signPostNum = 0
    self.isSignPostEdited = {}
    self.is_need_update_sign = false
    self.logBlock = {}
    self.loadBlockRecord = {}
    self.isNotInPartyCreateOrOver = true
    self.ActorCache = {}
    self.ActorEntity = {}
    self.actorNum = 0
    self.isInRoomForceRelease = false
end

function GameManor:clearMem()
    self.visitor = nil
    self.manorBlock = nil
    self.updateBlock = nil
    self.furnace = nil
    self.furnaceFill = nil
    self.chest = nil
    self.chestIsFill = nil
    self.treeInfo = nil
    self.dirtInfo = nil
    self.cropsInfo = nil
    self.flowingHigh = nil
    self.waterSource = nil
    self.OtherSchmetic = nil
    self.OtherSchmeticDelete = nil
    self.attachUpdateBlock = nil
    self.signPost = nil
    self.isSignPostEdited = nil
    self.logBlock = nil
    self.loadBlockRecord = nil
    self.ActorCache = nil
    self.ActorEntity = nil
end

function GameManor:addTime(time)
    self.time = self.time + time
end

function GameManor:refreshTime()
    if self.isNotInPartyCreateOrOver == true then
        self.time = 0
    else
        HostApi.log("GameManor refreshTime isNotInPartyCreateOrOver manorId: " .. tostring(self.userId))
    end
end

function GameManor:getTime()
    return self.time
end

function GameManor:masterLogin()
    HostApi.log("masterLogin " .. tostring(self.userId))
end

function GameManor:masterLogout()
    HostApi.log("masterLogout " .. tostring(self.userId))
end

function GameManor:getMasterState()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    return player ~= nil
end

function GameManor:visitorInHouse(userId)
    HostApi.log("visitorInHouse selfUserId: " .. tostring(self.userId) .. "vistor: " .. tostring(userId))
    self.visitor[userId] = userId
end

function GameManor:visitorLeaveHouse(userId)
    HostApi.log("visitorLeaveHouse selfUserId: " .. tostring(self.userId) .. "vistor: " .. tostring(userId))
    self.visitor[userId] = nil
end

function GameManor:checkVisitor()
    if self.visitor then
        for k, userId in pairs(self.visitor) do
            local player = PlayerManager:getPlayerByUserId(userId)
            if player == nil then
                HostApi.log("checkVisitor visitorLeaveHouse info1: " .. tostring(userId))
                self.visitor[k] = ""
            else
                if player.isNotInPartyCreateOrOver == false then
                    HostApi.log("checkVisitor visitorLeaveHouse info2: " .. tostring(userId))
                    self.visitor[k] = ""
                end
            end
        end

        self.visitor = TableUtil.removeElementByFun(self.visitor, function (key, value)
            return value == ""
        end)
    end
end

function GameManor:isEmptyVisitor()
    return self.visitor == nil or next(self.visitor) == nil
end

-- pos start

function GameManor:initChunkIndex()
    if self.isNotInPartyCreateOrOver == false then
        HostApi.log("GameManor:initChunkIndex manor is in partyCreateOrOver manorId: " .. tostring(self.userId))
        return
    end

    self.release_status = Define.manorRelese.Not
    self.isInRoomForceRelease = false
    self.OtherSchmeticDelete = {}

    HostApi.log("GameManor:initChunkIndex manorId: " .. tostring(self.userId))

    local target_pos = GameConfig:getInitPosByManorIndex(self.manorIndex)
    local origin_pos = GameConfig:getOriginPosByManorIndex(self.manorIndex)
    if origin_pos and target_pos then
        self.posOriginX = origin_pos.x - GameConfig.manorMax / 2
        self.posOriginY = origin_pos.y - GameConfig.manorMax / 2
        self.posOriginZ = origin_pos.z - GameConfig.manorMax / 2

        origin_pos.x = origin_pos.x - GameConfig.initSchematicOffset
        origin_pos.y = origin_pos.y - GameConfig.initSchematicOffset
        origin_pos.z = origin_pos.z - GameConfig.initSchematicOffset

        EngineWorld:createSchematic(GameConfig.initSchemetic, origin_pos, false, false, true, 0, 0, 0, self.manorIndex)

        self.OtherSchmetic = TableUtil.copyTable(GameConfig.OtherSchmetic)
    end
end

function GameManor:isIn(vec3)
    local rPos = self:aPosToRPos(vec3)
    return rPos.x >= 0 and rPos.x <= GameConfig.manorMax
            and rPos.y >= 0 and rPos.y <= GameConfig.manorMax
            and rPos.z >= 0 and rPos.z <= GameConfig.manorMax
end


function GameManor:isNormal(vec3)
    local rPos = self:aPosToRPos(vec3)
    return rPos.x >= 0 and rPos.x <= GameConfig.manorMax
            and vec3.y <= 255
            and rPos.z >= 0 and rPos.z <= GameConfig.manorMax
end

-- pos end

-- GameManorUtil start

function GameManor:rPosToAPos(vec3)
    local x = self.posOriginX + vec3.x
    local y = self.posOriginY + vec3.y
    local z = self.posOriginZ + vec3.z

    return VectorUtil.newVector3i(x, y, z)
end

function GameManor:aPosToRPos(vec3)
    local x = vec3.x - self.posOriginX
    local y = vec3.y - self.posOriginY
    local z = vec3.z - self.posOriginZ

    return VectorUtil.newVector3i(x, y, z)
end

-- GameManorUtil end

-- block start

function GameManor:update()

    local target_pos = GameConfig:getInitPosByManorIndex(self.manorIndex)
    if self.release_status == Define.manorRelese.Start and self.OtherSchmeticDelete and target_pos then
        local schematicDeleteNum = 0
        for key, island in pairs(self.OtherSchmeticDelete) do
            local pos = VectorUtil.newVector3i(island.pos[1], island.pos[2], island.pos[3])
            pos.x = pos.x + target_pos.x - tonumber(island.offset)
            pos.y = pos.y + target_pos.y - tonumber(island.offset)
            pos.z = pos.z + target_pos.z - tonumber(island.offset)
            EngineWorld:destroySchematic(island.file, pos, false, false, 0, 0, -1)
            schematicDeleteNum = schematicDeleteNum + 1
            self.OtherSchmeticDelete[key] = ""
            if schematicDeleteNum >= Define.SchematicCreateOrDestroyNum then
                break
            end
        end

        self.OtherSchmeticDelete = TableUtil.removeElementByFun(self.OtherSchmeticDelete, function (key, value)
            return value == ""
        end)

        local startDeleteBlockTime = tonumber(HostApi.curTimeString())
        for hash, block in pairs(self.manorBlock) do
            if block ~= nil then
                local x, y, z = GameConfig:decodeHash(hash)
                local vec3i = VectorUtil.newVector3i(x, y, z)
                local aPos = self:rPosToAPos(vec3i)
                local invChest = EngineWorld:getWorld():getInventory(BlockID.CHEST, aPos)
                if invChest then
                    invChest:clear()
                end
                local invFurnaceIdle = EngineWorld:getWorld():getInventory(BlockID.FURNACE_IDLE, aPos)
                if invFurnaceIdle then
                    invFurnaceIdle:clear()
                end
                local invFurnaceBurning = EngineWorld:getWorld():getInventory(BlockID.FURNACE_BURNING, aPos)
                if invFurnaceBurning then
                    invFurnaceBurning:clear()
                end
                -- EngineWorld:setBlockToAir(aPos)
                EngineWorld:setBlock(aPos, 0, 0, 0)

                self.manorBlock[hash] = ""
                local endDeleteBlockTime = tonumber(HostApi.curTimeString())
                if endDeleteBlockTime - startDeleteBlockTime > 100 then
                    break
                end
            end
        end

        self.manorBlock = TableUtil.removeElementByFun(self.manorBlock, function (key, value)
            return value == ""
        end)

        if next(self.OtherSchmeticDelete) == nil and next(self.manorBlock) == nil then
            HostApi.log("Define.manorRelese.End " .. tostring(self.userId))
            self.release_status = Define.manorRelese.End
        end
    end

    if self.release_status == Define.manorRelese.Not and self.OtherSchmetic and target_pos then
        local schematicCreateNum = 0
        for key, island in pairs(self.OtherSchmetic) do
            local pos = VectorUtil.newVector3i(island.pos[1], island.pos[2], island.pos[3])
            pos.x = pos.x + target_pos.x - tonumber(island.offset)
            pos.y = pos.y + target_pos.y - tonumber(island.offset)
            pos.z = pos.z + target_pos.z - tonumber(island.offset)
            EngineWorld:createSchematic(island.file, pos, false, false, true, 0, 0, 0, self.manorIndex)
            schematicCreateNum = schematicCreateNum + 1
            self.OtherSchmetic[key] = ""
            if schematicCreateNum >= Define.SchematicCreateOrDestroyNum then
                break
            end
        end

        self.OtherSchmetic = TableUtil.removeElementByFun(self.OtherSchmetic, function (key, value)
            return value == ""
        end)
    end

    if self.release_status == Define.manorRelese.Not then
        local startUpdateBlockTime = tonumber(HostApi.curTimeString())
        for _, block in pairs(self.updateBlock) do
            if block.sync == false then
                local aPos = self:rPosToAPos(block.pos)
                if GameConfig:isAttachmentBlock(block.id)
                        or CropsConfig:checkCropsByBlockId(block.id)
                        or GameConfig:isDoorBlock(block.id) then
                    local item = {}
                    item.aPos = aPos
                    item.id = block.id
                    item.meta = block.meta
                    item.sync = false
                    table.insert(self.attachUpdateBlock, item)
                else
                    local set = true
                    if block.id == BlockID.CHEST
                            or block.id == BlockID.FURNACE_IDLE
                            or block.id == BlockID.FURNACE_BURNING
                            or block.id == BlockID.DOOR_WOOD
                            or block.id == BlockID.DOOR_IRON
                            or block.id == BlockID.WHITE_WOOD_DOOR then
                        if GameConfig:checkPosHasBlock(block.id, aPos) then
                            set = false
                        end
                    end

                    if set then
                        EngineWorld:setBlock(aPos, block.id, block.meta, 3)
                    end
                    self:recordBlock(aPos, block.id, block.meta)

                    if block.id == BlockID.CHEST then
                        self:recordChest(aPos, true)
                    end

                    if block.id == BlockID.FURNACE_IDLE
                            or block.id == BlockID.FURNACE_BURNING then
                        self:recordFurnace(aPos, true)
                    end
                end

                if block.id == BlockID.DIRT then
                    self:initDirtTimeByPos(block.pos)
                end

                if (block.id == BlockID.WATER_MOVING or block.id == BlockID.WATER_STILL) and block.meta == 0 then
                    self:recordWaterSource(aPos)
                end

                block.sync = true
                local endUpdateBlockTime = tonumber(HostApi.curTimeString())
                if endUpdateBlockTime - startUpdateBlockTime > 100 then
                    break
                end
            end
        end
        self.updateBlock = TableUtil.removeElementByFun(self.updateBlock, function (key, value)
            return value.sync == true
        end)
    end
end

function GameManor:updateAttachBlock()
    if next(self.updateBlock) == nil and self.release_status == Define.manorRelese.Not then
        for _, block in pairs(self.attachUpdateBlock) do
            if block.sync == false then
                block.sync = true
                EngineWorld:setBlock(block.aPos, block.id, block.meta, 0)
                self:recordBlock(block.aPos, block.id, block.meta)

                if block.id == BlockID.SIGN_POST or block.id == BlockID.SIGN_WALL then
                    if DBManager:isLoadDataFinished(self.userId, DbUtil.TAG_SIGN) then
                        self:initSignPostText(block.aPos)
                    else
                        self.is_need_update_sign = true
                        HostApi.log("DbUtil.TAG_SIGN loadDataFinished is false")
                    end
                end
                if block.id == BlockID.SAPLING then
                    local rPos = self:aPosToRPos(block.aPos)
                    self:initTreeTimeByPos(rPos, block.meta)
                end
            end
        end

        self.attachUpdateBlock = TableUtil.removeElementByFun(self.attachUpdateBlock, function (key, value)
            return value.sync == true
        end)

        if next(self.attachUpdateBlock) == nil then
            if self.loading then
                self.loading = false
                self:checkGameLoading()
            end

            self:initCropsTime()
        end
    end
end

function GameManor:checkGameLoading()
    local master = PlayerManager:getPlayerByUserId(self.userId)
    if master then
        GamePacketSender:sendSkyBlockLoadingResult(master.rakssid, self.loading)
    end

    if self.visitor then
        for _, userId in pairs(self.visitor) do
            local player = PlayerManager:getPlayerByUserId(userId)
            if player then
                GamePacketSender:sendSkyBlockLoadingResult(player.rakssid, self.loading)
            end
        end
    end
end

function GameManor:initBlockFromDB(data, subKey)
    if not data.__first then

        self.loading = true
        self:checkGameLoading()
        local blockCount = 0
        for key, block in pairs(data.block) do
            if type(key) == "number" then
                local dataString = StringUtil.split(block, ":")
                if dataString then
                    local x = tonumber(dataString[1])
                    local y = tonumber(dataString[2])
                    local z = tonumber(dataString[3])
                    local blockId = tonumber(dataString[4])
                    local blockMeta = tonumber(dataString[5])
                    local vec3 = VectorUtil.newVector3i(x, y, z)
                    local item = {}
                    item.pos = vec3
                    item.id = blockId
                    item.meta = blockMeta
                    item.sync = false
                    table.insert(self.updateBlock, item)
                    blockCount = blockCount + 1
                end
            elseif type(key) == "string" then
                local dataKeyString = StringUtil.split(key, ":")
                local dataValueString = StringUtil.split(block, ":")
                if dataKeyString and dataValueString then
                    local x = tonumber(dataKeyString[1])
                    local y = tonumber(dataKeyString[2])
                    local z = tonumber(dataKeyString[3])
                    local blockId = tonumber(dataValueString[1])
                    local blockMeta = tonumber(dataValueString[2])
                    local vec3 = VectorUtil.newVector3i(x, y, z)
                    local item = {}
                    item.pos = vec3
                    item.id = blockId
                    item.meta = blockMeta
                    item.sync = false
                    table.insert(self.updateBlock, item)
                    blockCount = blockCount + 1
                end
            end

        end
        local endPos = string.find(subKey, 'v2')

        if not endPos then
            local newKey = subKey .. "v2"
            self.newKeyLock[newKey] = nil
            self.logBlock[newKey] = blockCount
            self.loadBlockRecord[newKey] = nil
            self.loadBlockRecord[newKey] = TableUtil.copyTable(data.block)
        else
            self.logBlock[subKey] = blockCount
            self.loadBlockRecord[subKey] = nil
            self.loadBlockRecord[subKey] = TableUtil.copyTable(data.block)
        end
        HostApi.log("initBlockFromDB userId: " .. tostring(self.userId) .. " subkey: " .. subKey .. " blockCount: " .. blockCount)
    else
        -- try to get old block key
        local endPos = string.find(subKey, 'v2')
        if endPos then
            local oldKey = string.sub(subKey, 1, endPos - 1)
            if oldKey then
                --HostApi.log("try to get oldKey " .. tostring(self.userId) .. " " .. subKey .. " " .. oldKey)
                if not DBManager:isLoadDataFinished(self.userId, oldKey) then
                    self.newKeyLock[subKey] = true
                    DbUtil.getPlayerData(self.userId, oldKey)
                end
            end
        else
            local newKey = subKey .. "v2"
            self.newKeyLock[newKey] = nil
        end
    end
end

function GameManor:isLoadingBlock()
    if self.updateBlock and next(self.updateBlock) == nil and self.attachUpdateBlock and next(self.attachUpdateBlock) == nil then
        return false
    end
    return true
end

function GameManor:recordBlock(vec3, blockId, blockMeta)

    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    local value = blockId .. ":" .. blockMeta

    if blockMeta == 0 and (blockId == BlockID.WATER_MOVING or
            blockId == BlockID.WATER_STILL or
            blockId == BlockID.LAVA_MOVING or
            blockId == BlockID.LAVA_STILL) then

        self.flowingHigh[hash] = true
    end

    if self.release_status ~= Define.manorRelese.Not then
        return
    end

    self.manorBlock[hash] = value
end


function GameManor:recordWaterSource(vec3)

    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z

    self.waterSource[hash] = true
end

function GameManor:checkNewWaterSource(vec3)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z

    if self.waterSource and next(self.waterSource) ~= nil and self.waterSource[hash] == nil then
        return true
    end

    return false
end

function GameManor:checkFlowHigh(vec3)
    local rPos = self:aPosToRPos(vec3)

    for x = rPos.x - 1, rPos.x + 1 do
        for y = rPos.y, rPos.y + 1 do
            for z = rPos.z - 1, rPos.z + 1 do
                local hash = x .. ":" .. y .. ":" .. z
                if self.flowingHigh[hash] == true then
                    return true
                end
            end
        end
    end
    return false
end

function GameManor:clearBlockDb()
    self.release_status = Define.manorRelese.Start

    local origin_pos = GameConfig:getOriginPosByManorIndex(self.manorIndex)
    local startPos = VectorUtil.newVector3i(origin_pos.x - GameConfig.manorMax / 2, origin_pos.y - GameConfig.manorMax / 2, origin_pos.z - GameConfig.manorMax / 2)
    local endPos = VectorUtil.newVector3i(origin_pos.x + GameConfig.manorMax / 2, origin_pos.y + GameConfig.manorMax / 2, origin_pos.z + GameConfig.manorMax / 2)
    HostApi.log("clearBlockDb " .. startPos.x .. " " .. startPos.y .. " " .. startPos.z .. " " .. endPos.x .. " " .. endPos.y .. " " .. endPos.z)

    local target_pos = GameConfig:getInitPosByManorIndex(self.manorIndex)

    if origin_pos and target_pos then
        self.posOriginX = origin_pos.x - GameConfig.manorMax / 2
        self.posOriginY = origin_pos.y - GameConfig.manorMax / 2
        self.posOriginZ = origin_pos.z - GameConfig.manorMax / 2

        origin_pos.x = origin_pos.x - GameConfig.initSchematicOffset
        origin_pos.y = origin_pos.y - GameConfig.initSchematicOffset
        origin_pos.z = origin_pos.z - GameConfig.initSchematicOffset

        EngineWorld:destroySchematic(GameConfig.initSchemetic, origin_pos, false, false, 0, 0, -1)

        self.OtherSchmeticDelete = TableUtil.copyTable(GameConfig.OtherSchmetic)
    end
end

-- block end

-- chest start

function GameManor:initChestDataFromDB(data)
    if not data.__first then
        self.chest = data.chest or {}
        self.isInitBox = data.isInitBox or false
    end

    if self.isInitBox == false then
        self:initBox()
        self.isInitBox = true
    end

    if self.chest then
        self.boxNum = 0
        for key, chest in pairs(self.chest) do
            local x, y, z = GameConfig:decodeHash(key)
            if not GameConfig:isInitBoxPos(x, y, z) then
                self.boxNum = self.boxNum + 1
            end
        end
    end
    self.chestIsFill = {}
    HostApi.log("initChestDataFromDB manorId: " .. tostring(self.userId) .. " boxNum: " .. self.boxNum)
end

function GameManor:recordChest(vec3, notAdd)
    if self:checkBoxLimit() then
        HostApi.log("recordChest " .. tostring(self.userId) .. "boxNum: " .. self.boxNum)
        -- return
    end
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.chest[hash] == nil then
        self.chest[hash] = {}
    end

    if notAdd then
        return
    end

    -- set chest to empty chest to stop cheat
    self.chest[hash] = {}

    self.boxNum = self.boxNum + 1
end

function GameManor:removeChest(vec3)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.chest[hash] ~= nil then
        self.chest[hash] = nil
        self.boxNum = self.boxNum - 1
    end
end

function GameManor:checkBoxLimit()
    return self.boxNum >= GameConfig.boxLimit
end

function GameManor:initBox()
    local initInfo = GameConfig.boxInit
    if initInfo then
        for _, info in pairs(initInfo) do
            local hash = tonumber(info.pos[1]) .. ":" .. tonumber(info.pos[2]) .. ":" .. tonumber(info.pos[3])
            local items = info.items
            if self.chest[hash] == nil and items then
                self.chest[hash] = {}
                for k, v in pairs(items) do
                    local item = {}
                    item.id = tonumber(v.id)
                    item.num = tonumber(v.num)
                    item.meta = tonumber(v.meta)
                    table.insert(self.chest[hash], item)
                end
            end
        end
    end
end

function GameManor:fillOpenChest(vec3, inv)
    if inv == nil then
        return
    end

    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        ---主人不在家
        inv:clearChest()
        return
    end

    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_CHEST) then
        ---没有拉取到数据
        inv:clearChest()
        return
    end

    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    HostApi.log("fillOpenChest userId: " .. tostring(self.userId) .. " " .. hash)

    if self.chest[hash] == nil then
        ---没有缓存记录
        inv:clearChest()
        return
    end

    if self.chestIsFill[hash] ~= nil then
        return
    end

    inv:clearChest()
    local itemSlot = 0
    for i, v in pairs(self.chest[hash]) do
        if v.id ~= nil and v.num ~= nil and v.id > 0  and v.num > 0 then
            inv:initByItem(itemSlot, v.id, v.num, v.meta)
            itemSlot = itemSlot + 1
        end
    end
    self.chestIsFill[hash] = true
end

function GameManor:updateAllChestData(player)
    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_CHEST) then
        return
    end

    local hashTable = {}
    for hash, info in pairs(self.chest) do
        -- do not fill
        if self.chestIsFill[hash] == nil then
            hashTable[hash] = info
            -- has fill
        else
            hashTable[hash] = {}
        end
    end

    local copyChest = {}

    for hash, hashInfo in pairs(hashTable) do
        local x, y, z = GameConfig:decodeHash(hash)
        local aPos = self:rPosToAPos(VectorUtil.newVector3i(x, y, z))
        local inv = EngineWorld:getWorld():getInventory(BlockID.CHEST, aPos)
        if inv then
            local empty = next(hashInfo)
            -- has fill, need calculate again
            if empty == nil then
                local info = {}
                for i = 0, 35 do
                    local itemInfo = inv:getItemStackInfo(i)
                    if itemInfo and itemInfo.id > 0 and itemInfo.num > 0 then
                        local item = {}
                        item.id = tonumber(itemInfo.id)
                        item.num = tonumber(itemInfo.num)
                        item.meta = tonumber(itemInfo.meta)
                        table.insert(info, item)
                    end
                end
                copyChest[hash] = info
                -- do not fill
            else
                copyChest[hash] = hashInfo
            end
        end
    end

    for hash, info in pairs(copyChest) do
        self.chest[hash] = info
    end

end

-- chest end

-- furnace start

function GameManor:initFurnaceDataFromDB(data)
    if not data.__first then
        self.furnace = data.furnace or {}
    end

    if self.furnace then
        self.furnaceNum = 0
        for _, furnace in pairs(self.furnace) do
            self.furnaceNum = self.furnaceNum + 1
        end
    end

    HostApi.log("initFurnaceDataFromDB manorId: " .. tostring(self.userId) .. " furnaceNum: " .. self.furnaceNum)
end

function GameManor:checkFurnaceLimit()
    return self.furnaceNum >= GameConfig.furnaceLimit
end

function GameManor:recordFurnace(vec3, notAdd)
    if self:checkFurnaceLimit() then
        HostApi.log("recordFurnace " .. tostring(self.userId) .. "furnaceNum: " .. self.furnaceNum)
        -- return
    end
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.furnace[hash] == nil then
        self.furnace[hash] = {}
    end
    if notAdd then
        return
    end
    self.furnaceNum = self.furnaceNum + 1
end

function GameManor:removeFurnace(vec3)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.furnace[hash] ~= nil then
        self.furnace[hash] = nil
        self.furnaceNum = self.furnaceNum - 1
    end
end

function GameManor:fillFurnace(vec3, inv)
    if inv == nil then
        return
    end

    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_FURNACE) then
        return
    end

    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    HostApi.log("fillFurnace userId: " .. tostring(self.userId) .. " " .. hash)

    if self.furnace[hash] == nil then
        return
    end

    if self.furnaceFill[hash] ~= nil then
        return
    end

    inv:clearFurnace()
    for i, v in pairs(self.furnace[hash]) do
        if v.id ~= nil and v.num ~= nil and v.id > 0 and v.num > 0 and v.index >= 0 and v.index <= 2 then
            inv:initByItem(v.index, v.id, v.num, v.meta)
        end
    end
    self.furnaceFill[hash] = true
end

function GameManor:updateAllFurnaceData(player)
    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_FURNACE) then
        return
    end

    local hashTable = {}
    for hash, info in pairs(self.furnace) do
        -- do not fill
        if self.furnaceFill[hash] == nil then
            hashTable[hash] = info
            -- has fill
        else
            hashTable[hash] = {}
        end
    end
    local copyFurnace = {}

    for hash, hashInfo in pairs(hashTable) do
        local x, y, z = GameConfig:decodeHash(hash)
        local aPos = self:rPosToAPos(VectorUtil.newVector3i(x, y, z))
        local inv = EngineWorld:getWorld():getInventory(BlockID.FURNACE_IDLE, aPos)
        if inv then
            local empty = next(hashInfo)
            -- has fill, need calculate again
            if empty == nil then
                local info = {}
                for i = 0, 2 do
                    local itemInfo = inv:getItemStackInfo(i)
                    if itemInfo and itemInfo.id > 0 and itemInfo.num > 0 then
                        local item = {}
                        item.id = tonumber(itemInfo.id)
                        item.num = tonumber(itemInfo.num)
                        item.meta = tonumber(itemInfo.meta)
                        item.index = i
                        table.insert(info, item)
                    end
                end
                copyFurnace[hash] = info
                -- do not fill
            else
                copyFurnace[hash] = hashInfo
            end
        end
    end

    for h1, v1 in pairs(self.furnace) do
        for h2, v2 in pairs(copyFurnace) do
            if h1 == h2 then
                self.furnace[h1] = copyFurnace[h2]
            end
        end
    end
end

-- furnace end

-- manorinfo start

function GameManor:initManorInfoFromDB(data)
    if not data.__first then
        self.treeInfo = data.treeInfo or {}
        self.dirtInfo = data.dirtInfo or {}
        self.cropsInfo = data.cropsInfo or {}
    end
    HostApi.log("initManorInfoFromDB manorId: " .. tostring(self.userId))
    self:initAllTree()
    self:initAllDirt()
end

function GameManor:initAllTree()
    if self.treeInfo then
        for hash, meta in pairs(self.treeInfo) do
            local x, y, z = GameConfig:decodeHash(hash)
            local rPos = VectorUtil.newVector3i(x, y, z)
            self:initTreeTimeByPos(rPos, meta)
        end
    end
end

function GameManor:initTreeTimeByPos(vec3, bMeta)
    local master = PlayerManager:getPlayerByUserId(self.userId)
    if master == nil then
        return
    end

    local hash = vec3.x .. ":" .. vec3.y .. ":" .. vec3.z
    local dataString = StringUtil.split(hash, ":")
    if dataString then
        local x = tonumber(dataString[1])
        local y = tonumber(dataString[2])
        local z = tonumber(dataString[3])

        local vec3r = VectorUtil.newVector3i(x, y, z)
        local vec3a = self:rPosToAPos(vec3r)
        local vec3Hash = vec3a.x .. ":" .. vec3a.y .. ":" .. vec3a.z
        local blockMeta = bMeta
        local time = TreeSectionConfig:getTimeByBlockMeta(blockMeta)
        local nowId = EngineWorld:getBlockId(vec3a)
        if nowId == BlockID.SAPLING then
            if GameMatch.treeTime[tostring(master.userId)] == nil then
                GameMatch.treeTime[tostring(master.userId)] = {}
            end

            if GameMatch.treeTime[tostring(master.userId)][vec3Hash] == nil then
                GameMatch.treeTime[tostring(master.userId)][vec3Hash] = LuaTimer:scheduleTimer(function(userId)
                    local master = PlayerManager:getPlayerByUserId(userId)
                    if master then
                        local masterManor = GameMatch:getUserManorInfo(master.userId)
                        if masterManor then
                            TreeSectionConfig:theSchematicReplaceBlock(master.rakssid, blockMeta, vec3a)
                            masterManor:removeTreeInfo(vec3a)
                        end

                        if GameMatch.treeTime[tostring(master.userId)][vec3Hash] then
                            LuaTimer:cancel(GameMatch.treeTime[tostring(master.userId)][vec3Hash])
                            GameMatch.treeTime[tostring(master.userId)][vec3Hash] = nil
                        end

                    end
                end, time * 1000, 1, tostring(master.userId))
            end
        end
    end
end

function GameManor:initAllDirt()
    if self.dirtInfo then
        for hash, meta in pairs(self.dirtInfo) do
            local x, y, z = GameConfig:decodeHash(hash)
            local rPos = VectorUtil.newVector3i(x, y, z)
            self:initDirtTimeByPos(rPos, meta)
        end
    end
end

function GameManor:initDirtTimeByPos(vec3)
    local master = PlayerManager:getPlayerByUserId(self.userId)
    if master == nil then
        return
    end

    local hash = vec3.x .. ":" .. vec3.y .. ":" .. vec3.z
    local dataString = StringUtil.split(hash, ":")
    if dataString then
        local x = tonumber(dataString[1])
        local y = tonumber(dataString[2])
        local z = tonumber(dataString[3])

        local vec3r = VectorUtil.newVector3i(x, y, z)
        local vec3a = self:rPosToAPos(vec3r)
        local vec3Hash = vec3a.x .. ":" .. vec3a.y .. ":" .. vec3a.z
        local time = GameConfig.dirtTime

        if GameMatch.dirtTime[tostring(master.userId)] == nil then
            GameMatch.dirtTime[tostring(master.userId)] = {}
        end

        if GameMatch.dirtTime[tostring(master.userId)][vec3Hash] == nil then
            GameMatch.dirtTime[tostring(master.userId)][vec3Hash] = LuaTimer:scheduleTimer(function(userId)
                local master = PlayerManager:getPlayerByUserId(userId)
                if master then
                    local masterManor = GameMatch:getUserManorInfo(master.userId)
                    if masterManor then
                        EngineWorld:setBlock(vec3a, BlockID.GRASS, 0, 0, true)
                        masterManor:recordBlock(vec3a, BlockID.GRASS, 0)
                        masterManor:removeDirtInfo(vec3a)
                    end

                    if GameMatch.dirtTime[tostring(master.userId)][vec3Hash] then
                        LuaTimer:cancel(GameMatch.dirtTime[tostring(master.userId)][vec3Hash])
                        GameMatch.dirtTime[tostring(master.userId)][vec3Hash] = nil
                    end
                end
            end, time * 1000, 1, tostring(master.userId))
        end
    end
end

function GameManor:initCropsTime()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE)
            or not DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA)
            or not DbUtil.CanSavePlayerData(player, DbUtil.TAG_MANOR) then
        return
    end

    for hash , value in pairs(self.cropsInfo) do
        local dataString = StringUtil.split(hash, ":")
        local data = StringUtil.split(value, ":")
        if dataString then
            local x = tonumber(dataString[1])
            local y = tonumber(dataString[2])
            local z = tonumber(dataString[3])
            local vec3 = VectorUtil.newVector3i(x, y, z)
            vec3 = self:rPosToAPos(vec3)
            if data then
                local blockId = tonumber(data[1])
                local state = tonumber(data[2])
                local lastUpdateTime = tonumber(data[3])
                EngineWorld:getWorld():setCropsBlock(self.userId, vec3, blockId, state, 0, 0, lastUpdateTime, 1)
            end
        end
    end
end

function GameManor:insertTreeInfo(vec3, meta)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.treeInfo[hash] == nil then
        self.treeInfo[hash] = meta
    end
end

function GameManor:removeTreeInfo(vec3)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.treeInfo[hash] ~= nil then
        self.treeInfo[hash] = nil
    end
end

function GameManor:insertDirtInfo(vec3, blockId)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.dirtInfo[hash] == nil then
        self.dirtInfo[hash] = blockId
    end
end

function GameManor:removeDirtInfo(vec3)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.dirtInfo[hash] ~= nil then
        self.dirtInfo[hash] = nil
    end
end

function GameManor:insertCropsInfo(vec3, blockId, blockMeta, lastUpdateTime)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    local value = blockId .. ":" .. blockMeta.. ":" .. lastUpdateTime

    self.cropsInfo[hash] = value
end

function GameManor:removeCropsInfo(vec3)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.cropsInfo[hash] ~= nil then
        self.cropsInfo[hash] = nil
    end
end

function GameManor:clearCrops()
    for hash, crop in pairs(self.cropsInfo) do
        local x, y, z = GameConfig:decodeHash(hash)
        local aPos = self:rPosToAPos(VectorUtil.newVector3i(x, y, z))
        HostApi.deleteBlockCrops(aPos)
    end
end

function GameManor:clearCache()
    self.furnaceFill = {}
    self.isSignPostEdited = {}
end

function GameManor:checkPower(userId, pos, sign)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    if player:checkReady() == false then
        return false
    end

    local master = PlayerManager:getPlayerByUserId(self.userId)
    if master == nil then
        return false
    end

    if master:checkReady() == false then
        return false
    end

    if not player:checkAreaNormal() then
        return false
    end

    if pos and not self:isIn(pos) then
        HostApi.log("Weak net back home bug . sign: " .. tostring(sign) .. " manorId " .. tostring(self.userId) .. " playerUserId "
                .. tostring(userId) .. " pos " .. pos.x .. " " .. pos.y .. " " .. pos.z)
        return false
    end

    if GameMatch:isMaster(userId) then
        return true
    end

    -- check userId's power
    if tostring(player.buildGroupId) == tostring(self.userId) then
        for k,v in pairs(player.buildGroupData) do
            if tostring(v.masterId) == tostring(self.userId) and tostring(v.memberId) == tostring(userId) then

                return true
            end
        end
    end

    -- MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgCannotOperatorVistor())
    return false
end
-- manorinfo end

-- signPost start
function GameManor:initSignPostTextDataFromDB(data)
    if not data.__first then
        self.signPost = data.signPost or {}
    end
    if self.signPost then
        for key, signPost in pairs(self.signPost) do
            if not self:checkSignPostTextLimit() then
                self.signPostNum = self.signPostNum + 1
            end
        end
    end
    print("initSignPostTextDataFromDB self.signPostNum : "..self.signPostNum .. " userid " .. tostring(self.userId))
end

function GameManor:initSignPostText(vec3)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.isSignPostEdited == nil or self.signPost == nil then
        return
    end
    --从数据库初始化留言板
    for key, s_text in pairs(self.signPost) do
        if key == hash then
            LuaTimer:scheduleTimer(function()
                local blockId = EngineWorld:getBlockId(vec3)
                if blockId == BlockID.SIGN_POST or blockId == BlockID.SIGN_WALL then
                    EngineWorld:getWorld():resetSignText(vec3, 0, tostring(s_text), true)
                    HostApi.log(string.format("initSignPostText vec3 is x : %3d  y : %3d  z : %3d  s_text : %s", vec3.x, vec3.y, vec3.z, tostring(s_text)))
                    self.isSignPostEdited[hash] = true
                end
            end, 2000, 1)
        end
    end
end

function GameManor:recordSignPostText(vec3, text, isBreak)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.isSignPostEdited == nil or self.signPost == nil then
        return
    end
    --编辑已有文字的留言板
    if self.signPost[hash] ~= nil then
        if isBreak or text == "" then
            self.signPost[hash] = nil
            self.signPostNum = self.signPostNum - 1
            if self.signPostNum < 0 then
                self.signPostNum = 0
            end
            EngineWorld:getWorld():resetSignText(vec3, 0, "", true)
            self.isSignPostEdited[hash] = ""
            self.isSignPostEdited = TableUtil.removeElementByFun(self.isSignPostEdited, function (key, value)
                return value == ""
            end)
        else
            self.signPost[hash] = text
            EngineWorld:getWorld():resetSignText(vec3, 0, text, true)
            self.isSignPostEdited[hash] = true
        end
    else
        --编辑没有文字的留言板
        if text ~= "" then
            self.signPost[hash] = text
            self.signPostNum = self.signPostNum + 1
            HostApi.log("recordSignPostText self.signPostNum : " .. self.signPostNum)
            EngineWorld:getWorld():resetSignText(vec3, 0, text, true)
            self.isSignPostEdited[hash] = true
        end
    end
end

function GameManor:checkSignPostTextLimit()
    return self.signPostNum >= GameConfig.signPostNum
end

function GameManor:checkSignPostIsNew(vec3)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.signPost and self.signPost[hash] == nil then
        return nil
    else
        return tostring(self.signPost[hash])
    end
end

function GameManor:updateSignPostText()
    -- 留言板方块加载时，文字数据没有加载完成才会调用此更新函数
    if not self.is_need_update_sign then
        return
    end
    if not DBManager:isLoadDataFinished(self.userId, DbUtil.TAG_SIGN) then
        return
    end
    if self.isSignPostEdited == nil or self.signPost == nil then
        return
    end
    for key, s_text in pairs(self.signPost) do
        if self.isSignPostEdited[key] == nil then
            local x, y, z = GameConfig:decodeHash(key)
            local vec3 = self:rPosToAPos(VectorUtil.newVector3i(x, y, z))
            local blockId = EngineWorld:getBlockId(vec3)
            if blockId == BlockID.SIGN_POST or blockId == BlockID.SIGN_WALL then
                EngineWorld:getWorld():resetSignText(vec3, 0, tostring(s_text), true)
                self.isSignPostEdited[key] = true
                HostApi.log(string.format("updateSignPostText initSignPostText vec3 is x : %3d  y : %3d  z : %3d  s_text : %s", vec3.x, vec3.y, vec3.z, tostring(s_text)))
            end
        end
    end
end
-- signPost end

-- actor start
function GameManor:initActorFromDB(data)
    if not data.__first then
        self.ActorCache = data.ActorCache or {}
        for hash, itemID in pairs(self.ActorCache) do
            if self.ActorCache[hash] and not self:checkCanPlaceActorLimit() then
                local isCan, actor = GameConfig:isCanPlaceItem(itemID)
                if isCan then
                    local x, y, z = GameConfig:decodeHash(hash)
                    local vec3 = self:rPosToAPos(VectorUtil.newVector3i(x, y, z))
                    self:addActorByPos(vec3, itemID, actor)
                else
                    self.ActorCache[hash] = ""
                end
            end
        end
        self.ActorCache = TableUtil.removeElementByFun(self.ActorCache, function (key, value)
            return value == ""
        end)
    end
end

function GameManor:recordActor(vec3, itemID, actor)
    if self:checkCanPlaceActorLimit() or self:checkActorExistByPos(vec3) then
        return false
    end
    if EngineWorld:getBlockId(vec3) == BlockID.CHEST then
        return false
    end
    self:addActorByPos(vec3, itemID, actor)
    return true
end

function GameManor:removeActorById(entityId)
    local hash = self:checkManorActorById(entityId)
    if hash then
        EngineWorld:removeEntity(entityId)
        local itemID = self.ActorCache[hash]
        self.ActorCache[hash] = ""
        self.ActorCache = TableUtil.removeElementByFun(self.ActorCache, function (key, value)
            return value == ""
        end)
        self.ActorEntity[hash] = ""
        self.ActorEntity = TableUtil.removeElementByFun(self.ActorEntity, function (key, value)
            return value == ""
        end)
        self.actorNum = self.actorNum - 1
        if self.actorNum < 0 then
            self.actorNum = 0
        end
        local x, y, z = GameConfig:decodeHash(hash)
        HostApi.log(string.format("removeActorById aPos is x : %3d  y : %3d  z : %3d  itemID : %3s", x, y, z, itemID))
        HostApi.log(string.format("removeActorById actorNum : %3d  userId : %3s", self.actorNum, tostring(self.userId)))
        local rPos = VectorUtil.newVector3i(x, y, z)
        local aPos = self:rPosToAPos(rPos)
        if EngineWorld:getBlockId(aPos) ~= BlockID.CHEST then
            EngineWorld:setBlock(aPos, 0, 0, 0)
        end
        return tonumber(itemID)
    end
    return 0
end

function GameManor:checkManorActorById(entityId)
    for hash,id in pairs(self.ActorEntity) do
        if tostring(entityId) == id then
            if self.ActorCache[hash] then
                return hash
            end
        end
    end
    return nil
end

function GameManor:checkCanPlaceActorLimit()
    -- HostApi.log(string.format("checkCanPlaceActorLimit actorNum : %3d  userId : %3s", self.actorNum, tostring(self.userId)))
    return self.actorNum >= GameConfig.canPlaceActorLimit
end

function GameManor:checkActorExistByPos(vec3)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.ActorCache[hash] or self.ActorEntity[hash] then
        HostApi.log(string.format("checkActorExistByPos aPos is x : %3d  y : %3d  z : %3d", vec3.x, vec3.y, vec3.z))
        return true
    end
    return false
end

function GameManor:addActorByPos(vec3, itemID, actor)
    local actorPos = VectorUtil.newVector3(vec3.x + 0.5, vec3.y, vec3.z + 0.5)
    local entityId = EngineWorld:addActorNpc(actorPos, 0, actor, function(entity)
        entity:setCanObstruct(false)
        entity:setCanCollided(true)
        entity:setSuspended(true, actorPos)
    end)
    local rPos = self:aPosToRPos(vec3)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    self.ActorCache[hash] = tostring(itemID)
    self.ActorEntity[hash] = tostring(entityId)
    self.actorNum = self.actorNum + 1
    HostApi.log(string.format("addActorByPos aPos is x : %3d  y : %3d  z : %3d  itemID : %3s", vec3.x, vec3.y, vec3.z, itemID))
    HostApi.log(string.format("addActorByPos actorNum  : %3d  userId : %3s", self.actorNum, tostring(self.userId)))
    local lightBlock = GameConfig:getBlockIdWithPlaceItemId(itemID)
    if lightBlock and lightBlock ~= -1 then
        HostApi.log("lightBlock userId: " .. tostring(self.userId) .. " " ..  lightBlock)
        EngineWorld:setBlock(vec3, lightBlock, 0, 3)
    end
end

function GameManor:clearActors()
    for hash,entityId in pairs(self.ActorEntity) do
        EngineWorld:removeEntity(tonumber(entityId))
        local x, y, z = GameConfig:decodeHash(hash)
        HostApi.log(string.format("clearActors userId : %3s aPos is x : %3d  y : %3d  z : %3d  entityId : %3s", tostring(self.userId), x, y, z, entityId))
        local rPos = VectorUtil.newVector3i(x, y, z)
        local aPos = self:rPosToAPos(rPos)
        EngineWorld:setBlock(aPos, 0, 0, 0)
    end
end

function GameManor:canPlaceChest(blockPos)
    local rPos = self:aPosToRPos(blockPos)
    local hash = rPos.x .. ":" .. rPos.y .. ":" .. rPos.z
    if self.ActorCache[hash] or self.ActorEntity[hash]
        or self.treeInfo[hash] or self.dirtInfo[hash] or self.cropsInfo[hash]
    then
        return false
    end
    return true
end
-- actor end