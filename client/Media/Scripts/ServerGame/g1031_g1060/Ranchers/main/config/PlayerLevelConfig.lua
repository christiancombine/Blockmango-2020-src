PlayerLevelConfig = {}
PlayerLevelConfig.data = {}

function PlayerLevelConfig:init(config)
    for i, v in pairs(config) do
        local items = {}
        items.level = tonumber(v.level)
        items.exp = tonumber(v.exp)
        items.items = StringUtil.split(v.items, "#")
        items.rewardMoney = tonumber(v.rewardMoney)
        items.rewardItems = StringUtil.split(v.rewardItems, "#")
        self.data[items.level] = items
    end
end

function PlayerLevelConfig:getExpByLevel(level)
    if self.data[level] ~= nil then
        return self.data[level].exp
    end

    return 0
end

function PlayerLevelConfig:getMaxLevel()
    return #self.data
end

function PlayerLevelConfig:getUnlockItemsByLevel(level)
    if self.data[level] == nil then
        return nil
    end

    return self.data[level].items
end

function PlayerLevelConfig:getRewardItemsByLevel(level)
    if self.data[level] == nil then
        return nil
    end

    local items = {}
    for i, v in pairs(self.data[level].rewardItems) do
        local data = RanchCommon.new()
        local item = StringUtil.split(v, ":")
        data.itemId = tonumber(item[1])
        data.num = tonumber(item[2])
        table.insert(items, data)
    end

    return items
end

function PlayerLevelConfig:getRewardMoneyByLevel(level)
    if self.data[level] == nil then
        return 0
    end
    return self.data[level].rewardMoney
end

return PlayerLevelConfig