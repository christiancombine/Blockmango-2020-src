--region *.lua
require "Match"
require "config.YardConfig"
require "config.BlockConfig"

BlockListener = {}

function BlockListener:init()
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    BlockCropsPlaceEvent.registerCallBack(self.onBlockCropsPlace)
    BlockCropsUpdateEvent.registerCallBack(self.onBlockCropsUpdate)
    BlockCropsPickEvent.registerCallBack(self.onBlockCropsPick)
    BlockCropsStealEvent.registerCallBack(self.onBlockCropsSteal)
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if blockId >= 1256 then  -- 1256 is first index of blockCrops
        if YardConfig:isInYard(player.manorIndex, player.yardLevel, vec3) then
            return true
        end

        if player.vegetableNum <= 0 then
            HostApi.sendManorOperationTip(player.rakssid, "gui_manor_steal_count_message")
            return false
        end

        return true
    end

    if GameMatch.userManorIndex[tostring(player.userId)] == nil then
        return false
    end

    if YardConfig:isInYard(player.manorIndex, player.yardLevel, vec3) then
        for i, v in pairs(BlockConfig.block) do
            if v.id == blockId and (blockId == 50 or blockId == 65)  then  -- 火把和 梯子 特殊处理
                local offsetVec3 = YardConfig:getOffsetByVec3i(player.manorIndex, vec3)
                local sign = VectorUtil.hashVector3(offsetVec3)
                if GameMatch.temporaryStore[tostring(player.userId)] ~= nil then
                    if GameMatch.temporaryStore[tostring(player.userId)][sign] ~= nil then
                        GameMatch.temporaryStore[tostring(player.userId)][sign] = nil
                        player:addItem(blockId, 1, 0)
                        player.modify_block = true
                    end
                end

                return true
            end

            if v.id == blockId and v.meta == blockMeta then
                local offsetVec3 = YardConfig:getOffsetByVec3i(player.manorIndex, vec3)
                local sign = VectorUtil.hashVector3(offsetVec3)
                if GameMatch.temporaryStore[tostring(player.userId)] ~= nil then
                    if GameMatch.temporaryStore[tostring(player.userId)][sign] ~= nil then
                        GameMatch.temporaryStore[tostring(player.userId)][sign] = nil
                        player:addItem(blockId, 1, blockMeta)
                    else
                        local temporaryStore = {}
                        local offsetVec3 = YardConfig:getOffsetByVec3i(player.manorIndex, vec3)
                        temporaryStore.id = 0
                        temporaryStore.meta = 0
                        temporaryStore.offsetVec3 = offsetVec3
                        GameMatch:initTemporaryStore(player.userId, temporaryStore)
                    end
                else
                    local temporaryStore = {}
                    local offsetVec3 = YardConfig:getOffsetByVec3i(player.manorIndex, vec3)
                    temporaryStore.id = 0
                    temporaryStore.meta = 0
                    temporaryStore.offsetVec3 = offsetVec3
                    GameMatch:initTemporaryStore(player.userId, temporaryStore)
                end
                player.modify_block = true
                return true
            end
        end
    end
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if GameMatch.userManorIndex[tostring(player.userId)] == nil then
        return false
    end

    if itemId >= 900 then
        toPlacePos.y = toPlacePos.y - 1 -- 种子放置的坐标 -1 后与底下泥土的坐标相等
        if YardConfig:isInGarden(player.manorIndex, player.yardLevel, toPlacePos) then
            local yard = YardConfig:getYardByLevel(player.yardLevel, player.manorIndex)
            if yard == nil then
                HostApi.log("==== LuaLog: BlockListener.onBlockPlace : yard is nil, place BrockCrops failure")
                return false
            end
            for i, seed in pairs(yard.seed) do
                if itemId == seed.itemId then
                    return true
                end
            end
            HostApi.sendManorOperationTip(rakssid, "gui_manor_lack_of_level_message")
            return false
        else
            return false
        end
    end


    if YardConfig:isInYard(player.manorIndex, player.yardLevel, toPlacePos) then
        for i, v in pairs(BlockConfig.block) do
            if v.id == itemId and ((v.meta == meta) or (itemId == 50 or itemId == 65))  then
                local temporaryStore = {}
                local offsetVec3 = YardConfig:getOffsetByVec3i(player.manorIndex, toPlacePos)
                temporaryStore.id = tonumber(itemId)
                temporaryStore.meta = tonumber(meta)
                temporaryStore.offsetVec3 = offsetVec3
                GameMatch:initTemporaryStore(player.userId, temporaryStore)
                player.modify_block = true
                return true
            end
        end
    else
        return false
    end

    return false
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

function BlockListener.onBlockCropsPlace(userId, blockId, vec3, residueHarvestNum)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local offsetVec3 = YardConfig:getOffsetOfSeed(player.manorIndex, player.yardLevel, vec3)
    local curState = 1
    local curStateTime = 0
    local fruitNum = 0
    local lastUpdateTime = os.time()
    local pluckingState = 1

    local blockCrops = {}
    blockCrops.blockId = blockId
    blockCrops.userId  = userId
    blockCrops.vec3 = vec3
    blockCrops.offsetVec3 = offsetVec3
    blockCrops.curState = curState
    blockCrops.curStateTime = curStateTime
    blockCrops.lastUpdateTime = lastUpdateTime
    blockCrops.fruitNum = fruitNum
    blockCrops.manorIndex = player.manorIndex
    blockCrops.level = player.yardLevel
    blockCrops.manorId = player.manorId
    blockCrops.pluckingState = pluckingState
    GameMatch:initBlockCrops(userId, blockCrops)

    HostApi.ploughCrop(player.manorId, offsetVec3.z, offsetVec3.x, tostring(blockId), fruitNum, curState, pluckingState, curStateTime, lastUpdateTime)
    -- HostApi.log("=== LugLog: BlockListener.onBlockCropsPlace successful : row = " .. offsetVec3.z .. ", col = " .. offsetVec3.x)
end

function BlockListener.onBlockCropsUpdate(userId, blockId, curState, curStateTime, fruitNum, vec3, residueHarvestNum)
    if GameMatch.blockCrops[tostring(userId)] == nil then
        return
    end

    local sign = VectorUtil.hashVector3(vec3)
    local block = GameMatch.blockCrops[tostring(userId)][sign]
    if block ~= nil then
        local manorIndex = block.manorIndex
        local level = block.level
        local manorId = block.manorId
        local pluckingState = block.pluckingState

        if curState == 5 then
            pluckingState = 0
        end

        local offsetVec3 = YardConfig:getOffsetOfSeed(manorIndex, level, vec3)
        local lastUpdateTime = os.time()

        local blockCrops = {}
        blockCrops.blockId = blockId
        blockCrops.userId  = userId
        blockCrops.vec3 = vec3
        blockCrops.offsetVec3 = offsetVec3
        blockCrops.curState = curState
        blockCrops.curStateTime = curStateTime
        blockCrops.lastUpdateTime = lastUpdateTime
        blockCrops.fruitNum = fruitNum
        blockCrops.manorIndex = manorIndex
        blockCrops.level = level
        blockCrops.manorId = manorId
        blockCrops.pluckingState = pluckingState
        GameMatch:initBlockCrops(userId, blockCrops)
        -- HostApi.log("==== LuaLog: BlockListener.onBlockCropsUpdate : manorId = " .. tostring(manorId) .. " curState = " .. curState .. ", pluckingState = " .. pluckingState)
        HostApi.ploughCrop(manorId, offsetVec3.z, offsetVec3.x, tostring(blockId), fruitNum, curState, pluckingState, curStateTime, lastUpdateTime)
    end
end

function BlockListener.onBlockCropsPick(userId, blockId, productionId, quantity, vec3, stage, residueHarvestNum)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    local sign = VectorUtil.hashVector3(vec3)
    local blockCrops = GameMatch.blockCrops[tostring(userId)][sign]
    if blockCrops ~= nil then
        local pluckingTime = os.time()
        local offsetVec3 = YardConfig:getOffsetOfSeed(player.manorIndex, player.yardLevel, vec3)
        player:addItem(productionId, quantity, 0)
        HostApi.ploughPlucking(player.manorId, userId, offsetVec3.z, offsetVec3.x, quantity, productionId, pluckingTime)
        GameMatch.blockCrops[tostring(userId)][sign] = nil
        return true
    end

    return false
end

function BlockListener.onBlockCropsSteal(userId, blockId, productionId, quantity, vec3, residueHarvestNum)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local sign = VectorUtil.hashVector3(vec3)
    local blockCrops = GameMatch.blockCrops
    for i, v in pairs(blockCrops) do
        if blockCrops[tostring(i)][sign] ~= nil then
            local manorId = blockCrops[tostring(i)][sign].manorId
            local manorIndex = blockCrops[tostring(i)][sign].manorIndex
            local level =  blockCrops[tostring(i)][sign].level
            local pluckingTime = os.time()
            local offsetVec3 = YardConfig:getOffsetOfSeed(manorIndex, level, vec3)
            player:addItem(productionId, quantity, 0)
            player.vegetableNum = player.vegetableNum - 1
            HostApi.ploughPlucking(manorId, userId, offsetVec3.z, offsetVec3.x, quantity, productionId, pluckingTime)
            break
        end
    end

end

return BlockListener
--endregion
