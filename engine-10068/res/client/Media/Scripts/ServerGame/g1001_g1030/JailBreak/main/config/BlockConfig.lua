---
--- Created by Jimmy.
--- DateTime: 2018/1/26 0026 11:06
---
BlockConfig = {}

BlockConfig.breakBlock = {}
BlockConfig.placeBlock = {}
BlockConfig.autoChange = {}

BlockConfig.STATUS_WAIT_CHANGE = 1
BlockConfig.STATUS_WAIT_RESET = 2

function BlockConfig:init(config)
    self:initBreakBlock(config.breakBlock)
    self:initPlaceBlock(config.placeBlock)
    self:initAutoChange(config.autoChange)
end

function BlockConfig:initBreakBlock(config)
    for i, v in pairs(config) do
        local block = {}
        block.id = tonumber(v.id)
        block.toolId = {}
        for j, toolId in pairs(v.toolId) do
            block.toolId[j] = tonumber(toolId)
        end
        self.breakBlock[tostring(block.id)] = block
    end
end

function BlockConfig:initPlaceBlock(config)
    for i, v in pairs(config) do
        self.placeBlock[tostring(v.id)] = tonumber(v.id)
    end
end

function BlockConfig:initAutoChange(config)
    for i, v in pairs(config) do
        local auto = {}
        auto.id = tonumber(v.id)
        auto.change = {}
        auto.change[1] = tonumber(v.change[1])
        auto.change[2] = tonumber(v.change[2])
        auto.changeTime = math.random(auto.change[1], auto.change[2])
        auto.reset = {}
        auto.reset[1] = tonumber(v.reset[1])
        auto.reset[2] = tonumber(v.reset[2])
        auto.resetTime = math.random(auto.reset[1], auto.reset[2])
        auto.time = os.time()
        auto.status = BlockConfig.STATUS_WAIT_CHANGE
        auto.blocks = {}
        for j, b in pairs(v.blocks) do
            local block = {}
            block.id = 0
            block.pos = VectorUtil.newVector3i(b.pos[1], b.pos[2], b.pos[3])
            auto.blocks[j] = block
        end
        self.autoChange[i] = auto
    end
end

function BlockConfig:prepareAutoChangeBlock()
    for i, auto in pairs(self.autoChange) do
        for j, block in pairs(auto.blocks) do
            block.id = EngineWorld:getBlockId(block.pos)
        end
    end
end

function BlockConfig:canBreakBlock(id, toolId)
    local block = self.breakBlock[tostring(id)]
    if block == nil then
        return false
    end
    if #block.toolId == 0 then
        return true
    end
    for i, v in pairs(block.toolId) do
        if v == toolId then
            return true
        end
    end
    return false
end

function BlockConfig:canPlaceBlock(id)
    local block = self.placeBlock[tostring(id)]
    return block ~= nil
end

function BlockConfig:onTick()
    for i, auto in pairs(self.autoChange) do
        if auto.status == BlockConfig.STATUS_WAIT_CHANGE then
            if os.time() - auto.time >= auto.changeTime then
                for j, block in pairs(auto.blocks) do
                    EngineWorld:setBlock(block.pos, auto.id)
                end
                auto.status = BlockConfig.STATUS_WAIT_RESET
                auto.time = os.time()
                auto.changeTime = math.random(auto.change[1], auto.change[2])
            end
        end
        if auto.status == BlockConfig.STATUS_WAIT_RESET then
            if os.time() - auto.time >= auto.resetTime then
                for j, block in pairs(auto.blocks) do
                    EngineWorld:setBlock(block.pos, block.id)
                end
                auto.status = BlockConfig.STATUS_WAIT_CHANGE
                auto.time = os.time()
                auto.resetTime = math.random(auto.reset[1], auto.reset[2])
            end
        end
    end
end

return BlockConfig