WeekRewardConfig = {}
WeekRewardConfig.reward = {}

function WeekRewardConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.rank = tonumber(v.rank)
        data.type = tonumber(v.type)
        data.uniqueId = tonumber(v.uniqueId)
        data.num = tonumber(v.num)
        self.reward[data.id] = data
    end
end

function WeekRewardConfig:getRewardsByRank(rank)
    rank = tonumber(rank)
    local item = {}
    for _, v in pairs(self.reward) do
        if rank == v.rank then
            table.insert(item, v)
        end
    end

    return item
end

function WeekRewardConfig:getRewardsByRankClassify()
    local weekRewards = {}
    for _, reward in pairs(self.reward) do
        local rank = tonumber(reward.rank)
        if not weekRewards[rank] then
            weekRewards[rank] = {
                rank = rank,
                items = {},
            }
        end
        
        local item = {}
        item.id = reward.uniqueId
        item.type = tonumber(reward.type)
        table.insert(weekRewards[rank].items, item)
    end
    return weekRewards
end

return WeekRewardConfig