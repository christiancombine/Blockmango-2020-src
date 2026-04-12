CDRewardConfig = {}
CDRewardConfig.reward = {}
CDRewardConfig.maxLevel = 0

function CDRewardConfig:init(data)
    for _, v in pairs(data) do
        local info = {}
        info.id = tonumber(v.id)
        info.level = tonumber(v.level)
        info.cash = tonumber(v.cash)
        info.cdTime = tonumber(v.cdTime)
        info.image = v.image

        self.reward[info.level] = info
        self.maxLevel = (info.level > self.maxLevel and {info.level} or {self.maxLevel})[1]
    end
end

function CDRewardConfig:getCDRewardInfoByLevel(level)
    return self.reward[tonumber(level)]
end

function CDRewardConfig:getCashByLevel(level)
    return self.reward[tonumber(level)].cash
end

function CDRewardConfig:getCDRewardTimeByLevel(level)
    return (self.reward[tonumber(level)] and {self.reward[tonumber(level)].cdTime} or {0})[1]
end

return CDRewardConfig