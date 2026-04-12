RewardConfig = {}

RewardConfig.settings = {}

function RewardConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.rank = tonumber(vConfig.n_rank) or 0 --排名
        data.coinReward = tonumber(vConfig.n_coinReward) or 0 --代币奖励
        data.advancedReward301 = vConfig.s_advancedReward301 or "" --阶数奖励301
        data.advancedReward302 = vConfig.s_advancedReward302 or "" --阶数奖励301
        data.advancedReward303 = vConfig.s_advancedReward303 or "" --阶数奖励301
        data.advancedReward304 = vConfig.s_advancedReward304 or "" --阶数奖励301
        data.advancedReward305 = vConfig.s_advancedReward305 or "" --阶数奖励301
        data.advancedReward306 = vConfig.s_advancedReward306 or "" --阶数奖励301
        data.advancedReward307 = vConfig.s_advancedReward307 or "" --阶数奖励301
        data.advancedReward308 = vConfig.s_advancedReward308 or "" --阶数奖励301
        data.advancedReward309 = vConfig.s_advancedReward309 or "" --阶数奖励301
        data.advancedReward310 = vConfig.s_advancedReward310 or "" --阶数奖励301
        data.advancedReward311 = vConfig.s_advancedReward311 or "" --阶数奖励301
        data.advancedReward312 = vConfig.s_advancedReward312 or "" --阶数奖励301
        data.advancedReward313 = vConfig.s_advancedReward313 or "" --阶数奖励301
        data.advancedReward314 = vConfig.s_advancedReward314 or "" --阶数奖励301
        data.advancedReward315 = vConfig.s_advancedReward315 or "" --阶数奖励301
        data.advancedReward316 = vConfig.s_advancedReward316 or "" --阶数奖励301
        data.advancedReward317 = vConfig.s_advancedReward317 or "" --阶数奖励301
        data.advancedReward318 = vConfig.s_advancedReward318 or "" --阶数奖励301
        table.insert(RewardConfig.settings, data)
    end
end

function RewardConfig:getRewardConfig(rank)
    for _, setting in pairs(RewardConfig.settings) do
        if setting.rank == rank then
            return setting
        end
    end
    return nil
end

function RewardConfig:getCoinReward(rank)
    local config = self:getRewardConfig(rank)
    if config then
        return config.coinReward
    end
    return 0
end

function RewardConfig:getGoldReward(rank, advancedLevel)
    local config = self:getRewardConfig(rank)
    if not config then
        return 0
    end

    local advancedReward = config["advancedReward" .. advancedLevel]
    if not advancedReward then
        return 0
    end

    local result = StringUtil.split(advancedReward, ",")

    return tonumber(result[Define.RewardType.Gold])
end

function RewardConfig:getMuscleReward(rank, advancedLevel)
    local config = self:getRewardConfig(rank)
    if not config then
        return 0
    end

    local advancedReward = config["advancedReward" .. advancedLevel]
    if not advancedReward then
        return 0
    end

    local result = StringUtil.split(advancedReward, ",")

    return tonumber(result[Define.RewardType.Muscle])
end

return RewardConfig
