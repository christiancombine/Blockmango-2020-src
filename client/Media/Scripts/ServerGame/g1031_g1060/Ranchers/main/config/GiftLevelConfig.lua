GiftLevelConfig = {}
GiftLevelConfig.data = {}

function GiftLevelConfig:init(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.minLevel = tonumber(v.minLevel)
        item.maxLevel = tonumber(v.maxLevel)
        item.count = tonumber(v.count)
        self.data[item.id] = item
    end
end

function GiftLevelConfig:getMaxCount()
    return self.data[#self.data].count
end

function GiftLevelConfig:getCountByPlayerLevel(level)
    for i, v in pairs(self.data) do
        if v.minLevel <= level and v.maxLevel >= level then
            return v.count
        end
    end

    return self:getMaxCount()
end

function GiftLevelConfig:getTheFirstMinLevel()
    if self.data[1].count > 0 then
        return self.data[1].minLevel
    end

    return self.data[2].minLevel
end

return GiftLevelConfig