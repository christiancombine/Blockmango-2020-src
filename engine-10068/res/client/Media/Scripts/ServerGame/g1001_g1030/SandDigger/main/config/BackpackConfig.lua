BackpackConfig = {}
BackpackConfig.backpack = {}
BackpackConfig.maxLevel = 1

function BackpackConfig:init(config)
    self:initBackpack(config.backpack)
    self.maxLevel = config.maxLevel
end

function BackpackConfig:initBackpack(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.level = tonumber(v.level)
        item.capacity = tonumber(v.capacity)
        item.model = v.model
        item.modelID = tonumber(v.modelID)
        item.desc = v.desc
        self.backpack[item.id] = item
    end
end

function BackpackConfig:getBackpackById(id)
    if self.backpack[id] ~= nil then
        return self.backpack[id]
    end
    return nil
end

function BackpackConfig:getBackpackByLevel(level)
    for i, v in pairs(self.backpack) do
        if level == v.level then
            return v
        end
    end
    return nil
end


return BackpackConfig