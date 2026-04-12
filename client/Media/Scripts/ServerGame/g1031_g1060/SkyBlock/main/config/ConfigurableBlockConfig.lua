ConfigurableBlockConfig = {}
ConfigurableBlockConfig.List = {}

ConfigurableBlockConfig.BT_CUBE = 0
ConfigurableBlockConfig.BT_STAIRS = 1
ConfigurableBlockConfig.BT_STEP_SINGLE = 2
ConfigurableBlockConfig.BT_STEP_DOUBLE = 3

function ConfigurableBlockConfig:initConfig(configs)
    for _, config in pairs(configs) do
        local item = {
            UniqueId = tonumber(config.UniqueId),
            BlockId = tonumber(config.BlockId),
            BlockMeta = tonumber(config.BlockMeta),
            BlockType = tonumber(config.BlockType),
            ExtraParam1 = tonumber(config.ExtraParam1),
            ExtraParam2 = tonumber(config.ExtraParam2)
        }
        self.List[item.UniqueId] = item
    end
end

function ConfigurableBlockConfig:isStairs(blockId)
    for _, block in pairs(self.List) do
        if block.BlockId == blockId then
            return block.BlockType == ConfigurableBlockConfig.BT_STAIRS
        end
    end
    return false
end

function ConfigurableBlockConfig:isConfigurableBlock(blockId)
    for _, block in pairs(self.List) do
        if block.BlockId == blockId then
            return true
        end
    end
    return false
end

function ConfigurableBlockConfig:isSingleHalfSlab(blockId)
    for _, block in pairs(self.List) do
        if block.BlockId == blockId then
            return block.BlockType == ConfigurableBlockConfig.BT_STEP_SINGLE
        end
    end
    return false
end

return ConfigurableBlockConfig