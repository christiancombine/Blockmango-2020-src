BlockConfig = {}
BlockConfig.block = {}
BlockConfig.blockInfo = {}
BlockConfig.blockScore = {}
BlockConfig.simplifyBlock = {}

function BlockConfig:initBlock(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.blockId = tonumber(v.blockId)
        data.meta = tonumber(v.meta)
        data.score = tonumber(v.score)
        data.level = tonumber(v.level)
        data.price = tonumber(v.price)
        data.cube = tonumber(v.cube)
        data.type = tonumber(v.type)
        data.weight = tonumber(v.weight)
        data.feature = v.feature
        data.isNew = tonumber(v.isNew)
        data.moneyType = tonumber(v.moneyType)
        self.block[data.id] = data
        self.blockInfo[v.blockId .. ":" .. v.meta] = data
        self.blockScore[data.blockId] = data
    end
end

function BlockConfig:getScoreById(id)
    if not self.blockScore[id] then
        return 0
    end

    return self.blockScore[id].score
end

function BlockConfig:getScoreByOnlyId(id)
    id = tonumber(id)
    if not self.block[id] then
        return 0
    end

    return self.block[id].score
end

function BlockConfig:getBlockInfo(id, meta)
    if not self.blockInfo[id .. ":" .. meta] then
        print("BlockConfig:getBlockInfo() block.csv can not find Info by id =", id,"and meta =", meta)
        return nil
    end

    return self.blockInfo[id .. ":" .. meta]
end

function BlockConfig:getBlockById(id)
    id = tonumber(id)
    if not self.block[id] then
        print("BlockConfig:getBlockById()  block.csv can not find Info by id =", id)
        return nil
    end

    return self.block[id]
end

function BlockConfig:isSpecialBlock(id)
    -- 这类方块需要依靠在别的方块之上
    id = tonumber(id)
    return id == 37 or id == 38 or id == 50 or id == 64
            or id == 65 or id == 71 or id == 76 or id == 106
end

function BlockConfig:initSimplifyBlock(config)
    for _, v in pairs(config) do
        self.simplifyBlock[v.blockId .. ":" .. v.meta] = tonumber(v.masterMeta)
    end
end

function BlockConfig:getSimplifyBlockMasterMeta(blockId, meta)
    if not self.simplifyBlock[blockId .. ":" .. meta] then
        return meta
    end

    return self.simplifyBlock[blockId .. ":" .. meta]
end

function BlockConfig:isStairsBlock(id)
    return id == 53 or id == 67 or id == 108
            or id == 109 or id == 114 or id == 128
            or id == 134 or id == 135 or id == 136
            or id == 156 or id == 163 or id == 164 or id == 180
end

function BlockConfig:isLadderBlock(id)
    return id == 65
end

function BlockConfig:isTorchWoodBlock(id)
    return id == 50 or id == 76
end

function BlockConfig:isWoodBlock(id)
    return id == 17 or id == 162
end

function BlockConfig:isDoorBlock(id)
    return id == 64 or id == 71
end

function BlockConfig:isDoorItemBlock(id)
    return id == 324 or id == 330
end

function BlockConfig:isSingleSlabBlock(id)
    return id == 44 or id == 126 or id == 182
end

function BlockConfig:isDoubleSlabBlock(id)
    return id == 43 or id == 125 or id == 181
end

function BlockConfig:isFenceGateBlock(id)
    return id == 107 or id == 184
end

function BlockConfig:isTrapDoorBlock(id)
    return id == 96
end

function BlockConfig:getSingleSlabIdByDoubleSlabId(id)
    if id == 43 then
        return 44
    end

    if id == 125 then
        return 126
    end

    if id == 181 then
        return 182
    end

    return id
end

function BlockConfig:getMetaByOrientationTo180(id, meta, orientation)
    if orientation == 90 then
        orientation = 270
    elseif orientation == 270 then
        orientation = 90
    end

    return BlockConfig:getMetaByOrientation(id, meta, orientation)
end

function BlockConfig:getMetaByOrientation(id, meta, orientation)
    if orientation == 180 then
        return meta
    end

    if BlockConfig:isStairsBlock(id) then
        return BlockConfig:stairsBlockRotate(meta, orientation)
    end

    if BlockConfig:isLadderBlock(id) then
        return BlockConfig:ladderBlockRotate(meta, orientation)
    end

    if BlockConfig:isTorchWoodBlock(id) then
        return BlockConfig:torchWoodBlockRotate(meta, orientation)
    end

    if BlockConfig:isWoodBlock(id) then
        return BlockConfig:woodBlockRotate(meta, orientation)
    end

    if BlockConfig:isDoorBlock(id) then
        return BlockConfig:doorBlockRotate(meta, orientation)
    end

    if BlockConfig:isFenceGateBlock(id) then
        return BlockConfig:fenceGateRotate(meta, orientation)
    end

    if BlockConfig:isTrapDoorBlock(id) then
        return BlockConfig:trapDoorBlockRotate(meta, orientation)
    end

    return meta
end

function BlockConfig:stairsBlockRotate(meta, orientation)
    if meta == 0 then
        if orientation == 0 then return 1 end
        if orientation == 90 then return 3 end
        if orientation == 270 then return 2 end
    end

    if meta == 1 then
        if orientation == 0 then return 0 end
        if orientation == 90 then return 2 end
        if orientation == 270 then return 3 end
    end

    if meta == 2 then
        if orientation == 0 then return 3 end
        if orientation == 90 then return 0 end
        if orientation == 270 then return 1 end
    end

    if meta == 3 then
        if orientation == 0 then return 2 end
        if orientation == 90 then return 1 end
        if orientation == 270 then return 0 end
    end

    if meta == 4 then
        if orientation == 0 then return 5 end
        if orientation == 90 then return 7 end
        if orientation == 270 then return 6 end
    end

    if meta == 5 then
        if orientation == 0 then return 4 end
        if orientation == 90 then return 6 end
        if orientation == 270 then return 7 end
    end

    if meta == 6 then
        if orientation == 0 then return 7 end
        if orientation == 90 then return 4 end
        if orientation == 270 then return 5 end
    end

    if meta == 7 then
        if orientation == 0 then return 6 end
        if orientation == 90 then return 5 end
        if orientation == 270 then return 4 end
    end

    return meta
end

function BlockConfig:ladderBlockRotate(meta, orientation)
    if meta == 0 then
        if orientation == 0 then return 3 end
        if orientation == 90 then return 4 end
        if orientation == 270 then return 5 end
    end

    if meta == 3 then
        if orientation == 0 then return 0 end
        if orientation == 90 then return 5 end
        if orientation == 270 then return 4 end
    end

    if meta == 4 then
        if orientation == 0 then return 5 end
        if orientation == 90 then return 3 end
        if orientation == 270 then return 0 end
    end

    if meta == 5 then
        if orientation == 0 then return 4 end
        if orientation == 90 then return 0 end
        if orientation == 270 then return 3 end
    end

    return meta
end

function BlockConfig:torchWoodBlockRotate(meta, orientation)
    if meta == 1 then
        if orientation == 0 then return 2 end
        if orientation == 90 then return 4 end
        if orientation == 270 then return 3 end
    end

    if meta == 2 then
        if orientation == 0 then return 1 end
        if orientation == 90 then return 3 end
        if orientation == 270 then return 4 end
    end

    if meta == 3 then
        if orientation == 0 then return 4 end
        if orientation == 90 then return 1 end
        if orientation == 270 then return 2 end
    end

    if meta == 4 then
        if orientation == 0 then return 3 end
        if orientation == 90 then return 2 end
        if orientation == 270 then return 1 end
    end

    return meta
end

function BlockConfig:woodBlockRotate(meta, orientation)
    if meta == 4 then
        if orientation == 0 then return 4 end
        if orientation == 90 then return 8 end
        if orientation == 270 then return 8 end
    end

    if meta == 8 then
        if orientation == 0 then return 8 end
        if orientation == 90 then return 4 end
        if orientation == 270 then return 4 end
    end

    if meta == 5 then
        if orientation == 0 then return 5 end
        if orientation == 90 then return 9 end
        if orientation == 270 then return 9 end
    end

    if meta == 9 then
        if orientation == 0 then return 9 end
        if orientation == 90 then return 5 end
        if orientation == 270 then return 5 end
    end

    if meta == 6 then
        if orientation == 0 then return 6 end
        if orientation == 90 then return 10 end
        if orientation == 270 then return 10 end
    end

    if meta == 10 then
        if orientation == 0 then return 10 end
        if orientation == 90 then return 6 end
        if orientation == 270 then return 6 end
    end

    if meta == 7 then
        if orientation == 0 then return 7 end
        if orientation == 90 then return 11 end
        if orientation == 270 then return 11 end
    end

    if meta == 11 then
        if orientation == 0 then return 11 end
        if orientation == 90 then return 7 end
        if orientation == 270 then return 7 end
    end

    return meta
end

function BlockConfig:doorBlockRotate(meta, orientation)
    if meta == 0 then
        if orientation == 0 then return 2 end
        if orientation == 90 then return 3 end
        if orientation == 270 then return 1 end
    end

    if meta == 1 then
        if orientation == 0 then return 3 end
        if orientation == 90 then return 0 end
        if orientation == 270 then return 2 end
    end

    if meta == 2 then
        if orientation == 0 then return 0 end
        if orientation == 90 then return 1 end
        if orientation == 270 then return 3 end
    end

    if meta == 3 then
        if orientation == 0 then return 1 end
        if orientation == 90 then return 2 end
        if orientation == 270 then return 0 end
    end

    return meta
end

function BlockConfig:fenceGateRotate(meta, orientation)
    if meta == 0 then
        if orientation == 0 then return 2 end
        if orientation == 90 then return 3 end
        if orientation == 270 then return 1 end
    end

    if meta == 1 then
        if orientation == 0 then return 3 end
        if orientation == 90 then return 0 end
        if orientation == 270 then return 2 end
    end

    if meta == 2 then
        if orientation == 0 then return 0 end
        if orientation == 90 then return 1 end
        if orientation == 270 then return 3 end
    end

    if meta == 3 then
        if orientation == 0 then return 1 end
        if orientation == 90 then return 2 end
        if orientation == 270 then return 0 end
    end

    if meta == 4 then
        if orientation == 0 then return 6 end
        if orientation == 90 then return 7 end
        if orientation == 270 then return 5 end
    end

    if meta == 5 then
        if orientation == 0 then return 3 end
        if orientation == 90 then return 7 end
        if orientation == 270 then return 6 end
    end

    if meta == 6 then
        if orientation == 0 then return 4 end
        if orientation == 90 then return 5 end
        if orientation == 270 then return 7 end
    end

    if meta == 7 then
        if orientation == 0 then return 5 end
        if orientation == 90 then return 6 end
        if orientation == 270 then return 4 end
    end

    return meta
end

function BlockConfig:trapDoorBlockRotate(meta, orientation)
    if meta == 0 then
        if orientation == 0 then return 1 end
        if orientation == 90 then return 2 end
        if orientation == 270 then return 3 end
    end

    if meta == 1 then
        if orientation == 0 then return 0 end
        if orientation == 90 then return 3 end
        if orientation == 270 then return 2 end
    end

    if meta == 2 then
        if orientation == 0 then return 3 end
        if orientation == 90 then return 1 end
        if orientation == 270 then return 0 end
    end

    if meta == 3 then
        if orientation == 0 then return 2 end
        if orientation == 90 then return 0 end
        if orientation == 270 then return 1 end
    end

    if meta == 4 then
        if orientation == 0 then return 5 end
        if orientation == 90 then return 6 end
        if orientation == 270 then return 7 end
    end

    if meta == 5 then
        if orientation == 0 then return 4 end
        if orientation == 90 then return 7 end
        if orientation == 270 then return 6 end
    end

    if meta == 6 then
        if orientation == 0 then return 7 end
        if orientation == 90 then return 5 end
        if orientation == 270 then return 4 end
    end

    if meta == 7 then
        if orientation == 0 then return 6 end
        if orientation == 90 then return 4 end
        if orientation == 270 then return 5 end
    end

    if meta == 8 then
        if orientation == 0 then return 9 end
        if orientation == 90 then return 10 end
        if orientation == 270 then return 11 end
    end

    if meta == 9 then
        if orientation == 0 then return 8 end
        if orientation == 90 then return 11 end
        if orientation == 270 then return 10 end
    end

    if meta == 10 then
        if orientation == 0 then return 11 end
        if orientation == 90 then return 9 end
        if orientation == 270 then return 8 end
    end

    if meta == 11 then
        if orientation == 0 then return 10 end
        if orientation == 90 then return 8 end
        if orientation == 270 then return 9 end
    end

    if meta == 12 then
        if orientation == 0 then return 13 end
        if orientation == 90 then return 14 end
        if orientation == 270 then return 15 end
    end

    if meta == 13 then
        if orientation == 0 then return 12 end
        if orientation == 90 then return 15 end
        if orientation == 270 then return 14 end
    end

    if meta == 14 then
        if orientation == 0 then return 15 end
        if orientation == 90 then return 13 end
        if orientation == 270 then return 12 end
    end

    if meta == 15 then
        if orientation == 0 then return 14 end
        if orientation == 90 then return 12 end
        if orientation == 270 then return 13 end
    end

    return meta
end

return BlockConfig