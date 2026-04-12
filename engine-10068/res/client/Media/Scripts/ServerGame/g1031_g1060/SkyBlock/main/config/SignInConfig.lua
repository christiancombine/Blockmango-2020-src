SignInConfig = {}
SignInConfig.List = {}

function SignInConfig:initConfig(configs)
    --Id	Title	Side	Effect	Background	Image	RewardType	RewardId	Num	Meta	RewardName	Desc
    for _, config in pairs(configs) do
        local item = {
            Id = tonumber(config.Id),
            Day = tonumber(config.Day),
            RewardType = tonumber(config.RewardType),
            RewardId = tonumber(config.RewardId),
            Num = tonumber(config.Num),
            Meta = tonumber(config.Meta)
        }
        self.List[item.Id] = item
    end
end

function SignInConfig:getConfigById(id)
    return self.List[id]
end

function SignInConfig:getNextSignInConfig(id)
    local config = self:getConfigById(id)
    if config then
        local nextDay = config.Day + 1
        for _, next in pairs(self.List) do
            if next.Day == nextDay then
                return next
            end
        end
    end
    return nil
end

return SignInConfig