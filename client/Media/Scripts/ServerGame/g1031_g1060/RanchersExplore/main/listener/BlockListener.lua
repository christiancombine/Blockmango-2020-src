--region *.lua

require "Match"

BlockListener = {}
BlockListener.pressure_info = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockSwitchEvent.registerCallBack(self.onBlockSwitch)
    BlockPressurePlateWeightedEvent.registerCallBack(self.onBlockPressurePlateWeighted)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    BlockTNTExplodeBreakBlockEvent.registerCallBack(self.onBlockTNTExplodeBreakBlock)
    BlockOreDroppedEvent.registerCallBack(self.onBlockOreDropped)
    ItemPickaxeCanHarvestEvent.registerCallBack(self.onItemPickaxeCanHarvest)
    BlockStationaryNotStationaryEvent.registerCallBack(self.onBlockStationaryNotStationary)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    return true
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3, itemID, currentStackIndex)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    -- HostApi.log("onBlockBreakWithMeta:" .. GameMatch.curStatus .. " " .. itemID)

    if GameMatch.curStatus ~= GameMatch.Status.Start then
        return false
    end

    if ToolConfig:isTool(itemID) == false then
        return false
    end

    -- HostApi.log("onBlockBreakWithMeta: " .. blockId)

    if not GameConfig:CanBreak(blockId, blockMeta) then
        return false
    end

    if not RoomConfig:isArea(vec3) then
        return false
    end

    if RoomConfig:isFindBlockId(blockId) then
        local roomId = RoomConfig:getRoomId(vec3)
        HostApi.log("onBlockBreakWithMeta " .. roomId .. " " .. blockId)
        if RoomConfig.room[roomId] ~= nil then
            local findblockId = RoomConfig:getRoomFindBlockId(roomId)
            if findblockId == blockId then
                GameMatch:onFindRoom(player, roomId, tonumber(RoomConfig.room[roomId].type))
            end
        end
    end

    local inv = player:getInventory()
    if inv then
        for i = 1, 9 do
            local info = inv:getItemStackInfo(i - 1)
            if info and info.id ~= 0 and info.id == itemID and currentStackIndex == (i - 1) then

                local des = GameConfig:getCurrentItemDesById(info.id)
                local durable = ToolConfig:getDurableByToolId(info.id) - info.meta

                if ToolConfig:isTool(info.id) then
                    -- Has not break block(durable has not changed)
                    HostApi.showRanchExCurrentItemInfo(player.rakssid, true, des, 0, durable - 1)
                end

                break
            end
        end
    end

    return true
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if not GameConfig:CanPlaceBreak(itemId, meta) then
        -- HostApi.log("GameConfig:CanPlaceBreak")
        return false
    end

    if not RoomConfig:isArea(toPlacePos) then
        -- HostApi.log("RoomConfig:isArea")
        return false
    end

    return true
end

function BlockListener.onBlockSwitch(entityId, downOrUp, blockId, vec3i)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return false
    end

    -- HostApi.log("onBlockSwitch " .. blockId)

    if player.entityPlayerMP ~= nil then
        if downOrUp and blockId == BlockID.PRESSURE_PLATE_PLANKS then
            local roomId = RoomConfig:getRoomId(vec3i)
            -- HostApi.log("onBlockSwitch222 " .. roomId)
            if RoomConfig.room[roomId] ~= nil and RoomConfig.room[roomId].jumpblock ~= nil then
                player.entityPlayerMP:specialJump(RoomConfig.room[roomId].jumpblock[1], RoomConfig.room[roomId].jumpblock[2], RoomConfig.room[roomId].jumpblock[3])
                return true
            end
        end
    end

    return true
end

function BlockListener.onBlockPressurePlateWeighted(entityId, power, blockId, vec3i)--, entityId)
    if blockId == 217 then
        local roomId = RoomConfig:getRoomId(vec3i)
        -- HostApi.log("onBlockPressurePlateWeighted " .. roomId)
        if RoomConfig.room[roomId] ~= nil then
            local config = RoomConfig.room[roomId]
            local hash = VectorUtil.hashVector3(vec3i)

            if BlockListener.pressure_info[hash] == nil then
                BlockListener.pressure_info[hash] = {}
                BlockListener.pressure_info[hash].power = power
            else
                BlockListener.pressure_info[hash].power = power
            end

            local canTrigger = false
            local press_count = 0
            if config.pressure then
                for i = 1, #config.pressure do
                    for pos, info in pairs(BlockListener.pressure_info) do
                        local roomPosVec3i = VectorUtil.newVector3i(config.pressure[i].pos[1], config.pressure[i].pos[2], config.pressure[i].pos[3])
                    
                        if pos == VectorUtil.hashVector3(roomPosVec3i) and info.power > 0 then
                            press_count = press_count + 1
                        end
                    end
                end

                if press_count >= #config.pressure then
                    canTrigger = true
                end
            end

            if canTrigger then
                -- HostApi.log("onBlockPressurePlateWeighted!!!!!!")
                if config.chest then
                    for k, v in pairs(config.chest) do
                        local chest_pos_x = tonumber(v.pos[1])
                        local chest_pos_y = tonumber(v.pos[2])
                        local chest_pos_z = tonumber(v.pos[3])

                        for X = chest_pos_x - 1, chest_pos_x + 1 do
                            for Y = chest_pos_y, chest_pos_y + 1 do
                                for Z = chest_pos_z - 1, chest_pos_z + 1 do
                                    if not ((X == chest_pos_x) and (Y == chest_pos_y) and (Z == chest_pos_z)) then
                                        EngineWorld:setBlockToAir(VectorUtil.newVector3i(X, Y, Z))
                                    end
                                end
                            end
                        end
                    end
                end
            else
                local player = PlayerManager:getPlayerByEntityId(entityId)
                if player ~= nil then
                    MsgSender.sendBottomTipsToTarget(player.rakssid, 3, Messages:PressureTip(#config.pressure - press_count))
                end
            end
        end
    end
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    if GameMatch.curStatus ~= GameMatch.Status.Start then
        return false
    end

    attr.isBreakBlock = true
    attr.isCanHurt = false
    attr.custom_explosionSize = GameConfig.TNTrange
    return false
end

function BlockListener.onBlockTNTExplodeBreakBlock(blockPos, blockId)

    if GameMatch.curStatus ~= GameMatch.Status.Start then
        return false
    end

    if not GameConfig:CanBreak(blockId, -1) then
        return false
    end

    if not RoomConfig:isArea(blockPos) then
        return false
    end

    return true
end

function BlockListener.onBlockOreDropped(blockId)
    if GameMatch.curStatus ~= GameMatch.Status.Start then
        return true
    end

    if not GameConfig:CanBreak(blockId, -1) then
        return true
    end

    return false
end


function BlockListener.onItemPickaxeCanHarvest(blockId)
    if GameMatch.curStatus ~= GameMatch.Status.Start then
        return true
    end

    if not GameConfig:CanBreak(blockId, -1) then
        return true
    end

    return false
end

function BlockListener.onBlockStationaryNotStationary(blockId)
    return false
end

return BlockListener
--endregion
