GameBlock = class()

function GameBlock:ctor(manorIndex, userId)
    self.userId = userId
    self.manorIndex = manorIndex
    self.blocks = {}
    self.tempNormalBlock = {}
    self.tempSpecialBlocks = {}
    self.blockScore = 0
    self.orientation = ManorConfig:getOrientationByIndex(manorIndex)
    self.isReady = false
end

function GameBlock:initDataFromDB(data)
    self:initBlocksData(data.blocks or {})
    self:setReady(true)
end

function GameBlock:initBlocksData(data)
    -- 把玩家移出领地
    SceneManager:removePlayersOutManor(self.manorIndex)
    for key, value in pairs(data) do
        local pos = StringUtil.split(key, ":")
        local x = tonumber(pos[1])
        local y = tonumber(pos[2])
        local z = tonumber(pos[3])

        local block = StringUtil.split(value, ":")
        local id = tonumber(block[1])
        local meta = tonumber(block[2])

        self:addBlock(VectorUtil.newVector3i(x, y, z), id, meta)

        meta = BlockConfig:getMetaByOrientation(id, meta, self.orientation)
        if BlockConfig:isSpecialBlock(id) then -- 火把, 梯子之类的方块
            self:addTempSpecialBlock(VectorUtil.newVector3i(x, y, z), id, meta)
        else
            self:addTempNormalBlock(VectorUtil.newVector3i(x, y, z), id, meta)
        end
    end

    for _, v in pairs(self.tempNormalBlock) do
        local vec3 = ManorConfig:getPosByOffset(self.manorIndex, v.offset, true)
        EngineWorld:setBlock(vec3, v.id, v.meta)
    end

    for _, v in pairs(self.tempSpecialBlocks) do
        local vec3 = ManorConfig:getPosByOffset(self.manorIndex, v.offset, true)
        EngineWorld:setBlock(vec3, v.id, v.meta)
    end

    self.tempNormalBlock = {}
    self.tempSpecialBlocks = {}
    
end

function GameBlock:removeFromWorld()
    local manorInfo = SceneManager:getUserManorInfo(self.userId)
    if not manorInfo then
        return
    end

    local startPos, endPos = ManorConfig:getOpenManorPos(manorInfo.level,self.manorIndex, true)
    if not startPos or not endPos then
        return
    end

    -- remove block
    SceneManager:removeBlockFromWorld(startPos, endPos)
end

function GameBlock:prepareDataSaveToDB()
    local data = {
        blocks = self:prepareBlocksToDB()
    }
    return data
end

function GameBlock:prepareBlocksToDB()
    local blocks = {}
    for _, v in pairs(self.blocks) do
        local pos = v.offset.x .. ":" .. v.offset.y .. ":" .. v.offset.z
        local block = v.id .. ":" .. v.meta
        blocks[pos] = block
    end

    return blocks
end

function GameBlock:addBlock(offset, id, meta)
    local sign = VectorUtil.hashVector3(offset)
    self.blocks[sign] = {
        id = id,
        meta = meta,
        offset = offset
    }

    self:addBlockScore(id)
end

function GameBlock:addTempNormalBlock(offset, id, meta)
    local block = {
        id = id,
        meta = meta,
        offset = offset
    }
    table.insert(self.tempNormalBlock, block)
end

function GameBlock:addTempSpecialBlock(offset, id, meta)
    local block = {
        id = id,
        meta = meta,
        offset = offset
    }
    table.insert(self.tempSpecialBlocks, block)
end

function GameBlock:checkHasBlock(offset)
    local sign = VectorUtil.hashVector3(offset)
    if self.blocks[sign] then
        return self.blocks[sign].id, self.blocks[sign].meta
    end

    return 0, 0
end

function GameBlock:removeBlock(offset, id, meta)
    local sign = VectorUtil.hashVector3(offset)
    self.blocks[sign] = nil
    self:subBlockScore(id)
end

function GameBlock:addBlockScore(id)
    self.blockScore = self.blockScore + BlockConfig:getScoreById(id)
end

function GameBlock:subBlockScore(id)
    self.blockScore = self.blockScore - BlockConfig:getScoreById(id)
    if self.blockScore < 0 then
        self.blockScore = 0
    end
end

function GameBlock:getBlockScore()
    return self.blockScore
end

function GameBlock:getTotalBlockNum()
    local num = 0
    for i, v in pairs(self.blocks) do
        num = num + 1
    end

    return num
end

function GameBlock:setReady(isReady)
    self.isReady = isReady
end

function GameBlock:getReady()
    return self.isReady
end

function GameBlock:blockPlacedBy(offset, id, meta)
    local sign = VectorUtil.hashVector3(offset)
    self.blocks[sign] = {
        id = id,
        meta = meta,
        offset = offset
    }
end