BlockConfig = {}
BlockConfig.block = {}

function BlockConfig:init(config)
    self:initBlock(config)
end

function BlockConfig:initBlock(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.hardness = tonumber(v.hardness)
        self.block[item.id] = item
    end
end

function BlockConfig:prepareBlock()
    for i, v in pairs(self.block) do
        HostApi.setBlockAttr(v.id, v.hardness)
    end
end

return BlockConfig