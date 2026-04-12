BlockManager = {}
BlockManager.BlockLifeCache = {}
BlockManager.PlayerBlockCache = {}
BlockManager.MapBlockCache = {}

function BlockManager:clearAllBlock()
    for hash, cache in pairs(BlockManager.PlayerBlockCache) do
        EngineWorld:setBlockToAir(cache.position)
    end
    BlockManager.BlockLifeCache = {}
    BlockManager.PlayerBlockCache = {}
    BlockManager.MapBlockCache = {}
end

function BlockManager:clearPlayerBlock(player)
    local removes = {}
    for hash, cache in pairs(BlockManager.PlayerBlockCache) do
        if cache.userId == player.userId then
            EngineWorld:setBlockToAir(cache.position)
            table.insert(removes, hash)
        end
    end
    for hash, cache in pairs(BlockManager.MapBlockCache) do
        if cache.userId == player.userId then
            EngineWorld:setBlock(cache.position, cache.blockId)
            table.insert(removes, hash)
        end
    end
    for _, remove in pairs(removes) do
        BlockManager.BlockLifeCache[remove] = nil
        BlockManager.PlayerBlockCache[remove] = nil
        BlockManager.MapBlockCache[remove] = nil
    end
end

function BlockManager:removeWaterBlocks(position)
    local pos = VectorUtil.toBlockVector3(position.x, position.y, position.z)
    pos.y = pos.y +1
    for i = pos.x - 8, pos.x + 8 do
        for j = pos.z - 8, pos.z + 8 do
            for k = pos.y + 8, pos.y - 8 , -1 do
                local v3i = VectorUtil.toBlockVector3(i, k, j)
                local id = EngineWorld:getBlockId(v3i)
                if id == BlockID.WATER_MOVING or id == BlockID.WATER_STILL then
                    EngineWorld:setBlock(v3i, 0, 0, 3, true)
                end
            end
        end
    end
end

function BlockManager:setSlimeBlock(pos)
    for i = pos.x - 2, pos.x + 2 do
        for k = pos.z - 2, pos.z + 2 do
            local v3i = VectorUtil.toBlockVector3(i, pos.y, k)
            EngineWorld:setBlock(v3i, BlockID.SLIME, 0, 3, true)
            LuaTimer:schedule(function (vec3i)
                EngineWorld:setBlockToAir(VectorUtil.newVector3i(vec3i.x, vec3i.y, vec3i.z))
            end, 5000, nil, v3i)
        end
    end
end

return BlockManager