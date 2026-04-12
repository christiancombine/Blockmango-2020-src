
require "Match"

BlockListener = {}

function BlockListener:init()
    BlockStartBreakEvent.registerCallBack(self.onBlockStartBreak)
    BlockAbortBreakEvent.registerCallBack(self.onBlockAbortBreak)
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
end

function BlockListener.onBlockStartBreak(rakssid, blockId, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    for i, v in pairs(BlockConfig.area) do
        local meta = EngineWorld:getWorld():getBlockMeta(vec3)
        for i, v in pairs(BlockConfig.block) do
            if v.id == blockId and v.meta == meta then
                local hashKey = VectorUtil.hashVector3(vec3)
                if BlockConfig.blockHp[hashKey] == nil then
                    BlockConfig.blockHp[hashKey] = v.hp
                end
            end
        end
    end
end

function BlockListener.onBlockAbortBreak(rakssid, blockId, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    for i, v in pairs(BlockConfig.area) do
        local meta = EngineWorld:getWorld():getBlockMeta(vec3)
        for i, v in pairs(BlockConfig.block) do
            if v.id == blockId and v.meta == meta then
                local hashKey = VectorUtil.hashVector3(vec3)
                if BlockConfig.blockHp[hashKey] == nil then
                    BlockConfig.blockHp[hashKey] = v.hp
                end
            end
        end
    end
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    for i, v in pairs(BlockConfig.area) do
        local meta = EngineWorld:getWorld():getBlockMeta(vec3)
        for i, v in pairs(BlockConfig.block) do
            if v.id == blockId and v.meta == meta then
                local hashKey = VectorUtil.hashVector3(vec3)
                if BlockConfig.blockHp[hashKey] == nil then
                    BlockConfig.blockHp[hashKey] = v.hp
                end

                local damage = ToolConfig:getDamageByToolId(player:getHeldItemId())
                if BlockConfig.blockHp[hashKey] - damage > 0 then
                    BlockConfig.blockHp[hashKey] = BlockConfig.blockHp[hashKey] - damage
                    return false
                else
                    BlockConfig.blockHp[hashKey] = nil
                    if blockId ~= 211 then  -- 211是宝箱
                        local itemId = BlockConfig:getItemByBlockId(blockId)
                        if itemId ~= nil then
                            player:addItem(itemId, 1, 0)
                        end
                    else
                        player:addMoney(BlockConfig:getMineMoneyById(blockId))
                    end

                    return true
                end

                return true
            end
        end
    end


    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    return true
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

return BlockListener
--endregion
