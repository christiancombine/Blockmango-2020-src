BlockConfig = {}
BlockConfig.block = {}
BlockConfig.blockHardness = {}

function BlockConfig:init(config)
    self:initBlock(config.block)
    self:initBlockHardness(config.blockHardness)
end

function BlockConfig:initBlock(config)
    for i, v in pairs(config) do
        local block = {}
        block.id = tonumber(v.id)
        block.meta = tonumber(v.meta)
        self.block[i] = block
    end
end

function BlockConfig:initBlockHardness(config)
    for i, v in pairs(config) do
        local hardness = {}
        hardness.id = tonumber(v.id)
        hardness.hardness = tonumber(v.hardness)
        self.blockHardness[i] = hardness
    end
end

function BlockConfig:prepareBlockHardness()
    for i, v in pairs(self.blockHardness) do
        HostApi.setBlockAttr(v.id, v.hardness)
    end
end

return BlockConfig