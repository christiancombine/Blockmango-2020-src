PraiseRewardConfig = {}
PraiseRewardConfig.reward = {}

function PraiseRewardConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.type = tonumber(v.type)
        data.uniqueId = tonumber(v.uniqueId)
        data.num = tonumber(v.num)
        self.reward[data.id] = data
    end
end

function PraiseRewardConfig:getNumOfReward()
    return #self.reward
end

function PraiseRewardConfig:randomReward()
    local id = HostApi.random("reward", 1, PraiseRewardConfig:getNumOfReward() + 1)
    return self.reward[id]
end

return PraiseRewardConfig