BlockConfig = {}
BlockConfig.block = {}
BlockConfig.blockHardness = {}

function BlockConfig:initBlock(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.hp = tonumber(v.hp)
        self.block[data.id] = data
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